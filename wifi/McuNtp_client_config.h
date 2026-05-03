/*
 * Copyright (c) 2025-2026, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MCU_NTP_CLIENT_CONFIG_H_
#define MCU_NTP_CLIENT_CONFIG_H_

#ifndef MCU_NTP_CLIENT_CONFIG_ENABLED
  #define MCU_NTP_CLIENT_CONFIG_ENABLED        (0)
    /*!< if module is enabled or not */
#endif

#ifndef MCU_NTP_CLIENT_CONFIG_USE_SHELL
  #define MCU_NTP_CLIENT_CONFIG_USE_SHELL     (1 && MCU_NTP_CLIENT_CONFIG_ENABLED)
    /*!< if module shall implements McuShell support */
#endif

#ifndef MCU_NTP_CLIENT_CONFIG_USE_MININI
  #define MCU_NTP_CLIENT_CONFIG_USE_MININI     (1 && MCU_NTP_CLIENT_CONFIG_ENABLED)
    /*!< if module uses MinINI for settings */
#endif

#ifndef MCU_NTP_CLIENT_CONFIG_MININI_FILE_NAME
  #define MCU_NTP_CLIENT_CONFIG_MININI_FILE_NAME       "settings.ini" 
    /*!< 'file' name used */
#endif

#ifndef MCU_NTP_CLIENT_CONFIG_MININI_SECTION
  #define MCU_NTP_CLIENT_CONFIG_MININI_SECTION               "NTP"
    /*!< section name */
#endif

#ifndef MCU_NTP_CLIENT_CONFIG_MININI_KEY_START
  #define MCU_NTP_CLIENT_CONFIG_MININI_KEY_START                "start"
    /*!< bool, if start ntp or not after reset */
#endif

#endif /* MCU_NTP_CLIENT_CONFIG_H_ */