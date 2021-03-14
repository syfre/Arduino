#include <HardwareTimer.h>
//#include "Canvas9341_stm32_.h"
#include "Canvas.h"
#include <XPT2046.h>
#include <AD7793.h>

#define pinLED  PC13
#define pinOUT  PB0

#define freqKhz         50
#define periodus        1000 / freqKhz
#define ticksBySecond   1000 * freqKhz
#define ticksByMilliSeconds   freqKhz
#define ticksByMeasure   ticksBySecond / 4

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

#define pinAD PB8
AD7793 ad7793 = AD7793(pinAD);

#define VALUES  4
char buffers[2*VALUES][24];
Rect rcText[VALUES];
bool needCalibrate = 1;
bool needSetZero = 0;
long value_ref = 8388608;

COLOR color;

volatile long milliSeconds;
volatile long ticks;
volatile long tickms; 
void TimerIRQ(void) {
  digitalWrite(pinOUT, ! digitalRead(pinOUT));
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

void Calibrate() {

  uint8_t state;
  uint16_t vx1,vx2,vy1,vy2;
  
  Rect rc;
  rc = tft.clientRect;
  tft.fillRectangle(rc,COLOR_BLACK);
  
  rc.right=rc.left+10;
  rc.bottom=rc.top+10;
  rc.draw(tft,COLOR_WHITE);

  state = 0;
  while (1) {
    //
    if (tickms>=ticksByMilliSeconds) {

      milliSeconds++;

      if (touch.touched(milliSeconds)) { 
        digitalWrite(pinLED, ! digitalRead(pinLED));  
        switch (state) {
          case 0 :
            vx1 = touch.vx;
            vy1 = touch.vy;
            rc.draw(tft,COLOR_BLACK);
            rc = tft.clientRect;
            rc.left = rc.right-10;
            rc.top  = rc.bottom-10;
            rc.draw(tft,COLOR_WHITE);
            state = 1;
            break;
            
          case 1:
            vx2 = touch.vx;
            vy2 = touch.vy;
            rc.draw(tft,COLOR_BLACK);
            rc = tft.clientRect;
            touch.xCalib.calibrate(rc.left+5,vx1,rc.right-5,vx2);
            touch.yCalib.calibrate(rc.top+5,vy1,rc.bottom-5,vy2);
            return;
        }
      }
      tickms = 0;
    }
  }
  
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

void setupScreen() {
  
  tft.clearScreen();
  tft.setFontSize(3);

  for (int i=0; i<VALUES; i++) {
    rcText[i] = tft.clientRect;
    rcText[i].top = i*tft.textHeight("A");
    rcText[i].bottom = rcText[i].top+tft.textHeight("A");
    strcpy(buffers[2*i], "");
    strcpy(buffers[2*i+1], "");
  }
  
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
  ad7793.begin();

  setupTFT();
  
  //Calibrate();
  touch.xCalib.calibrate(5,297,314,3623);
  touch.yCalib.calibrate(5,388,234,3651);
  
  setupScreen();
  //Touch();
  //Test();
  
  ad7793.initialize(CHANNEL_AN2);
}

void loop() {

  if (tickms>=ticksByMilliSeconds) {
    //digitalWrite(pinLED, ! digitalRead(pinLED));

    milliSeconds++;
    if (touch.touched(milliSeconds)) { 
      //digitalWrite(pinLED, ! digitalRead(pinLED));  
      needSetZero = 1;
    }
    tickms = 0;
  }

  if (ticks>=ticksByMeasure) {
      digitalWrite(pinLED, ! digitalRead(pinLED));

      char buffer[40];
      uint8_t output = 0;
      long value = 0;
    
      output = ad7793.readID();
      if (output==0x4b) {
    
        if (needCalibrate) {
          Serial.println("Calibrate");
          ad7793.calibrate(CHANNEL_AN2);
          needCalibrate = 0;
        }
    
        Serial.println("Measure");

        value = ad7793.read(CHANNEL_AN2);

        if (needSetZero) {
          value_ref = value;
          needSetZero = 0;
          Serial.println("Set zero");
        }
        
        sprintf(buffer,"ID:%x V1:%u V1:%0.4f S:%d",output,value, ad7793.ToVolt(value), value-value_ref);
        Serial.println(buffer);

        sprintf(buffers[0],"N  :%u",value);
        sprintf(buffers[2],"V  :%0.5f",ad7793.ToVolt(value));
        sprintf(buffers[4],"dN :%d",value-value_ref);
        sprintf(buffers[6],"mT : %0.2f", (ad7793.ToVolt(value)-ad7793.ToVolt(value_ref))/0.018 );
        
        for (int i=0; i<VALUES; i++) {
           rcText[i].textDiff(tft,leftAlign,buffers[i*2],buffers[i*2+1],COLOR_WHITE,COLOR_BLACK);
           strcpy(buffers[i*2+1],buffers[i]);
        }

        
      } else {
        sprintf(buffer,"ID:%x, Reset",output);
        Serial.println(buffer);
        ad7793.reset();
        needCalibrate = 1;
      }
 
    ticks = 0;
  }

}


