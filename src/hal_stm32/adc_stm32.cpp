#include <stm32f4xx_dma.h>
#include <stm32f4xx_tim.h>
#include <hal_stm32/adc_stm32.h>
#include <stdlib.h>

ADC_STM32::ADC_STM32(uint32_t frequency, uint16_t transfersize, ADC_TypeDef* adc, uint8_t ADC_Channel, uint8_t ADC_SampleTime):
	ADC(frequency, transfersize),
	_adc(adc),
	_DMA_Stream(DMA2_Stream0),
	_tim(TIM3),
	_NVIC_IRQChannel(0)
{
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Resolution=ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode=DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode=DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge=ADC_ExternalTrigConvEdge_Rising;
	ADC_InitStructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_T3_TRGO;
	ADC_InitStructure.ADC_DataAlign=ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion=1;

	Init(_adc, ADC_Channel, ADC_SampleTime,  &ADC_InitStructure);
}

void ADC_STM32::Init(ADC_TypeDef* adc, uint8_t ADC_Channel, uint8_t ADC_SampleTime, ADC_InitTypeDef* ADC_InitStructure) {
	_adc=adc;

	if(_adc==ADC1){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	} else if(_adc==ADC2){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
	} else if(_adc==ADC3){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);
	} else {
		return;
	}
	ADC_Init(_adc, ADC_InitStructure);

	RCC_ClocksTypeDef RCCClocks;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_CommonStructInit(&ADC_CommonInitStructure);
	ADC_CommonInit(&ADC_CommonInitStructure);

	//TODO Make ADC available for more than 1 channel
	ADC_RegularChannelConfig(_adc, ADC_Channel, 1, ADC_SampleTime);

	RCC_GetClocksFreq(&RCCClocks);

	//TODO Make ADC available for more than 1 timer
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	TIM_DeInit(_tim);
	TIM_SetAutoreload(_tim, RCCClocks.PCLK1_Frequency*2/_frequency);
	TIM_SelectOutputTrigger(_tim, TIM_TRGOSource_Update);

	DMA_InitTypeDef DMA_InitStructure;
	if(_DMA_Stream==DMA2_Stream0){
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
		_NVIC_IRQChannel=DMA2_Stream0_IRQn;
		if(_adc==ADC1){
			DMA_InitStructure.DMA_Channel=DMA_Channel_0;
		} else {
			DMA_InitStructure.DMA_Channel=DMA_Channel_2;
		}
	} else if(_DMA_Stream==DMA2_Stream1){
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
		DMA_InitStructure.DMA_Channel=DMA_Channel_2;
		_NVIC_IRQChannel=DMA2_Stream1_IRQn;
	} else if(_DMA_Stream==DMA2_Stream2){
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
		DMA_InitStructure.DMA_Channel=DMA_Channel_1;
		_NVIC_IRQChannel=DMA2_Stream2_IRQn;
	} else if(_DMA_Stream==DMA2_Stream3){
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
		DMA_InitStructure.DMA_Channel=DMA_Channel_1;
		_NVIC_IRQChannel=DMA2_Stream3_IRQn;
	} else if(_DMA_Stream==DMA2_Stream4){
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
		DMA_InitStructure.DMA_Channel=DMA_Channel_0;
		_NVIC_IRQChannel=DMA2_Stream4_IRQn;
	}

	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(_adc->DR));
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)_buffer1;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = _transfersize;
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
	DMA_Init(_DMA_Stream, &DMA_InitStructure);

	DMA_DoubleBufferModeConfig(_DMA_Stream, (uint32_t)_buffer2, DMA_Memory_0);
	DMA_DoubleBufferModeCmd(_DMA_Stream, ENABLE);
	DMA_Cmd(_DMA_Stream, ENABLE);
	DMA_ITConfig(_DMA_Stream, DMA_IT_HT, ENABLE);

	ADC_DMARequestAfterLastTransferCmd(_adc, ENABLE);
	ADC_DMACmd(_adc, ENABLE);
	ADC_Cmd(_adc, ENABLE);
}

uint16_t* ADC_STM32::GetBuffer() {
	if(_DMA_Stream)
		return (uint16_t*)((DMA_GetCurrentMemoryTarget(_DMA_Stream)==DMA_Memory_0)?_DMA_Stream->M1AR:_DMA_Stream->M0AR);
	return 0;
}

void ADC_STM32::Start() {
	TIM_Cmd(_tim, ENABLE);
}

void ADC_STM32::Stop() {
	TIM_Cmd(_tim, DISABLE);
}

uint8_t ADC_STM32::GetIRQChannel() {
	return _NVIC_IRQChannel;
}

ADC_STM32::~ADC_STM32() {
}
