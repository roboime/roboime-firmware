#include "stm32f4xx_conf.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_dma.h"
#include "misc.h"
#include "adc.h"
#include "config.h"

#define NR_CANAIS 7
#define ADC1_DR_Address ((uint32_t)0x4001244C)

__IO uint32_t ADC_Conversao[NR_CANAIS];


/*
 * 0 Canal 4  - Corrente M3
 * 1 Canal 5  - Corrente M1
 * 2 Canal 10 - V+_DIV
 * 3 Canal 11 - VCHUTE_DIV
 * 4 Canal 12 - SENSOR_BOLA
 * 5 Canal 14 - Corrente M0
 * 6 Canal 15 - Corrente M2
 */

void adc_init(){
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_DMA2, ENABLE);


	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_4 | GPIO_Pin_5;

	GPIO_Init(GPIOC,&GPIO_InitStructure);

	DMA_DeInit(DMA2_Stream0);

	////Checking if the DMA is disable to use the function DMA_Init()
	//if(DMA_GetCmdStatus(DMA2_Stream0) == ENABLE){
//		DMA_Cmd(DMA2_Stream0, DISABLE);
//		while(DMA_GetCmdStatus(DMA2_Stream0) == ENABLE);
//	}





	DMA_InitStructure.DMA_Channel = DMA_Channel_0 ;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1->DR);
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADC_Conversao;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory; //DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = NR_CANAIS;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord ;
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

/* ADC Init ****************************************************************/
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//!< Configures the ADC resolution dual mode.
	//ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; See ref. sec 11.3.5
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//!< Select the external trigger edge and enable the trigger of a regular group. */
	//ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None;//ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	//ADC_InitStructure.ADC_NbrOfChannel = NR_CANAIS;
	ADC_InitStructure.ADC_NbrOfConversion = NR_CANAIS;  /*!< Specifies the number of ADC conversions
	                                               that will be done using the sequencer for
	                                               regular channel group.
	                                               This parameter must range from 1 to 16. */
	ADC_Init(ADC1, &ADC_InitStructure);

/* ADC2 regular channels configuration */
	//changed from ADC_SampleTime_55Cycles5 to ADC_SampleTime_56Cycles
	ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 1, ADC_SampleTime_56Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 2, ADC_SampleTime_56Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 3, ADC_SampleTime_56Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 4, ADC_SampleTime_56Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 5, ADC_SampleTime_56Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 6, ADC_SampleTime_56Cycles);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 7, ADC_SampleTime_56Cycles);

	/* Enable DMA request after last transfer (Single-ADC mode) */
	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

	/* Enable ADC1 DMA */
	ADC_DMACmd(ADC1, ENABLE);

	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);

	/* Enable ADC1 reset calibration register */
	//ADC_ResetCalibration(ADC1);
	/* Check the end of ADC1 reset calibration register */
	//while(ADC_GetResetCalibrationStatus(ADC1));

	/* Start ADC1 calibration */
	//ADC_StartCalibration(ADC1);
	/* Check the end of ADC1 calibration */
	//while(ADC_GetCalibrationStatus(ADC1));

	/* Start ADC1 Software Conversion */
	//ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	ADC_SoftwareStartConv(ADC1);

}

inline u16 adc_getConversion(u8 index){
	u16 c;
	c=ADC_Conversao[index];
	return c;
}
