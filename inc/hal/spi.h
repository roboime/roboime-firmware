#pragma once

#ifdef SPI
#undef SPI
#endif

class SPI
{
public:
	virtual uint8_t WriteByte(uint8_t byte)=0;
	virtual uint8_t WriteBuffer(uint8_t *buffer, uint16_t size)=0;
	virtual void Assert()=0;
	virtual void Release()=0;
	virtual uint8_t SelfTest()=0;

protected:
};
