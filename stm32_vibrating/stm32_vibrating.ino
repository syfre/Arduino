#include <HardwareTimer.h>
#include "Canvas.h"

#define pinLED  PC13
#define pinOUT  PB0
#define pinIN   PB9

#define freqKhz         50
#define periodus        1000 / freqKhz
#define ticksBySecond   1000 * freqKhz
#define ticksByMilliSeconds   freqKhz

// pin definition for the TFT
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

///////////////////////////////////////////////////
#define STATE_ON  0
#define STATE_OFF 1
#define STATE_0 0
#define STATE_1 1
#define STATE_2 2
#define STATE_3 3

#define RUN_WAIT      0
#define RUN_MEASURE   1

volatile long milliSeconds;
volatile long ticks;
volatile long tickms; 
//

volatile long measure;
volatile long lastMeasure;
volatile long sumMeasure;
int runState;
int state;
int count;
int newMeasure;
int ctnMeasure;

// Circular buffer to
// avoid incoherent measures
// 
#define MAX_MEASURES 15
long measures[MAX_MEASURES];
int indxMeasures;
int ctnMeasures;

#define DISPLAY_LINES 5
#define TEXT_SIZE     3
#define TEXT_HEIGHT   8*TEXT_SIZE
char lines[DISPLAY_LINES][40];

void InitMeasure() {
  
  Serial.println("Initialization");
  //
  Rect rc;
  rc = tft.clientRect;
  tft.fillRectangle(rc,COLOR_BLACK);
  tft.setFontSize(TEXT_SIZE);
  //  
  runState = RUN_WAIT;
  state = STATE_0;
  lastMeasure = 0;
  sumMeasure = 0;
  ctnMeasure = 0;
  newMeasure = 0;
  count = 0;
  measure = 0;
  //
  indxMeasures = 0;
  ctnMeasures = 0;
  //
  newMeasure++;
}

void Touched() {
  
  Serial.print("Touched State:"); Serial.println(runState);
  
  switch (runState) {
    case RUN_WAIT:
    InitMeasure();
    runState = RUN_MEASURE;
    break;
    
    case RUN_MEASURE:
    newMeasure++;
    runState = RUN_WAIT;
    break;
  }

}

double _ToMilliseconds(long ticks) {
  return ticks / freqKhz;
}

void TimerIRQ(void) {
  //digitalWrite(pinOUT, ! digitalRead(pinOUT));
  
  measure++;
  ticks++;
  tickms++;

  int pin = digitalRead(pinIN);
  digitalWrite(pinOUT,pin);

  if (runState==RUN_MEASURE) {
    
    switch(state) {
        case STATE_0:
          // wait until pin go ON
          if (pin==STATE_ON) {
            state = STATE_1;
            //
            digitalWrite(pinLED, ! digitalRead(pinLED));  
            //
            if (count) {
              lastMeasure = measure;

              measures[indxMeasures++] = lastMeasure;
              if (indxMeasures==MAX_MEASURES) indxMeasures=0;
              if (ctnMeasures<MAX_MEASURES) ctnMeasures++;
              
              newMeasure++;
              ctnMeasure++;
              sumMeasure += lastMeasure;
            }
            measure = 0;
            count++;
          }
        break;
       
        case STATE_1:
          // wait until pin go OFF
          if (pin==STATE_OFF) {
            state = STATE_2;
          }
        break;
  
        case STATE_2:
          // wait until pin go ON
          if (pin==STATE_ON) {
            state = STATE_3;
          }
        break;
       
        case STATE_3:
          // wait until pin go OFF
          if (pin==STATE_OFF) {
            state = STATE_0;
          }
        break;
    }
  }
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

void setupTFT() {

  tft.begin();
  tft.setOrientation(landscape_reverse); 
  Serial.print("W:"); Serial.print(tft.clientRect.width()); Serial.print(" H:"); Serial.println(tft.clientRect.height());
}

void setup() {
  Serial.begin(115200);
  pinMode(pinLED, OUTPUT);
  pinMode(pinOUT, OUTPUT);
  pinMode(pinIN,INPUT);
  digitalWrite(pinLED, 1);
  Serial.println("START");  

  ticks = 0;
  tickms = 0;
  milliSeconds = 0;
  Timer3.setPeriod(periodus);
  Timer3.attachInterrupt(3, TimerIRQ);

  touch.begin();

  setupTFT();
  
  //touch.calibrate(tft);
  //touch.xCalib.calibrate(5,297,314,3623);
  //touch.yCalib.calibrate(5,388,234,3651);

  InitMeasure();
}

void loop() {

  if (tickms>=ticksByMilliSeconds) {
    
    //digitalWrite(pinLED, ! digitalRead(pinLED));

    milliSeconds++;
    if (touch.touched(milliSeconds)) { 
      //
      Touched();
    }
    tickms = 0;
  }


  if (ticks==ticksBySecond) {
    //digitalWrite(pinLED, ! digitalRead(pinLED));

    ticks = 0;
  }

  if (newMeasure) {
    // clear
    for (int idx=0; idx<DISPLAY_LINES; idx++) {
      tft.text(0,idx*TEXT_HEIGHT,lines[idx],COLOR_BLACK);
    }
    //
    if (runState==RUN_WAIT) { 
      sprintf(lines[0],"Touch for start");
    } else {
      sprintf(lines[0],"Touch for stop");
    }
    //
    double bph,average=0;

    // average from the circular buffer
    //
    long sum = 0;
    for (int idx=0; idx<ctnMeasures; idx++) sum+=measures[idx]; 
    if (sum!=0) average = _ToMilliseconds(sum) / ctnMeasures; else average = 0;
    if (average!=0) bph = 7200000/average; else bph = 0;
    //
    sprintf(lines[1],"Ctn :%d",ctnMeasure);
    sprintf(lines[2],"Last:%.2f",_ToMilliseconds(lastMeasure));
    sprintf(lines[3],"Ave :%.2f",average);
    sprintf(lines[4],"BPH :%.2f",bph);
 
    
    // print
    for (int idx=0; idx<DISPLAY_LINES; idx++) {
      tft.text(0,idx*TEXT_HEIGHT,lines[idx],COLOR_WHITE);
    }
    
    Serial.print("Count:"); Serial.print(ctnMeasure); Serial.print(" Last:"); Serial.print(lastMeasure); Serial.print(" Sum:"); Serial.println(sumMeasure);
    newMeasure--;
  }
  
}


