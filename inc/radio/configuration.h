#pragma once

#include <inttypes.h>
#include <initializer_list>

#include <radio/buttons.h>
#include <hal/modem.h>

class RadioNet{
public:

	uint32_t TXFrequency;
	uint32_t RXFrequency;
	MODEM_POWER Power;
	MODEM_WAVEFORM Waveform;
	RadioNet(uint32_t txfrequency,  uint32_t rxfrequency, MODEM_POWER power, MODEM_WAVEFORM waveform);
	//RadioNet(){}
};


class Configuration{
public:


	RadioNet* Net[15];
	Buttons buttons;


	Configuration(std::initializer_list<RadioNet*> c);
};
