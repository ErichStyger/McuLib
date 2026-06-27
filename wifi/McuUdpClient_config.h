/*
 * Copyright (c) 2025-2026, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MCU_UDP_CLIENT_CONFIG_H_
#define MCU_UDP_CLIENT_CONFIG_H_

#ifndef MCU_UDP_CLIENT_CONFIG_ENABLED
  #define MCU_UDP_CLIENT_CONFIG_ENABLED        (0)
    /*!< if module is enabled or not */
#endif

#ifndef MCU_UDP_CLIENT_CONFIG_USE_SHELL
  #define MCU_UDP_CLIENT_CONFIG_USE_SHELL     (1 && MCU_UDP_CLIENT_CONFIG_ENABLED)
    /*!< if module implements McuShell support */
#endif

#ifndef MCU_UDP_CLIENT_CONFIG_DEFAULT_SERVER
  #define MCU_UDP_CLIENT_CONFIG_DEFAULT_SERVER     "udp_server"
    /*!< default UDP host for UDB client */
#endif

#ifndef MCU_UDP_CLIENT_CONFIG_DEFAULT_SERVER_PORT
  #define MCU_UDP_CLIENT_CONFIG_DEFAULT_SERVER_PORT     1234
    /*!< default UDP host port number */
#endif


#endif /* MCU_UDP_SERVER_CONFIG_H_ */