/*
 * File:   max7219.c
 * Author: syfre
 * MAXIM max7219
 * Created on 8 mai 2014, 21:13
 */

#include "Arduino.h"
#include "Max7219.h"

// MAX commands
#define MAX7219_NOOP            0x00
#define MAX7219_DIGIT0          0x01
#define MAX7219_DIGIT1          0x02
#define MAX7219_DIGIT2          0x03
#define MAX7219_DIGIT3          0x04
#define MAX7219_DIGIT4          0x05
#define MAX7219_DIGIT5          0x06
#define MAX7219_DIGIT6          0x07
#define MAX7219_DIGIT7          0x08
#define MAX7219_DECODEMODE      0x09
#define MAX7219_INTENSITY       0x0A
#define MAX7219_SCANLIMIT       0x0B
#define MAX7219_SHUTDOWN        0x0C
#define MAX7219_DISPLAYTEST     0x0F

#define MAX7219_SHUTDOWN_ON             0x00
#define MAX7219_SHUTDOWN_OFF            0x01
#define MAX7219_DECODEMODE_ALL_BCD      0xFF

#define BCD_0                   0x00
#define BCD_1                   0x01
#define BCD_2                   0x02
#define BCD_3                   0x03
#define BCD_4                   0x04
#define BCD_5                   0x05
#define BCD_6                   0x06
#define BCD_7                   0x07
#define BCD_8                   0x08
#define BCD_9                   0x09
#define BCD_MINUS               0x0A
#define BCD_E                   0x0B
#define BCD_H                   0x0C
#define BCD_L                   0x0D
#define BCD_P                   0x0E
#define BCD_BLANK               0x0F
#define BCD_DECIMAL             0x80

#define MAX7219_SCAN_0          0x00
#define MAX7219_SCAN_1          0x01
#define MAX7219_SCAN_2          0x02
#define MAX7219_SCAN_3          0x03
#define MAX7219_SCAN_4          0x04
#define MAX7219_SCAN_5          0x05
#define MAX7219_SCAN_6          0x06
#define MAX7219_SCAN_7          0x07

#define _DELAY_DATA() delayMicroseconds(1)
#define _DELAY_LOAD() delayMicroseconds(1)

Max7219::Max7219(char _pinData, char _pinClock, char _pinLoad) {
 // Assign variables.
  pinData = _pinData;
  pinClock = _pinClock;
  pinLoad = _pinLoad;
  // Set pins to input.
  pinMode(pinData, OUTPUT);
  pinMode(pinClock, OUTPUT);
  pinMode(pinLoad, OUTPUT);
  reset();
}

void Max7219::_send(unsigned char b) {
    digitalWrite(pinData,b);
    _DELAY_DATA();
    digitalWrite(pinClock,HIGH);
    _DELAY_DATA();
    digitalWrite(pinClock,LOW);
    _DELAY_DATA();
}

void Max7219::_load() {
    digitalWrite(pinLoad,HIGH);
    _DELAY_LOAD();
    digitalWrite(pinLoad,LOW);
    _DELAY_LOAD();
}

void Max7219::_send1(unsigned char data) {

    // data 8 bits
    //
    for (unsigned char i=0; i<8; i++) {
        if (data & 0x80) {_send(HIGH); } else { _send(LOW); }
        data = data << 1;
    }
}

void Max7219::_send2(unsigned char addr, unsigned char data) {
        _send1(addr);
        _send1(data);
        _load();
}

void Max7219::reset() {
    // Reset maxim 7219
    //
    digitalWrite(pinClock,LOW);
    digitalWrite(pinLoad,LOW);
	//
    _send2(MAX7219_SCANLIMIT, MAX7219_SCAN_7);
    _send2(MAX7219_DECODEMODE, MAX7219_DECODEMODE_ALL_BCD );
    _send2(MAX7219_DIGIT7,BCD_BLANK);
    _send2(MAX7219_DIGIT6,BCD_BLANK);
    _send2(MAX7219_DIGIT5,BCD_BLANK);
    _send2(MAX7219_DIGIT4,BCD_BLANK);
    _send2(MAX7219_DIGIT3,BCD_BLANK);
    _send2(MAX7219_DIGIT2,BCD_BLANK);
    _send2(MAX7219_DIGIT1,BCD_BLANK);
    _send2(MAX7219_DIGIT0,BCD_0);
    //
    _send2(MAX7219_SHUTDOWN, MAX7219_SHUTDOWN_OFF);
}

void Max7219::_output_digits() {

    unsigned char c;
	unsigned char dp;
	unsigned char j;
	dp = 0; j = 1;
    for (unsigned char i=1; i<=DIGIT_COUNT; i++) {
        c = digits[DIGIT_COUNT-i];
		switch (c) {
			case '-' : _send2(j++,BCD_MINUS); break;
			case '.' : dp = 0x80; break;
			case ' ' : _send2(j++, BCD_BLANK); break;
			case 'E' : _send2(j++, BCD_E); break;
			case 'H' : _send2(j++, BCD_H); break;
			case 'L' : _send2(j++, BCD_L); break;
			case 'P' : _send2(j++, BCD_P); break;
			default  :
				if ((c>=0x30)&&(c<=0x39)) {
					_send2(j++, (c-0x30) | dp);
					dp = 0;
				}
		}
    }

}

void Max7219::printFixedPoint(long value) {
	for (unsigned char i=0; i<4; i++) digits[i]=' ';
	sprintf(digits+4,"%04d",value);
	_output_digits();
	_send2(MAX7219_DIGIT3,BCD_DECIMAL+digits[4]-0x30);
}

void Max7219::print(long value) {
	sprintf(digits,"%8d",value);
    _output_digits();
}

void Max7219::print(unsigned long value) {
	sprintf(digits,"%8u",value);
    _output_digits();
}

void Max7219::print(unsigned char value) {
	sprintf(digits,"%8u",value);
	_output_digits();
}

void Max7219::print(int value) {
	sprintf(digits,"%8d",value);
	_output_digits();
}

void Max7219::print(unsigned int value) {
	sprintf(digits,"%8u",value);
	_output_digits();
}

void Max7219::print(float value, int decimals) {
	char fmt[6];
	if (decimals>99) decimals = 99;
	sprintf(fmt,"%%.%df",decimals);
	sprintf(digits,fmt,value);
	_output_digits();
}

