/*
 * GPIO.h
 *
 *  Created on: Mar 12, 2016
 *      Author: lenovoi7
 */
#include "stm32f4xx.h"

#ifndef GPIO_H_
#define GPIO_H_

class GPIO {
public:
	GPIO(GPIO_TypeDef* Port, uint16_t Pin);
	void Set();
	void Reset();
	bool Status();
private:
	GPIO_TypeDef* GPIO_Port;
	uint16_t GPIO_Pin;
};

#endif /* GPIO_H_ */
