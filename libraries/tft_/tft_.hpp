#ifndef __TFT__
#define __TFT__

// Interface with the TFT 1.8" SPI LCD Module
// Code adapted from https://github.com/adafruit/Adafruit-TFT-Library

/*
1.8" Serial SPI TFT LCD Module (TFT Controller) with SD Card Socket Pinout:

1. GND
2. VCC
3. NC
4. NC
5. NC
6. LCD RESET
7. LCD A0 (R/S)
8. LCD SDA
9. LCD SCK
10. LCD CS
11. SD SCK 
12. SD MISO 
13. SD MOSI 
14. SD CS 
15. LED+
16. LED-

Alternate PinOut

VCC
GND
CS
RESET
CMD/DATA
SDO
SLK
LED
*/

#include <GenericTypeDefs.h>
#include "proc.h"
#include "port.hpp"
#include "spi.hpp"
#include "font.h"
#include "delay.h"
#include "fileio.h"

#define TFT_ILI9341

// LCD 1"8 screen definition (ST7735)
#define LCD_1P8_SCREEN_WIDTH 128
#define LCD_1P8_SCREEN_HEIGHT 160
#define LCD_1P8_SCREEN_CENTER_X 64
#define LCD_1P8_SCREEN_CENTER_Y 80

// LCD 2"2 screen definition (ILI9341)
#define LCD_2P2_SCREEN_WIDTH 240
#define LCD_2P2_SCREEN_HEIGHT 320
#define LCD_2P2_SCREEN_CENTER_X 120
#define LCD_2P2_SCREEN_CENTER_Y 160

#ifdef TFT_ILI9341
#define TFT_CHIP    IL9391
#define TFT_SCREEN_WIDTH    LCD_2P2_SCREEN_WIDTH
#define TFT_SCREEN_HEIGHT   LCD_2P2_SCREEN_HEIGHT
#endif

#ifdef TFT_ST7735
#define TFT_CHIP    ST7735
#define TFT_SCREEN_WIDTH    LCD_1P8_SCREEN_WIDTH
#define TFT_SCREEN_HEIGHT   LCD_1P8_SCREEN_HEIGHT
#endif

///////////////////////////////////////////////////////////////////////////////

#ifdef TFT_ST7735
// rrrrrggg gggbbbbb
#define RGB(r,g,b) ((((r>>3)<<11) | ((g>>2)<<5) | (b>>3)))
#define COLOR_WHITE		0xFFFF
#define COLOR_BLACK		0x0000
#define COLOR_RED		0xF800
#define COLOR_GREEN		0x07E0
#define COLOR_BLUE		0x001F
#endif

#ifdef TFT_ILI9341
// bbbbbggg gggrrrrr
#define RGB(r,g,b) ((((b>>3)<<11) | ((g>>2)<<5) | (r>>3)))
#define COLOR_WHITE		0xFFFF
#define COLOR_BLACK		0x0000
#define COLOR_RED		0x001F
#define COLOR_GREEN		0x07E0
#define COLOR_BLUE		0xF800
#endif

#ifndef COLOR_WHITE
#error Need TFT Chip type in configuration tft_cfg.h
#endif

///////////////////////////////////////////////////////////////////////////////

typedef UINT16	COORD;
typedef UINT16	COLOR;

///////////////////////////////////////////////////////////////////////////////

#define _IMAGE_STORAGE_  rom far COLOR
#define _IMAGE_PTR_  rom far COLOR *

typedef struct IMAGE_DESC
{
    COORD       width;
    COORD       height;
    _IMAGE_PTR_  data;
} IMAGE_DESC;


typedef struct PIXEL_RLE_DESC
{
    COLOR pcount;
    COLOR pcolor;
} PIXEL_RLE_DESC;


typedef struct NEEDLE_DESC {
    COORD cx;           // center position
    COORD cy;
    COORD len;          // Needle length
    COLOR color;        // Needle color
    UINT8 degree;       // Current degree value
    UINT8 degreeMin;    // degree min (lower position of needle)
    UINT8 degreeMax;    // degree max (upper position of needle)
    COORD *pixels;      // buffer to store needle pixels
} NEEDLE_DESC;

///////////////////////////////////////////////////////////////////////////////

//
//  A-----------B
//  |           |
//  |           |
//  |           |
//  |           |
//  |           |
//  |           |
//  |           |
//  C-----------D
//    ========= (connector)
//

#ifdef TFT_ST7735
// AB=X ; AC=Y (portrait, connector down)
#define TFT_PORTRAIT		0xE0
// DC=X ; DB=Y (reverse portrait, connector up)
#define TFT_PORTRAIT_REVERSE	0x20
// CA=X ; CD=Y (landscape, connector left)
#define TFT_LANDSCAPE	0x40
// BD=X ; BA=Y (landscape, connector right)
#define TFT_LANDSCAPE_REVERSE	0x80
#endif

#ifdef TFT_ILI9341
// AB=X ; AC=Y (portrait, connector down)
#define TFT_PORTRAIT		0x20
// DC=X ; DB=Y (reverse portrait, connector up)
#define TFT_PORTRAIT_REVERSE	0xE0
// CA=X ; CD=Y (landscape, connector left)
#define TFT_LANDSCAPE	0x80
// BD=X ; BA=Y (landscape, connector right)
#define TFT_LANDSCAPE_REVERSE	0x40
#endif

///////////////////////////////////////////////////////////////////////////////

#ifdef TFT_ILI9341
#define TFT_initialize(x,y,m)   ILI9341_initialize(x,y,m)
#endif

#ifdef TFT_ST7735
#define TFT_initialize(x,y,m)   ST7735_initialize(x,y,m)
#endif

class TFT {
private:
	SPI *		spi;
	PIN *		pincs;		// cs
	PIN * 		pindc;		// data/command
	PIN *		pinreset;	// reset
	UINT16		speed;		// spi speed for acquire
	UINT8       madctl;     // device mode give orientation
	
        void writecommand(UINT8 c);
        void write_byte(UINT8 b);
        void write_uint(UINT16 ui);
        void write_color(COLOR c);

        UINT8 readcommand(UINT8 c, UINT8 index);
        void _setAddrWindow(COORD x0, COORD y0, COORD x1, COORD y1);
        void setAddrWindow(COORD x0, COORD y0, COORD x1, COORD y1);

	void _writePixel(COORD x, COORD y, COLOR color);

        void _scrollVertical(void);
        void _newline(void);
        void _incCursor(void);

        void ILI9341_initialize(UINT8 mode);
	void ST7735_initialize(UINT8 mode);
public:
	COORD		HScreenX;   // hardware screen size, never change
	COORD		HScreenY;
	COORD		screenX;    // screen size, orientation dependant
	COORD		screenY;
        // Terminal
        COORD		cursorX;    // cursor position
	COORD		cursorY;
	COLOR		backColor;  // background color
	COLOR		fontColor;  // foreground color
	UINT8           fontSize;   // font size

	TFT(SPI * Spi, PIN *cs, PIN *dc, PIN *reset, UINT16 Speed, COORD screenX, COORD screenY, UINT8 mode);
        void initialize(UINT8 mode);
	void assign_stdout(void);
	void printChars(char *c, int size);
	void startOfLine(void);
        void clear(void);
	void clearEndOfLine(void);
	void printDisplayStatus(void);
        void _putc(char data);

	UINT8 readRegister(UINT8 addr, UINT8 index);
	void dumpFont(COORD x0, COORD y0, COLOR color, COLOR backcolor, unsigned int size);

	void setRotation(UINT8 m);
	UINT8 getRotation(void);

	void writePixel(COORD x, COORD y, COLOR color);
	COLOR readPixel(COORD x, COORD y);
	void writePixels(COORD * buffer);

	void fillScreen(COLOR color);
	void fillRect(COORD x, COORD y, unsigned int w, unsigned int h, COLOR color);
	void flipRectV(COORD x, COORD y, COORD w, COORD h);

	void drawString(COORD x, COORD y, char *c, COLOR color, COLOR backcolor, UINT8 size);
	void drawChar(COORD x, COORD y, UINT8 c, COLOR color, COLOR backcolor, UINT8 size);
	void drawChar2(COORD x, COORD y, UINT8 c, COLOR color, COLOR backcolor, UINT8 size);
	void drawULong(COORD x, COORD y, unsigned long value, COLOR color, COLOR backcolor, UINT8 size);

	void fillCircle(COORD x0, COORD y0, unsigned int r, COLOR color);
	void drawCircle(COORD x0, COORD y0, unsigned int r, COLOR color);

	void drawVerticalLine(COORD x, COORD y, unsigned int length, COLOR color);
	void drawHorizontalLine(COORD x, COORD y, unsigned int length, COLOR color);
	void drawRect(COORD x, COORD y, unsigned int w, unsigned int h, COLOR color);
    void drawCrosshair(COORD x, COORD y, unsigned int w, unsigned int h, COLOR color);
	void drawFastLine(COORD x, COORD y, unsigned int length, COLOR color, UINT8 rotflag);
	COORD drawLine(int x0, int y0, int x1, int y1, COLOR color);
	COORD drawLine(int x0, int y0, int x1, int y1, COLOR color, COORD *buffer);
	void drawNeedle(NEEDLE_DESC *needle);

	void drawImage(COORD x, COORD y, IMAGE_DESC *image);
	void drawImagePart(COORD x, COORD y, COORD ofsx, COORD ofsy, COORD width, COORD height, IMAGE_DESC *image);
        unsigned drawFile(COORD x, COORD y, MFILE *file);

        void drawULongFmt(COORD x, COORD y, rom far char *fmt, unsigned long value, COLOR color, COLOR backcolor, UINT8 size);
        void drawLong(COORD x, COORD y, long value, COLOR color, COLOR backcolor, UINT8 size);

        COORD readLine(int x0, int y0, int x1, int y1, COLOR color, COORD *buffer);
};

#endif