#pragma once
#include <inttypes.h>
#include <radio/tpp_peripheral.h>
#include <hal/io_pin.h>
#include <list>

class PTT: public TPP_PERIPHERAL{
public:
	PTT(std::list<IO_Pin*>& keys, std::list<IO_Pin*>& switches);
	void Init();
	uint8_t SelfTest();
	void Config();
	uint8_t ReadKeys();
	uint8_t ReadSwitch();
protected:
	std::list<IO_Pin*>* _keys;
	std::list<IO_Pin*>* _switches;
};
