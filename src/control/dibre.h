/*
 * dibre.h
 *
 *  Created on: Aug 24, 2016
 *      Author: OniasC
 */
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"

#ifndef DIBRE_H_
#define DIBRE_H_

class dibre{
public:
	Pwm* Drible_Pwm;
	dibre();
	void Set_Vel(uint16_t value);

};


#endif /* DIBRE_H_ */
