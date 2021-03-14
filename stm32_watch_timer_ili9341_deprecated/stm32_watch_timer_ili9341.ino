#include <HardwareTimer.h>
#include <STM32ADC.h>
#include <Canvas9341_stm32_.h>
#include <XPT2046.h>
#include <WatchCore.h>
#include <WatchScreen.h>
#include <WatchVibrograph.h>
#include <WatchMenu.h>
#include <Scope.h>

#define pinLED  PC13
#define pinOUT  PB0
#define pinPPS  PB8

#define VIBRO
#define SCOPE
#define _PPS

//////////////////////////////////////
// ADC
//////////////////////////////////////

#define maxSamples  6000
#define cBufferSize 3000
uint16_t buffer[maxSamples];
uint16_t *buffers[2];
volatile uint8_t bufr;
volatile uint8_t bufw;
volatile uint16_t bufl;

#define STM32_1
//#define STM32_2

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
WatchCore watch = WatchCore(cActualFreq,cSampleRange);
WatchScreen wscreen = WatchScreen(tft);
#ifdef VIBRO
WatchVibrograph wvibro = WatchVibrograph(tft, watch.ToTime(200.0),1,10.0);
#endif
#ifdef SCOPE
Scope wscope = Scope(tft, cActualFreq,cSampleRange);
#endif
WatchMenu menu = WatchMenu(tft);

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

void bphChange(Bph bph) {
  #ifdef VIBRO
  wvibro.setBeatSamples(watch.beatSamples());
  #endif
  #ifdef SCOPE
  wscope.setDelayMilliSeconds(watch.beatPeriod()*0.9).setTimeMilliSeconds(watch.beatDuration()*4.0);
  #endif
}

void setupWatch() {
  watch.begin();
 
  wscreen.begin();
  #ifdef VIBRO
  wvibro.begin();
  #endif
  #ifdef SCOPE
  wscope.begin();
  #endif

  watch.attachBphChange(bphChange);
  watch.setBphRef(BPH_21600);
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

void OnWatchParamChange(void) {
  watch.setBphRef((Bph)menu.bph);
  watch.setLiftAngle(menu.liftAngle);
}

void OnWatchModeChange(WatchMode _old, WatchMode _new) {
  bufl = 0;
  switch (_old) {
    case wmVibro : 
     #ifdef VIBRO 
     wvibro.enable(0); 
     #endif 
     break;
    case wmScope : 
     #ifdef SCOPE 
     wscope.enable(0); 
     #endif 
     break;
    case wmMenu  : menu.enable(0); break;
  }
  switch (_new) {
    case wmVibro : 
     #ifdef VIBRO 
     wvibro.enable(1); 
     #endif 
     break;
    case wmScope : 
     #ifdef SCOPE 
     wscope.enable(1); 
     #endif 
     break;
    case wmMenu  : menu.enable(1); break;
  }
}

void setupMenu() {

  touch.begin();
  menu.begin();
  menu.onWatchModeChange = OnWatchModeChange;
  menu.onWatchParamChange = OnWatchParamChange;
  
  Timer2.setPeriod(1000);
  Timer2.setMasterModeTrGo(TIMER_CR2_MMS_UPDATE);
  Timer2.attachInterrupt(2, tickmsIRQ);

  //Calibrate();

  #ifdef STM32_1
  touch.xCalib.calibrate(5,3727,314,348);
  touch.yCalib.calibrate(5,3296,234,425);
  #endif
  
  #ifdef STM32_2
  touch.xCalib.calibrate(5,297,314,3623);
  touch.yCalib.calibrate(5,388,234,3651);
  #endif
  
  menu.enable(1);

  menu.bph = watch.bph;
  menu.liftAngle = watch.liftAngle;
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

    uint16_t *pbuf = buffers[bufr];
    for (int i=0; i<cBufferSize; i++) {
      // 
      if (watch.process(*pbuf++)) {
        #ifdef VIBRO
        wvibro.addBeat(watch.beatDetector.beat);
        #endif
        #ifdef SCOPE
        wscope.trigger();
        #endif
      }

      #ifdef SCOPE
      wscope.addSample(watch.sample,watch.beatDetector.beatOn());
      #endif
    }
    
    bufr = (bufr+1)%2;
  }

  if (tickms>=ticksByMilliSeconds) {
    milliSeconds++;
    
    if (touch.touched(milliSeconds)) { 
      int sel = menu.selected(touch.x, touch.y);
    }
    tickms = 0;
  }
  
  if (ticks>ticksPerSecond) {
    digitalWrite(pinLED, ! digitalRead(pinLED));
  
   // every seconds
    watch.updateResult(bufl);
    wscreen.refresh(watch.result);
        
    ticks = 0;
  }  
}


