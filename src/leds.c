#include "stm32f4xx_conf.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

#define LED_0_PERIPH RCC_APB2Periph_GPIOD
#define LED_1_PERIPH RCC_APB2Periph_GPIOC

#define LED_0_PORT GPIOD
#define LED_1_PORT GPIOC

#define LED_0_PIN GPIO_Pin_2
#define LED_1_PIN GPIO_Pin_12


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

	RCC_APB2PeriphClockCmd(LED_0_PERIPH | LED_1_PERIPH,ENABLE);

//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin=LED_0_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_Init(LED_0_PORT,&GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin=LED_1_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;
	GPIO_Init(LED_1_PORT,&GPIO_InitStructure);

//	Led_Pwr_on();
}

