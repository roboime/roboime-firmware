/*
 * NRF_REGISTERS.cpp
 *
 *  Created on: Mar 27, 2016
 *      Author: Renan
 */
#include "own_libraries/CONFIG.h"
#include "own_libraries/NRF24.h"

void NRF_REGISTER_MAP::REFRESH(){
	uint8_t i=0;
	REGISTER* current_register = &(CONFIG);
	for(i=0x00;i<=0x19;i++){
		R_REGISTER(current_register->get_address(), current_register->get_size(),current_register->content);
		for(int j=0;j<0xffff;j++);
		current_register++;
	}
	return;
}//WORKED!

//inicializa objeto da classe NRF_REGISTERS, põe os valores de address e size, coloca os valores de RESET dos registradores
NRF_REGISTER_MAP::NRF_REGISTER_MAP(){
	REGISTER* tmp_pointer = &CONFIG;//percorre os registradores do objeto NRF_REGISTERS

	//inicializa os campos address
	uint8_t i=0;
	for(i=0x00;i<=0x17;i++){
		tmp_pointer->set_address(i);
		tmp_pointer++;
	}
	for(i=0x1c;i<=0x1d;i++){
		tmp_pointer->set_address(i);
		tmp_pointer++;
	}//terminou de inicializar os campos de address

	tmp_pointer = &CONFIG;// tmp_pointer tem de percorrer os registradores 3 vezes
	//inicializa os campos size
	for(i=0x00;i<=0x09;i++){
		tmp_pointer->set_size(1);
		tmp_pointer++;
	}
	for(i=0x0a;i<=0x0b;i++){
		tmp_pointer->set_size(5);
		tmp_pointer++;
	}
	for(i=0x0c;i<=0x0f;i++){
		tmp_pointer->set_size(1);
		tmp_pointer++;
	}
	for(i=0x10;i<=0x10;i++){
		tmp_pointer->set_size(5);
		tmp_pointer++;
	}
	for(i=0x11;i<=0x19;i++){
		tmp_pointer->set_size(1);
		tmp_pointer++;
	}//terminou de inicializar os campos de size

	//inicializa os campos content
	(CONFIG).content[0] = 0b1000;
	(EN_AA).content[0] = 0b111111;
	(EN_RXADDR).content[0] = 0b11;
	(SETUP_AW).content[0] = 0b11;
	(SETUP_RETR).content[0] = 0b11;
	(RF_CH).content[0] = 0b10;
	(RF_SETUP).content[0] = 0b1111;
	(STATUS).content[0] = 0b1110;
	(OBSERVE_TX).content[0] = 0b0;
	(CD).content[0] = 0b0;

	for(i=0;i<5;i++){
		(RX_ADDR_P0).content[i] = 0xe7;
	}
	for(i=0;i<5;i++){
		(RX_ADDR_P1).content[i] = 0xc2;
	}
	(RX_ADDR_P2).content[0] = 0xc3;
	(RX_ADDR_P3).content[0] = 0xc4;
	(RX_ADDR_P4).content[0] = 0xc5;
	(RX_ADDR_P5).content[0] = 0xc6;
	for(i=0;i<5;i++){
		(TX_ADDR).content[i] = 0xe7;
	}
	(RX_PW_P0).content[0] = 0x00;
	(RX_PW_P1).content[0] = 0x00;
	(RX_PW_P2).content[0] = 0x00;
	(RX_PW_P3).content[0] = 0x00;
	(RX_PW_P4).content[0] = 0x00;
	(RX_PW_P5).content[0] = 0x00;
	(FIFO_STATUS).content[0] = 0b10001;
	(DYNPD).content[0] = 0x00;
	(FEATURE).content[0] = 0x00;
	//terminou de inicializar os campos de content
}


