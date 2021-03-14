#include <DueTimer.h>

#define SERIALUSB

#define cSampleFreq 10000
#define cSampleRange 2048
#define cBufferCount 4
#define cBufferSize 256

volatile long ticks;
volatile int bufn,obufn;
uint16_t buf[cBufferCount][cBufferSize];   // 4 buffers of 256 readings

void ADC_Handler(){     // move DMA pointers to next buffer
  int f = ADC->ADC_ISR;
  if (f & ADC_ISR_ENDRX) {
   bufn=(bufn+1) & (cBufferCount-1);
   ADC->ADC_RNPR=(uint32_t)buf[bufn];
   ADC->ADC_RNCR = cBufferSize;
  } 
  ticks++;
}

void setup(){
  pinMode(LED_BUILTIN, OUTPUT);
  
  #ifdef SERIALUSB
  SerialUSB.begin(0);
  while(!SerialUSB);
  #endif
  
  //Serial.begin(9600);
  
  pmc_enable_periph_clk(ID_ADC);
  adc_init(ADC, SystemCoreClock, ADC_FREQ_MAX, ADC_STARTUP_FAST);
  
  //ADC->ADC_MR |= ADC_MR_FREERUN_ON; // free running
  ADC->ADC_MR |= ADC_MR_TRGEN_EN  | ADC_MR_TRGSEL_ADC_TRIG1; // Timer TIOA channel 0

  // Channel selection
  // ADC Channel 7 = PA16 = Bord Analog0
  ADC->ADC_CHER = ADC_CHER_CH7; 

  NVIC_EnableIRQ(ADC_IRQn);
  ADC->ADC_IDR = ~(ADC_IER_ENDRX);
  ADC->ADC_IER = ADC_IER_ENDRX;
  ADC->ADC_RPR = (uint32_t)buf[0];   // DMA buffer
  ADC->ADC_RCR = cBufferSize;
  ADC->ADC_RNPR = (uint32_t)buf[1]; // next DMA buffer
  ADC->ADC_RNCR = cBufferSize;
  bufn=obufn=1;
  ADC->ADC_PTCR = ADC_PTCR_RXTEN;
  ADC->ADC_CR = ADC_CR_START;

  // Timer 0 start at 10Khz
  Timer0.setFrequency(cSampleFreq).start();
}

void loop(){

  /*
  if (ticks==10000) {
    digitalWrite(LED_BUILTIN, ! digitalRead(LED_BUILTIN));
    ticks=0;
  }
  */
  if (obufn!=bufn) {
    
    digitalWrite(LED_BUILTIN, ! digitalRead(LED_BUILTIN));
    
    #ifdef SERIALUSB
    SerialUSB.write((uint8_t *)buf[obufn],2*cBufferSize); // send it - 512 bytes = 256 uint16_t
    #endif
   
    // next obufn
    obufn = (obufn+1) & (cBufferCount-1);    
  }
}
