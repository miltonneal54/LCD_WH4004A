#include <EEPROM.h>

#include <LCD_WH4004A.h>

const int rs = 12, en1 = 11, en2 = 10, d4 = 2, d5 = 3, d6 = 4, d7 = 5;
LiquidCrystal lcd(rs, en1, en2, d4, d5, d6, d7); // create an object
int line = 0;
/*
 Connections:
 * LCD RS pin to digital pin 12
 * LCD Enable1 pin to digital pin 11
 * LCD Enable2 pin to digital pin 10
 * LCD D4 pin to digital pin 2
 * LCD D5 pin to digital pin 3
 * LCD D6 pin to digital pin 4
 * LCD D7 pin to digital pin 5
 * LCD R/W pin to ground
 * 5K potentiometer:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 12)

 Library originally added 18 Apr 2008
 by David A. Mellis
 library modified 5 Jul 2009
 by Limor Fried (http://www.ladyada.net)
 example added 9 Jul 2009
 by Tom Igoe
 modified 22 Nov 2010
 by Tom Igoe
 modified 7 Nov 2016
 by Arturo Guadalupi
 modified 16 Noc 2020
 by Milton Neal

 This example code is in the public domain.
*/

void setup()
{
  // set up the LCD's number of columns and rows.
  lcd.begin(40, 4);
  lcd.clear();
}

void loop()
{
  // for autoscroll to function correctly for the WH4004A display we must treat it as 2 x 2 line display.
  // set the cursor to the first position of the line to autoscroll.
  for(int x = 0; x < 4; x++)
  {
    lcd.setCursor(0, x);
    // print from 0 to 9
    for (int thisChar = 0; thisChar < 10; thisChar++)
    {
      lcd.print(thisChar);
      delay(500);
    }

    // set the cursor to last postion of the next line or the previous line if it's the last.
    line = x + 1;
    if((line == 2) || (line == 4))
     line -= 2;
     
    lcd.setCursor(40, line);
    // set the display to automatically scroll
    lcd.autoscroll();
    // print from 0 to 9
    for (int thisChar = 0; thisChar < 10; thisChar++)
    {
      lcd.print(thisChar);
      delay(500);
    }
    // turn off automatic scrolling
    lcd.noAutoscroll();
    // clear screen for the next loop:
    lcd.clear();
  }
}
