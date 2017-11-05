/*
 * Pwm.h
 *
 *  Created on: Mar 12, 2016
 *      Author: lenovoi7
 */
#include "stm32f4xx.h"

#ifndef PWM_H_
#define PWM_H_

class Pwm{
public:
	Pwm(GPIO_TypeDef* Port, uint32_t Pin, TIM_TypeDef * Tim, uint8_t Af_Pin, uint8_t Af, uint8_t Channel, bool nState);
	void set_DutyCycle(uint16_t duty_cycle1);
private:
	int8_t PWM_Channel;
	TIM_TypeDef * PWM_Tim;
};

#endif /* PWM_H_ */
