/*
 * Copyright (c) 2025-2026, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * \file
 * \brief NTP client module configuration.
 */

#ifndef MCU_NTP_CLIENT_CONFIG_H_
#define MCU_NTP_CLIENT_CONFIG_H_

#ifndef MCU_NTP_CLIENT_CONFIG_ENABLED
  #define MCU_NTP_CLIENT_CONFIG_ENABLED        (0) /*!< 1: enable NTP client module */
#endif

#ifndef MCU_NTP_CLIENT_CONFIG_USE_SHELL
  #define MCU_NTP_CLIENT_CONFIG_USE_SHELL     (1 && MCU_NTP_CLIENT_CONFIG_ENABLED) /*!< 1: enable shell command support */
#endif

#ifndef MCU_NTP_CLIENT_CONFIG_USE_MININI
  #define MCU_NTP_CLIENT_CONFIG_USE_MININI     (1 && MCU_NTP_CLIENT_CONFIG_ENABLED) /*!< 1: store settings with MinINI */
#endif

#ifndef MCU_NTP_CLIENT_CONFIG_MININI_FILE_NAME
  #define MCU_NTP_CLIENT_CONFIG_MININI_FILE_NAME       "settings.ini" /*!< MinINI file name */
#endif

#ifndef MCU_NTP_CLIENT_CONFIG_MININI_SECTION
  #define MCU_NTP_CLIENT_CONFIG_MININI_SECTION               "NTP" /*!< MinINI section name */
#endif

#ifndef MCU_NTP_CLIENT_CONFIG_MININI_KEY_START
  #define MCU_NTP_CLIENT_CONFIG_MININI_KEY_START                "start" /*!< Bool: start NTP after reset */
#endif

#endif /* MCU_NTP_CLIENT_CONFIG_H_ */