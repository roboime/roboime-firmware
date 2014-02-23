#ifndef CHUTE_H
#define CHUTE_H

#ifndef U8_U32_S16
	#define U8_U32_S16
	#include "stm32f4xx.h"
#endif

void chute_init();
u32 chutar_alto(u32 tempo);
u32 chutar_baixo(u32 tempo);

#endif


