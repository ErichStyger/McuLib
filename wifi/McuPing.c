/*
 * Copyright (c) 2021-2026, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#if MCU_PING_CONFIG_ENABLED
#include "McuPing.h"
#include "sdkconfig.h"
#include "lwip/inet.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#if McuLib_CONFIG_CPU_IS_ESP32
  #include "esp_event.h"
  #include "nvs_flash.h"
//  #include "argtable3/argtable3.h"
  #include "ping/ping_sock.h"
#endif
#include "McuLog.h"

static void cmd_ping_on_ping_success(esp_ping_handle_t hdl, void *args) {
#if McuLib_CONFIG_CPU_IS_ESP32
  uint8_t ttl;
  uint16_t seqno;
  uint32_t elapsed_time, recv_len;
  ip_addr_t target_addr;
  esp_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
  esp_ping_get_profile(hdl, ESP_PING_PROF_TTL, &ttl, sizeof(ttl));
  esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
  esp_ping_get_profile(hdl, ESP_PING_PROF_SIZE, &recv_len, sizeof(recv_len));
  esp_ping_get_profile(hdl, ESP_PING_PROF_TIMEGAP, &elapsed_time, sizeof(elapsed_time));
  McuLog_info("%lu bytes from %s icmp_seq=%d ttl=%d time=%lu ms\n",
         recv_len, inet_ntoa(target_addr.u_addr.ip4), seqno, ttl, elapsed_time);
#endif
}

#if McuLib_CONFIG_CPU_IS_ESP32
static void cmd_ping_on_ping_timeout(esp_ping_handle_t hdl, void *args) {
  uint16_t seqno;
  ip_addr_t target_addr;
  esp_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
  esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
  McuLog_info("From %s icmp_seq=%d timeout\n", inet_ntoa(target_addr.u_addr.ip4), seqno);
}
#endif

#if McuLib_CONFIG_CPU_IS_ESP32
void cmd_ping_on_ping_end(esp_ping_handle_t hdl, void *args) {
  ip_addr_t target_addr;
  uint32_t transmitted;
  uint32_t received;
  uint32_t total_time_ms;
  esp_ping_get_profile(hdl, ESP_PING_PROF_REQUEST, &transmitted, sizeof(transmitted));
  esp_ping_get_profile(hdl, ESP_PING_PROF_REPLY, &received, sizeof(received));
  esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
  esp_ping_get_profile(hdl, ESP_PING_PROF_DURATION, &total_time_ms, sizeof(total_time_ms));
  uint32_t loss = (uint32_t)((1 - ((float)received) / transmitted) * 100);
  if (IP_IS_V4(&target_addr)) {
      McuLog_info("\n--- %s ping statistics ---\n", inet_ntoa(*ip_2_ip4(&target_addr)));
  } else {
      McuLog_info("\n--- %s ping statistics ---\n", inet6_ntoa(*ip_2_ip6(&target_addr)));
  }
  McuLog_info("%lu packets transmitted, %lu received, %lu%% packet loss, time %lums\n",
         transmitted, received, loss, total_time_ms);
  // delete the ping sessions, so that we clean up all resources and can create a new ping session
  // we don't have to call delete function in the callback, instead we can call delete function from other tasks
  esp_ping_delete_session(hdl);
}
#endif

int McuPing_DoPingCmd(const char *host) {
  esp_ping_config_t config = ESP_PING_DEFAULT_CONFIG();

  /* parse IP address */
  struct sockaddr_in6 sock_addr6;
  ip_addr_t target_addr;
  const char *hostp = host; /* extra pointer for getaddrinfo() as it might change it */
  memset(&target_addr, 0, sizeof(target_addr));

  if (inet_pton(AF_INET6, hostp, &sock_addr6.sin6_addr) == 1) {
    /* convert ip6 string to ip6 address */
    ipaddr_aton(host, &target_addr);
  } else {
    struct addrinfo hint;
    struct addrinfo *res = NULL;
    memset(&hint, 0, sizeof(hint));
    /* convert ip4 string or hostname to ip4 or ip6 address */
    if (getaddrinfo(hostp, NULL, &hint, &res) != 0) {
      McuLog_info("ping: unknown host %s\n", host);
      return 1;
    }
    if (res->ai_family == AF_INET) {
      struct in_addr addr4 = ((struct sockaddr_in *) (res->ai_addr))->sin_addr;
      inet_addr_to_ip4addr(ip_2_ip4(&target_addr), &addr4);
    } else {
      struct in6_addr addr6 = ((struct sockaddr_in6 *) (res->ai_addr))->sin6_addr;
      inet6_addr_to_ip6addr(ip_2_ip6(&target_addr), &addr6);
    }
    freeaddrinfo(res);
  }
  config.target_addr = target_addr;

#if McuLib_CONFIG_CPU_IS_ESP32
  /* set callback functions */
  esp_ping_callbacks_t cbs = {
      .on_ping_success = cmd_ping_on_ping_success,
      .on_ping_timeout = cmd_ping_on_ping_timeout,
      .on_ping_end = cmd_ping_on_ping_end,
      .cb_args = NULL
  };
  esp_ping_handle_t ping;
  esp_ping_new_session(&config, &cbs, &ping);
  esp_ping_start(ping);
#endif
  return 0; /* ok */
}

void McuPing_Deinit(void) {
}

void McuPing_Init(void) {
}

#endif /* MCU_PING_CONFIG_ENABLED */
