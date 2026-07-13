/*
 * Copyright (c) 2021-2026, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * \file
 * \brief Shell interface for the UDP client module.
 */

#ifndef MCU_UDP_CLIENT_SHELL_H_
#define MCU_UDP_CLIENT_SHELL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "McuUdpClient.h"

#if MCU_UDP_CLIENT_CONFIG_USE_SHELL
  #include "McuShell.h"

  /*!
   * \brief Command line and shell handler
   * \param cmd The command to be parsed
   * \param handled If command has been recognized and handled
   * \param io I/O handler to be used
   * \return error code, otherwise ERR_OK
   */
  uint8_t McuUdpClient_ParseCommand(const unsigned char* cmd, bool *handled, const McuShell_StdIOType *io);
#endif /* MCU_UDP_CLIENT_CONFIG_USE_SHELL */

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* MCU_UDP_CLIENT_SHELL_H_ */
