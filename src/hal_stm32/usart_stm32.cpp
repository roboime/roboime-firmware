#include <stm32f4xx_rcc.h>
#include <stm32f4xx_usart.h>
#include <misc.h>
//#include "stdio.h"

#include <hal_stm32/usart_stm32.h>
#include <utils/time_functions.h>

USART_STM32::USART_STM32(USART_TypeDef *usart, uint32_t USART_BaudRate, uint16_t USART_WordLength, uint16_t USART_StopBits, uint16_t USART_Parity, uint16_t USART_Mode, uint16_t USART_HardwareFlowControl, uint8_t HalfDuplex):
	USART_STM32(usart, usart, USART_BaudRate, USART_WordLength, USART_StopBits, USART_Parity, USART_Mode, USART_HardwareFlowControl, HalfDuplex)
{
}

USART_STM32::USART_STM32(USART_TypeDef *usarttx, USART_TypeDef *usartrx, uint32_t USART_BaudRate, uint16_t USART_WordLength, uint16_t USART_StopBits, uint16_t USART_Parity, uint16_t USART_Mode, uint16_t USART_HardwareFlowControl, uint8_t HalfDuplex):
	_usarttx(usarttx),
	_usartrx(usartrx)
{
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate=USART_BaudRate;
	USART_InitStructure.USART_WordLength=USART_WordLength;
	USART_InitStructure.USART_StopBits=USART_StopBits;
	USART_InitStructure.USART_Parity=USART_Parity;
	USART_InitStructure.USART_Mode=USART_Mode;
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl;
	Config(usarttx, usartrx, &USART_InitStructure, HalfDuplex);
}

USART_STM32::USART_STM32(USART_TypeDef *usarttx, USART_TypeDef *usartrx, USART_InitTypeDef *USART_InitStructure, uint8_t HalfDuplex):
	_usarttx(usarttx),
	_usartrx(usartrx)
{
	Config(usarttx, usartrx, USART_InitStructure, HalfDuplex);
}

void USART_STM32::Config(USART_TypeDef *usarttx, USART_TypeDef *usartrx, USART_InitTypeDef *USART_InitStructure, uint8_t HalfDuplex)
{
	_usarttx=usarttx;
	_usartrx=usartrx;
	if(usarttx==0 && usartrx==0){
		return;
	}
	RCC_ON(_usarttx);
	RCC_ON(_usartrx);
	uint16_t USART_Mode=USART_InitStructure->USART_Mode;
	if(_usarttx!=_usartrx){
		if(_usarttx){
			USART_InitStructure->USART_Mode=USART_Mode & USART_Mode_Tx;
			USART_Init(usarttx, USART_InitStructure);
			USART_HalfDuplexCmd(usarttx, HalfDuplex?ENABLE:DISABLE);
			USART_Cmd(usarttx, ENABLE);
		}
		if(_usartrx){
			USART_InitStructure->USART_Mode=USART_Mode & USART_Mode_Rx;
			USART_Init(usartrx, USART_InitStructure);
			USART_HalfDuplexCmd(usartrx, HalfDuplex?ENABLE:DISABLE);
			USART_Cmd(usartrx, ENABLE);
		}
	} else {
		USART_InitStructure->USART_Mode=USART_Mode;
		USART_Init(usartrx, USART_InitStructure);
		USART_HalfDuplexCmd(usartrx, HalfDuplex?ENABLE:DISABLE);
		USART_Cmd(usartrx, ENABLE);
	}
}


void USART_STM32::RCC_ON(USART_TypeDef *usart)
{
	if(usart==USART1){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	} else if(usart==USART2){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	} else if(usart==USART3){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	} else if(usart==UART4){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	} else if(usart==UART5){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);
	} else if(usart==USART6){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
	} else if(usart==UART7){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART7, ENABLE);
	} else if(usart==UART8){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART8, ENABLE);
	}
}

uint8_t USART_STM32::ReadByte(){
	uint32_t now=GetLocalTime();
	while(!USART_GetFlagStatus(_usartrx, USART_FLAG_RXNE)){
		if((GetLocalTime()-now)>500){
			return 0;
		}
	}
	return USART_ReceiveData(_usartrx);
}
uint8_t USART_STM32::ReadBuffer(uint8_t *buffer, uint16_t size){
	return 0;
}
void USART_STM32::WriteByte(uint8_t byte){
	while(!USART_GetFlagStatus(_usarttx, USART_FLAG_TXE));
	USART_SendData(_usarttx, byte);
}

void USART_STM32::WriteByteNonBlock(uint8_t byte) {
	USART_SendData(_usarttx, byte);
}

void USART_STM32::SendBreak() {
	USART_SendBreak(_usarttx);
}

void USART_STM32::ClearRx() {
	USART_GetFlagStatus(_usartrx, USART_FLAG_FE);
	USART_ReceiveData(_usartrx);
}

uint8_t USART_STM32::Idle() {
	return USART_GetFlagStatus(_usarttx, USART_FLAG_TC)?1:0;
}

void USART_STM32::HalfDuplex(uint8_t newstate) {
	USART_HalfDuplexCmd(_usarttx, (FunctionalState)newstate);
}

uint8_t USART_STM32::DataAvailable() {
	return USART_GetFlagStatus(_usartrx, USART_FLAG_RXNE);
}
