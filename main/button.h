#ifndef BUTTON_H
#define BUTTON_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define BUTTON	GPIO_NUM_16 
#define LONG_PRESS_IN_SECONDS 1
#define Total_WIFI_Config 3 //total AP's MAC address
#define TOtal_Shop_Config  3
#define UPDATE_DATABASE_ENABLED 1 //TBD: Config
#define ARRAY_SIZE(a)   (sizeof(a)/sizeof((a)[0]))

struct ShopWifiCombo
{
    char ShopName[20];
    char WifiName[20];
};

extern struct ShopWifiCombo floorMap[];

void shop_selection();

#endif