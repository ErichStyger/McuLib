/*!
 * Copyright (c) 2020-2024, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * \file
 * \brief Interface for the McuShellUart module.
 */

#ifndef MCUSHELLUART_H_
#define MCUSHELLUART_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "McuShellUartconfig.h"
#if McuShellUart_CONFIG_IS_ENABLED

#include "McuShell.h"

/*! Default McuShell buffer */
extern uint8_t McuShellUart_DefaultShellBuffer[McuShell_DEFAULT_SHELL_BUFFER_SIZE]; /* default buffer which can be used by the application */

/*! Standard I/O for input/output */
extern McuShell_ConstStdIOType McuShellUart_stdio;

/*!
 * \brief Returns the stdio interface for the UART shell transport.
 * \return Pointer to stdio descriptor.
 */
McuShell_ConstStdIOTypePtr McuShellUart_GetStdio(void);

/*!
 * \brief Writes a number of data bytes to the UART.
 * \return The number of bytes written.
 */
int McuShellUart_WriteBytes(const void *src, size_t size);

/*!
 * \brief Writes a string to the UART.
 * \return The number of character written.
 */
int McuShellUart_WriteString(const char *str);

/*!
 * \brief Polls for a UART character, might be blocking
 * \return The character or EOF
 */
int McuShellUart_PollChar(void);

/*!
 * \brief perform muxing of the UART pins
 * \param uart One of the preconfigured and supported UART types, e.g. McuShellUart_CONFIG_UART_K22FN512_LPUART0_C3_C4
 */
void McuShellUart_MuxUartPins(int uart);

/*!
 * \brief Module de-initialization
 */
void McuShellUart_Deinit(void);

/*!
 * \brief Module initialization
 */
void McuShellUart_Init(void);

#endif /* McuShellUart_CONFIG_IS_ENABLED */

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* MCUSHELLUART_H_ */
