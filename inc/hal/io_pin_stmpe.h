#pragma once

#include <hal/io_pin.h>
#include <hal/stmpe.h>

class IO_Pin_STMPE: public IO_Pin{
public:
	IO_Pin_STMPE(IO_Pin_Mode mode, STMPE &stmpe, uint8_t pins);
	uint8_t Read();
	void Write(uint8_t value);
	void Set();
	void Reset();
	void Interrupt(uint8_t newstate);

protected:
	void Config();
	uint8_t CheckConfig();
	STMPE *_stmpe;
	uint8_t _pins;
	uint8_t _configured;
};
