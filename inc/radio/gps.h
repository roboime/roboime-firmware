#pragma once
#include <radio/tpp_peripheral.h>
#include <hal/usart.h>
#include <hal/io_pin.h>

class NMEA{
};

class GPS: public TPP_PERIPHERAL{
public:
	GPS(USART &usart, IO_Pin &resetpin=*(IO_Pin*)0, IO_Pin &vddpin=*(IO_Pin*)0);
	void Init();
	void Config();
	uint8_t SelfTest();
	void Reset(uint8_t State);
	void VDD(uint8_t State);
	void DataCallback(uint8_t *data, uint16_t size);
	uint8_t ReadByte();
	uint8_t DataAvailable();
protected:
	USART *_usart;
	IO_Pin *_resetpin;
	IO_Pin *_vddpin;
	//const GPS_Peripheral_t *GPS_s;
};
