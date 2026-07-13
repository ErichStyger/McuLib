/*
 * Copyright (c) 2022, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * \file
 * \brief Modbus communication interface.
 */

#ifndef MCUMODBUS_H_
#define MCUMODBUS_H_

#include "McuModbus_config.h"
#include <stdint.h>
#include <stdbool.h>
#include "McuShell.h"

typedef enum McuModbus_Func_Code_e {
  McuModbus_Func_ReadCoils              = 0x01, /*!< Read coils */
  McuModbus_Func_ReadDiscreteInputs     = 0x02, /*!< Read discrete inputs */
  McuModbus_Func_ReadHoldingRegisters   = 0x03, /*!< Read holding registers */
  McuModbus_Func_ReadInputRegisters     = 0x04, /*!< Read input registers */
  McuModbus_Func_WriteSingleCoil        = 0x05, /*!< Write single coil */
  McuModbus_Func_WriteSingleRegister    = 0x06, /*!< Write single register */
  McuModbus_Func_WriteMultipleCoils     = 0x0f, /*!< Write multiple coils */
  McuModbus_Func_WriteMultipleRegisters = 0x10, /*!< Write multiple registers */
} McuModbus_Func_Code_e;

/*!
 * \brief Shell command parser for the Modbus module.
 * \param cmd Command string to parse.
 * \param handled Set to true if command has been recognized and handled.
 * \param io I/O handler used for shell output.
 * \return Error code, ERR_OK if everything was fine.
 */
uint8_t McuModbus_ParseCommand(const unsigned char *cmd, bool *handled, const McuShell_StdIOType *io);

/*!
 * \brief Sends a generic Modbus request.
 * \param slaveID Modbus slave/device ID.
 * \param function Modbus function code.
 * \param addr Start address for the request.
 * \param nofCoils Number of coils/registers involved.
 * \param result Pointer to result buffer.
 * \param io I/O handler for diagnostic output, or NULL.
 */
void McuModbus_Send(uint8_t slaveID, McuModbus_Func_Code_e function, uint16_t addr, uint16_t nofCoils, uint8_t *result, const McuShell_StdIOType *io);

/*!
 * \brief Reads Modbus input registers.
 * \param deviceID Modbus slave/device ID.
 * \param addr Start register address.
 * \param nofRegs Number of registers to read.
 * \param result Buffer receiving register values.
 * \return Error code, ERR_OK if successful.
 */
uint8_t McuModbus_ReadInputRegisters(uint8_t deviceID, uint16_t addr, uint16_t nofRegs, uint16_t *result);
/*!
 * \brief Writes a single Modbus input register.
 * \param deviceID Modbus slave/device ID.
 * \param addr Register address.
 * \param value Register value to write.
 * \return Error code, ERR_OK if successful.
 */
uint8_t McuModbus_WriteInputRegister(uint8_t deviceID, uint16_t addr, uint16_t value);

/*!
 * \brief Reads Modbus holding registers.
 * \param deviceID Modbus slave/device ID.
 * \param addr Start register address.
 * \param nofRegs Number of registers to read.
 * \param result Buffer receiving register values.
 * \return Error code, ERR_OK if successful.
 */
uint8_t McuModbus_ReadHoldingRegisters(uint8_t deviceID, uint16_t addr, uint16_t nofRegs, uint16_t *result);
/*!
 * \brief Writes a single Modbus holding register.
 * \param deviceID Modbus slave/device ID.
 * \param addr Register address.
 * \param value Register value to write.
 * \return Error code, ERR_OK if successful.
 */
uint8_t McuModbus_WriteHoldingRegister(uint8_t deviceID, uint16_t addr, uint16_t value);

/*!
 * \brief Deinitializes the Modbus module.
 */
void McuModbus_Deinit(void);
/*!
 * \brief Initializes the Modbus module.
 */
void McuModbus_Init(void);

#endif /* MCUMODBUS_H_ */
