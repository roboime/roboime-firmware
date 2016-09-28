#pragma once
#include <hal/mcp4xxx.h>
#include <hal/ad7992.h>
#include <stdio.h>
#include <utils/time_functions.h>
#include <hal/memory.h>

class RF_POWER_CTRL{
public:
	RF_POWER_CTRL(MCP4XXX& mcp4xxx, AD7992& ad7992, MEMORY &memory=*(MEMORY*)0);
	void SetPower(uint32_t freq, uint8_t powerdBm);
	uint8_t GetPower();
	void Relax(uint8_t state=1);
	void SetCalTable(uint8_t table[12][50]);
	uint8_t CalcCalPot(uint32_t freq, uint8_t pot);
	void GeneratePoll();
	float ReadPower();
	float ReadReflectedPower();
	void SetCalPot(uint8_t* buffer);
	void GetCalPot(uint8_t* buffer);
	void SaveCal();
	void SetCalDetTrans(float value[]);
protected:
	MEMORY &_memory;
	MCP4XXX &_mcp4xxx;
	AD7992 &_ad7992;
	uint8_t _powerdBm;
	uint32_t _freq;
	uint8_t _mcp4xxxval[1024];
	uint8_t _trimpot;
	uint8_t _relax=1;
	uint8_t _calloaded=0;
	float *_dettranscal=(float*)&_mcp4xxxval[(36-24)*50];

	void CheckLoaded();
};
