#include <HardwareTimer.h>
#include "Canvas.h"
#include <OneWireSTM.h>
#include <DallasTemperature.h>


#define pinLED   PC13
#define pinLED2  PB0

#define freqKhz         50
#define periodus        1000 / freqKhz
#define ticksBySecond   1000 * freqKhz
#define ticksByMilliSeconds   freqKhz

// pin definition for the TFT
// SPI1 MISI  PA7 
// SPI1 CLK   PA5
// C/D        PA4
// RST        PA3
// CS         PA2

#define tft_cs   PA2
#define tft_dc   PA4
#define tft_rst  PA3

#define touch_cs  PB11
#define touch_irq PB10

#define BACK_COLOR  COLOR_BLACK
#define BACK_TEXT   COLOR_RED
#define TEXT_COLOR  COLOR_WHITE
#define cTextSize   3


Canvas9341 tft = Canvas9341(tft_cs,tft_dc,tft_rst);
Touch_xpt2046 touch = Touch_xpt2046(touch_cs,touch_irq,0,TOUCH_LANDSCAPE);

#define ONE_WIRE_BUS   PB9
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

COORD XPos,YPos;
COLOR color;
char buf[40];
float cTemp,pTemp;

volatile long milliSeconds;
volatile long ticks;
volatile long tickms; 

void TimerIRQ(void) {
  //digitalWrite(pinOUT, ! digitalRead(pinOUT));
  ticks++;
  tickms++;
}

void InitScreen() {
  
  tft.clearScreen(COLOR_BLACK);
  tft.setFontSize(6);
  color = COLOR_WHITE;

  cTemp = 0;
  pTemp = 0;
  sprintf(buf,"%.2f",pTemp);

  XPos = (tft.width()-tft.textWidth(buf)) / 2;
  YPos = (tft.height() - tft.textHeight(buf)) / 2;

  Serial.print("XPos:"); Serial.print(XPos); Serial.print(" YPos:"); Serial.println(YPos);
  
  
  tft.text(XPos,YPos,buf,color);
}


void Touch() {

  char buffer[9][12] = {"0","1","2","3","4","5","6","7","Exit"};
  int sel;
  
  Menu menu;
  
  menu.rect(tft.clientRect).horzCount(3).vertCount(3);
  menu.colors(COLOR_WHITE,COLOR_GREEN,COLOR_BLACK,COLOR_GREEN,COLOR_RED);
  
  for (int i=0; i<menu.itemCount(); i++) {
    menu.textItem(i, buffer[i] );
  }
  

  tft.clearScreen();
  menu.draw(tft);
  menu.selected(tft,0);

  while (1) {
    //
    if (tickms>=ticksByMilliSeconds) {

      milliSeconds++;

      if (touch.touched(milliSeconds)) { 
        digitalWrite(pinLED, ! digitalRead(pinLED)); 

        sel = menu.selected(tft, touch.x, touch.y);

        char buf[40];
        sprintf(buf,"Touh x:%u y:%u menu:%d",touch.x,touch.y,sel);
        Serial.println(buf);

        switch (sel) {
          case -1: return;
          case 8: return;
        }
      }
      tickms = 0;
    }
  }
}

void setupTFT() {

  tft.begin();
  tft.setOrientation(landscape); 
  Serial.print("W:"); Serial.print(tft.clientRect.width()); Serial.print(" H:"); Serial.println(tft.clientRect.height());
}

void setup() {
  Serial.begin(115200);
  pinMode(pinLED, OUTPUT);
  pinMode(pinLED2, OUTPUT);
  digitalWrite(pinLED, 1);
  digitalWrite(pinLED2, 1);
  Serial.println("START");  

  ticks = 0;
  tickms = 0;
  milliSeconds = 0;
  Timer3.setPeriod(periodus);
  Timer3.attachInterrupt(3, TimerIRQ);

  touch.begin();

  setupTFT();
  InitScreen();
  
  // Touch calibration
  //touch.calibrate(tft);
  //touch.xCalib.calibrate(5,297,314,3623);
  //touch.yCalib.calibrate(5,388,234,3651);
  // module with ad
  touch.xCalib.calibrate(5,264,314,3592);
  touch.yCalib.calibrate(5,377,234,3557);
  
  // Start up dallas
  sensors.begin();
  
}

void loop() {


  if (tickms>=ticksByMilliSeconds) {
    
    tickms = 0;
  }

  if (ticks==ticksBySecond) {

      
      // clear line
      //      
      Serial.print("Requesting temperatures...");
      sensors.requestTemperatures(); // Send the command to get temperatures
      Serial.println("DONE");
      // After we got the temperatures, we can print them here.
      // We use the function ByIndex, and as an example get the temperature from the first sensor only.
      cTemp = sensors.getTempCByIndex(0);
      //
      Serial.print("Temperature for the device 1 (index 0) is: ");
      Serial.println(cTemp); 
      //
      if (cTemp!=pTemp) {
        tft.text(XPos,YPos,buf,COLOR_BLACK);
        sprintf(buf,"%.2f",cTemp);
        tft.text(XPos,YPos,buf,color);
        pTemp = cTemp;
      }
      //    
    ticks = 0;
  }

}


