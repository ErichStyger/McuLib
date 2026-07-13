/*
 * Copyright (c) 2024, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * Devines the SDEP IDs.
 */

/*!
 * \file
 * \brief Generic buffered I/O descriptor interface.
 */

#ifndef _MCU_IO_H__
#define _MCU_IO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "McuIOconfig.h"
#include <stddef.h>

typedef struct McuIO_Desc_t {
  struct {
    void *buf; /*!< Handle to the underlying buffer object */
    int (*write)(struct McuIO_Desc_t *io, char ch); /*!< Writes one character, returns number of bytes written */
    int (*read)(struct McuIO_Desc_t *io); /*!< Reads one character and returns it, or EOF */
    size_t (*nofData)(struct McuIO_Desc_t *io); /*!< Returns number of buffered data bytes */
    size_t (*nofFree)(struct McuIO_Desc_t *io); /*!< Returns number of free bytes in buffer */
  } buffer;
  struct {
    void (*write)(char ch); /*!< Low-level character output function */
    void (*flush)(void); /*!< Optional flush function, can be NULL */
  } out;
} McuIO_Desc_t;

/*!
 * \brief Allocates and initializes a new I/O descriptor with buffer storage.
 * \param nofBufferElements Number of elements in the internal buffer.
 * \return Pointer to new descriptor, or NULL on error.
 */
McuIO_Desc_t *McuIO_NewIO(size_t nofBufferElements);
/*!
 * \brief Frees an I/O descriptor and associated resources.
 * \param io Descriptor to release.
 * \return NULL.
 */
McuIO_Desc_t *McuIO_FreeIO(McuIO_Desc_t *io);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* _MCU_IO_H__ */