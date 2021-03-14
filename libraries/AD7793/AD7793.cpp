#include "AD7793.h"

#define DEBUG

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define REG_READ	0x40
#define REG_STATUS	0x00
#define REG_MODE	0x08
#define REG_CONFIG	0x10
#define REG_DATA	0x18
#define REG_ID		0x20
#define REG_IO		0x28
#define REG_OFFSET	0x30
#define REG_FULLSCALE 0x38

/////////////////////////
// Configuration register
/////////////////////////
//val1 : bias and gain
//////////////////////
// CON15..CON14 : bias voltage (AVD/2)
// 0x00	disable
// 0x04	bias voltage connected to AN1-
// 0x80	bias voltage connected to AN2-
// 0xC0	reserved
// CON13 : burnout current 1:enable 0:disable
// CON12 : output coding 1:unipolar 0:bipolar
// CON11 : bias voltage boost : 1:enable 0:disable
// CON10..CON8 : gain selection
// 0x00	1		2.5 v
// 0x01	2		1.25
// 0x02	4		0.6225
// 0x03	8		0.3125
// 0x04	16		0.1562
// 0x05	32		0.078125
// 0x06	64		0.03906
// 0x07	128		0.01953

#define BIAS_MASK		0xC0
#define BIAS_BITS		6
#define BURNOUT_ENABLE	0x20
#define CODING_UNIPOLAR	0x10
#define BIAS_BOOST		0x08
#define GAIN_MASK		0x07
#define GAIN_BITS		0

// val2
///////
// CON7 : Reference selection 1:Internal 0:External
// CON4 : buffer configuration 1:enable 0:disable
// CON2..CON0 : Channel selection
// 0x00	AN1
// 0x01	AN2
// 0x02	AN3
// 0x03	AN1- - AN1-
// 0x04	reserved
// 0x05	reserved
// 0x06	Temp sensor
// 0x07	AVdd monitor

#define REF_INTERNAL		0x80
#define CFG_BUFFERED		0x01

#define CHANNEL_MASK	0x07
#define CHANNEL_BITS	0
#define CHANNEL_0		0x00
#define CHANNEL_1		0x01
#define CHANNEL_2		0x02
#define CHANNEL_TEMP	0x06
#define CHANNEL_AVDD	0x07

//////////////////
// Mode register
//////////////////
// val 1: conversion mode
//
//MDx 15..13
// 0x00 continuous conversion  
// 0x20 single conversion  
// 0x40 idle
// 0x60 power down
// 0x80 internal zero scale calibration
// 0xA0 internal full scale calibration
// 0xC0 system zero scale calibration
// 0xE0 system full scale calibration

#define	MODE_CONTINUOUS	0x00
#define MODE_SINGLE		0x20
#define	MODE_IDLE		0x40
#define MODE_POWERDOWN	0x60
#define MODE_CAL_ZERO	0x80
#define MODE_CAL_FULLSCALE 0xA0

// val2 : clock, updaterate
//MR7..MR6 : clock
// 0x00	Internal clock 64Khz
// 0x40	Internal clock 64Khz available on CLK
// 0x80	External clock 64Khz
// 0xC0	External clock divided by 2

#define CLOCK_INTERNAL	0x00

//MR3..MR0 : update rate
// 0x01  470Hz 4mS
// 0x02  242Hz 8mS
// 0x03  123Hz 16mS
// 0x04   62Hz 32mS 
// 0x05   50Hz 40mS
// 0x06   39Hz 48mS
// 0x07 33.2Hz 60mS 
// 0x08 19.6Hz 101mS -90dB for 60Hz  
// 0x09 16.7Hz 120mS -80dB for 50Hz
// 0x0A 16.7Hz 120mS -65dB for 50/60Hz
// 0x0B 12.5Hz 160mS -66dB for 50/60Hz
// 0x0C   10Hz 200mS -69dB for 50/60Hz
// 0x0D 8.33Hz 240mS -70dB for 50/60Hz  
// 0x0E 6.25Hz 320mS -72dB for 50/60Hz 
// 0x0F 4.17Hz 480mS -74dB for 50/60Hz Optimum noise performance

#define UPDATE_4_17HZ	0x0F

#define	DEFAULT_CONF_1	CODING_UNIPOLAR
#define	DEFAULT_CONF_2	REF_INTERNAL+CFG_BUFFERED
#define DEFAULT_MODE_1	MODE_SINGLE
#define DEFAULT_MODE_2	CLOCK_INTERNAL + UPDATE_4_17HZ
 
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#define _freq 4000000

#define spi_begin() { \
    SPI.beginTransaction(SPISettings(_freq,MSBFIRST,SPI_MODE3,DATA_SIZE_8BIT)); \
    digitalWrite(pinCS, LOW); \
}

#define spi_end() { \
  digitalWrite(pinCS, HIGH); \
  SPI.endTransaction(); \
}

void AD7793::begin() {
	pinMode(pinCS, OUTPUT);
	digitalWrite(pinCS, HIGH);
}

void AD7793::initialize(uint8_t ch) {
	reset();
	setChannel(ch);
}

void AD7793::reset() {

  spi_begin();
  SPI.transfer(0xff);
  SPI.transfer(0xff);
  SPI.transfer(0xff);
  SPI.transfer(0xff);
  spi_end();
}

uint8_t AD7793::readReg8bits(uint8_t reg) {

  uint8_t rslt;
  spi_begin();
  SPI.transfer(reg | REG_READ);
  rslt = SPI.transfer(0x00);  
  spi_end();
  return rslt;
}

void AD7793::writeReg16bits(uint8_t reg, uint8_t val1, uint8_t val2) {
  
	#ifdef DEBUG
	char buffer[40];
	sprintf(buffer,"Write, R:%x V1:%x V2:%x",reg,val1,val2);
	Serial.println(buffer);
	#endif
	
	spi_begin();
	SPI.transfer(reg);
	SPI.transfer(val1);
	SPI.transfer(val2);
	spi_end();
}

void AD7793::readReg16bits(uint8_t reg, uint8_t& val1, uint8_t& val2) 
{
	spi_begin();
	SPI.transfer(reg | REG_READ);
	val1 = SPI.transfer(0x00);
	val2 = SPI.transfer(0x00);
	spi_end();
	#ifdef DEBUG
	char buffer[40];
	sprintf(buffer,"Read, R:%x V1:%x V2:%x",reg,val1,val2);
	Serial.println(buffer);
	#endif
	return;
}

uint16_t AD7793::readReg16bits(uint8_t reg) 
{
  uint16_t rslt = 0;
  spi_begin();
  SPI.transfer(reg | REG_READ);
  rslt = (SPI.transfer(0x00) << 8) & 0xFF00;
  rslt += SPI.transfer(0x00) & 0xFF;
  spi_end();
  return rslt;
}

uint8_t AD7793::readID() {
  return readReg8bits(REG_ID);
}

uint8_t AD7793::readStatus() {
  return readReg8bits(REG_STATUS);
}
uint16_t AD7793::readMode() {
  return readReg16bits(REG_MODE);
}
uint16_t AD7793::readConfig() {
  return readReg16bits(REG_CONFIG);
}

void AD7793::setMode() {
	#ifdef DEBUG
	Serial.println("setMode");
	#endif
	writeReg16bits(REG_MODE, DEFAULT_MODE_1, DEFAULT_MODE_2);
}

void AD7793::selectGain(uint8_t gain) {

	#ifdef DEBUG
	Serial.print("selectGain "); Serial.println(gain);
	#endif
	uint8_t val1,val2;
	readReg16bits(REG_CONFIG, val1, val2);
	val1 &= ~GAIN_MASK;
	val1 |= (gain << GAIN_BITS); 
	writeReg16bits(REG_CONFIG, val1, val2);
}

void AD7793::selectBias(ad7793Bias bias) {

	#ifdef DEBUG
	Serial.print("selectBias "); Serial.println(bias);
	#endif
	uint8_t val1,val2;
	readReg16bits(REG_CONFIG, val1, val2);
	val1 &= ~BIAS_MASK;
	val1 |= (bias << BIAS_BITS); 
	writeReg16bits(REG_CONFIG, val1, val2);
}

void AD7793::setChannel(uint8_t ch)
{
	#ifdef DEBUG
	Serial.print("setChannel "); Serial.println(ch);
	#endif
	ch &= CHANNEL_MASK;
	ch |= DEFAULT_CONF_2;
	writeReg16bits(REG_CONFIG, DEFAULT_CONF_1, ch);
}

void AD7793::selectChannel(uint8_t ch)
{
	#ifdef DEBUG
	Serial.print("selectChannel "); Serial.println(ch);
	#endif
	ch &= CHANNEL_MASK;
	uint8_t val1,val2;
	readReg16bits(REG_CONFIG, val1, val2);
	val2 &= ~CHANNEL_MASK;
	val2 |= (ch << CHANNEL_BITS); 
	writeReg16bits(REG_CONFIG, val1, val2);
}

void AD7793::calibrate(uint8_t ch)
{
  selectBias(bias_none);
  selectChannel(ch);
  // zero
  writeReg16bits(REG_MODE, MODE_CAL_ZERO, DEFAULT_MODE_2);
  while(readStatus() > 127) ;
  
  // full scale
  writeReg16bits(REG_MODE, MODE_CAL_FULLSCALE, DEFAULT_MODE_2);
  while(readStatus() > 127) ;
}

// read value from ADC, 
long AD7793::read(uint8_t ch)
{ 
  long value = 0; //uint16_t byte1, byte2, byte3;
  
  selectChannel(ch);
  setMode();
  
  while(readStatus() > 127) {
      //wait until !RDY
      //Serial.print(".");
  }

  spi_begin();
  
  SPI.transfer(REG_DATA|REG_READ); //select DATA register (01011000)(0x58) //one read  try (01011100)(0x5C) for CREAD
  
  value = (SPI.transfer(0x00) << 16) & 0xFF0000;  
  value += (SPI.transfer(0x00) << 8) & 0xFF00; //grab top byte
  value += SPI.transfer(0x00) & 0xFF; //grab bottom byte  

  spi_end();
  return value;
} 

double AD7793::ToVolt(long value) {
    // 24 bits is 16777215
    // internal reference is 1.17 volt +/- 0.01 %
    // 8 digit precision
    return (double) value * 1.17 / 16777215 ;
}
