
#include <SPI.h>
#include <DueTimer.h>
#include <Canvas9341_due.h>
#include <Watch.h>
#include <WatchScreen.h>
#include <WatchVibrograph.h>
#include <Scope.h>


// pin definition for the TFT
#define cs   7
#define dc   3
#define rst  6
#define pinPPS  10

#define SERIALUSB_
#define _VIBRO
#define SCOPE
#define PPS

// max 300K ; over that we lost buffer
#define cSampleFreq 50000
#define cSampleRange 2048
#define cBufferCount 4
#define cBufferSize  10000
#define cTickRefresh 5

uint16_t* pbuf;
uint16_t buflost;
volatile int bufw,bufnw,bufr;              // bufwrite, nextbufwrite, bufread
uint16_t buf[cBufferCount][cBufferSize];   // 4 buffers of 256 readings

char charBuffer[40];

#ifdef SERIALUSB
Sample  sbuf[cBufferSize];
#endif

#ifdef DEBUG
char  debugBuffer[128];
#endif

Canvas9341 tft = Canvas9341(cs, dc, rst);

Watch watch = Watch(cSampleFreq,cSampleRange);
WatchScreen wscreen = WatchScreen(tft);
#ifdef VIBRO
WatchVibrograph wrecord = WatchVibrograph(tft, watch.ToTime(200.0),1,10.0);
#endif
#ifdef SCOPE
Scope wscope = Scope(tft, cSampleFreq,cSampleRange);
#endif

long ticks;
long seconds;

//////////////////////////////////////
// Measure PPS for calibration
//////////////////////////////////////
#ifdef PPS
long ppsTicks;

void timerIRQ(void) {
   ticks++;
}
void ppsIRQ(void) {
   ppsTicks = ticks;
   ticks = 0;
}
#endif

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

  }
}

void setupTFT() {

  tft.begin();
  tft.setOrientation(landscape); 
  tft.clearScreen();
}

void setupWatch() {

  seconds = 0;
  ticks = 0;
  buflost = 0;

  watch.begin();
  wscreen.begin();
  #ifdef VIBRO
  wrecord.begin();
  #endif
  #ifdef SCOPE
  wscope.begin();
  #endif
}

void setupADC(void) {
 
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
  
  ADC->ADC_PTCR = ADC_PTCR_RXTEN;
  ADC->ADC_CR = ADC_CR_START;

   // Timer 0 start at 10Khz
  Timer0.setFrequency(cSampleFreq).start();
}

#ifdef PPS
void setupPPS(void) {

  tft.setFontSize(3);
 
  pinMode(pinPPS, INPUT);
  attachInterrupt(pinPPS, ppsIRQ, RISING);

  Timer0.setFrequency(cSampleFreq).attachInterrupt(timerIRQ).start();
}
#endif

void setup(){
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  Serial.println("serial start");
  #ifdef SERIALUSB
  SerialUSB.begin(0);
  while(!SerialUSB);
  #endif
  
  setupTFT();
  #ifdef PPS
  setupPPS();
  #else
  setupWatch();
  setupADC();
  #endif
  
}

void loop(){

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

    ticks++;
    digitalWrite(LED_BUILTIN, ! digitalRead(LED_BUILTIN));
    
    #ifdef SERIALUSB_RAW
    SerialUSB.write((uint8_t *)buf[bufr],2*cBufferSize); // send it - 512 bytes = 256 uint16_t
    #endif
    //
    // obufn ready for processing
  
    pbuf = buf[bufr];
    for (int i=0; i<cBufferSize; i++) {
      // 
      #ifdef VIBRO
      if (watch.process(*pbuf++)) {
        wrecord.addBeat(watch.beatDetector.beat);
      }
      #endif
      
      #ifdef SCOPE
      if (watch.process(*pbuf++)) {
        wscope.trigger();
      }
      wscope.addSample(watch.sample,watch.beatDetector.beatOn());
      #endif
      
      #ifdef SERIALUSB
      sbuf[i] = sample;
      #endif
    }
    
    #ifdef SERIALUSB
    SerialUSB.write((uint8_t *)sbuf,4*cBufferSize); // send it - 512 bytes = 256 uint16_t
    #endif

    // next obufn
    bufr = (bufr+1) & (cBufferCount-1);
  }
  
  
  if (ticks==cTickRefresh) {
    seconds++;

    // every seconds
    watch.updateResult(buflost);
    wscreen.refresh(watch.result);

    ticks = 0;
  }
  
}
