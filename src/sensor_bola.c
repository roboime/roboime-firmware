#include "stm32f4xx_conf.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "sensor_bola.h"
#include "config.h"

#define SENSOR_BOLA_0_PERIPH RCC_AHB1Periph_GPIOC //
#define SENSOR_BOLA_1_PERIPH RCC_AHB1Periph_GPIOC //
#define SENSOR_BOLA_0_PORT GPIOC
#define SENSOR_BOLA_1_PORT GPIOC
#define SENSOR_BOLA_0_PIN GPIO_Pin_2
#define SENSOR_BOLA_1_PIN GPIO_Pin_13

void sensor_bola_init(){
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(SENSOR_BOLA_0_PERIPH,ENABLE); //mudado de RCC_APB2PeriphClockCmd para RCC_AHB1PeriphClockCmd
	RCC_AHB1PeriphClockCmd(SENSOR_BOLA_1_PERIPH,ENABLE); //mudado de RCC_APB2PeriphClockCmd para RCC_AHB1PeriphClockCmd

	//GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
#ifndef CHUTE_BARRO
	GPIO_InitStructure.GPIO_Pin=SENSOR_BOLA_0_PIN;
	GPIO_Init(SENSOR_BOLA_0_PORT, &GPIO_InitStructure);
#endif
	GPIO_InitStructure.GPIO_Pin=SENSOR_BOLA_1_PIN;
	GPIO_Init(SENSOR_BOLA_1_PORT, &GPIO_InitStructure);


}
u8 sensor_bola(u8 sensor){
	switch(sensor){
	case 0:
		return !GPIO_ReadInputDataBit(SENSOR_BOLA_0_PORT, SENSOR_BOLA_0_PIN);
	case 1:
		return !GPIO_ReadInputDataBit(SENSOR_BOLA_1_PORT, SENSOR_BOLA_1_PIN);
	default:
		return 0;
	}
}
