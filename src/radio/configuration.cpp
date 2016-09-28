#include <radio/configuration.h>

RadioNet::RadioNet(uint32_t txfrequency,  uint32_t rxfrequency, MODEM_POWER power, MODEM_WAVEFORM waveform):
TXFrequency(txfrequency),
RXFrequency(rxfrequency),
Power(power),
Waveform(waveform)
{}

Configuration::Configuration(std::initializer_list<RadioNet*> c):
	buttons(10,0,0,0)
{
	if(c.size() <= sizeof(Net)/sizeof(Net[0])){
		std::copy(c.begin(), c.end(), Net);
	}
}


