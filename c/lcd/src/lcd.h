#ifndef lcd_h
#define lcd_h

#include <stdint.h>
#include <stdio.h>


// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

void lcdCreate(uint8_t rs,  uint8_t enable,
		uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3);

void lcdBegin(uint8_t cols, uint8_t rows, uint8_t charsize); // LCD_5x8DOTS

void lcdClear();
void lcdHome();

void lcdNoDisplay();
void lcdDisplay();
void lcdNoBlink();
void lcdBlink();
void lcdNoCursor();
void lcdCursor();
void lcdScrollDisplayLeft();
void lcdScrollDisplayRight();
void lcdLeftToRight();
void lcdRightToLeft();
void lcdAutoscroll();
void lcdNoAutoscroll();

void lcdCreateChar(uint8_t, uint8_t[]);
void lcdSetCursor(uint8_t, uint8_t);
size_t lcdWrite(uint8_t pin, uint8_t value);
void lcdCommand(uint8_t value);
void lcdPrint(const char *format, ...);

void lcdSend(uint8_t, uint8_t);
void lcdWrite4bits(uint8_t);
void lcdPulseEnable();

uint8_t rs_pin; // LOW(false): command.  HIGH(true): character.
uint8_t enable_pin; // activated by a HIGH pulse.
uint8_t data_pins[4];

uint8_t _displayfunction;
uint8_t _displaycontrol;
uint8_t _displaymode;

uint8_t _initialized;

uint8_t _numlines,_currline;

#endif //lcd_h
