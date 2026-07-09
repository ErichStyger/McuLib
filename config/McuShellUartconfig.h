/*!
 * Copyright (c) 2020-2024, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * \file
 * \brief \brief Configuration items for the McuShellUart module.
 */

#ifndef MCUSHELLUARTCONFIG_H_
#define MCUSHELLUARTCONFIG_H_

/* supported UART implementation: */
#define McuShellUart_CONFIG_UART_NONE                         (0)
/* LPC845 */
#define McuShellUart_CONFIG_UART_LPC845_USART0                (1) /* Rx on pin 20 (PIO0_24), Tx on pin 19 (PIO0_25) */
/* Kinetis K22FX */
#define McuShellUart_CONFIG_UART_K22FX512_UART0_A1_A2         (2) /* Rx on PTA1 (pin 27), Tx on PTA2 (pin 28) */
#define McuShellUart_CONFIG_UART_K22FX512_UART1_E1_E0         (3) /* Rx on PTE1 (pin 2), Tx on PTE2 (pin 1) */
/* Kinetis K22FN */
#define McuShellUart_CONFIG_UART_K22FN512_LPUART0_C3_C4       (4) /* PTC3: Rx, (pin 46), PTC4: Tx (pin 49) (OpenSDA UART on tinyK22) */
#define McuShellUart_CONFIG_UART_K22FN512_UART0_B16_B17       (5) /* PTB16 (Rx), PTB17 (Tx) */
#define McuShellUart_CONFIG_UART_K22FN512_UART0_A1_A2         (6) /* PTA1 (Rx), PTA1 (Tx) */
#define McuShellUart_CONFIG_UART_K22FN512_UART1_E1_E0         (7) /* PTE1 (Rx), PTE0 (Tx) (OpenSDA UART on FRDM-K22F) */
#define McuShellUart_CONFIG_UART_K22FN512_UART2_D2_D3         (8) /* PTD2 (Rx), PTD3 (Tx) */
/* Kinetis K64FN1M */
#define McuShellUart_CONFIG_UART_K64FN1M_UART0_B16_B17        (9) /* PTB16 (Rx), PTB17 (Tx) */
/* LPC55S16 */
#define McuShellUart_CONFIG_UART_LPC55S16_USART0              (10)  /* FlexComm0, pin 92 (Rx) and pin 94 (Tx) */
#define McuShellUart_CONFIG_UART_LPC55S16_USART2              (11) /* FlexComm2, pin  3 (Rx) and pin 27 (Tx) */
/* LPC55S69 */
#define McuShellUart_CONFIG_UART_LPC55S69_USART0              (12) /* FlexComm0, P0_29, pin92 (Rx) and P0_30, pin94 (Tx) */
#define McuShellUart_CONFIG_UART_LPC55S69_USART1              (13) /* FlexComm1, P1_10, pin40 (Rx) and P1_11, pin93 (Tx) */
/* RP2040 */
#define McuShellUart_CONFIG_UART_RP2040_UART1_GPIO4_GPIO5     (14) /* UART1 with Tx on GPIO4 and Rx on GPIO5 */
/* NXP FRDM-MCXN947 */
#define McuShellUart_CONFIG_UART_MCXN947_FC4_P1_8_P1_9        (15) /* LPUART4 on FlexComm4 with Tx on P1_8/FC4 and Rx on P1_9/FC4 */
/* NXP FRDM-MCXA153 */
#define McuShellUart_CONFIG_UART_MCXN153_LPUART0_P0_2_P0_3    (16) /* LPUART0 with Tx on P0_3 and Rx on P0_2 */

/* default UART used */
#ifndef McuShellUart_CONFIG_UART
  #define McuShellUart_CONFIG_UART      McuShellUart_CONFIG_UART_NONE
#endif

#ifndef McuShellUart_CONFIG_DO_MUXING
  #define McuShellUart_CONFIG_DO_PIN_MUXING    (1)
    /*!< 1: Do the pin muxing in the Init(); 0: no pin muxing is done in the Init() */
#endif

/* UART configuration items */
#if McuShellUart_CONFIG_UART==McuShellUart_CONFIG_UART_NONE
  /* no UART used */
#elif McuShellUart_CONFIG_UART==McuShellUart_CONFIG_UART_LPC845_USART0
  #include "fsl_usart.h"
  #include "fsl_swm.h"
  #define McuShellUart_CONFIG_UART_DEVICE                   USART0
  #define McuShellUart_CONFIG_UART_SET_UART_CLOCK()         CLOCK_Select(kUART0_Clk_From_MainClk) /* Select the main clock as source clock of USART0. */
  #define McuShellUart_CONFIG_UART_WRITE_BLOCKING           USART_WriteBlocking
  #define McuShellUart_CONFIG_UART_GET_FLAGS                USART_GetStatusFlags
  #define McuShellUart_CONFIG_UART_HW_RX_READY_FLAGS        (kUSART_RxReady|kUSART_HardwareOverrunFlag)
  #define McuShellUart_CONFIG_UART_READ_BYTE                USART_ReadByte
  #define McuShellUart_CONFIG_UART_CONFIG_STRUCT            usart_config_t
  #define McuShellUart_CONFIG_UART_GET_DEFAULT_CONFIG       USART_GetDefaultConfig
  #define McuShellUart_CONFIG_UART_ENABLE_INTERRUPTS        USART_EnableInterrupts
  #define McuShellUart_CONFIG_UART_ENABLE_INTERRUPT_FLAGS   (kUSART_RxReadyInterruptEnable | kUSART_HardwareOverRunInterruptEnable)
  #define McuShellUart_CONFIG_UART_IRQ_NUMBER               USART0_IRQn
  #define McuShellUart_CONFIG_UART_INIT                     USART_Init
  #ifndef McuShellUart_CONFIG_UART_GET_CLOCK_FREQ_SELECT
    #define McuShellUart_CONFIG_UART_GET_CLOCK_FREQ_SELECT  kCLOCK_MainClk
  #endif
  #define McuShellUart_CONFIG_UART_IRQ_HANDLER              USART0_IRQHandler
  #define McuShellUART_CONFIG_CLEAR_STATUS_FLAGS            USART_ClearStatusFlags
  #define McuShellUART_CONFIG_CLEAR_EXTRA_STATUS_FLAGS      (0) /* no extra flags to clear */
  #define McuShellUart_CONFIG_HAS_FIFO                      (0)
  
  #ifndef McuShellUart_CONFIG_DO_CONFIGURE_CLOCKS
    #define McuShellUart_CONFIG_DO_CONFIGURE_CLOCKS            (0) 
      /*!< if we configure the clocks for the UART or not */
  #endif
#elif McuShellUart_CONFIG_UART==McuShellUart_CONFIG_UART_LPC55S16_USART0
  #include "fsl_usart.h"
  #include "fsl_iocon.h"
  #define McuShellUart_CONFIG_UART_DEVICE                   USART0
  #define McuShellUart_CONFIG_UART_SET_UART_CLOCK()         CLOCK_AttachClk(kFRO12M_to_FLEXCOMM0)
  #define McuShellUart_CONFIG_UART_WRITE_BLOCKING           USART_WriteBlocking
  #define McuShellUart_CONFIG_UART_GET_FLAGS                USART_GetStatusFlags
  #define McuShellUart_CONFIG_UART_HW_RX_READY_FLAGS        (kUSART_RxFifoNotEmptyFlag | kUSART_RxError)
  #define McuShellUart_CONFIG_UART_READ_BYTE                USART_ReadByte
  #define McuShellUart_CONFIG_UART_CONFIG_STRUCT            usart_config_t
  #define McuShellUart_CONFIG_UART_GET_DEFAULT_CONFIG       USART_GetDefaultConfig
  #define McuShellUart_CONFIG_UART_ENABLE_INTERRUPTS        USART_EnableInterrupts
  #define McuShellUart_CONFIG_UART_ENABLE_INTERRUPT_FLAGS   (kUSART_RxLevelInterruptEnable | kUSART_RxErrorInterruptEnable)
  #define McuShellUart_CONFIG_UART_IRQ_NUMBER               FLEXCOMM0_IRQn
  #define McuShellUart_CONFIG_UART_INIT                     USART_Init
  #ifndef McuShellUart_CONFIG_UART_GET_CLOCK_FREQ_SELECT
    #define McuShellUart_CONFIG_UART_GET_CLOCK_FREQ_SELECT  kCLOCK_Fro12M
  #endif
  #define McuShellUart_CONFIG_UART_IRQ_HANDLER              FLEXCOMM0_IRQHandler
  #define McuShellUART_CONFIG_CLEAR_STATUS_FLAGS            USART_ClearStatusFlags
  #define McuShellUART_CONFIG_CLEAR_EXTRA_STATUS_FLAGS      (0) /* no extra flags to clear */
  
  #ifndef McuShellUart_CONFIG_DO_CONFIGURE_CLOCKS
    #define McuShellUart_CONFIG_DO_CONFIGURE_CLOCKS            (0) 
      /*!< if we configure the clocks for the UART or not */
  #endif
#elif McuShellUart_CONFIG_UART==McuShellUart_CONFIG_UART_LPC55S16_USART2
  #include "fsl_usart.h"
  #include "fsl_iocon.h"

  #define McuShellUart_CONFIG_UART_DEVICE                   USART2
  #define McuShellUart_CONFIG_UART_SET_UART_CLOCK()         CLOCK_AttachClk(kFRO12M_to_FLEXCOMM2)
  #define McuShellUart_CONFIG_UART_WRITE_BLOCKING           USART_WriteBlocking
  #define McuShellUart_CONFIG_UART_GET_FLAGS                USART_GetStatusFlags
  #define McuShellUart_CONFIG_UART_HW_RX_READY_FLAGS        (kUSART_RxFifoNotEmptyFlag | kUSART_RxError)
  #define McuShellUart_CONFIG_UART_READ_BYTE                USART_ReadByte
  #define McuShellUart_CONFIG_UART_CONFIG_STRUCT            usart_config_t
  #define McuShellUart_CONFIG_UART_GET_DEFAULT_CONFIG       USART_GetDefaultConfig
  #define McuShellUart_CONFIG_UART_ENABLE_INTERRUPTS        USART_EnableInterrupts
  #define McuShellUart_CONFIG_UART_ENABLE_INTERRUPT_FLAGS   (kUSART_RxLevelInterruptEnable | kUSART_RxErrorInterruptEnable)
  #define McuShellUart_CONFIG_UART_IRQ_NUMBER               FLEXCOMM2_IRQn
  #define McuShellUart_CONFIG_UART_INIT                     USART_Init
  #ifndef McuShellUart_CONFIG_UART_GET_CLOCK_FREQ_SELECT
    #define McuShellUart_CONFIG_UART_GET_CLOCK_FREQ_SELECT  kCLOCK_Fro12M
  #endif
  #define McuShellUart_CONFIG_UART_IRQ_HANDLER              FLEXCOMM2_IRQHandler
  #define McuShellUART_CONFIG_CLEAR_STATUS_FLAGS            USART_ClearStatusFlags
  #define McuShellUART_CONFIG_CLEAR_EXTRA_STATUS_FLAGS      (0) /* no extra flags to clear */
  
  #ifndef McuShellUart_CONFIG_DO_CONFIGURE_CLOCKS
    #define McuShellUart_CONFIG_DO_CONFIGURE_CLOCKS            (0) 
      /*!< if we configure the clocks for the UART or not */
  #endif
#elif McuShellUart_CONFIG_UART==McuShellUart_CONFIG_UART_LPC55S69_USART0
  #include "fsl_usart.h"
  #include "fsl_iocon.h"
  #define McuShellUart_CONFIG_UART_DEVICE                   USART0
  #define McuShellUart_CONFIG_UART_SET_UART_CLOCK()         CLOCK_AttachClk(kFRO12M_to_FLEXCOMM0)
  #define McuShellUart_CONFIG_UART_WRITE_BLOCKING           USART_WriteBlocking
  #define McuShellUart_CONFIG_UART_GET_FLAGS                USART_GetStatusFlags
  #define McuShellUart_CONFIG_UART_HW_RX_READY_FLAGS        (kUSART_RxFifoNotEmptyFlag | kUSART_RxError)
  #define McuShellUart_CONFIG_UART_READ_BYTE                USART_ReadByte
  #define McuShellUart_CONFIG_UART_CONFIG_STRUCT            usart_config_t
  #define McuShellUart_CONFIG_UART_GET_DEFAULT_CONFIG       USART_GetDefaultConfig
  #define McuShellUart_CONFIG_UART_ENABLE_INTERRUPTS        USART_EnableInterrupts
  #define McuShellUart_CONFIG_UART_ENABLE_INTERRUPT_FLAGS   (kUSART_RxLevelInterruptEnable | kUSART_RxErrorInterruptEnable)
  #define McuShellUart_CONFIG_UART_IRQ_NUMBER               FLEXCOMM0_IRQn
  #define McuShellUart_CONFIG_UART_INIT                     USART_Init
  #ifndef McuShellUart_CONFIG_UART_GET_CLOCK_FREQ_SELECT
    #define McuShellUart_CONFIG_UART_GET_CLOCK_FREQ_SELECT  kCLOCK_Fro12M
  #endif
  #define McuShellUart_CONFIG_UART_IRQ_HANDLER              FLEXCOMM0_IRQHandler
  #define McuShellUART_CONFIG_CLEAR_STATUS_FLAGS            USART_ClearStatusFlags
  #define McuShellUART_CONFIG_CLEAR_EXTRA_STATUS_FLAGS      (0) /* no extra flags to clear */
  
  #ifndef McuShellUart_CONFIG_DO_CONFIGURE_CLOCKS
    #define McuShellUart_CONFIG_DO_CONFIGURE_CLOCKS            (0) 
      /*!< if we configure the clocks for the UART or not */
  #endif
#elif McuShellUart_CONFIG_UART==McuShellUart_CONFIG_UART_LPC55S69_USART1
  #include "fsl_usart.h"
  #include "fsl_iocon.h"
  #define McuShellUart_CONFIG_UART_DEVICE                   USART1
  #define McuShellUart_CONFIG_UART_SET_UART_CLOCK()         CLOCK_AttachClk(kFRO12M_to_FLEXCOMM1)
  #define McuShellUart_CONFIG_UART_WRITE_BLOCKING           USART_WriteBlocking
  #define McuShellUart_CONFIG_UART_GET_FLAGS                USART_GetStatusFlags
  #define McuShellUart_CONFIG_UART_HW_RX_READY_FLAGS        (kUSART_RxFifoNotEmptyFlag | kUSART_RxError)
  #define McuShellUart_CONFIG_UART_READ_BYTE                USART_ReadByte
  #define McuShellUart_CONFIG_UART_CONFIG_STRUCT            usart_config_t
  #define McuShellUart_CONFIG_UART_GET_DEFAULT_CONFIG       USART_GetDefaultConfig
  #define McuShellUart_CONFIG_UART_ENABLE_INTERRUPTS        USART_EnableInterrupts
  #define McuShellUart_CONFIG_UART_ENABLE_INTERRUPT_FLAGS   (kUSART_RxLevelInterruptEnable | kUSART_RxErrorInterruptEnable)
  #define McuShellUart_CONFIG_UART_IRQ_NUMBER               FLEXCOMM1_IRQn
  #define McuShellUart_CONFIG_UART_INIT                     USART_Init
  #ifndef McuShellUart_CONFIG_UART_GET_CLOCK_FREQ_SELECT
    #define McuShellUart_CONFIG_UART_GET_CLOCK_FREQ_SELECT  kCLOCK_Fro12M
  #endif
  #define McuShellUart_CONFIG_UART_IRQ_HANDLER              FLEXCOMM1_IRQHandler
  #define McuShellUART_CONFIG_CLEAR_STATUS_FLAGS            USART_ClearStatusFlags
  #define McuShellUART_CONFIG_CLEAR_EXTRA_STATUS_FLAGS      (0) /* no extra flags to clear */
  
  #ifndef McuShellUart_CONFIG_DO_CONFIGURE_CLOCKS
    #define McuShellUart_CONFIG_DO_CONFIGURE_CLOCKS            (0) 
      /*!< if we configure the clocks for the UART or not */
  #endif
#elif McuShellUart_CONFIG_UART==McuShellUart_CONFIG_UART_MCXN947_FC4_P1_8_P1_9
  #include "fsl_lpuart.h"
  #include "fsl_port.h"
  #define McuShellUart_CONFIG_UART_DEVICE                   LPUART4
  #define McuShellUart_CONFIG_UART_SET_UART_CLOCK()         
  #define McuShellUart_CONFIG_UART_WRITE_BLOCKING           LPUART_WriteBlocking
  #define McuShellUart_CONFIG_UART_GET_FLAGS                LPUART_GetStatusFlags
  #define McuShellUart_CONFIG_UART_HW_RX_READY_FLAGS        (kLPUART_RxDataRegFullFlag)
  #define McuShellUart_CONFIG_UART_READ_BYTE                LPUART_ReadByte
  #define McuShellUart_CONFIG_UART_CONFIG_STRUCT            lpuart_config_t
  #define McuShellUart_CONFIG_UART_GET_DEFAULT_CONFIG       LPUART_GetDefaultConfig
  #define McuShellUart_CONFIG_UART_ENABLE_INTERRUPTS        LPUART_EnableInterrupts
  #define McuShellUart_CONFIG_UART_ENABLE_INTERRUPT_FLAGS   (kLPUART_RxDataRegFullInterruptEnable)
  #define McuShellUart_CONFIG_UART_IRQ_NUMBER               LP_FLEXCOMM4_IRQn
  #define McuShellUart_CONFIG_UART_INIT                     LPUART_Init
  #ifndef McuShellUart_CONFIG_UART_GET_CLOCK_FREQ_SELECT
    #define McuShellUart_CONFIG_UART_GET_CLOCK_FREQ_SELECT  CLOCK_GetLPFlexCommClkFreq(4u)
  #endif
  #define McuShellUart_CONFIG_UART_IRQ_HANDLER              LP_FLEXCOMM4_IRQHandler
  #define McuShellUART_CONFIG_CLEAR_STATUS_FLAGS            LPUART_ClearStatusFlags
  #define McuShellUART_CONFIG_CLEAR_EXTRA_STATUS_FLAGS      (0) /* no extra flags to clear */
  
  #ifndef McuShellUart_CONFIG_DO_CONFIGURE_CLOCKS
    #define McuShellUart_CONFIG_DO_CONFIGURE_CLOCKS            (1) 
      /*!< if we configure the clocks for the UART or not */
  #endif

  #ifndef McuShellUart_CONFIG_CLOCKING_CONFIG
    #define McuShellUart_CONFIG_CLOCKING_CONFIG()  \
      /* attach FRO 12M to FLEXCOMM4 (debug console) */ \
      CLOCK_SetClkDiv(kCLOCK_DivFlexcom4Clk, 1u); \
      CLOCK_AttachClk(kFRO12M_to_FLEXCOMM4);
  #endif
#elif McuShellUart_CONFIG_UART==McuShellUart_CONFIG_UART_MCXN153_LPUART0_P0_2_P0_3
  #include "fsl_lpuart.h"
  #include "fsl_port.h"
  #define McuShellUart_CONFIG_UART_DEVICE                   LPUART0
  #define McuShellUart_CONFIG_UART_SET_UART_CLOCK()         
  #define McuShellUart_CONFIG_UART_WRITE_BLOCKING           LPUART_WriteBlocking
  #define McuShellUart_CONFIG_UART_GET_FLAGS                LPUART_GetStatusFlags
  #define McuShellUart_CONFIG_UART_HW_RX_READY_FLAGS        (kLPUART_RxDataRegFullFlag)
  #define McuShellUart_CONFIG_UART_READ_BYTE                LPUART_ReadByte
  #define McuShellUart_CONFIG_UART_CONFIG_STRUCT            lpuart_config_t
  #define McuShellUart_CONFIG_UART_GET_DEFAULT_CONFIG       LPUART_GetDefaultConfig
  #define McuShellUart_CONFIG_UART_ENABLE_INTERRUPTS        LPUART_EnableInterrupts
  #define McuShellUart_CONFIG_UART_ENABLE_INTERRUPT_FLAGS   (kLPUART_RxDataRegFullInterruptEnable)
  #define McuShellUart_CONFIG_UART_IRQ_NUMBER               LPUART0_IRQn
  #define McuShellUart_CONFIG_UART_INIT                     LPUART_Init
  #ifndef McuShellUart_CONFIG_UART_GET_CLOCK_FREQ_SELECT
    #define McuShellUart_CONFIG_UART_GET_CLOCK_FREQ_SELECT  kCLOCK_MainClk
  #endif
  #define McuShellUart_CONFIG_UART_IRQ_HANDLER              LPUART0_IRQHandler
  #define McuShellUART_CONFIG_CLEAR_STATUS_FLAGS            LPUART_ClearStatusFlags
  #define McuShellUART_CONFIG_CLEAR_EXTRA_STATUS_FLAGS      (0) /* no extra flags to clear */

  #ifndef McuShellUart_CONFIG_DO_CONFIGURE_CLOCKS
    #define McuShellUart_CONFIG_DO_CONFIGURE_CLOCKS            (1) 
      /*!< if we configure the clocks for the UART or not */
  #endif

  #ifndef McuShellUart_CONFIG_CLOCKING_CONFIG
    #define McuShellUart_CONFIG_CLOCKING_CONFIG()  \
    /* Write to PORT0: Peripheral clock is enabled */ \
    CLOCK_EnableClock(kCLOCK_GatePORT0); \
    /* LPUART0 peripheral is released from reset */ \
    RESET_ReleasePeripheralReset(kLPUART0_RST_SHIFT_RSTn); \
    /* PORT0 peripheral is released from reset */ \
    RESET_ReleasePeripheralReset(kPORT0_RST_SHIFT_RSTn);
  #endif

#elif McuShellUart_CONFIG_UART==McuShellUart_CONFIG_UART_K22FX512_UART0_A1_A2
  /* UART0 on K22FX512 */
  #include "fsl_uart.h"
  #include "fsl_port.h"
  #define McuShellUart_CONFIG_UART_DEVICE                   UART0
  #define McuShellUart_CONFIG_UART_SET_UART_CLOCK()         /* nothing needed */
  #define McuShellUart_CONFIG_UART_WRITE_BLOCKING           UART_WriteBlocking
  #define McuShellUart_CONFIG_UART_GET_FLAGS                UART_GetStatusFlags
  #define McuShellUart_CONFIG_UART_HW_RX_READY_FLAGS        (kUART_RxDataRegFullFlag|kUART_RxOverrunFlag)
  #define McuShellUart_CONFIG_UART_READ_BYTE                UART_ReadByte
  #define McuShellUart_CONFIG_UART_CONFIG_STRUCT            uart_config_t
  #define McuShellUart_CONFIG_UART_GET_DEFAULT_CONFIG       UART_GetDefaultConfig
  #define McuShellUart_CONFIG_UART_ENABLE_INTERRUPTS        UART_EnableInterrupts
  #define McuShellUart_CONFIG_UART_ENABLE_INTERRUPT_FLAGS   (kUART_RxDataRegFullInterruptEnable | kUART_RxOverrunInterruptEnable)
  #define McuShellUart_CONFIG_UART_IRQ_NUMBER               UART0_RX_TX_IRQn
  #define McuShellUart_CONFIG_UART_INIT                     UART_Init
  #ifndef McuShellUart_CONFIG_UART_GET_CLOCK_FREQ_SELECT
    #define McuShellUart_CONFIG_UART_GET_CLOCK_FREQ_SELECT    kCLOCK_CoreSysClk
  #endif
  #define McuShellUart_CONFIG_UART_IRQ_HANDLER              UART0_RX_TX_IRQHandler
  #define McuShellUART_CONFIG_CLEAR_STATUS_FLAGS            UART_ClearStatusFlags
  #define McuShellUART_CONFIG_CLEAR_EXTRA_STATUS_FLAGS      (kUART_RxOverrunFlag|kUART_RxFifoOverflowFlag)
  #define McuShellUart_CONFIG_HAS_FIFO                      (1)

  #ifndef McuShellUart_CONFIG_DO_CONFIGURE_CLOCKS
    #define McuShellUart_CONFIG_DO_CONFIGURE_CLOCKS            (0) 
      /*!< if we configure the clocks for the UART or not */
  #endif
#elif McuShellUart_CONFIG_UART==McuShellUart_CONFIG_UART_K22FN512_UART0_A1_A2
  #include "fsl_uart.h"
  #include "fsl_port.h"
  #define McuShellUart_CONFIG_UART_DEVICE                   UART0
  #define McuShellUart_CONFIG_UART_SET_UART_CLOCK()         /* nothing needed */
  #define McuShellUart_CONFIG_UART_WRITE_BLOCKING           UART_WriteBlocking
  #define McuShellUart_CONFIG_UART_GET_FLAGS                UART_GetStatusFlags
  #define McuShellUart_CONFIG_UART_HW_RX_READY_FLAGS        (kUART_RxDataRegFullFlag|kUART_RxOverrunFlag)
  #define McuShellUart_CONFIG_UART_READ_BYTE                UART_ReadByte
  #define McuShellUart_CONFIG_UART_CONFIG_STRUCT            uart_config_t
  #define McuShellUart_CONFIG_UART_GET_DEFAULT_CONFIG       UART_GetDefaultConfig
  #define McuShellUart_CONFIG_UART_ENABLE_INTERRUPTS        UART_EnableInterrupts
  #define McuShellUart_CONFIG_UART_ENABLE_INTERRUPT_FLAGS   (kUART_RxDataRegFullInterruptEnable | kUART_RxOverrunInterruptEnable)
  #define McuShellUart_CONFIG_UART_IRQ_NUMBER               UART0_RX_TX_IRQn
  #define McuShellUart_CONFIG_UART_INIT                     UART_Init
  #ifndef McuShellUart_CONFIG_UART_GET_CLOCK_FREQ_SELECT
    #define McuShellUart_CONFIG_UART_GET_CLOCK_FREQ_SELECT  kCLOCK_CoreSysClk
  #endif
  #define McuShellUart_CONFIG_UART_IRQ_HANDLER              UART0_RX_TX_IRQHandler
  #define McuShellUART_CONFIG_CLEAR_STATUS_FLAGS            UART_ClearStatusFlags
  #define McuShellUART_CONFIG_CLEAR_EXTRA_STATUS_FLAGS      (kUART_RxOverrunFlag|kUART_RxFifoOverflowFlag)
  #define McuShellUart_CONFIG_HAS_FIFO                      (1)
  
  #ifndef McuShellUart_CONFIG_DO_CONFIGURE_CLOCKS
    #define McuShellUart_CONFIG_DO_CONFIGURE_CLOCKS            (0) 
      /*!< if we configure the clocks for the UART or not */
  #endif
#elif McuShellUart_CONFIG_UART==McuShellUart_CONFIG_UART_K22FN512_UART0_B16_B17
  #include "fsl_uart.h"
  #include "fsl_port.h"
  #define McuShellUart_CONFIG_UART_DEVICE                   UART0
  #define McuShellUart_CONFIG_UART_SET_UART_CLOCK()         /* nothing needed */
  #define McuShellUart_CONFIG_UART_WRITE_BLOCKING           UART_WriteBlocking
  #define McuShellUart_CONFIG_UART_GET_FLAGS                UART_GetStatusFlags
  #define McuShellUart_CONFIG_UART_HW_RX_READY_FLAGS        (kUART_RxDataRegFullFlag|kUART_RxOverrunFlag)
  #define McuShellUart_CONFIG_UART_READ_BYTE                UART_ReadByte
  #define McuShellUart_CONFIG_UART_CONFIG_STRUCT            uart_config_t
  #define McuShellUart_CONFIG_UART_GET_DEFAULT_CONFIG       UART_GetDefaultConfig
  #define McuShellUart_CONFIG_UART_ENABLE_INTERRUPTS        UART_EnableInterrupts
  #define McuShellUart_CONFIG_UART_ENABLE_INTERRUPT_FLAGS   (kUART_RxDataRegFullInterruptEnable | kUART_RxOverrunInterruptEnable)
  #define McuShellUart_CONFIG_UART_IRQ_NUMBER               UART0_RX_TX_IRQn
  #define McuShellUart_CONFIG_UART_INIT                     UART_Init
  #ifndef McuShellUart_CONFIG_UART_GET_CLOCK_FREQ_SELECT
    #define McuShellUart_CONFIG_UART_GET_CLOCK_FREQ_SELECT  kCLOCK_CoreSysClk
  #endif
  #define McuShellUart_CONFIG_UART_IRQ_HANDLER              UART0_RX_TX_IRQHandler
  #define McuShellUART_CONFIG_CLEAR_STATUS_FLAGS            UART_ClearStatusFlags
  #define McuShellUART_CONFIG_CLEAR_EXTRA_STATUS_FLAGS      (kUART_RxOverrunFlag|kUART_RxFifoOverflowFlag)
  #define McuShellUart_CONFIG_HAS_FIFO                      (1)
  
  #ifndef McuShellUart_CONFIG_DO_CONFIGURE_CLOCKS
    #define McuShellUart_CONFIG_DO_CONFIGURE_CLOCKS            (0) 
      /*!< if we configure the clocks for the UART or not */
  #endif
#elif McuShellUart_CONFIG_UART==McuShellUart_CONFIG_UART_K22FN512_UART1_E1_E0
  #include "fsl_uart.h"
  #include "fsl_port.h"
  #define McuShellUart_CONFIG_UART_DEVICE                   UART1
  #define McuShellUart_CONFIG_UART_SET_UART_CLOCK()         /* nothing needed */
  #define McuShellUart_CONFIG_UART_WRITE_BLOCKING           UART_WriteBlocking
  #define McuShellUart_CONFIG_UART_GET_FLAGS                UART_GetStatusFlags
  #define McuShellUart_CONFIG_UART_HW_RX_READY_FLAGS        (kUART_RxDataRegFullFlag|kUART_RxOverrunFlag)
  #define McuShellUart_CONFIG_UART_READ_BYTE                UART_ReadByte
  #define McuShellUart_CONFIG_UART_CONFIG_STRUCT            uart_config_t
  #define McuShellUart_CONFIG_UART_GET_DEFAULT_CONFIG       UART_GetDefaultConfig
  #define McuShellUart_CONFIG_UART_ENABLE_INTERRUPTS        UART_EnableInterrupts
  #define McuShellUart_CONFIG_UART_ENABLE_INTERRUPT_FLAGS   (kUART_RxDataRegFullInterruptEnable | kUART_RxOverrunInterruptEnable)
  #define McuShellUart_CONFIG_UART_IRQ_NUMBER               UART1_RX_TX_IRQn
  #define McuShellUart_CONFIG_UART_INIT                     UART_Init
  #ifndef McuShellUart_CONFIG_UART_GET_CLOCK_FREQ_SELECT
    #define McuShellUart_CONFIG_UART_GET_CLOCK_FREQ_SELECT  kCLOCK_CoreSysClk
  #endif
  #define McuShellUart_CONFIG_UART_IRQ_HANDLER              UART1_RX_TX_IRQHandler
  #define McuShellUART_CONFIG_CLEAR_STATUS_FLAGS            UART_ClearStatusFlags
  #define McuShellUART_CONFIG_CLEAR_EXTRA_STATUS_FLAGS      (kUART_RxOverrunFlag|kUART_RxFifoOverflowFlag)
  #define McuShellUart_CONFIG_HAS_FIFO                      (1)
  
  #ifndef McuShellUart_CONFIG_DO_CONFIGURE_CLOCKS
    #define McuShellUart_CONFIG_DO_CONFIGURE_CLOCKS            (0) 
      /*!< if we configure the clocks for the UART or not */
  #endif
#elif McuShellUart_CONFIG_UART==McuShellUart_CONFIG_UART_K22FN512_UART2_D2_D3
  #include "fsl_uart.h"
  #include "fsl_port.h"
  #define McuShellUart_CONFIG_UART_DEVICE                   UART2
  #define McuShellUart_CONFIG_UART_SET_UART_CLOCK()         /* nothing needed */
  #define McuShellUart_CONFIG_UART_WRITE_BLOCKING           UART_WriteBlocking
  #define McuShellUart_CONFIG_UART_GET_FLAGS                UART_GetStatusFlags
  #define McuShellUart_CONFIG_UART_HW_RX_READY_FLAGS        (kUART_RxDataRegFullFlag|kUART_RxOverrunFlag)
  #define McuShellUart_CONFIG_UART_READ_BYTE                UART_ReadByte
  #define McuShellUart_CONFIG_UART_CONFIG_STRUCT            uart_config_t
  #define McuShellUart_CONFIG_UART_GET_DEFAULT_CONFIG       UART_GetDefaultConfig
  #define McuShellUart_CONFIG_UART_ENABLE_INTERRUPTS        UART_EnableInterrupts
  #define McuShellUart_CONFIG_UART_ENABLE_INTERRUPT_FLAGS   (kUART_RxDataRegFullInterruptEnable | kUART_RxOverrunInterruptEnable)
  #define McuShellUart_CONFIG_UART_IRQ_NUMBER               UART2_RX_TX_IRQn
  #define McuShellUart_CONFIG_UART_INIT                     UART_Init
  #ifndef McuShellUart_CONFIG_UART_GET_CLOCK_FREQ_SELECT
    #define McuShellUart_CONFIG_UART_GET_CLOCK_FREQ_SELECT  kCLOCK_BusClk
  #endif
  #define McuShellUart_CONFIG_UART_IRQ_HANDLER              UART2_RX_TX_IRQHandler
  #define McuShellUART_CONFIG_CLEAR_STATUS_FLAGS            UART_ClearStatusFlags
  #define McuShellUART_CONFIG_CLEAR_EXTRA_STATUS_FLAGS      (kUART_RxOverrunFlag|kUART_RxFifoOverflowFlag)
  #define McuShellUart_CONFIG_HAS_FIFO                      (1)
  
  #ifndef McuShellUart_CONFIG_DO_CONFIGURE_CLOCKS
    #define McuShellUart_CONFIG_DO_CONFIGURE_CLOCKS            (0) 
      /*!< if we configure the clocks for the UART or not */
  #endif
#elif McuShellUart_CONFIG_UART==McuShellUart_CONFIG_UART_K22FN512_LPUART0_C3_C4
  /* LPUART on K22FN512 */
  #include "fsl_lpuart.h"
  #include "fsl_port.h"
  #define McuShellUart_CONFIG_UART_DEVICE                   LPUART0
/*! LPUARTSRC - LPUART clock source select
 *  0b00..Clock disabled
 *  0b01..MCGFLLCLK , or MCGPLLCLK , or IRC48M clock as selected by SOPT2[PLLFLLSEL].
 *  0b10..OSCERCLK clock
 *  0b11..MCGIRCLK clock
 */
  #define SIM_LPUART_CLK_SEL_PLLFLLSEL_CLK                  1U  /*!< LPUART clock select: PLLFLLSEL output clock */
  #define McuShellUart_CONFIG_UART_SET_UART_CLOCK()         CLOCK_SetLpuartClock(SIM_LPUART_CLK_SEL_PLLFLLSEL_CLK)
  #define McuShellUart_CONFIG_UART_WRITE_BLOCKING           LPUART_WriteBlocking
  #define McuShellUart_CONFIG_UART_GET_FLAGS                LPUART_GetStatusFlags
  #define McuShellUart_CONFIG_UART_HW_RX_READY_FLAGS        (kLPUART_RxDataRegFullFlag|kLPUART_RxOverrunFlag)
  #define McuShellUart_CONFIG_UART_READ_BYTE                LPUART_ReadByte
  #define McuShellUart_CONFIG_UART_CONFIG_STRUCT            lpuart_config_t
  #define McuShellUart_CONFIG_UART_GET_DEFAULT_CONFIG       LPUART_GetDefaultConfig
  #define McuShellUart_CONFIG_UART_ENABLE_INTERRUPTS        LPUART_EnableInterrupts
  #define McuShellUart_CONFIG_UART_ENABLE_INTERRUPT_FLAGS   (kLPUART_RxDataRegFullInterruptEnable)
  #define McuShellUart_CONFIG_UART_IRQ_NUMBER               LPUART0_IRQn
  #define McuShellUart_CONFIG_UART_INIT                     LPUART_Init
  #ifndef McuShellUart_CONFIG_UART_GET_CLOCK_FREQ_SELECT
    #define McuShellUart_CONFIG_UART_GET_CLOCK_FREQ_SELECT  /* kCLOCK_Osc0ErClkUndiv */ kCLOCK_PllFllSelClk /* has to match Clocks setting! */
  #endif
  #define McuShellUart_CONFIG_UART_IRQ_HANDLER              LPUART0_IRQHandler
  #define McuShellUART_CONFIG_CLEAR_STATUS_FLAGS            LPUART_ClearStatusFlags
  #define McuShellUART_CONFIG_CLEAR_EXTRA_STATUS_FLAGS      (0) /* no extra flags to clear */
  #define McuShellUart_CONFIG_HAS_FIFO                      (0)
  
  #ifndef McuShellUart_CONFIG_DO_CONFIGURE_CLOCKS
    #define McuShellUart_CONFIG_DO_CONFIGURE_CLOCKS            (0) 
      /*!< if we configure the clocks for the UART or not */
  #endif
#elif McuShellUart_CONFIG_UART==McuShellUart_CONFIG_UART_K64FN1M_UART0_B16_B17
  #include "fsl_uart.h"
  #include "fsl_port.h"
  #define McuShellUart_CONFIG_UART_DEVICE                   UART0
  #define McuShellUart_CONFIG_UART_SET_UART_CLOCK()         /* nothing needed */
  #define McuShellUart_CONFIG_UART_WRITE_BLOCKING           UART_WriteBlocking
  #define McuShellUart_CONFIG_UART_GET_FLAGS                UART_GetStatusFlags
  #define McuShellUart_CONFIG_UART_HW_RX_READY_FLAGS        (kUART_RxDataRegFullFlag|kUART_RxOverrunFlag)
  #define McuShellUart_CONFIG_UART_READ_BYTE                UART_ReadByte
  #define McuShellUart_CONFIG_UART_CONFIG_STRUCT            uart_config_t
  #define McuShellUart_CONFIG_UART_GET_DEFAULT_CONFIG       UART_GetDefaultConfig
  #define McuShellUart_CONFIG_UART_ENABLE_INTERRUPTS        UART_EnableInterrupts
  #define McuShellUart_CONFIG_UART_ENABLE_INTERRUPT_FLAGS   (kUART_RxDataRegFullInterruptEnable | kUART_RxOverrunInterruptEnable)
  #define McuShellUart_CONFIG_UART_IRQ_NUMBER               UART0_RX_TX_IRQn
  #define McuShellUart_CONFIG_UART_INIT                     UART_Init
  #ifndef McuShellUart_CONFIG_UART_GET_CLOCK_FREQ_SELECT
    #define McuShellUart_CONFIG_UART_GET_CLOCK_FREQ_SELECT  kCLOCK_CoreSysClk
  #endif
  #define McuShellUart_CONFIG_UART_IRQ_HANDLER              UART0_RX_TX_IRQHandler
  #define McuShellUART_CONFIG_CLEAR_STATUS_FLAGS            UART_ClearStatusFlags
  #define McuShellUART_CONFIG_CLEAR_EXTRA_STATUS_FLAGS      (kUART_RxOverrunFlag|kUART_RxFifoOverflowFlag)
  #define McuShellUart_CONFIG_HAS_FIFO                      (1)
  
  #ifndef McuShellUart_CONFIG_DO_CONFIGURE_CLOCKS
    #define McuShellUart_CONFIG_DO_CONFIGURE_CLOCKS            (0) 
      /*!< if we configure the clocks for the UART or not */
  #endif
#elif McuShellUart_CONFIG_UART==McuShellUart_CONFIG_UART_RP2040_UART1_GPIO4_GPIO5
  #define McuShellUart_CONFIG_UART_SET_UART_CLOCK()         /* nothing needed */
  #define McuShellUart_CONFIG_UART_WRITE_BLOCKING           RP_WriteBlocking

  #define McuShellUart_CONFIG_UART_DEVICE   uart1
  #define McuShellUart_CONFIG_DATA_BITS     8
  #define McuShellUart_CONFIG_STOP_BITS     1
  #define McuShellUart_CONFIG_PARITY        UART_PARITY_NONE

  #define McuShellUart_CONFIG_UART_TX_PIN   4
  #define McuShellUart_CONFIG_UART_RX_PIN   5
  
  #ifndef McuShellUart_CONFIG_DO_CONFIGURE_CLOCKS
    #define McuShellUart_CONFIG_DO_CONFIGURE_CLOCKS            (0) 
      /*!< if we configure the clocks for the UART or not */
  #endif
#else
  /* you have to put your configuration here */
#endif

/* set default values */
#ifndef McuShellUart_CONFIG_HAS_FIFO
  #define McuShellUart_CONFIG_HAS_FIFO                      (0)
#endif

#ifndef McuShellUart_CONFIG_DO_CONFIGURE_CLOCKS
  #define McuShellUart_CONFIG_DO_CONFIGURE_CLOCKS           (0) 
    /*!< if we configure the clocks for the UART or not */
#endif

#ifndef McuShellUart_CONFIG_UART_RX_QUEUE_LENGTH
  #define McuShellUart_CONFIG_UART_RX_QUEUE_LENGTH    (McuShell_CONFIG_DEFAULT_SHELL_BUFFER_SIZE)
#endif

#ifndef McuShellUart_CONFIG_UART_BAUDRATE
  #define McuShellUart_CONFIG_UART_BAUDRATE           115200
#endif

#ifndef McuShellUart_CONFIG_USE_FREERTOS
  #define McuShellUart_CONFIG_USE_FREERTOS  (McuLib_CONFIG_SDK_USE_FREERTOS)
    /*!< If FreeRTOS (Queues) shall be used or not */
#endif

#endif /* MCUSHELLUARTCONFIG_H_ */
