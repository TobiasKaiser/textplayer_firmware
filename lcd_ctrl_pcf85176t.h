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

