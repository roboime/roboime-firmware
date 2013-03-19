#ifndef MOTORES_H
#define MOTORES_H

#ifndef U8_U32_S16
	#define U8_U32_S16
	#include "stm32f4xx.h"
#endif

u8 motor_parar(u8 motor);
u8 motor_frear(u8 motor);
u8 motor_tensao(u8 motor, s16 valor);
u16 motor_get_posicao(u8 motor);
s16 motor_get_tensao(u8 motor);

void motor_inicializar();
void motores_amostrar();
u8 motor_velocidade(u8 motor, float vel);
float motor_get_velocidade(u8 motor);

#endif
