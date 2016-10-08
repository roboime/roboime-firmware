/*
 * Enconder.cpp
 *
 *  Created on: Mar 12, 2016
 *      Author: lenovoi7
 */

#include <Encoder.h>

Encoder::Encoder( GPIO_TypeDef* Port1, GPIO_TypeDef* Port2,
	uint32_t Pin1, uint32_t Pin2,
	TIM_TypeDef * Tim, uint8_t Af_Pin1, uint8_t Af_Pin2,
	uint8_t Af)
{

	if((Port1 == GPIOA)||(Port2 == GPIOA))
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	if((Port1 == GPIOB)||(Port2 == GPIOB))
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	if((Port1 == GPIOC)||(Port2 == GPIOC))
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	if((Port1 == GPIOD)||(Port2 == GPIOD))
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	if((Port1 == GPIOE)||(Port2 == GPIOE))
			RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	if(Tim == TIM2)
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	if(Tim == TIM3)
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	if(Tim == TIM4)
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	if(Tim == TIM5)
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;

	GPIO_InitStructure.GPIO_Pin = Pin1;
	GPIO_Init(Port1, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = Pin2;
	GPIO_Init(Port2, &GPIO_InitStructure);

	GPIO_PinAFConfig(Port1, Af_Pin1, Af);
	GPIO_PinAFConfig(Port2, Af_Pin2, Af);

	TIM_EncoderInterfaceConfig(Tim, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_Cmd(Tim, ENABLE);

	TIM_SetCounter(Tim, (uint32_t) 0);
	Encoder_Tim = Tim;
};

uint32_t Encoder::get_position(){
	return TIM_GetCounter(Encoder_Tim);
};
void Encoder::set_position(uint32_t pos){
	TIM_SetCounter(Encoder_Tim, pos);
};
