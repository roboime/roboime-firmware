/*
 * GPIO.cpp
 *
 *  Created on: Mar 12, 2016
 *      Author: lenovoi7
 */

#include "GPIO.h"
GPIO::GPIO(GPIO_TypeDef* Port, uint16_t Pin) {
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
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = Pin;
	GPIO_Init(Port,&GPIO_InitStructure);

	GPIO_ResetBits(Port, Pin);
	GPIO_Port = Port;
	GPIO_Pin = Pin;
}
bool GPIO::Status(){
  uint8_t status_bit = GPIO_ReadOutputDataBit(GPIO_Port, GPIO_Pin);
  if(status_bit==Bit_SET)
    return true;
  return false;
}
void GPIO::Set(){
	GPIO_SetBits(GPIO_Port, GPIO_Pin);
	return;
}
void GPIO::Reset(){
	GPIO_ResetBits(GPIO_Port, GPIO_Pin);
	return;
}

