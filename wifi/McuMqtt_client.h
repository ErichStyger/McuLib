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
void McuMqttClient_GetDataString(unsigned char *buf, size_t bufSize, const uint8_t *data, uint16_t len);

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
void McuMqttClientSetCallbacks(
  void (conn_cb)(void *client, void *arg, int status),
  void (data_cb) (void *arg, const uint8_t *data, uint16_t len, uint8_t flags),
  void (pub_cb) (void *arg, const char *topic, uint32_t tot_len)
);

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
