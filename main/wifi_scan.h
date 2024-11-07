
#ifndef WIFI_SCAN_H
#define WIFI_SCAN_H 

#include "stdio.h"
#include <stdlib.h>
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define DEFAULT_SCAN_LIST_SIZE 15 //TBD :Config
// extern uint8_t *Mac_database[DEFAULT_SCAN_LIST_SIZE];
extern const char *Mac_database[DEFAULT_SCAN_LIST_SIZE];
void wifi_my_config();
void wifi_scan();

#endif