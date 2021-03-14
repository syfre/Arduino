#ifndef Canvas9341_stm32_h
#define Canvas9341_stm32_h

#include "../Canvas9341/Canvas9341.h"
#include "spiio.h"

class Canvas9341:public Canvas9341_ {
protected:
	SPIIO spiio;
public:
	Canvas9341(int8_t _CS, int8_t _DC, int8_t _RST = -1) : Canvas9341_(spiio,_CS,_DC,_RST) {} ;
};

#endif
