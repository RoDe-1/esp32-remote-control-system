/*
 * coding_style.h
 *
 *  Created on: Jan 4, 2022
 *      Author: kjagu
 */

#ifndef MAIN_CODING_STYLE_H_
#define MAIN_CODING_STYLE_H_

// Includes
#include "esp_netif.h"
#include "esp_err.h"
#include <stddef.h>
// Macros and defines
#define CODING_STYLE_EXAMPLE_DEFINE	"Coding Style"		// Describe it if necessary

// Externed variables
extern esp_netif_t* coding_style_esp_netif_sta;
extern esp_netif_t* coding_style_esp_netif_ap;

// Typedefs
/**
 * Provide a description for your typedef
 * @note Add notes if necessary.
 * @note postfix enums with "_e".
 */
typedef enum
{
	CODING_STYLE_FIRST = 0,
	CODING_STYLE_SECOND,
	CODING_STYLE_THIRD,
} coding_style_message_e;

/**
 * Provide a description for your typedef
 * @note postfix structs with "_t".
 */
typedef struct
{
	coding_style_message_e msgID;
	void *param;
} coding_style_queue_message_t;

// Public function prototypes
/**
 * Public function defintions require a description in the header file only.
 * @Note use the "file_name" prefix for the function name e.g. "coding_style"
 * @param var_a describe the input parameter for var_a
 * @param var_b describe the input parameter for var_b
 * @param var_c describe the input parameter for var_c
 * @return ESP_OK, if successful
 */
esp_err_t coding_style_public(int var_a, int var_b, int var_c);

#endif /* MAIN_CODING_STYLE_H_ */













