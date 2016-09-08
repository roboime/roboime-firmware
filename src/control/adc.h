/*
 * adc.h
 *
 *  Created on: Aug 22, 2016
 *      Author: Cel DeSordi
 */
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#ifndef ADC_H_
#define ADC_H_
class adc{
public:
	void ADC_Config();
	float adc_getConversion();
private:
};

/*
	   * ct tensao chute: P1 11 - PC0 ch 10
	   * s1 sensor 1: P1 7 - PC1 ch 11
	   * bat reference: P1 8 - PB0 ch 8
	   * s2 sensor 2: P1 14 - PA2 channel 2
	   */
	  /* Infinite loop */


#endif /* ADC_H_ */
