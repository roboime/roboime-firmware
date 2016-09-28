#pragma once

#include <inttypes.h>
#include <hal/modem.h>
#include <hal/spi.h>
#include <hal/io_pin.h>
#include <utils/circularbuffer.h>
#include <initializer_list>


class NULLMODEM: public MODEM{
public:
	void InterruptCallback(){};
	void Init(){};
	void Config(){};
	uint8_t SelfTest(){return 0;};
	uint8_t TxReady(){return 1;};
	void TxPackage(uint8_t *data, uint16_t size){};
	uint16_t RxSize(){return 0;};
	uint16_t RxData(uint8_t *data, uint16_t maxsize){return 0;};
	void GeneratePoll(){};
	void SetFrequency(uint32_t freq){};
	void SetPower(uint8_t power){};
	void VCC(uint8_t state){};
	void Shutdown(uint8_t state){};
	void StartRX(uint16_t size){};
	uint8_t Busy(){return 0;};
	void ContinueRX(uint16_t size){};

protected:
};

