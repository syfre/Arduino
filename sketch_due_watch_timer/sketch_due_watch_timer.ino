#define SERIALUSB_

#include <DueTimer.h>
#include <UTFT.h>
#include "CanvasUTFT.h"
#include <XPT2046.h>
#include <Watch.h>

#define LED_FAULT 4
#define pinLED  LED_BUILTIN

///////////////////////////////////////////////
// CANVAS
///////////////////////////////////////////////
CanvasUTFT tft = CanvasUTFT(ITDB50,25,26,27,28);

///////////////////////////////////////////////
// TOUCH
///////////////////////////////////////////////
#define touch_cs    5
#define touch_irq   2
#define touch_clk   6
#define touch_din   4
#define touch_dout  3
XPT2046SOFT touch = XPT2046SOFT(touch_cs,touch_irq,touch_clk,touch_din,touch_dout,TOUCH_PORTRAIT);


///////////////////////////////////////////////
// ADC
///////////////////////////////////////////////
// max 300K ; over that we lost buffer
#define cSampleFreq   250000
#define cActualFreq   cSampleFreq
#define cSampleRange  2048
#define cBufferCount  4
#define cBufferSize   10000
#define ticksPerSecond  25

volatile long ticks = 0;
uint16_t* pbuf;
uint16_t buflost;
volatile int bufw,bufnw,bufr;              // bufwrite, nextbufwrite, bufread
uint16_t buf[cBufferCount][cBufferSize];   // 4 buffers of 256 readings


void ADC_Handler(){     // move DMA pointers to next buffer
  int f = ADC->ADC_ISR;
  if (f & ADC_ISR_ENDRX) {

     if (bufnw==bufr) { 
        buflost++;
     }
     //
     bufw = bufnw;
     bufnw = (bufnw+1) & (cBufferCount-1);
     ADC->ADC_RNPR=(uint32_t)buf[bufnw];
     ADC->ADC_RNCR = cBufferSize;
     ticks++;
  }
}

///////////////////////////////////////////////
// WATCH
///////////////////////////////////////////////
Watch watch = Watch(tft,touch,cActualFreq,cSampleRange);

///////////////////////////////////////////////
// Ticks
///////////////////////////////////////////////

#define ticksByMilliSeconds 1
volatile long milliSeconds = 0;
volatile long tickms = 0;
void tickmsIRQ(void) {
   milliSeconds++;
   watch.irqMilliSeconds(milliSeconds);
   tickms++;
}

void setupADC() {
  
  pmc_enable_periph_clk(ID_ADC);
  adc_init(ADC, SystemCoreClock, ADC_FREQ_MAX, ADC_STARTUP_FAST);
  
  //ADC->ADC_MR |= ADC_MR_FREERUN_ON; // free running
  ADC->ADC_MR |= ADC_MR_TRGEN_EN  | ADC_MR_TRGSEL_ADC_TRIG1; // Timer TIOA channel 0

  // Channel selection
  // ADC Channel 7 = PA16 = Bord Analog0
  ADC->ADC_CHER = ADC_CHER_CH7; 
  //ADC->ADC_CGR = ADC_CGR_GAIN0(2);
  
  NVIC_EnableIRQ(ADC_IRQn);
  ADC->ADC_IDR = ~(ADC_IER_ENDRX);
  ADC->ADC_IER = ADC_IER_ENDRX;
  ADC->ADC_RPR = (uint32_t)buf[0];   // DMA buffer
  ADC->ADC_RCR = cBufferSize;
  ADC->ADC_RNPR = (uint32_t)buf[1]; // next DMA buffer
  ADC->ADC_RNCR = cBufferSize;

  bufw=bufr=0;
  bufnw=1;
  buflost = 0;
  
  ADC->ADC_PTCR = ADC_PTCR_RXTEN;
  ADC->ADC_CR = ADC_CR_START;
}

void setupTFT() {
  tft.begin();
  tft.setOrientation(landscape); 
  tft.clearScreen(COLOR_BLACK);
}

void setupWatch() {
  
  watch.setup();  
  touch.begin();
  
  //Calibrate();

  #ifdef STM32_1
  touch.xCalib.calibrate(5,3727,314,348);
  touch.yCalib.calibrate(5,3296,234,425);
  #endif
  
  #ifdef STM32_2
  touch.xCalib.calibrate(5,297,314,3623);
  touch.yCalib.calibrate(5,388,234,3651);
  #endif
}

void setup(){
  pinMode(LED_BUILTIN, OUTPUT);

  #ifdef SERIALUSB
  SerialUSB.begin(0);
  while(!SerialUSB);
  #endif

  Serial.begin(9600);
  Serial.println("Serial start");

  setupTFT();
  setupWatch();
  
  setupADC();

  // Timer 0 sample frequency
  Timer0.setFrequency(cSampleFreq).start();
  
  //touch.calibrate(tft);
  touch.xCalib.calibrate(5,312,793,7978);
  touch.yCalib.calibrate(5,7618,473,350);
  //(TS) Calibrate P1:5 V1:312 P2:793 V2:7978
  //(TS) Calibrate P1:5 V1:7618 P2:473 V2:344
  
  //(TS) Calibrate P1:5 V1:326 P2:793 V2:7972
  //(TS) Calibrate P1:5 V1:7604 P2:473 V2:400
  
  // 1 ms ticks
  Timer2.setPeriod(1000).attachInterrupt(tickmsIRQ).start();
}

void loop(){

  if (bufr!=bufw) {

   #ifdef SERIALUSB
    SerialUSB.write((uint8_t *)buf[bufr],2*cBufferSize); // send it - 512 bytes = 256 uint16_t
    #endif
    
    watch.processBuffer(buf[bufr], cBufferSize);
    
    bufr = (bufr+1) & (cBufferCount-1);
  }

  if (tickms>0) {
    
    watch.loopMilliSeconds(milliSeconds);
    tickms=0;
  }
 
  if (ticks>ticksPerSecond) {
    digitalWrite(pinLED, ! digitalRead(pinLED));

    watch.loopSeconds(buflost);
        
    ticks = 0;
  }  
}
