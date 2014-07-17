#include "stm32f4xx_conf.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_gpio.h"
#include <stdio.h>
#include "leds.h"
#include "pid.h"
#include "motores.h"


u16 freq_amost=0;

u16 Get_Freq(){
	return freq_amost;
}

void Timer_Init(u16 freq){

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
	TIM_DeInit(TIM6);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_ClockDivision=0;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Prescaler=(SystemCoreClock/2/10000);
	TIM_TimeBaseStructure.TIM_Period=10000/freq;
	TIM_TimeBaseInit(TIM6,&TIM_TimeBaseStructure);

	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);
	if(freq){
		TIM_Cmd(TIM6,ENABLE);
	} else {
		TIM_Cmd(TIM6,DISABLE);
	}

	freq_amost=freq;

	NVIC_SetPriority(TIM6_DAC_IRQn,1); //Mudado de TIM6_IRQn para TIM6_DAC_IRQn
	NVIC_EnableIRQ(TIM6_DAC_IRQn);





}






void TIM6_DAC_IRQHandler(){
	if(TIM_GetITStatus(TIM6,TIM_IT_Update)){
		TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
		motores_amostrar();



	}
}
