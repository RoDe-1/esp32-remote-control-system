/*
 * emqx_mqtt.h
 *
 *  Created on: Apr 30, 2026
 *      Author: Pentium center
 */

#ifndef MAIN_EMQX_MQTT_H_
#define MAIN_EMQX_MQTT_H_

#define EMQX_CLIENT_ID "Udemy_ESP32_Test"

void emqx_mqtt_start(void);

void emqx_mqtt_publish_sensor_data(float temperature, float humidity,int rssi);

#endif /* MAIN_EMQX_MQTT_H_ */
