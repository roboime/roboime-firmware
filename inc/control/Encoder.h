/*
 * Enconder.h
 *
 *  Created on: Mar 12, 2016
 *      Author: lenovoi7
 */

#include "stm32f4xx.h"


#ifndef ENCONDER_H_
#define ENCONDER_H_

class Encoder {
public:
	Encoder( GPIO_TypeDef* Port1, GPIO_TypeDef* Port2,
			uint32_t Pin1, uint32_t Pin2,
			TIM_TypeDef * Tim, uint8_t Af_Pin, uint8_t Af_Pin2,
			uint8_t Af);
	uint32_t get_position();
	void set_position(uint32_t pos);
private:
	TIM_TypeDef * Encoder_Tim;
};

#endif /* ENCONDER_H_ */
