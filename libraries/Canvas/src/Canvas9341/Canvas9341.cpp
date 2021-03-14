#include "Canvas9341.h"

//#define DEBUG
//#define DEBUG_CHAR
//#define DEBUG_STR
//#define USE_CALLBACK

////////////////////////////////////////////////////////////////////////
#define ILI9341_TFTWIDTH  240
#define ILI9341_TFTHEIGHT 320

////////////////////////////////////////////////////////////////////////

#define spi_init() { tftio.init(); }
#define	spi_begin() { tftio.begin(); }
#define spi_end()	{ tftio.end(); }
#define spi_write(u)  { tftio.write(u); }
#define spi_write_uint(u)  { tftio.write_uint(u); }
#define spi_write_buffer(u,l,i) {tftio.write_buffer_uint(u,l,i); }
#define spi_write_bufferCB(u,l,i,cb,cbd) {tftio.write_buffer_uint(u,l,i,cb,cbd); }


// default DC is data
//When DCX = ’1’, data is selected. 
//When DCX = ’0’, command is selected. 
#define _DC_Cmd()	{ *dcport &= ~dcpinmask; }
#define _DC_Data()	{ *dcport |=  dcpinmask; }
#define _CS_On()	{ *csport &= ~cspinmask; }
#define _CS_Off() 	{ *csport |=  cspinmask; }

#define writecommand(u) { \
	_DC_Cmd(); \
	_CS_On(); \
	spi_write(u); \
	_CS_Off();	\
	_DC_Data(); \
}

#define writedata(u) { \
	_CS_On(); \
	spi_write(u); \
	_CS_Off();	\
}

#define swap(a, b) { int16_t t = a; a = b; b = t; }

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

#define iliRotation0	0
#define iliRotation90	1

void Canvas9341_::setRotation(uint8_t m) {

	#ifdef DEBUG
	char buffer[80];
	sprintf(buffer,"(CV) SetRotation Mode:%u",m);
	Serial.println(buffer);
	#endif

	spi_begin();
	writecommand(ILI9341_MADCTL);
	switch (m % 4) {
		case 0:
		  writedata(MADCTL_MX | MADCTL_BGR);
		  _width  = ILI9341_TFTWIDTH;
		  _height = ILI9341_TFTHEIGHT;
		  break;
		case 1:
		  writedata(MADCTL_MV | MADCTL_BGR);
		  _width  = ILI9341_TFTHEIGHT;
		  _height = ILI9341_TFTWIDTH;
		  break;
		case 2:
		  writedata(MADCTL_MY | MADCTL_BGR);
		  _width  = ILI9341_TFTWIDTH;
		  _height = ILI9341_TFTHEIGHT;
		  break;
		case 3:
		  writedata(MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
		  _width  = ILI9341_TFTHEIGHT;
		  _height = ILI9341_TFTWIDTH;
		  break;
	}
	spi_end();

	#ifdef DEBUG
	sprintf(buffer,"(CV) SetRotation W:%u H:%u",_width,_height);
	Serial.println(buffer);
	#endif
}

COORD Canvas9341_::_getDisplayWidth() {
	return _width;
}

COORD Canvas9341_::_getDisplayHeight() {
	return _height;
}


////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
void Canvas9341_::init(void) {

	// landscape
	clientRect.position(0,0).width(ILI9341_TFTHEIGHT).height(ILI9341_TFTWIDTH);
	textColor = COLOR_WHITE;
	backColor = COLOR_BLACK;
	textSize = 3;
}

void Canvas9341_::begin(void) {

	#ifdef DEBUG
	char buffer[80];
	sprintf(buffer,"(CV) begin cs:%u dc:%u rst:%u",_cs,_dc,_rst);
	Serial.println(buffer);
	#endif

	if (_rst > 0) {
		pinMode(_rst, OUTPUT);
		digitalWrite(_rst, LOW);
	}
	pinMode(_dc, OUTPUT);
	pinMode(_cs, OUTPUT);
	csport    = portOutputRegister(digitalPinToPort(_cs));
	cspinmask = digitalPinToBitMask(_cs);
	dcport    = portOutputRegister(digitalPinToPort(_dc));
	dcpinmask = digitalPinToBitMask(_dc);

	// default state
	_DC_Data();
	spi_init();

	// toggle RST low to reset
	if (_rst > 0) {
		#ifdef DEBUG
		Serial.println("(CV) hardware reset");
		#endif
		//
		digitalWrite(_rst, HIGH);
		delay(5);
		digitalWrite(_rst, LOW);
		delay(20);
		digitalWrite(_rst, HIGH);
		delay(150);
	}
	
	spi_begin();
	writecommand(0xEF);
	writedata(0x03);
	writedata(0x80);
	writedata(0x02);

	writecommand(0xCF);
	writedata(0x00);
	writedata(0XC1);
	writedata(0X30);

	writecommand(0xED);
	writedata(0x64);
	writedata(0x03);
	writedata(0X12);
	writedata(0X81);

	writecommand(0xE8);
	writedata(0x85);
	writedata(0x00);
	writedata(0x78);

	writecommand(0xCB);
	writedata(0x39);
	writedata(0x2C);
	writedata(0x00);
	writedata(0x34);
	writedata(0x02);

	writecommand(0xF7);
	writedata(0x20);

	writecommand(0xEA);
	writedata(0x00);
	writedata(0x00);

	writecommand(ILI9341_PWCTR1);    //Power control
	writedata(0x23);   //VRH[5:0]

	writecommand(ILI9341_PWCTR2);    //Power control
	writedata(0x10);   //SAP[2:0];BT[3:0]

	writecommand(ILI9341_VMCTR1);    //VCM control
	writedata(0x3e);
	writedata(0x28);

	writecommand(ILI9341_VMCTR2);    //VCM control2
	writedata(0x86);

	writecommand(ILI9341_MADCTL);    // Memory Access Control
	writedata(0x48);

	writecommand(ILI9341_PIXFMT);
	writedata(0x55);

	writecommand(ILI9341_FRMCTR1);
	writedata(0x00);
	writedata(0x18);

	writecommand(ILI9341_DFUNCTR);    // Display Function Control
	writedata(0x08);
	writedata(0x82);
	writedata(0x27);

	writecommand(0xF2);    // 3Gamma Function Disable
	writedata(0x00);

	writecommand(ILI9341_GAMMASET);    //Gamma curve selected
	writedata(0x01);

	writecommand(ILI9341_GMCTRP1);    //Set Gamma
	writedata(0x0F);
	writedata(0x31);
	writedata(0x2B);
	writedata(0x0C);
	writedata(0x0E);
	writedata(0x08);
	writedata(0x4E);
	writedata(0xF1);
	writedata(0x37);
	writedata(0x07);
	writedata(0x10);
	writedata(0x03);
	writedata(0x0E);
	writedata(0x09);
	writedata(0x00);

	writecommand(ILI9341_GMCTRN1);    //Set Gamma
	writedata(0x00);
	writedata(0x0E);
	writedata(0x14);
	writedata(0x03);
	writedata(0x11);
	writedata(0x07);
	writedata(0x31);
	writedata(0xC1);
	writedata(0x48);
	writedata(0x08);
	writedata(0x0F);
	writedata(0x0C);
	writedata(0x31);
	writedata(0x36);
	writedata(0x0F);

	writecommand(ILI9341_SLPOUT);    //Exit Sleep
	spi_end();

	delay(120);

	#ifdef DEBUG
	Serial.println("(CV) display ON");
	#endif

	spi_begin();
	writecommand(ILI9341_DISPON);    //Display on
	spi_end();
}

void Canvas9341_::setOrientation(Orientation value) {

	switch(value) {
		case portrait:
			clientRect.position(0,0).width(ILI9341_TFTWIDTH).height(ILI9341_TFTHEIGHT);
			setRotation(iliRotation0); 
		break;
		case landscape:
			clientRect.position(0,0).width(ILI9341_TFTHEIGHT).height(ILI9341_TFTWIDTH);
			setRotation(iliRotation90); 
		break;
		case portrait_reverse:
			clientRect.position(0,0).width(ILI9341_TFTWIDTH).height(ILI9341_TFTHEIGHT);
			setRotation(2); 
		break;
		case landscape_reverse:
			clientRect.position(0,0).width(ILI9341_TFTHEIGHT).height(ILI9341_TFTWIDTH);
			setRotation(3); 
		break;
	}
}

COLOR Canvas9341_::rgb(uint8_t r,uint8_t g, uint8_t b) {
	return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

void Canvas9341_::endOfTransfert(void) {
	_CS_Off();
	spi_end();
}

void _endOfTransfert(void * p) {
	static_cast<Canvas9341_*>(p)->endOfTransfert();
}

void Canvas9341_::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {

	#ifdef DEBUG
	char buffer[80];
	sprintf(buffer,"(CV) setAddrWindow x0:%u y0:%u x1:%u y1:%u",x0,y0,x1,y1);
	Serial.println(buffer);
	#endif

	writecommand(ILI9341_CASET); // Column addr set
	_CS_On();

	lineBuffer[0] = x0;
	lineBuffer[1] = x1;
	spi_write_buffer(lineBuffer,2,1);

	writecommand(ILI9341_PASET); // Row addr set
	_CS_On();

	lineBuffer[0] = y0;
	lineBuffer[1] = y1;
	spi_write_buffer(lineBuffer,2,1);

	writecommand(ILI9341_RAMWR); // write to RAM
}

void Canvas9341_::_fillPixels(COORD x, COORD y, uint16_t w, uint16_t h, COLOR color) {


	spi_begin();

	setAddrWindow(x,y, x+w-1, y+h-1);

	_CS_On();
	
	uint32_t len = (h*w);
	lineBuffer[0] = color;

	#ifdef USE_CALLBACK
	spi_write_bufferCB(lineBuffer, len, 0, &_endOfTransfert, this);
	#else
	spi_write_buffer(lineBuffer, len, 0);
	_CS_Off();
	spi_end();
	#endif
}

void Canvas9341_::_setPixel(COORD x, COORD y, COLOR color) {

	if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) return;
	
	spi_begin();

	setAddrWindow(x, y, x + 1, y + 1);

	_CS_On();
	
	lineBuffer[0] = color;
	#ifdef USE_CALLBACK
	spi_write_bufferCB(lineBuffer,1,0, &_endOfTransfert, this);
	#else
	spi_write_buffer(lineBuffer,1,0);
	_CS_Off();
	spi_end();
	#endif
}

void Canvas9341_::_drawHLine(COORD x, COORD y, uint16_t w, COLOR color) {
  
	#ifdef DEBUG
	char buffer[80];
	sprintf(buffer,"(CV) drawHLine x:%u y:%u w:%u C:%u",x,y,w,color);
	Serial.println(buffer);
	#endif

	if ( (x>=_width) || (y>=_height) || (w < 1)) return;
	if ((x + w - 1) >= _width)  w = _width  - x;
	if (w == 1) {
		_setPixel(x,y,color);
		return;
	}

	spi_begin();

	setAddrWindow(x, y, x + w - 1, y);

	_CS_On();
	
	lineBuffer[0] = color;
	
	#ifdef USE_CALLBACK
	spi_write_bufferCB(lineBuffer, w, 0, &_endOfTransfert, this);
	#else
	spi_write_buffer(lineBuffer, w, 0);
	_CS_Off();
	spi_end();
	#endif
}

void Canvas9341_::_drawVLine(COORD x, COORD y, uint16_t h, COLOR color) {

	#ifdef DEBUG
	char buffer[80];
	sprintf(buffer,"(CV) drawVLine x:%u y:%u h:%u C:%u",x,y,h,color);
	Serial.println(buffer);
	#endif

	if ((x >= _width) || (y >= _height) || (h < 1)) return;
	if ((y + h - 1) >= _height) h = _height - y;
	if (h== 1) {
		_setPixel(x,y,color);
		return;
	}

	spi_begin();

	setAddrWindow(x, y, x, y + h - 1);

	_CS_On();

	lineBuffer[0] = color;

	#ifdef USE_CALLBACK
	spi_write_bufferCB(lineBuffer, h, 0, &_endOfTransfert, this);
	#else
	spi_write_buffer(lineBuffer, h, 0);
	_CS_Off();
	spi_end();
	 #endif
}
