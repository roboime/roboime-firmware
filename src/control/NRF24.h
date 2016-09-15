/*
 * NRF24.h
 *
 *  Created on: Aug 17, 2016
 *      Author: lenovoi7
 */

#ifndef NRF24_H_
#define NRF24_H_

#include "stm32f4xx.h"
#include "GPIO.h"
#include "my_spi.h"


class NRF24 {
public:
	NRF24();
	int Config();
	int WriteRegister(uint8_t adress, uint8_t *new_value, int size);
	int ReadRegister(uint8_t adress, uint8_t *value, int size);
	int WritePayload(uint8_t *data, int size);
	int ReadPayload(uint8_t *data, int size);
	int FlushTx();
	int FlushRx();
	int WriteAckPayload(uint8_t *data, int size);
	int CheckPayloadWidth();
	int Test();
	int SetId(uint8_t roboId);
	int DataSent();
	int DataReady();
	int MaxRt();
	int CleanDataSent();
	int CleanDataReady();
	int CleanMaxRt();
	int RxEmpty();
	int RxFull();
	int TxEmpty();
	int TxFull();
	GPIO *NRF_CE;
	bool is_rx;
	int channel;
	int id;
	MySpi *NRF_Spi;
private:
};

#define REG_CONFIG 		0x00
#define REG_EN_AA 		0x01
#define REG_EN_RXADDR	0x02
#define REG_SETUP_AW	0x03
#define REG_SETUP_RETR	0x04
#define REG_RF_CH		0x05
#define REG_RF_SETUP	0x06
#define REG_STATUS		0x07
#define REG_OBSERVE_TX	0x08
#define REG_RPD			0x09
#define REG_ADDR_P0		0x0A
#define REG_ADDR_P1		0x0B
// ...
#define REG_TX_ADDR		0x10
#define REG_RX_PW_P0	0x11
#define REG_RX_PW_P1	0x12
// ...
#define REG_FIFO_STATUS	0x17
#define REG_DYNPD		0x1C
#define REG_FEATURE		0x1D

#endif /* NRF24_H_ */
