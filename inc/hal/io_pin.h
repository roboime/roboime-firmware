#pragma once

#include "inttypes.h"

class IO_Pin {
public:
	enum IO_Pin_Mode{
		IO_Pin_Mode_IN,
		IO_Pin_Mode_OUT,
		IO_Pin_Mode_IN_OUT,
		IO_Pin_Mode_ANALOG,
		IO_Pin_Mode_SPECIAL,
	};
	enum IO_Pin_PUPD{
		IO_Pin_PUPD_NOPULL,
		IO_Pin_PUPD_UP,
		IO_Pin_PUPD_DOWN,
	};
	virtual uint8_t Read()=0;
	virtual void Write(uint8_t value)=0;
	virtual void Set()=0;
	virtual void Reset()=0;
	virtual void Interrupt(uint8_t newstate)=0;
	IO_Pin(IO_Pin_Mode mode);
protected:
	IO_Pin_Mode _mode;
};


