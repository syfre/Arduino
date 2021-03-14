#include "Canvas9341_due.h"

#if defined (__arm__) && defined (__SAM3X8E__) // Arduino Due compatible

#endif

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
void Canvas9341::init(void) {

	// landscape
	clientRect.position(0,0).width(ILI9341_TFTHEIGHT).height(ILI9341_TFTWIDTH);
	textColor = COLOR_WHITE;
	backColor = COLOR_BLACK;
}

void Canvas9341::setOrientation(Orientation value) {

	switch(value) {
		case portrait:
			clientRect.position(0,0).width(ILI9341_TFTWIDTH).height(ILI9341_TFTHEIGHT);
			_ILI9341::setRotation(iliRotation0); 
		break;
		case landscape:
			clientRect.position(0,0).width(ILI9341_TFTHEIGHT).height(ILI9341_TFTWIDTH);
			_ILI9341::setRotation(iliRotation90); 
		break;
	}
	_ILI9341::setFont(SystemFont5x7);
}

COLOR Canvas9341::rgb(uint8_t r,uint8_t g, uint8_t b) {
	return color565(r,g,b);
}

COORD Canvas9341::_getDisplayWidth(void) {
	return clientRect.width();
}

COORD Canvas9341::_getDisplayHeight(void) {
	return clientRect.height();
}

void Canvas9341::_fillPixels(COORD x, COORD y, uint16_t w, uint16_t h, COLOR color) {
	_ILI9341::fillRect(x,y,w,h,color);
}

void Canvas9341::_setPixel(COORD x, COORD y, COLOR color) {
	drawPixel(x,y,color);
}

void Canvas9341::_drawHLine(COORD x, COORD y, uint16_t w, COLOR color) {
	_ILI9341::drawFastHLine(x,y,w,color);
}

void Canvas9341::_drawVLine(COORD x, COORD y, uint16_t h, COLOR color) {
	_ILI9341::drawFastVLine(x,y,h,color);
}
