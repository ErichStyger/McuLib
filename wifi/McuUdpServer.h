/*
 * Copyright (c) 2019, 2020, 2023, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * \file
 * \brief UDP server interface.
 */

#ifndef SRC_UDP_SERVER_H_
#define SRC_UDP_SERVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "McuUdpServer_config.h"
#if MCU_UDP_SERVER_CONFIG_ENABLED

#include <stddef.h> /* size_t */
/*!
 * \brief Set an optional callback which can be used by the application
 * \param rxMsg Pointer to the received message
 * \param responseBuf Buffer for the response back to the sender
 */
void McuUdpServer_SetIncomingCallback(void (*callback)(const char *rxBuffer, int rxLen, char *responseBuf, size_t responseBufLen, size_t *responseSize));

/*! \brief Resume the UDP server */
void McuUdpServer_Resume(void);

/*! \brief stop the UDP server */
void McuUdpServer_Suspend(void);

/*! \brief Module initialization */
void McuUdpServer_Init(void);

#endif /* MCU_UDP_SERVER_CONFIG_ENABLED */

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* SRC_UDP_SERVER_H_ */
