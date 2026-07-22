/*
 * Copyright (c) 2024, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * 
 * cdcTask() -> device USB process -> tud_cdc_rx_cb() -> McuShellCdcDevice_callbacks.buffer_rx_char()
 * 
 */

/*!
 * \file
 * \brief Implementation of McuShellCdcDevice module.
 */
#include "McuLib.h"
#if McuShellCdcDevice_CONFIG_IS_ENABLED
#include "McuShellCdcDevice.h"
#include <stdio.h>
#include "McuShell.h"
#include "McuLog.h"
#include "McuRTOS.h"
#include "McuUtility.h"
#include "McuRB.h"
#include "McuWait.h"
#include "McuXFormat.h"
#include "tusb.h"
#if McuLib_CONFIG_CPU_VARIANT==McuLib_CONFIG_CPU_VARIANT_NXP_K22FN
  #include "MK22F51212.h"
  #include "clock_config.h"
#elif McuLib_CONFIG_CPU_IS_RPxxxx
  /* no other include needed */
#else
  #error "device not supported"
#endif

#if McuShellCdcDevice_CONFIG_USE_FREERTOS
  static QueueHandle_t rxQueue;
#else
  static McuRB_Handle_t rxRingBuffer;
#endif

/* callbacks to deal with the CDC communication channel */
static struct McuShellCdcDevice_s {
  void (*buffer_rx_char)(char ch);   /* called for incoming characters from CDC device */
} McuShellCdcDevice_callbacks;

void McuShellCdcDevice_SetBufferRxCharCallback(void (*buffer_rx_char_cb)(char ch)) {
  McuShellCdcDevice_callbacks.buffer_rx_char = buffer_rx_char_cb;
}

/*********************************************************************************************************/
/* Shell interface */
/*********************************************************************************************************/
static void McuShellCdcDevice_SendChar(unsigned char ch) {
  if (McuShellCdcDevice_IsReady()) {
    #if McuShellCdcDevice_CONFIG_BLOCKING_SEND
    for(int i=0; i<McuShellCdcDevice_CONFIG_BLOCKING_SEND_TIMEOUT_MS; i++) { /* timeout of McuShellCdcDevice_CONFIG_BLOCKING_SEND_TIMEOUT_MS ms */
      if (tud_cdc_n_write_available(0)>=1) {
        (void)tud_cdc_write_char(ch);
        break; /* was able to write it */
      } else {
        McuWait_WaitOSms(McuShellCdcDevice_CONFIG_BLOCKING_SEND_WAIT_MS);
      }
    } /* for */
    #else /* just send: might loose data */
      (void)tud_cdc_write_char(ch);
    #endif
    if (ch=='\n') { /* only flush for new line ending */
      McuShellCdcDevice_Flush();
    }
  } /* if */
}

static void McuShellCdcDevice_ReceiveChar(uint8_t *c) {
  uint8_t ch;

#if McuShellCdcDevice_CONFIG_USE_FREERTOS
  if (xQueueReceive(rxQueue, &ch, 0)==pdPASS ) {
    *c = ch; /* return received character */
  } else {
    *c = '\0'; /* nothing received */
  }
#else
  if (McuRB_Get(rxRingBuffer, &ch)!=ERR_OK) {
    *c = '\0'; /* nothing received */
  } else {
    *c = ch; /* return received character */
  }
#endif
}

int McuShellCdcDevice_ReadByte(void) {
  uint8_t ch;

#if McuShellCdcDevice_CONFIG_USE_FREERTOS
  if (xQueueReceive(rxQueue, &ch, 0)==pdPASS ) {
    return ch; /* return received character */
  } else {
    return EOF; /* nothing received */
  }
#else
  if (McuRB_Get(rxRingBuffer, &ch)==ERR_OK) {
    return ch; /* return received character */
  } else {
    return EOF; /* nothing received */
  }
#endif
}

static bool McuShellCdcDevice_CharPresent(void) {
#if McuShellCdcDevice_CONFIG_USE_FREERTOS
  return uxQueueMessagesWaiting(rxQueue)!=0;
#else
  return McuRB_NofElements(rxRingBuffer)!=0;
#endif
}

void McuShellCdcDevice_QueueChar(char ch) {
#if McuShellCdcDevice_CONFIG_USE_FREERTOS
  if (xQueueSend(rxQueue, &ch, portMAX_DELAY)!=pdPASS) {
    McuLog_fatal("failed adding to queue");
  }
#else
  if (McuRB_Put(rxRingBuffer, &ch)!=ERR_OK) {
    McuLog_fatal("failed adding to queue");
  }
#endif
}

McuShell_ConstStdIOType McuShellCdcDevice_stdio = {
    .stdIn = (McuShell_StdIO_In_FctType)McuShellCdcDevice_ReceiveChar,
    .stdOut = (McuShell_StdIO_OutErr_FctType)McuShellCdcDevice_SendChar,
    .stdErr = (McuShell_StdIO_OutErr_FctType)McuShellCdcDevice_SendChar,
    .keyPressed = McuShellCdcDevice_CharPresent, /* if input is not empty */
  #if McuShell_CONFIG_ECHO_ENABLED
    .echoEnabled = false,
  #endif
};

McuShell_ConstStdIOTypePtr McuShellCdcDevice_GetStdio(void) {
  return &McuShellCdcDevice_stdio;
}

uint8_t McuShellCdcDevice_DefaultShellBuffer[McuShell_DEFAULT_SHELL_BUFFER_SIZE]; /* default buffer which can be used by the application */
/*********************************************************************************************************/

void McuShellCdcDevice_Flush(void) {
  (void)tud_cdc_write_flush();
}

void McuShellCdcDevice_WriteChar(char ch) {
  McuShellCdcDevice_SendChar(ch);
}

void McuShellCdcDevice_ReadChar(char *ch) {
  McuShellCdcDevice_ReceiveChar((uint8_t*)ch);
}

bool McuShellCdcDevice_IsDataPresent(void) {
  return McuShellCdcDevice_CharPresent();
}

void McuShellCdcDevice_WriteAndFlush(const char *buf, size_t count) {
  tud_cdc_n_write(0, buf, count);
  (void)tud_cdc_write_flush();
}

uint32_t McuShellCdcDevice_WriteStr(const char *str) {
  return tud_cdc_write_str(str);
}

unsigned McuShellCdcDevice_printf(const char *fmt, ...) {
  va_list args;
  unsigned int count = 0;

  va_start(args,fmt);
  count = McuXFormat_xvformat(McuShell_printfPutChar, (void*)McuShellCdcDevice_GetStdio()->stdOut, fmt, args);
  va_end(args);
  return count;
}

bool McuShellCdcDevice_IsConnected(void) {
  return tud_cdc_connected(); /* note: 'connected' seems to be not accurate: one can be 'not connected' but 'ready' to send/receive data? */
}

bool McuShellCdcDevice_IsReady(void) { /* device is ready to transfer */
  return tud_cdc_ready();
}

uint8_t McuShellCdcDevice_GetLineState(void) { 
  return tud_cdc_get_line_state();
}

static void McuShellCdcDevice_GetLineCodingStr(unsigned char *buf, size_t bufSize) {
  cdc_line_coding_t info;

  tud_cdc_get_line_coding(&info);
  buf[0] = '\0';
  McuUtility_strcatNum32u(buf, bufSize, info.bit_rate);
  McuUtility_strcat(buf, bufSize, (unsigned char*)", data: ");
  McuUtility_strcatNum8u(buf, bufSize, info.data_bits);
  switch(info.parity) {
    case 0: McuUtility_strcat(buf, bufSize, (unsigned char*)", parity: None"); break;
    case 1: McuUtility_strcat(buf, bufSize, (unsigned char*)", parity: Odd"); break;
    case 2: McuUtility_strcat(buf, bufSize, (unsigned char*)", parity: Even"); break;
    case 3: McuUtility_strcat(buf, bufSize, (unsigned char*)", parity: Mark"); break;
    case 4: McuUtility_strcat(buf, bufSize, (unsigned char*)", parity: Space"); break;
    default: McuUtility_strcat(buf, bufSize, (unsigned char*)", parity: ???"); break;
  }
  switch(info.stop_bits) {
    case 0: McuUtility_strcat(buf, bufSize, (unsigned char*)", stop: 1"); break;
    case 1: McuUtility_strcat(buf, bufSize, (unsigned char*)", stop: 1.5"); break;
    case 2: McuUtility_strcat(buf, bufSize, (unsigned char*)", stop: 2"); break;
    default: McuUtility_strcat(buf, bufSize, (unsigned char*)", stop: ???"); break;
  }
  McuUtility_strcat(buf, bufSize, (unsigned char*)"\r\n");
}

/* Invoked when CDC interface received data from host */
void tud_cdc_rx_cb(uint8_t itf) {
  (void)itf; /*not used */
  char buf[McuShellCdcDevice_CONFIG_RX_BUFFER_SIZE];
  uint32_t count = tud_cdc_read(buf, sizeof(buf));
  
  if (count>0) {
    if (McuShellCdcDevice_callbacks.buffer_rx_char!=NULL) {
  #if McuShellCdcDevice_CONFIG_USE_FREERTOS
      for(int i=0; i<count; i++) {
        McuShellCdcDevice_callbacks.buffer_rx_char(buf[i]);
      }
  #else
      McuRB_Putn(rxRingBuffer, buf, count);
  #endif
    }
  }
}

/*!
 * \brief sets up USB Clock and USB interrupts
 */
static void usb_hardware_init(void) {
#if McuLib_CONFIG_CPU_IS_KINETIS
  #if CFG_TUSB_OS == OPT_OS_FREERTOS
    /* If freeRTOS is used, IRQ priority is limit by max syscall ( smaller is higher ) */
    NVIC_SetPriority(USB0_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY );
  #endif
  /* Set Clock for USB (is not enabled by default. */
  SystemCoreClockUpdate();
  CLOCK_EnableUsbfs0Clock(kCLOCK_UsbSrcIrc48M, 48000000U);
#elif McuLib_CONFIG_CPU_IS_RPxxxx
  /* hardware setup done in SDK */
#else
  #error "target not supported"
#endif
}

#define RH_PORT_NUM  (0)

#if McuLib_CONFIG_CPU_IS_KINETIS
  //--------------------------------------------------------------------+
  // Forward USB interrupt events to TinyUSB IRQ Handler
  //--------------------------------------------------------------------+
  void USB0_IRQHandler(void) {
  #if CFG_TUH_ENABLED
    tuh_int_handler(RH_PORT_NUM);
  #endif
  #if CFG_TUD_ENABLED
    tud_int_handler(RH_PORT_NUM);
  #endif
  }
#endif

static bool tiny_usb_init(void) {
  tusb_rhport_init_t const rhport_init = {
    .role  = TUSB_ROLE_DEVICE,
    .speed = TUSB_SPEED_FULL,
  };
  return tusb_init(RH_PORT_NUM, &rhport_init); /* init device stack on native usb (roothub port0) */
}

static void (*McuShellCdcDevice_RtsCtsCallback)(bool rts, bool cts) = NULL;

void McuShellCdcDevice_SetRtsCtsCallback(void (*callback)(bool rts, bool cts)) {
  McuShellCdcDevice_RtsCtsCallback = callback;
}

/* callback called from tinyusb stack to indicate DTR/RTS changes */
void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts) {
  // http://markdingst.blogspot.com/2014/06/implementing-usb-communication-device.html
  // bit 0: Indicates to DCE if DTE is present or not. This signal corresponds to V.24 signal 108/2 and RS232 signal DTR.
  // 0: DTE is not present.
  // 1: DTE is present

  // bit 1: Carrier control for half duplex modems. This signal corresponds to V.24 signal 105 and RS232 signal RTS.
  //  0: Deactivate carrier.
  //  1: Activate carrier.
  //  The device ignores the value of this bit when operating in full duplex mode.
  /*
   * Keep this callback short. If substantial processing is needed,
   * set an event flag and handle it in the application task.
   */
  if (McuShellCdcDevice_RtsCtsCallback!=NULL) {
    McuShellCdcDevice_RtsCtsCallback(dtr, rts);
  }
}

void tud_cdc_line_coding_cb(uint8_t itf, cdc_line_coding_t const *coding) {
  McuLog_info("tinyusb: request for %d baud", coding->bit_rate);
  //uart_set_baudrate(coding->bit_rate);
}

static void UsbDeviceRestart(void) {
  tud_deinit(RH_PORT_NUM);
  McuWait_WaitOSms(100);
  tiny_usb_init();
}

#if McuShellCdcDevice_CONFIG_USE_FREERTOS
  static void cdcTask(void *pv) {
    usb_hardware_init();
    if (!tiny_usb_init()) {
      McuLog_fatal("failed initializing tinyusb");
      for(;;) { /* error */}
    }
    for(;;) {
      tud_task(); /* tinyusb (CDC) device task */
      #if 0 /* no need to wait, as tinyUSB will block inside tud_task() and waits for data */
        vTaskDelay(pdMS_TO_TICKS(pdMS_TO_TICKS(McuShellCdcDevice_CONFIG_PROCESS_WAIT_TIME_MS)));
      #endif
    }
  }
#else
  void McuShellCdcDevice_Process(void) {
    tud_task(); /* tinyusb (CDC) device task */
  }
#endif

static uint8_t PrintStatus(McuShell_ConstStdIOType *io) {
  unsigned char buf[64];
  uint8_t val;

  McuShell_SendStatusStr((const unsigned char*)"McuShellCdc", (const unsigned char*)"McuShellCdc module status\r\n", io->stdOut);
  McuShell_SendStatusStr((const unsigned char*)"  ready", McuShellCdcDevice_IsReady()?(const unsigned char*)"yes\r\n":(const unsigned char*)"no\r\n", io->stdOut);
  
  val = McuShellCdcDevice_GetLineState();
  McuUtility_strcpy(buf, sizeof(buf), (unsigned char*)"0x");
  McuUtility_strcatNum8Hex(buf, sizeof(buf), val);
  McuUtility_strcat(buf, sizeof(buf), (unsigned char*)"\r\n");
  McuShell_SendStatusStr((const unsigned char*)"  line state", buf, io->stdOut);

  McuShellCdcDevice_GetLineCodingStr(buf, sizeof(buf));
  McuShell_SendStatusStr((const unsigned char*)"  line coding", buf, io->stdOut);
  return ERR_OK;
}

static uint8_t PrintHelp(McuShell_ConstStdIOType *io) {
  McuShell_SendHelpStr((unsigned char*)"McuShellCdc", (const unsigned char*)"Group of McuShellCdc commands\r\n", io->stdOut);
  McuShell_SendHelpStr((unsigned char*)"  help|status", (const unsigned char*)"Print help or status information\r\n", io->stdOut);
  McuShell_SendHelpStr((unsigned char*)"  restart", (const unsigned char*)"Restart USB stack\r\n", io->stdOut);
  McuShell_SendHelpStr((unsigned char*)"  text <txt>", (const unsigned char*)"Send text\r\n", io->stdOut);
  return ERR_OK;
}

uint8_t McuShellCdcDevice_ParseCommand(const unsigned char *cmd, bool *handled, const McuShell_StdIOType *io) {
  if (McuUtility_strcmp((char*)cmd, McuShell_CMD_HELP)==0 || McuUtility_strcmp((char*)cmd, "McuShellCdc help")==0) {
    *handled = true;
    return PrintHelp(io);
  } else if ((McuUtility_strcmp((char*)cmd, McuShell_CMD_STATUS)==0) || (McuUtility_strcmp((char*)cmd, "McuShellCdc status")==0)) {
    *handled = true;
    return PrintStatus(io);
  } else if (McuUtility_strcmp((char*)cmd, "McuShellCdc restart")==0) {
    *handled = true;
    UsbDeviceRestart();
    return ERR_OK;
  } else if (McuUtility_strncmp((char*)cmd, "McuShellCdc text ", sizeof("McuShellCdc text ")-1)==0) {
    *handled = true;
    McuShellCdcDevice_WriteStr((char*)cmd + sizeof("McuShellCdc text ")-1);
    McuShellCdcDevice_WriteStr("\r\n");
    McuShellCdcDevice_Flush();
    return ERR_OK;
  }
  return ERR_OK;
}

void McuShellCdcDevice_Deinit(void) {
  McuShellCdcDevice_SetBufferRxCharCallback(NULL);
#if McuShellCdcDevice_CONFIG_USE_FREERTOS
  vQueueDelete(rxQueue);
  rxQueue = NULL;
#else
  rxRingBuffer = McuRB_DeinitRB(rxRingBuffer);
#endif
}

void McuShellCdcDevice_Init(void) {
#if McuShellCdcDevice_CONFIG_USE_FREERTOS
  BaseType_t res = xTaskCreate(cdcTask, "cdcTask", 4*1024/sizeof(StackType_t), NULL, McuShellCdcDevice_CONFIG_PROCESS_PRIORITY, NULL);
  if (res!=pdPASS) {
    McuLog_fatal("creating ShellTask task failed!");
    for(;;) {}
  }
  rxQueue = xQueueCreate(McuShellCdcDevice_CONFIG_RX_BUFFER_SIZE, sizeof(uint8_t));
  if (rxQueue==NULL) {
    for(;;){} /* out of memory? */
  }
  vQueueAddToRegistry(rxQueue, "rxQueue");
#else
  McuRB_Config_t config;

  McuRB_GetDefaultconfig(&config);
  config.elementSize = sizeof(uint8_t);
  config.nofElements = McuShellCdcDevice_CONFIG_RX_BUFFER_SIZE;
  rxRingBuffer = McuRB_InitRB(&config);
  if (rxRingBuffer==NULL) {
    for(;;) {/* error */}
  }
#endif
}

#endif /* McuShellCdcDevice_CONFIG_IS_ENABLED */