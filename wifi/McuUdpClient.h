/*
 * Copyright (c) 2019-2026, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MCU_UDP_CLIENT_H_
#define MCU_UDP_CLIENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "McuUdpClient_config.h"
#if MCU_UDP_CLIENT_CONFIG_ENABLED
#include <stdint.h>
#include <stddef.h>

uint8_t McuUdpClient_Send(const char *host, uint16_t port, const char *msg, char *rxBuffer, size_t rxBufferSize);

/*! \brief Module de-initialization */
void McuUdpClient_DeInit(void);

/*! \brief Module initialization */
void McuUdpClient_Init(void);

#endif /* MCU_UDP_CLIENT_CONFIG_ENABLED */

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* MCU_UDP_CLIENT_H_ */
