
#include "lcd.h"
#include <stdint.h>
#include "button.h"

extern uint8_t TotalShops;
// extern struct ShopWifiCombo floorMap[];

const char *menuItems[]; 
SAppMenu menu;

// to print on lcd
void MenuInit()
{
    int i;
    for(i=0;i<TotalShops;i++)
    {
        menuItems[i]=floorMap[i].ShopName;
        #ifdef UPDATE_DATABASE_ENABLED 
        menuItems[TotalShops-1]= "UpdateDatabase";
        #endif
        // printf("----debug:MenuItems%d: %s----\n",i,menuItems[i]);
    }
    
}

void LCD_Init() 
{
    ssd1306_128x64_i2c_init();
    ssd1306_fillScreen(0x00);
    ssd1306_setFixedFont(ssd1306xled_font6x8);
	return;
}

//list of shop print on lcd
void LCD_Print()
{
    /* Initialize main menu state */
	ssd1306_clearScreen();
    ssd1306_createMenu( &menu, menuItems, TotalShops );

    // OLED introduction to project
	ssd1306_printFixedN (14, 1, "LOCATION", STYLE_NORMAL, FONT_SIZE_2X);
	ssd1306_printFixed (38, 30, "TRACKER", STYLE_BOLD);
	ssd1306_printFixed (0, 57, "Bits Pilani", STYLE_NORMAL);

}