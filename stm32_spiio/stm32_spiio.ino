#include <tftio.h>
#include <spiio.h>

#define pinLED  PC13
#define pinOUT  PB0
#define freqKhz         50
#define periodus        1000 / freqKhz
#define ticksBySecond   1000 * freqKhz

SPIIO spi;

#define TRSLEN  98000
#define BUFLEN  512
uint16_t buffer[BUFLEN];

long ticks;
void TimerIRQ(void) {
  digitalWrite(pinOUT, ! digitalRead(pinOUT));
  ticks++;
}

long call;
long callback;
void spiCallBack(void *p) {

    callback++;
}

void setup() {
  Serial.begin(9600);
  pinMode(pinLED, OUTPUT);
  pinMode(pinOUT, OUTPUT);
  digitalWrite(pinLED, 1);

  ticks = 0;
  Timer3.setPeriod(periodus);
  Timer3.attachInterrupt(3, TimerIRQ);

  spi.init();

  call = 0,
  callback = 0;
  for(int i=0; i<BUFLEN;i++) buffer[i] = 0xFFFF;
}

void loop() {
  // put your main code here, to run repeatedly:

 if (ticks==ticksBySecond) {
    //digitalWrite(pinLED, ! digitalRead(pinLED));


    call++;
    spi.write_buffer_uint(buffer,TRSLEN,0,spiCallBack,NULL);
    call++;
    spi.write_buffer_uint(buffer,TRSLEN,0,spiCallBack,NULL);
  
    
    ticks = 0;
  }

}
