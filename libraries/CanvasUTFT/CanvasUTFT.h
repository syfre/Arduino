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
#ifndef CanvasUTFT_h
#define CanvasUTFT_h

#include "Canvas.h"
#include "UTFT.h"

#define COLOR_BLACK			VGA_BLACK
#define COLOR_NAVY			VGA_NAVY
#define COLOR_DARKGREEN		VGA_DARKGREEN
#define COLOR_DARKCYAN		VGA_DARKCYAN
#define COLOR_MAROON		VGA_MAROON
#define COLOR_PURPLE		VGA_PURPLE
#define COLOR_OLIVE			VGA_OLIVE
#define COLOR_LIGHTGREY		VGA_LIGHTGREY
#define COLOR_DARKGREY		VGA_DARKGREY
#define COLOR_BLUE			VGA_BLUE
#define COLOR_GREEN			VGA_GREEN
#define COLOR_CYAN			VGA_CYAN
#define COLOR_RED			VGA_RED
#define COLOR_MAGENTA		VGA_MAGENTA
#define COLOR_YELLOW		VGA_YELLOW
#define COLOR_WHITE			VGA_WHITE
#define COLOR_ORANGE		VGA_ORANGE
#define COLOR_GREENYELLOW	VGA_GREENYELLOW
#define COLOR_PINK			VGA_PINK


class CanvasUTFT: public Canvas {
protected:
	UTFT	utft;	
	void init(void);
	COORD _getDisplayWidth(void);
	COORD _getDisplayHeight(void);
	void _setPixel(COORD x, COORD y, COLOR color);
	void _fillPixels(COORD x, COORD y, uint16_t w, uint16_t h, COLOR color);
	void _drawHLine(COORD x, COORD y, uint16_t w, COLOR color);
	void _drawVLine(COORD x, COORD y, uint16_t h, COLOR color);
public:
	CanvasUTFT(byte model, int RS, int WR, int CS, int RST, int SER=0) :utft(model,RS,WR,CS,RST,SER) { init(); };
	COLOR rgb(uint8_t r, uint8_t g, uint8_t b);
	void begin(void);
	void setOrientation(Orientation value); 
};

#endif
