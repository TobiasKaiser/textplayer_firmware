enum button {
	BUTTON_NONE=0,
	BUTTON_UP=(1<<0),
	BUTTON_DOWN=(1<<1),
	BUTTON_LEFT=(1<<2),
	BUTTON_RIGHT=(1<<3),
	BUTTON_A=(1<<4),
	BUTTON_B=(1<<5)
};

void buttons_init(void);

enum button buttons_getkey(void);
