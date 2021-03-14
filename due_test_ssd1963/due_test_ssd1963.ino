
//
// RS (DC)  38
// WR       39
// RD       45 => F_CS
// CS       40
// RST      41


//UTFT(byte model, int RS, int WR, int CS, int RST, int SER=0);
//UTFT myGLCD(ITDB50,38,39,40,41);

#define pinCS   40
#define pinRST  41
#define pinWR   39
#define pinRS   38
#define pinRD   45

/*
 * Data bus
 * 
 DB0    D37   PC5
 DB1    D36   PC4
 DB2    D35   PC3
 DB3    D34   PC2
 DB4    D33   PC1
 DB5    D32   PD10
 DB6    D31   PA7
 DB7    D30   PD9
 
 DB8     D22   PB26
 DB9     D23   PA14
 DB10    D24   PA15
 DB11    D25   PD0
 DB12    D26   PC1
 DB13    D27   PC2
 DB14    D28   PC3
 DB15    D29   PC6
 */


void LCD_read_bus(uint16_t& data) {

    // disable output
    REG_PIOA_ODR=0x0000c080; //PA14,PA15,PA7 enable
    REG_PIOB_ODR=0x04000000; //PB26 enable
    REG_PIOD_ODR=0x0000064f; //PD0-3,PD6,PD9-10 enable
    REG_PIOC_ODR=0x0000003e; //PC1 - PC5 enable

    digitalWrite(pinCS,LOW);
    digitalWrite(pinRD,LOW);
    delay(1);

    long A = REG_PIOA_PDSR;
    long B = REG_PIOB_PDSR;
    long C = REG_PIOC_PDSR;
    long D = REG_PIOD_PDSR;

    data = 0;
    data += (C & (1<<5)) << 0; 
    data += (C & (1<<4)) << 1; 
    data += (C & (1<<3)) << 2; 
    data += (C & (1<<2)) << 3; 
    data += (C & (1<<1)) << 4; 
    data += (D & (1<<10)) << 5; 
    data += (A & (1<<7)) << 6; 
    data += (D & (1<<9)) << 7; 

    data += (B & (1<<26)) << 8; 
    data += (A & (1<<14)) << 9; 
    data += (A & (1<<15)) << 10; 
    data += (D & (1<<0)) << 11; 
    data += (C & (1<<1)) << 12; 
    data += (C & (1<<2)) << 13; 
    data += (C & (1<<3)) << 14; 
    data += (C & (1<<6)) << 15; 
    
    digitalWrite(pinRD,HIGH);
    digitalWrite(pinCS,HIGH);

  char buffer[40];
  sprintf(buffer,"A:%x B:%x C:%x D:%x",A,B,C,D);
  Serial.println(buffer);    
}

void LCD_Write_Bus(char VH,char VL)
{   
    // enable output
    REG_PIOA_OER=0x0000c080; //PA14,PA15,PA7 enable
    REG_PIOB_OER=0x04000000; //PB26 enable
    REG_PIOD_OER=0x0000064f; //PD0-3,PD6,PD9-10 enable
    REG_PIOC_OER=0x0000003e; //PC1 - PC5 enable
    
    REG_PIOA_CODR=0x0000C080;
    REG_PIOC_CODR=0x0000003E;
    REG_PIOD_CODR=0x0000064F;
    REG_PIOA_SODR=((VH & 0x06)<<13) | ((VL & 0x40)<<1);
    (VH & 0x01) ? REG_PIOB_SODR = 0x4000000 : REG_PIOB_CODR = 0x4000000;
    REG_PIOC_SODR=((VL & 0x01)<<5) | ((VL & 0x02)<<3) | ((VL & 0x04)<<1) | ((VL & 0x08)>>1) | ((VL & 0x10)>>3);
    REG_PIOD_SODR=((VH & 0x78)>>3) | ((VH & 0x80)>>1) | ((VL & 0x20)<<5) | ((VL & 0x80)<<2);

    digitalWrite(pinCS,LOW);
    digitalWrite(pinWR,LOW);
    delay(1);
    digitalWrite(pinWR,HIGH);
    digitalWrite(pinCS,HIGH);
}

void LCD_Write_CMD(char cmd) {
    digitalWrite(pinRS,LOW);
    LCD_Write_Bus(0,cmd);
    digitalWrite(pinRS,HIGH);
}

void LCD_Write_DATA(char data) {
    LCD_Write_Bus(data>>8,data);
}

void setup() {
  Serial.begin(9600);

  // IO 
  REG_PIOA_PER=0x0000c080; //PA14,PA15,PA7 enable
  REG_PIOB_PER=0x04000000; //PB26 enable
  REG_PIOD_PER=0x0000064f; //PD0-3,PD6,PD9-10 enable
  REG_PIOC_PER=0x0000003e; //PC1 - PC5 enable
 
  pinMode(pinCS, OUTPUT); 
  pinMode(pinRD, OUTPUT); 
  pinMode(pinWR, OUTPUT); 
  pinMode(pinRST, OUTPUT); 
  pinMode(pinRS, OUTPUT); 

  digitalWrite(pinCS,HIGH);
  digitalWrite(pinRST,HIGH);
  digitalWrite(pinWR,HIGH);
  digitalWrite(pinRD,HIGH);
  digitalWrite(pinRS,HIGH);

  digitalWrite(pinRST,LOW);  
  delay(200);
  digitalWrite(pinRST,HIGH);  
}

uint16_t data1,data2,data3,data4,data5;

void loop() {
  // put your main code here, to run repeatedly:
  /*
  LCD_Write_CMD(0xA1);
  LCD_read_bus(data1);
  LCD_read_bus(data2);
  LCD_read_bus(data3);
  LCD_read_bus(data4);
  LCD_read_bus(data5);

  char buffer[40];
  sprintf(buffer,"%x %x %x %x %x",data1,data2,data3,data4,data5);
  Serial.println(buffer);
  delay(1000);
  */
  for (int i=0; i<1000; i++) {
    LCD_Write_Bus(0x55,0x55);
    LCD_Write_Bus(0xAA,0xAA);
  }
  delay(100);
  

}
