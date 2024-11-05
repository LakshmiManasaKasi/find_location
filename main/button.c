#include "button.h"
#include "ssd1306.h"
#include "driver/gpio.h"
#include "lcd.h"

extern SAppMenu menu;

static int button_press=1;
static int Button_press();
int selected =0;
static const char *TAGG = "BTN";



struct ShopWifiCombo floorMap[] = 
{
    {"Puma", "Puma123"},
    {"Foreever21", "Foreever123"},
    {"Nike", "Nike123"}
};

uint8_t TotalShops = ARRAY_SIZE(floorMap);

//selection from menu
void shop_selection()
{
		for(int i=0; i<TotalShops; i++)
		{
			printf("\nPress %d->%s\n",i+1,(floorMap[i].ShopName));
		}
		vTaskDelay(100 / portTICK_RATE_MS);
    	ssd1306_clearScreen();
		ssd1306_printFixed (0, 30, "Selected shop :", STYLE_BOLD);
		vTaskDelay(1100 / portTICK_RATE_MS);
    	ssd1306_clearScreen();
		/* show menu on the display */
		MenuInit();
    	ssd1306_showMenu( &menu );
    	button_press = Button_press();
		printf("\n");
		switch(button_press){
			case 1:printf("selected Puma store\n");
					ssd1306_printFixed (0, 30, "selected Puma store", STYLE_BOLD);
					selected=button_press;
					break;
			case 2:printf("selected Forever21 store\n");
					ssd1306_printFixed (0, 30, "selected Forever21 store", STYLE_BOLD);
					selected=button_press;
					break;
			case 3:printf("selected Nike store\n");
					ssd1306_printFixed (0, 30, "selected Nike store", STYLE_BOLD);
					selected=button_press;
					break;
			/*case 4:printf("selected Panda store\n");
					ssd1306_printFixed (0, 30, "selected Panda store", STYLE_BOLD);
					selected=button_press;
					break;
			case 5:printf("selected Amma store\n");
					ssd1306_printFixed (0, 30, "selected Amma store", STYLE_BOLD);
					selected=button_press;
					break;
			case 6:printf("selected Sana store\n");
					ssd1306_printFixed (0, 30, "selected Sana store", STYLE_BOLD);
					selected=button_press;
					break;
			case 7:printf("selected jio_fiber store\n");
					ssd1306_printFixed (0, 30, "selected jio_fiber store", STYLE_BOLD);
					selected=button_press;
					break;
			case 8:printf("selected Appa store\n");
					ssd1306_printFixed (0, 30, "selected Appa store", STYLE_BOLD);
					selected=button_press;
					break;*/
			default:printf("select from above listed store\n");
					ssd1306_printFixed (0, 30, "select from above listed stores", STYLE_BOLD);
					break;
		}
		vTaskDelay(500 / portTICK_RATE_MS);
}


// function for selection from menu using button
static int Button_press()
{
    uint16_t ticks = 0;
	ESP_LOGI(TAGG, "Waiting For Press.");
	for (;;) 
	{
		if(button_press >= Total_WIFI_Config){
		button_press =0;
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
				return button_press;
			}
			else
			{
				ESP_LOGI(TAGG, "Short Press");
				button_press = button_press + 1;
				printf("%d\n",button_press);
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