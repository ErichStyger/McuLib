/*
 * Copyright (c) 2020-2026 Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * \file
 * \brief WiFi connection management interface.
 */

#ifndef MCU_WIFI_H_
#define MCU_WIFI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "McuWiFi_config.h"

#if MCU_WIFI_CONFIG_USE_SHELL
  #include "McuShell.h"

  /*!
   * \brief Command line and shell handler
   * \param cmd The command to be parsed
   * \param handled If command has been recognized and handled
   * \param io I/O handler to be used
   * \return error code, otherwise ERR_OK
   */
  uint8_t McuWiFi_ParseCommand(const unsigned char* cmd, bool *handled, const McuShell_StdIOType *io);
#endif

typedef enum {
  McuWiFi_EAP_PEAP = EAP_PEAP, /*!< WPA2 Enterprise with password and no certificate */
  McuWiFi_EAP_TTLS = EAP_TTLS, /*!< TLS method with SSID and password */
} McuWiFi_EAP_e;

typedef struct McuWiFi_Autentification_t {
  McuWiFi_EAP_e type; /*!< Authentication type */
  unsigned char ssid[32]; /*!< SSID of access point */
  unsigned char pass[64]; /*!< Password for access point */
#if CONFIG_WIFI_EAP_METHOD==EAP_PEAP
  unsigned char id[32]; /*!< Additional identity/user name for enterprise login */
#endif
  unsigned char hostname[32]; /*!< Host name to use */
} McuWiFi_Autentification_t;

/*!
 * \brief Callback which can set custom authentification settings
 * \param customConfig Pointer to data to configure
 */
void McuWiFi_SetCustomConfigCallback(void (*customConfig)(McuWiFi_Autentification_t *));

/*!
 * \brief Decides if we are connected or not
 * \return true if we are connected to WiFi, false otherwise
 */
bool McuWiFi_isConnected(void);

/*!
 * \brief Decides if reconnect data is available
 * \return true if WiFi can reconnect with already configured credentials
 */
bool McuWiFi_canReconnect(void);

/*!
 * \brief return the host name
 * \return Host name string.
 */
const char *McuWifi_GetHostName(void);

/*!
 * \brief Module de-initialization
 */
void McuWiFi_Deinit(void);

/*!
 * \brief Module initialization
 */
void McuWiFi_Init(void);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* MCU_WIFI_H_ */
