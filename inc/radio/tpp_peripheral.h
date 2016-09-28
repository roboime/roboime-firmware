#pragma once
#include "inttypes.h"

class TPP_PERIPHERAL{
public:
	virtual void Init()=0;
	virtual uint8_t SelfTest()=0;
	virtual void Config()=0;
};

