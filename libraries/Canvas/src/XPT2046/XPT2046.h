#ifndef __TOUCH__
#define __TOUCH__
/*
	XPT2046 support

	Copyright (C) 2017 sylvain Frere
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <Arduino.h>
#include <Canvas.h>

// This is dependant of the physical construction of the panel
// 	Portrait  : XPT2046.x === X_CHANNEL / XPT2046.y === Y_CHANNEL
// 	Landscape : XPT2046.x === Y_CHANNEL / XPT2046.y === X_CHANNEL
// Its depend how X_CHANNEL and Y_CHANNEL has been wired.

//#define TOUCH_PORTRAIT  0
//#define TOUCH_LANDSCAPE 1
//typedef int16_t COORD;
//typedef uint16_t Value;

// position = a * value + b
// we need two points to calibrate
// a = (v1-v2)/(p1-p2)
// b = p1-a*v1

class Calibration {
public:
    int an,ad,b;
    void initialize(void);
    void calibrate(COORD p1, Value v1, COORD p2, Value v2);
    COORD valueToPosition(Value value);
};

class XPT2046: public Touch {
protected:
    uint8_t pincs;
    uint8_t pinirq;
    uint16_t speed;
    uint8_t state;
    uint16_t nextTick;

	// hardware
    virtual void init(void) = 0;
    virtual Value read(uint8_t ctrl) = 0;
	
public:
    XPT2046(uint8_t _cs, uint8_t _irq, uint16_t _speed, uint8_t _mode=TOUCH_LANDSCAPE) 
		: pincs(_cs), pinirq(_irq), speed(_speed), Touch(_mode) { } ;
    //Value vx,vy;			 // Raw values
    //COORD x,y;				 // see Portrat / Landscape 
    Calibration xCalib;      // calibration data for x
    Calibration yCalib;      // calibration data for y
	void begin(void);
    void getPosition(void);
    Value getTemperature(void);
    Value getVBat(void);
    Value getVAux(void);
    bool touched(uint16_t tick);
    void calibrateX(COORD p1, Value v1, COORD p2, Value v2) {xCalib.calibrate(p1,v1,p2,v2); } ;
    void calibrateY(COORD p1, Value v1, COORD p2, Value v2) {yCalib.calibrate(p1,v1,p2,v2); } ;
};

class XPT2046SPI : public XPT2046 {
protected:
    void init(void);
    Value read(uint8_t ctrl);
public:
    XPT2046SPI(uint8_t _cs, uint8_t _irq, uint16_t _speed, uint8_t _mode=TOUCH_LANDSCAPE) 
		: XPT2046(_cs, _irq, _speed, _mode) { } ;
};

class XPT2046SOFT : public XPT2046 {
private:
	uint8_t pinClk;
	uint8_t pinDin;
	uint8_t pinDout;
	uint8_t send(uint8_t val);
protected:
    void init(void);
    Value read(uint8_t ctrl);
public:
    XPT2046SOFT(uint8_t _cs, uint8_t _irq, uint8_t _clk, uint8_t _din, uint8_t _dout, uint8_t _mode=TOUCH_LANDSCAPE) 
		: pinClk(_clk), pinDin(_din), pinDout(_dout), XPT2046(_cs, _irq, 0, _mode) { } ;
};

#define	Touch_xpt2046 XPT2046SPI


#endif