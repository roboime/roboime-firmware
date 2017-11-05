/*
 * switch.h
 *
 *  Created on: 1 de abr de 2017
 *      Author: OniasC
 */

#ifndef SRC_CONTROL_SWITCH_H_
#define SRC_CONTROL_SWITCH_H_

#include <hal_stm32/io_pin_stm32.h>

class Switch{
public:
	Switch(IO_Pin_STM32 sw1, IO_Pin_STM32 sw2, IO_Pin_STM32 sw3);
	uint8_t status[3];
	bool idbin[3];
	uint8_t id;
};


#endif /* SRC_CONTROL_SWITCH_H_ */
