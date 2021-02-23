#include "lcd.h"

#include <wiringPi.h>
#include <stdarg.h>     /* va_list, va_start, va_arg, va_end */

#define LOW 0
#define HIGH 1


// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set:
//    DL = 1; 8-bit interface data
//    N = 0; 1-line display
//    F = 0; 5x8 dot character font
// 3. Display on/off control:
//    D = 0; Display off
//    C = 0; Cursor off
//    B = 0; Blinking off
// 4. Entry mode set:
//    I/D = 1; Increment by 1
//    S = 0; No shift
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// lcd constructor is called).




void lcdCreate(uint8_t rs,  uint8_t enable,
		uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
{
	rs_pin = rs;
	enable_pin = enable;

	data_pins[0] = d0;
	data_pins[1] = d1;
	data_pins[2] = d2;
	data_pins[3] = d3;

	_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;

	/* lcdBegin(16, 2, LCD_5x8DOTS); // default to 16x2 display */
	delayMicroseconds(15000);          /* 15 ms, Power-On delay*/
    lcdCommand(0x02);    /*send for initialization of LCD with nibble method */
    lcdCommand(0x28);    /*use 2 line and initialize 5*7 matrix in (4-bit mode)*/
    lcdCommand(0x01);    /*clear display screen*/
    lcdCommand(0x0c);    /*display on cursor off*/
    lcdCommand(0x06);    /*increment cursor (shift cursor to right)*/
}

void lcdBegin(uint8_t cols, uint8_t lines, uint8_t dotsize)
{
	if (lines > 1) {
		_displayfunction |= LCD_2LINE;
	}
	_numlines = lines;
	_currline = 0;

	// for some 1 line displays you can select a 10 pixel high font
	if ((dotsize != 0) && (lines == 1)) {
		_displayfunction |= LCD_5x10DOTS;
	}

	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
	delayMicroseconds(50000);
	// Now we pull both RS and R/W low to begin commands
	lcdWrite(rs_pin, LOW);
	lcdWrite(enable_pin, LOW);

	// note: this port supports only 4 bit mode
	//put the LCD into 4 bit or 8 bit mode
	if (! (_displayfunction & LCD_8BITMODE)) {
		// this is according to the hitachi HD44780 datasheet
		// figure 24, pg 46

		// we start in 8bit mode, try to set 4 bit mode
		lcdWrite4bits(0x03);
		delayMicroseconds(4500); // wait min 4.1ms

		// second try
		lcdWrite4bits(0x03);
		delayMicroseconds(4500); // wait min 4.1ms

		// third go!
		lcdWrite4bits(0x03);
		delayMicroseconds(150);

		// finally, set to 4-bit interface
		lcdWrite4bits(0x02);
	}
	else {
		// this is according to the hitachi HD44780 datasheet
		// page 45 figure 23

		// Send function set command sequence
		lcdCommand(LCD_FUNCTIONSET | _displayfunction);
		delayMicroseconds(4500);  // wait more than 4.1ms

		// second try
		lcdCommand(LCD_FUNCTIONSET | _displayfunction);
		delayMicroseconds(150);

		// third go
		lcdCommand(LCD_FUNCTIONSET | _displayfunction);
	}

	// finally, set # lines, font size, etc.
	lcdCommand(LCD_FUNCTIONSET | _displayfunction);

	// turn the display on with no cursor or blinking default
	_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	lcdDisplay();

	// clear it off
	lcdClear();

	// Initialize to default text direction (for romance languages)
	_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	// set the entry mode
	lcdCommand(LCD_ENTRYMODESET | _displaymode);

}

/********** high level commands, for the user! */
void lcdClear()
{
	lcdCommand(LCD_CLEARDISPLAY);  // clear display, set cursor position to zero
	delayMicroseconds(2000);  // this command takes a long time!
}

void lcdHome()
{
	lcdCommand(LCD_RETURNHOME);  // set cursor position to zero
	delayMicroseconds(2000);  // this command takes a long time!
}


// printf syntax like lcd printing
void lcdPrint(const char *format, ...)
{
	char buffer [32];
	va_list argptr;
	va_start(argptr, format);
	vsnprintf (buffer, 32, format, argptr);
	va_end(argptr);
	for (int i = 0; buffer[i] != '\0'; ++i) {
		lcdSend(buffer[i], HIGH);
	}
}

void lcdSetCursor(uint8_t col, uint8_t row)
{
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if ( row >= _numlines ) {
		row = _numlines-1;    // we count rows starting w/0
	}

	lcdCommand(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void lcdNoDisplay()
{
	_displaycontrol &= ~LCD_DISPLAYON;
	lcdCommand(LCD_DISPLAYCONTROL | _displaycontrol);
}
void lcdDisplay()
{
	_displaycontrol |= LCD_DISPLAYON;
	lcdCommand(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void lcdNoCursor()
{
	_displaycontrol &= ~LCD_CURSORON;
	lcdCommand(LCD_DISPLAYCONTROL | _displaycontrol);
}
void lcdCursor()
{
	_displaycontrol |= LCD_CURSORON;
	lcdCommand(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void lcdNoBlink()
{
	_displaycontrol &= ~LCD_BLINKON;
	lcdCommand(LCD_DISPLAYCONTROL | _displaycontrol);
}
void lcdBlink()
{
	_displaycontrol |= LCD_BLINKON;
	lcdCommand(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void lcdScrollDisplayLeft(void)
{
	lcdCommand(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void lcdScrollDisplayRight(void)
{
	lcdCommand(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void lcdLeftToRight(void)
{
	_displaymode |= LCD_ENTRYLEFT;
	lcdCommand(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void lcdRightToLeft(void)
{
	_displaymode &= ~LCD_ENTRYLEFT;
	lcdCommand(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void lcdAutoscroll(void)
{
	_displaymode |= LCD_ENTRYSHIFTINCREMENT;
	lcdCommand(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void lcdNoAutoscroll(void)
{
	_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
	lcdCommand(LCD_ENTRYMODESET | _displaymode);
}

/*********** mid level commands, for sending data/cmds */

inline void lcdCommand(uint8_t value)
{
	lcdSend(value, LOW);
}

/************ low level data pushing commands **********/

// write either command or data
void lcdSend(uint8_t value, uint8_t mode)
{
	lcdWrite(rs_pin, mode); //digitalWrite(_rs_pin, mode);

	lcdWrite4bits(value>>4);
	lcdWrite4bits(value);
}

void lcdPulseEnable(void)
{
	lcdWrite(enable_pin, LOW); //digitalWrite(_enable_pin, LOW);
	delayMicroseconds(1);
	lcdWrite(enable_pin, HIGH); //digitalWrite(_enable_pin, HIGH);
	delayMicroseconds(1);    // enable pulse must be >450ns
	lcdWrite(enable_pin, LOW); //digitalWrite(_enable_pin, LOW);
	delayMicroseconds(100);   // commands need > 37us to settle
}

void lcdWrite4bits(uint8_t value)
{
	for (int i = 0; i < 4; i++) {
		lcdWrite(data_pins[i], (value >> i) & 0x01); //digitalWrite(_data_pins[i], (value >> i) & 0x01);
	}

	lcdPulseEnable();
}

size_t lcdWrite(uint8_t pin, uint8_t value) {
	digitalWrite(pin, value);
}
