/*
 * STM32F4-DISCOVERY
 * Example 1:  LEDs
 */
 
 #define LED1	PA6
 #define LED2	PA7
 
void setup() {
  // put your setup code here, to run once:
  
  // Set the MCU's pin data direction.
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  // Set all outputs LOW to have all LED's initially turned off.
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:

  // Turn off the GREEN LED, turn on the ORANGE LED and wait for 1000ms.
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, HIGH);
  delay(1000);
  
  // Turn off the ORANGE LED, turn on the RED LED and wait for 1000ms.
  digitalWrite(LED2, LOW);
  digitalWrite(LED1, HIGH);
  delay(1000);

}