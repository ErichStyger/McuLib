/*
 * Copyright (c) 2019-2026, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * \file
 * \brief UDP client interface.
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

/*!
 * \brief Sends a UDP message and optionally receives a response.
 * \param host Target host name or IP address.
 * \param port Target UDP port.
 * \param msg Message string to send.
 * \param rxBuffer Receive buffer for reply data, or NULL.
 * \param rxBufferSize Size of receive buffer in bytes.
 * \return Error code, ERR_OK if successful.
 */
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
