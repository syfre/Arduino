#include <HardwareTimer.h>
#include <DCC_Decoder.h>
#include "Canvas.h"

#define __SERIAL_

#define pinLED  PC13
#define pinOUT  PB0

#define freqKhz         50
#define periodus        1000 / freqKhz
#define ticksBySecond   1000 * freqKhz
#define ticksByMilliSeconds   freqKhz

// pin definition for the TFT
// SPI1 MISI  PA7 
// SPI1 CLK   PA5
// C/D        PA4
// RST        PA3
// CS         PA2

#define tft_cs   PA2
#define tft_dc   PA4
#define tft_rst  PA3

#define touch_cs  PB11
#define touch_irq PB10

#define BACK_COLOR  COLOR_BLACK
#define BACK_TEXT   COLOR_RED
#define TEXT_COLOR  COLOR_WHITE
#define cTextSize   3


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define kDCC_INTERRUPT PB9

typedef struct
{
    int count;
    byte validBytes;
    byte data[6];
} DCCPacket;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// The dcc decoder object and global data
//
int gPacketCount = 0;
int gIdlePacketCount = 0;
int gLongestPreamble = 0;

DCCPacket gPackets[25];

static unsigned long lastMillis = millis();
    
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Packet handlers
//

// ALL packets are sent to the RawPacket handler. Returning true indicates that packet was handled. DCC library starts watching for 
// next preamble. Returning false and library continue parsing packet and finds another handler to call.
boolean RawPacket_Handler(byte byteCount, byte* packetBytes)
{
        // Bump global packet count
    ++gPacketCount;
    
    int thisPreamble = DCC.LastPreambleBitCount();
    if( thisPreamble > gLongestPreamble )
    {
        gLongestPreamble = thisPreamble;
    }
    
        // Walk table and look for a matching packet
    for( int i=0; i<(int)(sizeof(gPackets)/sizeof(gPackets[0])); ++i )
    {
        if( gPackets[i].validBytes )
        {
                // Not an empty slot. Does this slot match this packet? If so, bump count.
            if( gPackets[i].validBytes==byteCount )
            {
                char isPacket = true;
                for( int j=0; j<byteCount; j++)
                {
                    if( gPackets[i].data[j] != packetBytes[j] )
                    {
                        isPacket = false;
                        break;
                    } 
                }
                if( isPacket )
                {
                   gPackets[i].count++;
                   return false;
                }
            }
        }else{
                // Empty slot, just copy over data
            gPackets[i].count++;
            gPackets[i].validBytes = byteCount;
            for( int j=0; j<byteCount; j++)
            {
                gPackets[i].data[j] = packetBytes[j];
            }
            return false;
        }
    }    
    
    return false;
}

// Idle packets are sent here (unless handled in rawpacket handler). 
void IdlePacket_Handler(byte byteCount, byte* packetBytes)
{
    ++gIdlePacketCount;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Canvas9341 tft = Canvas9341(tft_cs,tft_dc,tft_rst);
Touch_xpt2046 touch = Touch_xpt2046(touch_cs,touch_irq,0,TOUCH_LANDSCAPE);

COLOR color;

volatile long milliSeconds;
volatile long ticks;
volatile long tickms; 
void TimerIRQ(void) {
  //digitalWrite(pinOUT, ! digitalRead(pinOUT));
  ticks++;
  tickms++;
}

void setupTFT() {

  tft.begin();
  tft.setOrientation(landscape_reverse); 
  tft.setFontSize(2);
  
  #ifdef _SERIAL_
  Serial.print("W:"); Serial.print(tft.clientRect.width()); Serial.print(" H:"); Serial.println(tft.clientRect.height());
  #endif
}

void setup() {
  #ifdef _SERIAL_
  Serial.begin(115200);
  #endif
  pinMode(kDCC_INTERRUPT, INPUT);
  pinMode(pinLED, OUTPUT);
  pinMode(pinOUT, OUTPUT);
  digitalWrite(pinLED, 1);
  #ifdef _SERIAL_
  Serial.println("START");  
  #endif

  ticks = 0;
  tickms = 0;
  milliSeconds = 0;
  Timer3.setPeriod(periodus);
  Timer3.attachInterrupt(3, TimerIRQ);

  touch.begin();

  setupTFT();
  clearScreen();

   DCC.SetRawPacketHandler(RawPacket_Handler);   
   DCC.SetIdlePacketHandler(IdlePacket_Handler);
            
   DCC.SetupMonitor( kDCC_INTERRUPT );   
}

#define FONT_HEIGHT   16

void clearScreen() {
  Rect rc;
  rc = tft.clientRect;
  tft.fillRectangle(rc,COLOR_BLACK);
}

void outputLine(Rect *rc, char * buffer) {

    rc->bottom = rc->top+tft.textHeight(buffer);
    tft.fillRectangle(*rc,COLOR_BLACK);
    tft.text(rc->left,rc->top,buffer,COLOR_GREEN);
    rc->top = rc->bottom;
}

void DumpAndResetTable()
{
    char buffer[70];
    Rect rc1;
    rc1 = tft.clientRect;
    
    sprintf(buffer,"Total Packet Count: %u",gPacketCount);
    outputLine(&rc1,buffer);
    
    sprintf(buffer,"Idle Packet Count:  %u",gIdlePacketCount);
    outputLine(&rc1,buffer);
        
    sprintf(buffer,"Longest Preamble:  %u",gLongestPreamble);
    outputLine(&rc1,buffer);
   
    for( int i=0; i<(int)(sizeof(gPackets)/sizeof(gPackets[0])); ++i )
    {
        if( gPackets[i].validBytes > 0 )
        {
            sprintf(buffer,"Count:%u Bytes:%u",gPackets[i].count,gPackets[i].validBytes);
            outputLine(&rc1,buffer);
            DCC.MakePacketString(buffer, gPackets[i].validBytes, &gPackets[i].data[0]);
            outputLine(&rc1,buffer);
        }
        gPackets[i].validBytes = 0;
        gPackets[i].count = 0;
    }
    
    gPacketCount = 0;
    gIdlePacketCount = 0;
    gLongestPreamble = 0;
    
    rc1.bottom = tft.height();
    tft.fillRectangle(rc1,COLOR_BLACK);
}

void loop() {

  DCC.loop();
  
  if( millis()-lastMillis > 2000 )
  {
      DumpAndResetTable();
      lastMillis = millis();
  }


  if (tickms>=ticksByMilliSeconds) {
    
    milliSeconds++;
    if (touch.touched(milliSeconds)) { 
      digitalWrite(pinLED, ! digitalRead(pinLED));  
    }
    
    tickms = 0;
  }


  if (ticks==ticksBySecond) {
    //digitalWrite(pinLED, ! digitalRead(pinLED));
    ticks = 0;
  }

}
