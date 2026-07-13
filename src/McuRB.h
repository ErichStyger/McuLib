/*
 * Copyright (c) 2019, Erich Styger
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*!
 * \file
 * \brief Generic ring buffer interface.
 */

#ifndef MCURINGBUFFER_H_
#define MCURINGBUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include "McuRBconfig.h"

typedef void *McuRB_Handle_t; /*!< Opaque ring buffer handle */

typedef struct {
  size_t nofElements; /*!< Maximum number of elements in buffer */
  size_t elementSize; /*!< Size of one element in bytes */
} McuRB_Config_t;

/*!
 * \brief Returns number of used elements in the ring buffer.
 * \param rb Ring buffer handle.
 * \return Number of elements currently stored.
 */
size_t McuRB_NofElements(McuRB_Handle_t rb);

/*!
 * \brief Returns number of free element slots in the ring buffer.
 * \param rb Ring buffer handle.
 * \return Number of free elements.
 */
size_t McuRB_NofFreeElements(McuRB_Handle_t rb);

/*!
 * \brief Adds one element to the ring buffer.
 * \param rb Ring buffer handle.
 * \param data Pointer to element data.
 * \return ERR_OK on success.
 */
uint8_t McuRB_Put(McuRB_Handle_t rb, void *data);

/*!
 * \brief Adds multiple elements to the ring buffer.
 * \param rb Ring buffer handle.
 * \param data Pointer to the first element.
 * \param nof Number of elements to add.
 * \return ERR_OK on success.
 */
uint8_t McuRB_Putn(McuRB_Handle_t rb, void *data, size_t nof);

/*!
 * \brief Clears all elements from the ring buffer.
 * \param rb Ring buffer handle.
 */
void McuRB_Clear(McuRB_Handle_t rb);

/*!
 * \brief Retrieves and removes one element from the ring buffer.
 * \param rb Ring buffer handle.
 * \param data Pointer receiving the element.
 * \return ERR_OK on success.
 */
uint8_t McuRB_Get(McuRB_Handle_t rb, void *data);

/*!
 * \brief Reads one element from the ring buffer without removing it.
 * \param rb Ring buffer handle.
 * \param index Element index relative to current read position.
 * \param data Pointer receiving the element.
 * \return ERR_OK on success.
 */
uint8_t McuRB_Peek(McuRB_Handle_t rb, size_t index, void *data);

/*!
 * \brief Compares ring buffer content with provided data.
 * \param rb Ring buffer handle.
 * \param index Start index relative to current read position.
 * \param data Pointer to comparison data.
 * \param nof Number of elements to compare.
 * \return 0 if equal, non-zero otherwise.
 */
uint8_t McuRB_Compare(McuRB_Handle_t rb, size_t index, void *data, size_t nof);

/*!
 * \brief Fills a configuration with default ring buffer settings.
 * \param config Pointer to configuration structure.
 */
void McuRB_GetDefaultconfig(McuRB_Config_t *config);

/*!
 * \brief Creates and initializes a new ring buffer instance.
 * \param config Pointer to ring buffer configuration.
 * \return Ring buffer handle, or NULL on error.
 */
McuRB_Handle_t McuRB_InitRB(McuRB_Config_t *config);

/*!
 * \brief Deinitializes a ring buffer instance.
 * \param rb Ring buffer handle.
 * \return NULL.
 */
McuRB_Handle_t McuRB_DeinitRB(McuRB_Handle_t rb);

/*!
 * \brief Deinitializes the ring buffer module.
 */
void McuRB_Deinit(void);

/*!
 * \brief Initializes the ring buffer module.
 */
void McuRB_Init(void);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* MCURINGBUFFER_H_ */
