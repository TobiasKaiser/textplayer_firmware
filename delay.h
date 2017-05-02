#define _delay_ms(x) delay_ms(x) // avr compatibility layer 8-)

void delay_ms(int ms);

extern volatile int systemTimeMs;
