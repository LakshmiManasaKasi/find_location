#include "Algorithm.h"
#include <stdio.h>
#include "string.h"

char converted_mac_str[30];

//function for convert mac address to string
void mac_str(uint8_t bssid[]) 
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
	return;
}
