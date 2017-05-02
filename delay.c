#include "stm32f4xx.h"
#include "core_cm4.h"


#include "delay.h"


volatile int systemTimeMs=0;

void SysTick_Handler (void)
{
  systemTimeMs++;
}

void systick_init()
{
	SysTick->LOAD = 16000;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk;
}


void delay_ms(int ms)
{
	int delayEndMs = systemTimeMs + ms;
	while(delayEndMs >= systemTimeMs); // This delays _at least_ ms. Replace >= by > to delay _at most_ ms.
}
