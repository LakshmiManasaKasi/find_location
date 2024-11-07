
// functions included for wifi scan functionality 
#include "stdio.h"
#include "esp_wifi.h"

#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "wifi_scan.h"
#include "esp_mac.h"
#include "string.h"
#include "Algorithm.h"

static const char *TAG = "Wifi";
uint16_t number = DEFAULT_SCAN_LIST_SIZE;
wifi_ap_record_t ap_info[DEFAULT_SCAN_LIST_SIZE];
uint16_t ap_count = 0;

const char *Mac_database[DEFAULT_SCAN_LIST_SIZE];
extern bool Database_update;

void wifi_my_config()
{
    // Init NVS flash --------------------- 
    int ret = nvs_flash_init();
    if((ret == ESP_ERR_NVS_NO_FREE_PAGES) || (ret = ESP_ERR_NVS_NEW_VERSION_FOUND))
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();

    }
    ESP_ERROR_CHECK(ret);
    //Complete NVS Flash init 
    //---------------------------
    // Initialize wifi 
    wifi_init_config_t wifi_cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_cfg) );
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));  //ESP connects to wifi like computer
    ESP_ERROR_CHECK(esp_wifi_start()); //start wifi 
    // WIFI init complete 
}

void wifi_scan()
{
    uint16_t wifi_ap_count =0;
    esp_wifi_scan_start(NULL,true);
    esp_wifi_scan_get_ap_num(&wifi_ap_count);
    ap_count = wifi_ap_count;
    //Allocate memory for scanned data---------------
    wifi_ap_record_t *ap_records = malloc(sizeof(wifi_ap_record_t) * wifi_ap_count);
    if(ap_records == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate memory for AP records");
        return ;
    }
    //----------------------------------
    //Get the scan data and print it 
    esp_wifi_scan_get_ap_records(&wifi_ap_count,ap_records);
    for(int i=0; i<wifi_ap_count;i++)
    {
        int j=0;
        printf("SSID: %s  RSSI: %d dBm\n", ap_records[i].ssid, ap_records[i].rssi);
        // printf("MAC: " MACSTR "\n", MAC2STR(ap_records[i].bssid));
        ap_info[i]=ap_records[i];
        // printf("debug:----------\n");
        // printf("SSID: %s  RSSI: %d dBm\t", ap_info[i].ssid, ap_info[i].rssi);
        // printf("MAC: " MACSTR "\n", MAC2STR(ap_info[i].bssid));
        // printf("debug:---------------------------\n");
        //update database if selected
         Mac_database[0] = mac_str(ap_records[i].bssid);
        if(Database_update ==true)
        {
            for(j=0;j<i;j++)
            {
                if(Mac_database[j] != mac_str(ap_info[i].bssid))
                {
                     Mac_database[j] = mac_str(ap_records[i].bssid);
                     printf("--debug entered");
                    // printf("----debug: Updated Mac Value %x at %d----\n",Mac_database[j],j);
                    j++;
                }
            }
        }
    }
    
    free(ap_records);
    vTaskDelay(10 / portTICK_PERIOD_MS);
}