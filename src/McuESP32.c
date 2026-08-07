/*
 * Copyright (c) 2019, Erich Styger
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * \file
 * \brief Implementation of McuESP32 module.
 */

#include "McuESP32config.h"
#if McuESP32_CONFIG_IS_ENABLED
#include "McuESP32.h"
#include "McuGPIO.h"
#include "McuRTOS.h"
#include "McuUtility.h"
#include "McuWait.h"
#include "McuLog.h"
#include "McuShellUart.h"

#if McuESP32_CONFIG_USE_CTRL_PINS
  static McuGPIO_Handle_t McuESP32_RF_EN_Pin;  /* pin pulled LOW to reset the module */
  static McuGPIO_Handle_t McuESP32_RF_IO0_Pin; /* pin pulled LOW to enable programming mode */
#endif

#define McuESP32_UART_RX_BUFFER_SIZE   (2*McuESP32_CONFIG_RX_BUFFER_SIZE) /*!< rx streambuffer size*/
#define McuESP32_UART_TX_BUFFER_SIZE   (2*McuESP32_CONFIG_TX_BUFFER_SIZE) /*!< tx streambufer size */

#define MCUESP32_CONFIG_STREAM_BUFFER_TRIGGER_LEVEL  (16) /*!< streambuffer trigger level */
static StreamBufferHandle_t rxStreamBuffer;
static StreamBufferHandle_t txStreamBuffer;

#if McuESP32_CONFIG_USE_USB_CDC
  typedef enum McuESP32_USB_PrgMode_e {
    McuESP32_USB_PRG_MODE_AUTO,
    McuESP32_USB_PRG_MODE_ON,
    McuESP32_USB_PRG_MODE_OFF,
  } McuESP32_USB_PrgMode_e;
  static McuESP32_USB_PrgMode_e McuESP32_UsbPrgMode = McuESP32_USB_PRG_MODE_AUTO;
  static bool McuESP32_IsProgramming = false; /* if we are currently programming the ESP32 */
  static void (*McuESP32_ProgrammingCallback)(bool isProgramming) = NULL; /* optional programming callback for application, e.g. to reduce CPU load. Set with McuESP32_SetProgrammingCallback() */
  static McuShell_ConstStdIOType *McuESP32_UsbCdcIo = NULL; /* I/O handler to be used for USB CDC. Configure with McuESP32_SetUsbCdcStdio() */
  static bool (*McuESP32_UsbIsConnected)(void) = NULL; /* callback which decides if USB CDC is connected or not. Configure with McuESP32_SetUsbCdcIsConnectedCallback() */
  static void (*McuESP32_UsbFlush)(void) = NULL; /* callback to flush the outgoing data. Required for ESP idf.py flash usage. Configure McuESP_SetUsbFlushCallback() */
#endif
static bool McuESP32_CopyUartToShell = false; /* if we copy the ESP32 UART to the Shell */ /* \TODO fails USB enumeration if McuESP32_CopyUartToShell enabled by default? */

/* Below is the I/O handler for the console: data from the ESP is sent optionally to that stdout (e.g. shell console).
 */
static McuShell_ConstStdIOType *McuESP32_RxFromESPStdIO = NULL; /* can be overwritten with McuESP32_SetRxFromESPStdio(); */

void McuESP32_SetRxFromESPStdio(McuShell_ConstStdIOTypePtr stdio) {
  McuESP32_RxFromESPStdIO = stdio;
}

McuShell_ConstStdIOTypePtr McuESP32_GetRxFromESPStdio(void) {
  return McuESP32_RxFromESPStdIO;
}

void McuESP32_SetProgrammingCallback(void (*callback)(bool isProgramming)) {
  McuESP32_ProgrammingCallback = callback;
}

static void McuESP32_Programming(bool isProgramming) {
  if (McuESP32_IsProgramming!=isProgramming) { /* state change? */
    McuLog_trace("programming callback: %s", isProgramming?"start":"stop");
    McuESP32_IsProgramming = isProgramming;
    if (McuESP32_ProgrammingCallback!=NULL) {
      McuESP32_ProgrammingCallback(isProgramming); /* call application callback */
    }
  }
}

#if McuESP32_CONFIG_USE_USB_CDC
void McuESP32_SetUsbCdcIsConnectedCallback(bool (*callback)(void)) {
  McuESP32_UsbIsConnected = callback;
}
#endif

#if McuESP32_CONFIG_USE_USB_CDC
void McuESP32_SetUsbFlushCallback(void (*callback)(void)) {
  McuESP32_UsbFlush = callback;
}
#endif

#if McuESP32_CONFIG_USE_USB_CDC
void McuESP32_SetUsbCdcStdio(McuShell_ConstStdIOTypePtr stdio) {
  McuESP32_UsbCdcIo = stdio;
}
#endif

#if McuESP32_CONFIG_USE_CTRL_PINS
static void AssertReset(void) {
  McuGPIO_SetAsOutput(McuESP32_RF_EN_Pin, false); /* output, LOW */
}
#endif

#if McuESP32_CONFIG_USE_CTRL_PINS
static void DeassertReset(void) {
  McuGPIO_SetAsInput(McuESP32_RF_EN_Pin);
}
#endif

#if McuESP32_CONFIG_USE_CTRL_PINS
static void DoReset(void) {
  AssertReset();
  McuWait_Waitus(50); /* give signal some time */
  DeassertReset();
}
#endif

#if McuESP32_CONFIG_USE_CTRL_PINS
static void AssertBootloaderMode(void) {
  McuGPIO_SetAsOutput(McuESP32_RF_IO0_Pin, false); /* output, LOW */
}
#endif

#if McuESP32_CONFIG_USE_CTRL_PINS
static void DeassertBootloaderMode(void) {
  McuGPIO_SetAsInput(McuESP32_RF_IO0_Pin);
}
#endif

#if McuESP32_CONFIG_USE_USB_CDC
void McuESP32_UartStateCallback(bool dtr, bool rts) { /* callback for DTR and RTS lines */
  /*
   * DTR  RTS->EN  GPIO0
   * 1    1    1   1
   * 0    0    1   1
   * 1    0    0   0
   * 0    1    1   0
   */
#if McuESP32_CONFIG_VERBOSE_CONTROL_SIGNALS
  McuLog_trace("dtr: %d, rts: %d", dtr, rts);
#endif
  if (dtr && rts) {
    McuGPIO_SetAsInput(McuESP32_RF_EN_Pin); 
    McuGPIO_SetAsInput(McuESP32_RF_IO0_Pin);
  } else if (dtr) {
    McuGPIO_SetAsInput(McuESP32_RF_EN_Pin); 
    McuGPIO_SetAsOutput(McuESP32_RF_IO0_Pin, false); 
  } else if (rts) {
    McuGPIO_SetAsOutput(McuESP32_RF_EN_Pin, false); 
    McuGPIO_SetAsInput(McuESP32_RF_IO0_Pin);
  } else {
    McuGPIO_SetAsInput(McuESP32_RF_EN_Pin); 
    McuGPIO_SetAsInput(McuESP32_RF_IO0_Pin);
  }
}
#endif /* McuESP32_CONFIG_USE_USB_CDC */

/*********************************************************************************************************/
/* Stdio Handler for sending text to the ESP32 */
static void QueueTxChar(unsigned char ch) {
  if (xStreamBufferSend(txStreamBuffer, &ch, 1, portMAX_DELAY)!=1) {
    McuLog_error("failed sending to streambuffer");
  }
}

uint32_t McuESP32_SendTxData(const void *data, uint32_t nofBytes) {
  return xStreamBufferSend(txStreamBuffer, data, nofBytes, portMAX_DELAY);
}

static void Dummy_ReadChar(uint8_t *c) {
  *c = '\0'; /* nothing received */
}

static bool Dummy_CharPresent(void) {
  return false;
}

/* for sending data to the ESP32 (tx only) */
static const McuShell_ConstStdIOType McuESP32_Tx_stdio = {
    .stdIn = (McuShell_StdIO_In_FctType)Dummy_ReadChar,
    .stdOut = (McuShell_StdIO_OutErr_FctType)QueueTxChar,
    .stdErr = (McuShell_StdIO_OutErr_FctType)QueueTxChar,
    .keyPressed = Dummy_CharPresent, /* if input is not empty */
 #if McuShell_CONFIG_ECHO_ENABLED
    .echoEnabled = false,
  #endif
  };

McuShell_ConstStdIOTypePtr McuESP32_GetTxToESPStdio(void) {
  return &McuESP32_Tx_stdio;
}
/*********************************************************************************************************/
void McuESP32_CONFIG_UART_IRQ_HANDLER(void) {
  uint32_t flags;
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  uint8_t count;

  flags = McuESP32_CONFIG_UART_GET_FLAGS(McuESP32_CONFIG_UART_DEVICE);
#if McuESP32_CONFIG_UART_HAS_FIFO
  if (flags&kUART_RxFifoOverflowFlag) {
    count = 0; /* statement to allow debugger to set a breakpoint here */
  }
#endif
  /* If new data arrived. */
  if (flags&McuESP32_CONFIG_UART_HW_RX_READY_FLAGS) {
  #if McuESP32_CONFIG_UART_HAS_FIFO
    count = McuESP32_CONFIG_UART_DEVICE->RCFIFO;
  #else
    count = 1;
  #endif
    if (count!=0) {
      unsigned char buf[8];
      for(int i=0; i<count; i++) {
        buf[i] = McuESP32_CONFIG_UART_READ_BYTE(McuESP32_CONFIG_UART_DEVICE);
      }
      size_t nof = xStreamBufferSendFromISR(rxStreamBuffer, buf, count, &xHigherPriorityTaskWoken);
      if (nof!=1) { /* was not able to send? */
        for(;;) {}
      }
    }
  }
  McuESP32_CONFIG_UART_CLEAR_STATUS_FLAGS(McuESP32_CONFIG_UART_DEVICE, flags|McuESP32_CONFIG_UART_CLEAR_EXTRA_STATUS_FLAGS);
  if (xHigherPriorityTaskWoken != pdFALSE) {
    vPortYieldFromISR();
  }
#if McuLib_CONFIG_CPU_IS_ARM_CORTEX_M && ((McuLib_CONFIG_CORTEX_M==4) || (McuLib_CONFIG_CORTEX_M==7))
  /* ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping exception return operation might vector to incorrect interrupt.
  * For Cortex-M7, if core speed much faster than peripheral register write speed, the peripheral interrupt flags may be still set after exiting ISR, this results to
  * the same error similar with errata 83869. */
  __DSB();
#endif
}

static uint8_t McuESP32_PrintHelp(const McuShell_StdIOType *io) {
  McuShell_SendHelpStr((unsigned char*)"esp32", (unsigned char*)"Group of ESP32 WiFi module commands\r\n", io->stdOut);
  McuShell_SendHelpStr((unsigned char*)"  help|status", (unsigned char*)"Shows ESP32 help or status\r\n", io->stdOut);
#if McuESP32_CONFIG_USE_CTRL_PINS
  McuShell_SendHelpStr((unsigned char*)"  reset", (unsigned char*)"Perform reset sequence with pulling low/assert EN and release/deassert\r\n", io->stdOut);
  McuShell_SendHelpStr((unsigned char*)"  assert|deassart reset", (unsigned char*)"Assert or deassert reset (EN) pin\r\n", io->stdOut);
  McuShell_SendHelpStr((unsigned char*)"  assert|deassart bl", (unsigned char*)"Assert or deassert bootloader (IO0) pin\r\n", io->stdOut);
  McuShell_SendHelpStr((unsigned char*)"  prg start|stop", (unsigned char*)"Start and stop programming sequence\r\n", io->stdOut);
  McuShell_SendHelpStr((unsigned char*)"  uarttoshell on|off", (unsigned char*)"Copy UART Rx to Shell\r\n", io->stdOut);
  McuShell_SendHelpStr((unsigned char*)"  usbprg auto|on|off", (unsigned char*)"Use USB CDC-UART bridge for programming\r\n", io->stdOut);
#endif
  McuShell_SendHelpStr((unsigned char*)"  send <cmd>", (unsigned char*)"Send a command or string to the ESP32 (non-blocking), <cmd> can be double quoted\r\n", io->stdOut);
  return ERR_OK;
}

static uint8_t McuESP32_PrintStatus(const McuShell_StdIOType *io) {
  uint8_t buf[64];

  McuShell_SendStatusStr((unsigned char*)"esp32", (unsigned char*)"ESP32 status\r\n", io->stdOut);
#if McuESP32_CONFIG_USE_CTRL_PINS
  McuGPIO_GetPinStatusString(McuESP32_RF_EN_Pin, buf, sizeof(buf));
  McuUtility_strcat(buf, sizeof(buf), (unsigned char*)"\r\n");
  McuShell_SendStatusStr((unsigned char*)"  EN", buf, io->stdOut);

  McuGPIO_GetPinStatusString(McuESP32_RF_IO0_Pin, buf, sizeof(buf));
  McuUtility_strcat(buf, sizeof(buf), (unsigned char*)"\r\n");
  McuShell_SendStatusStr((unsigned char*)"  IO0", buf, io->stdOut);
#endif
  if (McuESP32_UsbPrgMode==McuESP32_USB_PRG_MODE_ON) {
    McuUtility_strcpy(buf, sizeof(buf), (unsigned char*)"on\r\n");
  } else if (McuESP32_UsbPrgMode==McuESP32_USB_PRG_MODE_AUTO) {
    McuUtility_strcpy(buf, sizeof(buf), (unsigned char*)"auto\r\n");
  } else if (McuESP32_UsbPrgMode==McuESP32_USB_PRG_MODE_OFF) {
    McuUtility_strcpy(buf, sizeof(buf), (unsigned char*)"off\r\n");
  } else {
    McuUtility_strcpy(buf, sizeof(buf), (unsigned char*)"ERROR\r\n");
  }
  McuShell_SendStatusStr((unsigned char*)"  usbprg", buf, io->stdOut);
  McuShell_SendStatusStr((unsigned char*)"  programming", McuESP32_IsProgramming?(unsigned char*)"yes\r\n":(unsigned char*)"no\r\n", io->stdOut);
  McuShell_SendStatusStr((unsigned char*)"  uarttoshell", McuESP32_CopyUartToShell?(unsigned char*)"on\r\n":(unsigned char*)"off\r\n", io->stdOut);
  return ERR_OK;
}

uint8_t McuESP32_ParseCommand(const unsigned char *cmd, bool *handled, const McuShell_StdIOType *io) {
  unsigned char cmd_buffer[McuShell_CONFIG_DEFAULT_SHELL_BUFFER_SIZE];
  const unsigned char *p;

  if (McuUtility_strcmp((char*)cmd, (char*)McuShell_CMD_HELP)==0 || McuUtility_strcmp((char*)cmd, (char*)"esp32 help")==0) {
    *handled = true;
    return McuESP32_PrintHelp(io);
  } else if (McuUtility_strcmp((char*)cmd, (char*)McuShell_CMD_STATUS)==0 || McuUtility_strcmp((char*)cmd, (char*)"esp32 status")==0) {
    *handled = true;
    return McuESP32_PrintStatus(io);
#if McuESP32_CONFIG_USE_CTRL_PINS
  } else if (McuUtility_strcmp((char*)cmd, (char*)"esp32 reset")==0) {
    *handled = true;
    DoReset();
    return ERR_OK;
  } else if (McuUtility_strcmp((char*)cmd, (char*)"esp32 prg start")==0) {
    *handled = true;
    McuESP32_Programming(true);
    /* pulling prg pin low, followed by a reset */
    AssertBootloaderMode(); /* pull prg pin low: during reset, device will enter serial programming mode */
    vTaskDelay(pdMS_TO_TICKS(1));
    DoReset();
    vTaskDelay(pdMS_TO_TICKS(1));
    DeassertBootloaderMode();
    return ERR_OK;
  } else if (McuUtility_strcmp((char*)cmd, (char*)"esp32 prg stop")==0) {
    /* release prg pin, followed by a reset */
    *handled = true;
    McuESP32_Programming(false);
    DeassertBootloaderMode(); /* return prg pin to high (normal) again */
    vTaskDelay(pdMS_TO_TICKS(1));
    DoReset();
    return ERR_OK;
  } else if (McuUtility_strcmp((char*)cmd, (char*)"esp32 assert bl")==0) {
    *handled = true;
    AssertBootloaderMode();
    return ERR_OK;
  } else if (McuUtility_strcmp((char*)cmd, (char*)"esp32 deassert bl")==0) {
    *handled = true;
    DeassertBootloaderMode();
    return ERR_OK;
  } else if (McuUtility_strcmp((char*)cmd, (char*)"esp32 assert reset")==0) {
    *handled = true;
    AssertReset();
    return ERR_OK;
  } else if (McuUtility_strcmp((char*)cmd, (char*)"esp32 deassert reset")==0) {
    *handled = true;
    DeassertReset();
    return ERR_OK;
#endif
#if McuESP32_CONFIG_USE_USB_CDC
  } else if (McuUtility_strcmp((char*)cmd, (char*)"esp32 usbprg auto")==0) {
    *handled = true;
    McuESP32_UsbPrgMode = McuESP32_USB_PRG_MODE_AUTO;
    McuESP32_IsProgramming = false;
    return ERR_OK;
  } else if (McuUtility_strcmp((char*)cmd, (char*)"esp32 usbprg on")==0) {
    *handled = true;
    McuESP32_UsbPrgMode = McuESP32_USB_PRG_MODE_ON;
    McuESP32_IsProgramming = true;
    return ERR_OK;
  } else if (McuUtility_strcmp((char*)cmd, (char*)"esp32 usbprg off")==0) {
    *handled = true;
    McuESP32_UsbPrgMode = McuESP32_USB_PRG_MODE_OFF;
    McuESP32_IsProgramming = false;
    return ERR_OK;
#endif
  } else if (McuUtility_strcmp((char*)cmd, (char*)"esp32 uarttoshell on")==0) {
    *handled = true;
    McuESP32_CopyUartToShell = true;
    return ERR_OK;
  } else if (McuUtility_strcmp((char*)cmd, (char*)"esp32 uarttoshell off")==0) {
    *handled = true;
    McuESP32_CopyUartToShell = false;
    return ERR_OK;
  } else if (McuUtility_strncmp((char*)cmd, (char*)"esp32 send ", sizeof("esp32 send ")-1)==0) {
    *handled = true;
    p = cmd+sizeof("esp32 send ")-1;
    if (*p=='"') { /* double-quoted command: it can contain multiple commands */
      if (McuUtility_ScanDoubleQuotedString(&p, cmd_buffer, sizeof(cmd_buffer))!=ERR_OK) {
        return ERR_FAILED;
      }
      p = cmd_buffer;
    }
    McuShell_SendStr(p, McuESP32_GetTxToESPStdio()->stdOut);
    McuShell_SendStr((unsigned char*)"\r\n", McuESP32_GetTxToESPStdio()->stdOut);
    return ERR_OK;
  }
  return ERR_OK;
}

static void sendData(const void *buffer, uint32_t size) {
  uint32_t McuShellCdcDevice_Send(void const *buf, uint32_t nofBytes); /* using private interface \todo */

  #if McuESP32_CONFIG_VERBOSE_TRAFFIC
    McuLog_trace("tx USB: %d", size);
  #endif
  uint32_t nof = McuShellCdcDevice_Send(buffer, size);
  if (nof!=size) {
    McuLog_fatal("wanted to send %d, but did %d", size, nof);
  }
  if (   McuESP32_CopyUartToShell
#if McuESP32_CONFIG_USE_USB_CDC
      && !McuESP32_IsProgramming
#endif
     )
  { /* only write to shell if not in programming mode. Programming mode might crash RTT */
    McuShell_ConstStdIOTypePtr io = McuESP32_GetRxFromESPStdio();
    if (io!=NULL) {
      for(int i=0; i<size; i++) {
        McuShell_SendCh(((char*)buffer)[i], io->stdOut); /* forward character */ /* \TODO use buffer send instead of char by char */
      }
    }
  }
}

static void UartRxTask(void *pv) { /* task handling characters sent by the ESP32 module */
  size_t size;
  static unsigned char buffer[128];
 
  (void)pv; /* not used */
  for(;;) {
    size = xStreamBufferReceive(rxStreamBuffer, buffer, sizeof(buffer), pdMS_TO_TICKS(50)); /* use longer timeout to prevent too much polling */
    if (size!=0) { /* received something */
  #if McuESP32_CONFIG_USE_USB_CDC
      if (McuESP32_UsbCdcIo!=NULL && McuESP32_UsbIsConnected!=NULL && McuESP32_UsbIsConnected()) { /* send directly to programmer attached on the USB or to the IDF monitor */
        do {
          sendData(buffer, size);
          size = xStreamBufferReceive(rxStreamBuffer, buffer, sizeof(buffer), pdMS_TO_TICKS(5)); /* use shorter timeout */
        } while(size>0);
        if (McuESP32_UsbFlush!=NULL) {
          McuESP32_UsbFlush();
        }
      } /* forward to USB CDC */
  #endif
    }
  }
}

static void UartTxTask(void *pv) { /* task handling sending data to the ESP32 module */
  static unsigned char buffer[64];
  size_t size;
 
  (void)pv; /* not used */
  for(;;) {
    size = xStreamBufferReceive(txStreamBuffer, buffer, sizeof(buffer), pdMS_TO_TICKS(50)); /* use longer timeout to prevent too much CPU usage with polling */
    if (size!=0) { /* received something */
      do {
        #if McuESP32_CONFIG_VERBOSE_TRAFFIC
          McuLog_trace("tx->esp: %d", size);
        #endif
        McuESP32_CONFIG_UART_WRITE_BLOCKING(McuESP32_CONFIG_UART_DEVICE, buffer, size); /* send to ESP */
        size = xStreamBufferReceive(txStreamBuffer, buffer, sizeof(buffer), pdMS_TO_TICKS(5)); /* use shorter timeout */
      } while(size>0);
    }
  }
}

static void InitQueues(void) {
  rxStreamBuffer = xStreamBufferCreate(McuESP32_UART_RX_BUFFER_SIZE, MCUESP32_CONFIG_STREAM_BUFFER_TRIGGER_LEVEL);
  if (rxStreamBuffer==NULL) {
    for(;;) {}
  }
  txStreamBuffer = xStreamBufferCreate(McuESP32_UART_TX_BUFFER_SIZE, MCUESP32_CONFIG_STREAM_BUFFER_TRIGGER_LEVEL);
  if (txStreamBuffer==NULL) {
    for(;;) {}
  }
}

void McuESP32_ChangeUartBaudCallback(uint32_t baud) {
  static uint32_t currBaud = McuESP32_CONFIG_UART_BAUDRATE;
  if (baud!=currBaud) {
    #if McuESP32_CONFIG_VERBOSE_CONTROL_SIGNALS
      McuLog_trace("changing baud from %d to %d", currBaud, baud);
    #endif
    status_t res = UART_SetBaudRate(McuESP32_CONFIG_UART_DEVICE, baud, CLOCK_GetFreq(McuESP32_CONFIG_UART_GET_CLOCK_FREQ_SELECT));
    if (res!=kStatus_Success) {
      McuLog_error("failed to set baud rate %d", baud);
    }
    currBaud = baud;
  }
}

static void InitUart(void) {
  McuESP32_CONFIG_UART_CONFIG_STRUCT config;

  McuESP32_CONFIG_UART_SET_UART_CLOCK();
  McuESP32_CONFIG_UART_GET_DEFAULT_CONFIG(&config);
  config.baudRate_Bps = McuESP32_CONFIG_UART_BAUDRATE;
  config.enableRx     = true;
  config.enableTx     = true;

  /* Initialize the USART with configuration. */
  McuESP32_CONFIG_UART_INIT(McuESP32_CONFIG_UART_DEVICE, &config, CLOCK_GetFreq(McuESP32_CONFIG_UART_GET_CLOCK_FREQ_SELECT));
  McuESP32_CONFIG_UART_ENABLE_INTERRUPTS(McuESP32_CONFIG_UART_DEVICE, McuESP32_CONFIG_UART_ENABLE_INTERRUPT_FLAGS);
  NVIC_SetPriority(McuESP32_CONFIG_UART_IRQ_NUMBER, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
  EnableIRQ(McuESP32_CONFIG_UART_IRQ_NUMBER);
#if McuESP32_CONFIG_UART_HAS_FIFO
  UART_EnableRxFIFO(McuESP32_CONFIG_UART_DEVICE, true); /* enable UART Rx FIFO */
#endif
  McuShellUart_MuxUartPins(McuESP32_CONFIG_SHELL_UART); /* mux the UART pins */
}

static void InitPins(void) {
#if McuESP32_CONFIG_USE_CTRL_PINS
  McuGPIO_Config_t gpioConfig;

  McuESP32_CONFIG_ENABLE_CTRL_PINS_CLOCK();
  McuGPIO_GetDefaultConfig(&gpioConfig);
  gpioConfig.isInput = true; /* we are initializeing pins as input: that way we do not drive the signals and do not touch them */
  gpioConfig.hw.gpio = McuESP32_CONFIG_EN_GPIO;
  gpioConfig.hw.port = McuESP32_CONFIG_EN_PORT;
  gpioConfig.hw.pin = McuESP32_CONFIG_EN_PIN;
  McuESP32_RF_EN_Pin = McuGPIO_InitGPIO(&gpioConfig);

  gpioConfig.hw.gpio = McuESP32_CONFIG_RST_GPIO;
  gpioConfig.hw.port = McuESP32_CONFIG_RST_PORT;
  gpioConfig.hw.pin = McuESP32_CONFIG_RST_PIN;
  McuESP32_RF_IO0_Pin = McuGPIO_InitGPIO(&gpioConfig);
#endif
}

void McuESP32_Deinit(void) {
#if McuESP32_CONFIG_USE_CTRL_PINS
  McuESP32_RF_EN_Pin = McuGPIO_DeinitGPIO(McuESP32_RF_EN_Pin);
  McuESP32_RF_IO0_Pin = McuGPIO_DeinitGPIO(McuESP32_RF_IO0_Pin);
#endif
  vStreamBufferDelete(rxStreamBuffer);
  rxStreamBuffer = NULL;
  vStreamBufferDelete(txStreamBuffer);
  txStreamBuffer = NULL;
}

void McuESP32_Init(void) {
  InitPins();
  InitQueues();
  InitUart();
  if (xTaskCreate(
      UartRxTask,  /* pointer to the task */
      "ESP32UartRx", /* task name for kernel awareness debugging */
      1024/sizeof(StackType_t), /* task stack size */
      (void*)NULL, /* optional task startup argument */
      McuESP32_CONFIG_PROCESS_PRIORITY,  /* initial priority */
      (TaskHandle_t*)NULL /* optional task handle to create */
    ) != pdPASS)
  {
    McuLog_fatal("failed creating ESP32 Rx Task");
    for(;;){} /* error! probably out of memory */
  }
  if (xTaskCreate(
      UartTxTask,  /* pointer to the task */
      "ESP32UartTx", /* task name for kernel awareness debugging */
      1024/sizeof(StackType_t), /* task stack size */
      (void*)NULL, /* optional task startup argument */
      McuESP32_CONFIG_PROCESS_PRIORITY,  /* initial priority */
      (TaskHandle_t*)NULL /* optional task handle to create */
    ) != pdPASS)
  {
    McuLog_fatal("failed creating ESP32 Tx Task");
    for(;;){} /* error! probably out of memory */
  }
  DoReset(); /* reset ESP */
}
#endif /* #if McuESP32_CONFIG_IS_ENABLED */
