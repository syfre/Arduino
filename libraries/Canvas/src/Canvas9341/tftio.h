#ifndef _TFTIO_H_
#define _TFTIO_H_

#include <Arduino.h>

// because std::function is not supported
// use a static call back with the object address in parameter
typedef void (*TFTIOCallback)(void *);

class TFTIO {
public:
	virtual void init() = 0;
	virtual bool ready(void) = 0;
	virtual void begin(void) = 0;
	virtual void end(void) = 0;
	virtual void write(uint8_t u) = 0;
	virtual void write_uint(uint16_t u) = 0;
	virtual void write_buffer_uint(uint16_t* buffer, uint32_t len, bool inc) = 0;
	virtual void write_buffer_uint(uint16_t* buffer, uint32_t len, bool inc, TFTIOCallback callback, void * callbackData) = 0;
};
#endif