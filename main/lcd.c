#include "lcd.h"
#include <stdint.h>

const char *menuItems[] =
{
    "vivo",
    "samsung",
	"fibernet"
};

SAppMenu menu;

void LCD_Init() 
{
    ssd1306_128x64_i2c_init();
    ssd1306_fillScreen(0x00);
    ssd1306_setFixedFont(ssd1306xled_font6x8);
	return;
}

void LCD_Print()
{
    /* Initialize main menu state */
	ssd1306_clearScreen();
    ssd1306_createMenu( &menu, menuItems, sizeof(menuItems) / sizeof(char *) );

    // OLED introduction to project
	ssd1306_printFixedN (14, 1, "LOCATION", STYLE_NORMAL, FONT_SIZE_2X);
	ssd1306_printFixed (38, 30, "TRACKER", STYLE_BOLD);
	ssd1306_printFixed (0, 57, "Bits Pilani", STYLE_NORMAL);

}
