/*
 * Copyright (c) 2022, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * \file
 * \brief Heidelberg wallbox integration interface.
 */

#ifndef MODBUS_MCUHEIDELBERG_H_
#define MODBUS_MCUHEIDELBERG_H_

#include "McuShell.h"
#include <stdint.h>
#include <stdbool.h>

typedef enum McuHeidelberg_Event_e {
  McuHeidelberg_Event_HW_State_Changed,           /*!< Hardware charger state has changed */
  McuHeidelberg_Event_UserChargingMode_Changed,   /*!< User charging mode has changed */
  McuHeidelberg_Event_ChargingSetCurrent_Changed, /*!< Charging current has changed */
  McuHeidelberg_Event_SolarPower_Changed,         /*!< Produced solar power has changed */
  McuHeidelberg_Event_SitePower_Changed,          /*!< Site power consumption has changed */
  McuHeidelberg_Event_ChargerPower_Changed,       /*!< Charger/car power has changed */
  McuHeidelberg_Event_GridPower_Changed,          /*!< Grid power has changed */
  McuHeidelberg_Event_BatteryPower_Changed,       /*!< Battery power has changed */
} McuHeidelberg_Event_e;

typedef void (*McuHeidelberg_EventCallback)(McuHeidelberg_Event_e);
  /*!< Event callback function prototype */

/*!
 * \brief Register a callback for events
 * \param callback function pointer for the callback
 */
void McuHeidelberg_RegisterEventCallback(McuHeidelberg_EventCallback callback);

typedef enum McuHeidelberg_ChargingMode_e {
  McuHeidelberg_User_ChargingMode_Stop,             /*!< Stop charging immediately */
  McuHeidelberg_User_ChargingMode_OnlyPV,           /*!< Charge only with available PV power */
  McuHeidelberg_User_ChargingMode_Slow,             /*!< Charge immediately with minimal power */
  McuHeidelberg_User_ChargingMode_SlowPlusPV,       /*!< Minimal charging power plus additional PV power if available */
  McuHeidelberg_User_ChargingMode_Fast,             /*!< Charge immediately with maximum power */
  McuHeidelberg_User_ChargingMode_6_Amp,            /*!< Fixed current mode: 6 A */
  McuHeidelberg_User_ChargingMode_7_Amp,            /*!< Fixed current mode: 7 A */
  McuHeidelberg_User_ChargingMode_8_Amp,            /*!< Fixed current mode: 8 A */
  McuHeidelberg_User_ChargingMode_9_Amp,            /*!< Fixed current mode: 9 A */
  McuHeidelberg_User_ChargingMode_10_Amp,           /*!< Fixed current mode: 10 A */
  McuHeidelberg_User_ChargingMode_11_Amp,           /*!< Fixed current mode: 11 A */
  McuHeidelberg_User_ChargingMode_12_Amp,           /*!< Fixed current mode: 12 A */
  McuHeidelberg_User_ChargingMode_13_Amp,           /*!< Fixed current mode: 13 A */
  McuHeidelberg_User_ChargingMode_14_Amp,           /*!< Fixed current mode: 14 A */
  McuHeidelberg_User_ChargingMode_15_Amp,           /*!< Fixed current mode: 15 A */
  McuHeidelberg_User_ChargingMode_16_Amp,           /*!< Fixed current mode: 16 A */
  McuHeidelberg_User_ChargingMode_NofChargingMode,  /*!< Sentinel, must be last in list */
} McuHeidelberg_UserChargingMode_e;

/*!
 * \brief Returns the currently selected user charging mode.
 * \return Current charging mode.
 */
McuHeidelberg_UserChargingMode_e McuHeidelberg_GetUserChargingMode(void);
/*!
 * \brief Sets the user charging mode.
 * \param mode Charging mode to apply.
 */
void McuHeidelberg_SetUserChargingMode(McuHeidelberg_UserChargingMode_e mode);

typedef enum McuHeidelbergChargerState_e {
  McuHeidelberg_ChargerState_A1       = 2,  /*!< No vehicle, charging not possible */
  McuHeidelberg_ChargerState_A2       = 3,  /*!< No vehicle, charging possible */
  McuHeidelberg_ChargerState_B1       = 4,  /*!< Vehicle plugged, charging not possible */
  McuHeidelberg_ChargerState_B2       = 5,  /*!< Vehicle plugged, charging possible */
  McuHeidelberg_ChargerState_C1       = 6,  /*!< Vehicle requests charging, charging not possible */
  McuHeidelberg_ChargerState_C2       = 7,  /*!< Vehicle requests charging, charging possible */
  McuHeidelberg_ChargerState_Derating = 8,  /*!< Charging current derating due to temperature */
  McuHeidelberg_ChargerState_E        = 9,  /*!< Error state */
  McuHeidelberg_ChargerState_F        = 10, /*!< Wallbox locked or not ready */
  McuHeidelberg_ChargerState_Error    = 11, /*!< Generic error state */
} McuHeidelbergChargerState_e;

/*!
 * \brief Returns the hardware charger state.
 * \return Current hardware charger state.
 */
McuHeidelbergChargerState_e McuHeidelberg_GetHWChargerState(void);
/*!
 * \brief Returns a short text for a hardware charger state.
 * \param state Charger state to translate.
 * \return Short state string.
 */
const unsigned char *McuHeidelberg_GetShortHWChargerStateString(McuHeidelbergChargerState_e state);

/*!
 * \brief Returns current solar power in Watt.
 * \return Solar power in Watt.
 */
uint32_t McuHeidelberg_GetSolarPowerWatt(void);
/*!
 * \brief Sets current solar power in Watt.
 * \param powerW Solar power in Watt.
 */
void McuHeidelberg_SetSolarPowerWatt(uint32_t powerW);

/*!
 * \brief Returns current site power in Watt.
 * \return Site power in Watt.
 */
uint32_t McuHeidelberg_GetSitePowerWatt(void);
/*!
 * \brief Sets current site power in Watt.
 * \param powerW Site power in Watt.
 */
void McuHeidelberg_SetSitePowerWatt(uint32_t powerW);

/*!
 * \brief Returns current grid power in Watt.
 * \return Grid power in Watt.
 */
int32_t McuHeidelberg_GetGridPowerWatt(void);
/*!
 * \brief Sets current grid power in Watt.
 * \param powerW Grid power in Watt.
 */
void McuHeidelberg_SetGridPowerWatt(int32_t powerW);

/*!
 * \brief Returns current battery power in Watt.
 * \return Battery power in Watt.
 */
int32_t McuHeidelberg_GetBatteryPowerWatt(void);
/*!
 * \brief Sets current battery power in Watt.
 * \param powerW Battery power in Watt.
 */
void McuHeidelberg_SetBatteryPowerWatt(int32_t powerW);

/*!
 * \brief Returns the maximum car charging power.
 * \return Maximum car charging power.
 */
uint32_t McuHeidelberg_GetMaxCarPower(void);

/*!
 * \brief Returns the current charger power.
 * \return Current charger power.
 */
uint32_t McuHeidelberg_GetCurrChargerPower(void);

/*!
 * \brief Shell command parser for the Heidelberg module.
 * \param cmd Command string to parse.
 * \param handled Set to true if command has been recognized and handled.
 * \param io I/O handler used for shell output.
 * \return Error code, ERR_OK if everything was fine.
 */
uint8_t McuHeidelberg_ParseCommand(const unsigned char *cmd, bool *handled, const McuShell_StdIOType *io);

/*!
 * \brief Deinitializes the Heidelberg module.
 */
void McuHeidelberg_Deinit(void);
/*!
 * \brief Initializes the Heidelberg module.
 */
void McuHeidelberg_Init(void);

#endif /* MODBUS_MCUHEIDELBERG_H_ */
