#include "LCD.h"
char datab;
void state(int st){
    switch(st){

    case BCM2835_I2C_REASON_OK :
    	printf("ST OK");
    	break;
    case BCM2835_I2C_REASON_ERROR_NACK:
    	printf("NACK");
    	break;
    case BCM2835_I2C_REASON_ERROR_CLKT:
    	printf("TIMEOUT");
    	break;
    case BCM2835_I2C_REASON_ERROR_DATA:
    	printf ("ERROR");
    	break;

    }
}

// float to string
void typeFloat(float myFloat) {
	char buffer[20];
	sprintf(buffer, "%4.2f", myFloat);
	typeln(buffer);
}

// int to string
void typeInt(int i) {
	char array1[20];
	sprintf(array1, "%d", i);
	typeln(array1);
}

// clr lcd go home loc 0x80
void ClrLcd(void) {
	lcd_byte(0x01, LCD_CMD);
	lcd_byte(0x02, LCD_CMD);
}

// go to location on LCD
void lcdLoc(int line) {
	lcd_byte(line, LCD_CMD);
}

// out char to LCD at current position
void typeChar(char val) {
	lcd_byte(val, LCD_CHR);
}


// this allows use of any size string
void typeln(const char *s) {
	while (*s) lcd_byte(*(s++), LCD_CHR);
}

void lcd_byte(int bits, int mode) {

	//Send byte to data pins
	// bits = the data
	// mode = 1 for data, 0 for command
	uint8_t bits_high;
	uint8_t bits_low;
	int st;
	// uses the two half byte writes to LCD
	bits_high = mode | (bits & 0xF0) | LCD_BACKLIGHT;
	bits_low = mode | ((bits << 4) & 0xF0) | LCD_BACKLIGHT;

	// High bits
	st = bcm2835_i2c_read_register_rs(&bits_high,&datab ,1);
	lcd_toggle_enable(bits_high);
	state(st);

	// Low bits
	st = bcm2835_i2c_read_register_rs(&bits_low,&datab ,1);
	lcd_toggle_enable(bits_low);
	state(st);
}

void lcd_toggle_enable(int bits) {
	int st;
	bcm2835_delayMicroseconds(500);
	uint8_t tmp = (bits | ENABLE);
	st= bcm2835_i2c_read_register_rs(&tmp ,&datab ,1);
	state(st);
	bcm2835_delayMicroseconds(500);
	tmp = (bits & ~ENABLE);
	st = bcm2835_i2c_read_register_rs(&tmp,&datab ,1);
	state(st);
	bcm2835_delayMicroseconds(500);
}


void lcd_init() {

    if (!bcm2835_i2c_begin())
    {
      printf("bcm2835_i2c_begin failed. Are you running as root??\n");
      return ;
    }
    bcm2835_i2c_setSlaveAddress(I2C_ADDR);

    bcm2835_i2c_set_baudrate(100000);

	// Initialise display
	lcd_byte(0x33, LCD_CMD); // Initialise
	lcd_byte(0x32, LCD_CMD); // Initialise
	lcd_byte(0x06, LCD_CMD); // Cursor move direction
	lcd_byte(0x0C, LCD_CMD); // 0x0F On, Blink Off
	lcd_byte(0x28, LCD_CMD); // Data length, number of lines, font size
	lcd_byte(0x01, LCD_CMD); // Clear display
	bcm2835_delayMicroseconds(500);
}
