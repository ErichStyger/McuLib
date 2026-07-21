/*
 * Copyright (c) 2024, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * \file
 * \brief USB CDC shell transport interface.
 */

#ifndef McuShellCdcDevice_H_
#define McuShellCdcDevice_H_

#include "McuShellCdcDeviceconfig.h"
#include "McuShell.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! Default McuShell buffer */
extern uint8_t McuShellCdcDevice_DefaultShellBuffer[McuShell_DEFAULT_SHELL_BUFFER_SIZE]; /* default buffer which can be used by the application */

/*! Standard I/O for input/output */
extern McuShell_ConstStdIOType McuShellCdcDevice_stdio;

/*!
 * \brief Returns the stdio interface for the USB CDC shell transport.
 * \return Pointer to stdio descriptor.
 */
McuShell_ConstStdIOTypePtr McuShellCdcDevice_GetStdio(void);

/*!
 * \brief Flushes pending USB CDC output data.
 */
void McuShellCdcDevice_Flush(void);

/*!
 * \brief Writes a buffer and flushes it immediately.
 * \param buf Pointer to bytes to write.
 * \param count Number of bytes to write.
 */
void McuShellCdcDevice_WriteAndFlush(const char *buf, size_t count);

/*!
 * \brief Writes a string to the USB CDC channel
 * \param str Zero terminated string to write
 * \return Number of characters written
 */
uint32_t McuShellCdcDevice_WriteStr(const char *str);

/*!
 * \brief Writes one character to USB CDC output.
 * \param ch Character to write.
 */
void McuShellCdcDevice_WriteChar(char ch);
/*!
 * \brief Reads one character from USB CDC input.
 * \param ch Pointer receiving the character.
 */
void McuShellCdcDevice_ReadChar(char *ch);
/*!
 * \brief Checks if at least one input character is available.
 * \return true if data is available.
 */
bool McuShellCdcDevice_IsDataPresent(void);
/*!
 * \brief Queues one received character into the shell buffer.
 * \param ch Character to enqueue.
 */
void McuShellCdcDevice_QueueChar(char ch);

/*!
 * \brief Writing a string to the USB CDC using printf() style.
 * \return The number of bytes written.
 */
unsigned McuShellCdcDevice_printf(const char *fmt, ...);

/*!
 * \brief Return a received byte.
 * \return The byte, or EOF
 */
int McuShellCdcDevice_ReadByte(void);

/*!
 * \brief Checks if USB CDC device is ready for data transfer.
 * \return true if ready to transfer.
 */
bool McuShellCdcDevice_IsReady(void); /* device is ready to transfer */
/*!
 * \brief Checks USB CDC connection state.
 * \return true if connected (best-effort status).
 */
bool McuShellCdcDevice_IsConnected(void); /* note: connection status might not be accurate */

/*!
 * \brief Set the callback for a character is received from the USB CDC channel.
 * \param buffer_rx_char_cb Callback to be used to store the character. Can be NULL.
 */
void McuShellCdcDevice_SetBufferRxCharCallback(void (*buffer_rx_char_cb)(char ch));

/*!
 * \brief Set callback for RTS/CTS changes
 * \param callback Callback indicating RTS and CTS settings
 */
void McuShellCdcDevice_SetRtsCtsCallback(void (*callback)(bool rts, bool cts));

/*!
 * \brief Parses shell commands handled by this module.
 * \param cmd Command string.
 * \param handled Set to true if command is handled.
 * \param io I/O stream for command output.
 * \return Error code, ERR_OK if command handling was successful.
 */
uint8_t McuShellCdcDevice_ParseCommand(const unsigned char *cmd, bool *handled, const McuShell_StdIOType *io);

/*!
 * \brief USB device processing function, useful for bare metal applications.
 *        Call this function periodically, you might use McuShellCdcDevice_CONFIG_PROCESS_WAIT_TIME_MS.
 */
void McuShellCdcDevice_Process(void);

/*!
 * \brief Module de-initialization
 */
void McuShellCdcDevice_Deinit(void);

/*!
 * \brief Module initialization
 */
void McuShellCdcDevice_Init(void);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* McuShellCdcDevice_H_ */