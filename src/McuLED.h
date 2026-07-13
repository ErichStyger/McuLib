/*
 * Copyright (c) 2019-2021, Erich Styger
 * All rights reserved.
 *
 * Driver for LEDs
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * \file
 * \brief Driver interface for LEDs.
 */

#ifndef McuLED_H_
#define McuLED_H_

#include <stdbool.h>
#include "McuLibconfig.h"
#include "McuLEDconfig.h"
#include "McuGPIO.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct McuLED_Dummy_s { int dummy; } McuLED_Dummy_s; /*!< using a pointer to a struct instead a pointer to void for handle, used for handle type 'safety' only */

typedef McuLED_Dummy_s *McuLED_Handle_t; /*!< LED handle type */

typedef struct {
  bool isLowActive;  /*!< true if LED output is active low */
  bool isOnInit;     /*!< true if LED shall be turned on during initialization */
  McuGPIO_HwPin_t hw;/*!< Hardware pin configuration used by this LED */
} McuLED_Config_t;

/*!
 * \brief Fills a LED configuration structure with default values.
 * \param config Pointer to configuration structure to initialize.
 */
void McuLED_GetDefaultConfig(McuLED_Config_t *config);

/*!
 * \brief Initializes one LED instance.
 * \param config Pointer to LED configuration.
 * \return LED handle, or NULL on error.
 */
McuLED_Handle_t McuLED_InitLed(McuLED_Config_t *config);

/*!
 * \brief Deinitializes one LED instance.
 * \param led LED handle.
 * \return NULL.
 */
McuLED_Handle_t McuLED_DeinitLed(McuLED_Handle_t led);

/*!
 * \brief Turns the LED on.
 * \param led LED handle.
 */
void McuLED_On(McuLED_Handle_t led);
/*!
 * \brief Turns the LED off.
 * \param led LED handle.
 */
void McuLED_Off(McuLED_Handle_t led);
/*!
 * \brief Toggles the LED state.
 * \param led LED handle.
 */
void McuLED_Toggle(McuLED_Handle_t led);
/*!
 * \brief Gets the current LED state.
 * \param led LED handle.
 * \return true if LED is on, false otherwise.
 */
bool McuLED_Get(McuLED_Handle_t led);
/*!
 * \brief Sets the LED state.
 * \param led LED handle.
 * \param isOn true to switch LED on, false to switch it off.
 */
void McuLED_Set(McuLED_Handle_t led, bool isOn);

/*!
 * \brief Returns a textual representation of the LED state.
 * \param led LED handle.
 * \param buf Destination buffer receiving the status string.
 * \param bufSize Size of destination buffer in bytes.
 */
void McuLED_GetLedStatusString(McuLED_Handle_t led, unsigned char *buf, size_t bufSize);

/*!
 * \brief Initializes the LED driver module.
 */
void McuLED_Init(void);

/*!
 * \brief Deinitializes the LED driver module.
 */
void McuLED_Deinit(void);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* McuLED_H_ */
