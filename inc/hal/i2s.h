#pragma once

#include <inttypes.h>
#include <stdlib.h>

class I2S{
public:
	I2S(uint32_t freq, uint16_t transfersize):
		_freq(freq),
		_transfersize(transfersize)
	{
    	_buffer1=new int16_t[_transfersize];
    	_buffer2=new int16_t[_transfersize];
	}
	virtual int16_t* GetBuffer()=0;
	uint16_t GetBufferSize(){return _transfersize;}
	virtual void Start()=0;
	virtual void Stop()=0;

protected:
	uint32_t _freq;
	uint16_t _transfersize;
	int16_t* _buffer1;
	int16_t* _buffer2;
};
