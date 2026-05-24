/*
 * Copyright (c) 2023-2026, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Useful read: https://github.com/particle-iot/lwip/blob/master/doc/mqtt_client.txt
 */

#if MCU_MQTT_CLIENT_CONFIG_ENABLED
#if McuLib_CONFIG_CPU_IS_RPxxxx
  #include "pico/cyw43_arch.h"
#endif
#include "lwip/apps/mqtt.h"
#include "lwip/dns.h"
#include "McuMqtt_client_config.h"
#include "McuMqtt_client.h"
#include "McuDns_resolver.h"
#include "McuLog.h"
#include "McuUtility.h"
#include "McuRTOS.h"
#if MCU_MQTT_CLIENT_CONFIG_USE_MININI
  #include "minIni/McuMinINI.h"
#endif

#if !MCU_DNS_RESOLVER_CONFIG_ENABLED
  #error "This module currently depends on DNS resolver available, please enable it."
#endif

#if LWIP_TCP

typedef struct mqtt_t {
  mqtt_client_t *mqtt_client;       /* lwIP MQTT client handle */
  bool isConnected;                 /* if we are connected or not */
  McuDnsResolver_info_t addr;          /* broker lwip address, resolved by DNS if hostname is used */
  unsigned char broker[32];         /* broker IP or hostname string. For hostname, DNS will be used */
  unsigned char client_id[32];      /* client ID used for connection: each client should have a unique ID */
  unsigned char client_user[32];    /* client user name used for connection */
  unsigned char client_pass[96];    /* client user password */
  int in_pub_ID;             /* incoming published ID, set in the incoming_publish_cb and used in the incoming_data_cb */
  /* configuration settings */
  bool doLogging; /* if it shall write log messages */
  bool doPublishing; /* if publish the topics */
  bool reconnect; /* if after a disconnect it shall try to reconnect */
  mqtt_connection_cb_t conn_cb;
  mqtt_incoming_data_cb_t data_cb;
  mqtt_incoming_publish_cb_t pub_cb;
} mqtt_t;

static mqtt_t mqtt; /* information used for MQTT connection */

static const struct mqtt_connect_client_info_t mqtt_client_info = {
  (const char*)mqtt.client_id, /* client ID */
  (const char*)mqtt.client_user, /* client user name */
  (const char*)mqtt.client_pass, /* client password */
  100,  /* keep alive timeout in seconds */
  NULL, /* will_topic */
  NULL, /* will_msg */
  0,    /* will_qos */
  0     /* will_retain */
#if LWIP_ALTCP && LWIP_ALTCP_TLS
  , NULL
#endif
};

void McuMqttClient_SetCallbacks(
  void (conn_cb)(void *client, void *arg, int status),
  void (data_cb) (void *arg, const uint8_t *data, uint16_t len, uint8_t flags),
  void (pub_cb) (void *arg, const char *topic, uint32_t tot_len)
) {
  mqtt.conn_cb = (mqtt_connection_cb_t)conn_cb;
  mqtt.data_cb = (mqtt_incoming_data_cb_t)data_cb;
  mqtt.pub_cb = (mqtt_incoming_publish_cb_t)pub_cb;
}

static void McuMqttClient_publish_request_cb(void *arg, err_t err) {
#if 0 && MQTT_CLIENT_CONFIG_EXTRA_LOGS /* be careful not to delay callback too much */
  const struct mqtt_connect_client_info_t *client_info = (const struct mqtt_connect_client_info_t*)arg;
  McuLog_trace("MQTT client \"%s\" publish request cb: err %d", client_info->client_id, (int)err);
#endif
}

uint8_t McuMqttClient_PublishText(const char *topic, const char *text) {
  err_t res;
  const uint8_t qos = 0; /* quos: 0: fire&forget, 1: at least once */
  const uint8_t retain = 0; /* 0: do not retain. 1: Store this message as the last known value for this topic. */

  res = mqtt_publish(McuMqttClient_getClient(), topic, text, strlen(text), qos, retain, McuMqttClient_publish_request_cb, NULL);
  if (res!=ERR_OK) {
    McuLog_error("Failed topic %s mqtt_publish: %d", topic, res);
    (void)McuMqttClient_Disconnect(); /* try disconnect and connect again */
    (void)McuMqttClient_Connect();
    return ERR_FAILED;
  }
  if (McuMqttClient_doLogging()) {
    McuLog_trace("published %s %s", topic, text);
  }
  return ERR_OK;
}

uint8_t McuMqttClient_PublishSwitch(const char *topic, bool isOn, bool asJSON) {
  uint8_t buf[64];

  buf[0] = '\0';
  if (asJSON) {
    McuUtility_strcat(buf, sizeof(buf), (unsigned char*)"{\"state\":");
    if (isOn) {
      McuUtility_strcat(buf, sizeof(buf), (unsigned char*)"\"ON\"");
    } else {
      McuUtility_strcat(buf, sizeof(buf), (unsigned char*)"\"OFF\"");
    }
    McuUtility_strcat(buf, sizeof(buf), (unsigned char*)"}");
  } else {
    if (isOn) {
      McuUtility_strcat(buf, sizeof(buf), (unsigned char*)"ON");
    } else {
      McuUtility_strcat(buf, sizeof(buf), (unsigned char*)"OFF");
    }    
  }
  return McuMqttClient_PublishText(topic, buf);
}

void McuMqttClient_IncomingSwitch(const uint8_t *data, uint16_t len, const char *logMsg, void (setter)(bool)) {
  unsigned char buf[32];

  McuMqttClient_GetDataString(buf, sizeof(buf), data, len);
  if (logMsg!=NULL && McuMqttClient_doLogging()) {
    McuLog_trace("Rx: %s: %s", logMsg, buf);
  }
  if (McuUtility_strcmp(buf, "ON")==0) {
    if (setter!=NULL) {
      setter(true);
    }
  } else if(McuUtility_strcmp(buf, "OFF")==0) {
    if (setter!=NULL) {
      setter(false);
    }
  }
}

uint8_t McuMqttClient_PublishTime(const char *topic, uint8_t hours, uint8_t minutes, bool asJSON) {
  uint8_t buf[64];

  if (asJSON) {
    McuUtility_strcpy(buf, sizeof(buf), (unsigned char*)"{\"time\": \"");
    McuUtility_strcatNum16uFormatted(buf, sizeof(buf), hours, '0', 2);
    McuUtility_chcat(buf, sizeof(buf), ':');
    McuUtility_strcatNum16uFormatted(buf, sizeof(buf), minutes, '0', 2);
    McuUtility_strcat(buf, sizeof(buf), (unsigned char*)"\"}");
  } else {
    /* NYI */
    return ERR_FAILED;
  }
  return McuMqttClient_PublishText(topic, buf);
}

void McuMqttClient_IncomingTime(const uint8_t *data, uint16_t len, const char *logMsg, void (setter)(uint8_t, uint8_t)) {
  unsigned char buf[32];
  
  McuMqttClient_GetDataString(buf, sizeof(buf), data, len);
  if (logMsg!=NULL && McuMqttClient_doLogging()) {
    McuLog_trace("Rx: %s: %s", logMsg, buf); /* e.g. "16:15" */
  }
  const unsigned char *p = buf;
  uint8_t hour, minute, second, hsecond;
  if (McuUtility_ScanTime(&p, &hour, &minute, &second, &hsecond)==ERR_OK) {
    if (setter!=NULL) {
      setter(hour, minute);
    }
  }
}

uint8_t McuMqttClient_PublishTemperature(const char *topic, float temperature, bool isJSON) {
  uint8_t buf[64];
 
  if (isJSON) {
    McuUtility_strcpy(buf, sizeof(buf), (unsigned char*)"{\"temperature\": ");
    McuUtility_strcatNumFloat(buf, sizeof(buf), temperature, 2);
    McuUtility_strcat(buf, sizeof(buf), (unsigned char*)", \"unit\": \"°C\"}");
  } else {
    return ERR_FAILED; /* NYI */
  }
  return McuMqttClient_PublishText(topic, buf);
}

void McuMqttClient_IncomingTemperature(const uint8_t *data, uint16_t len, const char *logMsg, void (setter)(float)) {
  unsigned char buf[32];
  
  McuMqttClient_GetDataString(buf, sizeof(buf), data, len);
  if (logMsg!=NULL && McuMqttClient_doLogging()) {
    McuLog_trace("Rx: %s: %s", logMsg, buf); /* e.g. "25.5" */
  }

  int32_t integral;
  uint32_t fractional;
  uint8_t nofFractionalZeros;
  const unsigned char *p = buf;

  if (McuUtility_ScanDecimal32sDotNumber(&p, &integral, &fractional, &nofFractionalZeros)!=ERR_OK) {
    McuLog_error("failed parsing data '%s'", buf);
    return;
  }
  if (integral<0) { /* only positive falues allowed */
    McuLog_error("only positive values allowed: '%s'", buf);
    return;
  }
  if (fractional!=0 && nofFractionalZeros>0) { /* do not accept fractional zeros */
    McuLog_error("only in 0.5 degree steps: '%s'", buf);
    return;
  }
  if (setter!=NULL) {
    setter((float)integral + (float)fractional*0.1f);
  }
}

static void reloadSettings(void) {
#if MCU_MQTT_CLIENT_CONFIG_USE_MININI
  McuMinINI_ini_gets(MCU_MQTT_CLIENT_MININI_SECTION_MQTT, MCU_MQTT_CLIENT_MININI_KEY_MQTT_BROKER, MCU_MQTT_CLIENT_CONFIG_BROKER, (char*)mqtt.broker, sizeof(mqtt.broker), MCU_MQTT_CLIENT_CONFIG_MININI_FILE_NAME);
  McuMinINI_ini_gets(MCU_MQTT_CLIENT_MININI_SECTION_MQTT, MCU_MQTT_CLIENT_MININI_KEY_MQTT_CLIENT, MCU_MQTT_CLIENT_CONFIG_CLIENT, (char*)mqtt.client_id, sizeof(mqtt.client_id), MCU_MQTT_CLIENT_CONFIG_MININI_FILE_NAME);
  McuMinINI_ini_gets(MCU_MQTT_CLIENT_MININI_SECTION_MQTT, MCU_MQTT_CLIENT_MININI_KEY_MQTT_USER, MCU_MQTT_CLIENT_CONFIG_USER, (char*)mqtt.client_user, sizeof(mqtt.client_user), MCU_MQTT_CLIENT_CONFIG_MININI_FILE_NAME);
  McuMinINI_ini_gets(MCU_MQTT_CLIENT_MININI_SECTION_MQTT, MCU_MQTT_CLIENT_MININI_KEY_MQTT_PASS, MCU_MQTT_CLIENT_CONFIG_PASS, (char*)mqtt.client_pass, sizeof(mqtt.client_pass), MCU_MQTT_CLIENT_CONFIG_MININI_FILE_NAME);
  mqtt.doPublishing = McuMinINI_ini_getbool(MCU_MQTT_CLIENT_MININI_SECTION_MQTT, MCU_MQTT_CLIENT_MININI_KEY_MQTT_PUBLISH, MCU_MQTT_CLIENT_CONFIG_PUBLISH, MCU_MQTT_CLIENT_CONFIG_MININI_FILE_NAME);
  mqtt.reconnect = McuMinINI_ini_getbool(MCU_MQTT_CLIENT_MININI_SECTION_MQTT, MCU_MQTT_CLIENT_MININI_KEY_MQTT_RECONNECT, MCU_MQTT_CLIENT_CONFIG_RECONNECT, MCU_MQTT_CLIENT_CONFIG_MININI_FILE_NAME);
#else
  McuUtility_strcpy(mqtt.broker, sizeof(mqtt.broker), (unsigned char*)MCU_MQTT_CLIENT_CONFIG_BROKER);
  McuUtility_strcpy(mqtt.client_id, sizeof(mqtt.client_id), (unsigned char*)MCU_MQTT_CLIENT_CONFIG_CLIENT);
  McuUtility_strcpy(mqtt.client_user, sizeof(mqtt.client_user), (unsigned char*)MCU_MQTT_CLIENT_CONFIG_USER);
  McuUtility_strcpy(mqtt.client_pass, sizeof(mqtt.client_pass), (unsigned char*)MCU_MQTT_CLIENT_CONFIG_PASS);
  mqtt.doPublishing = MCU_MQTT_CLIENT_CONFIG_PUBLISH;
  mqtt.reconnect = MCU_MQTT_CLIENT_CONFIG_RECONNECT;
#endif
}

static bool MqttClient_GetReconnect(void) {
  return mqtt.reconnect;
}

static void McuMqttClient_SetReconnect(bool on) {
  mqtt.reconnect = on;
#if MCU_MQTT_CLIENT_CONFIG_USE_MININI
  McuMinINI_ini_putl(MCU_MQTT_CLIENT_MININI_SECTION_MQTT, MCU_MQTT_CLIENT_MININI_KEY_MQTT_RECONNECT, on, MCU_MQTT_CLIENT_CONFIG_MININI_FILE_NAME);
#endif
}

McuMqtt_client_handle McuMqttClient_getClient(void)  {
  return mqtt.mqtt_client;
}

bool McuMqttClient_doLogging(void) {
  return mqtt.doLogging;
}

void McuMqttClient_set_in_pub_ID(int id) {
  mqtt.in_pub_ID = id;
}

int McuMqttClient_get_in_pub_ID(void) {
  return mqtt.in_pub_ID;
}

const char *McuMqttClient_GetTopicString(int id, const McuMqttClient_Topic_ID_Map_t map[], size_t nofMapEntries) {
  for(int i=0; i<nofMapEntries; i++) {
    if (id==map[i].id) {
      return map[i].topic;
    }
  }
  McuLog_error("unknown topic id %d", id);
  return NULL; /* not found */
}

int McuMqttClient_GetTopicID(const char *topic, const McuMqttClient_Topic_ID_Map_t map[], size_t nofMapEntries) {
  for(int i=0; i<nofMapEntries; i++) {
    if (McuUtility_strcmp(topic, map[i].topic)==0) {
      return map[i].id;
    }
  }
  McuLog_error("unknown topic %s", topic);
  return MCU_MQTT_CLIENT_TOPIC_ID_NONE; /* not found */
}

void McuMqttClient_SetIncomingPubID(const char *topic, const McuMqttClient_Topic_ID_Map_t map[], size_t nofMapEntries) {
  int id;

  id = McuMqttClient_GetTopicID(topic, map, nofMapEntries);
  if (id==MCU_MQTT_CLIENT_TOPIC_ID_NONE) { /* ID not found */
    McuLog_trace("Unknown topic %s", topic);
    McuMqttClient_set_in_pub_ID(MCU_MQTT_CLIENT_TOPIC_ID_NONE); /* set as unknown ID */
  } else {
    McuLog_trace("incoming topic '%s'", topic);
    McuMqttClient_set_in_pub_ID(id);
  }
}

static void mqtt_publish_request_cb(void *arg, err_t err) {
#if 0 && MCU_MQTT_CLIENT_CONFIG_EXTRA_LOGS /* be careful not to delay callback too much */
  const struct mqtt_connect_client_info_t *client_info = (const struct mqtt_connect_client_info_t*)arg;
  McuLog_trace("MQTT client \"%s\" publish request cb: err %d", client_info->client_id, (int)err);
#endif
}

bool McuMqttClient_isConnected(void) {
  return mqtt.isConnected;
}

bool McuMqttClient_CanPublish(void) {
  return mqtt.doPublishing && mqtt.mqtt_client!=NULL && mqtt.isConnected;
}

void McuMqttClient_SetDoPublish(bool publish) {
#if MCU_MQTT_CLIENT_CONFIG_USE_MININI
  McuMinINI_ini_putl(MCU_MQTT_CLIENT_MININI_SECTION_MQTT, MCU_MQTT_CLIENT_MININI_KEY_MQTT_PUBLISH, publish, MCU_MQTT_CLIENT_CONFIG_MININI_FILE_NAME);
#endif
  mqtt.doPublishing = publish;
}

void McuMqttClient_GetDataString(char *buf, size_t bufSize, const uint8_t *data, uint16_t len) {
  buf[0] = '\0';
  for(int i=0; i<len; i++){
    McuUtility_chcat(buf, bufSize, data[i]);
  }
}

static void mqtt_request_cb(void *arg, err_t err) {
}

void McuMqttClient_subscribeTopic(McuMqtt_client_handle client, McuMqtt_connect_client_info_handle client_info, const char * topic) {
  err_t err;

  err = mqtt_sub_unsub(client,
          topic, /* topic string */
          1, /* quos: 0: fire&forget, 1: at least once */
          mqtt_request_cb, /* callback */
          LWIP_CONST_CAST(void*, client_info),
          1 /* subscribe */
          );
  if (err!=ERR_OK) {
    McuLog_error("failed subscribing to '%s', err %d", topic, err);
  }
}
#endif /* LWIP_TCP */

static void McuMqttClient_connection_cb(mqtt_client_t *client, void *arg, mqtt_connection_status_t status) {
  const struct mqtt_connect_client_info_t *client_info = (const struct mqtt_connect_client_info_t*)arg;
  LWIP_UNUSED_ARG(client);

#if MCU_MQTT_CLIENT_CONFIG_EXTRA_LOGS
  McuLog_trace("MQTT client \"%s\" connection cb: status %d", client_info->client_id, (int)status);
#endif
  if (status!=MQTT_CONNECT_ACCEPTED) {
    McuLog_error("MQTT client \"%s\" connection cb: FAILED status %d", client_info->client_id, (int)status);
  }
  if (status == MQTT_CONNECT_ACCEPTED) {
    McuLog_trace("MQTT connection accepted");
    mqtt.isConnected = true;
  } else if (status==MQTT_CONNECT_DISCONNECTED) {
    McuLog_trace("MQTT connection disconnected");
    McuMqttClient_Disconnect();
  }
  mqtt.conn_cb(client, arg, status); /* call user connection callback */
}

uint8_t McuMqttClient_Connect(void) {
#if LWIP_TCP
  if (mqtt.mqtt_client!=NULL) {
    McuLog_trace("MQTT client has been created");
    return ERR_OK; /* already connected */
  }
  mqtt.isConnected = false;
  mqtt.mqtt_client = mqtt_client_new(); /* create client handle */
  err_t err;

  if (mqtt.mqtt_client==NULL) {
    McuLog_fatal("new mqtt client is NULL");
    return ERR_FAILED;
  }
  reloadSettings(); /* reload all settings */

  /* setup connection information */
  McuLog_info("Connecting MQTT client to '%s'", mqtt.broker);

  if (memcmp(&mqtt.addr.resolved_addr, &(ip_addr_t){}, sizeof(ip_addr_t))==0) { /* check if it is still zero */
    McuLog_info("no broker address yet, query DNS ...");
    /* resolve host name to IP address: */
    int nofRetry, maxNofRetry = 3;
    for (nofRetry=0; nofRetry<maxNofRetry; nofRetry++) {
      if (McuDnsResolver_ResolveName((char*)mqtt.broker, &mqtt.addr, 5*1000)!=0) { /* use DNS to resolve name to IP address */
        McuLog_error("failed to resolve broker name '%s', retry #%d of #%d...", mqtt.broker, nofRetry, maxNofRetry);
        vTaskDelay(pdMS_TO_TICKS(5000));
      } else {
        break; /* success! leaving loop */
      }
    }
    McuLog_info("sent %d, received %d, addr %s", mqtt.addr.dns_request_sent, mqtt.addr.dns_response_received, ipaddr_ntoa(&mqtt.addr.resolved_addr));
    if (nofRetry<0) {  //// TODO
      McuLog_fatal("failed to resolve broker name '%s', giving up", mqtt.broker);
      return ERR_FAILED;
    }
  }
  /* setup callbacks for incoming data: */
  mqtt_set_inpub_callback(
      mqtt.mqtt_client, /* client handle */
      mqtt.pub_cb, /* callback for incoming publish messages */
      mqtt.data_cb, /* callback for incoming data */
      LWIP_CONST_CAST(void*, &mqtt_client_info) /* argument for callbacks */
      );
  /* connect to broker */
  #if McuLib_CONFIG_CPU_IS_RPxxxx
  cyw43_arch_lwip_begin(); /* start section for to lwIP access */
  #endif
  err = mqtt_client_connect(
      mqtt.mqtt_client, /* client handle */
      &mqtt.addr.resolved_addr, /* broker IP address */
      MQTT_PORT, /* port to be used */
      McuMqttClient_connection_cb,
      LWIP_CONST_CAST(void*, &mqtt_client_info), /* connection callback with argument */
      &mqtt_client_info /* client information */
      );
  #if McuLib_CONFIG_CPU_IS_RPxxxx
    cyw43_arch_lwip_end(); /* end section accessing lwIP */
  #endif
  if (err!=ERR_OK) {
    McuLog_error("failed connecting client to server");
  } else {
    McuLog_trace("client connecting");
  }
  return err;
#else
  return ERR_FAILED;
#endif /* LWIP_TCP */
}

uint8_t McuMqttClient_Disconnect(void) {
  mqtt.isConnected = false;
  if (mqtt.mqtt_client!=NULL) {
    McuLog_trace("disconnecting client");
    #if McuLib_CONFIG_CPU_IS_RPxxxx
      cyw43_arch_lwip_begin(); /* start section for to lwIP access */
    #endif
    mqtt_disconnect(mqtt.mqtt_client);
    mqtt_client_free(mqtt.mqtt_client); /* free data structure */
    #if McuLib_CONFIG_CPU_IS_RPxxxx
      cyw43_arch_lwip_end(); /* end section accessing lwIP */
    #endif
    mqtt.mqtt_client = NULL;
  }
  return ERR_OK;
}

static uint8_t SetBroker(const unsigned char *broker) {
  unsigned char buf[64];

  McuUtility_ScanDoubleQuotedString(&broker, buf, sizeof(buf));
  McuUtility_strcpy(mqtt.broker, sizeof(mqtt.broker), buf);
#if MCU_MQTT_CLIENT_CONFIG_USE_MININI
  McuMinINI_ini_puts(MCU_MQTT_CLIENT_MININI_SECTION_MQTT, MCU_MQTT_CLIENT_MININI_KEY_MQTT_BROKER, (char*)mqtt.broker, MCU_MQTT_CLIENT_CONFIG_MININI_FILE_NAME);
#endif
  return ERR_OK;
}

static uint8_t SetID(const unsigned char *id) {
  unsigned char buf[64];

  McuUtility_ScanDoubleQuotedString(&id, buf, sizeof(buf));
  McuUtility_strcpy(mqtt.client_id, sizeof(mqtt.client_id), buf);
#if MCU_MQTT_CLIENT_CONFIG_USE_MININI
  McuMinINI_ini_puts(MCU_MQTT_CLIENT_MININI_SECTION_MQTT, MCU_MQTT_CLIENT_MININI_KEY_MQTT_CLIENT, (char*)mqtt.client_id, MCU_MQTT_CLIENT_CONFIG_MININI_FILE_NAME);
#endif
  return ERR_OK;
}

static uint8_t SetUser(const unsigned char *user) {
  unsigned char buf[64];

  McuUtility_ScanDoubleQuotedString(&user, buf, sizeof(buf));
  McuUtility_strcpy(mqtt.client_user, sizeof(mqtt.client_user), buf);
#if MCU_MQTT_CLIENT_CONFIG_USE_MININI
  McuMinINI_ini_puts(MCU_MQTT_CLIENT_MININI_SECTION_MQTT, MCU_MQTT_CLIENT_MININI_KEY_MQTT_USER, (char*)mqtt.client_user, MCU_MQTT_CLIENT_CONFIG_MININI_FILE_NAME);
#endif
  return ERR_OK;
}

static uint8_t SetPassword(const unsigned char *pass) {
  unsigned char buf[96];

  McuUtility_ScanDoubleQuotedString(&pass, buf, sizeof(buf));
  McuUtility_strcpy(mqtt.client_pass, sizeof(mqtt.client_pass), buf);
#if MCU_MQTT_CLIENT_CONFIG_USE_MININI
  McuMinINI_ini_puts(MCU_MQTT_CLIENT_MININI_SECTION_MQTT, MCU_MQTT_CLIENT_MININI_KEY_MQTT_PASS, (char*)mqtt.client_pass, MCU_MQTT_CLIENT_CONFIG_MININI_FILE_NAME);
#endif
  return ERR_OK;
}

static uint8_t PrintStatus(const McuShell_StdIOType *io) {
  McuShell_SendStatusStr((unsigned char*)"mqttclient", (unsigned char*)"mqttclient status\r\n", io->stdOut);
  McuShell_SendStatusStr((unsigned char*)"  log", mqtt.doLogging?(unsigned char*)"on\r\n":(unsigned char*)"off\r\n", io->stdOut);
  McuShell_SendStatusStr((unsigned char*)"  publish", mqtt.doPublishing?(unsigned char*)"on\r\n":(unsigned char*)"off\r\n", io->stdOut);
  McuShell_SendStatusStr((unsigned char*)"  client", mqtt.isConnected?(unsigned char*)"connected\r\n":(unsigned char*)"disconnected\r\n", io->stdOut);
  McuShell_SendStatusStr((unsigned char*)"  broker", mqtt.broker, io->stdOut);
  McuShell_SendStr((unsigned char*)"\r\n", io->stdOut);
  McuShell_SendStatusStr((unsigned char*)"  reconnect", MqttClient_GetReconnect()?(unsigned char*)"on\r\n":(unsigned char*)"off\r\n", io->stdOut);
  McuShell_SendStatusStr((unsigned char*)"  client ID", mqtt.client_id, io->stdOut);
  McuShell_SendStr((unsigned char*)"\r\n", io->stdOut);
  McuShell_SendStatusStr((unsigned char*)"  client user", mqtt.client_user, io->stdOut);
  McuShell_SendStr((unsigned char*)"\r\n", io->stdOut);
  McuShell_SendStatusStr((unsigned char*)"  client password", mqtt.client_pass, io->stdOut);
  McuShell_SendStr((unsigned char*)"\r\n", io->stdOut);
  return ERR_OK;
}

static uint8_t PrintHelp(const McuShell_StdIOType *io) {
  McuShell_SendHelpStr((unsigned char*)"mqttclient", (unsigned char*)"Group of mqttclient commands\r\n", io->stdOut);
  McuShell_SendHelpStr((unsigned char*)"  help|status", (unsigned char*)"Print help or status information\r\n", io->stdOut);
  McuShell_SendHelpStr((unsigned char*)"  log on|off", (unsigned char*)"Turn logging on or off\r\n", io->stdOut);
  McuShell_SendHelpStr((unsigned char*)"  publish on|off", (unsigned char*)"Publishing on or off\r\n", io->stdOut);
  McuShell_SendHelpStr((unsigned char*)"  connect|disconnect", (unsigned char*)"Connect or disconnect from server\r\n", io->stdOut);
  McuShell_SendHelpStr((unsigned char*)"  reconnect on|off", (unsigned char*)"Reconnect after disconnect\r\n", io->stdOut);
  McuShell_SendHelpStr((unsigned char*)"  set broker \"<broker>\"", (unsigned char*)"Set broker name\r\n", io->stdOut);
  McuShell_SendHelpStr((unsigned char*)"  set id \"<id>\"", (unsigned char*)"Set client ID\r\n", io->stdOut);
  McuShell_SendHelpStr((unsigned char*)"  set user \"<user>\"", (unsigned char*)"Set client user name\r\n", io->stdOut);
  McuShell_SendHelpStr((unsigned char*)"  set pass \"<password>\"", (unsigned char*)"Set client password\r\n", io->stdOut);
  McuShell_SendHelpStr((unsigned char*)"  pub \"<topic>\" \"<txt>\"", (unsigned char*)"Publish text to a topic\r\n", io->stdOut);
  return ERR_OK;
}

uint8_t McuMqttClient_ParseCommand(const unsigned char *cmd, bool *handled, const McuShell_StdIOType *io) {
  const unsigned char *p;

  if (McuUtility_strcmp((char*)cmd, McuShell_CMD_HELP)==0 || McuUtility_strcmp((char*)cmd, "mqttclient help")==0) {
    *handled = true;
    return PrintHelp(io);
  } else if ((McuUtility_strcmp((char*)cmd, McuShell_CMD_STATUS)==0) || (McuUtility_strcmp((char*)cmd, "mqttclient status")==0)) {
    *handled = true;
    return PrintStatus(io);
  } else if (McuUtility_strcmp((char*)cmd, "mqttclient log on")==0) {
    *handled = true;
    mqtt.doLogging = true;
  } else if (McuUtility_strcmp((char*)cmd, "mqttclient log off")==0) {
    *handled = true;
    mqtt.doLogging = false;
  } else if (McuUtility_strcmp((char*)cmd, "mqttclient publish on")==0) {
    *handled = true;
    McuMqttClient_SetDoPublish(true);
  } else if (McuUtility_strcmp((char*)cmd, "mqttclient publish off")==0) {
    *handled = true;
    McuMqttClient_SetDoPublish(false);
  } else if (McuUtility_strcmp((char*)cmd, "mqttclient connect")==0) {
    *handled = true;
    McuMqttClient_Connect();
  } else if (McuUtility_strcmp((char*)cmd, "mqttclient disconnect")==0) {
    *handled = true;
    McuMqttClient_Disconnect();
  } else if (McuUtility_strcmp((char*)cmd, "mqttclient reconnect on")==0) {
    *handled = true;
    McuMqttClient_SetReconnect(true);
    } else if (McuUtility_strcmp((char*)cmd, "mqttclient reconnect off")==0) {
    *handled = true;
    McuMqttClient_SetReconnect(false);
  } else if (McuUtility_strncmp((char*)cmd, "mqttclient set broker ", sizeof("mqttclient set broker ")-1)==0) {
    *handled = TRUE;
    p = cmd + sizeof("mqttclient set broker ")-1;
    return SetBroker(p);
  } else if (McuUtility_strncmp((char*)cmd, "mqttclient set id ", sizeof("mqttclient set id ")-1)==0) {
    *handled = TRUE;
    p = cmd + sizeof("mqttclient set id ")-1;
    return SetID(p);
  } else if (McuUtility_strncmp((char*)cmd, "mqttclient set user ", sizeof("mqttclient set user ")-1)==0) {
    *handled = TRUE;
    p = cmd + sizeof("mqttclient set user ")-1;
    return SetUser(p);
  } else if (McuUtility_strncmp((char*)cmd, "mqttclient set pass ", sizeof("mqttclient set pass ")-1)==0) {
    *handled = TRUE;
    p = cmd + sizeof("mqttclient set pass ")-1;
    return SetPassword(p);
  } else if (McuUtility_strncmp((char*)cmd, "mqttclient pub ", sizeof("mqttclient pub ")-1)==0) {
    unsigned char topic[128], text[128];
    *handled = TRUE;
    p = cmd + sizeof("mqttclient pub ")-1;
    if (McuUtility_ScanDoubleQuotedString(&p, topic, sizeof(topic))!=ERR_OK) {
      return ERR_FAILED;
    }
    McuUtility_SkipSpaces(&p);
    if (McuUtility_ScanDoubleQuotedString(&p, text, sizeof(text))!=ERR_OK) {
      return ERR_FAILED;
    }
    return McuMqttClient_PublishText(topic, text);
  }
  return ERR_OK;
}

static TaskHandle_t taskHandle = NULL;

void mqttClientTask(void *pv) {
  /* tasks which periodically checks if we are disconnected and then reconnects */
  for(;;) {
    vTaskDelay(pdMS_TO_TICKS(5000));
    if (!mqtt.isConnected && MqttClient_GetReconnect()) {
      McuLog_info("reconnecting MQTT client");
      McuMqttClient_Connect();
    }
  }
}

void McuMqttClient_Deinit(void) {
  McuMqttClient_Disconnect();
  if (taskHandle!=NULL) {
    vTaskSuspend(taskHandle);
  }
}

void McuMqttClient_Init(void) {
  mqtt.doLogging = true;
  mqtt.doPublishing = MCU_MQTT_CLIENT_CONFIG_PUBLISH;
  mqtt.reconnect = MCU_MQTT_CLIENT_CONFIG_RECONNECT;
  if (taskHandle==NULL) {
    /* create task for reconnect to broker */
    if (xTaskCreate(mqttClientTask, "mqttclient", 4*1024/sizeof(StackType_t), NULL, tskIDLE_PRIORITY, &taskHandle)!=pdPASS) {
      McuLog_fatal("failed creating mqtt client task!");
    }
  } else {
    vTaskResume(taskHandle);
  }
}

#endif /* MCU_MQTT_CLIENT_CONFIG_ENABLED */
