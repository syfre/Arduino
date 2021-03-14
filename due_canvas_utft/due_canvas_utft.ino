#include "DueTimer.h"
#include "CanvasUTFT.h"
#include "UTFT.h"
#include "XPT2046.h"

#define pinLED        LED_BUILTIN

#define freqKhz         50
#define periodus        1000 / freqKhz
#define ticksBySecond   1000 * freqKhz
#define ticksByMilliSeconds   freqKhz


#define BACK_COLOR  COLOR_BLACK
#define BACK_TEXT   COLOR_RED
#define TEXT_COLOR  COLOR_WHITE
#define cTextSize   3

CanvasUTFT tft = CanvasUTFT(ITDB50,25,26,27,28);

#define touch_cs    5
#define touch_irq   2
#define touch_clk   6
#define touch_din   4
#define touch_dout  3
XPT2046SOFT touch = XPT2046SOFT(touch_cs,touch_irq,touch_clk,touch_din,touch_dout,TOUCH_PORTRAIT);

COLOR color;

volatile long milliSeconds;
volatile long ticks;
volatile long tickms; 
void TimerIRQ(void) {
  #ifdef pinOUT
  digitalWrite(pinOUT, ! digitalRead(pinOUT));
  #endif
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

  char buffer[9][12] = {"0","1","2","3","4","5","6","7","8"};
  
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

        char buf[40];
        sprintf(buf,"Touh x:%u y:%u",touch.x,touch.y);
        Serial.println(buf);

        switch (menu.selected(tft, touch.x, touch.y)) {
          case -1: break;
          case 0: break;
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
  pinMode(pinLED, OUTPUT);
  digitalWrite(pinLED, HIGH);

  #ifdef pinOUT
  pinMode(pinOUT, OUTPUT);
  digitalWrite(pinOUT, LOW);
  #endif
  
  Serial.begin(9600);
  Serial.println("START");  

  ticks = 0;
  tickms = 0;
  milliSeconds = 0;
  Timer3.setPeriod(periodus).attachInterrupt(TimerIRQ).start();
  
  touch.begin();
  setupTFT();

  touch.calibrate(tft);
  //touch.xCalib.calibrate(5,297,314,3623);
  //touch.yCalib.calibrate(5,388,234,3651);
  touch.test(tft, pinLED);

  //Test();
}

void loop() {

  Point pt = tft.clientRect.center();

  if (tickms>=ticksByMilliSeconds) {
    //digitalWrite(pinLED, ! digitalRead(pinLED));
    milliSeconds++;
    
    tickms = 0;
  }


  if (ticks>=ticksBySecond) {
    digitalWrite(pinLED, ! digitalRead(pinLED));

    //Test();
    ticks = 0;
  }

}


