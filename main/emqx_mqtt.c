/*
 * emqx_mqtt.c
 *
 *  Created on: Apr 30, 2026
 *      Author: Pentium center
 */

#include <stdio.h>
#include <stddef.h>
#include "emqx_mqtt.h"
#include "esp_log.h"
#include "esp_event.h"
#include "mqtt_client.h"

static const char *TAG = "emqx_mqtt";

/* CA certificate embedded from main/emqx_ca.crt */
extern const uint8_t emqx_ca_crt_start[] asm("_binary_emqx_ca_crt_start");
extern const uint8_t emqx_ca_crt_end[]   asm("_binary_emqx_ca_crt_end");

#define EMQX_BROKER_URI   "mqtts://d06f6068.ala.eu-central-1.emqxsl.com:8883"
#define EMQX_USERNAME     "Udemy_ESP32_Test"
#define EMQX_PASSWORD     "ESP32_Test_2026_RoDe!"

#define EMQX_PUB_TOPIC    "esp32/Udemy_ESP32_Test/pub"
#define EMQX_SUB_TOPIC    "esp32/Udemy_ESP32_Test/sub"

static esp_mqtt_client_handle_t mqtt_client = NULL;

static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    switch (event->event_id)
    {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT connected");

            esp_mqtt_client_subscribe(mqtt_client, EMQX_SUB_TOPIC, 1);
            ESP_LOGI(TAG, "Subscribed to topic: %s", EMQX_SUB_TOPIC);

            esp_mqtt_client_publish(mqtt_client, EMQX_PUB_TOPIC,
                                    "ESP32 connected to EMQX", 0, 1, 0);
            ESP_LOGI(TAG, "Published initial message to topic: %s", EMQX_PUB_TOPIC);
            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGW(TAG, "MQTT disconnected");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT subscribed, msg_id=%d", event->msg_id);
            break;

        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT unsubscribed, msg_id=%d", event->msg_id);
            break;

        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT published, msg_id=%d", event->msg_id);
            break;

        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT data received");
            ESP_LOGI(TAG, "TOPIC=%.*s", event->topic_len, event->topic);
            ESP_LOGI(TAG, "DATA=%.*s", event->data_len, event->data);
            break;

        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "MQTT event error");
            break;

        default:
            ESP_LOGI(TAG, "Other MQTT event id:%d", event->event_id);
            break;
    }

    return ESP_OK;
}

void emqx_mqtt_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = EMQX_BROKER_URI,
        .client_id = EMQX_CLIENT_ID,
        .username = EMQX_USERNAME,
        .password = EMQX_PASSWORD,
        .cert_pem = (const char *)emqx_ca_crt_start,
        .event_handle = mqtt_event_handler,
    };

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(mqtt_client);

    ESP_LOGI(TAG, "EMQX MQTT client started");
}

void emqx_mqtt_publish_sensor_data(float temperature, float humidity, int rssi)
{
    if (mqtt_client == NULL)
    {
        return;
    }

    char sensor_msg[160];
    snprintf(sensor_msg, sizeof(sensor_msg),
             "{\"temperature\":%.2f,\"humidity\":%.2f,\"rssi\":%d}",
             temperature, humidity, rssi);

    esp_mqtt_client_publish(mqtt_client,
                            "esp32/Udemy_ESP32_Test/sensor",
                            sensor_msg,
                            0,
                            1,
                            0);
}
