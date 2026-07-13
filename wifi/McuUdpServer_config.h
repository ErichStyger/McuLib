/*
 * Copyright (c) 2025-2026, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * \file
 * \brief UDP server module configuration.
 */

#ifndef MCU_UDP_SERVER_CONFIG_H_
#define MCU_UDP_SERVER_CONFIG_H_

#ifndef MCU_UDP_SERVER_CONFIG_ENABLED
  #define MCU_UDP_SERVER_CONFIG_ENABLED        (0) /*!< 1: enable UDP server module */
#endif

#ifndef MCU_UDP_SERVER_CONFIG_USE_SHELL
  #define MCU_UDP_SERVER_CONFIG_USE_SHELL     (1 && MCU_UDP_SERVER_CONFIG_ENABLED) /*!< 1: enable shell command support */
#endif

#ifndef MCU_UDP_SERVER_CONFIG_PORT
  #define MCU_UDP_SERVER_CONFIG_PORT          (1234) /*!< Default UDP server port */
#endif

#endif /* MCU_UDP_SERVER_CONFIG_H_ */