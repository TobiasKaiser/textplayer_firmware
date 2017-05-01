#include "stm32f4xx.h"

#define AF04 (4)
#define AF15 (15)

// len has to be >1
void i2c_master_sendmsg(uint8_t *msg, uint32_t len)
{
	_delay_ms(0);
	I2C1->CR1|= I2C_CR1_START;
	while(!(I2C1->SR2 & I2C_SR2_MSL)); // wait to get in master mode

	I2C1->DR = *(msg++);
	//while(!(I2C1->SR1 & I2C_SR1_ADDR)); // after the first byte we have to wait for the ADDR bit
	len--;

	while(len>0) {
		while(1) {
			volatile uint32_t sr1, sr2;
			sr1=I2C1->SR1;
			sr2=I2C1->SR2;
			if(sr1 & I2C_SR1_TXE)
				break;
		}
		I2C1->DR = *(msg++);
		len--;
	}

	I2C1->CR1|= I2C_CR1_STOP;
	while((I2C1->SR2 & I2C_SR2_MSL)); // wait to get back into slave mode

}

void i2c_master_init(void)
{
	// Enable clocks for port B and I2C1
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;


	GPIOB->MODER = (GPIOB->MODER&(~GPIO_MODER_MODER8)&(~GPIO_MODER_MODER9)) | (GPIO_MODER_MODER8_1) | (GPIO_MODER_MODER9_1); // MODER=10 => Alternate function output mode
	GPIOB->OTYPER = (GPIOB->OTYPER&~(GPIO_OTYPER_OT_8|GPIO_OTYPER_OT_9)) | (GPIO_OTYPER_OT_8) | (GPIO_OTYPER_OT_9); // OTYPER=1 => Open drain :-)
	GPIOB->OSPEEDR = (GPIOB->OSPEEDR&~(GPIO_OSPEEDER_OSPEEDR8|GPIO_OSPEEDER_OSPEEDR9)) | (GPIO_OSPEEDER_OSPEEDR8)|(GPIO_OSPEEDER_OSPEEDR9); // OSPEEDR=11 => High speed


	// Use PB8 and PB9 as alternate functions AF04, which is I2C1_SCL and I2C1_SDA:
	GPIOB->AFR[1] = (GPIOB->AFR[1] & ~((AF15<<(0*4)) | (AF15<<(1*4)))) | (AF04<<(0*4)) | (AF04<<(1*4));


	I2C1->CR2 |= 16; // 16 MHz APB clock

	I2C1->CR1 &= ~I2C_CR1_PE;
	I2C1->CR1 |= I2C_CR1_SWRST;
	_delay_ms(250);
	I2C1->CR1 &= ~I2C_CR1_SWRST;
	_delay_ms(250);

	I2C1->CCR |=  80;

	I2C1->TRISE = 17;

	I2C1->OAR1 = (1<<14);

	I2C1->CR1 |=  I2C_CR1_PE;
}
