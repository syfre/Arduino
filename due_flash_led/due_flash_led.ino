#include <DueTimer.h>

#ifdef  _VARIANT_ARDUINO_DUE_X_
  #pragma message "_VARIANT_ARDUINO_DUE_X_"
#endif
#ifdef _SAM3XA_
  #pragma message  "_SAM3XA_"
#endif
#ifdef __arm__
 #pragma message  "__arm__"
#endif
#ifdef __ARM__
 #pragma message "__ARM__"
#endif
#ifdef STM32
 #pragma message "STM32"
#endif
#ifdef __STM32__
 #pragma message "__STM32__"
#endif
#ifdef __STM32F1__
 #pragma message "STM32F1"
#endif
#ifdef __ARDUINO_ARCH_STM32__
 #pragma message "__ARDUINO_ARCH_STM32__"
#endif
#ifdef ARDUINO_ARCH_STM32
 #pragma message "ARDUINO_ARCH_STM32"
#endif
#ifdef ARDUINO_ARCH_STM32F1
 #pragma message "ARDUINO_ARCH_STM32F1"
#endif


#define pinLED  LED_BUILTIN
#define pinOUT  11

#define frequency       50000
#define periodus        (uint32_t) 1000000 / frequency
#define ticksBySecond   (uint32_t) frequency

long ticks;

void timerIRQ(void) {
  digitalWrite(pinOUT, ! digitalRead(pinOUT));
  ticks++;
}

void setup() {
  Serial.begin(9600);
  pinMode(pinLED, OUTPUT);
  pinMode(pinOUT, OUTPUT);
  Serial.println("START");  

  ticks = 0;
  Timer3.attachInterrupt(timerIRQ).setFrequency(frequency).start();
}

void loop() {
  if (ticks>=ticksBySecond) {
    digitalWrite(pinLED, ! digitalRead(pinLED));
    ticks = 0;
  }
}
