#include "LCD_WH4004A.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"

// **********************************************************************
// Modified library for the Winstar WH4004A-xxx-yyy 4 line 40 column LCD
// Milton Neal.
// 15th November 2020
//
// These displays have 2 x HD44780 controllers each enabled by separate
// inputs, E1 on pin 9 and E2 on pin 15.
// Controller E1 for lines 0 and 1 and Controller E2 for lines 2 and 3.
// **********************************************************************

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
// LiquidCrystal constructor is called).

LiquidCrystal::LiquidCrystal(uint8_t rs, uint8_t enable1, uint8_t enable2,
			     uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
			     uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
  init(0, rs, enable1, enable2, d0, d1, d2, d3, d4, d5, d6, d7);
}


LiquidCrystal::LiquidCrystal(uint8_t rs, uint8_t enable1, uint8_t enable2,
			     uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
{
	init(1, rs, enable1, enable2, d0, d1, d2, d3, 0, 0, 0, 0);
}

void LiquidCrystal::init(uint8_t fourbitmode, uint8_t rs, uint8_t enable1, uint8_t enable2,
			 uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
			 uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
  _enable1 = enable1;
  _enable2 = enable2;
  _rs_pin = rs;
  _enable_pin = enable1;
  
  _data_pins[0] = d0;
  _data_pins[1] = d1;
  _data_pins[2] = d2;
  _data_pins[3] = d3; 
  _data_pins[4] = d4;
  _data_pins[5] = d5;
  _data_pins[6] = d6;
  _data_pins[7] = d7; 

  if (fourbitmode)
    _displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;

  else 
    _displayfunction = LCD_8BITMODE | LCD_1LINE | LCD_5x8DOTS;
  
  begin(40, 1);  
}

void LiquidCrystal::begin(uint8_t cols, uint8_t lines, uint8_t dotsize)
{
  if (lines > 1)
    _displayfunction |= LCD_2LINE;

  _numlines = lines;

  setRowOffsets(0x00, 0x40, 0x00, 0x40);

  // for some 1 line displays you can select a 10 pixel high font
  if ((dotsize != LCD_5x8DOTS) && (lines == 1))
    _displayfunction |= LCD_5x10DOTS;

  pinMode(_rs_pin, OUTPUT);
  pinMode(_enable1, OUTPUT);
  pinMode(_enable2, OUTPUT);
  
  // Do these once, instead of every time a character is drawn for speed reasons.
  for (int i = 0; i < ((_displayfunction & LCD_8BITMODE) ? 8 : 4); ++i)
	pinMode(_data_pins[i], OUTPUT);

  // SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
  // according to datasheet, we need at least 40ms after power rises above 2.7V
  // before sending commands. Arduino can turn on way before 4.5V so we'll wait 50
  delayMicroseconds(50000);
  // Initalise both controllers.
  _enable_pin = _enable2;
  initController();
  _enable_pin = _enable1;
  initController();
}

void LiquidCrystal::initController()
{
	// Pull RS and Enable pins low to begin commands.
	digitalWrite(_rs_pin, LOW);
	digitalWrite(_enable1, LOW);
	digitalWrite(_enable2, LOW);

	// Put the LCD into 4 bit or 8 bit mode.
	if (!(_displayfunction & LCD_8BITMODE))
	{
		// This is according to the hitachi HD44780 datasheet figure 24, pg 46
		// Start in 8 bit mode, try to set 4 bit mode.
		write4bits(0x03);
		delayMicroseconds(4500); // wait min 4.1ms

		// second try
		write4bits(0x03);
		delayMicroseconds(4500); // wait min 4.1ms

		// third go!
		write4bits(0x03);
		delayMicroseconds(150);

		// finally, set to 4-bit interface
		write4bits(0x02);
	}

	else
	{
		// This is according to the hitachi HD44780 datasheet page 45 figure 23
		// Send function set command sequence.
		command(LCD_FUNCTIONSET | _displayfunction);
		delayMicroseconds(4500);  // wait more than 4.1ms

		// second try
		command(LCD_FUNCTIONSET | _displayfunction);
		delayMicroseconds(150);

		// third go
		command(LCD_FUNCTIONSET | _displayfunction);
	}

	// Finally, set # lines, font size, etc.
	command(LCD_FUNCTIONSET | _displayfunction);
	// Turn the display on with no cursor or blinking. 
	_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	display();
	// Clear the display.
	clear();
	// Initialize to default text direction (for romance languages)
	_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	// Set the entry mode
	command(LCD_ENTRYMODESET | _displaymode);
}

void LiquidCrystal::setRowOffsets(int row0, int row1, int row2, int row3)
{
  _row_offsets[0] = row0;
  _row_offsets[1] = row1;
  _row_offsets[2] = row2;
  _row_offsets[3] = row3;
}

/********** high level commands, for the user! */
void LiquidCrystal::clear()
{
  _enable_pin = _enable2;
  command(LCD_CLEARDISPLAY);  // clear the lower display, set cursor position to zero
  delayMicroseconds(2000); 

  _enable_pin = _enable1;
  command(LCD_CLEARDISPLAY);  // clear the upper display, set cursor position to zero
  delayMicroseconds(2000);
}

void LiquidCrystal::home()
{
  _enable_pin = _enable1;	// ensure the upper display is enabled
  command(LCD_RETURNHOME);  // set cursor position to zero
  delayMicroseconds(2000);  // this command takes a long time!
}

void LiquidCrystal::setCursor(uint8_t col, uint8_t row)
{
  const size_t max_lines = sizeof(_row_offsets) / sizeof(*_row_offsets);
  if ( row >= max_lines )
    row = max_lines - 1;    // we count rows starting w/0

  if ( row >= _numlines )
    row = _numlines - 1;    // we count rows starting w/0
  
  _enable_pin = _enable1;
  if (row > 1)
	  _enable_pin = _enable2;

  command(LCD_SETDDRAMADDR | (col + _row_offsets[row]));
}

// Turn the display on/off (quickly)
void LiquidCrystal::noDisplay()
{
  _displaycontrol &= ~LCD_DISPLAYON;

  _enable_pin = _enable2;
  command(LCD_DISPLAYCONTROL | _displaycontrol);

  _enable_pin = _enable1;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LiquidCrystal::display()
{
  _displaycontrol |= LCD_DISPLAYON;

  digitalWrite(_enable1, LOW);
  _enable_pin = _enable2;
  command(LCD_DISPLAYCONTROL | _displaycontrol);

  digitalWrite(_enable2, LOW);
  _enable_pin = _enable1;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void LiquidCrystal::noCursor()
{
  _displaycontrol &= ~LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LiquidCrystal::cursor()
{
  _displaycontrol |= LCD_CURSORON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void LiquidCrystal::noBlink()
{
  _displaycontrol &= ~LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LiquidCrystal::blink() {
  _displaycontrol |= LCD_BLINKON;
  command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void LiquidCrystal::scrollDisplayLeft(void)
{
  _enable_pin = _enable2;
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);

  _enable_pin = _enable1;
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void LiquidCrystal::scrollDisplayRight(void)
{
  _enable_pin = _enable2;
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);

  _enable_pin = _enable1;
  command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void LiquidCrystal::leftToRight(void)
{
  _displaymode |= LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void LiquidCrystal::rightToLeft(void)
{
  _displaymode &= ~LCD_ENTRYLEFT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void LiquidCrystal::autoscroll(void)
{
  _displaymode |= LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void LiquidCrystal::noAutoscroll(void)
{
  _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
  command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void LiquidCrystal::createChar(uint8_t location, uint8_t charmap[])
{
  location &= 0x7; // we only have 8 locations 0-7
  command(LCD_SETCGRAMADDR | (location << 3));
  for (int i = 0; i < 8; i++)
    write(charmap[i]);
}

/*********** mid level commands, for sending data/cmds */

inline void LiquidCrystal::command(uint8_t value)
{
  send(value, LOW);
}

inline size_t LiquidCrystal::write(uint8_t value)
{
  send(value, HIGH);
  return 1; // assume sucess
}

/************ low level data pushing commands **********/

// Write either command or data, with automatic 4/8-bit selection
void LiquidCrystal::send(uint8_t value, uint8_t mode)
{
  digitalWrite(_rs_pin, mode);
  if (_displayfunction & LCD_8BITMODE)
    write8bits(value); 
  
  else
  {
    write4bits(value >> 4);
    write4bits(value);
  }
}

void LiquidCrystal::pulseEnable(void)
{
  digitalWrite(_enable_pin, LOW);
  delayMicroseconds(1);    
  digitalWrite(_enable_pin, HIGH);
  delayMicroseconds(1);    // enable pulse must be > 450ns
  digitalWrite(_enable_pin, LOW);
  delayMicroseconds(100);   // commands need > 37us to settle
}

void LiquidCrystal::write4bits(uint8_t value)
{
  for (int i = 0; i < 4; i++)
    digitalWrite(_data_pins[i], (value >> i) & 0x01);

  pulseEnable();
}

void LiquidCrystal::write8bits(uint8_t value)
{
  for (int i = 0; i < 8; i++)
    digitalWrite(_data_pins[i], (value >> i) & 0x01);
  
  pulseEnable();
}
