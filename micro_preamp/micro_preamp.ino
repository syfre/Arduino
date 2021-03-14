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
#include <TimerThree.h>
#include <IRremote.h>
#include <IRmapping_car_mp3.h>
#include <IRmapping_zero_zone.h>
#include <Rotary.h>

#define _WATCHDOG_
#define _TEST_WATCHDOG_

#ifdef _WATCHDOG_
// specific
#include <avr/wdt.h>
#endif

#define TXLEDToggle() PORTD ^= (1<<5) 
#define RXLEDToggle() PORTB ^= (1<<0) 

///////////////////////////////////////////////
// Hardware
///////////////////////////////////////////////
int RECV_PIN = 7;       // IR LED pin
int MOTOR_P1 = 2;       // Motor P1 plus
int MOTOR_P2 = 3;       // Motor P2 minus
int SSD_MAIN_POWER  = 4;     // Solid state relay POWER
int LED_POWER_MAIN = 5; // LED Main power ON
int LED_POWER_AUX  = 6; // LED Aux  power ON / Flash IR
int MAIN_POWER_IN = A0;  // IN main power sensor
int ENCODER_A = 8;      // Selector encoder A
int ENCODER_B = 9;      // Selector encoder B


///////////////////////////////////////////////
// INPUT SELECTOR
///////////////////////////////////////////////
typedef
  enum {
    INPUT_SEL_1 = 0,
    INPUT_SEL_2 = 1,
    INPUT_SEL_3 = 2,
    INPUT_SEL_4 = 3,
    INPUT_SEL_MUTE = 4
} input_state_type_t;

input_state_type_t  input_state = INPUT_SEL_MUTE;
input_state_type_t  input_before_mute = INPUT_SEL_MUTE;

#define SEL_INPUT_MAX 4
int SEL_INPUT[SEL_INPUT_MAX]  = {15,14,11,10};

///////////////////////////////////////////////
// MOTOR
///////////////////////////////////////////////
typedef
  enum {
    VOL_MOTOR_OFF = 0,
    VOL_MOTOR_PLUS  = 1,
    VOL_MOTOR_MINUS = 2
} motor_state_type_t;

#define MOTOR_DELAY 1500
volatile unsigned long tickmotor = 0;

///////////////////////////////////////////////
// POWER
///////////////////////////////////////////////

// 1:main power is off
#define main_power_on() digitalRead(MAIN_POWER_IN)
#define main_power_is_on()  !main_power_on()
#define main_power_is_off()  main_power_on()

// Power led is cathode commun
#define main_power_leds()  { digitalWrite(LED_POWER_MAIN, !digitalRead(MAIN_POWER_IN)); digitalWrite(LED_POWER_AUX, digitalRead(MAIN_POWER_IN)); }
#define main_power_led_on() digitalWrite(LED_POWER_MAIN, HIGH)
#define main_power_led_off() digitalWrite(LED_POWER_MAIN, LOW)

#define aux_power_led_on() digitalWrite(LED_POWER_AUX, HIGH)
#define aux_power_led_off() digitalWrite(LED_POWER_AUX, LOW)

// SSD main power
#define power_On() digitalWrite(SSD_MAIN_POWER,HIGH)
#define power_Off() digitalWrite(SSD_MAIN_POWER,LOW)

#define led  17
#define led_on() digitalWrite(led, LOW)
#define led_off() digitalWrite(led, HIGH)

#define flashOn() { aux_power_led_off(); main_power_led_off(); }
#define flashOff() 

#define FLASH_DELAY 200
volatile unsigned long tickflash = 0;

#define POWER_FAULT_DELAY  2000
int main_power_state;
volatile unsigned long tickPowerFault = 0;

#define POWER_UP_DELAY  2000
volatile unsigned tickPowerUp = 0;

///////////////////////////////////////////////
// RECEIVER
///////////////////////////////////////////////
IRrecv irrecv(RECV_PIN);
decode_results results;
unsigned long newKey = 0;
unsigned long lastKey = 0;

///////////////////////////////////////////////
// Encoder
///////////////////////////////////////////////
Rotary encoder = Rotary(ENCODER_A, ENCODER_B);

int RXLED = 17;  // The RX LED has a defined Arduino pin
// The TX LED was not so lucky, we'll need to use pre-defined
// macros (TXLED1, TXLED0) to control that.
// (We could use the same macros for the RX LED too -- RXLED1,
//  and RXLED0.)

///////////////////////////////////////////////
// Timer
///////////////////////////////////////////////
#define freqKhz         1
#define periodus        1000 / freqKhz
#define ticksBySecond   1000 * freqKhz
#define ticksByMilliSeconds   freqKhz

volatile unsigned long milliSeconds;
volatile unsigned long ticks;
volatile unsigned long tickms;

void TimerIRQ(void) {
  //digitalWrite(pinOUT, ! digitalRead(pinOUT));
  ticks++;
  tickms++;
}

void setup()
{
  // init motor
  pinMode(MOTOR_P1,OUTPUT);
  digitalWrite(MOTOR_P1,HIGH);
  pinMode(MOTOR_P2,OUTPUT);
  digitalWrite(MOTOR_P2,HIGH);
  motor(VOL_MOTOR_OFF);

  // Init input selector
  for (int i=0; i<SEL_INPUT_MAX; i++) {
    pinMode(SEL_INPUT[i],OUTPUT);
    digitalWrite(SEL_INPUT[i],HIGH);
  }

  // Init POWER SSD default Off
  pinMode(SSD_MAIN_POWER,OUTPUT);
  digitalWrite(SSD_MAIN_POWER,LOW);
  
  // Main power leds, 
  pinMode(LED_POWER_MAIN,OUTPUT);
  pinMode(LED_POWER_AUX,OUTPUT);
  pinMode(MAIN_POWER_IN,INPUT);
  main_power_state = main_power_on();

  // Internal pin led
  pinMode(led, OUTPUT);  // Set RX LED as an output
  led_off();

  // IR
  irrecv.enableIRIn();   // Start the receiver

  // Start timer
  ticks = 0;
  tickms = 0;
  milliSeconds = 0;
  Timer3.initialize(periodus);
  Timer3.attachInterrupt(TimerIRQ);

  Serial.begin(9600); //This pipes to the serial monitor
  Serial.println("Preamp start");  // Print "Hello World" to the Serial Monitor

  // To execute initialization code after setup()
  tickPowerUp = POWER_UP_DELAY;
}

void loop()
{
  if (tickms>=ticksByMilliSeconds) {
    //digitalWrite(pinLED, ! digitalRead(pinLED));
    milliSeconds++;

    // Main power LED status
    if (!tickflash) {
      main_power_leds();
    }
   
    if (main_power_state != main_power_on()) {
      // detect change of power state, only for trace
      main_power_state = main_power_on();
      Serial.print("main power change ");
      if (main_power_state) Serial.println("Off"); else Serial.println("On");
    }

    if (irrecv.decode(&results)) {
      if (main_power_is_on()) Serial.print("Power is On "); else Serial.print("Power is off ");
      Serial.print("Receive"); 
      Serial.print(" D:"); Serial.print(results.decode_type,DEC);
      Serial.print(" B:"); Serial.print(results.bits, DEC); 
      Serial.print(" V:"); Serial.println(results.value, HEX);
      newKey = results.value;
      irrecv.resume(); // Receive the next value

      flashOn();
      tickflash = FLASH_DELAY;
    }

    if (tickPowerUp>0) {
      tickPowerUp--;
      if (!tickPowerUp) {
          // Executed only once after start        
          // It looks that there is code which set up this led on
          // and disable the watchdog after setup() has been called
          //          
          led_off();
          #ifdef _WATCHDOG_
          wdt_enable(WDTO_2S); 
          #endif
     }
    }

    if (tickflash>0) {
      tickflash--;
      if (!tickflash) flashOff();
    }

    if (tickmotor>0) {
      tickmotor--;
      if (!tickmotor) motor(VOL_MOTOR_OFF);
    }


    if (tickPowerFault>0) {
      tickPowerFault--;
      if (!tickPowerFault) {
        if (main_power_is_off()) {
          // This is a fault
          // Whe have SSD power ON and main power sensor still off
          // Go power Off
          Serial.println("Power Fault ; Go power off");
          power_Off();
        }
      }
    }

    if (newKey==cKey_CMP3_repeat) {
      newKey = lastKey;
    }

    if (newKey ==  cKey_CMP3_ch_plus) {
      // ON / OFF key
      //
      Serial.println("Key Power On/Off");
      //
      if (main_power_is_on()) {
        // Mute input
        Serial.println("Go Power Off");
        if (input_state!=INPUT_SEL_MUTE) {
          input(INPUT_SEL_MUTE);
        }
        // Power off
        power_Off();
      }
      else {
        // Power ON
        Serial.println("Go Power On");
        power_On();
        tickPowerFault = POWER_FAULT_DELAY;
        
        // Restore input
        input(INPUT_SEL_MUTE);
      }
    }

    #ifdef _TEST_WATCHDOG_
    if (newKey == cKey_CMP3_eq) {
      // Test watch dog
      // disable timer
      Timer3.stop();
    }
    #endif

    if (main_power_is_on()) {

        // only if main power ON

        switch (newKey) {
          
          case cKey_CMP3_plus : 
            motor(VOL_MOTOR_PLUS);
            break;
          case cKey_CMP3_minus : 
            motor(VOL_MOTOR_MINUS);
            break;
          case cKey_CMP3_0 :
            input(INPUT_SEL_MUTE);
            break;
          case cKey_CMP3_1 :
            input(INPUT_SEL_1);
            break;
          case cKey_CMP3_2 :
            input(INPUT_SEL_2);
            break;
          case cKey_CMP3_3 :
            input(INPUT_SEL_3);
            break;
          case cKey_CMP3_4 :
            input(INPUT_SEL_4);
            break;
        }

        unsigned char encrslt = encoder.process();
        if (encrslt) {
           Serial.print("Selector :"); Serial.println(encrslt == DIR_CW ? "right" : "left");
           if (encrslt==DIR_CW) 
                 input_inc();
           else  input_dec();
        }
    }

    lastKey = newKey;
    newKey = 0;
    tickms = 0;
  }

  if (ticks>=ticksBySecond) {
    //digitalWrite(pinLED, ! digitalRead(pinLED));
  
    TXLEDToggle();
    #ifdef _WATCHDOG_
    wdt_reset(); 
    #endif
    
    ticks = 0;
  }
}

void motor(motor_state_type_t state) {

  digitalWrite(MOTOR_P1,HIGH);
  digitalWrite(MOTOR_P2,HIGH);
  switch (state) {
    case VOL_MOTOR_OFF : 
      Serial.println("Motor OFF");
      tickmotor = 0;
      break;
    case VOL_MOTOR_MINUS : 
      digitalWrite(MOTOR_P2,LOW);
      Serial.println("Motor vol - ON");
      tickmotor = MOTOR_DELAY;
      break;
    case VOL_MOTOR_PLUS  : 
      digitalWrite(MOTOR_P1,LOW);
      Serial.println("Motor vol + ON");
      tickmotor = MOTOR_DELAY;
      break;
  }
}

void input(input_state_type_t sel) {

    // off
    for (int i=0; i<SEL_INPUT_MAX; i++) {
      digitalWrite(SEL_INPUT[i],HIGH);
    }

    // mute twice restore last input
    //
    if ((sel==INPUT_SEL_MUTE) && (input_state==INPUT_SEL_MUTE)) {
      // already mute restore last
      sel = input_before_mute;
      input_before_mute = INPUT_SEL_MUTE;
    }
    
    switch (sel) {
      case INPUT_SEL_MUTE:
      input_before_mute = input_state;
      Serial.println("Sel input MUTE");
      break;
      
      case INPUT_SEL_1 :
      case INPUT_SEL_2 :
      case INPUT_SEL_3 :
      case INPUT_SEL_4 :
      digitalWrite(SEL_INPUT[sel],LOW);
      Serial.print("Sel input "); Serial.println(sel+1);
      break;
  }

  input_state = sel;
}

void input_inc() {

  // increment input

  if ((input_state==INPUT_SEL_MUTE) && (input_before_mute!=INPUT_SEL_MUTE)) {
    input(input_before_mute);
    return;
  }

  if (input_state==INPUT_SEL_MUTE) {
    input(INPUT_SEL_1);
    return;
  }

  if (input_state < INPUT_SEL_4) input(input_state+1); else input(INPUT_SEL_1);
}

void input_dec() {

  // decrement input

  if ((input_state==INPUT_SEL_MUTE) && (input_before_mute!=INPUT_SEL_MUTE)) {
    input(input_before_mute);
    return;
  }

  if (input_state==INPUT_SEL_MUTE) {
    input(INPUT_SEL_4);
    return;
  }

  if (input_state > INPUT_SEL_1) input(input_state-1); else input(INPUT_SEL_4);
}


