#ifndef QMC5883L_H
#define QMC5883L_H

#include "qmc5883l.h"
#include <math.h>


#define Declination       -1.18333 //http://www.magnetic-declination.com 
#define I2C_MASTER_PORT I2C_NUM_0
#define I2C_SDA_PIN     5
#define I2C_CLK_PIN     4
#define PI 3.142857

void Sensor_Config();
static void wait_for_data();
void Compass_Read();

#endif