#include <hal_stm32/i2s_stm32.h>
#include <stm32f4xx_rcc.h>

I2S_STM32::I2S_STM32(SPI_TypeDef* spi, uint32_t freq, uint16_t transfersize, DMA_Stream_TypeDef *DMA_Stream):
	I2S(freq, transfersize),
	_spi(spi),
	_DMA_Stream(DMA_Stream),
	_NVIC_IRQChannel(0)
{
    if(_spi==SPI1){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	} else if(_spi==SPI2){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	} else if(_spi==SPI3){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
	} else if(_spi==SPI4){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI4, ENABLE);
	} else if(_spi==SPI5){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI5, ENABLE);
	} else if(_spi==SPI6){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI6, ENABLE);
	} else {
		return;
	}
    SPI_I2S_DeInit(_spi);

	I2S_InitTypeDef I2S_InitStructure;

    I2S_InitStructure.I2S_AudioFreq = _freq;
    I2S_InitStructure.I2S_Standard = I2S_Standard_PCMShort;
    I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16bextended;
    I2S_InitStructure.I2S_CPOL = I2S_CPOL_Low;
    I2S_InitStructure.I2S_Mode = I2S_Mode_MasterTx;
    I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Enable;

    I2S_Init(_spi, &I2S_InitStructure);

    if(transfersize && _DMA_Stream){
    	DMA_InitTypeDef DMA_InitStructure;
		DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(spi->DR));
		DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)_buffer1;
		DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
		DMA_InitStructure.DMA_BufferSize = transfersize;
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
		DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
		DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
		DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
		DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
		DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
		DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;


		if(_DMA_Stream==DMA1_Stream4){
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
			DMA_InitStructure.DMA_Channel=DMA_Channel_0;
			_NVIC_IRQChannel=DMA1_Stream4_IRQn;
		} else if(_DMA_Stream==DMA1_Stream5){
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
			DMA_InitStructure.DMA_Channel=DMA_Channel_0;
			_NVIC_IRQChannel=DMA1_Stream5_IRQn;
		} else if(_DMA_Stream==DMA1_Stream7){
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
			DMA_InitStructure.DMA_Channel=DMA_Channel_0;
			_NVIC_IRQChannel=DMA1_Stream7_IRQn;
		} else if(_DMA_Stream==DMA2_Stream1){
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
			DMA_InitStructure.DMA_Channel=DMA_Channel_4;
			_NVIC_IRQChannel=DMA2_Stream1_IRQn;
		} else if(_DMA_Stream==DMA2_Stream3){
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
			DMA_InitStructure.DMA_Channel=DMA_Channel_3;
			_NVIC_IRQChannel=DMA2_Stream3_IRQn;
		} else if(_DMA_Stream==DMA2_Stream4){
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
			DMA_InitStructure.DMA_Channel=(_spi==SPI4)?DMA_Channel_5:DMA_Channel_2;
			_NVIC_IRQChannel=DMA2_Stream4_IRQn;
		} else if(_DMA_Stream==DMA2_Stream5){
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
			DMA_InitStructure.DMA_Channel=(_spi==SPI1)?DMA_Channel_3:DMA_Channel_1;
			_NVIC_IRQChannel=DMA2_Stream5_IRQn;
		} else if(_DMA_Stream==DMA2_Stream6){
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
			DMA_InitStructure.DMA_Channel=DMA_Channel_7;
			_NVIC_IRQChannel=DMA2_Stream6_IRQn;
		}

		DMA_Cmd(_DMA_Stream, DISABLE);
		DMA_DeInit(_DMA_Stream);
		DMA_Init(_DMA_Stream, &DMA_InitStructure);
		DMA_DoubleBufferModeConfig(_DMA_Stream, (uint32_t)_buffer2, DMA_Memory_0);
		DMA_DoubleBufferModeCmd(_DMA_Stream, ENABLE);

		DMA_ITConfig(_DMA_Stream, DMA_IT_HT, ENABLE);

		SPI_I2S_DMACmd(_spi, SPI_I2S_DMAReq_Tx,  ENABLE);
		DMA_Cmd(_DMA_Stream, ENABLE);
    }
    I2S_Cmd(_spi, ENABLE);
}

int16_t* I2S_STM32::GetBuffer() {
	if(_DMA_Stream)
		return (int16_t*)((DMA_GetCurrentMemoryTarget(_DMA_Stream)==DMA_Memory_0)?_DMA_Stream->M1AR:_DMA_Stream->M0AR);
	return 0;
}

void I2S_STM32::Start() {
    I2S_Cmd(_spi, ENABLE);
}

void I2S_STM32::Stop() {
    I2S_Cmd(_spi, DISABLE);
}

uint8_t I2S_STM32::GetIRQChannel() {
	return _NVIC_IRQChannel;
}
