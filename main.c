#include "stm32f4xx.h"
#include <string.h>
#include "delay.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "i2c_master.h"
#include "lcd_vim878.h"
#include "buttons.h"


void example_simple_counter(void)
{
	while(1) {
		char buf[20];
		sprintf(buf, "t=%06i", systemTimeMs);
		lcd_vim878_puts(buf);
		lcd_vim878_update();

	}
}

void example_print_button(void)
{
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


union func_arg {
	struct menu_item *menu_item; // null terminated array
	char *text;
};

struct menu_item {
	char *label;
	void (*handler)(union func_arg);
	union func_arg handler_arg;
};

void func_text(union func_arg arg) {
	char *text = arg.text;

	char *cur=text;
	while(1) {
		lcd_vim878_puts(cur);
		lcd_vim878_update();
		switch(buttons_getkey()) {
		case BUTTON_B:
			return;
		case BUTTON_LEFT:
			if(cur!=text) {
				cur--;
			}
			break;
		case BUTTON_RIGHT:
			if((*cur)!='\0') {
				cur++;
			}
			break;
		}
	}

}

void func_timer(union func_arg arg)
{
	bool running=false;

	int counter=0;
	int lastSystemTimeMs=systemTimeMs;

	while(1) {
		char buf[32];

		sprintf(buf, "t=%i.%02i", counter/1000, (counter%1000)/10);

		lcd_vim878_puts(buf);
		lcd_vim878_update();


		switch(running?buttons_getkey_timeout(50):buttons_getkey()) {
		case BUTTON_B:
			return;
		case BUTTON_A:
			running = !running;
			lastSystemTimeMs=systemTimeMs;
			break;
		}

		int nowSystemTimeMs=systemTimeMs;

		int delta=nowSystemTimeMs-lastSystemTimeMs;
		if(running) {
			counter+=delta;
		}

		lastSystemTimeMs=nowSystemTimeMs;
	}
}

void func_menu(union func_arg arg)
{
	struct menu_item *start=arg.menu_item;



	struct menu_item *cur=start;
	while(1) {
		lcd_vim878_puts(cur->label);
		lcd_vim878_update();
		switch(buttons_getkey()) {
		case BUTTON_B:
			return;
		case BUTTON_A:
			cur->handler(cur->handler_arg);
			break;
		case BUTTON_UP:
			if(cur==start) {
				for(;cur->label;cur++);
				cur--;
			} else {
				cur--;
			}
			break;
		case BUTTON_DOWN:
			cur++;
			if(!(cur->label)) {
				cur=start;
			}
			break;
		}


	}

}

int main(void)
{
	systick_init();

	i2c_master_init();


	lcd_vim878_init();

	buttons_init();

	lcd_vim878_update();


	struct menu_item main_menu[]={
			{"itext", func_text, {.text="Hello world asdasd hfjsdkhfjs"}},
			{"etext", func_text, {.text="not implemented yet"}},
			{"memdump", func_text, {.text="not implemented yet"}},
			{"timer", func_timer, {.text=NULL}},
			{NULL, NULL, {.text=NULL}}
	};

	while(1) {
		union func_arg arg;
		arg.menu_item=main_menu;

		func_menu(arg);
	}
}
