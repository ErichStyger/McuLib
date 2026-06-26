/*
 * Copyright (c) 2022-2026, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MCU_PICO_WIFI_H_
#define MCU_PICO_WIFI_H_

#include "McuPicoWiFi_config.h"
#include <stdbool.h>

#if MCU_PICO_WIFI_CONFIG_ENABLED
/*!
 * \brief Sets the initialized status of the CYW43 architecture
 * \param isInitialized If initialized or not
 */
void McuPicoWiFi_SetArchIsInitialized(bool isInitialized);

/*!
 * \brief Decides if the CY43 architecture and subsystem is initialized or not.
 * \return true if subsystem is initialized, false otherwise.
 */
bool McuPicoWiFi_GetArchIsInitialized(void);

/*!
 * \brief Returns the WiFi status as string
 * \param linkStatus Linkstatus ID
 * \return String for the link status
 */
const unsigned char *McuPicoWiFi_GetTcpIpLinkStatusString(int linkStatus);

/*!
 * \brief Module de-initialization
 */
void McuPicoWiFi_Deinit(void);

/*!
 * \brief Module initialization
 */
void McuPicoWiFi_Init(void);

#endif /* MCU_PICO_WIFI_CONFIG_ENABLED */

#endif /* MCU_PICO_WIFI_H_ */
