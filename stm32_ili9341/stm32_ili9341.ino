#include <HardwareTimer.h>
#include "Canvas.h"


#define pinLED  PC13
#define pinOUT  PB0

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

COLOR color;

volatile long milliSeconds;
volatile long ticks;
volatile long tickms; 
void TimerIRQ(void) {
  //digitalWrite(pinOUT, ! digitalRead(pinOUT));
  ticks++;
  tickms++;
}

void Test(void) {

  tft.setPixel(0,0,COLOR_RED);
  tft.setPixel(0,0,COLOR_RED);

  Rect rc;
  rc = tft.clientRect;
  //rc.bottom = (rc.top+rc.bottom)/2;
  tft.fillRectangle(rc,COLOR_RED);
  //Serial.println("1");

  rc = tft.clientRect;
  rc.top = (rc.top+rc.bottom)/2;
  //tft.fillRectangle(rc,COLOR_GREEN);
  //Serial.println("2");

  

  rc = tft.clientRect;
  rc.serialPrint();
  rc.inflatePercent(0,-20.0);
  rc.serialPrint();
  tft.fillRectangle(rc,COLOR_BLUE);
  Serial.println("FR1");

   rc = tft.clientRect;

  // Horz line
  tft.lineTo(rc.left,(rc.top+rc.bottom) /2,rc.right,(rc.top+rc.bottom) /2,COLOR_WHITE);
  // Vert line
  tft.lineTo((rc.left+rc.right)/2,rc.top,(rc.left+rc.right)/2,rc.bottom,COLOR_WHITE);

  tft.lineTo(rc.left,rc.top,rc.right,rc.bottom,COLOR_WHITE);

  tft.setFontSize(1);
  tft.text(0,0,"! #$%&'()*+,-./");
  Serial.println("T1");

  tft.setFontSize(2);
  tft.text(0,20,"Text size 2");
  Serial.println("T2");
  tft.setFontSize(3);
  tft.text(0,60,"Text size 3");
  Serial.println("T3");
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
  pinMode(pinOUT, OUTPUT);
  digitalWrite(pinLED, 1);
  Serial.println("START");  

  ticks = 0;
  tickms = 0;
  milliSeconds = 0;
  Timer3.setPeriod(periodus);
  Timer3.attachInterrupt(3, TimerIRQ);

  touch.begin();

  setupTFT();
  // Touch calibration
  //touch.calibrate(tft);
  //touch.xCalib.calibrate(5,297,314,3623);
  //touch.yCalib.calibrate(5,388,234,3651);
  // module with ad
  touch.xCalib.calibrate(5,264,314,3592);
  touch.yCalib.calibrate(5,377,234,3557);
  

  Touch();
  Test();
}

Rect rc1,rc2;

void loop() {

  Point pt = tft.clientRect.center();

  if (tickms>=ticksByMilliSeconds) {
    
    //digitalWrite(pinLED, ! digitalRead(pinLED));

    milliSeconds++;
    if (touch.touched(milliSeconds)) { 
      digitalWrite(pinLED, ! digitalRead(pinLED));  


      char buffer[40];
      sprintf(buffer,"X:%u Y:%u",touch.x,touch.y);

      tft.fillRectangle(rc2,COLOR_BLUE);
      rc1 = tft.textRect(pt,buffer);
      tft.text(rc1.left,rc1.top,buffer,COLOR_GREEN);
      rc2 = rc1;
      
    }
    //touch.getTemperature();
    //touch.getVBat();
    //touch.getPosition();
    
    tickms = 0;
  }


  if (ticks==ticksBySecond) {
    //digitalWrite(pinLED, ! digitalRead(pinLED));

    //Test();
  
    
    ticks = 0;
  }

}


