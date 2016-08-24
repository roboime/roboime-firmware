/*
 * adc.cpp
 *
 *  Created on: Aug 22, 2016
 *      Author: OniasC
 */
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "adc.h"

__IO uint32_t ADC_Conversao;

void adc::ADC_Config(int channel, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin){
	ADC_InitTypeDef       ADC_InitStructure;
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
    DMA_InitTypeDef       DMA_InitStructure;
    GPIO_InitTypeDef      GPIO_InitStructure;

    /* Enable ADCx, DMA and GPIO clocks ****************************************/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_DMA2, ENABLE);

    /* Configure ADC3 Channelx pin as analog input ******************************/

	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin;
	GPIO_Init(GPIOx, &GPIO_InitStructure);

    /* DMA2 Stream0 channel0 configuration **************************************/

    DMA_InitStructure.DMA_Channel = DMA_Channel_0;
   	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1->DR);
   	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADC_Conversao;
   	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory; //DMA_DIR_PeripheralSRC;
   	DMA_InitStructure.DMA_BufferSize = 1;
   	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
   	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
   	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
   	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
   	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
   	//DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
   	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;          //!< Specifies if the FIFO mode or Direct mode will be used for the specified Stream.
   	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;      //!< Specifies the FIFO threshold level. Its set to full mode for no special reason. So in the future we need to analyze more carefully
    //quoting page 228: FIFO error will be generated when the stream is enabled, then the stream will be AUTOMATICALLY DISABLED."
   	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;        //!< Specifies the Burst transfer configuration for the memory transfers.
    //  DMA_InitStructure.DMA_PeripheralBurst: The burst mode is possible only if the address Increment mode is enabled. */
   	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
   	DMA_Init(DMA2_Stream0, &DMA_InitStructure);

   	/* Enable DMA1 channel1 */
   	DMA_Cmd(DMA2_Stream0, ENABLE);
   	//Waiting until the DMA Stream is enable
   	while(DMA_GetCmdStatus(DMA2_Stream0) == DISABLE);

    /* ADC Common Init **********************************************************/

   	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
   	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
   	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;//ADC_DMAAccessMode_2;//ADC_Direct_memory_access_mode_for_multi_mode
   	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
   	ADC_CommonInit(&ADC_CommonInitStructure);

    /* ADC1 Init ****************************************************************/

	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//!< Configures the ADC resolution dual mode.
	//ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; See ref. sec 11.3.5
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//!< Select the external trigger edge and enable the trigger of a regular group. */
	//ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None;//ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	//ADC_InitStructure.ADC_NbrOfChannel = NR_CANAIS;
	ADC_InitStructure.ADC_NbrOfConversion = 1;  /*!< Specifies the number of ADC conversions
		                                               that will be done using the sequencer for
		                                               regular channel group.
		                                               This parameter must range from 1 to 16. */
	ADC_Init(ADC1, &ADC_InitStructure);

	/* ADC3 regular channelx configuration **************************************/
  if(channel==10)
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_15Cycles);
  else if (channel==9)
	  ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 1, ADC_SampleTime_15Cycles);
  else if(channel==8)
	  ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_15Cycles);
  else if(channel==7)
	  ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 1, ADC_SampleTime_15Cycles);
  else if(channel==6)
	  ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 1, ADC_SampleTime_15Cycles);
  else if(channel==5)
	  ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_15Cycles);
  else if(channel==4)
	  ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_15Cycles);
  else if(channel==3)
	  ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 1, ADC_SampleTime_15Cycles);


   /* Enable DMA request after last transfer (Single-ADC mode) */
    ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

    /* Enable ADC1 DMA */
    ADC_DMACmd(ADC1, ENABLE);

    /* Enable ADC1 */
    ADC_Cmd(ADC1, ENABLE);

	ADC_SoftwareStartConv(ADC1);
}

float adc::adc_getConversion(void){
  float c;
  c = (float)ADC_Conversao*330*3250/(5280*0xFFF);
  return c;
}
