/*
 * NRF_REGISTERS.h
 *
 *  Created on: Mar 27, 2016
 *      Author: Renan
 */

#ifndef NRF_REGISTER_MAP_H_
#define NRF_REGISTER_MAP_H_

//acessible registers of NRF
class NRF_REGISTER_MAP{
protected:
	REGISTER CONFIG;
	REGISTER EN_AA;
	REGISTER EN_RXADDR;
	REGISTER SETUP_AW;
	REGISTER SETUP_RETR;
	REGISTER RF_CH;
	REGISTER RF_SETUP;
	REGISTER STATUS;
	REGISTER OBSERVE_TX;
	REGISTER CD;
	REGISTER RX_ADDR_P0;
	REGISTER RX_ADDR_P1;
	REGISTER RX_ADDR_P2;
	REGISTER RX_ADDR_P3;
	REGISTER RX_ADDR_P4;
	REGISTER RX_ADDR_P5;
	REGISTER TX_ADDR;
	REGISTER RX_PW_P0;
	REGISTER RX_PW_P1;
	REGISTER RX_PW_P2;
	REGISTER RX_PW_P3;
	REGISTER RX_PW_P4;
	REGISTER RX_PW_P5;
	REGISTER FIFO_STATUS;
	//there are some registers between FIFO_STATUS and DYNPD, but they are reserved and ignored here
	REGISTER DYNPD;
	REGISTER FEATURE;
protected:
	NRF_REGISTER_MAP();//CONSTRUTOR, automaticamente inicializa os registradores com seus valores de reset
	virtual int W_REGISTER(uint8_t adress, uint8_t size,uint8_t* value)=0;//WORKED! but SPI seems too much fast to NRF
	virtual int R_REGISTER(uint8_t adress, uint8_t size,uint8_t* pointer)=0;//WORKED!
public:
	void REFRESH();
};
#endif /* NRF_REGISTER_MAP_H_ */
