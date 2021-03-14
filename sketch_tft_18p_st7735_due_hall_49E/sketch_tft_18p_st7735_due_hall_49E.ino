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
#define dc   5
#define rst  6

// Hall 49E
// sensitivity = 18mv / mT
// adc : 3.3v 1024 => 1pt = 3300mv/1024
// (10bits) 1 pt = 3300/1024/18 mT = 0.17903 mt
// (12bits) 1 pt = 3300/4096/18 mT = 0.047591145 mt
#define cst_SensorZero  2048

// create an instance of the library
TFT TFTscreen = TFT(cs, dc, rst);

// char array to print to the screen
// max dipslayed chars at textSize 5 are 5
#define cChars  12
#define cTextSize 5
char sensorPrintout[cChars];

// sensor value
float sensorValue = 0;
float sensorSensibility = 0.047591145;
byte sensorChange = 0;


void timerISR() {
  sensorValue = (int)(analogRead(A0) - cst_SensorZero) * sensorSensibility;
  sensorChange++;
}

void setup() {

  // analog 12 bits
  analogReadResolution(12);

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
  TFTscreen.setTextSize(cTextSize);
  
  // for debugging monitor
  //Serial.begin(9600); //This pipes to the serial monitor
  //Serial1.begin(9600); //This is the UART, pipes to sensors attached to board

  // Start timer 3 1 second
  Timer3.attachInterrupt(timerISR).setFrequency(1).start();
}

void loop() {

  if (sensorChange) {
    // clar
    TFTscreen.stroke(0, 0, 0);
    TFTscreen.text(sensorPrintout, 0, 20);

    sprintf(sensorPrintout,"%.2f",sensorValue);

    // set the font color
    TFTscreen.stroke(255, 255, 255);
    // print the sensor value
    TFTscreen.text(sensorPrintout, 0, 20);
    
    sensorChange--;
  }
}


