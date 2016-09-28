#pragma once

#include <hal/onewire.h>
#include <hal/io_pin.h>


class DS2413
{
public:
	DS2413(ONEWIRE &onewire);
	uint8_t Read();
	uint8_t SelfTest();
protected:
	ONEWIRE *_onewire;
	int8_t Serial[8];
};



#define DS2413_PIO_ACEES_READ				0xF5
#define DS2413_PIO_ACEES_Write				0x5A

#define DS2413_CMD_READ_ROM					0x33
#define DS2413_CMD_MATCH_ROM				0x55
#define DS2413_CMD_SEARCH_ROM				0xF0
#define DS2413_CMD_SKIP_ROM					0xCC
#define DS2413_CMD_RESUME					0xA5
#define DS2413_CMD_OVERD_SKIP_ROM			0x3C
#define DS2413_CMD_OVERD_MATCH_ROM			0X69
