/*
	Canvas implementation for iLI9341

	Copyright (C) 2017 sylvain Frere
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef Canvas9341_h
#define Canvas9341_h

#include "../core/Canvas.h"
#include "ili9341.h"
#include "tftio.h"

#define COLOR_BLACK			ILI9341_BLACK
#define COLOR_NAVY			ILI9341_NAVY
#define COLOR_DARKGREEN		ILI9341_DARKGREEN
#define COLOR_DARKCYAN		ILI9341_DARKCYAN
#define COLOR_MAROON		ILI9341_MAROON
#define COLOR_PURPLE		ILI9341_PURPLE
#define COLOR_OLIVE			ILI9341_OLIVE
#define COLOR_LIGHTGREY		ILI9341_LIGHTGREY
#define COLOR_DARKGREY		ILI9341_DARKGREY
#define COLOR_BLUE			ILI9341_BLUE
#define COLOR_GREEN			ILI9341_GREEN
#define COLOR_CYAN			ILI9341_CYAN
#define COLOR_RED			ILI9341_RED
#define COLOR_MAGENTA		ILI9341_MAGENTA
#define COLOR_YELLOW		ILI9341_YELLOW
#define COLOR_WHITE			ILI9341_WHITE
#define COLOR_ORANGE		ILI9341_ORANGE
#define COLOR_GREENYELLOW	ILI9341_GREENYELLOW
#define COLOR_PINK			ILI9341_PINK

////////////////////////////////////
// 	Canvas class for ILI9341
////////////////////////////////////

//	Landscape
//
//	+0-------------------------320| |
//	0  --> X                      | |
//	| |                           | |
//	| V                           | |
//	| Y                           | |
//	|                             | |
//	|                             | |
//	240---------------------------| |
//	


class Canvas9341_: public Canvas {
protected:
    volatile uint32 *dcport, *rsport, *csport;
    uint32_t  _cs, _dc, _rst;
    uint32_t  cspinmask, dcpinmask;
	COLOR	lineBuffer[ILI9341_TFTHEIGHT]; // DMA buffer. 16bit color data per pixel
	
	TFTIO& tftio;
	
	COORD	_width,_height;
	void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
	void setRotation(uint8_t m);

	void init(void);
	COORD _getDisplayWidth(void);
	COORD _getDisplayHeight(void);
	void _setPixel(COORD x, COORD y, COLOR color);
	void _fillPixels(COORD x, COORD y, uint16_t w, uint16_t h, COLOR color);
	void _drawHLine(COORD x, COORD y, uint16_t w, COLOR color);
	void _drawVLine(COORD x, COORD y, uint16_t h, COLOR color);
public:

	Canvas9341_(TFTIO& _tftio, int8_t _CS, int8_t _DC, int8_t _RST = -1) :tftio(_tftio),_cs(_CS),_dc(_DC),_rst(_RST) { init(); } ;
	COLOR rgb(uint8_t r, uint8_t g, uint8_t b);
	void begin(void);
	void setOrientation(Orientation value); 
	void setFontColor(COLOR color) { textColor = color; };
	void endOfTransfert(void);
};

#endif
