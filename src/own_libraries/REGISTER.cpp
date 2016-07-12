/*
 * REGISTER.cpp
 *
 *  Created on: Mar 26, 2016
 *      Author: Renan
 */
#include "stm32f4xx.h" //para incluir stdint.h
#include "own_libraries/REGISTER.h"

REGISTER::REGISTER(){//CONSTRUTOR, incializa o REGISTER
	size = 0;
	address = 0;
	uint8_t i;
	for(i=0;i<5;i++)
		content[i]=0;
}

void REGISTER::set_size(uint8_t _size){
	size = _size;
}

uint8_t REGISTER::get_size(){
	return size;
}

void REGISTER::set_address(uint8_t _address){
	address = _address;
}

uint8_t REGISTER::get_address(){
	return address;
}
