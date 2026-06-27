/*
 * Copyright (c) 2021-2023, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "McuUdpClientShell.h"
#if MCU_UDP_CLIENT_CONFIG_USE_SHELL
#include "McuUdpClient.h"
#include "McuShell.h"
#include "McuUtility.h"

static uint16_t udp_client_destination_port = MCU_UDP_CLIENT_CONFIG_DEFAULT_SERVER_PORT;
static char udp_client_destination_host[48] = MCU_UDP_CLIENT_CONFIG_DEFAULT_SERVER;

uint16_t McuUdpClient_GetDefaultHostPort(void) {
  return udp_client_destination_port;
}

void McuUdpClient_SetDefaultHostPort(uint16_t port) {
  udp_client_destination_port = port;
}

const char *McuUdpClient_GetDefaultHostName(void) {
  return udp_client_destination_host;
}

void McuUdpClient_SetDefaultHostName(const char *host) {
  McuUtility_strcpy(udp_client_destination_host, sizeof(udp_client_destination_host), host);
}

static uint8_t PrintStatus(const McuShell_StdIOType *io) {
  unsigned char buf[64];

  McuShell_SendStatusStr((unsigned char*)"udpc", (unsigned char*)"UDP client status\r\n", io->stdOut);
  McuUtility_strcpy(buf, sizeof(buf), McuUdpClient_GetDefaultHostName());
  McuUtility_chcat(buf, sizeof(buf), ':');
  McuUtility_strcatNum16u(buf, sizeof(buf), McuUdpClient_GetDefaultHostPort());
  McuUtility_strcat(buf, sizeof(buf), (unsigned char*)"\r\n");
  McuShell_SendStatusStr((unsigned char*)"  host:port", buf, io->stdOut);
  return ERR_OK;
}

static uint8_t PrintHelp(const McuShell_StdIOType *io) {
  McuShell_SendHelpStr((unsigned char*)"udpc", (unsigned char*)"Group of UDP client commands\r\n", io->stdOut);
  McuShell_SendHelpStr((unsigned char*)"  help|status", (unsigned char*)"Shows motor help or status\r\n", io->stdOut);
  McuShell_SendHelpStr((unsigned char*)"  host <string>", (unsigned char*)"Set default host destination IP address or host name\r\n", io->stdOut);
  McuShell_SendHelpStr((unsigned char*)"  port <number>", (unsigned char*)"Set default host destination port number\r\n", io->stdOut);
  McuShell_SendHelpStr((unsigned char*)"  tx <msg>", (unsigned char*)"Send message to default host and port\r\n", io->stdOut);
  McuShell_SendHelpStr((unsigned char*)"  send \"<host\" <p> <msg>", (unsigned char*)"Send message to ip/host (double quoted) and port\r\n", io->stdOut);
  return ERR_OK;
}

uint8_t McuUdpClient_ParseCommand(const unsigned char* cmd, bool *handled, const McuShell_StdIOType *io) {
  const unsigned char *p;
  unsigned char rxBuf[128];

  if (McuUtility_strcmp((char*)cmd, (char*)McuShell_CMD_HELP)==0 || McuUtility_strcmp((char*)cmd, (char*)"udpc help")==0) {
    *handled = TRUE;
    return PrintHelp(io);
  } else if (McuUtility_strcmp((char*)cmd, (char*)McuShell_CMD_STATUS)==0 || McuUtility_strcmp((char*)cmd, (char*)"udpc status")==0) {
    *handled = TRUE;
    return PrintStatus(io);
  } else if (McuUtility_strncmp((char*)cmd, (char*)"udpc host ", sizeof("udpc host ")-1)==0) {
    *handled = TRUE;
    p = cmd + sizeof("udpc host ")-1;
    McuUdpClient_SetDefaultHostName(p);
    return ERR_OK;
  } else if (McuUtility_strncmp((char*)cmd, (char*)"udpc port ", sizeof("udpc port ")-1)==0) {
    int16_t port;
    *handled = TRUE;
    p = cmd + sizeof("udpc port ")-1;
    if (McuUtility_ScanDecimal16uNumber(&p, &port)!=ERR_OK) {
      return ERR_FAILED;
    }
    McuUdpClient_SetDefaultHostPort(port);
    return ERR_OK;
  } else if (McuUtility_strncmp((char*)cmd, (char*)"udpc tx ", sizeof("udpc tx ")-1)==0) {
   *handled = TRUE;
    p = cmd + sizeof("udpc tx ")-1;
    if (McuUdpClient_Send(McuUdpClient_GetDefaultHostName(), McuUdpClient_GetDefaultHostPort(), p, rxBuf, sizeof(rxBuf))!=ERR_OK) {
      return ERR_FAILED;
    }
    McuShell_SendStr(rxBuf, io->stdOut); /* writing data received to shell */
    return ERR_OK;
  } else if (McuUtility_strncmp((char*)cmd, (char*)"udpc send ", sizeof("udpc send ")-1)==0) {
    unsigned char ip[64];
    uint16_t port;

    *handled = TRUE;
    p = cmd + sizeof("udpc send ")-1;
    if (McuUtility_ScanDoubleQuotedString(&p, ip, sizeof(ip))!=ERR_OK) {
      return ERR_FAILED;
    }
    if (McuUtility_ScanDecimal16uNumber(&p, &port)!=ERR_OK) {
      return ERR_FAILED;
    }
    while(*p==' ') { /* skip spaces */
       p++;
    }
    if (McuUdpClient_Send(ip, port, p, rxBuf, sizeof(rxBuf))!=ERR_OK) {
      return ERR_FAILED;
    }
    McuShell_SendStr(rxBuf, io->stdOut);
    return ERR_OK;
  }
  return ERR_OK;
}

#endif /* MCU_UDP_CLIENT_CONFIG_USE_SHELL */
