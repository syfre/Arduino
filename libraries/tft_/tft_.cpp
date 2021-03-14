#include <stdlib.h>
#include <stdio.h>
#include "tft.hpp"

//////////////////////////////////////////////////////////////////////
// ST7735/ILI9341 commands
//////////////////////////////////////////////////////////////////////

#define TFT_NOP 0x0
#define TFT_SWRESET 0x01
#define TFT_RDDID 0x04
#define TFT_RDDST 0x09

#define TFT_SLPIN  0x10
#define TFT_SLPOUT  0x11
#define TFT_PTLON  0x12
#define TFT_NORON  0x13

#define TFT_INVOFF 0x20
#define TFT_INVON 0x21
#define TFT_DISPOFF 0x28
#define TFT_DISPON 0x29
#define TFT_CASET 0x2A
#define TFT_RASET 0x2B
#define TFT_RAMWR 0x2C
#define TFT_RAMRD 0x2E
#define TFT_SETLTU 0x2D
#define TFT_RAMWRC 0x3C

#define TFT_COLMOD 0x3A
#define TFT_MADCTL 0x36


#define TFT_FRMCTR1 0xB1
#define TFT_FRMCTR2 0xB2
#define TFT_FRMCTR3 0xB3
#define TFT_INVCTR 0xB4
#define TFT_DISSET5 0xB6

#define TFT_PWCTR1 0xC0
#define TFT_PWCTR2 0xC1
#define TFT_PWCTR3 0xC2
#define TFT_PWCTR4 0xC3
#define TFT_PWCTR5 0xC4
#define TFT_VMCTR1 0xC5

#define TFT_RDID1 0xDA
#define TFT_RDID2 0xDB
#define TFT_RDID3 0xDC
#define TFT_RDID4 0xDD

#define TFT_PWCTR6 0xFC

#define TFT_GMCTRP1 0xE0
#define TFT_GMCTRN1 0xE1

// memory access control bits
//
#define MADCTL		0xE0
#define	MADCTL_MY	0x80
#define	MADCTL_MX	0x40
#define MADCTL_MV	0x20
#define	MADCTL_ML	0x10
#define	MADCTL_BGR	0x80
#define MADCTL_MH	0x40

//////////////////////////////////////////////////////////////////////

#define swap(a, b) { int t = a; a = b; b = t; }

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define	DIGIT_ULONG_FMT (rom far char *)"%010lu"
#define	DIGIT_LONG_FMT (rom far char *)"%010ld"

///////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////

// conversion buffer
char convBuffer[12];

//0<= a <=90
//sin = sinwave
//cos = sinwave[90-a]
//90< a <=180
//sin = sinwave[90-(a-90)] = sinwave[180-a]
//cos = -sinwave[a-90]
//
rom far UINT16 sinwave[91] = {
0,18,36,54,71,89,107,125,143,160,178,195,213,230,248,265,282,299,316,333,350,367,384,400,416,433,449,465,481,496,512,527
,543,558,573,587,602,616,630,644,658,672,685,698,711,724,737,749,761,773,784,796,807,818,828,839,849,859,868,878,887,896,904,912
,920,928,935,943,949,956,962,968,974,979,984,989,994,998,1002,1005,1008,1011,1014,1016,1018,1020,1022,1023,1023,1024,1024
};

///////////////////////////////////////////////////////////
// code
///////////////////////////////////////////////////////////

#define _RESET_1()	pinreset->set()
#define _RESET_0()	pinreset->clear()
#define _RS_0()		pindc->clear()
#define _RS_1()		pindc->set()
#define _CS_ON()	{spi->acquire(speed); pincs->clear();}
#define _CS_OFF()	{pincs->set(); spi->release();}

inline void TFT::writecommand(UINT8 c) {
	_RS_0();
	_CS_ON();
	spi->write(c);
	_CS_OFF();
}

inline void TFT::write_byte(UINT8 b) {
	_CS_ON();
	_RS_1();
        spi->write(b);
	_CS_OFF();
}

inline void TFT::write_uint(UINT16 ui) {
	_CS_ON();
	_RS_1();
	spi->write((ui) >> 8);
	spi->write(ui);
	_CS_OFF();
}

inline void TFT::write_color(COLOR c) {
	_CS_ON();
	_RS_1();
	spi->write((c) >> 8);
	spi->write(c);
	_CS_OFF();
}

UINT8 TFT::readcommand(UINT8 c, UINT8 index) {

UINT8 r;
    _RS_0();
    _CS_ON();
    spi->write(0xd9);
    _RS_1();
    spi->write(0x10+index);
    _CS_OFF();

    _RS_0();
    _CS_ON();
    spi->write(c);

    _RS_1();
    r = spi->write(0);

    _CS_OFF();
    _RS_0();

    return r;
}

UINT8 TFT::getRotation() {
	return madctl;
}

void TFT::setRotation(UINT8 m) {

	if (madctl!=m) {
		madctl = m;
                switch (m) {
                    case TFT_PORTRAIT:
                        screenX = HScreenX;
                        screenY = HScreenY;
                    break;
                    case TFT_PORTRAIT_REVERSE:
                        screenX = HScreenX;
                        screenY = HScreenY;
                    break;
                    case TFT_LANDSCAPE:
                        screenX = HScreenY;
                        screenY = HScreenX;
                    break;
                    case TFT_LANDSCAPE_REVERSE:
                        screenX = HScreenY;
                        screenY = HScreenX;
                    break;
                }
		writecommand(TFT_MADCTL);  // memory access control (directions)
		write_byte(madctl); // row address/col address, bottom to top refresh
	}
}

UINT8 TFT::readRegister(UINT8 addr, UINT8 index) {
    
    // index : 1...
    return readcommand(addr,index);
}

inline void TFT::_setAddrWindow(COORD x0, COORD y0, COORD x1, COORD y1)
{
        _RS_0();
	spi->write(TFT_CASET);
        _RS_1();
	spi->write((y0) >> 8);
	spi->write(y0);
	spi->write((y1) >> 8);
	spi->write(y1);

        _RS_0();
	spi->write(TFT_RASET);
        _RS_1();
	spi->write((x0) >> 8);
	spi->write(x0);
	spi->write((x1) >> 8);
	spi->write(x1);

	_RS_0();
	spi->write(TFT_RAMWR);
	spi->write(TFT_RAMWRC);
}

inline void TFT::setAddrWindow(COORD x0, COORD y0, COORD x1, COORD y1) {
	_CS_ON();
        _setAddrWindow(x0,y0,x1,y1);
        _CS_OFF();
}

inline void TFT::_writePixel(COORD x, COORD y, COLOR color)
{
	_setAddrWindow(x, y, x+1, y+1);
	_RS_1();
	spi->write((color) >> 8);
	spi->write(color);
}

inline void TFT::writePixel(COORD x, COORD y, COLOR color)
{
	_CS_ON();
        _writePixel(x,y,color);
        _CS_OFF();
}

inline COLOR TFT::readPixel(COORD x, COORD y)
{
    UINT8 br,bg,bb;

        // Read data are always 32 bits in 18 bits format
        // b1 : dummy byte
        // b2 : Blue
        // b3 : Green
        // b4 : Red
        // bits RRRRRRxx

        _CS_ON();
        _setAddrWindow(x, y, x+1, y+1);

        _RS_0();
        spi->write(TFT_RAMRD);
        _RS_1();
        bb = spi->write(0);
        bb = spi->write(0);
        bg = spi->write(0);
        br = spi->write(0);
        _CS_OFF();

        return (((COLOR)(bb)<<8) & 0xf800)  + (((COLOR)(bg)<<3) & 0x07e0)  + (((COLOR)(br) >> 3) & 0x001f);
}


void TFT::drawChar(COORD x, COORD y, UINT8 c, COLOR color, COLOR backcolor, UINT8 size)
{
	UINT8 i,j,si,sj;
	UINT8 line;

	_FONT_PTR_ font = TFT_FONT + ( (unsigned int)c-FONT_BASE)*FONT_WIDTH;

	_CS_ON();
	if (size==1) {

		_setAddrWindow(x, y, x+FONT_WIDTH-1, y+FONT_HEIGHT-1);
		_RS_1();
		for (i=0; i<FONT_WIDTH; i++ ) {
			line = *font++;
			for (j = 0; j<FONT_HEIGHT; j++) {

				if (line & 0x1) {
					spi->write_uint(color);
				} else {
					spi->write_uint(backcolor);
				}

				line >>= 1;
			}
		}

	} else {

		_setAddrWindow(x, y, x+size*FONT_WIDTH-1, y+size*FONT_HEIGHT-1);
		_RS_1();
		for (i=0; i<FONT_WIDTH; i++ ) {
			for (si = 0; si<size; si++) {
				line = *font;
				for (j = 0; j<FONT_HEIGHT; j++) {
					for (sj=0; sj<size; sj++)
					if (line & 0x1) {
						spi->write_uint(color);
					} else {
						spi->write_uint(backcolor);
					}
					line >>= 1;
				}
			}
			font++;
		}
	}
	_CS_OFF();
}

void TFT::drawString(COORD x, COORD y, char *c, COLOR color, COLOR backcolor, UINT8 size)
{
	while (*c != 0) {
		drawChar(x, y, *c++, color, backcolor, size);
		x += size*(FONT_WIDTH+FONT_SEP_WIDTH);
		if (x + size*FONT_WIDTH >= screenX) {
			y += size*FONT_HEIGHT+FONT_SEP_HEIGHT;
			x = 0;
		}
	}
}

void TFT::drawULongFmt(COORD x, COORD y, rom far char *fmt, unsigned long value, COLOR color, COLOR backcolor, UINT8 size) {
	sprintf(convBuffer,fmt,value);
	drawString(x,y, convBuffer, color, backcolor, size);
}

void TFT::drawULong(COORD x, COORD y, unsigned long value, COLOR color, COLOR backcolor, UINT8 size) {

	sprintf(convBuffer,DIGIT_ULONG_FMT,value);
	drawString(x,y, convBuffer, color, backcolor, size);
}

void TFT::drawLong(COORD x, COORD y, long value, COLOR color, COLOR backcolor, UINT8 size) {
// max is 10 digit + sign

	sprintf(convBuffer,DIGIT_LONG_FMT,value);
	drawString(x,y, convBuffer, color, backcolor, size);
}

void TFT::dumpFont(COORD x0, COORD y0, COLOR color, COLOR backcolor, unsigned int size) {

	COORD x;
	UINT8 i;

	x=x0;
	for (i=0x20;  i<0x80; i++) {
		if ((x+FONT_WIDTH*size)>screenX) { x = x0; y0+=FONT_HEIGHT*size; }
		drawChar(x,y0, i, color, backcolor, size);
		x+=FONT_WIDTH*size;
	}
}

// fill a circle
void TFT::fillCircle(COORD x0, COORD y0, unsigned int r, COLOR color) {
	int f = 1 - r;
	int ddF_x = 1;
	int ddF_y = -2 * r;
	int x = 0;
	int y = r;

	drawVerticalLine(x0, y0-r, 2*r+1, color);

	while (x<y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		drawVerticalLine(x0+x, y0-y, 2*y+1, color);
		drawVerticalLine(x0-x, y0-y, 2*y+1, color);
		drawVerticalLine(x0+y, y0-x, 2*x+1, color);
		drawVerticalLine(x0-y, y0-x, 2*x+1, color);
	}
}

// draw a circle outline
void TFT::drawCircle(COORD x0, COORD y0, unsigned int r, COLOR color) {
	int f = 1 - r;
	int ddF_x = 1;
	int ddF_y = -2 * r;
	int x = 0;
	int y = r;

	writePixel(x0, y0+r, color);
	writePixel(x0, y0-r, color);
	writePixel(x0+r, y0, color);
	writePixel(x0-r, y0, color);

	while (x<y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		writePixel(x0 + x, y0 + y, color);
		writePixel(x0 - x, y0 + y, color);
		writePixel(x0 + x, y0 - y, color);
		writePixel(x0 - x, y0 - y, color);
		
		writePixel(x0 + y, y0 + x, color);
		writePixel(x0 - y, y0 + x, color);
		writePixel(x0 + y, y0 - x, color);
		writePixel(x0 - y, y0 - x, color);
	}
}

void TFT::fillRect(COORD x, COORD y, unsigned int w, unsigned int h, COLOR color) {

	setAddrWindow(x, y, x+w-1, y+h-1);

	// setup for data
	_RS_1();
	_CS_ON();
	for (x=0; x < w; x++) {
            for (y=0; y < h; y++) {
            spi->write_uint(color);
	}
	}
	_CS_OFF();
}

void TFT::flipRectV(COORD x, COORD y, COORD w, COORD h) {

    // Reverse a rect vertically using read/write pixel

    COORD xx,yy,w2,newx;
    COLOR p1,p2;

    // w must be 2 multiple
    w &= 0xfff7;
    w2 = w >> 1;

    for (xx=x; xx<x+w2; xx++)
    for (yy=y; yy<y+h; yy++) {
        newx = x+x+h - xx;
        p1 = readPixel(newx,yy);
        p2 = readPixel(xx,yy);
        writePixel(newx,yy,p2);
        writePixel(xx,yy,p1);
    }

}

void TFT::fillScreen(COLOR color)
{
	fillRect(0,0,screenX,screenY,color);
}

void TFT::drawFastLine(COORD x, COORD y, unsigned int length, COLOR color, UINT8 rotflag)
{
	_CS_ON();
	if (rotflag) {
            // Vert
            _setAddrWindow(x, y, x, y+length);
	} else {
            // Horz
            _setAddrWindow(x, y, x+length, y);
	}
	// setup for data
	_RS_1();
	while (length--) spi->write_uint(color);
	_CS_OFF();
}

void TFT::drawVerticalLine(COORD x, COORD y, unsigned int length, COLOR color)
{
	if (x >= screenX) return;
	if (y+length >= screenY) length = screenY-y-1;
	drawFastLine(x,y,length,color,1);
}

void TFT::drawHorizontalLine(COORD x, COORD y, unsigned int length, COLOR color)
{
	if (y >= screenY) return;
	if (x+length > screenX) length = screenX-x-1;
	drawFastLine(x,y,length,color,0);
}

// draw a rectangle
void TFT::drawRect(COORD x, COORD y, unsigned int w, unsigned int h, COLOR color) {
	// smarter version
	drawHorizontalLine(x, y, w, color);
	drawHorizontalLine(x, y+h-1, w, color);
	drawVerticalLine(x, y, h, color);
	drawVerticalLine(x+w-1, y, h, color);
}

void TFT::drawCrosshair(COORD x, COORD y, unsigned int w, unsigned int h, COLOR color) {
	// smarter version
	drawHorizontalLine(x-w, y, w+w, color);
	drawVerticalLine(x, y-h, h+h, color);
}

void TFT::writePixels(COORD * buffer)
{
    COORD x,y,ctn;
    COLOR color;
    COORD * ptr = buffer;
    _CS_ON();
    ctn = *ptr++;
    while (ctn--) {
        x = *ptr++;
        y = *ptr++;
        color = *ptr++;
        _writePixel(x,y,color);
    }
    _CS_OFF();
}

// bresenham's algorithm - thx wikpedia
COORD TFT::readLine(int x0, int y0, int x1, int y1, COLOR color, COORD *buffer) {

        COORD dx, dy;
	COORD steep = abs(y1 - y0) > abs(x1 - x0);
	int err;
	int ystep;
        COORD   ctn = 0;

	if (steep) {
		swap(x0, y0);
		swap(x1, y1);
	}

	if (x0 > x1) {
		swap(x0, x1);
		swap(y0, y1);
	}

	dx = x1 - x0;
	dy = abs(y1 - y0);
	err = dx / 2;

	if (y0 < y1) {
		ystep = 1;
	} else {
		ystep = -1;
        }
	for (; x0<=x1; x0++) {
            ctn++;
            if (steep) {
                if (buffer) {
                    *buffer++ =  y0;
                    *buffer++ =  x0;
                    *buffer++ = readPixel(y0,x0);
                }
                //TFT_writePixel(y0, x0, color);
            } else {
                if (buffer) {
                    *buffer++ =  x0;
                    *buffer++ =  y0;
                    *buffer++ = readPixel(x0,y0);
                }
                //TFT_writePixel(x0, y0, color);
            }
            err -= dy;
            if (err < 0) {
                    y0 += ystep;
                    err += dx;
            }
	}
        return ctn;
}

COORD TFT::drawLine(int x0, int y0, int x1, int y1, COLOR color) {

        COORD dx, dy;
	COORD steep = abs(y1 - y0) > abs(x1 - x0);
	int err;
	int ystep;
        COORD   ctn = 0;

	if (steep) {
		swap(x0, y0);
		swap(x1, y1);
	}

	if (x0 > x1) {
		swap(x0, x1);
		swap(y0, y1);
	}

	dx = x1 - x0;
	dy = abs(y1 - y0);
	err = dx / 2;

	if (y0 < y1) {
		ystep = 1;
	} else {
		ystep = -1;
        }
	_CS_ON();
	for (; x0<=x1; x0++) {
            ctn++;
            if (steep) {
                _writePixel(y0, x0, color);
            } else {
                _writePixel(x0, y0, color);
            }
            err -= dy;
            if (err < 0) {
                    y0 += ystep;
                    err += dx;
            }
	}
	_CS_OFF();
        return ctn;
}

COORD TFT::drawLine(int x0, int y0, int x1, int y1, COLOR color, COORD *buffer) {

        COORD dx, dy;
	COORD steep = abs(y1 - y0) > abs(x1 - x0);
	int err;
	int ystep;
        COORD   ctn = 0;

	if (steep) {
		swap(x0, y0);
		swap(x1, y1);
	}

	if (x0 > x1) {
		swap(x0, x1);
		swap(y0, y1);
	}

	dx = x1 - x0;
	dy = abs(y1 - y0);
	err = dx / 2;

	if (y0 < y1) {
		ystep = 1;
	} else {
		ystep = -1;
        }

	for (; x0<=x1; x0++) {
            ctn++;
            if (steep) {
                if (buffer) {
                    *buffer++ =  y0;
                    *buffer++ =  x0;
                    *buffer++ = readPixel(y0,x0);
                }
                writePixel(y0, x0, color);
            } else {
                if (buffer) {
                    *buffer++ =  x0;
                    *buffer++ =  y0;
                    *buffer++ = readPixel(x0,y0);
                }
                writePixel(x0, y0, color);
            }
            err -= dy;
            if (err < 0) {
                    y0 += ystep;
                    err += dx;
            }
	}
        return ctn;
}

void TFT::drawNeedle(NEEDLE_DESC *needle)
{
    // angle : 0..180 degree
    //
    //0<= a <=90
    //sin = sinwave
    //cos = sinwave[90-a]
    //90< a <=180
    //sin = sinwave[90-(a-90)] = sinwave[180-a]
    //cos = -sinwave[a-90]
    //
    COORD x1,y1;
    COORD ctn;

    if (needle->degree <=90)  {
        x1 = needle->cx - (needle->len * sinwave[90-needle->degree] / 1024);
        y1 = needle->cy - (needle->len * sinwave[needle->degree] / 1024);
    } else {
        x1 = needle->cx + (needle->len * sinwave[needle->degree-90] / 1024);
        y1 = needle->cy - (needle->len * sinwave[180-needle->degree] / 1024);
    }

    if (needle->pixels) writePixels(needle->pixels);

    ctn = 0;
    ctn  += readLine(needle->cx-1,needle->cy,x1-1,y1,needle->color,needle->pixels+1);
    ctn += readLine(needle->cx,needle->cy,x1,y1,needle->color,needle->pixels+1+3*ctn);
    ctn += readLine(needle->cx+1,needle->cy,x1+1,y1,needle->color,needle->pixels+1+3*ctn);

    *needle->pixels = ctn;

    drawLine(needle->cx-1,needle->cy,x1-1,y1,needle->color);
    drawLine(needle->cx,needle->cy,x1,y1,needle->color);
    drawLine(needle->cx+1,needle->cy,x1+1,y1,needle->color);

}

//////////////////////////////////////////////////////////////////////
// Image support
//////////////////////////////////////////////////////////////////////

unsigned TFT::drawFile(COORD x, COORD y, MFILE *file) {

    // image file is binary content
    //
    IMAGE_DESC image;
    if (freadM(&image,sizeof(IMAGE_DESC),file)!=sizeof(IMAGE_DESC)) {
        return 0;
    }

    setAddrWindow(x, y, x+image.width-1, y+image.height-1);

    PIXEL_RLE_DESC rle;
    unsigned bytes = sizeof(IMAGE_DESC);

    _RS_1();
    for (x=0; x < image.width; x++) {

     COORD colcount = 0;
     while (colcount < image.height) {
       if (freadM(&rle, sizeof(PIXEL_RLE_DESC), file)==sizeof(PIXEL_RLE_DESC)) {
            bytes += sizeof(PIXEL_RLE_DESC);
            colcount += rle.pcount;
            _CS_ON();
            while (rle.pcount--)
            spi->write_uint(rle.pcolor);
            _CS_OFF();
       } else {
           return bytes;
       }
     }
    }
    _CS_OFF();
    return bytes;
}

void TFT::drawImage(COORD x, COORD y, IMAGE_DESC *image) {

    // image is RLE encoded
    _IMAGE_PTR_ pdata = image->data;

    setAddrWindow(x, y, x+image->width-1, y+image->height-1);

    _RS_1();
    _CS_ON();
    for (x=0; x < image->width; x++) {
     COORD colcount = 0;
     while (colcount < image->height) {
       COLOR pcount = *pdata++;
       COLOR pcolor = *pdata++;
       colcount += pcount;
       while (pcount--) 
       spi->write_uint(pcolor);
     }
    }
    _CS_OFF();
}

void TFT::drawImagePart(COORD x, COORD y, COORD ofsx, COORD ofsy, COORD width, COORD height, IMAGE_DESC *image) {

    // image is RLE encoded
    // As the data are RLE encoded the data line is not constant
    // We must scan all the image until done.
    //
    _IMAGE_PTR_ pdata = image->data;

    UINT8 inx;
    COORD xx;

    for (xx=0; xx < image->width; xx++) {

     // check done
     if (xx >= x+width) break;

     // check inside dest rectangle
     if ((xx>=x) && (xx<x+width)) {
        setAddrWindow(xx, y, xx, y+height);
        _RS_1();
        _CS_ON();
        inx = 1;
     } else inx=0;

     COORD yy = 0;
     while (yy < image->height) {
       COLOR pcount = *pdata++;
       COLOR pcolor = *pdata++;
       while (pcount--) {
         if ((inx) && (yy>=y) && (yy<y+height)) {
           spi->write_uint(pcolor);
         }
         yy++;
       }
     }
    }
    _CS_OFF();
}



//////////////////////////////////////////////////////////////////////
// Terminal support
//////////////////////////////////////////////////////////////////////

TFT	* stdTFT;

// stdlib character handler
// This is C CODE
// 
#ifdef __cplusplus
extern "C" {
#endif

void _mon_putc(char data) {
	if (stdTFT) stdTFT->_putc(data);
}
#ifdef __cplusplus
}
#endif
//////////////////////////////////////////////////////////////////////

void TFT::_scrollVertical(void) {
}

void TFT::_newline(void) {

	// newline
	cursorY += fontSize*FONT_HEIGHT+FONT_SEP_HEIGHT;
	cursorX = 0;

	if (cursorY + fontSize*FONT_HEIGHT+FONT_SEP_HEIGHT >= screenY) {
            _scrollVertical();
            cursorY = 0;
	}
}

void TFT::_incCursor(void) {

	// love cursor 
	cursorX += fontSize*(FONT_WIDTH+FONT_SEP_WIDTH);
	if (cursorX + fontSize*FONT_WIDTH >= screenX) {
		_newline();
	}
}

void TFT::_putc(char data) {

	if (data==13) {
            return;
        }
	if (data==10) {
	 // newline
	 _newline();
	 return;
	}

	stdTFT->drawChar(cursorX, cursorY, data, fontColor, backColor, fontSize);
	_incCursor();
	return;
}

void TFT::assign_stdout(void) {

	// set the user_putc function
	// assign stdout to _USER_
    
    setbuf(stdout, NULL);
 	fontSize = 2;
	backColor = COLOR_WHITE;
	fontColor = COLOR_BLACK;
	cursorX = 0;
	cursorY = 0;
	stdTFT = this;
}

void TFT::printChars(char *c, int size)
{
    while (size--) _putc(*c++);
}

void TFT::startOfLine(void) {
    cursorX = 0;
}

void TFT::clear(void) {
    fillScreen(backColor);
    cursorX = 0;
    cursorY = 0;
}

void TFT::clearEndOfLine(void) {

    COORD save = cursorX;
    while (cursorX + fontSize*FONT_WIDTH < screenX) {
	drawChar(cursorX, cursorY, 0x20, fontColor, backColor, fontSize);
	cursorX += fontSize*(FONT_WIDTH+FONT_SEP_WIDTH);
    }
    cursorX = save;
}

void TFT::printDisplayStatus(void) {

    UINT8 reg;
    reg = readRegister(0xd3,1);
    printf("ID #1 %x\n",reg);
    reg = readRegister(0xd3,2);
    printf("ID #2 %x\n",reg);
    reg = readRegister(0xd3,3);
    printf("ID #3 %x\n",reg);

    reg = readRegister(0x0b,1);
    printf("MADCTL %x\n",reg);
    reg = readRegister(0x0c,1);
    printf("Pixel format %x\n",reg);
    reg = readRegister(0x0d,1);
    printf("Image format %x\n",reg);
    reg = readRegister(0x0a,1);
    printf("Power mdoe %x\n",reg);
}

////////////////////////////////////////////////////////////////////////////////////

void TFT::ST7735_initialize(UINT8 mode) {

	_RESET_1();
	delay_us(500);
	_RESET_0();
	delay_us(500);
	_RESET_1();
	delay_us(500);

	_CS_ON();

	writecommand(TFT_SWRESET); // software reset
	delay_us(150);

	writecommand(TFT_SLPOUT);  // out of sleep mode
	delay_us(500);

	writecommand(TFT_COLMOD);  // set color mode
	write_byte(0x05);        // 16-bit color
	delay_us(10);

	writecommand(TFT_FRMCTR1);  // frame rate control - normal mode
	write_byte(0x01);  // frame rate = fosc / (1 x 2 + 40) * (LINE + 2C + 2D)
	write_byte(0x2C); 
	write_byte(0x2D); 

	writecommand(TFT_FRMCTR2);  // frame rate control - idle mode
	write_byte(0x01);  // frame rate = fosc / (1 x 2 + 40) * (LINE + 2C + 2D)
	write_byte(0x2C); 
	write_byte(0x2D); 

	writecommand(TFT_FRMCTR3);  // frame rate control - partial mode
	write_byte(0x01); // dot inversion mode
	write_byte(0x2C); 
	write_byte(0x2D); 
	write_byte(0x01); // line inversion mode
	write_byte(0x2C); 
	write_byte(0x2D); 

	writecommand(TFT_INVCTR);  // display inversion control
	write_byte(0x07);  // no inversion

	writecommand(TFT_PWCTR1);  // power control
	write_byte(0xA2);      
	write_byte(0x02);      // -4.6V
	write_byte(0x84);      // AUTO mode

	writecommand(TFT_PWCTR2);  // power control
	write_byte(0xC5);      // VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD

	writecommand(TFT_PWCTR3);  // power control
	write_byte(0x0A);      // Opamp current small 
	write_byte(0x00);      // Boost frequency

	writecommand(TFT_PWCTR4);  // power control
	write_byte(0x8A);      // BCLK/2, Opamp current small & Medium low
	write_byte(0x2A);     

	writecommand(TFT_PWCTR5);  // power control
	write_byte(0x8A);    
	write_byte(0xEE);     

	writecommand(TFT_VMCTR1);  // power control
	write_byte(0x0E);  

	writecommand(TFT_INVOFF);    // don't invert display

	// set mode
	madctl = 0xFF;
	setRotation(mode);

	writecommand(TFT_COLMOD);  // set color mode
	write_byte(0x05);        // 16-bit color

	writecommand(TFT_CASET);  // column addr set
	write_byte(0x00);
	write_byte(0x00);   // XSTART = 0
	write_byte(0x00);
	write_byte(0x7F);   // XEND = 127

	writecommand(TFT_RASET);  // row addr set
	write_byte(0x00);
	write_byte(0x00);    // XSTART = 0
	write_byte(0x00);
	write_byte(0x9F);    // XEND = 159

	writecommand(TFT_GMCTRP1);
	write_byte(0x0f);
	write_byte(0x1a);
	write_byte(0x0f);
	write_byte(0x18);
	write_byte(0x2f);
	write_byte(0x28);
	write_byte(0x20);
	write_byte(0x22);
	write_byte(0x1f);
	write_byte(0x1b);
	write_byte(0x23);
	write_byte(0x37);
	write_byte(0x00);
	write_byte(0x07);
	write_byte(0x02);
	write_byte(0x10);
	writecommand(TFT_GMCTRN1);
	write_byte(0x0f); 
	write_byte(0x1b); 
	write_byte(0x0f); 
	write_byte(0x17); 
	write_byte(0x33); 
	write_byte(0x2c); 
	write_byte(0x29); 
	write_byte(0x2e); 
	write_byte(0x30); 
	write_byte(0x30); 
	write_byte(0x39); 
	write_byte(0x3f); 
	write_byte(0x00); 
	write_byte(0x07); 
	write_byte(0x03); 
	write_byte(0x10); 

        // Initialize the LTU
        // 128 bytes
        // R:5 32 values
        // G:6 64 values
        // B:5 32 values
        UINT8 r;
        writecommand(TFT_SETLTU);
        for (r=0; r<32; r++) write_byte( r << 1);
        for (r=0; r<64; r++) write_byte( r);
        for (r=0; r<32; r++) write_byte( r << 1);

        writecommand(TFT_DISPON);
	delay_us(100);

	writecommand(TFT_NORON);  // normal display on
	delay_us(10);

	_CS_OFF();	\
}

void TFT::ILI9341_initialize(UINT8 mode)
{
    if (pinreset) {
	_RESET_1();
	delay_us(500);
	_RESET_0();
	delay_us(500);
	_RESET_1();
	delay_us(500);
    }

    _CS_ON();

    delay_ms(500);
    writecommand(0x01); // SWRESET
    delay_ms(200);

    writecommand(0xCF); // Power Control B
    write_byte(0x00);
    write_byte(0x8B);
    write_byte(0X30);

    writecommand(0xED); // Power on sequence Control
    write_byte(0x67);
    write_byte(0x03);
    write_byte(0X12);
    write_byte(0X81);

    writecommand(0xE8); // Driver  timing control A
    write_byte(0x85);
    write_byte(0x10);
    write_byte(0x7A);

    writecommand(0xCB); //Power control A
    write_byte(0x39);
    write_byte(0x2C);
    write_byte(0x00);
    write_byte(0x34);
    write_byte(0x02);

    writecommand(0xF7); //Pump ratio control
    write_byte(0x20);

    writecommand(0xEA); //Driver control B
    write_byte(0x00);
    write_byte(0x00);

    writecommand(0xC0); // PWCTR1	/* Power control                */
    write_byte(0x1B);			/* VRH[5:0]                     */

    writecommand(0xC1); //PWCTR2		/* Power control                */
    write_byte(0x10);  			/* SAP[2:0];BT[3:0]             */

    writecommand(0xC5); // VMCTR1  	/* VCM control                  */
    write_byte(0x3F);
    write_byte(0x3C);

    writecommand(0xC7);	//VCOMCTRL 2			/* VCM control2                 */
    write_byte(0XB7);

    madctl = 0xFF;
    setRotation(mode);
    //writecommand(0x36);	// MADCTL	/* Memory Access Control        */
    //write_byte(0x08);

    writecommand(0x3A);	// COLMOD	
    write_byte(0x55);

    writecommand(0xB1);	// FMCTR1
    write_byte(0x00);
    write_byte(0x1B);	// 70 hz default

    writecommand(0xB6);	//DISSETS	/* Display Function Control     */
    write_byte(0x0A);
    write_byte(0xA2);

    writecommand(0xF2); // Enable 3 gama		/* 3Gamma Function Disable      */
    write_byte(0x00);

    writecommand(0x26);	//			/* Gamma curve selected         */
    write_byte(0x01);

    writecommand(0xE0);	// GMCTRP1 /* Set Gamma                    */
    write_byte(0x0F);
    write_byte(0x2A);
    write_byte(0x28);
    write_byte(0x08);
    write_byte(0x0E);
    write_byte(0x08);
    write_byte(0x54);
    write_byte(0XA9);
    write_byte(0x43);
    write_byte(0x0A);
    write_byte(0x0F);
    write_byte(0x00);
    write_byte(0x00);
    write_byte(0x00);
    write_byte(0x00);

    writecommand(0XE1);	// GMCTRN1	/* Set Gamma                    */
    write_byte(0x00);
    write_byte(0x15);
    write_byte(0x17);
    write_byte(0x07);
    write_byte(0x11);
    write_byte(0x06);
    write_byte(0x2B);
    write_byte(0x56);
    write_byte(0x3C);
    write_byte(0x05);
    write_byte(0x10);
    write_byte(0x0F);
    write_byte(0x3F);
    write_byte(0x3F);
    write_byte(0x0F);

    writecommand(0x11);	//SLPOUT	/* Exit Sleep                   */
    delay_ms(120);
    writecommand(0x29);	//DISPON	/* Display on                   */
}

TFT::TFT(SPI * Spi, PIN *cs, PIN *dc, PIN *reset, UINT16 Speed, COORD screenX, COORD screenY, UINT8 mode) {

	spi = Spi;
	pincs = cs;
	pindc = dc;
	pinreset = reset;
	speed = Speed;
	HScreenX = screenX;
	HScreenY = screenY;
}

void TFT::initialize(UINT8 mode) {

	#ifdef TFT_ILI9341
	ILI9341_initialize(mode);
	#endif
	#ifdef TFT_ST7735
	ST7735_initialize(mode);
	#endif
}
