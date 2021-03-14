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
long capture = 0;

char charBuffer[20];

long capturePeriod = 0;
long captureTimeOn = 0;
int captureOverRun = 0;
byte captureChange = 0;

void timerISR() {
  // 1ms tick
  tickms++;
 }

void timerCapture() {

  capture++;

  uint32_t status = Timer8.statusRegister();

  if (status & TC_SR_LDRAS) {
    captureTimeOn = Timer8.captureValueA();
    captureChange++;
  } else
  if (status & TC_SR_LDRBS) {
    capturePeriod = Timer8.captureValueB();
    captureChange++;
    Timer8.resetCounterValue();
  } else 
  if ((status & TC_SR_CPCS)||(status & TC_SR_COVFS)) {
    // Overflow trigger
    capturePeriod = 0;
    captureTimeOn = 0;
    captureOverRun++;
  };

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
  TFTscreen.text("Period Value :\n ", 0, 0);
  // ste the font size very large for the loop
  TFTscreen.setTextSize(3);

  TFTscreen.fill(0, 0, 0);
  TFTscreen.noStroke();

  // for debugging monitor
  Serial.begin(9600); //This pipes to the serial monitor
  //Serial1.begin(9600); //This is the UART, pipes to sensors attached to board

  // Start timer 2 on 1Khz and enable output B
  // pin5
  Serial.println("start timer ");
  Timer1.attachInterrupt(timerISR).setPeriodMilliSeconds(10).setTimeOnBMilliSeconds(1).enablePinB().start();

  Serial.println("start timer capture");
  Timer8.attachInterrupt(timerCapture).setCaptureMilliSeconds(100).start();
  Serial.print("Timer freq:"); Serial.println(Timer8.getFrequency(),8);
}

void loop() {
  
  /*
  if (tickms==50) {
    digitalWrite(LED_BUILTIN, ! digitalRead(LED_BUILTIN));
    tickms = 0;
  */
    
  if (capture==50) {
    digitalWrite(LED_BUILTIN, ! digitalRead(LED_BUILTIN));
    capture = 0;
  }

  if (captureOverRun==10) {
    TFTscreen.rect(0,20,TFTscreen.width(),80);
    TFTscreen.stroke(255, 255, 255);
    
    sprintf(charBuffer,"No signal");
    TFTscreen.text(charBuffer, 0, 20);
    
    TFTscreen.noStroke();
    captureOverRun = 0;
  }
  

  if (captureChange==10) {
    TFTscreen.rect(0,20,TFTscreen.width(),80);
    TFTscreen.stroke(255, 255, 255);

    //double periodMS;
    double valMS = Timer8.valueToMilliSeconds(capturePeriod);
    sprintf(charBuffer,"%.2f",valMS);
    TFTscreen.text(charBuffer, 0, 20);
    
    valMS = Timer8.valueToMilliSeconds(captureTimeOn);
    sprintf(charBuffer,"%.2f",valMS);
    TFTscreen.text(charBuffer, 0, 60);

    TFTscreen.noStroke();
    captureChange = 0;
  }
 
}


