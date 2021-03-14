#include "CanvasUTFT.h"

//#define DEBUG

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
void CanvasUTFT::init(void) {

	// landscape
	textColor = COLOR_WHITE;
	backColor = COLOR_BLACK;
	textSize = 3;
}

void CanvasUTFT::begin(void) {

	#ifdef DEBUG
	char buffer[80];
	sprintf(buffer,"(CV) begin");
	Serial.println(buffer);
	#endif
	setOrientation(landscape);
 }

void CanvasUTFT::setOrientation(Orientation value) {

	switch(value) {
		case portrait:
			utft.InitLCD(PORTRAIT);
		break;
		case landscape:
			utft.InitLCD(LANDSCAPE);
		break;
	}
	clientRect.position(0,0).width(_getDisplayWidth()).height(_getDisplayHeight());
}

COORD CanvasUTFT::_getDisplayWidth(void) {
	return utft.getDisplayXSize() -1;
}

COORD CanvasUTFT::_getDisplayHeight(void) {
	return utft.getDisplayYSize() -1;
}

COLOR CanvasUTFT::rgb(uint8_t r,uint8_t g, uint8_t b) {
	utft.setColor(r,g,b);
	return utft.getColor();
}

void CanvasUTFT::_setPixel(COORD x, COORD y, COLOR color) {

	utft.setColor(color);
	utft.drawPixel(x,y);
}

void CanvasUTFT::_fillPixels(COORD x, COORD y, uint16_t w, uint16_t h, COLOR color) {

	utft.setColor(color);
	utft.fillRect(x,y,x+w,y+h);
}

void CanvasUTFT::_drawHLine(COORD x, COORD y, uint16_t w, COLOR color) {
  
	utft.setColor(color);
	utft.drawHLine(x,y,w);
}

void CanvasUTFT::_drawVLine(COORD x, COORD y, uint16_t h, COLOR color) {

	utft.setColor(color);
	utft.drawVLine(x,y,h);
}
