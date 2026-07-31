/*!
 * \file
 * \brief Interface for the Remote Debugger Interface (RdiMon).
 */

#ifndef MCURDIMON_H
#define MCURDIMON_H

#include "McuRdimon_config.h"


#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief Initializes the RdiMon standard I/O handles.
 */
void McuRdimon_Init(void);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* MCURDIMON_H */
