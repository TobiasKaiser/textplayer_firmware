#include "stm32f4xx.h"

int main(void) {

	// Code to toggle PA10:
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

	GPIOA->MODER = (GPIOA->MODER&(~GPIO_MODER_MODER10)) | (GPIO_MODER_MODER10_0); // MODER=01 => General purpose output mode
	GPIOA->OTYPER = (GPIOA->OTYPER&(~GPIO_OTYPER_OT_10)); // OTYPER=0 => Push pull
	GPIOA->OSPEEDR = (GPIOA->OSPEEDR&(~GPIO_OSPEEDER_OSPEEDR10)) | (GPIO_OSPEEDER_OSPEEDR10); // OSPEEDR=11 => High speed


	while(1) {
		GPIOA->BSRR = GPIO_BSRR_BS_10;
		GPIOA->BSRR = GPIO_BSRR_BR_10;
	}

}