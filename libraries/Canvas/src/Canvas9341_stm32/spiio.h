#ifndef _SPIIO_H_
#define _SPIIO_H_

#include "../Canvas9341/tftio.h"
#include <libmaple/libmaple_types.h>
#include <libmaple/spi.h>
#include <libmaple/dma.h>


class SPIIO: public TFTIO {
private:
	uint8 _dmaSend(uint16_t *transmitBuf, uint16_t length, bool minc);
	uint8 _dmaAsyncSend(uint16_t *transmitBuf, uint32_t length, bool minc, TFTIOCallback callback, void * callbackData);
public:
	SPIIO();
	void init();
	void begin(void);
	void end(void);
	bool ready(void);
	void write(uint8_t u);
	void write_uint(uint16_t u);
	void write_buffer_uint(uint16_t* buffer, uint32_t len, bool inc);
	void write_buffer_uint(uint16_t* buffer, uint32_t len, bool inc, TFTIOCallback callback, void * callbackData);
};



#endif