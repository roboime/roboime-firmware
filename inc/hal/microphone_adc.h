#pragma once
#include <radio/microphone.h>
#include <hal/adc.h>

class MICROPHONE_ADC: public MICROPHONE{

public:
	MICROPHONE_ADC(ADC &adc, VOLUME &volume=*(VOLUME*)0):MICROPHONE(volume),_adc(&adc){}
	void Start(){_adc->Start();}
	void Stop(){_adc->Stop();}
	void Poll(){}
	int16_t* GetBuffer(){ return (int16_t*)_adc->GetBuffer();}
	uint16_t GetBufferSize(){ return _adc->ADC::GetBufferSize();}
protected:
	ADC *_adc;
};
