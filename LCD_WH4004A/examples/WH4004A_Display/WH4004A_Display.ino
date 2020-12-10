#include <LCD_WH4004A.h>

const int rs = 12, en1 = 11, en2 = 10, d4 = 2, d5 = 3, d6 = 4, d7 = 5;
LiquidCrystal lcd(rs, en1, en2, d4, d5, d6, d7); // create an object

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

  lcd.begin(40, 4); // initialize the display 4 lines of 40 characters
  lcd.clear();
}

void loop()
{
  lcd.setCursor(0,0);                                     
  lcd.print("This is Line 1");
  lcd.setCursor(0,1);                                     
  lcd.print("This is Line 2");
  lcd.setCursor(0,2);                                     
  lcd.print("This is Line 3");
  lcd.setCursor(0,3);                                     
  lcd.print("This is Line 4");
  // Turn off display.
  lcd.noDisplay();
  delay(1000);
  // Turn on the display.
  lcd.display();
  delay(1000);
}
