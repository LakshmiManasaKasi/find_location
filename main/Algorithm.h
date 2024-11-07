#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <stdint.h>
#include <stdbool.h>

#define UPDATE_DATABASE_TIMER   1 //Mins
#define UPDATE_DATABASE_SECS    (UPDATE_DATABASE_TIMER*60) //secs
extern bool Database_update;

const char* mac_str(uint8_t bssid[]) ;
void update_database();

#endif