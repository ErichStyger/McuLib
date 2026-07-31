/*
 * Copyright (c) 2025-2026, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * \file
 * \brief WiFi module configuration.
 */

#ifndef MCU_WIFI_CONFIG_H_
#define MCU_WIFI_CONFIG_H_

#ifndef MCU_WIFI_CONFIG_ENABLED
  #define MCU_WIFI_CONFIG_ENABLED        (0) /*!< 1: enable WiFi module */
#endif

#ifndef MCU_WIFI_CONFIG_USE_SHELL
  #define MCU_WIFI_CONFIG_USE_SHELL     (1 && MCU_WIFI_CONFIG_ENABLED) /*!< 1: enable shell command support */
#endif

#ifndef MCU_WIFI_CONFIG_USE_MININI
  #define MCU_WIFI_CONFIG_USE_MININI     (1 && MCU_WIFI_CONFIG_ENABLED) /*!< 1: use MinINI for settings */
#endif

#ifndef MCU_WIFI_CONFIG_MININI_FILE_NAME
  #define MCU_WIFI_CONFIG_MININI_FILE_NAME       "settings.ini" /*!< MinINI file name */
#endif

#ifndef MCU_WIFI_CONFIG_MININI_SECTION_WIFI
  #define MCU_WIFI_CONFIG_MININI_SECTION_WIFI              "WiFi" /*!< MinINI section name */
#endif

#ifndef MCU_WIFI_CONFIG_MININI_KEY_WIFI_SSID
  #define MCU_WIFI_CONFIG_MININI_KEY_WIFI_SSID                "ssid" /*!< String key: SSID of network */
#endif

#ifndef MCU_WIFI_CONFIG_MININI_KEY_WIFI_PASS
  #define MCU_WIFI_CONFIG_MININI_KEY_WIFI_PASS                "pass" /*!< String key: password */
#endif

#ifndef MCU_WIFI_CONFIG_MININI_KEY_WIFI_HOSTNAME
  #define MCU_WIFI_CONFIG_MININI_KEY_WIFI_HOSTNAME            "hostname" /*!< String key: hostname to use */
#endif

#ifndef MCU_WIFI_CONFIG_MININI_KEY_WIFI_ENABLE
  #define MCU_WIFI_CONFIG_MININI_KEY_WIFI_ENABLE              "enable" /*!< Bool key: WiFi enabled */
#endif

#ifndef MCU_WIFI_CONFIG_MININI_KEY_WIFI_RECONNECT
  #define MCU_WIFI_CONFIG_MININI_KEY_WIFI_RECONNECT           "reconnect" /*!< Bool key: automatic reconnect enabled */
#endif

#ifndef MCU_WIFI_CONFIG_WIFI_DEFAULT_ENABLE
  #define MCU_WIFI_CONFIG_WIFI_DEFAULT_ENABLE       true /*!< Default WiFi enabled state */
#endif

#ifndef MCU_WIFI_CONFIG_WIFI_DEFAULT_RECONNECT
  #define MCU_WIFI_CONFIG_WIFI_DEFAULT_RECONNECT    true /*!< Default auto-reconnect state */
#endif

#define EAP_PEAP 1 /*!< WPA2 Enterprise with password and no certificate */
#define EAP_TTLS 2 /*!< TLS method with SSID and password */

#ifndef CONFIG_WIFI_EAP_METHOD
  #define CONFIG_WIFI_EAP_METHOD    EAP_TTLS /*!< Default EAP method */
#endif

#ifndef CONFIG_WIFI_DEFAULT_HOSTNAME
  #define CONFIG_WIFI_DEFAULT_HOSTNAME        "host" /*!< Default host name */
#endif
#ifndef CONFIG_WIFI_DEFAULT_SSID
  #define CONFIG_WIFI_DEFAULT_SSID            "ssid" /*!< Default SSID */
#endif
#ifndef WIFI_DEFAULT_PASS
  #define CONFIG_WIFI_DEFAULT_PASS            "password" /*!< Default WiFi password */
#endif
#ifndef WIFI_DEFAULT_ID
  #define CONFIG_WIFI_DEFAULT_ID              "id" /*!< Default enterprise identity */
#endif

#ifndef MCU_WIFI_CONFIG_USE_MININI
  #define MCU_WIFI_CONFIG_USE_MININI    (1) /*!< 1: use MinINI for settings */
#endif

#ifndef MCU_WIFI_CONFIG_USE_PING
  #define MCU_WIFI_CONFIG_USE_PING      (0) /*!< 1: enable ping integration */
#endif

#ifndef MCU_WIFI_CONFIG_USE_WATCHDOG
  #define MCU_WIFI_CONFIG_USE_WATCHDOG  (0) /*!< 1: enable watchdog integration */
#endif

#endif /* MCU_WIFI_CONFIG_H_ */