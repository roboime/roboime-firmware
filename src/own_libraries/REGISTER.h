/*
 * REGISTER.h
 *
 *  Created on: Mar 26, 2016
 *      Author: renan
 */
#include "stm32f4xx.h"//para incluir stdint.h

#ifndef REGISTER_H_
#define REGISTER_H_

class REGISTER{
private:
	uint8_t size;//em bytes
	uint8_t address;//conforme a product specification
public:
	//atributo
	uint8_t content[5];

	//métodos
	REGISTER();
	void set_size(uint8_t);
	uint8_t get_size();
	void set_address(uint8_t);
	uint8_t get_address();
};

#endif /* REGISTER_H_ */
