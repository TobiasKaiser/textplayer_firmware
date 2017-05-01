#include "stm32f4xx.h"
#include <string.h>


uint16_t display_digits[8];

#define AF04 (4)
#define AF15 (15)

/*******************************************************
 * PCF8576D LCD DRIVER COMMUNICATION INTERFACE
 *******************************************************/
#define CMD_CONTINUE (1<<7)
#define CMD_OPCODE_MODE_SET (1<<6)
#define CMD_OPCODE_LOAD_DATA_POINTER (0)
#define CMD_OPCODE_DEVICE_SELECT ((1<<6)|(1<<5))
#define CMD_OPCODE_BANK_SELECT ((1<<6)|(1<<5)|(1<<4)|(1<<3))
#define CMD_OPCODE_BLINK ((1<<6)|(1<<5)|(1<<4))

#define CMD_MODE_SET_POWER_SAVING (1<<4) // do not use this bit
#define CMD_MODE_SET_ENABLE (1<<3)
#define CMD_MODE_SET_HALF_BIAS (1<<2)
#define CMD_MODE_SET_THIRD_BIAS (0)
#define CMD_MODE_SET_1_BP (1<<0)
#define CMD_MODE_SET_2_BP (1<<1)
#define CMD_MODE_SET_3_BP ((1<<0)|(1<<1))
#define CMD_MODE_SET_4_BP (0)

#define SLAVE_ADDR (0b01110000)

// len has to be >1
void sendmsg(uint8_t *msg, uint32_t len) {
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

/*******************************************************
 * DRIVER <-> LCD CONNECTION TABLE
 *******************************************************/
// columns: parts (col0: DEFCA, col1: LKJI, col2: DPCBA, col3: MNGH), rows: digits 1...8
uint8_t digit_addrs[8][4]={
	{	38,	39,	0,	1	},
	{	36,	37,	2,	3	},
	{	34,	35,	4,	5	},
	{	32,	25,	6,	7	},
	{	22,	23,	8,	9	},
	{	20,	21,	10,	11	},
	{	18,	19,	12,	13	},
	{	16, 17,	14,	15	}
};

/*******************************************************
 * 14 SEGMENT LCD "FONT"
 *******************************************************/
#define D_D  (1<<3)
#define D_E  (1<<2)
#define D_F ( 1<<1)
#define D_CA (1<<0)
#define D_L  (1<<7)
#define D_K  (1<<6)
#define D_J  (1<<5)
#define D_I  (1<<4)
#define D_DP (1<<11)
#define D_C  (1<<10)
#define D_B  (1<<9)
#define D_A  (1<<8)
#define D_M  (1<<15)
#define D_N  (1<<14)
#define D_G  (1<<13)
#define D_H  (1<<12)

const uint16_t nums[]={
	D_A|D_B|D_C|D_D|D_E|D_F|D_N|D_J, 	// 0
	D_B|D_C,							// 1
	D_A|D_B|D_K|D_G|D_E|D_D,			// 2
	D_A|D_B|D_K|D_G|D_C|D_D,			// 3
	D_F|D_B|D_K|D_G|D_C,				// 4
	D_A|D_F|D_K|D_G|D_C|D_D,			// 5
	D_F|D_K|D_G|D_E|D_C|D_D,			// 6
	D_A|D_F|D_B|D_C,					// 7
	D_A|D_B|D_C|D_D|D_E|D_F|D_G|D_K,	// 8
	D_A|D_B|D_C|D_G|D_K|D_F,			// 9
};

const uint16_t alpha[]={
	D_A|D_B|D_C|D_E|D_F|D_G|D_K,		// A
	D_I|D_M|D_K|D_A|D_B|D_C|D_D,		// B
	D_A|D_D|D_E|D_F,					// C
	D_I|D_M|D_A|D_B|D_C|D_D,			// D
	D_A|D_D|D_E|D_F|D_G,				// E
	D_A|D_E|D_F|D_G,					// F
	D_A|D_D|D_E|D_F|D_C|D_K,			// G
	D_F|D_E|D_C|D_B|D_K|D_G,			// H
	D_A|D_I|D_M|D_D,					// I
	D_E|D_D|D_C|D_B,					// J
	D_F|D_E|D_G|D_J|D_L,				// K
	D_F|D_E|D_D,						// L
	D_E|D_F|D_H|D_J|D_B|D_C,			// M
	D_E|D_F|D_H|D_L|D_B|D_C,			// N
	D_A|D_B|D_C|D_D|D_E|D_F,			// O
	D_E|D_F|D_G|D_K|D_A|D_B,			// P
	D_A|D_B|D_C|D_D|D_E|D_F|D_L,		// Q
	D_E|D_F|D_G|D_K|D_A|D_B|D_L,		// R
	D_A|D_H|D_K|D_C|D_D,				// S
	D_I|D_M|D_A,						// T
	D_B|D_C|D_D|D_E|D_F,				// U
	D_F|D_E|D_N|D_J,					// V
	D_E|D_F|D_N|D_L|D_B|D_C,			// W
	D_H|D_J|D_N|D_L,					// X
	D_H|D_J|D_M,						// Y
	D_A|D_J|D_N|D_DP					// Z
};

/*******************************************************
 * LCD UPDATE ROUTINE (displays the contents of display_digits)
 *******************************************************/

void lcd_update(void) {
	uint8_t msg[3+20];
	uint8_t *display_mem=msg+3;
	msg[0]=SLAVE_ADDR;
	// We need to select the device (else the device will not feel responsible for the )
	msg[1]=CMD_OPCODE_DEVICE_SELECT | 0 | CMD_CONTINUE;
	msg[2]=CMD_OPCODE_LOAD_DATA_POINTER | 0;

	memset(display_mem, 0, 20);

	int i,j;
	for(i=0;i<8;i++) {
		//uint8_t col[4];
		for(j=0;j<4;j++) {
			uint8_t nibble=(display_digits[i]>>(4*j))&0xf;
			uint8_t nibbleaddr=digit_addrs[i][j];
			uint8_t byteaddr=nibbleaddr>>1;
			nibbleaddr&=1;
			display_mem[byteaddr]|=nibble<<(4*nibbleaddr);
		}

	}
	sendmsg(msg, 20+3);

}

void lcd_init(void) {

	uint8_t init_msg[]={
		SLAVE_ADDR,
		CMD_OPCODE_MODE_SET|CMD_MODE_SET_ENABLE|CMD_MODE_SET_THIRD_BIAS|CMD_MODE_SET_4_BP
	};

	sendmsg(init_msg, sizeof(init_msg));
}


void _delay_ms(int ms) {
	volatile int i;
	for(i=0;i<100000;i++);
}

int main(void) {

	// Enabled modules: GPIOA, GPIOB, I2C1

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;


	// Set PA10 as output:
	GPIOA->MODER = (GPIOA->MODER&(~GPIO_MODER_MODER10)) | (GPIO_MODER_MODER10_0); // MODER=01 => General purpose output mode
	GPIOA->OTYPER = (GPIOA->OTYPER&(~GPIO_OTYPER_OT_10)); // OTYPER=0 => Push pull
	GPIOA->OSPEEDR = (GPIOA->OSPEEDR&(~GPIO_OSPEEDER_OSPEEDR10)) | (GPIO_OSPEEDER_OSPEEDR10); // OSPEEDR=11 => High speed


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


	lcd_init();

	lcd_update();

	int i, j;

	char limerick[]="THERE WAS A YOUNG LADY FROM CORK WHOSE DAD MADE A FORTUNE IN PORK HE BOUGHT FOR HIS DAUGHTER A TUTOR WHO TAUGHT HER TO BALANCE GREEN PEAS ON HER FORK";
	while(1) {
		for(i=0;i<sizeof(limerick)-8;i++) {
			for(j=0;j<8;j++) {
				char c=limerick[i+j];
				if(c>='A' && c<='Z') {
					display_digits[j]=alpha[c-'A'];
				} else {
					display_digits[j]=0;
				}
			}
			lcd_update();
			_delay_ms(250);
		}
		_delay_ms(3000);
	}

}
