/*
 * Pwm.cpp
 *
 *  Created on: Mar 12, 2016
 *      Author: lenovoi7
 */

#include "Pwm.h"
Pwm::Pwm(GPIO_TypeDef* Port, uint32_t Pin, TIM_TypeDef * Tim, uint8_t Af_Pin, uint8_t Af, uint8_t Channel, bool nState)
{
	if(Port == GPIOA)
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	if(Port == GPIOB)
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	if(Port == GPIOC)
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	if(Port == GPIOD)
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	if(Port == GPIOE)
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	if(Tim == TIM1)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	if(Tim == TIM8)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
	if(Tim == TIM12)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;

	GPIO_InitStructure.GPIO_Pin =  Pin;
	GPIO_Init(Port, &GPIO_InitStructure); //onias
	GPIO_PinAFConfig(Port, Af_Pin, Af);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Prescaler=(SystemCoreClock/20000000)-1;
	TIM_TimeBaseStructure.TIM_Period=168000000/168000;

	TIM_TimeBaseInit(Tim, &TIM_TimeBaseStructure);
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	if(!nState){
		TIM_OCInitStructure.TIM_OutputState  = TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
		TIM_OCInitStructure.TIM_Pulse        = 0;
		TIM_OCInitStructure.TIM_OCPolarity   = TIM_OCPolarity_Low;
		TIM_OCInitStructure.TIM_OCIdleState  = TIM_OCIdleState_Set;
	}
	else{
	    TIM_OCInitStructure.TIM_OutputState  = TIM_OutputState_Disable;
	    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	    TIM_OCInitStructure.TIM_Pulse        = 0;
	    TIM_OCInitStructure.TIM_OCNPolarity  = TIM_OCNPolarity_Low;
        TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Set;
	}
	if(Channel==1){
		TIM_OC1Init(Tim, &TIM_OCInitStructure);
		TIM_OC1PreloadConfig(Tim, TIM_OCPreload_Enable);
	}
	if(Channel==2){
		TIM_OC2Init(Tim, &TIM_OCInitStructure);
		TIM_OC2PreloadConfig(Tim, TIM_OCPreload_Enable);
	}
	if(Channel==3){
		TIM_OC3Init(Tim, &TIM_OCInitStructure);
		TIM_OC3PreloadConfig(Tim, TIM_OCPreload_Enable);
	}
	if(Channel==4){
		TIM_OC4Init(Tim, &TIM_OCInitStructure);
		TIM_OC4PreloadConfig(Tim, TIM_OCPreload_Enable);
	}

	TIM_ARRPreloadConfig(Tim, ENABLE);

	TIM_Cmd(Tim, ENABLE);
	TIM_CtrlPWMOutputs(Tim,ENABLE);
	PWM_Tim = Tim;
	PWM_Channel = Channel;
};

void Pwm::set_DutyCycle(uint16_t duty_cycle1){
	if(PWM_Channel == 1)
		TIM_SetCompare1(PWM_Tim, duty_cycle1);
	if(PWM_Channel == 2)
		TIM_SetCompare2(PWM_Tim, duty_cycle1);
	if(PWM_Channel == 3)
		TIM_SetCompare3(PWM_Tim, duty_cycle1);
	if(PWM_Channel == 4)
		TIM_SetCompare4(PWM_Tim, duty_cycle1);
	return;
};
