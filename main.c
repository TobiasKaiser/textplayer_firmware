#include "stm32f4xx.h"
#include <string.h>
#include "delay.h"
#include <stdio.h>

#include "i2c_master.h"
#include "lcd_vim878.h"
#include "buttons.h"


int main(void) {
	systick_init();

	i2c_master_init();


	lcd_vim878_init();

	buttons_init();

	lcd_vim878_update();

	/*
	while(1) {
		char buf[20];
		sprintf(buf, "t=%06i", systemTimeMs);
		lcd_vim878_puts(buf);
		lcd_vim878_update();

	}
	*/
	int i=0;
	while(1) {
		enum button b;

		b=buttons_getkey();

		char *out;
		switch(b) {
		case BUTTON_UP:
			out="up";
			break;
		case BUTTON_LEFT:
			out="left";
			break;
		case BUTTON_RIGHT:
			out="right";
			break;
		case BUTTON_DOWN:
			out="down";
			break;
		case BUTTON_A:
			out="a";
			break;
		case BUTTON_B:
			out="b";
			break;
		default:
			out="unknown";
		}

		char buf[20];
		sprintf(buf, "%i%s", i, out);
		i++;

		lcd_vim878_puts(buf);
		lcd_vim878_update();
	}

}
