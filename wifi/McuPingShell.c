/*
 * Copyright (c) 2021-2026, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#if MCU_PING_CONFIG_ENABLED
#include "McuPingShell.h"
#include "McuPing.h"
#include "McuWiFi.h"
#include "McuShell.h"
#include "McuUtility.h"

static uint8_t PrintStatus(const McuShell_StdIOType *io) {
  McuShell_SendStatusStr((unsigned char*)"ping", (unsigned char*)"Ping status\r\n", io->stdOut);
  return ERR_OK;
}

static uint8_t PrintHelp(const McuShell_StdIOType *io) {
  McuShell_SendHelpStr((unsigned char*)"ping", (unsigned char*)"Group of ping commands\r\n", io->stdOut);
  McuShell_SendHelpStr((unsigned char*)"  help|status", (unsigned char*)"Shows ping help or status\r\n", io->stdOut);
  McuShell_SendHelpStr((unsigned char*)"  <host>", (unsigned char*)"Ping host\r\n", io->stdOut);
  return ERR_OK;
}

uint8_t McuPing_ParseCommand(const unsigned char* cmd, bool *handled, const McuShell_StdIOType *io) {
  const unsigned char *p;

  if (McuUtility_strcmp((char*)cmd, (char*)McuShell_CMD_HELP)==0 || McuUtility_strcmp((char*)cmd, (char*)"ping help")==0) {
    *handled = TRUE;
    return PrintHelp(io);
  } else if (McuUtility_strcmp((char*)cmd, (char*)McuShell_CMD_STATUS)==0 || McuUtility_strcmp((char*)cmd, (char*)"ping status")==0) {
    *handled = TRUE;
    return PrintStatus(io);
  } else if (McuUtility_strncmp((char*)cmd, (char*)"ping ", sizeof("ping ")-1)==0) {
    *handled = TRUE;
    if (!McuWiFi_isConnected()) {
      McuShell_SendStr((unsigned char*)"Network not connected\r\n", io->stdErr);
      return ERR_FAILED;
    }
    p = cmd + sizeof("ping ")-1;
    if (McuPing_DoPingCmd((const char*)p)==0) {
      return ERR_OK;
    } else {
      return ERR_FAILED;
    }
  }
  return ERR_OK;
}

#endif /* MCU_PING_CONFIG_ENABLED */
