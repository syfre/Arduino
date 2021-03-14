
#include <SPI.h>

#define _cs PB1
#define _ct PB0
#define _an PA1
#define _freq 4000000

#define spi_begin() { \
    SPI.beginTransaction(SPISettings(_freq,MSBFIRST,SPI_MODE3,DATA_SIZE_8BIT)); \
    digitalWrite(_cs, LOW); \
}

#define spi_end() { \
  digitalWrite(_cs, HIGH); \
  SPI.endTransaction(); \
}

void ad779x_reset() {

  spi_begin();
  digitalWrite(_cs, LOW);
  SPI.transfer(0xff);
  SPI.transfer(0xff);
  SPI.transfer(0xff);
  SPI.transfer(0xff);
  spi_end();
}

uint8_t ad779x_readReg8bits(uint8_t reg) {

  uint8_t rslt;
  spi_begin();
  SPI.transfer(reg);
  rslt = SPI.transfer(0x00);  
  spi_end();
  return rslt;
}

void ad779x_writeReg16bits(uint8_t reg, uint8_t val1, uint8_t val2) {
  
  spi_begin();
  SPI.transfer(reg);
  SPI.transfer(val1);
  SPI.transfer(val2);
  spi_end();
}

uint16_t ad779x_readReg16bits(uint8_t reg) 
{
  uint16_t rslt = 0;
  spi_begin();
  SPI.transfer(reg);
  rslt = (SPI.transfer(0x00) << 8) & 0xFF00;
  rslt += SPI.transfer(0x00) & 0xFF;
  spi_end();
  return rslt;
}

uint8_t ad779x_readID() {
  return ad779x_readReg8bits(0x60);
}

uint8_t ad779x_readStatus() {
  return ad779x_readReg8bits(0x40);
}
uint16_t ad779x_readMode() {
  return ad779x_readReg16bits(0x48);
}
uint16_t ad779x_readConfig() {
  return ad779x_readReg16bits(0x50);
}

void ad779x_selectMode() {

  // val 1:
  //0x20 for single conversion  preferred
  //0x40 for idle
  //0x60 for power down
  //0x80 internal zero scale calibration
  //0xA0 internal full scale calibration
  //0xC0 system zero scale calibration
  //0xE0 system full scale calibration

  // val2 : update rate
  //0x01  470Hz 4mS
  //0x02  242Hz 8mS
  //0x03  123Hz 16mS
  //0x04   62Hz 32mS 
  //0x05   50Hz 40mS
  //0x06   39Hz 48mS
  //0x07 33.2Hz 60mS 
  //0x08 19.6Hz 101mS -90dB for 60Hz  
  //0x09 16.7Hz 120mS -80dB for 50Hz
  //0x0A 16.7Hz 120mS -65dB for 50/60Hz
  //0x0B 12.5Hz 160mS -66dB for 50/60Hz
  //0x0C   10Hz 200mS -69dB for 50/60Hz
  //0x0D 8.33Hz 240mS -70dB for 50/60Hz  
  //0x0E 6.25Hz 320mS -72dB for 50/60Hz 
  //0x0F 4.17Hz 480mS -74dB for 50/60Hz 

  ad779x_writeReg16bits(0x08, 0x20, 0x0C);
}

void ad779x_selectChannel(uint8_t ch)
{
  if(ch > 2) ch = 2;
  // Internal Ref
  // buffered
  ch |= 0x90;

  // val1 : 
  //bias voltage: disabled 0 0
  //burnout current: disabled 0 
  //unipolar (1) / bipolar (0) coding
  //boost: disabled 1
  //gain: disabled 0 0 0
  //top 8b: 00011000 (0x18), disable boost yields 00010000 (0x10)

  // val2 :
  //0 0
  //buffer 1
  //0
  //000 (CH1) 001(CH2) 010(CH3) 
  //channel select = AIN1(+) - AIN1(-)  
  //bottom 8b: 10010000 (0x90)  10010001 (0x91) read CH2  10010010 (0x92) read CH3
  
  ad779x_writeReg16bits(0x10, 0x10, ch);
}

void ad779x_calibrate(uint8_t ch)
{
  ad779x_selectChannel(ch);
  // zero
  ad779x_writeReg16bits(0x08, 0x80, 0x0F);
  while(ad779x_readStatus() > 127) ;
  
  // full scale
  ad779x_writeReg16bits(0x08, 0xA0, 0x0F);
  while(ad779x_readStatus() > 127) ;
}

// read value from ADC, 
long ad779x_read(uint8_t ch)
{ 
  long value = 0; //uint16_t byte1, byte2, byte3;
  
  ad779x_selectChannel(ch);
  ad779x_selectMode();
  
  while(ad779x_readStatus() > 127) {
      //wait until !RDY
      //Serial.print(".");
  }

  spi_begin();
  
  SPI.transfer(0x58); //select DATA register (01011000)(0x58) //one read  try (01011100)(0x5C) for CREAD
  
  value = (SPI.transfer(0x00) << 16) & 0xFF0000;  
  value += (SPI.transfer(0x00) << 8) & 0xFF00; //grab top byte
  value += SPI.transfer(0x00) & 0xFF; //grab bottom byte  

  spi_end();
  return value;
} 

double ad779x_ToVolt(long value) {
    // 24 bits is 16777215
    // internal reference is 1.17 volt +/- 0.01 %
    // 8 digit precision
    return (double) value * 1.17 / 16777215 ;
}

void setup() {

  Serial.begin(9600);
  pinMode(_ct, OUTPUT);
  digitalWrite(_ct, LOW);
  
  pinMode(_cs, OUTPUT);
  digitalWrite(_cs, HIGH);

  ad779x_reset();
}

void loop() {
  // put your main code here, to run repeatedly:

  char buffer[40];
  uint8_t output = 0;
  long value_0 = 0;
  long value_1 = 0;
  long value_2 = 0;
  int value_a = 0;
  long value_ref = 8388608;

  output = ad779x_readID();

  if (output==0x4b) {

    Serial.println("Calibrate");
    ad779x_calibrate(1);

    Serial.println("Measure");
    
    //value_0 = ad779x_read(0);
    value_1 = ad779x_read(1);
    //value_2 = ad779x_read(2);

    value_a = analogRead(_an);
    
    //sprintf(buffer,"ID:%x V0:%u V1:%u V2:%u VA:%u",output,value_0,value_1,value_2,value_a);
    sprintf(buffer,"ID:%x V1:%u V1:%0.4f VA:%u S:%d",output,value_1, ad779x_ToVolt(value_1), value_a, value_1-value_ref);
    Serial.println(buffer);
    
  } else {
    Serial.println("Reset");
    ad779x_reset();
  }


  delay(100);
  
}
