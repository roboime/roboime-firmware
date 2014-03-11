
#include "stm32f4xx_conf.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "dip_switch.h"

#define dip_switch_gpio GPIOB
#define dip_switch_pin_1 GPIO_Pin_15
#define dip_switch_pin_2 GPIO_Pin_13
#define dip_switch_pin_3 GPIO_Pin_11
#define dip_switch_rcc RCC_AHB1Periph_GPIOB



void dip_switch_init(){
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(dip_switch_rcc,ENABLE);


	//GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin=dip_switch_pin_1 | dip_switch_pin_2 | dip_switch_pin_3;
	GPIO_Init(dip_switch_gpio, &GPIO_InitStructure);


}


u8 get_robot_id(){

	int a=0,b=0,c=0;
	a=!GPIO_ReadInputDataBit(dip_switch_gpio, dip_switch_pin_1);
	b=!GPIO_ReadInputDataBit(dip_switch_gpio, dip_switch_pin_2);
	c=!GPIO_ReadInputDataBit(dip_switch_gpio, dip_switch_pin_3);
	u8 d=a+2*b+4*c;
	return d;

}
