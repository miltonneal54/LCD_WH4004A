#include <LCD_WH4004A.h>

const int rs = 12, en1 = 11, en2 = 10, d4 = 2, d5 = 3, d6 = 4, d7 = 5;
LiquidCrystal lcd(rs, en1, en2, d4, d5, d6, d7);

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
  // set up the LCD's number of columns and rows
  lcd.begin(40, 4);
  // Print a message to the LCD.
  lcd.setCursor(0,0);                                     
  lcd.print("This is Line 1");
  lcd.setCursor(0,1);                                     
  lcd.print("This is Line 2");
  lcd.setCursor(0,2);                                     
  lcd.print("This is Line 3");
  lcd.setCursor(0,3);                                     
  lcd.print("This is Line 4");
  delay(1000);
}

void loop()
{
  // scroll 14 positions (string length) to the left
  // to move it offscreen left
  for (int positionCounter = 0; positionCounter < 14; positionCounter++)
  {
    // scroll one position left:
    lcd.scrollDisplayLeft();
    // wait a bit:
    delay(150);
  }

  // scroll 54 positions (string length + display length) to the right
  // to move it offscreen right:
  for (int positionCounter = 0; positionCounter < 54; positionCounter++)
  {
    // scroll one position right
    lcd.scrollDisplayRight();
    // wait a bit:
    delay(150);
  }

  // scroll 27 positions (display length + string length / 2) to the left
  // to move it back to center
  for (int positionCounter = 0; positionCounter < 27; positionCounter++)
  {
    // scroll one position left
    lcd.scrollDisplayLeft();
    // wait a bit:
    delay(150);
  }

  // stop at the center a bit
  delay(1000);

  // scroll 13 positions (display length + string length / 2) to the left
  // to move it back to starting left:
  for (int positionCounter = 0; positionCounter < 13; positionCounter++)
  {
    // scroll one position left
    lcd.scrollDisplayLeft();
    // wait a bit:
    delay(150);
  }

   // delay at the end of the full loop:
  delay(2000);
}
