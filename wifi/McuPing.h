/*
 * Copyright (c) 2021-2026, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MCU_PING_H_
#define MCU_PING_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "McuPing.h"

/*!
 * \brief performs a network ping
 * \param host host name or address
 * \return 0 for ok, negative values for error
 */
int McuPing_DoPingCmd(const char *host);

/*!
 * \brief Module de-initialization
 */
void McuPing_Deinit(void);

/*!
 * \brief Module initialization
 */
void McuPing_Init(void);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* MCU_PING_H_ */
