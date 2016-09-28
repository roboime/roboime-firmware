#pragma once

#include <inttypes.h>
#include <hal/io_pin.h>
#include <hal_stm32/usart_stm32.h>

class ONEWIRE{
public:
	ONEWIRE(USART_STM32 &usart,IO_Pin &ONEWIRE_PULL_UP,uint32_t timeout);
	uint8_t ReadByte (void);
	void DMA_Config (void);
	void Callback();

protected:
	typedef enum {
		ONEWIRE_STATE_IDLE,
		ONEWIRE_STATE_DURING_BREAK,
		ONEWIRE_STATE_BREAK,
		ONEWIRE_STATE_AFTER_BREAK,
		ONEWIRE_STATE_TRANSMIT,
	} ONEWIRE_STATE;
	USART_STM32 *_usart;
	IO_Pin *_onewirepullup;
	uint32_t _timeout;
	void Config();
	ONEWIRE_STATE _state;
	uint8_t _value;
	uint32_t _timestamp;
};
