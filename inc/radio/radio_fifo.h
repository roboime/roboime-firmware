#pragma once

#include <inttypes.h>

class RADIO_FIFO {
public:
	RADIO_FIFO(uint8_t *buffer, uint16_t size);
	RADIO_FIFO(uint16_t size);
	~RADIO_FIFO();
	uint16_t In(uint8_t *buffer, uint16_t size);
	uint16_t In(uint8_t item);
	uint16_t Out(uint8_t *buffer, uint16_t size);
	uint8_t Out();
	uint16_t Ocupied();
	uint16_t Free();
private:
	uint8_t *Buffer;
	uint16_t Size;
	uint16_t Head;
	uint16_t Tail;
	uint8_t Allocated;
};

