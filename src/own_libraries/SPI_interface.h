/*
 * SPI_interface.h
 *
 *  Created on: Mar 28, 2016
 *      Author: Renan
 */
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "stm32f4xx_gpio.h"
#include "own_libraries/CONFIG.h"

#ifndef SPI_INTERFACE_H_
#define SPI_INTERFACE_H_

class SPI{
private:
	//atributos
	SPI_TypeDef* NRF_SPI;		//usar como default: SPI1
	GPIO_TypeDef* NRF_CS_GPIO;	//usar como default: GPIOA
	uint16_t NRF_CS_Pin;		//usar como default: GPIO_Pin_4
protected:
	//métodos
	void ASSERT_CS(int STATE);
	int CMD(uint8_t cmd, uint8_t size,uint8_t* ptr_send,uint8_t* ptr_receive);
	int W_REGISTER(uint8_t adress, uint8_t size,uint8_t* value);//WORKED! but SPI seems too much fast to NRF
	int R_REGISTER(uint8_t adress, uint8_t size,uint8_t* pointer);//WORKED!
	uint8_t R_RX_PL_WID(void);
	int R_RX_PAYLOAD(uint8_t* pointer);
	int W_TX_PAYLOAD(uint8_t* pointer,uint8_t number);
	void FLUSH_TX(void);//DOESN'T work
	void FLUSH_RX(void);//DOESN'T work
	void ACTIVATE(void);//DOESN'T work
	uint8_t NOP(void);
public:
	SPI(SPI_TypeDef* SPI,GPIO_TypeDef* CS_GPIO,uint16_t CS_Pin);//CONSTRUTOR
};

#endif /* SPI_INTERFACE_H_ */
