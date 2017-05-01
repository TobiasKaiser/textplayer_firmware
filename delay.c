#include "delay.h"

void _delay_ms(int ms)
{
	volatile int i;
	for(i=0;i<100000;i++);
}
