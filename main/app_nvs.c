/*
 * app_nvs.c
 *
 *  Created on: Apr 9, 2026
 *      Author: Pentium center
 */
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "nvs_flash.h"

#include "app_nvs.h"
#include "wifi_app.h"

// Tag for logging to the moniter
static const char TAG[] = "nvs";

// NVS namespace used for station mode credentials
const char app_nvs_sta_creds_namespace[] = "stdcreds";

/**
 * Saves station mode Wifi credrntials to NVS.
 * @return ESP_OK if seccessful.
 */
esp_err_t app_nvs_save_sta_creds(void)
{
    nvs_handle handle;
    esp_err_t esp_err;

    ESP_LOGI(TAG, "app_nvs_save_sta_creds: Saving station mode credentials to flash");

    wifi_config_t *wifi_sta_config = wifi_app_get_wifi_config();

    if (wifi_sta_config == NULL)
    {
        ESP_LOGE(TAG, "app_nvs_save_sta_creds: wifi_sta_config is NULL, nothing to save");
        return ESP_ERR_INVALID_STATE;
    }

    esp_err = nvs_open(app_nvs_sta_creds_namespace, NVS_READWRITE, &handle);
    if (esp_err != ESP_OK)
    {
        printf("app_nvs_save_sta_creds: Error (%s) opening NVS handle!\n", esp_err_to_name(esp_err));
        return esp_err;
    }

    // Set SSID
    esp_err = nvs_set_blob(handle, "ssid", wifi_sta_config->sta.ssid, MAX_SSID_LENGTH);
    if (esp_err != ESP_OK)
    {
        printf("app_nvs_save_sta_creds: Error (%s) setting SSID to NVS!\n", esp_err_to_name(esp_err));
        nvs_close(handle);
        return esp_err;
    }

    // Set Password
    esp_err = nvs_set_blob(handle, "password", wifi_sta_config->sta.password, MAX_PASSWORD_LENGTH);
    if (esp_err != ESP_OK)
    {
        printf("app_nvs_save_sta_creds: Error (%s) setting Password to NVS!\n", esp_err_to_name(esp_err));
        nvs_close(handle);
        return esp_err;
    }

    // Commit credentials to NVS
    esp_err = nvs_commit(handle);
    if (esp_err != ESP_OK)
    {
        printf("app_nvs_save_sta_creds: Error (%s) committing credentials to NVS!\n", esp_err_to_name(esp_err));
        nvs_close(handle);
        return esp_err;
    }

    nvs_close(handle);

    ESP_LOGI(TAG, "app_nvs_save_sta_creds: Station credentials saved successfully. SSID: %s",
             wifi_sta_config->sta.ssid);

    return ESP_OK;
}

/**
 * Loads the previously saved credentials from NVS
 * @return true if previously saved credentials were found.
 */

bool app_nvs_load_sta_creds(void)
{
    nvs_handle handle;
    esp_err_t esp_err;

    ESP_LOGI(TAG, "app_nvs_load_sta_creds: Loading WiFi credentials from flash");

    esp_err = nvs_open(app_nvs_sta_creds_namespace, NVS_READONLY, &handle);
    if (esp_err != ESP_OK)
    {
        printf("app_nvs_load_sta_creds: Error (%s) opening NVS handle!\n", esp_err_to_name(esp_err));
        return false;
    }

    wifi_config_t *wifi_sta_config = wifi_app_get_wifi_config();
    if (wifi_sta_config == NULL)
    {
        printf("app_nvs_load_sta_creds: wifi_sta_config is NULL\n");
        nvs_close(handle);
        return false;
    }

    memset(wifi_sta_config, 0x00, sizeof(wifi_config_t));

    size_t wifi_config_size;
    uint8_t *wifi_config_buff = NULL;

    // Load SSID
    wifi_config_size = sizeof(wifi_sta_config->sta.ssid);
    wifi_config_buff = (uint8_t *)malloc(wifi_config_size);
    if (wifi_config_buff == NULL)
    {
        printf("app_nvs_load_sta_creds: failed to allocate memory for SSID buffer\n");
        nvs_close(handle);
        return false;
    }

    memset(wifi_config_buff, 0x00, wifi_config_size);

    esp_err = nvs_get_blob(handle, "ssid", wifi_config_buff, &wifi_config_size);
    if (esp_err != ESP_OK)
    {
        printf("app_nvs_load_sta_creds: (%s) no station SSID found in NVS\n", esp_err_to_name(esp_err));
        free(wifi_config_buff);
        nvs_close(handle);
        return false;
    }

    memcpy(wifi_sta_config->sta.ssid, wifi_config_buff, wifi_config_size);
    free(wifi_config_buff);

    // Load Password
    wifi_config_size = sizeof(wifi_sta_config->sta.password);
    wifi_config_buff = (uint8_t *)malloc(wifi_config_size);
    if (wifi_config_buff == NULL)
    {
        printf("app_nvs_load_sta_creds: failed to allocate memory for password buffer\n");
        nvs_close(handle);
        return false;
    }

    memset(wifi_config_buff, 0x00, wifi_config_size);

    esp_err = nvs_get_blob(handle, "password", wifi_config_buff, &wifi_config_size);
    if (esp_err != ESP_OK)
    {
        printf("app_nvs_load_sta_creds: (%s) retrieving password!\n", esp_err_to_name(esp_err));
        free(wifi_config_buff);
        nvs_close(handle);
        return false;
    }

    memcpy(wifi_sta_config->sta.password, wifi_config_buff, wifi_config_size);
    free(wifi_config_buff);

    nvs_close(handle);

    printf("app_nvs_load_sta_creds: SSID loaded successfully: %s\n", wifi_sta_config->sta.ssid);

    return (wifi_sta_config->sta.ssid[0] != '\0');
}

/**
 * Clears station mode credentials from NVS
 * @return ESP_OK if successful.
 */
esp_err_t app_nvs_clear_sta_creds(void)
{
	nvs_handle handle;
	esp_err_t esp_err;
	ESP_LOGI(TAG,"app_nvs_clear_sta_creds: Clearing Wifi station mode credentials from flash");

	esp_err = nvs_open(app_nvs_sta_creds_namespace,NVS_READWRITE,&handle);
	if(esp_err != ESP_OK)
	{
		printf("app_nvs_clear_sta_creds: Error (%s) opening NVS handle!\n",esp_err_to_name(esp_err));
		return esp_err;
	}

	// Erase credentials
	esp_err = nvs_erase_all(handle);
	if(esp_err != ESP_OK)
	{
		printf("app_nvs_clear_sta_creds: Error (%s) erasing station mode credentials!\n",esp_err_to_name(esp_err));
		return esp_err;
	}

	// Commit clearing credentials from NVS
	esp_err = nvs_commit(handle);
	if(esp_err != ESP_OK)
	{
		printf("app_nvs_clear_sta_creds: Error (%s) NVS commit!\n",esp_err_to_name(esp_err));
		return esp_err;
	}
	nvs_close(handle);

	printf("app_nvs_clear_sta_creds: returned ESP_OK!\n");
	return ESP_OK;

}




