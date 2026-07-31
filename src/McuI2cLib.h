/*
 * Copyright (c) 2019-2022, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * \file
 * \brief I2C helper library interface.
 */

#ifndef MCUI2CLIB_H_
#define MCUI2CLIB_H_

#include <stdint.h>
#include "McuI2cLibconfig.h"

typedef enum McuI2cLib_EnumStartFlags_ {
  MCUI2CLIB_SEND_START,        /*!< Start condition is sent */
  MCUI2CLIB_DO_NOT_SEND_START  /*!< Start condition is not sent */
} McuI2cLib_EnumStartFlags;

typedef enum I2CLIB_EnumAckFlags_ {
  MCUI2CLIB_SEND_LAST_ACK,   /*!< ACK/NACK sequence is sent after last received byte */
  MCUI2CLIB_DO_NOT_LAST_ACK  /*!< ACK/NACK sequence is not sent after last received byte */
} McuI2cLib_EnumAckFlags;

/*!
 * \brief Receives a data block with configurable start/ack behavior.
 * \param Ptr Pointer to destination buffer.
 * \param Siz Number of bytes to receive.
 * \param Rcv Pointer receiving number of bytes received.
 * \param flagsStart Start condition handling.
 * \param flagsAck ACK handling for last byte.
 * \return Error code, ERR_OK if successful.
 */
uint8_t McuI2cLib_RecvBlockCustom(void *Ptr, uint16_t Siz, uint16_t *Rcv, McuI2cLib_EnumStartFlags flagsStart, McuI2cLib_EnumAckFlags flagsAck);

/*!
 * \brief Receives a data block.
 * \param Ptr Pointer to destination buffer.
 * \param Siz Number of bytes to receive.
 * \param Rcv Pointer receiving number of bytes received.
 * \return Error code, ERR_OK if successful.
 */
uint8_t McuI2cLib_RecvBlock(void *Ptr, uint16_t Siz, uint16_t *Rcv);
/*!
 * \brief Sends a data block.
 * \param Ptr Pointer to source buffer.
 * \param Siz Number of bytes to send.
 * \param Snt Pointer receiving number of bytes sent.
 * \return Error code, ERR_OK if successful.
 */
uint8_t McuI2cLib_SendBlock(void *Ptr, uint16_t Siz, uint16_t *Snt);
/*!
 * \brief Sends a data block continuing an active transfer.
 * \param Ptr Pointer to source buffer.
 * \param Siz Number of bytes to send.
 * \param Snt Pointer receiving number of bytes sent.
 * \return Error code, ERR_OK if successful.
 */
uint8_t McuI2cLib_SendBlockContinue(void *Ptr, uint16_t Siz, uint16_t *Snt);
/*!
 * \brief Sends an I2C STOP condition.
 * \return Error code, ERR_OK if successful.
 */
uint8_t McuI2cLib_SendStop(void);
/*!
 * \brief Selects a slave address for subsequent transfers.
 * \param Slv 7-bit/8-bit slave address (driver dependent).
 * \return Error code, ERR_OK if successful.
 */
uint8_t McuI2cLib_SelectSlave(uint8_t Slv);

/*!
 * \brief Reads bytes from a device memory/register address.
 * \param i2cAddr Device I2C address.
 * \param memAddr Pointer to memory/register address bytes.
 * \param memAddrSize Number of address bytes.
 * \param data Destination buffer.
 * \param dataSize Number of bytes to read.
 * \return Error code, ERR_OK if successful.
 */
uint8_t McuI2cLib_ReadAddress(uint8_t i2cAddr, uint8_t *memAddr, uint8_t memAddrSize, uint8_t *data, uint16_t dataSize);
/*!
 * \brief Writes bytes to a device memory/register address.
 * \param i2cAddr Device I2C address.
 * \param memAddr Pointer to memory/register address bytes.
 * \param memAddrSize Number of address bytes.
 * \param data Source buffer.
 * \param dataSize Number of bytes to write.
 * \return Error code, ERR_OK if successful.
 */
uint8_t McuI2cLib_WriteAddress(uint8_t i2cAddr, uint8_t *memAddr, uint8_t memAddrSize, uint8_t *data, uint16_t dataSize);

#if MCUI2CLIB_CONFIG_I2C_RELEASE_BUS
/*!
 * \brief Attempts to recover and reset a stuck I2C bus.
 * \return true if bus recovery succeeded.
 */
bool McuI2cLib_ResetBus(void);
#endif

/*!
 * \brief Deinitializes the I2C helper module.
 */
void McuI2cLib_Deinit(void);
/*!
 * \brief Initializes the I2C helper module.
 */
void McuI2cLib_Init(void);

#endif /* MCUI2CLIB_H_ */
