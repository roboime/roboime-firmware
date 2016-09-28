#pragma once
#include <inttypes.h>

class USART{
public:
	virtual uint8_t DataAvailable()=0;
	virtual uint8_t ReadByte()=0;
	virtual uint8_t ReadBuffer(uint8_t *buffer, uint16_t size);
	virtual void WriteByte(uint8_t byte)=0;
	virtual void WriteBuffer(uint8_t *buffer, uint16_t size);
	virtual void WriteString(const char *buffer);
protected:

};
