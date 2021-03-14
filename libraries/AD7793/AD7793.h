/*
The MIT License (MIT)

Copyright (c) 2017 Sylvain Frere syfre92@gmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef _AD7793_H_
#define _AD7793_H_

#include <arduino.h>
#include <SPI.h>

#define CHANNEL_AN1		0
#define CHANNEL_AN2		1
#define CHANNEL_AN3		2
#define CHANNEL_TEMP	3
#define CHANNEL_AVDD	4

enum ad7793Bias { bias_none, bias_an1, bias_an2};

class AD7793 {
protected:
	uint8_t	pinCS;
	
	uint8_t readReg8bits(uint8_t reg);
	void readReg16bits(uint8_t reg, uint8_t& val1, uint8_t& val2);
	void writeReg16bits(uint8_t reg, uint8_t val1, uint8_t val2);
	uint16_t readReg16bits(uint8_t reg);
	void setChannel(uint8_t ch);
	void setMode(void);
public:
	AD7793(uint8_t _cs) :pinCS(_cs) {};
	void begin();
	void initialize(uint8_t ch=0);
	
	void reset(void);
	uint8_t readID(void);
	uint8_t readStatus(void);
	uint16_t readMode(void);
	uint16_t readConfig(void);

	void selectChannel(uint8_t ch);
	void selectGain(uint8_t bias);
	void selectBias(ad7793Bias bias);

	void calibrate(uint8_t ch);
	long read(uint8_t ch);
	double ToVolt(long value);
	
};


#endif 