#include "Algorithm.h"
#include <stdio.h>
#include <stdint.h>
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "wifi_scan.h"

char converted_mac_str[30];
bool Database_update =false;
bool timesup = false;

//function for convert mac address to string
const char* mac_str(uint8_t bssid[]) 
{
	char ret[30]="";
	char mac_string[10];
	int k;
	for(k= 0; k< 12; k++) {
		sprintf(mac_string, "%.2x", bssid[k]);
		strcat(ret,mac_string);
	}
	ret[k] = '\0';
	strcpy(converted_mac_str, ret);
	return converted_mac_str;
}

void update_database()
{
	int Timer =0;
	while(!timesup)
	{
		Database_update = true;
		printf("Keep Moving...\n");
		wifi_scan();
		vTaskDelay((1000 * portTICK_RATE_MS) * UPDATE_DATABASE_SECS);
		Timer++;
		if(Timer > UPDATE_DATABASE_SECS)
		{
			Database_update =false;
			timesup=true;
			printf("---debug: Updated data base is---\n");
			for(int i=0; i<DEFAULT_SCAN_LIST_SIZE;i++)
			{
				if(Mac_database[i]!=NULL)
					printf("---debug:UpdatedDatabase %s\n",Mac_database[i]);
			}
		}

		
	}
}