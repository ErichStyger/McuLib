/*
 * Copyright (c) 2021, Erich Styger
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * \file
 * \brief Interface for McuESP32 module.
 */

#ifndef McuESP32_H_
#define McuESP32_H_

#include <stdint.h>
#include "McuESP32config.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "McuShell.h"
/*!
 * \brief Shell parser routine.
 * \param cmd Pointer to command line string.
 * \param handled Pointer to status if command has been handled. Set to TRUE if command was understood.
 * \param io Pointer to stdio handle
 * \return Error code, ERR_OK if everything was ok.
 */
uint8_t McuESP32_ParseCommand(const unsigned char *cmd, bool *handled, const McuShell_StdIOType *io);

/*!
 * \brief Set an optional standard I/O used for forwarding ESP Rx data
 * \param stdio Standard I/O to be used
 */
void McuESP32_SetRxFromESPStdio(McuShell_ConstStdIOTypePtr stdio);

/*!
 * \brief Return the current standard I/O assigned for forwarding ESP Rx data
 * \return I/O handler
 */
McuShell_ConstStdIOTypePtr McuESP32_GetRxFromESPStdio(void);

/*!
 * \brief Returns the standard I/O handler for sending data (only sending) to the ESP32
 * \return standard I/O handler
 */
McuShell_ConstStdIOTypePtr McuESP32_GetTxToESPStdio(void);

/*!
 * \brief Set the callback which decides if USB CDC is connected or not.
 * \param callback Function pointer to callback
 */
void McuESP32_SetUsbCdcIsConnectedCallback(bool (*callback)(void));

/*!
 * \brief Set optional callback to be called at start end end of programming, useful to silence high system load activities.
 * \param callback Pointer to callback function.
 */
void McuESP32_SetProgrammingCallback(void (*callback)(bool isProgramming));

/*!
 * \brief Used to deal with USB CDC flow control pins.
 * \param dtr State of DTR signal.
 * \param rts State of RTS signal.
 */
void McuESP32_UartStateCallback(bool dtr, bool rts);

/*!
 * \brief Callback to change the UART baud rate.
 * \param baud New baud or bitrate.
 */
void McuESP32_ChangeUartBaudCallback(uint32_t baud);

/*!
 * \brief Sets the USB flush callback, required for idf.py flash operations.
 * \param callback Callback to be called for a flush operation.
 */
void McuESP32_SetUsbFlushCallback(void (*callback)(void));

/*!
 * \brief Set the standard I/O handler to be used for sending ESP32 Rx traffic to.
 * \param stdio Standard I/O handler to be used.
 */
void McuESP32_SetUsbCdcStdio(McuShell_ConstStdIOTypePtr stdio);

/*!
 * \brief Function to send data to the ESP32 via UART.
 * \param data Pointer to the data to be sent.
 * \param nofBytes number of data bytes
 * \return number of bytes sent.
 */
uint32_t McuESP32_SendTxData(const void *data, uint32_t nofBytes);

/*!
 * \brief Module de-initialization
 */
void McuESP32_Deinit(void);

/*!
 * \brief Module initialization
 */
void McuESP32_Init(void);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* McuESP32_H_ */
