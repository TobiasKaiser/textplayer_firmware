#include "stm32f4xx.h"

void my_gpio_init(void)
{
	// Enabled modules: GPIOA, GPIOB, I2C1

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;



	// Set PA10 as output:
	GPIOA->MODER = (GPIOA->MODER&(~GPIO_MODER_MODER10)) | (GPIO_MODER_MODER10_0); // MODER=01 => General purpose output mode
	GPIOA->OTYPER = (GPIOA->OTYPER&(~GPIO_OTYPER_OT_10)); // OTYPER=0 => Push pull
	GPIOA->OSPEEDR = (GPIOA->OSPEEDR&(~GPIO_OSPEEDER_OSPEEDR10)) | (GPIO_OSPEEDER_OSPEEDR10); // OSPEEDR=11 => High speed

}
