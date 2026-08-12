/*
 * Copyright (c) 2021-2026, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MCUESP32_MAC_H_
#define MCUESP32_MAC_H_

#ifdef __cplusplus
extern "C" {
#endif

#if McuLib_CONFIG_CPU_IS_ESP32
#include <stdint.h>
#include <stddef.h> /* for size_t */

#define MCU_ESP32_MAC_ADDR_SIZE  (6) /*!< number of bytes for MAC address */

/*!
 * \brief transforms a binary MAC into a string, e.g. "d8:a0:1d:42:ed:50"
 * \param mac binary MAC
 * \param buf buffer where to store the string
 * \param bufSize size of buffer
 */
void McuEsp32Mac_MacToString(uint8_t mac[MCU_ESP32_MAC_ADDR_SIZE], uint8_t *buf, size_t bufSize);

/*!
 * \brief Read the MAC address into buffer
 * \param mac buffer where to store the MAC address
 */
uint8_t McuEsp32Mac_ReadMac(uint8_t mac[MCU_ESP32_MAC_ADDR_SIZE]);

/*!
 * \brief Module initialization
 */
void McuEsp32Mac_Init(void);

#endif /* McuLib_CONFIG_CPU_IS_ESP32 */

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* MCUESP32_MAC_H_ */
