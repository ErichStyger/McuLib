/*
 * Copyright (c) 2023, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MCU_TCP_SERVER_H_
#define MCU_TCP_SERVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "McuTcpServer_config.h"
#if MCU_TCP_SERVER_CONFIG_ENABLED

#if MCU_TCP_SERVER_CONFIG_USE_SHELL
  #include "McuShell.h"

  /*!
   * \brief Command line and shell handler
   * \param cmd The command to be parsed
   * \param handled If command has been recognized and handled
   * \param io I/O handler to be used
   * \return error code, otherwise ERR_OK
   */
  uint8_t McuTcpServer_ParseCommand(const unsigned char* cmd, bool *handled, const McuShell_StdIOType *io);
#endif

void McuTcpServer_Suspend(void);

void McuTcpServer_Resume(void);

void McuTcpServer_Init(void);

#endif /* MCU_TCP_SERVER_CONFIG_ENABLED */

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* MCU_TCP_SERVER_H_ */
