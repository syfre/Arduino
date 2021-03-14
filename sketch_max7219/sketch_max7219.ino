/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN takes care
  of use the correct LED pin whatever is the board used.
  If you want to know what pin the on-board LED is connected to on your Arduino model, check
  the Technical Specs of your board  at https://www.arduino.cc/en/Main/Products
 
  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
 
  modified 2 Sep 2016
  by Arturo Guadalupi
*/

#include <Max7219.h>

#define pinload   6
#define pindata   5
#define pinclock  7

Max7219 Digits = Max7219(pindata,pinclock,pinload);


long  vlong;
byte  vbyte;
word  vword;
int   vint;
float vfloat;


// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600); //This pipes to the serial monitor
}

// the loop function runs over and over again forever
void loop() {
  while (1) {
    
    vlong  = 1234567;
    Digits.print(vlong);
    delay(1000);

    vlong = 0;
    Digits.print(vlong);
    delay(1000);

    vlong = -1234567;
    Digits.print(vlong);
    delay(1000);

    vbyte  = 128;
    Digits.print(vbyte);
    delay(1000);

    vword  = 65535;
    Digits.print(vword);
    delay(1000);

    vint  = -32768;
    Digits.print(vint);
    delay(1000);

    vint  = +32768;
    Digits.print(vint);
    delay(1000);

    vfloat  = 1.234567;
    Digits.print(vfloat, 6);
    delay(1000);

    vfloat  = -1.234567;
    Digits.print(vfloat, 6);
    delay(1000);
  
    vfloat  = 12345.6789;
    Digits.print(vfloat, 2);
    delay(1000);

    vfloat  = -1234.6789;
    Digits.print(vfloat, 2);
    delay(1000);
  }
}
