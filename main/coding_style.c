/*
 * coding_style.c
 *
 *  Created on: Jan 4, 2022
 *      Author: kjagu
 */

// Include standard library header files e.g.:
#include <stddef.h>
#include "esp_err.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// Include ESP-IDF headers:
#include "esp_err.h"
#include "esp_log.h"
#include "nvs_flash.h"

// Include application headers:
#include "coding_style.h"

// Global netif objects for the WiFi station and access point
esp_netif_t* coding_style_esp_netif_sta = NULL;
esp_netif_t* coding_style_esp_netif_ap = NULL;

/**
 * Static function definitions require a description and of course, use the "static" keyword.
 * If you want to call static functions before they are defined (meaning call it  above this
 * definition), then you'll need to include prototypes for them.
 * @Note use the "file_name" prefix for the function name e.g. "coding_style"
 * @param var_a describe the input parameter for var_a
 * @param var_b describe the input parameter for var_b
 * @param var_c describe the input parameter for var_c
 * @return ESP_OK, if successful
 */
static esp_err_t coding_style_static(int var_a)
{
	esp_err_t esp_err = ESP_OK;

	if (var_a < 0)
	{
		esp_err = ESP_FAIL;
	}

	return esp_err;
}

// Public functions go below the private/static functions
// Note: a description of public funcitons is not necessary
// as it is already included in the header file.
esp_err_t coding_style_public(int var_a, int var_b, int var_c)
{
	int result = (var_a + var_b) - var_c;
	esp_err_t esp_err = coding_style_static(result);

	// Example use of ESP_ERROR_CHECK
	ESP_ERROR_CHECK(esp_err);

	return esp_err;
}











