
#include <ad7793.h>

#define _cs PB1
#define _ct PB0
#define _an PA1
#define _freq 4000000

AD7793 ad7793 = AD7793(_cs);

void setup() {

  Serial.begin(9600);

  pinMode(_ct, OUTPUT);
  digitalWrite(_ct, LOW);
  
  ad7793.begin();
}

void loop() {
  // put your main code here, to run repeatedly:

  char buffer[40];
  uint8_t output = 0;
  long value = 0;
  long value_ref = 8388608;

  output = ad7793.readID();

  if (output==0x4b) {

    Serial.println("Calibrate");
    ad7793.calibrate(CHANNEL_AN2);
    //ad7793.selectBias(bias_an2);

    Serial.println("Measure");
   
    value = ad7793.read(CHANNEL_AN2);

    sprintf(buffer,"ID:%x V1:%u V1:%0.4f S:%d",output,value, ad7793.ToVolt(value), value-value_ref);
    Serial.println(buffer);
    
  } else {
    Serial.println("Reset");
    ad7793.reset();
  }


  delay(100);
  
}
