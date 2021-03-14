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
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <DueTimer.h>


// pin definition for the Uno

// pin definition for the Leonardo
#define TFT_CS   7
#define TFT_RST  6
#define TFT_DC   5


// create an instance of the library
Adafruit_ILI9341 TFTscreen = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

// char array to print to the screen
#define cTextSize 5

#define X_POS       10
#define Y_POS       20
#define BACK_COLOR  ILI9341_WHITE
#define TEXT_COLOR  ILI9341_RED

#define cChars    5
char sensorPrintout[cChars];
word sensorValue = 0;
byte sensorChange = 0;

void timerISR() {
  sensorValue = analogRead(A0);
  sensorChange++;
}

void setup() {

  // Analog 12 bits
  analogReadResolution(12);

  // Put this line at the beginning of every sketch that uses the GLCD:
  TFTscreen.begin();
  // landscape
  TFTscreen.setRotation(1); 

  // clear the screen with a black background
  TFTscreen.fillScreen(BACK_COLOR);

  // write the static text to the screen
  // set the font color to white
  TFTscreen.setTextColor(TEXT_COLOR);
  // set the font size
  TFTscreen.setTextSize(2);
  TFTscreen.setCursor(0, 0);
  TFTscreen.println("Sensor Value :");
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

    // erase previous
    TFTscreen.setTextColor(BACK_COLOR);
    TFTscreen.setCursor(X_POS, Y_POS);
    TFTscreen.print(sensorPrintout);

    // Read the value of the sensor on A0
    String sensorVal = String(sensorValue);
    // convert the reading to a char array
    sensorVal.toCharArray(sensorPrintout, cChars);
  
    // set the font color
    // print the sensor value
    TFTscreen.setTextColor(TEXT_COLOR);
    TFTscreen.setCursor(X_POS, Y_POS);
    TFTscreen.print(sensorPrintout);
  
    sensorChange--;
  }
}
