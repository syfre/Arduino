
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
  ad7793.initialize();
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

  output = ad7793.readID();

  if (output==0x4b) {

    Serial.println("Calibrate");
    ad7793.calibrate(1);

    Serial.println("Measure");
    
    //value_0 = ad7793.read(0);
    value_1 = ad7793.read(1);
    //value_2 = ad7793.read(2);

    value_a = analogRead(_an);
    
    //sprintf(buffer,"ID:%x V0:%u V1:%u V2:%u VA:%u",output,value_0,value_1,value_2,value_a);
    sprintf(buffer,"ID:%x V1:%u V1:%0.4f VA:%u S:%d",output,value_1, ad7793.ToVolt(value_1), value_a, value_1-value_ref);
    Serial.println(buffer);
    
  } else {
    Serial.println("Reset");
    ad7793.reset();
  }


  delay(100);
  
}
