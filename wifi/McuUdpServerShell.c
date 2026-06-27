/*
 * Copyright (c) 2021-2026, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "McuUdpServer_config.h"
#if MCU_UDP_SERVER_CONFIG_USE_SHELL
#include "McuUdpServerShell.h"
#include "McuUdpServer.h"
#include "McuShell.h"
#include "McuUtility.h"
#include "shell.h"

static uint8_t PrintStatus(const McuShell_StdIOType *io) {
  unsigned char buf[32];

  McuShell_SendStatusStr((unsigned char*)"udps", (unsigned char*)"UDP server status\r\n", io->stdOut);
  McuUtility_Num32sToStr(buf, sizeof(buf), MCU_UDP_SERVER_CONFIG_PORT);
  McuUtility_strcat(buf, sizeof(buf), (unsigned char*)"\r\n");
  McuShell_SendStatusStr((unsigned char*)"  port", buf, io->stdOut);
  return ERR_OK;
}

static uint8_t PrintHelp(const McuShell_StdIOType *io) {
  McuShell_SendHelpStr((unsigned char*)"udps", (unsigned char*)"Group of udp server commands\r\n", io->stdOut);
  McuShell_SendHelpStr((unsigned char*)"  help|status", (unsigned char*)"Shows udp server help or status\r\n", io->stdOut);
  return ERR_OK;
}

uint8_t McuUdpServer_ParseCommand(const unsigned char* cmd, bool *handled, const McuShell_StdIOType *io) {
  if (McuUtility_strcmp((char*)cmd, (char*)McuShell_CMD_HELP)==0 || McuUtility_strcmp((char*)cmd, (char*)"udps help")==0) {
    *handled = TRUE;
    return PrintHelp(io);
  } else if (McuUtility_strcmp((char*)cmd, (char*)McuShell_CMD_STATUS)==0 || McuUtility_strcmp((char*)cmd, (char*)"udps status")==0) {
    *handled = TRUE;
    return PrintStatus(io);
  }
  return ERR_OK;
}

#endif /* MCU_UDP_SERVER_CONFIG_USE_SHELL */
