#pragma once
#include <inttypes.h>

typedef enum {
	MODEM_POWER_MEDIUM=0,
	MODEM_POWER_HIGH=1,
	MODEM_POWER_HIGHEST=2,
	MODEM_POWER_NORMAL=3,
	MODEM_POWER_LOW=4,
	MODEM_POWER_ZERO=5,
} MODEM_POWER;

typedef enum {
	MODEM_WAVE_FORM_MINIMAL,
} MODEM_WAVEFORM;

class MODEM{
public:
	virtual uint8_t TxReady()=0;
	virtual void TxPackage(uint8_t *data, uint16_t size, uint32_t frequency, MODEM_POWER power)=0;
	virtual uint16_t RxSize()=0;
	virtual uint16_t RxData(uint8_t *data, uint16_t maxsize)=0;
	virtual void StartRX(uint16_t size, uint32_t freqHz)=0;
	virtual void ContinueRX(uint16_t size)=0;
	virtual void GeneratePoll()=0;
	virtual void SetRXFrequency(uint32_t freqHz)=0;
	virtual uint8_t Busy()=0;
	virtual void CW(uint8_t state)=0;
	virtual uint8_t GetRSSI()=0;
	virtual void Calibrate(uint32_t freq)=0;
	virtual void Recovery()=0;
};
