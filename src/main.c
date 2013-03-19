#include "stm32f4xx_conf.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

#include "rtc.h"
#include "usart.h"
#include "motores.h"
#include "adc.h"
#include "timer.h"
#include "leds.h"
#include "drible.h"
#include "chute.h"
#include "sensor_bola.h"
#include "rfm12.h"
#include "protocolo.h"

s32 time_ms=0;

u8 led_bat_en = 0;
void SysTick_Handler(){
	s32 tempo = time_ms % 100;
	if(led_bat_en == 1 && tempo == 0){
		Led_Pwr_Toggle();
	}
	time_ms++;
}

void PORT_Init(){
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
			RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD |
			RCC_APB2Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

}

void wait_ms(u32 ms){
	u32 tempo_ini=time_ms;
	while((tempo_ini+ms)>time_ms);
}

void teste(){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);

//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);

	while(1){
		GPIOC->ODR=GPIO_Pin_1 | GPIO_Pin_0;
	}

}
