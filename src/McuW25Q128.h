/*
 * Copyright (c) 2022, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * \file
 * \brief Winbond W25Q128 SPI flash driver interface.
 */

#ifndef MCU_W25Q128_H_
#define MCU_W25Q128_H_

#include "McuW25Q128config.h"
#include <stdint.h>
#include <stddef.h>
#include "McuShell.h"

/* supported devices */
typedef enum {
  McuW25_DEVICE_UNKNOWN,             /*!< Unknown or unsupported device */
  McuW25_DEVICE_W25Q128JV_IQ_JQ,     /*!< W25Q128JV (IQ/JQ package) */
  McuW25_DEVICE_W25Q128JV_IM_JM,     /*!< W25Q128JV (IM/JM package) */
} McuW25_Device_e;

/*!
 * \brief Returns the detected flash device type.
 * \return Device type enumeration.
 */
McuW25_Device_e McuW25_GetDeviceType(void);

/*!
 * \brief Shell command parser for the W25Q128 driver.
 * \param cmd Command string.
 * \param handled Set to true if command has been handled.
 * \param io I/O stream for shell output.
 * \return Error code, ERR_OK if successful.
 */
uint8_t McuW25_ParseCommand(const unsigned char* cmd, bool *handled, const McuShell_StdIOType *io);

#define McuW25_SERIAL_BUF_SIZE  (8)
/*!
 * \brief Reads the unique serial number.
 * \param buf Destination buffer.
 * \param bufSize Buffer size in bytes (at least McuW25_SERIAL_BUF_SIZE).
 * \return Error code, ERR_OK if successful.
 */
uint8_t McuW25_ReadSerialNumber(uint8_t *buf, size_t bufSize);

#define McuW25_ID_BUF_SIZE  (3)
/*!
 * \brief Reads manufacturer/device ID bytes.
 * \param buf Destination buffer.
 * \param bufSize Buffer size in bytes (at least McuW25_ID_BUF_SIZE).
 * \return Error code, ERR_OK if successful.
 */
uint8_t McuW25_ReadID(uint8_t *buf, size_t bufSize);

/*!
 * \brief Reads status register 1.
 * \param status Pointer receiving status byte.
 * \return Error code, ERR_OK if successful.
 */
uint8_t McuW25_ReadStatus1(uint8_t *status);

/*!
 * \brief Checks if the flash device is busy.
 * \return true if device is busy.
 */
bool McuW25_isBusy(void);

/*!
 * \brief Waits until device is no longer busy.
 */
void McuW25_WaitIfBusy(void);

/*!
 * \brief Reads data from flash.
 * \param address Start flash address.
 * \param buf Destination buffer.
 * \param bufSize Number of bytes to read.
 * \return Error code, ERR_OK if successful.
 */
uint8_t McuW25_Read(uint32_t address, uint8_t *buf, size_t bufSize);

/*!
 * \brief Erases the complete flash device.
 * \return Error code, ERR_OK if successful.
 */
uint8_t McuW25_EraseAll(void);

/*!
 * \brief Erases one 4 KByte sector.
 * \param address Address inside target sector.
 * \return Error code, ERR_OK if successful.
 */
uint8_t McuW25_EraseSector4K(uint32_t address);

/*!
 * \brief Erases one 32 KByte block.
 * \param address Address inside target block.
 * \return Error code, ERR_OK if successful.
 */
uint8_t McuW25_EraseBlock32K(uint32_t address);

/*!
 * \brief Erases one 64 KByte block.
 * \param address Address inside target block.
 * \return Error code, ERR_OK if successful.
 */
uint8_t McuW25_EraseBlock64K(uint32_t address);

/*!
 * Program a page with data
 * \param address, should be aligned on page (256 bytes) if programming 256 bytes
 * \param data pointer to data
 * \param dataSize size of data in bytes, max 256
 * \return error code, ERR_OK for no error
 */
uint8_t McuW25_ProgramPage(uint32_t address, const uint8_t *data, size_t dataSize);

/*!
 * \brief Returns total flash capacity in bytes.
 * \param capacity Pointer receiving capacity.
 * \return Error code, ERR_OK if successful.
 */
uint8_t McuW25_GetCapacity(uint32_t *capacity);

/*! \brief Module de-initialization */
void McuW25_Deinit(void);

/*! \brief Module initialization */
void McuW25_Init(void);

#endif /* MCU_W25Q128_H_ */
