/* BSD Socket API Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "McuUdpClient_config.h"
#if MCU_UDP_CLIENT_CONFIG_ENABLED
#include "lwip/sockets.h"
#include "McuDnsResolver.h"
#include "McuUdpClient.h"
#include "McuRTOS.h"
#include "McuLog.h"
#include "McuUtility.h"

uint8_t McuUdpClient_Send(const char *host, uint16_t port, const char *msg, char *rxBuffer, size_t rxBufferSize) {
  char addr_str[128];
  int addr_family;
  int ip_protocol;
  uint8_t res = ERR_OK;

   /* resolve host name to IP address: */
  int nofRetry;
  McuDnsResolver_info_t addr; 
  for (nofRetry=5; nofRetry>=0; nofRetry--) {
    if (McuDnsResolver_ResolveName((char*)host, &addr, 5*1000)!=0) { /* use DNS to resolve name to IP address */
      McuLog_error("failed to resolve udp server name '%s', retry ...", host);
      vTaskDelay(pdMS_TO_TICKS(5000));
    } else {
      break; /* success! leaving loop */
    }
  }
  if (nofRetry<0) {
    McuLog_fatal("failed to udp server name '%s', giving up", host);
    return ERR_FAILED;
  }

  struct sockaddr_in dest_addr;
  if (IP_IS_V4(&addr.resolved_addr)) {
    dest_addr.sin_addr.s_addr = ip_2_ip4(&addr.resolved_addr)->addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);
    addr_family = AF_INET;
    ip_protocol = IPPROTO_IP;
    inet_ntoa_r(dest_addr.sin_addr, addr_str, sizeof(addr_str) - 1);
  } else {
    McuLog_error("Only IPv4 addresses supported");
    return ERR_FAILED;
  }

  int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);

  for(;;) { /* breaks in in case of error */
    if (sock < 0) {
      McuLog_error("Unable to create socket: errno %d", errno);
      res = ERR_FAILED;
      break;
    }
    McuLog_info("Socket created, sending to %s:%d", host, port);

  #if LWIP_SO_RCVTIMEO /* set a timeout for the socket if feature is present in lwIP, might need to configure lwipopts.h */
    /* Linux or the patched ESP32 lwip can use a timeval struct */
    struct timeval to;
    to.tv_sec = 5; /* socket receive timeout */
    to.tv_usec = 0;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &to, sizeof(to)) < 0) {
      McuLog_error("setting socket timeout failed");
      res = ERR_FAILED;
      break;
    }
   #endif

    int err = sendto(sock, msg, strlen((char*)msg), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err < 0) {
      McuLog_error("Error occurred during sending: errno %d", errno);
      res = ERR_FAILED;
      break;
    }
    McuLog_info("Message sent, Waiting for response");
    struct sockaddr_in source_addr; /* Large enough for both IPv4 or IPv6 */
    socklen_t socklen = sizeof(source_addr);
    int len = recvfrom(sock, rxBuffer, rxBufferSize-1, 0, (struct sockaddr *)&source_addr, &socklen);

    /* Error occurred during receiving */
    if (len < 0) {
      McuLog_error("recvfrom failed: errno %d", errno);
      res = ERR_FAILED;
      break;
    } else { /* Data received */
      rxBuffer[len] = '\0'; /* Null-terminate whatever we received and treat like a string */
      McuLog_info("Received %d bytes from %s: '%s'", len, addr_str, rxBuffer);
    }
    break;
  } /* for */
  if (sock != -1) {
    McuLog_info("Shutting down socket");
    shutdown(sock, 0);
    close(sock);
  }
  return res;
}

void McuUdpClient_DeInit(void) {
  /* nothing to do */
}

void McuUdpClient_Init(void) {
  /* nothing to do */
}

#endif /* MCU_UDP_CLIENT_CONFIG_ENABLED */
