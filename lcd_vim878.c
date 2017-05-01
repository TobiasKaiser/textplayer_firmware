#include <stdint.h>
#include <string.h>

#include "i2c_master.h"
#include "lcd_ctrl_pcf85176t.h"
#include "delay.h"

uint16_t display_digits[8];

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

static const uint16_t nums[]={
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

static const uint16_t alpha[]={
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

void lcd_vim878_update(void)
{
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
	i2c_master_sendmsg(msg, 20+3);

}

void lcd_vim878_init(void)
{

	uint8_t init_msg[]={
		SLAVE_ADDR,
		CMD_OPCODE_MODE_SET|CMD_MODE_SET_ENABLE|CMD_MODE_SET_THIRD_BIAS|CMD_MODE_SET_4_BP
	};

	i2c_master_sendmsg(init_msg, sizeof(init_msg));
}

void lcd_vim878_test(void)
{

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
			lcd_vim878_update();
			_delay_ms(250);
		}
		_delay_ms(3000);
	}
}


