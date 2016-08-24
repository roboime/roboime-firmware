/*
 * adc.h
 *
 *  Created on: Aug 22, 2016
 *      Author: OniasC
 */

#ifndef ADC_H_
#define ADC_H_

class adc{
public:
	void ADC_Config(int channel, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
	//void ADC_Config(uint8_t ADC_Channel, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
	float adc_getConversion(void);
private:
};


#endif /* ADC_H_ */
