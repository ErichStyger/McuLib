/*
 * Copyright (c) 2019, 2020, 2023, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SRC_UDP_SERVER_H_
#define SRC_UDP_SERVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "McuUdpServer_config.h"
#if MCU_UDP_SERVER_CONFIG_ENABLED

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
