#ifndef WIFI_SCAN_H
#define WIFI_SCAN_H 

#include "stdio.h"
#include <stdlib.h>
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define DEFAULT_SCAN_LIST_SIZE 10

void wifi_my_config();
void wifi_scan();

#endif