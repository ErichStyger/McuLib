/*
 * Copyright (c) 2022, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * \file
 * \brief SPI driver interface.
 */

#ifndef MCUSPI_H_
#define MCUSPI_H_

#include "McuSPIconfig.h"
#include <stdint.h>
#include <stddef.h>

/*!
 * \brief Drives the SPI chip-select line low (active).
 */
void McuSPI_SetCS_Low(void);

/*!
 * \brief Drives the SPI chip-select line high (inactive).
 */
void McuSPI_SetCS_High(void);

/*!
 * \brief Sends and receives a block of bytes over SPI.
 * \param txDataBuf Transmit buffer pointer, can be NULL.
 * \param rxDataBuf Receive buffer pointer, can be NULL.
 * \param dataSize Number of bytes to transfer.
 * \return Error/status code from low-level driver.
 */
int McuSPI_SendReceiveBlock(const uint8_t *txDataBuf, uint8_t *rxDataBuf, size_t dataSize);

/*!
 * \brief Sends one byte over SPI.
 * \param ch Byte to send.
 * \return Error/status code from low-level driver.
 */
int McuSPI_SendByte(unsigned char ch);

/*!
 * \brief Sends one byte and receives one byte over SPI.
 * \param ch Byte to send.
 * \param chp Pointer to where to store received byte.
 * \return Error/status code from low-level driver.
 */
int McuSPI_SendReceiveByte(unsigned char ch, unsigned char *chp);

/*!
 * \brief Receives one byte over SPI.
 * \param chp Pointer to where to store received byte.
 * \return Error/status code from low-level driver.
 */
int McuSPI_ReceiveByte(unsigned char *chp);

/*!
 * \brief Sets the SPI baud rate.
 * \param baud Baud rate in Hz.
 * \return Error/status code from low-level driver.
 */
int McuSPI_SetBaudRate(uint32_t baud);

/*!
 * \brief Deinitializes the SPI module.
 */
void McuSPI_Deinit(void);

/*!
 * \brief Initializes the SPI module.
 */
void McuSPI_Init(void);

#endif /* MCUSPI_H_ */
