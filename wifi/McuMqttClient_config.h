/*
 * Copyright (c) 2025-2026, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * \file
 * \brief MQTT client module configuration.
 */

#ifndef MCU_MQTT_CLIENT_CONFIG_H_
#define MCU_MQTT_CLIENT_CONFIG_H_

#ifndef MCU_MQTT_CLIENT_CONFIG_ENABLED
  #define MCU_MQTT_CLIENT_CONFIG_ENABLED        (0) /*!< 1: enable MQTT client module */
#endif

#ifndef MCU_MQTT_CLIENT_CONFIG_USE_SHELL
  #define MCU_MQTT_CLIENT_CONFIG_USE_SHELL     (1 && MCU_MQTT_CLIENT_CONFIG_ENABLED) /*!< 1: enable shell command support */
#endif

#ifndef MCU_MQTT_CLIENT_CONFIG_BROKER
  #define MCU_MQTT_CLIENT_CONFIG_BROKER   "broker" /*!< Host name or IP of the MQTT broker */
#endif

#ifndef MCU_MQTT_CLIENT_CONFIG_CLIENT
  #define MCU_MQTT_CLIENT_CONFIG_CLIENT   "client" /*!< MQTT client ID */
#endif

#ifndef MCU_MQTT_CLIENT_CONFIG_USER
  #define MCU_MQTT_CLIENT_CONFIG_USER     "user" /*!< User name for MQTT connection */
#endif

#ifndef MCU_MQTT_CLIENT_CONFIG_PASS
  #define MCU_MQTT_CLIENT_CONFIG_PASS     "password" /*!< Password for MQTT connection */
#endif

#ifndef MCU_MQTT_CLIENT_CONFIG_PUBLISH
  #define MCU_MQTT_CLIENT_CONFIG_PUBLISH  (0) /*!< 1: client publishes messages */
#endif

#ifndef MCU_MQTT_CLIENT_CONFIG_RECONNECT
  #define MCU_MQTT_CLIENT_CONFIG_RECONNECT  (0) /*!< 1: client automatically reconnects */
#endif

#ifndef MCU_MQTT_CLIENT_CONFIG_EXTRA_LOGS
  #define MCU_MQTT_CLIENT_CONFIG_EXTRA_LOGS   (0) /*!< 1: enable extra log output */
#endif

#ifndef MCU_MQTT_CLIENT_CONFIG_USE_MININI
  #define MCU_MQTT_CLIENT_CONFIG_USE_MININI      (1) /*!< 1: use MinINI for storing settings */
#endif

#ifndef MCU_MQTT_CLIENT_CONFIG_MININI_FILE_NAME
  #define MCU_MQTT_CLIENT_CONFIG_MININI_FILE_NAME       "settings.ini" /*!< MinINI file name */
#endif

#ifndef MCU_MQTT_CLIENT_MININI_SECTION_MQTT
  #define MCU_MQTT_CLIENT_MININI_SECTION_MQTT              "MQTT" /*!< MinINI section name */
#endif

#ifndef MCU_MQTT_CLIENT_MININI_KEY_MQTT_BROKER
  #define MCU_MQTT_CLIENT_MININI_KEY_MQTT_BROKER              "broker" /*!< String key: broker name or IP */
#endif

#ifndef MCU_MQTT_CLIENT_MININI_KEY_MQTT_CLIENT
  #define MCU_MQTT_CLIENT_MININI_KEY_MQTT_CLIENT              "client" /*!< String key: client ID */
#endif

#ifndef MCU_MQTT_CLIENT_MININI_KEY_MQTT_USER
  #define MCU_MQTT_CLIENT_MININI_KEY_MQTT_USER                "user" /*!< String key: username */
#endif

#ifndef MCU_MQTT_CLIENT_MININI_KEY_MQTT_PASS
  #define MCU_MQTT_CLIENT_MININI_KEY_MQTT_PASS                "pass" /*!< String key: password */
#endif

#ifndef MCU_MQTT_CLIENT_MININI_KEY_MQTT_PUBLISH
  #define MCU_MQTT_CLIENT_MININI_KEY_MQTT_PUBLISH             "publish" /*!< Bool key: publishing enabled */
#endif

#ifndef MCU_MQTT_CLIENT_MININI_KEY_MQTT_RECONNECT
  #define MCU_MQTT_CLIENT_MININI_KEY_MQTT_RECONNECT           "reconnect" /*!< Bool key: reconnect enabled */
#endif

#endif /* MCU_MQTT_CLIENT_CONFIG_H_ */
