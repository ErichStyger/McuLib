/*
 * Copyright (c) 2021-2026, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PING_SHELL_H_
#define PING_SHELL_H_

#include "platform.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "McuShell.h"

/*!
  * \brief Command line and shell handler
  * \param cmd The command to be parsed
  * \param handled If command has been recognized and handled
  * \param io I/O handler to be used
  * \return error code, otherwise ERR_OK
  */
uint8_t McuPing_ParseCommand(const unsigned char* cmd, bool *handled, const McuShell_StdIOType *io);

/*!
 * \brief Module initialization
 */
void McuPing_Init(void);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* PING_SHELL_H_ */
