#pragma once

#include "stm32f4xx_usart.h"
#include <hal/usart.h>

class USART_STM32: public USART{
public:
	USART_STM32(USART_TypeDef *usarttx, USART_TypeDef *usartrx, USART_InitTypeDef *USART_InitStructure, uint8_t HalfDuplex=0);
	USART_STM32(USART_TypeDef *usart, uint32_t USART_BaudRate=9600, uint16_t USART_WordLength=USART_WordLength_8b, uint16_t USART_StopBits=USART_StopBits_1, uint16_t USART_Parity=USART_Parity_No, uint16_t USART_Mode=USART_Mode_Rx | USART_Mode_Tx, uint16_t USART_HardwareFlowControl=USART_HardwareFlowControl_None, uint8_t HalfDuplex=0);
	USART_STM32(USART_TypeDef *usarttx, USART_TypeDef *usartrx, uint32_t USART_BaudRate=9600, uint16_t USART_WordLength=USART_WordLength_8b, uint16_t USART_StopBits=USART_StopBits_1, uint16_t USART_Parity=USART_Parity_No, uint16_t USART_Mode=USART_Mode_Rx | USART_Mode_Tx, uint16_t USART_HardwareFlowControl=USART_HardwareFlowControl_None, uint8_t HalfDuplex=0);
	uint8_t ReadByte();
	uint8_t ReadBuffer(uint8_t *buffer, uint16_t size);
	void WriteByte(uint8_t byte);
	void WriteByteNonBlock(uint8_t byte);
	void SendBreak();
	void ClearRx();
	uint8_t Idle();
	void HalfDuplex(uint8_t newstate);
	uint8_t DataAvailable();
protected:
	USART_TypeDef *_usarttx;
	USART_TypeDef *_usartrx;
	void Config(USART_TypeDef *usarttx, USART_TypeDef *usartrx, USART_InitTypeDef *USART_InitStructure, uint8_t HalfDuplex);
	void RCC_ON(USART_TypeDef *usart);
};

