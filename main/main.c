#include <stdint.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "ssd1306.h"
#include <stdlib.h>
#include "driver/gpio.h"
#include "qmc5883l.h"
// #include <math.h>
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
#include "wifi_scan.h"
#include "magnetometer.h"
#include "lcd.h"
#include "button.h"
#include "Algorithm.h"
#include "esp_mac.h"
// for magneto meter 

extern qmc5883l_t dev;
extern SAppMenu menu;
extern double Heading;
static const char *TAG = "scan";
extern int selected; // to hold selected AP's Index
SemaphoreHandle_t xSemaphore = NULL;


//mac_address of wifi (can be read by using wifi analyzer app in android)
// database of mac address of available wifi 
const char Manasa[]="76d656579492";
const char Hariom[]="c0a5dd1eda91";
const char MicronEMS_Guest[]="5091e3c61c58";

//example : can be replaced by MAC2STR funxtionality
// printf("MAC: " MACSTR "\n", MAC2STR(ap_records[i].bssid));
// const char* another_mac = "30:AE:A4:07:0D:64";
// if (strcmp(mac_str, another_mac) == 0) {
//     printf("MAC addresses are equal.\n");
// } else {
//     printf("MAC addresses are not equal.\n");
// }

// #define Total_WIFI_Config 3 //total AP's MAC address
#define ThreshHold -5 //threshold value

//structure for All wifi names
typedef struct 
{
	char AccessPoint[60];
}allAP;
allAP config_wifi[Total_WIFI_Config]={{"Manasa"},{"Hariom"},{"MicronEMS_Guest"}};//wifi names wrt mac address above

static int rssi_values[Total_WIFI_Config]={0,0,0}; // rssi_values to find nearest ap
extern char converted_mac_str[30];// converted string of mac address of all mac id
int large; // holds the index of nearest AP

static char direction_next[25];//final direction values
static float direction_deg = 0.0; //finale direction degrees

static int temp=0; //holds the previous nearest AP RSSI value
static bool update = true; 
static int inde = -1; //holds the index of previous nearest AP
//static bool count = true;

extern uint16_t number;// = DEFAULT_SCAN_LIST_SIZE;
extern wifi_ap_record_t ap_info[DEFAULT_SCAN_LIST_SIZE];
extern uint16_t ap_count;// = 0;

//Flour Data mapping structure
typedef struct
{
	char shop_name[50];
	struct shop
	{
		char toshop_name[50];
		char direction[15];
		float dir_angle;
	}toshop[Total_WIFI_Config-1];
}flourmap;

//configured Data Of Map's(Shop Location's)
flourmap shop_data[Total_WIFI_Config] = {{"Manasa",{{"Hariom","NILL",-1},{"MicronEMS_Guest","NILL",-1}}},
                            {"Hariom",{{"MicronEMS_Guest","NILL",-1},{"Manasa","NILL",-1}}},
                            {"MicronEMS_Guest",{{"Manasa","NILL",-1},{"Hariom","NILL",-1}}}};

//function to find the nearest ap's index for the device
void near() {
	int data[Total_WIFI_Config];
	int data_2[Total_WIFI_Config];
    	for (int i = 0; i < Total_WIFI_Config; ++i) {
        	if(rssi_values[i] == 0) {
        		data[i]=-1000;
        		printf("%d ",data[i]);
        		continue;
        	}
        		data[i]=rssi_values[i];
        		data_2[i] = rssi_values[i];
        		printf("%d ",data[i]);
    	}
    	large =-1;
	for (int i = 1; i < Total_WIFI_Config; ++i) {
    		if (data[0] < data[i]) {
      			data[0] = data[i];
      			large = i;
    		}
  	}
  	// printf("\ntemp = %d\tinde = %d\tdata =%d\tlarge = %d\n",temp,inde,data_2[large], large);
  	if(update) {
		temp = data_2[large];;
		inde= large;
		update = false;    		
	}
	// printf("temp = %d\tinde = %d\tdata =%d\tlarge = %d\n",temp,inde,data_2[large], large);

	//for ignoring the ap when the difference is less to find nearest location
	//commenting 92 lin2(bool count)
  	// if((temp - data_2[large]) >= ThreshHold && count ) {
	// 	large=inde;
	// 	count = false;
	// }
	// else {
	// 	count = true;
	// }
	
  	temp = data_2[large];
  	inde = large;	
  	printf("temp = %d\tinde = %d\tdata =%d\tlarge = %d\n",temp,inde,data_2[large], large);
}


//function to find the current location or nearest ap to device
void FindLocation(wifi_ap_record_t wifi_apRecord[], int wifi_count)
{
	for(int k=0;(k < DEFAULT_SCAN_LIST_SIZE) && (k < wifi_count);k++) 
	{
		char *mac = (char *)wifi_apRecord[k].bssid;
		// printf("Mac : %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n" , mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		mac_str(wifi_apRecord[k].bssid);
		printf("%s\n",converted_mac_str);

		if(!strcmp(converted_mac_str,Manasa)) {
         rssi_values[0] =  wifi_apRecord[k].rssi;
		}
		else if(!strcmp(converted_mac_str,Hariom)) {
				rssi_values[1] = wifi_apRecord[k].rssi;
		}
		else if(!strcmp(converted_mac_str,MicronEMS_Guest)) {
				rssi_values[2] =wifi_apRecord[k].rssi;
		}
		/*else if(!strcmp(converted_mac_str,panda)) {
				rssi_values[3] =wifi_apRecord[k].rssi;
		}
		else if(!strcmp(converted_mac_str,amma)) {
				rssi_values[4] = ap_info[k].rssi;
		}
		else if(!strcmp(converted_mac_str,sana)) {
				rssi_values[5] = ap_info[k].rssi;
		}
		else if(!strcmp(converted_mac_str,chandanjio)) {
				rssi_values[6] = ap_info[k].rssi;
		}*/	
			
	}
	return;
}

void Map_Mac_Rssi(wifi_ap_record_t wifi_apRecord[], int wifi_count)
{	
	for(int i=0;(i < wifi_count);i++) 
	{
		char *mac ;//= (char *)Mac_database[i];
		printf("MAC: " MACSTR "\n", MAC2STR(wifi_apRecord[i].bssid));
		for(int j=0;j<DEFAULT_SCAN_LIST_SIZE;j++)
		{
			mac = mac_str(Mac_database[j]);
			if(!strcmp(MACSTR,mac))
			{
				rssi_values[i] =  wifi_apRecord[i].rssi;
				printf("----debug:---macMatchFound %s",mac);
			}
		}
	}

		// char *mac = (char *)wifi_apRecord[k].bssid;
		// printf("Mac : %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n" , mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		// mac_str(wifi_apRecord[k].bssid);
		// printf("%s\n",converted_mac_str);
		
		// if(!strcmp(converted_mac_str,)) 
		// {
		// 	rssi_values[k] =  wifi_apRecord[k].rssi;
		// }
			
	
	return;

}

void search_next_location(int present_ap,int next_location_ap){
	char present_location[60];
	char next_location[60];
	bool check = false;
	strcpy(present_location,config_wifi[present_ap].AccessPoint);
	strcpy(next_location,config_wifi[next_location_ap].AccessPoint);
	for(int i=0;i<Total_WIFI_Config;i++){
		if(!(strcmp(shop_data[i].shop_name,present_location))){
			for(int j=0;j<(Total_WIFI_Config-1);j++){
				if(!(strcmp(shop_data[i].toshop[j].toshop_name,next_location))){
					strcpy(direction_next,shop_data[i].toshop[j].direction);
					direction_deg = shop_data[i].toshop[j].dir_angle;
					check = true;
					return;
				}
			}
		}
	}
	if (check == false){
		strcpy(direction_next,"Reached");
		direction_deg = -1;
		return;		
	}
}

//function to guid direction as left and right
void guid_direction(){
	printf("Heading = %f\t%lf\n",Heading,direction_deg);
	int left , right;
	if(Heading < direction_deg){
		right=direction_deg-Heading;
	}
	else{
		right=(360-direction_deg)+Heading;
	}

	if(Heading < direction_deg){
		left=(360-direction_deg)+Heading;
	}
	else{
		left=Heading-direction_deg;
	}

	if(right <left){
		printf("right\n");
		char stt[100];
		sprintf(stt, "Turn - %s", "Right");
		ssd1306_printFixed (0, 50, stt, STYLE_NORMAL);
	}
	else{
		printf("left\n");
		char stt[100];
		sprintf(stt, "Turn - %s", "Left");
		ssd1306_printFixed (0, 50, stt, STYLE_NORMAL);
	}

}
void find_selected_location() {
    int present_ap=large, next_location_ap = selected - 1; // present location and selected location
	printf("\npresent_ap = %d , next_location_ap = %d\n",present_ap, next_location_ap);
	vTaskDelay(1000/ portTICK_RATE_MS);
	ssd1306_clearScreen();

	ssd1306_printFixed (0, 0, "Follow Direction :", STYLE_BOLD);
	char present_location[60];
	char next_location[60];
	strcpy(present_location,config_wifi[present_ap].AccessPoint);
	strcpy(next_location,config_wifi[next_location_ap].AccessPoint);
	ssd1306_printFixed (0, 16, present_location, STYLE_NORMAL);
	ssd1306_printFixed (58, 16, " -> ", STYLE_NORMAL);
	ssd1306_printFixed (72, 16, next_location, STYLE_NORMAL);

	printf("\nnext_location_ap = %d\n",next_location_ap);
    search_next_location(present_ap,next_location_ap); //to find selected location
	char stt[100];
	char loc[100];
	strcpy(loc,direction_next);
	sprintf(stt, " - %.2lf", direction_deg);
	strcat(loc,stt);
	ssd1306_printFixed (0, 31, loc, STYLE_NORMAL);
	vTaskDelay(100/ portTICK_RATE_MS);

	if (present_ap == next_location_ap){
            printf("You are in the selected location\n");
			ssd1306_clearScreen();
			ssd1306_printFixed (0, 5, direction_next, STYLE_NORMAL);
			ssd1306_printFixed (0, 20, "You are at selected", STYLE_NORMAL);
			ssd1306_printFixed (0, 30, "shop", STYLE_NORMAL);
			ssd1306_printFixed (30, 55, "Thank You", STYLE_BOLD);
			vTaskDelay(1300/ portTICK_RATE_MS);
            return;
        }
			
    while (1) {
		Compass_Read();
        search_next_location(large,next_location_ap); //to find selected location
		for (int i = 0; i < Total_WIFI_Config; ++i) {
        	rssi_values[i]=0;
    	}
		wifi_scan();
		FindLocation(ap_info, ap_count); //find mac and load rssi values
		// Map_Mac_Rssi(ap_info,ap_count);
        near(); //identify the least rssi value 
		vTaskDelay(1/ portTICK_RATE_MS);
        if(large == next_location_ap){
			ssd1306_clearScreen();
			search_next_location(large,next_location_ap);
			ssd1306_printFixed (0, 5, direction_next, STYLE_NORMAL);
			ssd1306_printFixed (0, 20, "You are at selected", STYLE_NORMAL);
			ssd1306_printFixed (0, 30, "shop", STYLE_NORMAL);
			ssd1306_printFixed (30, 55, "Thank You", STYLE_BOLD);
			vTaskDelay(1300/ portTICK_RATE_MS);
            return;
        }
        else{
            printf("Moving\n");
			guid_direction();
        }
		TIMERG0.wdtwprotect.val=TIMG_WDT_WKEY_V;
		TIMERG0.wdtfeed.val=1;
		TIMERG0.wdtwprotect.val=0;
    }
}

//scan wifi task
void task1 (void *arg) 
{
	while(1) 
	{
		if(xSemaphoreTake(xSemaphore, portMAX_DELAY)){
    		wifi_scan();
    		xSemaphoreGive(xSemaphore);
    	}
		vTaskDelay(1/ portTICK_RATE_MS);
	}
	vTaskDelete(NULL);
}

//find
void task2 (void *arg) 
{
	while(1) 
	{
		if(xSemaphoreTake(xSemaphore, portMAX_DELAY))
		{
			vTaskDelay(200/ portTICK_RATE_MS);
			shop_selection();
			printf("Index of selected Shop = %d\n",selected);		
			FindLocation(ap_info, ap_count); // to find the nearest ap
			//find mac and load rssi values
			printf("yyyyyyyyyyyyyyyyyyyyyyyyyyyyyy %d %d %d \n",rssi_values[0] ,rssi_values[1] ,rssi_values[2]);
			near(); // To find the index of highest RSSI value or nearest ap : //identify the least rssi value
			find_selected_location();
			printf("Direction = %s\tDegrees = %f\n",direction_next,direction_deg);
		/*	if (large == 0) {
				printf("\n\nLocation KSPOT-Back\n\n");
				//SSD1306_SetFont( &I2CDisplay, &Font_droid_sans_fallback_15x17 );
				//SSD1306_FontDrawString( &I2CDisplay, 0, 1, "Location :", SSD_COLOR_WHITE );
				//SSD1306_SetFont( &I2CDisplay, &Font_droid_sans_fallback_11x13 );
				//SSD1306_FontDrawString( &I2CDisplay, 0, 30, "KSPOT-Back", SSD_COLOR_WHITE );
				
					//SSD1306_Update( &I2CDisplay );
			}
				else if (large == 1) {
					printf("\n\nLocation KSPOT-FRONT\n\n");
					//SSD1306_SetFont( &I2CDisplay, &Font_droid_sans_fallback_15x17 );
				//SSD1306_FontDrawString( &I2CDisplay, 0, 1, "Location :", SSD_COLOR_WHITE );
				//SSD1306_SetFont( &I2CDisplay, &Font_droid_sans_fallback_11x13 );
				//SSD1306_FontDrawString( &I2CDisplay, 0, 30, "KSPOT-FRONT", SSD_COLOR_WHITE );
					//SSD1306_Update( &I2CDisplay );
				}
				else if (large == 2) {
					printf("\n\nLocation GAMMA-RECEPTION\n\n");
					//SSD1306_SetFont( &I2CDisplay, &Font_droid_sans_fallback_15x17 );
				//SSD1306_FontDrawString( &I2CDisplay, 0, 1, "Location :", SSD_COLOR_WHITE );
				//SSD1306_SetFont( &I2CDisplay, &Font_droid_sans_fallback_11x13 );
				//SSD1306_FontDrawString( &I2CDisplay, 0, 30, "GAMMA-RECEPTION", SSD_COLOR_WHITE );
					//SSD1306_Update( &I2CDisplay );
				}
				else if (large == 3) {
					printf("\n\nLocation PANDA\n\n");
					//SSD1306_SetFont( &I2CDisplay, &Font_droid_sans_fallback_15x17 );
				//SSD1306_FontDrawString( &I2CDisplay, 0, 1, "Location :", SSD_COLOR_WHITE );
				//SSD1306_SetFont( &I2CDisplay, &Font_droid_sans_fallback_11x13 );
				//SSD1306_FontDrawString( &I2CDisplay, 0, 30, "PANDA", SSD_COLOR_WHITE );
					//SSD1306_Update( &I2CDisplay );
				}
				else if (large == 4) {
					printf("\n\nLocation amma\n\n");
					//SSD1306_SetFont( &I2CDisplay, &Font_droid_sans_fallback_15x17 );
				//SSD1306_FontDrawString( &I2CDisplay, 0, 1, "Location :", SSD_COLOR_WHITE );
				//SSD1306_SetFont( &I2CDisplay, &Font_droid_sans_fallback_11x13 );
				//SSD1306_FontDrawString( &I2CDisplay, 0, 30, "Amma", SSD_COLOR_WHITE );
					//SSD1306_Update( &I2CDisplay );
				}
				else if (large == 5) {
					printf("\n\nLocation sana\n\n");
					//SSD1306_SetFont( &I2CDisplay, &Font_droid_sans_fallback_15x17 );
				//SSD1306_FontDrawString( &I2CDisplay, 0, 1, "Location :", SSD_COLOR_WHITE );
				//SSD1306_SetFont( &I2CDisplay, &Font_droid_sans_fallback_11x13 );
				//SSD1306_FontDrawString( &I2CDisplay, 0, 30, "sana", SSD_COLOR_WHITE );
					//SSD1306_Update( &I2CDisplay );
				}
				else if (large == 6) {
					printf("\n\nLocation chandanjio\n\n");
					//SSD1306_SetFont( &I2CDisplay, &Font_droid_sans_fallback_15x17 );
				//SSD1306_FontDrawString( &I2CDisplay, 0, 1, "Location :", SSD_COLOR_WHITE );
				//SSD1306_SetFont( &I2CDisplay, &Font_droid_sans_fallback_11x13 );
				//char stt[100];
				//char loc[100]="chandanjio - ";
				//sprintf(stt, "%.2lf", Heading);
				//strcat(loc,stt);
				//SSD1306_FontDrawString( &I2CDisplay, 0, 30,loc , SSD_COLOR_WHITE );
					//SSD1306_Update( &I2CDisplay );
				}
				
				else {
					printf("\n\n####-Unknown Location-####\n\n");
					//SSD1306_FontDrawString( &I2CDisplay, 35, 30, "###Unknown###", SSD_COLOR_WHITE );
					//SSD1306_Update( &I2CDisplay );
				}
			*/	
			for (int i = 0; i < Total_WIFI_Config; ++i) {
					rssi_values[i]=0;
			}
			xSemaphoreGive(xSemaphore);
			}
				vTaskDelay(500/ portTICK_RATE_MS);
	}
	vTaskDelete(NULL);
}
	
void app_main(void)
{
	wifi_my_config(); //initialization of wifi
	Sensor_Config(); // initialization for magneto meter sensor
    LCD_Init();	//checking I2c Bus connection with OLED display
	LCD_Print();
   
    gpio_set_direction(BUTTON, GPIO_MODE_INPUT); // setting gpio 16 as input fow switch
	gpio_pullup_en(BUTTON);
	
    xSemaphore = xSemaphoreCreateMutex(); //creating semaphore
    	
	xTaskCreatePinnedToCore(task1, "task1", 1024*4, NULL, 6, NULL, 0); // task1
	xTaskCreatePinnedToCore(task2, "task2", 1024*4, NULL, 6, NULL, 1); // task2
	
}