/*
 * Copyright (c) 2021-2026, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#if McuLib_CONFIG_CPU_IS_ESP32
#include "McuEsp32Mac.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_mac.h"
#include "Identify.h"
#include "McuUtility.h"
#include "McuXFormat.h"
#include "McuLog.h"
#include "McuWiFi.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void McuEsp32Mac_SetMac(uint8_t *mac) {
  esp_err_t err = esp_wifi_set_mac(WIFI_IF_STA, mac);
  if (err == ESP_OK) {
    McuLog_info("MAC address", "MAC address successfully set to %02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  } else {
    McuLog_error("MAC address", "Failed to set MAC address");
  }
}

bool McuEsp32Mac_MacAreSame(const uint8_t macA[6], const uint8_t macB[6]) {
  for(int j=0; j<6; j++) {
    if(macA[j] != macB[j]) {
      return false; /* no match */
    }
  }
  return true; /* match! */
}

void McuEsp32Mac_MacToString(uint8_t mac[MCU_ESP32_MAC_ADDR_SIZE], uint8_t *buf, size_t bufSize) {
  McuXFormat_xsnprintf((char*)buf, bufSize, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

uint8_t McuEsp32Mac_ReadMac(uint8_t mac[MCU_ESP32_MAC_ADDR_SIZE]) {
  if (esp_wifi_get_mac(WIFI_IF_STA, mac)!=ESP_OK) {
    McuLog_fatal("failed reading MAC");
    return ERR_FAILED;
  }
  return ERR_OK;
}

void McuEsp32Mac_Init(void) {
  esp_err_t  res;

  /* set MAC address, otherwise will get "system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE" whenever asking for it */
  uint8_t mac[MCU_ESP32_MAC_ADDR_SIZE];
  res = esp_read_mac(&mac[0], ESP_MAC_EFUSE_FACTORY); /* read from fuses */
  if (res==ESP_OK) {
    if (esp_iface_mac_addr_set(&mac[0], ESP_MAC_BASE)!=ESP_OK) {
      McuLog_fatal("failed setting MAC");
    }
  } else {
    McuLog_fatal("failed reading MAC");
  }
}

#endif /* McuLib_CONFIG_CPU_IS_ESP32 */
