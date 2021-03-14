/*************************************************************************************
    UTFT_ViewFont (C)2013 Henning Karlsen
    web: http://www.henningkarlsen.com/electronics
    Fonts: http://www.henningkarlsen.com/electronics/r_fonts.php
  
    This program is a demo of the included fonts Plus a few downloaded ones.
    This demo was made for modules with a screen resolution 
    of 320x240 pixels.
    This program requires the UTFT library.
**************************************************************************************

    AliExpress ILI9341 2.2 Inch 240*320 SPI TFT LCD  Display
    http://www.aliexpress.com/item/Wholesale-1PC-2-2-Inch-240-320-Dots-SPI-TFT-LCD-Serial-Port-Module-Display-ILI9341/1188518505.html
    
    ILI9341 ............. Teensy 3.x
    VCC ................. 3.3V
    GND ................. Ground
    CS .................. 5
    RESET ............... 4
    D/C ................. 6
    SDO (MOSI) .......... 16
    SCK ................. 15
    LED ................. 100 Ohm -> 5V
    SDO (MISO) .......... 14

***************************************************************************************/

#include <UTFT.h>

// Declare which fonts we will be using

extern uint8_t arial_bold[];
extern uint8_t Inconsola[];
extern uint8_t SevenSegNumFont[];

#define _CS		5
#define _RST	4
#define _DC		6
#define	_MOSI	16
#define	_SCK	15
#define _MISO	14

// ILI9341 on Teensy 3.1
UTFT myGLCD(ILI9341_S5P,_MOSI,_SCK,_CS,_RST,_DC);   // Remember to change the model parameter to suit your display module!

void setup()
{

  myGLCD.InitLCD();
  myGLCD.clrScr();
}

void loop()
{
 
  for (int i=319; i>20; i=i-2){
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.setColor(255, 31, 31);
  //myGLCD.setFont(arial_bold);
  myGLCD.print("ILI9341", i, 30, 0);       // Print String   x=i, y=30, rotation angle = 0
  myGLCD.print("Teensy 3.x", i, 50, 0);    // Print String   x=i, y=50, rotation angle = 0
  }

  myGLCD.setColor(0, 255, 255);
  //myGLCD.setFont(SevenSegNumFont);

  for (int i=0; i<250; i++){
  myGLCD.printNumI(i,20,90,3,48);        // Print Integer Number, x=20, y=90, 4 characters, 48= "0" (filler) 
   }
   
  myGLCD.setColor(255, 127, 0);
  //myGLCD.setFont(Inconsola);  
  for (float f=0; f<10000; f=f+0.01){
  myGLCD.printNumF(f,2,20,170,46,8,48);    // Print Floating Number   2 decimal places, x=20, y=170, 46 = ".", 8 characters, 48= "0" (filler)
  }
  }
