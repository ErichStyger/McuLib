/*
 * Copyright (c) 2021-2026, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#if MCU_PING_CONFIG_ENABLED
#include "McuPing.h"
#if McuLib_CONFIG_CPU_IS_ESP32
  #include "sdkconfig.h"
#endif
#include "lwip/inet.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#if McuLib_CONFIG_CPU_IS_ESP32
  #include "esp_event.h"
  #include "nvs_flash.h"
  #include "ping/ping_sock.h"
#elif McuLib_CONFIG_CPU_IS_RPxxxx
  #include <errno.h>
  #include <string.h>

  #include "pico/stdlib.h"

  #include "lwip/inet.h"
  #include "lwip/netdb.h"
  #include "lwip/sockets.h"
  #include "lwip/icmp.h"
  #include "lwip/prot/icmp.h"
  #include "lwip/prot/ip4.h"
#endif
#include "McuLog.h"

#if McuLib_CONFIG_CPU_IS_ESP32
static void cmd_ping_on_ping_success(esp_ping_handle_t hdl, void *args) {
  uint8_t ttl;
  uint16_t seqno;
  uint32_t elapsed_time, recv_len;
  ip_addr_t target_addr;
  esp_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
  esp_ping_get_profile(hdl, ESP_PING_PROF_TTL, &ttl, sizeof(ttl));
  esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
  esp_ping_get_profile(hdl, ESP_PING_PROF_SIZE, &recv_len, sizeof(recv_len));
  esp_ping_get_profile(hdl, ESP_PING_PROF_TIMEGAP, &elapsed_time, sizeof(elapsed_time));
  McuLog_info("%lu bytes from %s icmp_seq=%d ttl=%d time=%lu ms",
         recv_len, inet_ntoa(target_addr.u_addr.ip4), seqno, ttl, elapsed_time);
}
#endif

#if McuLib_CONFIG_CPU_IS_ESP32
static void cmd_ping_on_ping_timeout(esp_ping_handle_t hdl, void *args) {
  uint16_t seqno;
  ip_addr_t target_addr;
  esp_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
  esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
  McuLog_info("From %s icmp_seq=%d timeout", inet_ntoa(target_addr.u_addr.ip4), seqno);
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
    McuLog_info("--- %s ping statistics ---", inet_ntoa(*ip_2_ip4(&target_addr)));
  } else {
    McuLog_info("--- %s ping statistics ---", inet6_ntoa(*ip_2_ip6(&target_addr)));
  }
  McuLog_info("%lu packets transmitted, %lu received, %lu%% packet loss, time %lums",
         transmitted, received, loss, total_time_ms);
  // delete the ping sessions, so that we clean up all resources and can create a new ping session
  // we don't have to call delete function in the callback, instead we can call delete function from other tasks
  esp_ping_delete_session(hdl);
}
#endif

#if McuLib_CONFIG_CPU_IS_ESP32
static int McuPing_DoPingEsp32(const char *host) {
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
      McuLog_info("ping: unknown host %s", host);
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
  return 0; /* ok */
}
#endif

#if McuLib_CONFIG_CPU_IS_RPxxxx

#define MCU_PING_COUNT          4u
#define MCU_PING_DATA_SIZE      32u
#define MCU_PING_TIMEOUT_MS     1000u
#define MCU_PING_INTERVAL_MS    1000u

typedef struct {
  struct icmp_echo_hdr header;
  uint8_t data[MCU_PING_DATA_SIZE];
} McuPing_IcmpPacket;

static uint16_t McuPing_Checksum(const void *data, size_t length) {
  const uint8_t *ptr = data;
  uint32_t sum = 0;

  while (length > 1u) {
    sum += ((uint16_t)ptr[0] << 8) | ptr[1];
    ptr += 2;
    length -= 2;
  }

  if (length != 0u) {
    sum += (uint16_t)ptr[0] << 8;
  }

  while ((sum >> 16) != 0u) {
    sum = (sum & 0xffffu) + (sum >> 16);
  }

  return (uint16_t)~sum;
}

static int McuPing_ResolveIPv4(const char *host,
                              struct sockaddr_in *destination) {
  struct addrinfo hints;
  struct addrinfo *result = NULL;
  int status;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_RAW;
  hints.ai_protocol = IPPROTO_ICMP;

  status = getaddrinfo(host, NULL, &hints, &result);
  if (status != 0 || result == NULL) {
    McuLog_info("ping: unknown host %s", host);
    return -1;
  }

  memcpy(destination, result->ai_addr, sizeof(*destination));
  destination->sin_port = 0;

  freeaddrinfo(result);
  return 0;
}

static int McuPing_SendIPv4(const char *host) {
  struct sockaddr_in destination;
  uint32_t transmitted = 0;
  uint32_t received = 0;
  uint32_t totalTimeMs = 0;
  int socketFd;

  if (McuPing_ResolveIPv4(host, &destination) != 0) {
    return 1;
  }

  socketFd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  if (socketFd < 0) {
    McuLog_error("ping: socket failed, errno=%", errno);
    return 2;
  }

  struct timeval timeout = {
    .tv_sec = MCU_PING_TIMEOUT_MS / 1000u,
    .tv_usec = (MCU_PING_TIMEOUT_MS % 1000u) * 1000u
  };

  if (setsockopt(socketFd,
                 SOL_SOCKET,
                 SO_RCVTIMEO,
                 &timeout,
                 sizeof(timeout)) < 0) {
    McuLog_error("ping: setsockopt failed, errno=%d", errno);
    closesocket(socketFd);
    return 3;
  }

  char destinationText[INET_ADDRSTRLEN];

  if (inet_ntop(AF_INET,
                &destination.sin_addr,
                destinationText,
                sizeof(destinationText)) == NULL) {
    strcpy(destinationText, "?");
  }

  McuLog_info("PING %s (%s): %u data bytes",
              host,
              destinationText,
              (unsigned)MCU_PING_DATA_SIZE);

  const uint16_t identifier =
      (uint16_t)((uintptr_t)&destination & 0xffffu);

  for (uint16_t sequence = 1;
       sequence <= MCU_PING_COUNT;
       sequence++) {
    McuPing_IcmpPacket request;

    memset(&request, 0, sizeof(request));

    ICMPH_TYPE_SET(&request.header, ICMP_ECHO);
    ICMPH_CODE_SET(&request.header, 0);

    request.header.id = lwip_htons(identifier);
    request.header.seqno = lwip_htons(sequence);

    for (size_t index = 0; index < sizeof(request.data); index++) {
      request.data[index] = (uint8_t)index;
    }

    request.header.chksum = 0;
    request.header.chksum =
        McuPing_Checksum(&request, sizeof(request));

    uint32_t startTimeMs = to_ms_since_boot(get_absolute_time());

    int sent = sendto(socketFd,
                      &request,
                      sizeof(request),
                      0,
                      (const struct sockaddr *)&destination,
                      sizeof(destination));

    if (sent < 0) {
      McuLog_error("ping: sendto failed, errno=%d", errno);
      continue;
    }

    transmitted++;

    uint8_t receiveBuffer[128];
    struct sockaddr_in source;
    socklen_t sourceLength = sizeof(source);

    int receiveLength = recvfrom(socketFd,
                                 receiveBuffer,
                                 sizeof(receiveBuffer),
                                 0,
                                 (struct sockaddr *)&source,
                                 &sourceLength);

    if (receiveLength < 0) {
      McuLog_info("From %s icmp_seq=%u timeout",
                  destinationText,
                  (unsigned)sequence);
    } else {
      /*
       * A raw IPv4 socket returns the IPv4 header followed by the
       * ICMP packet.
       */
      if ((size_t)receiveLength >= sizeof(struct ip_hdr)) {
        const struct ip_hdr *ipHeader =
            (const struct ip_hdr *)receiveBuffer;

        size_t ipHeaderLength = IPH_HL_BYTES(ipHeader);

        if (ipHeaderLength + sizeof(struct icmp_echo_hdr)
              <= (size_t)receiveLength) {
          const struct icmp_echo_hdr *reply =
              (const struct icmp_echo_hdr *)
                  &receiveBuffer[ipHeaderLength];

          if (ICMPH_TYPE(reply) == ICMP_ER &&
              reply->id == lwip_htons(identifier) &&
              reply->seqno == lwip_htons(sequence)) {
            uint32_t elapsedTimeMs =
                to_ms_since_boot(get_absolute_time()) -
                startTimeMs;

            uint32_t payloadLength =
                (uint32_t)receiveLength -
                (uint32_t)ipHeaderLength;

            received++;
            totalTimeMs += elapsedTimeMs;

            McuLog_info(
                "%lu bytes from %s icmp_seq=%u ttl=%u time=%lu ms",
                (unsigned long)payloadLength,
                destinationText,
                (unsigned)sequence,
                (unsigned)IPH_TTL(ipHeader),
                (unsigned long)elapsedTimeMs);
          }
        }
      }
    }

    if (sequence < MCU_PING_COUNT) {
      sleep_ms(MCU_PING_INTERVAL_MS);
    }
  }

  closesocket(socketFd);

  uint32_t loss =
      transmitted == 0u
          ? 100u
          : ((transmitted - received) * 100u) / transmitted;

  McuLog_info(
    "--- %s ping statistics ---"
    "%lu packets transmitted, %lu received, "
    "%lu%% packet loss, time %lums",
    destinationText,
    (unsigned long)transmitted,
    (unsigned long)received,
    (unsigned long)loss,
    (unsigned long)totalTimeMs
  );
  return received != 0u ? 0 : 4;
}
#endif /* McuLib_CONFIG_CPU_IS_RPxxxx */

int McuPing_DoPingCmd(const char *host) {
  if (host == NULL || *host == '\0') {
    return -1;
  }
#if McuLib_CONFIG_CPU_IS_ESP32
  return McuPing_DoPingEsp32(host);
#elif McuLib_CONFIG_CPU_IS_RPxxxx
  return McuPing_SendIPv4(host);
#else
  #error "McuPing is not implemented for this platform"
#endif
}

void McuPing_Deinit(void) {
}

void McuPing_Init(void) {
}

#endif /* MCU_PING_CONFIG_ENABLED */
