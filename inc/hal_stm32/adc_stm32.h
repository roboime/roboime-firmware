#pragma once
#include <stm32f4xx_rcc.h>
#include <stm32f4xx_adc.h>

#include <hal/adc.h>

class ADC_STM32:public ADC{
public:
	ADC_STM32(uint32_t frequency,  uint16_t transfersize, ADC_TypeDef *adc, uint8_t ADC_Channel, uint8_t ADC_SampleTime=ADC_SampleTime_56Cycles);
	~ADC_STM32();
	void Init(ADC_TypeDef *adc, uint8_t ADC_Channel, uint8_t ADC_SampleTime, ADC_InitTypeDef *ADC_InitStructure);
	uint16_t* GetBuffer();
	void Start();
	void Stop();
	uint8_t GetIRQChannel();
protected:
	ADC_TypeDef *_adc;
	DMA_Stream_TypeDef *_DMA_Stream;
	TIM_TypeDef *_tim;
	uint8_t _NVIC_IRQChannel;
};
