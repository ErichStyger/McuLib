/*
 * Copyright (c) 2022, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * \file
 * \brief Heidelberg wallbox integration configuration.
 */

#ifndef MODBUS_MCUHEIDELBERG_CONFIG_H_
#define MODBUS_MCUHEIDELBERG_CONFIG_H_

#ifndef McuHeidelberg_CONFIG_USE_MOCK_WALLBOX
  #define McuHeidelberg_CONFIG_USE_MOCK_WALLBOX   (0) /*!< 1: use testing mocks for wallbox state and phase values */
#endif

#ifndef McuHeidelberg_CONFIG_USE_MOCK_SOLAR
  #define McuHeidelberg_CONFIG_USE_MOCK_SOLAR     (0) /*!< 1: use testing mocks for solar and site power */
#endif

#ifndef McuHeidelberg_CONFIG_USE_WATCHDOG
  #define McuHeidelberg_CONFIG_USE_WATCHDOG       (0) /*!< 1: use MCU watchdog functionality */
#endif

#ifndef McuHeidelberg_CONFIG_HYSTERESIS_POWER
  #define McuHeidelberg_CONFIG_HYSTERESIS_POWER    (150) /*!< Hysteresis power value */
#endif

#ifndef McuHeidelberg_CONFIG_BASE_SITE_POWER
  #define McuHeidelberg_CONFIG_BASE_SITE_POWER    (250) /*!< Site base power value */
#endif

#ifndef McuHeidelberg_CONFIG_DEFAULT_CHARGING_MODE
  #define McuHeidelberg_CONFIG_DEFAULT_CHARGING_MODE    McuHeidelberg_User_ChargingMode_SlowPlusPV /*!< Startup charging mode */
#endif

#endif /* MODBUS_MCUHEIDELBERG_CONFIG_H_ */
