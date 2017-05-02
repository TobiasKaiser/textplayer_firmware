#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "i2c_master.h"
#include "lcd_ctrl_pcf85176t.h"
#include "delay.h"

uint16_t lcd_vim878_digits[8];

/*******************************************************
 * DRIVER <-> LCD CONNECTION TABLE
 *******************************************************/
// columns: parts (col0: DEFCA, col1: LKJI, col2: DPCBA, col3: MNGH), rows: digits 1...8
uint8_t digit_addrs[8][4]={
	{	24,	27,	22,	25	},
	{	26,	29,	20,	23	},
	{	28,	31,	18,	21	},
	{	30,	33,	16,	19	},
	{	6,	7,	14,	17	},
	{	4,	5,	12,	15	},
	{	2,	3,	10,	13	},
	{	0,	1,	8,	11	}
/*
	{	25,	26,	23,	24	},
	{	27,	28,	21,	22	},
	{	29, 30,	19,	20	},
	{	31,	32,	17,	18	},
	{	7,	6,	15,	16	},
	{	5,	4,	13,	14	},
	{	3,	2,	11,	12	},
	{	1,	0,	9,	10	}
*/
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
			uint8_t nibble=(lcd_vim878_digits[i]>>(4*j))&0xf;
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
					lcd_vim878_digits[j]=alpha[c-'A'];
				} else {
					lcd_vim878_digits[j]=0;
				}
			}
			lcd_vim878_update();
			_delay_ms(250);
		}
		_delay_ms(3000);
	}
}

void lcd_vim878_puts(char *str)
{
	bool flag_apostrophe=false;
	bool last_inc_skipped=false;
	bool string_end_reached=false;

	memset(lcd_vim878_digits, 0, 8*sizeof(uint16_t));

	int char_idx=0;
	while(char_idx<8) {
		char c = *(str++);
		if(c=='\0') {
			string_end_reached=true;
			break;
		}
		else if(c>='A' && c<='Z') {
			lcd_vim878_digits[char_idx] |= alpha[c-'A'];
			char_idx++;
			last_inc_skipped=false;
			flag_apostrophe=false;
		}
		else if(c>='a' && c<='z') {
			lcd_vim878_digits[char_idx] |= alpha[c-'a'];
			char_idx++;
			last_inc_skipped=false;
			flag_apostrophe=false;
		}
		else if(c>='0' && c<='9') {
			lcd_vim878_digits[char_idx] |= nums[c-'0'];
			char_idx++;
			last_inc_skipped=false;
			flag_apostrophe=false;
		}
		else if(c=='.' || c==',') {
			if(char_idx==0) {
				// We have to leave out the first digit here then
				char_idx++;
			}
			if(last_inc_skipped) {
				char_idx++;
			}
			lcd_vim878_digits[char_idx - 1] |= D_DP;

			last_inc_skipped=true;
			flag_apostrophe=false;
		}
		else if(c=='\'') {
			if(last_inc_skipped) {
				char_idx++;
			}
			lcd_vim878_digits[char_idx] |= D_CA;

			last_inc_skipped=true;
			flag_apostrophe=true;
		}
		else if(c=='r') {
			// ignore this, so that \n\r does not lead to double white space.
		}
		else {
			// special characters
			uint16_t out;
			switch(c) {
			case ' ':
			case '\t':
			case '\n':
				out=0;
				break;
			case '\\':
				out=D_H | D_L;
				break;
			case '/':
				out=D_N | D_J;
				break;
			case '_':
				out=D_D;
				break;
			case '-':
				out=D_G | D_K;
				break;
			case '=':
				out=D_G | D_K | D_D;
				break;
			default:
				// fallback character:
				out=0xffff & (~D_DP) & (~D_CA);
			}
			lcd_vim878_digits[char_idx] |= out;
			char_idx++;
			last_inc_skipped=false;
			flag_apostrophe=false;
		}
	}

	if(!string_end_reached && (*str)=='.') {
		lcd_vim878_digits[char_idx - 1] |= D_DP;
	}


}


