/*
 * adc3.h
 *
 *  Created on: 2 de nov de 2018
 *      Author: Hugo
 */


#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#ifndef ADC3_H_
#define ADC3_H_
class adc3{
public:
	void ADC3_Config();
	float adc3_getConversion();
	//float readSensor(int nSensor);
private:
};

/*
	   * ct tensao chute: P1 11 - PC0 ch 10
	   * s1 sensor 1: P1 7 - PC1 ch 11
	   * bat reference: P1 8 - PB0 ch 8
	   * s2 sensor 2: P1 14 - PA2 channel 2
	   */
	  /* Infinite loop */


#endif /* ADC3_H_ */
