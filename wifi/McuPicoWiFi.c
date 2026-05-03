/*
 * Copyright (c) 2022-2026, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "McuPicoWiFi.h"
#if MCU_PICO_WIFI_CONFIG_ENABLED
#include "pico/cyw43_arch.h"

static bool cyw43IsInitialized = false;

void McuPicoWiFi_SetArchIsInitialized(bool isInitialized) {
  cyw43IsInitialized = isInitialized;
}

bool McuPicoWiFi_GetArchIsInitialized(void) {
  return cyw43IsInitialized;
}

const unsigned char *McuPicoWiFi_GetTcpIpLinkStatusString(int linkStatus) {
  /* return a string for the value returned by cyw43_tcpip_link_status() */
  const unsigned char *statusStr;

  switch(linkStatus) {
    case CYW43_LINK_DOWN:     statusStr = "LINK_DOWN, WiFi down"; break;
    case CYW43_LINK_JOIN:     statusStr = "LINK_JOIN, connected to WiFi"; break;
    case CYW43_LINK_NOIP:     statusStr = "LINK_NOIP, connected to WiFi, but no IP address "; break;
    case CYW43_LINK_UP:       statusStr = "LINK_UP, connect to WiFi with an IP address "; break;
    case CYW43_LINK_FAIL:     statusStr = "LINK_FAIL, connection failed "; break;
    case CYW43_LINK_NONET:    statusStr = "LINK_NONET, no matching SSID found "; break;
    case CYW43_LINK_BADAUTH:  statusStr = "LINK_BADAUTH, authentication failure"; break;
    default:                  statusStr = "<unknown>"; break;
  } /* switch */
  return statusStr;
}

void McuPicoWiFi_Deinit(void) {
  cyw43_arch_deinit();
  McuPicoWiFi_SetArchIsInitialized(false);
}

void McuPicoWiFi_Init(void) {
  McuPicoWiFi_SetArchIsInitialized(false);
}

#endif /* MCU_PIOO_WIFI_CONFIG_ENABLED */
