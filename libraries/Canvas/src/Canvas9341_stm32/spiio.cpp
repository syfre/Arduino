#include "spiio.h"
#include <SPI.h>

#define _USE_ASYNC
#define ASYNC_BUFFERED
#define __DEBUG_
#define LED_
#define pinLed	PC13

#define spi_datasize_16bits() {	SPI.setDataSize(DATA_SIZE_16BIT); }
#define spi_datasize_8bits() {	SPI.setDataSize(DATA_SIZE_8BIT); }

#define spi_begin() { \
	SPI.setBitOrder(MSBFIRST); \
	SPI.setDataMode(SPI_MODE0); \
	SPI.setDataSize(DATA_SIZE_8BIT); \
	SPI.setClockDivider(SPI_CLOCK_DIV2); \
	SPI.begin(); \
}
#define spi_end() { \
	SPI.setDataSize(DATA_SIZE_8BIT); \
}

typedef void (*dmaHandler)();

class SPIIOSettings {
public:
	spi_dev*	spi_d;
	dma_channel spiRxDmaChannel;
	dma_channel spiTxDmaChannel;
	dma_dev* 	spiDmaDev;
	//
	uint16_t*	transmit;
	uint32_t	length;
	bool		minc;
	//
	dmaHandler	dmahandler;
	TFTIOCallback callback;
	void * 		callbackData;
	//
	void config(spi_dev* _spi, dma_dev* _dma, dma_channel _dmatx, dma_channel _dmarx, dmaHandler _handler) {
	
		spi_d = _spi;
		spiDmaDev = _dma;
		spiTxDmaChannel = _dmatx;
		spiRxDmaChannel = _dmarx;
		dmahandler = _handler;
		callback = NULL;
		length = 0;
	}
	inline void setDataSize_8bits()	{
		spi_d->regs->CR1 &= ~(SPI_CR1_DFF);
	}
	inline void setDataSize_16bits()	{
		spi_d->regs->CR1 |= SPI_CR1_DFF;
	}
	void setBitOrder(BitOrder bitOrder)	{
		spi_d->regs->CR1 &= ~(SPI_CR1_LSBFIRST);
		if (bitOrder==LSBFIRST ) spi_d->regs->CR1 |= SPI_CR1_LSBFIRST;
	}
	void setClockDivider(uint32_t clockDivider)	{
		spi_d->regs->CR1 &=  ~(SPI_CR1_BR);
		spi_d->regs->CR1 |= (clockDivider & SPI_CR1_BR);
	}
	void setDataMode(uint8_t dataMode)	{
		spi_d->regs->CR1 &= ~(SPI_CR1_CPOL | SPI_CR1_CPHA);
		spi_d->regs->CR1 |= (dataMode & (SPI_CR1_CPOL | SPI_CR1_CPHA));
	}
	void init() {
		dma_init(spiDmaDev);
	}
	void begin(void) {
		// for SPI sharing
		spi_begin();
		setDataSize_16bits();
	}
	void end(void) {
		// for SPI sharing
		setDataSize_8bits();
		spi_end();
	}

	void setCallback(TFTIOCallback _callback, void * _callbackData) {
		callback = _callback;
		callbackData = _callbackData;
	}

	void _start() {

		uint32 flags = ((DMA_MINC_MODE * minc) |  DMA_FROM_MEM | DMA_TRNS_CMPLT | DMA_TRNS_ERR);
			
		// TX
		spi_tx_dma_enable(spi_d);
		//		
		dma_setup_transfer(spiDmaDev, 
							spiTxDmaChannel, 
							&spi_d->regs->DR, 
							DMA_SIZE_16BITS,
							transmit, 
							DMA_SIZE_16BITS,
							flags
						);

		if (callback) {
			dma_attach_interrupt(spiDmaDev, spiTxDmaChannel, dmahandler);
			dma_set_priority(spiDmaDev, spiTxDmaChannel, DMA_PRIORITY_LOW);
		}

		#ifdef ASYNC_BUFFERED
		if (length>65535) {
			dma_set_num_transfers(spiDmaDev, spiTxDmaChannel, 65535);
			if (minc) transmit += 65535;
			length -= 65535;
		} else  {
			dma_set_num_transfers(spiDmaDev, spiTxDmaChannel, length);
			length = 0;
		}
		#else
		dma_set_num_transfers(spiDmaDev, spiTxDmaChannel, length);
		length = 0;
		#endif
		dma_enable(spiDmaDev, spiTxDmaChannel);// enable transmit
	}
	
	void start(uint16_t* _buffer, uint32_t _length, bool _minc) {

		transmit = _buffer;
		length = _length;
		minc = _minc;

		#ifdef DEBUG
		char buffer[40];
		sprintf(buffer,"(SPI) start L:%u M:%u", length,minc);
		Serial.println(buffer);
		#endif
	
		#ifdef LED
		digitalWrite(pinLed, 0);
		#endif

		dma_clear_isr_bits(spiDmaDev, spiTxDmaChannel);
		_start();
	}
	
	void clear(void) {
		dma_clear_isr_bits(spiDmaDev, spiTxDmaChannel);
		dma_disable(spiDmaDev, spiTxDmaChannel);
		spi_tx_dma_disable(spi_d);
	}
	
	void waitSPI(void) {
		while (spi_is_tx_empty(spi_d) == 0); // "5. Wait until TXE=1 ..."
		while (spi_is_busy(spi_d) != 0); // "... and then wait until BSY=0 before disabling the SPI." 
		if (spi_is_rx_nonempty(spi_d) != 0){; // "4. Wait until RXNE=1 ..."
			uint8 x = spi_rx_reg(spi_d); // "... and read the last received data."
		}
	}
	
	void wait_ready() {
		#ifdef ASYNC_BUFFERED
		do {
			waitSPI();
		} while (length);
		#else
		waitSPI();
		#endif
	}

	void notify(void) {
		
		#ifdef ASYNC_BUFFERED
		if (length) {
			_start();
			return ;
		}
		#endif
	
		if (callback) {
			callback(callbackData);
			callback = NULL;
		}

		#ifdef LED
		digitalWrite(pinLed, 1);
		#endif
	}
	
	void wait(void) {
		waitSPI();
		clear();
		notify();
	}

	inline void write(uint16_t data) {
		waitSPI();
		spi_tx_reg(spi_d, data);
		waitSPI();
	}
	
	inline void write(uint8_t data) {
		waitSPI();
		setDataSize_8bits();
		spi_tx_reg(spi_d, data);
		waitSPI();
		setDataSize_16bits();
	}
};

SPIIOSettings _settings[BOARD_NR_SPI];

void dmaHandler0(void) {
	_settings[0].wait();
}
void dmaHandler1(void) {
	_settings[1].wait();
}
void dmaHandler2(void) {
	_settings[2].wait();
}

#define _wait() { \
	_settings[0].wait_ready(); \
}
#define _ready()

SPIIO::SPIIO() {
	//
	_settings[0].config(SPI1,DMA1,DMA_CH3,DMA_CH2,dmaHandler0);
	_settings[1].config(SPI2,DMA1,DMA_CH5,DMA_CH4,dmaHandler1);
	#if BOARD_NR_SPI >= 3
	_settings[2].config(SPI1,DMA2,DMA_CH2,DMA_CH1,dmaHandler2);
	#endif	
}

void SPIIO::init() {
	_settings[0].init();
	_ready();
}
void SPIIO::begin() {
	_settings[0].begin();
	_settings[0].wait_ready();
}
void SPIIO::end() {
	_settings[0].end();
}
bool SPIIO::ready(void) {
	return 1;
}
void SPIIO::write(uint8_t u) {
	_settings[0].write(u);
	_ready();
}
void SPIIO::write_uint(uint16_t u) {
	_settings[0].write(u);
	_ready();
}

void SPIIO::write_buffer_uint(uint16_t* buffer, uint32_t len, bool inc) {
	_wait();
	
	if (len>65535) {
	  _dmaSend(buffer,65535,inc);
	  len -= 65535;
	  _wait();
	}
	_dmaSend(buffer,len,inc);
	_ready();
}
void SPIIO::write_buffer_uint(uint16_t* buffer, uint32_t len, bool inc, TFTIOCallback callback, void * callbackData) {
	_wait();

	#ifdef USE_ASYNC
	
	#ifndef ASYNC_BUFFERED
	if (len>65535) {
	  _dmaSend(buffer,65535,inc);
	  len -= 65535;
	  _wait();
	}
	#endif
	
	_dmaAsyncSend(buffer,len,inc,callback,callbackData);
	#else
	write_buffer_uint(buffer,len,inc);
	_settings[0].setCallback(callback,callbackData);
	_settings[0].notify();
	#endif
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

uint8 SPIIO::_dmaSend(uint16_t *transmitBuf, uint16_t length, bool minc) {
	
	if (length == 0) return 0;

	// to do retreive the SPI number
	SPIIOSettings* _currentSetting = &_settings[0];
	//
	_currentSetting->start(transmitBuf,length,minc);
	_currentSetting->wait();

    return 0;
}

uint8 SPIIO::_dmaAsyncSend(uint16_t *transmitBuf, uint32_t length, bool minc, TFTIOCallback callback, void * callbackData) {
	
	if (length == 0) return 0;

	// to do retreive the SPI number
	SPIIOSettings* _currentSetting = &_settings[0];
	
	// for the isrHandler to call the callback
	_currentSetting->setCallback(callback,callbackData);
	_currentSetting->start(transmitBuf,length,minc);

    return 0;
}
