#ifndef ESP8266_H
#define ESP8266_H


// Combined include file for esp8266
// Actually misnamed, as it also works for ESP32.
// ToDo: Figure out better name



#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#ifdef FREERTOS
#include <stdint.h>

#ifdef ESP32
#define ICACHE_FLASH_ATTR /*xx*/
#define ICACHE_RODATA_ATTR /*xx*/
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "lwip/ip.h"


#include "esp_attr.h"
#include "esp_sleep.h"
//#include "esp_err.h" bug mit assert
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_heap_caps.h"
#include "esp_intr.h"
#include "esp_ipc.h"
#include "esp_ssc.h"
#include "esp_system.h"
#include "esp_task.h"
#include "esp_wifi.h"
#include "esp_types.h"
#include "esp_wifi_types.h"
//#include "heap_alloc_caps.h"

#include "esp_log.h"

//#include <esp_common.h>
#include <rom/queue.h>
#else
#include <espressif/esp_common.h>
#endif

#else
#include <ctype.h>
#include <c_types.h>
#include <ip_addr.h>
#include <espconn.h>
#include <ets_sys.h>
#include <gpio.h>
#include <mem.h>
#include <osapi.h>
#include <user_interface.h>
#include <upgrade.h>
#endif

#include "Platform.h"
#include "espmissingincludes.h"




#endif /*ESP8266_H*/
