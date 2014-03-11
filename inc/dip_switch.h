#ifndef DIP_SWITCH_H
#define DIP_SWITCH_H

#ifndef U8_U32_S16
	#define U8_U32_S16
	#include "stm32f4xx.h"
#endif


void dip_switch_init();
u8 get_robot_id();

#endif
