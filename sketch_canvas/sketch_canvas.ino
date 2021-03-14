#include <Canvas9341.h>
#include <DueTimer.h>

// pin definition for the TFT
#define cs   7
#define dc   3
#define rst  6

#define BACK_COLOR  COLOR_BLACK
#define BACK_TEXT   COLOR_RED
#define TEXT_COLOR  COLOR_WHITE
#define cTextSize   3

Canvas9341 tft = Canvas9341(cs,dc,rst);

uint8_t red,blue,green;
long change;

void timerISR() {
  change++;
}
void setupTFT() {

  tft.begin();
  tft.setOrientation(landscape); 
  Serial.print("W:"); Serial.print(tft.Width); Serial.print(" H:"); Serial.println(tft.Height);

  tft.clearScreen(COLOR_BLACK);
  Serial.println("CS1");

  tft.setTextScale(1);
  tft.text(0,0,"Text size 1");
  Serial.println("T1");
  tft.setTextScale(2);
  tft.text(0,20,"Text size 2");
  Serial.println("T2");
  tft.setTextScale(3);
  tft.text(0,60,"Text size 3");
  Serial.println("T3");

  Rect rc;
  tft.clientRect(rc);
  rc.serialPrint();
  rc.inflatePercent(0,-20.0);
  rc.serialPrint();
  tft.fillRectangle(rc,COLOR_RED);
  Serial.println("FR1");
  //
  rc.position(0,0).size(20,50).serialPrint().fill(tft,COLOR_GREEN);
  Serial.println("FR2");
  //
  rc.position(100,100).size(50,20).serialPrint().fill(tft,COLOR_BLUE);
  Serial.println("FR3");

  tft.clientRect(rc);
  rc.right -= 20;
  rc.bottom -= 20;
  rc.left = rc.right-100;
  rc.top = rc.bottom-100;
  rc.serialPrint();
  tft.fillRectangle(rc,COLOR_CYAN);
  Serial.println("FR4");

  tft.clearScreen();

  Rect  top;
  Rect  bottom;
  top.position(0,0).height(30).width(320);
  bottom.position(0,tft.Height-30).height(20).width(320);

  char buffer[4][40];
  sprintf(buffer[0],"Corner1");
  sprintf(buffer[1],"Corner2");
  sprintf(buffer[2],"Corner3");
  sprintf(buffer[3],"Corner4");

  Text text1(leftAlign, buffer[0]);
  text1.position(0,0).width(320);
  
  Text text2(rightAlign, buffer[1]);
  text2.position(0,0).width(320);
  
  Text text3(leftAlign, buffer[2]);
  text3.position(0,bottom.top).width(320);
  
  Text text4(rightAlign, buffer[3]);
  text4.position(0,bottom.top).width(320);

  WidgetList  list;
  list.add(top).add(bottom).add(text1).add(text2).add(text3).add(text4).print("list").paint(tft);
  Serial.println("link chain");
  if (list.head) list.head->print(); else Serial.println("no head");
  if (text1.next) text1.next->print(); else Serial.println("no next");
  if (text2.next) text2.next->print(); else Serial.println("no next");
  if (text3.next) text3.next->print(); else Serial.println("no next");

}


void setup() {
  Serial.begin(9600); //This pipes to the serial monitor
  setupTFT();

  red=0; blue=0; green=0;
  change = 0;
  pinMode(LED_BUILTIN, OUTPUT);
  Timer3.attachInterrupt(timerISR).setFrequency(1).start();
}

void loop() {
  // put your main code here, to run repeatedly:
    if (change) {
      digitalWrite(LED_BUILTIN, ! digitalRead(LED_BUILTIN));
     
    change--;
  }
}
