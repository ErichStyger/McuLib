/*
 * Copyright (c) 2021-2025, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MCU_NTP_CLIENT_H_
#define MCU_NTP_CLIENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "McuNtp_client_config.h"
#if MCU_NTP_CLIENT_CONFIG_ENABLED

#include <stdbool.h>

#if MCU_NTP_CLIENT_CONFIG_USE_SHELL
  #include "McuShell.h"

  /*!
   * \brief Command line and shell handler
   * \param cmd The command to be parsed
   * \param handled If command has been recognized and handled
   * \param io I/O handler to be used
   * \return error code, otherwise ERR_OK
   */
  uint8_t McuNtpClient_ParseCommand(const unsigned char* cmd, bool *handled, const McuShell_StdIOType *io);
#endif

/*!
 * \brief Decide if NTP client shall start by default.
 * \return true if it shall be started, false otherwise. 
 */
bool McuNtpClient_GetDefaultStart(void);

/*!
 * \brief Suspend the NTP client task
 */
void McuNtpClient_TaskSuspend(void);

/*!
 * \brief Resume the NTP client task
 */
void McuNtpClient_TaskResume(void);

/*!
 * \brief Module initialization
 */
void McuNtpClient_Init(void);

#endif /* MCU_NTP_CLIENT_CONFIG_ENABLED */

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* MCU_NTP_CLIENT_H_ */
