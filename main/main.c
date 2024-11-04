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
// for magneto meter 

// #define PIN_SDA 5 
// #define PIN_CLK 4
extern qmc5883l_t dev;


const char *menuItems[] =
{
    "vivo",
    "samsung",
	"fibernet"
};

/* This variable will hold menu state, processed by SSD1306 API functions */
SAppMenu menu;

// static void wait_for_data()
// {
//     while (true)
//     {
//         bool ready;
//         ESP_ERROR_CHECK(qmc5883l_data_ready(&dev, &ready));
//         if (ready)
//             return;
//         vTaskDelay(1);
//     }
// }
extern double Heading;

static const char *TAGG = "BTN";
#define BUTTON	GPIO_NUM_16 
#define LONG_PRESS_IN_SECONDS 1
static int select_s=1;

// #define DEFAULT_SCAN_LIST_SIZE CONFIG_EXAMPLE_SCAN_LIST_SIZE
static const char *TAG = "scan";
static int selected; // to hold selected AP's Index
SemaphoreHandle_t xSemaphore = NULL;

//mac adddress of kspot related AP's
// const char kspot_back[]="488f5ad9e1bb";
// const char kspot_front[]="488f5ad9e152";
// const char gamma_reception[]="a00460b31f5c";
// const char panda[]="0aba071fde39";
// const char amma[]= "a896757d5256";
// const char sana[]= "ce52a380f612";
// const char chandanjio[]= "c4954d36f500";
// const char appa[]= "c49dvddhdds00";
const char samsung[]="a8ba69a6761d";
const char vivo[]="2248a93111a9";
const char fiberNet[]="d8473239cc35";

#define Total_WIFI_Config 3 //total AP's MAC address
#define ThreshHold -5 //threshold value

//structure for All wifi names
typedef struct {
	char AccessPoint[60];
}allAP;
allAP config_wifi[Total_WIFI_Config]={{"samsung"},{"vivo"},{"fiberNet"}};//wifi names wrt mac address above

static int rssi_values[Total_WIFI_Config]={0,0,0}; // rssi_values to find nearest ap
char converted_mac_str[30];// converted string of mac address of all mac id
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
flourmap shop_data[Total_WIFI_Config] = {{"samsung",{{"vivo","NILL",-1},{"fibernet","NILL",-1}}},
                            {"vivo",{{"fibernet","NILL",-1},{"samsung","NILL",-1}}},
                            {"fibernet",{{"samsung","NILL",-1},{"vivo","NILL",-1}}}};

// function for initialization for oled display
void DefaultBusInit() {
    ssd1306_128x64_i2c_init();
    ssd1306_fillScreen(0x00);
    ssd1306_setFixedFont(ssd1306xled_font6x8);
	return;
}

// //wifi configuration
// void wifi_my_config() {
// 	ESP_ERROR_CHECK(esp_netif_init());
//    	ESP_ERROR_CHECK(esp_event_loop_create_default());
//     	esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
//     	assert(sta_netif);
//     	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
//     	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
//     	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
// }

//wifi scan list
// void wifi_scan(){
// 	memset(ap_info, 0, sizeof(ap_info));
//     		ESP_ERROR_CHECK(esp_wifi_start());
//     		esp_wifi_scan_start(NULL, true);
//     		ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));
//     		ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
//     		ESP_LOGI(TAG, "Total APs scanned = %u", ap_count);
//     		for (int i = 0; (i < DEFAULT_SCAN_LIST_SIZE) && (i < ap_count); i++) {
//         		ESP_LOGI(TAG, "SSID \t\t%s\n", ap_info[i].ssid);
// 				ESP_LOGI(TAG, ": RSSI \t\t%d\n", ap_info[i].rssi);
//     		}
// 			vTaskDelay(10 / portTICK_PERIOD_MS);
// }


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
  	printf("\ntemp = %d\tinde = %d\tdata =%d\tlarge = %d\n",temp,inde,data_2[large], large);
  	if(update) {
		temp = data_2[large];;
		inde= large;
		update = false;    		
	}
	printf("temp = %d\tinde = %d\tdata =%d\tlarge = %d\n",temp,inde,data_2[large], large);

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

//function for convert mac address to string
void mac_str(uint8_t bssid[]) {
	char ret[30]="";
	char mac_string[10];
	int k;
	for(k= 0; k< 12; k++) {
		sprintf(mac_string, "%.2x", bssid[k]);
		strcat(ret,mac_string);
	}
	ret[k] = '\0';
	strcpy(converted_mac_str, ret);
	return;
}

//function to find the current location or nearest ap to device
void FindLocation(wifi_ap_record_t disp_info[], int wifi_count){
	for(int k=0;(k < DEFAULT_SCAN_LIST_SIZE) && (k < wifi_count);k++) {
		char *mac = (char *)disp_info[k].bssid;
		printf("Mac : %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n" , mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		mac_str(disp_info[k].bssid);
		printf("%s\n",converted_mac_str);
		
			if(!strcmp(converted_mac_str,samsung)) {
				rssi_values[0] =  disp_info[k].rssi;
			}
			else if(!strcmp(converted_mac_str,vivo)) {
				rssi_values[1] = disp_info[k].rssi;
			}
			else if(!strcmp(converted_mac_str,fiberNet)) {
				rssi_values[2] =disp_info[k].rssi;
			}
			/*else if(!strcmp(converted_mac_str,panda)) {
				rssi_values[3] =disp_info[k].rssi;
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

// function for selection from menu using button
static int sel()
{
    uint16_t ticks = 0;
	ESP_LOGI(TAGG, "Waiting For Press.");
	for (;;) 
	{
		if(select_s >= Total_WIFI_Config){
		select_s =0;
		}
		// Wait here to detect press
		while( gpio_get_level(BUTTON) )
		{
			vTaskDelay(125 / portTICK_PERIOD_MS);
		}
		
		// Debounce
		vTaskDelay(50 / portTICK_PERIOD_MS);

		// Re-Read Button State After Debounce
		if (!gpio_get_level(BUTTON)) 
		{
			ESP_LOGI(TAGG, "BTN Pressed Down.");
			
			ticks = 0;
		
			// Loop here while pressed until user lets go, or longer that set time
			while ((!gpio_get_level(BUTTON)) && (++ticks < LONG_PRESS_IN_SECONDS * 100))
			{
				vTaskDelay(10 / portTICK_PERIOD_MS);
			} 

			// Did fall here because user held a long press or let go for a short press
			if (ticks >= LONG_PRESS_IN_SECONDS * 100)
			{
				ESP_LOGI(TAGG, "Long Press");
				//one_flag = 0;
				ssd1306_clearScreen();
				return select_s;
			}
			else
			{
				ESP_LOGI(TAGG, "Short Press");
				select_s = select_s + 1;
				printf("%d\n",select_s);
				ssd1306_menuDown( &menu );
                ssd1306_updateMenu( &menu );
			}

			// Wait here if they are still holding it
			while(!gpio_get_level(BUTTON))
			{
				vTaskDelay(100 / portTICK_PERIOD_MS);
			}
			
			ESP_LOGI(TAGG, "BTN Released.");
		}
	}
}

//selection from menu
void selection(){
		printf("\nPress 1->Samsung\t2->ViVo\t3->Fibernet\t0->exit\n\t\tSelect Store :: ");
		vTaskDelay(100 / portTICK_RATE_MS);
    	ssd1306_clearScreen();
		ssd1306_printFixed (0, 30, "Select the shop :", STYLE_BOLD);
		vTaskDelay(1100 / portTICK_RATE_MS);
    	ssd1306_clearScreen();
		/* show menu on the display */
    	ssd1306_showMenu( &menu );
    	select_s = sel();
		printf("\n");
		switch(select_s){
			case 1:printf("selected Samsung store\n");
					ssd1306_printFixed (0, 30, "selected Samsung store", STYLE_BOLD);
					selected=select_s;
					break;
			case 2:printf("selected ViVo store\n");
					ssd1306_printFixed (0, 30, "selected ViVo store", STYLE_BOLD);
					selected=select_s;
					break;
			case 3:printf("selected Fibernet store\n");
					ssd1306_printFixed (0, 30, "selected Fibernet store", STYLE_BOLD);
					selected=select_s;
					break;
			/*case 4:printf("selected Panda store\n");
					ssd1306_printFixed (0, 30, "selected Panda store", STYLE_BOLD);
					selected=select_s;
					break;
			case 5:printf("selected Amma store\n");
					ssd1306_printFixed (0, 30, "selected Amma store", STYLE_BOLD);
					selected=select_s;
					break;
			case 6:printf("selected Sana store\n");
					ssd1306_printFixed (0, 30, "selected Sana store", STYLE_BOLD);
					selected=select_s;
					break;
			case 7:printf("selected jio_fiber store\n");
					ssd1306_printFixed (0, 30, "selected jio_fiber store", STYLE_BOLD);
					selected=select_s;
					break;
			case 8:printf("selected Appa store\n");
					ssd1306_printFixed (0, 30, "selected Appa store", STYLE_BOLD);
					selected=select_s;
					break;*/
			default:printf("select from above listed store\n");
					ssd1306_printFixed (0, 30, "select from above listed store", STYLE_BOLD);
					break;
		}
		vTaskDelay(500 / portTICK_RATE_MS);
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
		FindLocation(ap_info, ap_count);
        near();
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
void task1 (void *arg) {
	while(1) {
		if(xSemaphoreTake(xSemaphore, portMAX_DELAY)){
    		wifi_scan();
    		xSemaphoreGive(xSemaphore);
    	}
		vTaskDelay(1/ portTICK_RATE_MS);
	}
	vTaskDelete(NULL);
}

//find
void task2 (void *arg) {
	while(1) {
	if(xSemaphoreTake(xSemaphore, portMAX_DELAY)){
		vTaskDelay(200/ portTICK_RATE_MS);
		selection();
		printf("Seleeeeeeeee = %d\n",selected);		
		FindLocation(ap_info, ap_count); // to find the nearest ap
		printf("yyyyyyyyyyyyyyyyyyyyyyyyyyyyyy %d %d %d \n",rssi_values[0] ,rssi_values[1] ,rssi_values[2]);
		near(); // To find the index of highest RSSI value or nearest ap
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
    DefaultBusInit();	//checking I2c Bus connection with OLED display

    /* Initialize main menu state */
	ssd1306_clearScreen();
    ssd1306_createMenu( &menu, menuItems, sizeof(menuItems) / sizeof(char *) );

    // OLED introduction to project
	ssd1306_printFixedN (14, 1, "LOCATION", STYLE_NORMAL, FONT_SIZE_2X);
	ssd1306_printFixed (38, 30, "TRACKER", STYLE_BOLD);
	ssd1306_printFixed (0, 57, "Bits Pilani", STYLE_NORMAL);

    gpio_set_direction(BUTTON, GPIO_MODE_INPUT); // setting gpio 16 as input fow switch
	gpio_pullup_en(BUTTON);
	
    xSemaphore = xSemaphoreCreateMutex(); //creating semaphore
    	
	xTaskCreatePinnedToCore(task1, "task1", 1024*4, NULL, 6, NULL, 0); // task1
	xTaskCreatePinnedToCore(task2, "task2", 1024*4, NULL, 6, NULL, 1); // task2
}
