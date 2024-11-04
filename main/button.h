#ifndef BUTTON_H
#define BUTTON_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define BUTTON	GPIO_NUM_16 
#define LONG_PRESS_IN_SECONDS 1
#define Total_WIFI_Config 3 //total AP's MAC address

void shop_selection();

#endif