/* 
 * File:   max7219.h
 * Author: syfre
 * MAXIM max7219
 * Created on 8 mai 2014, 21:13
 */

 
#ifndef MAX7219_H
#define	MAX7219_H
#include "Arduino.h"

#define DIGIT_COUNT 8

class Max7219
{
  public:
    Max7219(char _pinData, char _pinClock, char _pinLoad);
	void reset();
	void printFixedPoint(long value);
	void print(long value);
	void print(unsigned long value);
	void print(int value);
	void print(unsigned int value);
	void print(unsigned char value);
	void print(float value, int decimals);
   private:
    unsigned char pinData;
    unsigned char pinClock;
    unsigned char pinLoad;
	char digits[DIGIT_COUNT+1]; 	// right aligned, 0:left most, 7:right most ; on decimal point
	inline void _send(unsigned char);
	inline void _load(void);
	void _send1(unsigned char);
	void _send2(unsigned char, unsigned char);
	void _output_digits(void);
};

#endif	/* MAX7219_H */

