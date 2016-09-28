#pragma once
#include <stdlib.h>
#include <inttypes.h>

#ifdef ADC
#undef ADC
#endif

class ADC{
public:
	ADC(uint32_t frequency,  uint16_t transfersize):_frequency(frequency), _transfersize(transfersize)
	{
		_buffer1=new uint16_t[_transfersize];
		_buffer2=new uint16_t[_transfersize];
	}
	virtual ~ADC(){
		delete _buffer1;
		delete _buffer2;
	}
	virtual uint16_t* GetBuffer()=0;
	uint16_t GetBufferSize(){return _transfersize;}
	virtual void Start()=0;
	virtual void Stop()=0;
protected:
	uint32_t _frequency;
	uint16_t _transfersize;
	uint16_t *_buffer1;
	uint16_t *_buffer2;
};
