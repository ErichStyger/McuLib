/*
 * Copyright (c) 2025-2026, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * \file
 * \brief UDP client module configuration.
 */

#ifndef MCU_UDP_CLIENT_CONFIG_H_
#define MCU_UDP_CLIENT_CONFIG_H_

#ifndef MCU_UDP_CLIENT_CONFIG_ENABLED
  #define MCU_UDP_CLIENT_CONFIG_ENABLED        (0) /*!< 1: enable UDP client module */
#endif

#ifndef MCU_UDP_CLIENT_CONFIG_USE_SHELL
  #define MCU_UDP_CLIENT_CONFIG_USE_SHELL     (1 && MCU_UDP_CLIENT_CONFIG_ENABLED) /*!< 1: enable shell command support */
#endif

#ifndef MCU_UDP_CLIENT_CONFIG_DEFAULT_SERVER
  #define MCU_UDP_CLIENT_CONFIG_DEFAULT_SERVER     "udp_server" /*!< Default UDP host for client */
#endif

#ifndef MCU_UDP_CLIENT_CONFIG_DEFAULT_SERVER_PORT
  #define MCU_UDP_CLIENT_CONFIG_DEFAULT_SERVER_PORT     1234 /*!< Default UDP host port number */
#endif

#endif /* MCU_UDP_CLIENT_CONFIG_H_ */