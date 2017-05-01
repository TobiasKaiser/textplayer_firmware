#include "stm32f4xx.h"
#include <string.h>

#include "i2c_master.h"
#include "lcd_vim878.h"


int main(void) {



	i2c_master_init();


	lcd_vim878_init();

	lcd_vim878_update();

	lcd_vim878_test();

}
