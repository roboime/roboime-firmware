#include "stm32f4xx_conf.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

#define LED_0_PERIPH RCC_AHB1Periph_GPIOD
#define LED_1_PERIPH RCC_AHB1Periph_GPIOD

#define LED_0_PORT GPIOD
#define LED_1_PORT GPIOD

#define LED_0_PIN GPIO_Pin_2
#define LED_1_PIN GPIO_Pin_1


inline void Led_Pwr_on(){
	GPIO_SetBits(LED_0_PORT, LED_0_PIN);
}
inline void Led_Pwr_off(){
	GPIO_ResetBits(LED_0_PORT, LED_0_PIN);
}

inline void Led_Status_on(){
	GPIO_SetBits(LED_1_PORT, LED_1_PIN);
}
inline void Led_Status_off(){
	GPIO_ResetBits(LED_1_PORT, LED_1_PIN);
}

inline void Led_Status_Toggle(){
	if(GPIO_ReadOutputDataBit(LED_1_PORT,LED_1_PIN)){
		GPIO_ResetBits(LED_1_PORT, LED_1_PIN);
	} else {
		GPIO_SetBits(LED_1_PORT, LED_1_PIN);
	}
}

inline void Led_Pwr_Toggle(){
	if(GPIO_ReadOutputDataBit(LED_0_PORT,LED_0_PIN)){
		GPIO_ResetBits(LED_0_PORT, LED_0_PIN);
	} else {
		GPIO_SetBits(LED_0_PORT, LED_0_PIN);
	}
}

void Leds_init(){
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(LED_0_PERIPH | LED_1_PERIPH,ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin=LED_0_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(LED_0_PORT,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin=LED_1_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(LED_1_PORT,&GPIO_InitStructure);

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD,&GPIO_InitStructure);


//	Led_Pwr_on();
}



void Led_Acender(u8 led)
{
	//1=Led Verde
	//2=Led Laranja
	//3=Led Vermelho
	//4=Led Azul
	switch(led)
	{
	case 0:
		GPIO_SetBits(GPIOD,GPIO_Pin_12);
		break;
	case 1:
		GPIO_SetBits(GPIOD,GPIO_Pin_13);
		break;
	case 2:
		GPIO_SetBits(GPIOD,GPIO_Pin_14);
		break;
	case 3:
		GPIO_SetBits(GPIOD,GPIO_Pin_15);
		break;
	}
}

void Led_Apagar(u8 led)
{
	//1=Led Verde
	//2=Led Laranja
	//3=Led Vermelho
	//4=Led Azul
	switch(led)
	{
	case 0:
		GPIO_ResetBits(GPIOD,GPIO_Pin_12);
	case 1:
		GPIO_ResetBits(GPIOD,GPIO_Pin_13);
	case 2:
		GPIO_ResetBits(GPIOD,GPIO_Pin_14);
	case 3:
		GPIO_ResetBits(GPIOD,GPIO_Pin_15);
	}
}

