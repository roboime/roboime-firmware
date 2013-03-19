#ifndef ADC_H
#define ADC_H

#ifndef U8_U32_S16
	#define U8_U32_S16
	#include "stm32f4xx.h"
#endif

void adc_init();
inline u16 adc_getConversion(u8 index);

#endif
