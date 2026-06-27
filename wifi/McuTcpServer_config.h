/*
 * Copyright (c) 2025-2026, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MCU_TCP_SERVER_CONFIG_H_
#define MCU_TCP_SERVER_CONFIG_H_

#ifndef MCU_TCP_SERVER_CONFIG_ENABLED
  #define MCU_TCP_SERVER_CONFIG_ENABLED        (0)
    /*!< if module is enabled or not */
#endif

#ifndef MCU_TCP_SERVER_CONFIG_USE_SHELL
  #define MCU_TCP_SERVER_CONFIG_USE_SHELL     (1 && MCU_TCP_SERVER_CONFIG_ENABLED)
    /*!< if module implements McuShell support */
#endif

#endif /* MCU_TCP_SERVER_CONFIG_H_ */