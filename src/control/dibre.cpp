/*
 * dibre.cpp
 *
 *  Created on: Aug 24, 2016
 *      Author: OniasC
 */
#include "Pwm.h"
#include "dibre.h"
dibre::dibre(Pwm *pwm):
	Drible_Pwm(pwm)
{
	//Pwm oi;
	//Drible_Pwm = &oi;
	//o new substitui os comandos acima e ainda torna variavel "global"
}
void dibre::Set_Vel(uint16_t value){
	Drible_Pwm->set_DutyCycle(value);
}




