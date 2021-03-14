#include <HardwareTimer.h>
#include <STM32ADC.h>
#include "Canvas.h"
#include <Watch.h>

#define pinLED  PC13
#define pinOUT  PB0
#define pinPPS  PB8

#define _PPS

//////////////////////////////////////
// ADC
//////////////////////////////////////

/////////////////////////
// maxSample may change depending of compiler and library release
// must keep enough room for stack
/////////////////////////
#define maxSamples  5000
#define cBufferSize 2500
uint16_t buffer[maxSamples];
uint16_t *buffers[2];
volatile uint8_t bufr;
volatile uint8_t bufw;
volatile uint16_t bufl;

/////////////////////////////////////
// STM32 board number
// some constants change with board :
//   Timing adjustement
//   Touch calibration
//
#define STM32_1
//#define STM32_2
//
/////////////////////////////////////

#ifdef STM32_1
//#define cSampleFreq         50000
//#define cActualFreq         49970
#define cSampleFreq         100000
#define cActualFreq         99870
//#define cSampleFreq         200000
//#define cActualFreq         199462
#define pinADC  PB1
#endif

#ifdef STM32_2
#define cSampleFreq         50000
#define cActualFreq         49970
#define pinADC  PA0

//#define cSampleFreq         100000
//#define cActualFreq         99871
//#define cSampleFreq         200000
//#define cActualFreq         199466
#endif

#define cSampleRange        2048

#define sampleFreqKhz       (uint16_t)(cSampleFreq / 1000)
#define samplePeriodus      (uint16_t)(1000000 / cSampleFreq)
#define ticksPerSecond      (uint16_t)(2 * cSampleFreq / maxSamples)

long ticks;
uint8 pins = pinADC;
STM32ADC myADC(ADC1);

//////////////////////////////////////
// TFT/Canvas
//////////////////////////////////////
// pin definition for the TFT
// SPI1 MISI  PA7 
// SPI1 CLK   PA5
// C/D        PA4
// RST        PA3
// CS         PA2
#define cs   PA2
#define dc   PA4
#define rst  PA3
Canvas9341 tft = Canvas9341(cs,dc,rst);

//////////////////////////////////////
// Touch
//////////////////////////////////////
#define touch_cs  PB11
#define touch_irq PB10
Touch_xpt2046 touch = Touch_xpt2046(touch_cs,touch_irq,0,TOUCH_LANDSCAPE);

//////////////////////////////////////
// WATCH
//////////////////////////////////////
Watch watch = Watch(tft,touch,cActualFreq,cSampleRange);

//////////////////////////////////////
// Measure PPS for calibration
//////////////////////////////////////
#ifdef PPS
volatile long ppsTicks;

void timerIRQ(void) {
   ticks++;
}
void ppsIRQ(void) {
   ppsTicks = ticks;
   ticks = 0;
}
#endif

#define ticksByMilliSeconds 1
volatile long tickms = 0;
volatile long milliSeconds = 0;
void tickmsIRQ(void) {
   milliSeconds++;
   // WARNING : Debug by serial in IRQ may crash
   watch.irqMilliSeconds(milliSeconds);
   tickms++;
}

void DmaIRQ(void) {
  digitalWrite(pinOUT, ! digitalRead(pinOUT));
  if (bufw!=bufr) bufl++;
  bufw = (bufw+1)%2;
  ticks++;
}

void setupADC() {
  pinMode(pins, INPUT_ANALOG);

  ticks = 0;
  bufr = 0;
  bufw = 0;
  bufl = 0;
  buffers[0] = &buffer[0];
  buffers[1] = &buffer[maxSamples/2];

  Timer3.setPeriod(samplePeriodus);
  Timer3.setMasterModeTrGo(TIMER_CR2_MMS_UPDATE);

  myADC.calibrate();
  myADC.setSampleRate(ADC_SMPR_1_5); // ?
  myADC.setPins(&pins, 1);
  myADC.setDMA(buffer, maxSamples, (DMA_MINC_MODE | DMA_CIRC_MODE | DMA_HALF_TRNS | DMA_TRNS_CMPLT), DmaIRQ);
  myADC.setTrigger(ADC_EXT_EV_TIM3_TRGO);
  myADC.startConversion();  
}


void setupTFT() {
  tft.begin();
  tft.setOrientation(landscape); 
  tft.clearScreen(COLOR_BLACK);
}

#ifdef PPS
void setupPPS(void) {

  tft.setFontSize(3);
 
  pinMode(pinPPS, INPUT);
  attachInterrupt(pinPPS, ppsIRQ, RISING);

  Timer3.setPeriod(samplePeriodus);
  Timer3.setMasterModeTrGo(TIMER_CR2_MMS_UPDATE);
  Timer3.attachInterrupt(3, timerIRQ);
}
#endif

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


void setupWatch() {
  
  watch.setup();  
}

void setupMenu() {

  touch.begin();
  
  Timer2.setPeriod(1000);
  Timer2.setMasterModeTrGo(TIMER_CR2_MMS_UPDATE);
  Timer2.attachInterrupt(2, tickmsIRQ);

 // Calibrate();

  #ifdef STM32_1
  touch.xCalib.calibrate(5,3727,314,348);
  touch.yCalib.calibrate(5,3296,234,425);
  #endif
  
  #ifdef STM32_2
  touch.xCalib.calibrate(5,297,314,3623);
  touch.yCalib.calibrate(5,388,234,3651);
  #endif

}

void setup() {
  Serial.begin(115200);
  pinMode(pinLED, OUTPUT);
  pinMode(pinOUT, OUTPUT);

  setupTFT();

  #ifdef PPS
  setupPPS();
  #else

  setupWatch();
  setupMenu();
  setupADC();
  
  #endif

}

void loop() {

  #ifdef PPS
  if (ppsTicks) {
    char buffer[40];
    sprintf(buffer,"%u",ppsTicks);
    tft.clearScreen(COLOR_BLACK);
    tft.text(0,0,buffer,COLOR_WHITE);
    ppsTicks = 0;
  }
  return;
  
  #endif
  

  if (bufr!=bufw) {
    // process data 

    watch.processBuffer(buffers[bufr], cBufferSize);
    
    bufr = (bufr+1)%2;
  }

  if (tickms>=0) {
    watch.loopMilliSeconds(milliSeconds);
    tickms = 0;
  }
  
  if (ticks>ticksPerSecond) {
    digitalWrite(pinLED, ! digitalRead(pinLED));

    watch.loopSeconds(bufl);
        
    ticks = 0;
  }  
}


