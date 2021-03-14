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
#include <Rotary.h>

// pin definition for the TFT / DUE
#define cs   7
#define dc   5
#define rst  6

// pin definition for the Rotary encoder #1
#define rotary_1_A  2
#define rotary_1_B  3


// create an instance of the TFT library
TFT TFTscreen = TFT(cs, dc, rst);

// Create an instance of the rotary encoder
Rotary rotary_1 = Rotary(rotary_1_A, rotary_1_B);

// char array to print to the screen
char sensorPrintout[5];
word sensorValue = 0;
byte sensorChange = 1;

void timerISR() {
  unsigned char result = rotary_1.process();
  if (result) {
    if (result==DIR_CW) {
      if (sensorValue < 1024) sensorValue++; else sensorValue=1024;
    } else {
      if (sensorValue > 0) sensorValue--; else sensorValue=0;
    }
    sensorChange++;
  }
}

void setup() {

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
  TFTscreen.text("Sensor Value :\n ", 0, 0);
  // ste the font size very large for the loop
  TFTscreen.setTextSize(5);
  
  // for debugging monitor
  //Serial.begin(9600); //This pipes to the serial monitor
  //Serial1.begin(9600); //This is the UART, pipes to sensors attached to board

  // Start timer 3 1 ms
  Timer3.attachInterrupt(timerISR).setFrequency(1000).start();
}

void loop() {

  if (sensorChange) {
    TFTscreen.stroke(0, 0, 0);
    TFTscreen.text(sensorPrintout, 0, 20);

    // Read the value of the sensor on A0
    String sensorVal = String(sensorValue);

    // convert the reading to a char array
    sensorVal.toCharArray(sensorPrintout, 5);

    // set the font color
    TFTscreen.stroke(255, 255, 255);
    // print the sensor value
    TFTscreen.text(sensorPrintout, 0, 20);
    
    sensorChange--;
  }
}


