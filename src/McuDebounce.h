/*
 * McuDebounce.h
 *
 * Copyright (c) 2019, 2020, Erich Styger
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * \file
 * \brief Button debounce finite-state machine interface.
 */

#ifndef MCUDEBOUNCE_H_
#define MCUDEBOUNCE_H_

#include "McuDebounceconfig.h"
#include <stdint.h>
#include "McuRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  MCUDBMC_STATE_IDLE = 0,       /*!< initial state, not doing anything */
  MCUDBMC_STATE_START,          /*!< starting debouncing, entered after a button press */
  MCUDBMC_STATE_PRESS,          /*!< state after start to send a press notification */
  MCUDBMC_STATE_DEBOUNCE,       /*!< debounce after press */
  MCUDBMC_STATE_PRESSED,        /*!< while pressing */
  MCUDBMC_STATE_LONG_PRESSED,   /*!< enters this state after pressing for the long press time */
  MCUDBMC_STATE_RELEASED,       /*!< buttons have been released */
  MCUDBMC_STATE_END,            /*!< final state used for cleanup */
} McuDbnc_State_e;

typedef enum {
  MCUDBNC_EVENT_PRESSED,                /*!< Event for key(s) pressed */
  MCUDBNC_EVENT_PRESSED_REPEAT,         /*!< Event for key(s) while pressed */
  MCUDBNC_EVENT_LONG_PRESSED,           /*!< Event for key(s) pressed for a long time */
  MCUDBNC_EVENT_LONG_PRESSED_REPEAT,    /*!< Event for key(s) pressed for a long time and repeated */
  MCUDBNC_EVENT_RELEASED,               /*!< Event for key(s) released */
  MCUDBNC_EVENT_LONG_RELEASED,          /*!< Event for key(s) released after pressed a long time */
  MCUDBNC_EVENT_END                     /*!< Debouncing end event. This one is called when the FSM finishes. */
} McuDbnc_EventKinds;

typedef struct {
  McuDbnc_State_e state;        /*!< Runtime FSM state */
  /* uint32_t flags; */
  uint32_t timerPeriodMs;       /*!< Configuration: timer period in ms */
  #if McuLib_CONFIG_SDK_USE_FREERTOS && configUSE_TIMERS
  TimerHandle_t timer;          /*!< Configuration: RTOS timer handle */
  #endif
  uint32_t scanValue;           /*!< Runtime: button value captured at debounce start */
  uint32_t countTimeMs;         /*!< Runtime: elapsed time in ms since debounce start */
  uint32_t lastEventTimeMs;     /*!< Runtime: timestamp of the last emitted event */
  uint32_t debounceTimeMs;      /*!< Configuration: debounce time in ms */
  uint32_t repeatTimeMs;        /*!< Configuration: repeat interval in ms */
  uint32_t longKeyTimeMs;       /*!< Configuration: long-press threshold in ms */
  uint32_t (*getButtons)(void); /*!< Configuration: callback to read current button bitmask */
  void (*onDebounceEvent)(McuDbnc_EventKinds event, uint32_t buttons); /*!< Configuration: callback invoked on debounce events */
} McuDbnc_Desc_t;

/*!
 * \brief Processes one debounce FSM step.
 * \param data Pointer to debounce descriptor and runtime data.
 */
void McuDbnc_Process(McuDbnc_Desc_t *data);

/*!
 * \brief Deinitializes the debounce module.
 */
void McuDbnc_Deinit(void);
/*!
 * \brief Initializes the debounce module.
 */
void McuDbnc_Init(void);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* MCUDEBOUNCE_H_ */
