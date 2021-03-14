#include "XPT2046.h"
#include <SPI.h>

///////////////////////////////////////////////////////////////////////////////
// Control byte
//  S A2 A1 A0 MODE SER/DFR PD1 PD0
//  S :         Start
//  A2..A0 :    Chanel select
//  MODE:       0:12 bits, 1:8bits
//  SER /DFR:   0:DFR differential , 1:SER=1 single-ended
//PD1..PD0:     power down mode
//
///////////////////////////////////////////////////////////////////////////////

// differentail
#define X_CHANEL        0b11010000
#define Y_CHANEL        0b10010000
// single ended
#define TEMP_CHANEL     0b10000100
#define VBAT_CHANEL     0b10100100
#define AUX_CHANEL      0b11100100

// In tick (= 1 ms)
#define cDelay_low      5
#define cDelay_hight    500

/////////////////////////////////////////////
// Warning : debug by serial in IRQ may crash
/////////////////////////////////////////////
#define DEBUG
//#define DEBUG_LED
//#define pinLED	PC13
////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define _Input(p)	{ pinMode(p, INPUT); }
#define _Output(p)	{ pinMode(p, OUTPUT); }
#define _pin_low(p)	{ digitalWrite(p,LOW); }
#define _pin_high(p) { digitalWrite(p,HIGH); }
#define _pin_toggle(p) { _pin_high(p); _pin_low(p); }
#define _pin_toggle_(p) { _pin_high(p); delayMicroseconds(1); _pin_low(p); delayMicroseconds(1); }
#define _pin(p)		digitalRead(p)

#define _IRQ()		  digitalRead(pinirq)
#define _CS_Off()	{ digitalWrite(pincs,1); }
#define _CS_On()	{ digitalWrite(pincs,0); }

#define _freq 2000000

#ifdef DATA_SIZE_8BIT
#define spi_begin()		{ SPI.beginTransaction(SPISettings(_freq, MSBFIRST, SPI_MODE0,DATA_SIZE_8BIT)); }
#else
#define spi_begin()		{ SPI.beginTransaction(SPISettings(_freq, MSBFIRST, SPI_MODE0)); }
#endif

#define spi_end()		{ SPI.endTransaction(); }
#define spi_write(u)	SPI.transfer(u)
#define spi_read()		SPI.transfer((uint8_t)0)

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void Calibration::initialize(void) {
    // initialize to identity
    an=1; ad=1; b=0;
}

void Calibration::calibrate(COORD p1, Value v1, COORD p2, Value v2) {

    an = (p2-p1);
    ad = (v2-v1);
    if (ad==0) {
        an=1; ad=1; b=0;
    } else {
        b = (int)p1-an*(int)v1/ad;
    }
	#ifdef DEBUG
	char buffer[80];
	sprintf(buffer,"(TS) Calibrate P1:%u V1:%u P2:%u V2:%u AN:%d AD:%d B:%d",p1,v1,p2,v2,an,ad,b);
	Serial.println(buffer);
	#endif
};

inline COORD Calibration::valueToPosition(Value value) {
    return (COORD)(an*(int)value/ad + b);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void XPT2046SPI::init(void) {
}

Value XPT2046SPI::read(uint8_t ctrl) {
    
	uint8_t rlo,rhi;
	uint16_t val;
	
	spi_begin();
	_CS_On();
	spi_write(ctrl);
    rhi = spi_read();
    rlo = spi_read();
	_CS_Off();
	spi_end();

	val = (uint16_t)((rhi << 8) + rlo) >> 3;

	#ifdef _DEBUG_
	char buffer[80];
	sprintf(buffer,"(TS) read H:%x L:%x V:%u",rhi,rlo,val);
	Serial.println(buffer);
	#endif

    return val;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void XPT2046SOFT::init(void) {
		_Output(pinClk);
		_pin_low(pinClk);
		_Output(pinDin);
		_pin_low(pinDin);
		_Input(pinDout);
}

uint8_t XPT2046SOFT::send(uint8_t val) {

	//return 0;
	
	// MSB first
	uint8_t rslt = 0;  
	for (int i=0; i<8; i++) {
		rslt <<= 1;
		if (val & 0x80) _pin_high(pinDin) else _pin_low(pinDin);
		_pin_toggle(pinClk);
		if (_pin(pinDout)) { rslt |= 0x01; }
		val <<= 1;
	}
	return rslt;
}

Value XPT2046SOFT::read(uint8_t ctrl) {
    
	uint8_t rlo,rhi;
	uint16_t val;
	
	_CS_On();
	send(ctrl);
    rhi = send(0);
    rlo = send(0);
	_CS_Off();

	val = (uint16_t)((rhi << 8) + rlo) >> 3;

	#ifdef DEBUG
	char buffer[80];
	sprintf(buffer,"(TS) read H:%x L:%x V:%u",rhi,rlo,val);
	Serial.println(buffer);
	#endif

    return val;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void XPT2046::begin(void) {

		_Output(pincs);
		_Input(pinirq);		
		init();
		_CS_Off();
		//
        state = 0;
        xCalib.initialize();
        yCalib.initialize();
}

void XPT2046::getPosition(void) {

    Value tv;
    vx = read(X_CHANEL);
    vy = read(Y_CHANEL);
    if (mode==TOUCH_LANDSCAPE) {
        tv = vy; vy = vx; vx = tv;
    }
    x = xCalib.valueToPosition(vx);
    y = yCalib.valueToPosition(vy);
	
	#ifdef DEBUG
	char buffer[80];
	sprintf(buffer,"(TS) GetPosition X:%u,%u Y:%u,%u",vx,x,vy,y);
	Serial.println(buffer);
	#endif
}

Value XPT2046::getTemperature() {

	uint16_t rslt = read(TEMP_CHANEL);

	#ifdef DEBUG
	char buffer[80];
	sprintf(buffer,"(TS) GetTemperature %u",rslt);
	Serial.println(buffer);
	#endif
	
    return rslt;
}
Value XPT2046::getVBat() {

	uint16_t rslt = read(VBAT_CHANEL);

	#ifdef DEBUG
	char buffer[80];
	sprintf(buffer,"(TS) GetVBat %u",rslt);
	Serial.println(buffer);
	#endif
    return rslt;
}
Value XPT2046::getVAux() {
	uint16_t rslt = read(AUX_CHANEL);

	#ifdef DEBUG
	char buffer[80];
	sprintf(buffer,"(TS) GetVAux %u",rslt);
	Serial.println(buffer);
	#endif
    return rslt;
}


bool XPT2046::touched(uint16_t tick) {

    // pooling mode with debounce
    // Sample on high to low transition
    // Debounce delay on low to high
	// tick in milliseconds
	//
	


    switch (state) {
        // wait IRQ low
        case 0:
        if (_IRQ()==0) { 
			state = 1; 
			nextTick = tick + cDelay_low; 
			//
			getPosition();
			//
			#ifdef _DEBUG_
			Serial.println("(TS) IRQ low");
			#endif
			#ifdef DEBUG_LED
			digitalWrite(pinLED, ! digitalRead(pinLED));
			#endif
			} else {
			#ifdef _DEBUG_
			//Serial.println("(TS) IRQ high");
			#endif
			}
        break;

        // wait IRQ low delay
        case 1:
        if (tick > nextTick) {
            // Measure on low
			//
			getPosition();
			//
            state = 2;
			#ifdef _DEBUG_
			char buffer[40];
			sprintf(buffer,"(TS) Touch vx:%u vy:%u x:%u y:%u",vx,vy,x,y);
			Serial.println(buffer);
			#endif
            return 1;
        } else {
            getPosition();
		}	
        break;

        // wait IRQ hight
        case 2:
        if (_IRQ()==1) { 
			#ifdef _DEBUG_
			Serial.println("(TS) IRQ hight");
			#endif
			state = 3; 
			nextTick = tick + cDelay_hight; 
		}
        break;

        // wait IRQ after high delay
        case 3:
        if (tick > nextTick) {
            state = 0;
        }
        break;
    }
    return 0;
}

