/*
  Arduino TFT text example

  This example demonstrates how to draw text on the
  TFT with an Arduino. The Arduino reads the value
  of an analog sensor attached to pin A0, and writes
  the value to the LCD screen, updating every
  quarter second.

  This example code is in the public domain

  Created 15 April 2013 by Scott Fitzgerald

  http://www.arduino.cc/en/Tutorial/TFTDisplayText

 */

#include <TFT.h>  // Arduino LCD library
#include <SPI.h>
#include <DueTimer.h>

// pin definition for the Leonardo
#define cs   7
#define dc   3
#define rst  6

// create an instance of the library
TFT TFTscreen = TFT(cs, dc, rst);

long tickms = 0;
long counterValue = 0;
byte counterChange = 0;

char charBuffer[20];

void timerISR() {
  // 1ms tick
  tickms++;
}

void timerGate() {
  counterValue = Timer3.counterValueAndReset();
  counterChange++;
}

void setup() {
  // buil in led
  pinMode(LED_BUILTIN, OUTPUT);

  // Put this line at the beginning of every sketch that uses the GLCD:
  TFTscreen.begin();

  // clear the screen with a black background
  TFTscreen.background(0, 0, 0);

  // write the static text to the screen
  // set the font color to white
  TFTscreen.stroke(255, 255, 255);
  // set the font size
  TFTscreen.setTextSize(2);
  // write the text to the top left corner of the screen
  TFTscreen.text("Count Value :\n ", 0, 0);
  // ste the font size very large for the loop
  TFTscreen.setTextSize(3);

  TFTscreen.fill(0, 0, 0);
  TFTscreen.noStroke();

  // for debugging monitor
  Serial.begin(9600); //This pipes to the serial monitor
  //Serial1.begin(9600); //This is the UART, pipes to sensors attached to board

  // Start timer 2 on 1Khz and enable output B
  Serial.println("start generator ");
  Timer1.attachInterrupt(timerISR).setPeriodMilliSeconds(10).setTimeOnBMilliSeconds(1).enablePinB().start();

  Serial.println("start gate ");
  Timer2.attachInterrupt(timerGate).setPeriodMilliSeconds(1000).start();

  Serial.println("start counter ");
  Timer3.setCounter().start();
}

void loop() {
  
  if (counterChange) {
    TFTscreen.rect(0,20,TFTscreen.width(),40);
    TFTscreen.stroke(255, 255, 255);

    //double periodMS;
    sprintf(charBuffer,"%u",counterValue);
    TFTscreen.text(charBuffer, 0, 20);
    
    TFTscreen.noStroke();
    counterChange--;
  }
 
}


