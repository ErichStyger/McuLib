/*
 * Copyright (c) 2023-2026, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef MCU_MQTT_CLIENT_H_
#define MCU_MQTT_CLIENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "McuMqtt_client_config.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#if MCU_MQTT_CLIENT_CONFIG_USE_SHELL
  #include "McuShell.h"

  /*!
   * \brief Command line and shell handler
   * \param cmd The command to be parsed
   * \param handled If command has been recognized and handled
   * \param io I/O handler to be used
   * \return error code, otherwise ERR_OK
   */
  uint8_t McuMqttClient_ParseCommand(const unsigned char* cmd, bool *handled, const McuShell_StdIOType *io);
#endif

#define MCU_MQTT_CLIENT_TOPIC_ID_NONE   (-1) /* special topic ID for an unknown topic */

typedef void *McuMqtt_client_handle;  /* is actually a pointer to mqtt_client_s */
typedef const void *McuMqtt_connect_client_info_handle;  /* is actually a pointer to const struct mqtt_connect_client_info_t */

void McuMqttClient_subscribeTopic(McuMqtt_client_handle client, McuMqtt_connect_client_info_handle client_info, const char *topic);

/*!
 * \brief Returns the current MQTT client handle
 * \return MQTT client handle
 */
McuMqtt_client_handle McuMqttClient_getClient(void);

/*!
 * \brief Getter for the current logging mode
 * \return true if logging is enabled, false otherwise
 */
bool McuMqttClient_doLogging(void);

typedef struct McuMqttClient_Topic_ID_Map_t {
  const char *topic; /* topic string */
  int id;            /* ID for topic string */
} McuMqttClient_Topic_ID_Map_t;

/*!
 * \brief Return for a given topic id the corresponding topic string
 * \param id The topic ID
 * \param map Array of topic-ID map
 * \param nofMapEntries Number of elements in the map array
 * \return The topic string or NULL if not found
 */
const char *McuMqttClient_GetTopicString(int id, const McuMqttClient_Topic_ID_Map_t map[], size_t nofMapEntries);

/*!
 * \brief Return for a given topic string the corresponding ID
 * \param topic Pointer to the topic string
 * \param map Array of topic-ID map
 * \param nofMapEntries Number of elements in the map array
 * \return the topic ID or MCU_MQTT_CLIENT_TOPIC_ID_NONE if not found
 */
int McuMqttClient_GetTopicID(const char *topic, const McuMqttClient_Topic_ID_Map_t map[], size_t nofMapEntries);

/*!
 * \brief Set the incoming topic ID for a topic, called from the incoming_publish_cb().
 * \param topic Topic string
 * \param map Map of topic strings to topic IDs
 * \param nofMapEntries Number of entries in the map array
 */
void McuMqttClient_SetIncomingPubID(const char *topic, const McuMqttClient_Topic_ID_Map_t map[], size_t nofMapEntries);

/*!
 * \brief Set the incoming topic ID
*/
void McuMqttClient_set_in_pub_ID(int id);

/*!
 * \brief Get the incoming topic ID
 * \return ID
*/
int McuMqttClient_get_in_pub_ID(void);

/*!
 * \brief Copy the MQTT data string with a given length into a buffer and zero terminates it.
 * \param [out] buf Buffer, where to store the data
 * \param [in] bufSize Size of the buffer in bytes
 * \param [in] data MQTT data
 * \param [in] len Number of bytes in data
*/
void McuMqttClient_GetDataString(char *buf, size_t bufSize, const uint8_t *data, uint16_t len);

/*!
 * \brief Connect as client to the server
 * \return error code, otherwise ERR_OK
 */
uint8_t McuMqttClient_Connect(void);

/*!
 * \brief Disconnect from the server
 * \return error code, otherwise ERR_OK
 */
uint8_t McuMqttClient_Disconnect(void);

/*!
 * \brief Decides if client is connected or not
 * \return true if client is connected, false otherwise
 */
bool McuMqttClient_isConnected(void);

/*!
 * \brief Decides if client can publish or not
 * \return true if client is publishing, false otherwise
 */
bool McuMqttClient_CanPublish(void);

/*!
 * \brief Sets if the client is publishing or not
 * \param publish true to publish, false otherwise
 */
void McuMqttClient_SetDoPublish(bool publish);

/*!
 * \brief Set the MQTT client callbacks. Exact callback types not published here to avoid include problems
 * \param conn_cb Callback called on connection, of type mqtt_connection_cb_t
 * \param data_cb Callback called on incoming data, of type mqtt_incoming_data_cb_t
 * \param pub_cb Callback called on incoming publish messages, of type mqtt_incoming_publish_cb_t
 */
void McuMqttClient_SetCallbacks(
  void (conn_cb)(void *client, void *arg, int status),
  void (data_cb) (void *arg, const uint8_t *data, uint16_t len, uint8_t flags),
  void (pub_cb) (void *arg, const char *topic, uint32_t tot_len)
);

/*!
 * \brief Publish a text string to a topic
 * \param topic The topic to publish to
 * \param text Pointer to string with the content to publish
 */
uint8_t McuMqttClient_PublishText(const char *topic, const char *text);

/*!
 * \brief Publish a switch value to a topic
 * \param topic The topic to publish to
 * \param on If switch is ON or OFF
 * \param asJSON If text is published as JSON or simply as "ON" or "OFF"
 */
uint8_t McuMqttClient_PublishSwitch(const char *topic, bool isOn, bool asJSON);

/*!
 * \brief Handles an incoming switch topic
 * \param data Pointer to the data value
 * \param len Length of data in bytes
 * \param logMsg Log message used for printing, or NULL
 * \param setter Function point to be called for the switch value, or NULL
 */
void McuMqttClient_IncomingSwitch(const uint8_t *data, uint16_t len, const char *logMsg, void (setter)(bool));

/*!
 * \brief Publish time value to a topic
 * \param topic The topic to publish to
 * \param hours Number of hours
 * \param minutes Number of minutes
 * \param asJSON If text is published as JSON or simply e.g. as "12:25"
 */
uint8_t McuMqttClient_PublishTime(const char *topic, uint8_t hours, uint8_t minutes, bool asJSON);

/*!
 * \brief Handles an incoming time topic
 * \param data Pointer to the data value
 * \param len Length of data in bytes
 * \param logMsg Log message used for printing, or NULL
 * \param setter Function point to be called for the time value, or NULL
 */
void McuMqttClient_IncomingTime(const uint8_t *data, uint16_t len, const char *logMsg, void (setter)(uint8_t, uint8_t));

/*!
 * \brief Publish temperature value to a topic
 * \param topic The topic to publish to
 * \param temperature Temperature value (degree Celsius)
 * \param asJSON If text is published as JSON with value and unit
 */
uint8_t McuMqttClient_PublishTemperature(const char *topic, float temperature, bool isJSON);

/*!
 * \brief Handles an incoming temperature topic
 * \param data Pointer to the data value
 * \param len Length of data in bytes
 * \param logMsg Log message used for printing, or NULL
 * \param setter Function point to be called for the temperature value, or NULL
 */
void McuMqttClient_IncomingTemperature(const uint8_t *data, uint16_t len, const char *logMsg, void (setter)(float));

/*!
 * \brief Module de-initialization
 */
void McuMqttClient_Deinit(void);

/*!
 * \brief Module initialization
 */
void McuMqttClient_Init(void);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* MCU_MQTT_CLIENT_H_ */
