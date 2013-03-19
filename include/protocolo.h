#ifndef PROTOCOLO_H
#define PROTOCOLO_H

#ifndef U8_U32_S16
	#define U8_U32_S16
	#include "stm32f4xx.h"
#endif

void protocolo_transmitir(u8 type, u8 tam, u8 source, u8 dest, char *buf  );
void protocolo_poll();
void protocolo_init();
unsigned int protocolo_ultima_recepcao();
unsigned char protocolo_get_robo_id();


#endif
