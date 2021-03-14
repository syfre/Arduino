#include <HardwareTimer.h>

#define pinLED  PC13  // build-in
#define pinOUT  PB0
#define pinPPS  PB9

#define freqKhz         200
#define periodus        (uint32_t) 1000 / freqKhz
#define ticksBySecond   (uint32_t) 1000 * freqKhz

long ticks;
void TimerIRQ(void) {
  digitalWrite(pinOUT, ! digitalRead(pinOUT));
  ticks++;
}

long ppsTicks;
void ppsIRQ(void) {
   ppsTicks = ticks;
   ticks = 0;
}


void setup() {
  Serial.begin(115200);
  pinMode(pinLED, OUTPUT);
  pinMode(pinOUT, OUTPUT);
  pinMode(pinPPS, INPUT);
  Serial.println("START");  

  pinMode(pinPPS, INPUT);
  attachInterrupt(pinPPS, ppsIRQ, RISING);

  ticks = 0;
  ppsTicks = 0;
  Timer3.setPeriod(periodus);
  Timer3.attachInterrupt(3, TimerIRQ);
}

void loop() {

  if (ppsTicks) {
    char buffer[40];
    sprintf(buffer,"%u",ppsTicks);
    Serial.println(buffer);
    ppsTicks = 0;
  }
  
  if (ticks==ticksBySecond) {
    digitalWrite(pinLED, ! digitalRead(pinLED));
    //Serial.println("Hello World");  
    ticks = 0;
  }
}
