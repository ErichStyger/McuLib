/*
 * Copyright (c) 2023, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "McuUdpServer.h"
#if MCU_UDP_SERVER_CONFIG_ENABLED
#include "lwip/sockets.h"
#include "McuLib.h"
#include "McuRTOS.h"
#include "McuLog.h"
#include "McuUtility.h"
#include "shell.h"

#define CONFIG_EXAMPLE_IPV4  (1) /* 0: use IPV6; 1: use IPV4 */

static TaskHandle_t taskHandle = NULL; /* udp server task handle */
static void (*McuUdpServer_IncomingCallback)(const char *rxBuffer, int rxLen, char *responseBuf, size_t responseBufLen, size_t *responseSize); /* optional application callback for incoming data */

void McuUdpServer_SetIncomingCallback(void (*callback)(const char *rxBuffer, int rxLen, char *responseBuf, size_t responseBufLen, size_t *responseSize)) {
  McuUdpServer_IncomingCallback = callback;
}

static void HandleIncomingUdpMessage(const char *rxBuffer, int rxLen, int sock, struct sockaddr *source_addr_p, socklen_t source_addr_len) {
  char responseBuf[MCU_UDP_SERVER_CONFIG_RESPONSE_BUF_SIZE] = ""; /* buffer for response */
  size_t responseLen = 0;

  if (McuUdpServer_IncomingCallback!=NULL) { /* only if we have a user callback */
    McuUdpServer_IncomingCallback(rxBuffer, rxLen, responseBuf, sizeof(responseBuf), &responseLen); /* call user callback */
  } else {
    McuUtility_strcpy((unsigned char*)responseBuf, sizeof(responseBuf), (unsigned char*)"OK"); /* default response */
    responseLen = McuUtility_strlen(responseBuf);
  }
  if (responseLen>0) {
    if (responseBuf[responseLen]=='\0') { /* can use as string? */
      McuLog_info("Sending back response string, %d bytes, \"%s\"", responseLen, responseBuf);
    } else {
      McuLog_info("Sending back response, %d bytes", responseLen);
    }
    int err = sendto(sock, responseBuf, responseLen, 0, source_addr_p, source_addr_len);
    if (err < 0) {
      McuLog_error("Error occurred during sending response: errno %d", errno);
    }
  }
}

static void udp_server_task(void *pvParameters) {
  static char rx_buffer[MCU_UDP_SERVER_CONFIG_RX_BUF_SIZE];
  char addr_str[128];
  int addr_family;
  int ip_protocol;

  vTaskSuspend(NULL); /* UDP_Server_Start() will wake me up */
  for(;;) {
#ifdef CONFIG_EXAMPLE_IPV4
    struct sockaddr_in dest_addr;

    dest_addr.sin_addr.s_addr = htonl(INADDR_ANY); /** 0.0.0.0 */
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(MCU_UDP_SERVER_CONFIG_PORT);
    addr_family = AF_INET;
    ip_protocol = IPPROTO_IP;
    inet_ntoa_r(dest_addr.sin_addr, addr_str, sizeof(addr_str) - 1);
#else /* IPV6 */
    struct sockaddr_in6 dest_addr;
    bzero(&dest_addr.sin6_addr.un, sizeof(dest_addr.sin6_addr.un));
    dest_addr.sin6_family = AF_INET6;
    dest_addr.sin6_port = htons(MCU_UDP_SERVER_CONFIG_PORT);PO
    addr_family = AF_INET6;
    ip_protocol = IPPROTO_IPV6;
    inet6_ntoa_r(dest_addr.sin6_addr, addr_str, sizeof(addr_str) - 1);
#endif

    int sock = socket(addr_family, SOCK_DGRAM, ip_protocol);
    if (sock < 0) {
      McuLog_error("Unable to create socket: errno %d", errno);
      break;
    }
    McuLog_info("Socket created");
#if 0 /* optional timeout */
    /* set a timeout for the socket */
    struct timeval to;

    to.tv_sec = 5;
    to.tv_usec = 0;
    McuLog_info("setting socket timeout to %d sec", to.tv_sec);
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &to, sizeof(to)) < 0) {
      McuLog_error("setting socket timeout failed");
    }
#endif
    int err = bind(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err < 0) {
      McuLog_error("Socket unable to bind: errno %d", errno);
    }
    McuLog_info("Socket bound, port %d", MCU_UDP_SERVER_CONFIG_PORT);
    while (1) {
      McuLog_info("Waiting for data on port %d", MCU_UDP_SERVER_CONFIG_PORT);
    #if LWIP_IPV6
      struct sockaddr_in6 source_addr; /* Large enough for both IPv4 or IPv6 */
    #else 
      struct sockaddr source_addr; /* IP4 only */
    #endif
      socklen_t socklen = sizeof(source_addr);

      /* receive data (blocking): */
      int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer)-1, 0, (struct sockaddr *)&source_addr, &socklen);

      /* Error occurred during receiving */
      if (len < 0) {
        McuLog_error("recvfrom failed: errno %d", errno);
        break;
      } else { /* Data received */
        /* Get the sender's ip address as string */
      #if LWIP_IPV6
        if (source_addr.sin6_family == PF_INET) {
          inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr.s_addr, addr_str, sizeof(addr_str) - 1);
        } else if (source_addr.sin6_family == PF_INET6) {
          inet6_ntoa_r(source_addr.sin6_addr, addr_str, sizeof(addr_str) - 1);
        }
      #else
        if (source_addr.sa_family == PF_INET) {
          inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr.s_addr, addr_str, sizeof(addr_str) - 1);
        }
      #endif
        McuLog_info("Received %d bytes from %s:\n", len, addr_str);
        HandleIncomingUdpMessage(rx_buffer, len, sock, (struct sockaddr *)&source_addr, sizeof(source_addr));
      } /* if */
    } /* while */
    if (sock != -1) {
      McuLog_error("Shutting down socket and restarting...");
      shutdown(sock, 0);
      close(sock);
    }
  } /* for */
  vTaskDelete(NULL);
}

void McuUdpServer_Resume(void) {
  if (taskHandle!=NULL) {
    vTaskResume(taskHandle);
  }
}

void McuUdpServer_Suspend(void) {
  if (taskHandle!=NULL) {
    vTaskSuspend(taskHandle);
  }
}

void McuUdpServer_Init(void) {
  if (xTaskCreate(
      udp_server_task,  /* pointer to the task */
      "UdpServer", /* task name for kernel awareness debugging */
      (MCU_UDP_SERVER_CONFIG_TASK_STACK_SIZE)/sizeof(StackType_t), /* task stack size */
      (void*)NULL, /* optional task startup argument */
      (MCU_UDP_SERVER_CONFIG_TASK_PRIORITY),  /* initial priority */
      &taskHandle /* optional task handle to create */
    ) != pdPASS)
  {
    McuLog_fatal("failed creating task");
    for(;;){} /* error! probably out of memory */
  }
}
#endif /* MCU_UDP_SERVER_CONFIG_ENABLED */
