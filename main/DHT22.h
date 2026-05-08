/* 

	DHT22 temperature sensor driver

*/

#ifndef DHT22_H_  
#define DHT22_H_

#define DHT_OK 0
#define DHT_CHECKSUM_ERROR -1
#define DHT_TIMEOUT_ERROR -2
#include <stdbool.h>
#define DHT_GPIO 			19
extern float g_temperature;
extern float g_humidity;
extern bool g_dht_valid;
/**
 * Starts DHT22 sensor task
 */
void DHT22_task_start(void);

// == function prototypes =======================================

void 	setDHTgpio(int gpio);
void 	errorHandler(int response);
int 	readDHT();
float 	getHumidity();
float 	getTemperature();
int 	getSignalLevel( int usTimeOut, bool state );

#endif
