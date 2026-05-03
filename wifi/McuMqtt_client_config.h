/*
 * Copyright (c) 2025-2026, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MCU_MQTT_CLIENT_CONFIG_H_
#define MCU_MQTT_CLIENT_CONFIG_H_

#ifndef MCU_MQTT_CLIENT_CONFIG_ENABLED
  #define MCU_MQTT_CLIENT_CONFIG_ENABLED        (0)
    /*!< if module is enabled or not */
#endif

#ifndef MCU_MQTT_CLIENT_CONFIG_USE_SHELL
  #define MCU_MQTT_CLIENT_CONFIG_USE_SHELL     (1 && MCU_MQTT_CLIENT_CONFIG_ENABLED)
    /*!< if module shall implements McuShell support */
#endif

/* default entries for the MQTT broker connection: */
#ifndef MCU_MQTT_CLIENT_CONFIG_BROKER
  #define MCU_MQTT_CLIENT_CONFIG_BROKER   "broker"
    /*!< host name or IP of the MQTT broker */
#endif

#ifndef MCU_MQTT_CLIENT_CONFIG_CLIENT
  #define MCU_MQTT_CLIENT_CONFIG_CLIENT   "client"
    /*!< MQTT client ID: use a different client ID for each connection */
#endif

#ifndef MCU_MQTT_CLIENT_CONFIG_USER
  #define MCU_MQTT_CLIENT_CONFIG_USER     "user"
    /*!< user name for the MQTT connection */
#endif

#ifndef MCU_MQTT_CLIENT_CONFIG_PASS
  #define MCU_MQTT_CLIENT_CONFIG_PASS     "password"
    /*!< password for the MQTT connection */
#endif

#ifndef MCU_MQTT_CLIENT_CONFIG_PUBLISH
  #define MCU_MQTT_CLIENT_CONFIG_PUBLISH  (1)
    /*!< if set to 1, the client will publish messages */
#endif

#ifndef MCU_MQTT_CLIENT_CONFIG_RECONNECT
  #define MCU_MQTT_CLIENT_CONFIG_RECONNECT  (0)
    /*!< if set to 1, the client will automatically reconnect if disconnected */
#endif

#ifndef MCU_MQTT_CLIENT_CONFIG_EXTRA_LOGS
  #define MCU_MQTT_CLIENT_CONFIG_EXTRA_LOGS   (0)
    /*!< set to 1 to produce extra log output */
#endif

#ifndef MCU_MQTT_CLIENT_CONFIG_HEADER_FILE
  #define MCU_MQTT_CLIENT_CONFIG_HEADER_FILE            "my_header.h"
    /*!< header file to include for the MQTT client configuration */
#endif 

#ifndef MCU_MQTT_CLIENT_INCOMING_PUBLISH_CALLBACK
  #define MCU_MQTT_CLIENT_INCOMING_PUBLISH_CALLBACK     My_incoming_publish_cb
    /*! callback for incoming MQTT messages */
#endif

#ifndef MCU_MQTT_CLIENT_INCOMING_DATA_CALLBACK
  #define MCU_MQTT_CLIENT_INCOMING_DATA_CALLBACK        My_incoming_data_cb
    /*! callback for each fragment of payload that arrives */
#endif

#ifndef MCU_MQTT_CLIENT_CONNECTION_CALLBACK
  #define MCU_MQTT_CLIENT_CONNECTION_CALLBACK           My_connection_cb
    /*! callback for connection events */
#endif

#ifndef MCU_MQTT_CLIENT_CONFIG_MININI_FILE_NAME
  #define MCU_MQTT_CLIENT_CONFIG_MININI_FILE_NAME       "settings.ini" 
    /*!< 'file' name used */
#endif

#ifndef MCU_MQTT_CLIENT_MININI_SECTION_MQTT
  #define MCU_MQTT_CLIENT_MININI_SECTION_MQTT              "MQTT"
#endif

#ifndef MCU_MQTT_CLIENT_MININI_KEY_MQTT_BROKER
  #define MCU_MQTT_CLIENT_MININI_KEY_MQTT_BROKER              "broker"     /* string, broker name or IP */
#endif

#ifndef MCU_MQTT_CLIENT_MININI_KEY_MQTT_CLIENT
  #define MCU_MQTT_CLIENT_MININI_KEY_MQTT_CLIENT              "client"     /* string, client ID */
#endif

#ifndef MCU_MQTT_CLIENT_MININI_KEY_MQTT_USER
  #define MCU_MQTT_CLIENT_MININI_KEY_MQTT_USER                "user"       /* string, username */
#endif

#ifndef MCU_MQTT_CLIENT_MININI_KEY_MQTT_PASS
  #define MCU_MQTT_CLIENT_MININI_KEY_MQTT_PASS                "pass"       /* string, password */
#endif

#ifndef MCU_MQTT_CLIENT_MININI_KEY_MQTT_PUBLISH
  #define MCU_MQTT_CLIENT_MININI_KEY_MQTT_PUBLISH             "publish"    /* bool, if publishing */
#endif

#ifndef MCU_MQTT_CLIENT_MININI_KEY_MQTT_RECONNECT
  #define MCU_MQTT_CLIENT_MININI_KEY_MQTT_RECONNECT           "reconnect"  /* bool, if reconnecting */
#endif

#endif /* MCU_MQTT_CLIENT_CONFIG_H_ */
