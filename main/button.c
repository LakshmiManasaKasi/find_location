#include "button.h"
#include "ssd1306.h"
#include "driver/gpio.h"
#include "lcd.h"
#include "Algorithm.h"

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

#if UPDATE_DATABASE_ENABLED
uint8_t TotalShops = ARRAY_SIZE(floorMap) + 1U;
#else
uint8_t TotalShops = ARRAY_SIZE(floorMap);
#endif



//selection from menu
void shop_selection()
{
		bool shop_selected =false;
		int i;
		for(i=1; i<TotalShops; i++) //LCD print:MENU
		{
			printf("\nPress %d->%s\n",i,(floorMap[i-1].ShopName));
		}
		#ifdef UPDATE_DATABASE_ENABLED
		printf("\nPress %d->%s\n",i,"UpdateDatabase");
		#endif
		vTaskDelay(100 / portTICK_RATE_MS);
    	ssd1306_clearScreen();
		ssd1306_printFixed (0, 30, "Select a shop :", STYLE_BOLD);
		vTaskDelay(1100 / portTICK_RATE_MS);
    	ssd1306_clearScreen();
		/* show menu on the display */
		MenuInit();
    	ssd1306_showMenu( &menu );
    	button_press = Button_press();
		printf("---debug:buttonPress:%d\n",button_press);
		printf("---debug:TotalShops:%d\n",TotalShops);
		printf("\n");

		for(int i=1; i<= TotalShops; i++)
		{
			if((button_press == i) && (button_press != TotalShops))
			{
				printf("selected %s store\n",(floorMap[i-1].ShopName));
				const char *myString = floorMap[i-1].ShopName;
				ssd1306_printFixed (0, 30, myString, STYLE_BOLD);
				selected=button_press;
				shop_selected =true;
				break;
			}
			#ifdef UPDATE_DATABASE_ENABLED
			if(button_press == TotalShops)
			{
				printf("Updating Database: Move around");
				ssd1306_printFixed (0, 30, "Updating Database. Move!", STYLE_BOLD);
				update_database();
				shop_selected = false;
				// break;
			}
			#endif

		}
		if(shop_selected != true)
		{
			printf("select from above listed stores\n");
			ssd1306_printFixed (0, 30, "select from above listed stores", STYLE_BOLD);
			selected=0;
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
		if(button_press > TotalShops) //needs change ?
		{
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
				if(button_press > TotalShops) //needs change ?
				{
					button_press =0;
				}
				return button_press;
			}
			else
			{
				ESP_LOGI(TAGG, "Short Press");
				button_press = button_press + 1;
				if(button_press > TotalShops) //needs change ?
				{
					button_press =1;
				}
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