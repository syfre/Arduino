/* Pro Micro Test Code
   by: Nathan Seidle
   modified by: Jim Lindblom
   SparkFun Electronics
   date: September 16, 2013
   license: Public Domain - please use this code however you'd like.
   It's provided as a learning tool.

   This code is provided to show how to control the SparkFun
   ProMicro's TX and RX LEDs within a sketch. It also serves
   to explain the difference between Serial.print() and
   Serial1.print().

   Board : Arduino/Genuino Micro
   COM : COMx Arduino/Genuino Micro
   Reset twice to enter boot loader
*/
#include <TimerThree.h>

#define TXLEDToggle() PORTD ^= (1<<5) 
#define RXLEDToggle() PORTB ^= (1<<0) 
#define pinLED  17

#define freqKhz         1
#define periodus        1000 / freqKhz
#define ticksBySecond   1000 * freqKhz
#define ticksByMilliSeconds   freqKhz

volatile unsigned long milliSeconds;
volatile unsigned long ticks;
volatile unsigned long tickms;

void TimerIRQ(void) {
  //digitalWrite(pinOUT, ! digitalRead(pinOUT));
  ticks++;
  tickms++;
}

void setup()
{
  pinMode(pinLED, OUTPUT);  // Set RX LED as an output
  // TX LED is set as an output behind the scenes
  
  // Start timer
  ticks = 0;
  tickms = 0;
  milliSeconds = 0;
  Timer3.initialize(periodus);
  Timer3.attachInterrupt(TimerIRQ);

  Serial.begin(9600); //This pipes to the serial monitor
  Serial.println("Timer start");  // Print "Hello World" to the Serial Monitor
}

void loop()
{
  if (tickms>=ticksByMilliSeconds) {
    //digitalWrite(pinLED, ! digitalRead(pinLED));
    milliSeconds++;

    digitalWrite(pinLED, ! digitalRead(pinLED));

    tickms = 0;
  }

  if (ticks>=ticksBySecond) {
 
    TXLEDToggle();
    
    ticks = 0;
  }
}


