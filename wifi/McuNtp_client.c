/*
 * Copyright (c) 2021-2025, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#if MCU_NTP_CLIENT_CONFIG_ENABLED
#if McuLib_CONFIG_CPU_IS_RPxxxx
  #include "pico/stdlib.h"
  #include "pico/cyw43_arch.h"
#endif
#include "McuNtp_client.h"
#include "lwip/udp.h"
#include "McuShell.h"
#include "McuRTOS.h"
#include "McuTimeDate.h"
#include "McuUtility.h"
#include "McuLog.h"
#include "McuDns_resolver.h"

#if MCU_NTP_CLIENT_CONFIG_USE_MININI
  #include "minIni/McuMinINI.h"
#endif

#if !MCU_DNS_RESOLVER_CONFIG_ENABLED
  #error "the McuNtp_client module currently depends on DNS resolver available, please enable it."
#endif

static TaskHandle_t taskHandle; /* task handle of ntp task */

typedef struct ntp_desc_t {
  McuDnsResolver_info_t addr;       /* DNS resolved address information of DNS server */
  struct udp_pcb *ntp_pcb;          /* UDP protocol block */
  TimerHandle_t ntp_timeout_timer;  /* timer for timeout */
  bool gotNtpResponse;              /* flag which tells us if we got a response */
} ntp_desc_t;

#define NTP_SERVER          "pool.ntp.org"  /* ntp server name */
#define NTP_MSG_LEN         48
#define NTP_PORT            123             /* ntp udp port number */
#define NTP_DELTA           2208988800      /* seconds between 1 Jan 1900 and 1 Jan 1970 */
#define NTP_PERIOD_TIME_MS  (30*60*1000)    /* period in ms between ntp requests */
#define NTP_TIMEOUT_TIME_MS (10*1000)       /* timeout time in ms if there is no ntp response */

/* Called with results of operation */
static void ntp_result(ntp_desc_t *state, int status, time_t *result) {
  if (status==0 && result!=NULL) {
    struct tm *utc = gmtime(result);
    McuLog_info("got ntp response: %02d/%02d/%04d %02d:%02d:%02d",
        utc->tm_mday, utc->tm_mon + 1, utc->tm_year + 1900,
        utc->tm_hour, utc->tm_min, utc->tm_sec);
    McuTimeDate_SetDate(utc->tm_year+1900, utc->tm_mon+1, utc->tm_mday);
    McuTimeDate_SetTime(utc->tm_hour+1, utc->tm_min, utc->tm_sec, 0); /* +1 for GMT+1 */
    state->gotNtpResponse = true; /* mark that we got a response */
    if (state->ntp_timeout_timer !=NULL) {
      /* received a good resonse, stopping the timer */
      xTimerStop(state->ntp_timeout_timer, pdMS_TO_TICKS(100));
    }
  }
}

static void vTimerCallback(TimerHandle_t pxTimer) {
  ntp_desc_t *state = (ntp_desc_t*)pvTimerGetTimerID(pxTimer);
  McuLog_error("ntp request timeout");
  ntp_result(state, -1, NULL); /* abort */
  xTimerStop(pxTimer, pdMS_TO_TICKS(100)); /* stop timer */
}

/* send an UDP NTP request message */
static void ntp_request(ntp_desc_t *state) {
#if McuLib_CONFIG_CPU_IS_RPxxxx
  // cyw43_arch_lwip_begin/end should be used around calls into lwIP to ensure correct locking.
  // You can omit them if you are in a callback from lwIP. Note that when using pico_cyw_arch_poll
  // these calls are a no-op and can be omitted, but it is a good practice to use them in
  // case you switch the cyw43_arch type later.
  cyw43_arch_lwip_begin();
#endif
  state->gotNtpResponse = false; /* reset response flag */
  struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, NTP_MSG_LEN, PBUF_RAM);
  uint8_t *req = (uint8_t *)p->payload;
  memset(req, 0, NTP_MSG_LEN);
  req[0] = 0x1b;
  udp_sendto(state->ntp_pcb, p, &state->addr.resolved_addr, NTP_PORT);
  pbuf_free(p);
#if McuLib_CONFIG_CPU_IS_RPxxxx
  cyw43_arch_lwip_end();
#endif
}

/* lwIP callback for NTP data received */
static void ntp_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr, u16_t port) {
  ntp_desc_t *state = (ntp_desc_t*)arg;
  uint8_t mode = pbuf_get_at(p, 0) & 0x7;
  uint8_t stratum = pbuf_get_at(p, 1);

  /* Check the result */
  if (ip_addr_cmp(addr, &state->addr.resolved_addr) && port == NTP_PORT && p->tot_len == NTP_MSG_LEN && mode == 0x4 && stratum != 0) {
    uint8_t seconds_buf[4] = {0};
    pbuf_copy_partial(p, seconds_buf, sizeof(seconds_buf), 40);
    uint32_t seconds_since_1900 = seconds_buf[0] << 24 | seconds_buf[1] << 16 | seconds_buf[2] << 8 | seconds_buf[3];
    uint32_t seconds_since_1970 = seconds_since_1900 - NTP_DELTA;
    time_t epoch = seconds_since_1970;
    ntp_result(state, 0, &epoch);
  } else {
    McuLog_error("invalid ntp response");
    ntp_result(state, -1, NULL); /* abort */
  }
  pbuf_free(p);
}

static int ntp_init(ntp_desc_t *ntp_state) {
  memset(ntp_state, 0, sizeof(ntp_desc_t));
  ntp_state->ntp_pcb = udp_new_ip_type(IPADDR_TYPE_ANY); /* setup udp protocol block */
  if (!ntp_state->ntp_pcb) {
    McuLog_fatal("failed to create udp pcb");
    return -1; /* failed */
  }
  /* create timeout timer */
  ntp_state->ntp_timeout_timer = xTimerCreate(
      "ntp_timer", /* name */
      pdMS_TO_TICKS(NTP_TIMEOUT_TIME_MS), /* period/time */
      pdTRUE, /* auto reload */
      (void*)0, /* timer ID */
      vTimerCallback); /* callback */
  if (ntp_state->ntp_timeout_timer==NULL) {
    McuLog_fatal("failed creating ntp timer");
    return -1;
  }
  vTimerSetTimerID(ntp_state->ntp_timeout_timer, ntp_state); /* ntp_state is used in timer callback as data pointer */
  udp_recv(ntp_state->ntp_pcb, ntp_recv, ntp_state); /* set receive callback */
  return 0; /* ok */
}

static struct ntp {
  bool start; /* if start task automatically at boot time, otherwise task remains suspended after startup. Setting stored in minINI */
  bool taskIsSuspended; /* if task is suspended or not */
} ntp;

bool McuNtpClient_GetDefaultStart(void) {
  return ntp.start;
}

static uint8_t SetDefaultStart(bool start) {
#if MCU_NTP_CLIENT_CONFIG_USE_MININI
  if (McuMinINI_ini_putl(MCU_NTP_CLIENT_CONFIG_MININI_SECTION, MCU_NTP_CLIENT_CONFIG_MININI_KEY_START, start, MCU_NTP_CLIENT_CONFIG_MININI_FILE_NAME)!=1) { /* 1: success */
    return ERR_FAILED;
  }
#endif
  ntp.start = start;
  return ERR_OK;
}

static void ntpTask(void *pv) {
  ntp_desc_t ntp_state;

#define NTP_DEFAULT_START   true
#if MCU_NTP_CLIENT_CONFIG_USE_MININI
  ntp.start = McuMinINI_ini_getbool(MCU_NTP_CLIENT_CONFIG_MININI_SECTION, MCU_NTP_CLIENT_CONFIG_MININI_KEY_START, NTP_DEFAULT_START, MCU_NTP_CLIENT_CONFIG_MININI_FILE_NAME);
#else
  ntp.start = NTP_DEFAULT_START;
#endif
  McuNtpClient_TaskSuspend(); /* will be resumed by WiFi task */
  if (ntp_init(&ntp_state)!=0) {
    McuLog_fatal("failed initializing ntp");
    for(;;) {
      vTaskSuspend(NULL);
    }
  }
  /* resolve NTP server name */
  while (McuDnsResolver_ResolveName(NTP_SERVER, &ntp_state.addr, 5*1000)!=0) {
    McuLog_error("dns request for '%s' failed", NTP_SERVER);
    vTaskDelay(pdMS_TO_TICKS(30*1000));
  } /* retry until success */
  for(;;) {
    for(int i=0; i<3; i++) {
      McuLog_info("requesting time/date from NTP server, try #%d", i);
      /* Set timeout alarm in case udp requests are lost */
      xTimerStart(ntp_state.ntp_timeout_timer, pdMS_TO_TICKS(100)); /* timer will be stopped if we receive a NTP response */
      ntp_request(&ntp_state);
      do {
        vTaskDelay(pdMS_TO_TICKS(100)); /* wait for response or timeout */
      } while(xTimerIsTimerActive(ntp_state.ntp_timeout_timer)); /* request still going on, timeout not expired? */
      if (ntp_state.gotNtpResponse) {
        break; /* get out of retry loop */
      }
    } /* retry */
    vTaskDelay(pdMS_TO_TICKS(NTP_PERIOD_TIME_MS)); /* delay until next periodic ntp request is up */
  }
}

void McuNtpClient_TaskResume(void) {
  if (taskHandle!=NULL) {
    vTaskResume(taskHandle);
    ntp.taskIsSuspended = false;
  }
}

void McuNtpClient_TaskSuspend(void) {
  if (taskHandle!=NULL) {
    ntp.taskIsSuspended = true;
    vTaskSuspend(taskHandle);
  }
}

#if MCU_NTP_CLIENT_CONFIG_USE_SHELL
static uint8_t PrintStatus(const McuShell_StdIOType *io) {
  McuShell_SendStatusStr((unsigned char*)"ntpclient", (unsigned char*)"NTP client status\r\n", io->stdOut);
  McuShell_SendStatusStr((unsigned char*)"  start", ntp.start?(unsigned char*)"true\r\n":(unsigned char*)"false\r\n", io->stdOut);
  McuShell_SendStatusStr((unsigned char*)"  suspended", ntp.taskIsSuspended?(unsigned char*)"true\r\n":(unsigned char*)"false\r\n", io->stdOut);
  return ERR_OK;
}
#endif /* MCU_NTP_CLIENT_CONFIG_USE_SHELL */

#if MCU_NTP_CLIENT_CONFIG_USE_SHELL
static uint8_t PrintHelp(const McuShell_StdIOType *io) {
  McuShell_SendHelpStr((unsigned char*)"ntpclient", (unsigned char*)"Group of ntpclient commands\r\n", io->stdOut);
  McuShell_SendHelpStr((unsigned char*)"  help|status", (unsigned char*)"Print help or status information\r\n", io->stdOut);
  McuShell_SendHelpStr((unsigned char*)"  start yes|no", (unsigned char*)"If ntpclient task shall start at boot time\r\n", io->stdOut);
  McuShell_SendHelpStr((unsigned char*)"  suspend|resume", (unsigned char*)"Suspend or resume ntp client task\r\n", io->stdOut);
  return ERR_OK;
}
#endif /* MCU_NTP_CLIENT_CONFIG_USE_SHELL */

#if MCU_NTP_CLIENT_CONFIG_USE_SHELL
uint8_t McuNtpClient_ParseCommand(const unsigned char *cmd, bool *handled, const McuShell_StdIOType *io) {
  if (McuUtility_strcmp((char*)cmd, McuShell_CMD_HELP)==0 || McuUtility_strcmp((char*)cmd, "ntpclient help")==0) {
    *handled = true;
    return PrintHelp(io);
  } else if ((McuUtility_strcmp((char*)cmd, McuShell_CMD_STATUS)==0) || (McuUtility_strcmp((char*)cmd, "ntpclient status")==0)) {
    *handled = true;
    return PrintStatus(io);
  } else if (McuUtility_strcmp((char*)cmd, "ntpclient resume")==0) {
    *handled = true;
    McuNtpClient_TaskResume();
    return ERR_OK;
  } else if (McuUtility_strcmp((char*)cmd, "ntpclient suspend")==0) {
    *handled = true;
    McuNtpClient_TaskSuspend();
    return ERR_OK;
  } else if (McuUtility_strcmp((char*)cmd, "ntpclient start yes")==0) {
    *handled = true;
    return SetDefaultStart(true);
  } else if (McuUtility_strcmp((char*)cmd, "ntpclient start no")==0) {
    *handled = true;
    return SetDefaultStart(false);
  }
  return ERR_OK;
}
#endif /* MCU_NTP_CLIENT_CONFIG_USE_SHELL */

void McuNtpClient_Init(void) {
  if (xTaskCreate(
      ntpTask,  /* pointer to the task */
      "ntp", /* task name for kernel awareness debugging */
      4096/sizeof(StackType_t), /* task stack size */
      (void*)NULL, /* optional task startup argument */
      tskIDLE_PRIORITY+1,  /* initial priority */
      &taskHandle /* optional task handle to create */
    ) != pdPASS)
  {
    McuLog_fatal("failed creating task");
    for(;;){} /* error! probably out of memory */
  }
}
#endif /* MCU_NTP_CLIENT_CONFIG_ENABLED */
