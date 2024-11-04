#include <stdint.h>
#include "magnetometer.h"
#include "string.h"
// #include "driver/i2c.h"


qmc5883l_t dev;
double Heading;

//compass initialization
void Sensor_Config()
{
	memset(&dev, 0, sizeof(qmc5883l_t));
    ESP_ERROR_CHECK(i2cdev_init());
    //Set slave addr, i2c data and clk pins
	ESP_ERROR_CHECK(qmc5883l_init_desc(&dev, I2C_MASTER_PORT, QMC5883L_I2C_ADDR_DEF, (gpio_num_t)I2C_SDA_PIN, (gpio_num_t)I2C_CLK_PIN));
	//Select continuos mode of operation 
    ESP_ERROR_CHECK(qmc5883l_set_mode(&dev, QMC5883L_MODE_CONTINUOUS));
    // Data Rate :50Hz
    // OverSampleRate :128
    //FullScale Reange: 2g
    ESP_ERROR_CHECK(qmc5883l_set_config(&dev, QMC5883L_DR_50, QMC5883L_OSR_128, QMC5883L_RNG_2));
}

static void wait_for_data()
{
    while (true)
    {
        bool ready;
        ESP_ERROR_CHECK(qmc5883l_data_ready(&dev, &ready));
        if (ready)
            return;
        vTaskDelay(1);
    }
}

//compass function to read values
void Compass_Read()
{
	wait_for_data();
	qmc5883l_data_t data;
        if (qmc5883l_get_data(&dev, &data) == ESP_OK)
        {
            printf("Magnetic data: X:%.2f mG, Y:%.2f mG, Z:%.2f mG\n", data.x, data.y, data.z);
            Heading = atan2((double)data.y, (double)data.x) + Declination;
			if (Heading>2*PI) /* Due to declination check for >360 degree */
				Heading = Heading - 2*PI;
			if (Heading<0)    /* Check for sign */
				Heading = Heading + 2*PI;
			Heading = Heading* 180 / PI;// Convert into angle and return
			printf("Heading = %f\n",Heading);  
        }
        else
            printf("Could not read QMC5883L data\n");
	vTaskDelay(1/portTICK_PERIOD_MS);
}