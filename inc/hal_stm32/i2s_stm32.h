#pragma once
#include <hal/i2s.h>
#include <stm32f4xx_spi.h>
#include <stm32f4xx_dma.h>

class I2S_STM32:public I2S{
public:
	I2S_STM32(SPI_TypeDef *spi, uint32_t freq, uint16_t transfersize=0, DMA_Stream_TypeDef *DMA_Stream=0);
	int16_t* GetBuffer();
	void Start();
	void Stop();
	uint8_t GetIRQChannel();
protected:
	SPI_TypeDef *_spi;
	DMA_Stream_TypeDef *_DMA_Stream;
	uint8_t _NVIC_IRQChannel;
};
