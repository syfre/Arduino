
#define pinLED  LED_BUILTIN
/*
#define pinCS   40
#define pinRST  41
#define pinWR   39
#define pinRS   38
#define pinRD   45

#define pinDB0    37
#define pinDB1    36
#define pinDB2    35
#define pinDB3    34
#define pinDB4    33
#define pinDB5    32
#define pinDB6    31
#define pinDB7    30
#define pinDB8    22
#define pinDB9    23
#define pinDB10   24
#define pinDB11   25
#define pinDB12   26
#define pinDB13   27
#define pinDB14   28
#define pinDB15   29
*/
#define pinCS   27
#define pinRST  28
#define pinWR   26
#define pinRS   25
#define pinRD   

#define pinDB0    33
#define pinDB1    34
#define pinDB2    35
#define pinDB3    36
#define pinDB4    37
#define pinDB5    38
#define pinDB6    39
#define pinDB7    40
#define pinDB8    51
#define pinDB9    50
#define pinDB10   49
#define pinDB11   48
#define pinDB12   47
#define pinDB13   46
#define pinDB14   45
#define pinDB15   44

#define pin pinDB1

void setup() {
  // put your setup code here, to run once:
  pinMode(pinLED, OUTPUT);
  pinMode(pin, OUTPUT); 
  digitalWrite(pin,LOW);
   
}

void loop() {
  // put your main code here, to run repeatedly:

  digitalWrite(pinLED, ! digitalRead(pinLED));

  digitalWrite(pin,HIGH);
  delay(10);
  digitalWrite(pin,LOW);
  delay(10);
}
