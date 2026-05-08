/*
 * app_nvs.h
 *
 *  Created on: Apr 9, 2026
 *      Author: Pentium center
 */

#ifndef MAIN_APP_NVS_H_
#define MAIN_APP_NVS_H_

#include <stdbool.h>
#include "esp_netif.h"
#include "esp_wifi_types.h"


/**
 * Saves station mode Wifi credrntials to NVS.
 * @return ESP_OK if seccessful.
 */
esp_err_t app_nvs_save_sta_creds(void);


/**
 * Loads the previously saved credentials from NVS
 * @return true if previously saved credentials were found.
 */

bool app_nvs_load_sta_creds(void);

/**
 * Clears station mode credentials from NVS
 * @return ESP_OK if successful.
 */
esp_err_t app_nvs_clear_sta_creds(void);


#endif /* MAIN_APP_NVS_H_ */
