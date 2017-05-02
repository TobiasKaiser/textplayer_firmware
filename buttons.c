#include "stm32f4xx.h"

#include "buttons.h"
#include "delay.h"

void buttons_init(void)
{
	// Enable modules GPIOA, GPIOB
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;


	// Set PA3, PA4, PA5, PA6, PA7, PB0 as input with internal pull up:

	// MODER[1:0]=00

	GPIOA->MODER &=
			~GPIO_MODER_MODER3
			& ~GPIO_MODER_MODER4
			& ~GPIO_MODER_MODER5
			& ~GPIO_MODER_MODER6
			& ~GPIO_MODER_MODER7;
	GPIOB->MODER &= ~GPIO_MODER_MODER0;

	// PUPDR[1:0]=01

	GPIOA->PUPDR &=
			~GPIO_PUPDR_PUPDR3_1
			& ~GPIO_PUPDR_PUPDR4_1
			& ~GPIO_PUPDR_PUPDR5_1
			& ~GPIO_PUPDR_PUPDR6_1
			& ~GPIO_PUPDR_PUPDR7_1;
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR0_1;

	GPIOA->PUPDR |=
			GPIO_PUPDR_PUPDR3_0
			| GPIO_PUPDR_PUPDR4_0
			| GPIO_PUPDR_PUPDR5_0
			| GPIO_PUPDR_PUPDR6_0
			| GPIO_PUPDR_PUPDR7_0;
	GPIOB->PUPDR |= GPIO_PUPDR_PUPDR0_0;
}

enum button buttons_getkey(void) {

	uint32_t idr_a, idr_b, last_idr_a, last_idr_b;
	last_idr_a=GPIOA->IDR;
	last_idr_b=GPIOB->IDR;
	while(1) {
		uint32_t pressed_a, pressed_b;
		// uint32_t released_a, released_b;

		idr_a=GPIOA->IDR;
		idr_b=GPIOB->IDR;

		// inverse logic here because 1=released and 0=pressed
		pressed_a = last_idr_a & ~idr_a;
		pressed_b = last_idr_b & ~idr_b;
		//released_a = last_idr_a & ~idr_a;
		//released_b = last_idr_b & ~idr_b;

		if(pressed_a & (1<<3)) {
			return BUTTON_UP;
		}
		else if(pressed_a & (1<<4)) {
			return BUTTON_RIGHT;
		}
		else if(pressed_a & (1<<5)) {
			return BUTTON_LEFT;
		}
		else if(pressed_a & (1<<6)) {
			return BUTTON_DOWN;
		}
		else if(pressed_a & (1<<7)) {
			return BUTTON_B;
		}
		else if(pressed_b & (1<<0)) {
			return BUTTON_A;
		}

		delay_ms(10);
		last_idr_a=idr_a;
		last_idr_b=idr_b;
	}
}
