/*
 * TimerTime.cpp
 *
 *  Created on: Mar 12, 2016
 *      Author: lenovoi7
 */
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "TimerTime2.h"

Timer_Time2::Timer_Time2(){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
	TIM_DeInit(TIM7);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_ClockDivision=0;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Prescaler=(SystemCoreClock);
	TIM_TimeBaseStructure.TIM_Period=1000000;
	TIM_TimeBaseInit(TIM7,&TIM_TimeBaseStructure);

	//codigo novo
	TIM_Cmd(TIM7,ENABLE);
};
uint32_t Timer_Time2::current_time(){
	//uint32_t current_time1;
	//uint32_t atual_time = TIM_GetCounter(TIM6);
	//TIM_SetCounter(TIM6, (uint32_t) 0);
	//time_elapsed1 = atual_time - this->last_time;
	//this->last_time = atual_time;
	return TIM_GetCounter(TIM7);
};


/*	codigo d'de lima
 * void Timer_Init(uint_16t freq){

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
	TIM_DeInit(TIM6);

	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_ClockDivision=0;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Prescaler=(SystemCoreClock/2/10000);
	TIM_TimeBaseStructure.TIM_Period=10000;
	TIM_TimeBaseInit(TIM6,&TIM_TimeBaseStructure);

	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM6,ENABLE);
	//TIM_Cmd(TIM6,DISABLE);


	freq_amost=freq;

	NVIC_SetPriority(TIM6_DAC_IRQn,1); //Mudado de TIM6_IRQn para TIM6_DAC_IRQn
	NVIC_EnableIRQ(TIM6_DAC_IRQn);

}*/
