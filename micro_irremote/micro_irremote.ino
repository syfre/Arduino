/* Pro Micro Test Code
   by: Nathan Seidle
   modified by: Jim Lindblom
   SparkFun Electronics
   date: September 16, 2013
   license: Public Domain - please use this code however you'd like.
   It's provided as a learning tool.

   This code is provided to show how to control the SparkFun
   ProMicro's TX and RX LEDs within a sketch. It also serves
   to explain the difference between Serial.print() and
   Serial1.print().

   Board : Arduino/Genuino Micro
   COM : COMx Arduino/Genuino Micro
   Reset twice to enter boot loader
*/

#include <IRremote.h>

int RECV_PIN = 7;        // IR LED pin
IRrecv irrecv(RECV_PIN);
decode_results results;
int Code = 0;

int RXLED = 17;  // The RX LED has a defined Arduino pin
// The TX LED was not so lucky, we'll need to use pre-defined
// macros (TXLED1, TXLED0) to control that.
// (We could use the same macros for the RX LED too -- RXLED1,
//  and RXLED0.)

void setup()
{
 pinMode(RXLED, OUTPUT);  // Set RX LED as an output
 // TX LED is set as an output behind the scenes

 Serial.begin(9600); //This pipes to the serial monitor

 irrecv.enableIRIn();   // Start the receiver

 Serial.println("IR Receiver start");  // Print "Hello World" to the Serial Monitor
}

void loop()
{
  if (irrecv.decode(&results)) {
    Serial.print("Receive"); 
    Serial.print(" D:"); encoding(&results); 
    Serial.print(" B:"); Serial.print(results.bits, DEC); 
    Serial.print(" hV:"); Serial.print(results.value, HEX);
    Serial.print(" V high:"); Serial.print(results.value >> 16, DEC);
    Serial.print(" V low:"); Serial.println(results.value & 0x0000FFFF, DEC);
    Code = results.value;
    irrecv.resume(); // Receive the next value
  
    digitalWrite(RXLED, !digitalRead(RXLED));
  }

 TXLED0; //TX LED is not tied to a normally controlled pin
 delay(500);              // wait for a second
 TXLED1;
 delay(500);              // wait for a second
}


void  encoding (decode_results *results)
{
  switch (results->decode_type) {
    default:
    case UNKNOWN:      Serial.print("UNKNOWN");       break ;
    case NEC:          Serial.print("NEC");           break ;
    case SONY:         Serial.print("SONY");          break ;
    case RC5:          Serial.print("RC5");           break ;
    case RC6:          Serial.print("RC6");           break ;
    case DISH:         Serial.print("DISH");          break ;
    case SHARP:        Serial.print("SHARP");         break ;
    case JVC:          Serial.print("JVC");           break ;
    case SANYO:        Serial.print("SANYO");         break ;
    case MITSUBISHI:   Serial.print("MITSUBISHI");    break ;
    case SAMSUNG:      Serial.print("SAMSUNG");       break ;
    case LG:           Serial.print("LG");            break ;
    case WHYNTER:      Serial.print("WHYNTER");       break ;
    case AIWA_RC_T501: Serial.print("AIWA_RC_T501");  break ;
    case PANASONIC:    Serial.print("PANASONIC");     break ;
    case DENON:        Serial.print("Denon");         break ;
  }
}
