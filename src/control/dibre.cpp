/*
 * dibre.cpp
 *
 *  Created on: Aug 24, 2016
 *      Author: OniasC
 */
#include "Pwm.h"
#include "dibre.h"
dibre::dibre()
{
	Drible_Pwm = new Pwm(GPIOE, GPIO_Pin_5, TIM9, GPIO_PinSource5, GPIO_AF_TIM9, 1, false);//ok
	Drible_Pwm->set_DutyCycle(0);
	//Drible_Pwm = new Pwm(GPIOB, GPIO_Pin_15, TIM12, GPIO_PinSource15, GPIO_AF_TIM12, 2, false);
	//Pwm oi;
	//Drible_Pwm = &oi;
	//o new substitui os comandos acima e ainda torna variavel "global"
}
void dibre::Set_Vel(uint16_t value){
	Drible_Pwm->set_DutyCycle(value);
}




