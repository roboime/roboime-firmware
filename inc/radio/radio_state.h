#pragma once
typedef struct {
	struct {
		uint16_t nid;
		uint8_t name[16];
		uint32_t waveform;
		uint32_t txfrequency;
		uint32_t txpower;
		uint32_t rxfrequency;
		uint8_t rxonly;
		uint32_t cryptmode;
		uint8_t cryptkeyindex;
	} netcfg[15];
	uint16_t rid;
	uint8_t nome[32];
	uint8_t speakervolume;// 0 to 9
	uint8_t speakervolumefixed;//checkbox
	uint8_t speakerinitvolume;
	uint8_t micvolume;// 0 to 9
	uint8_t micvolumefixed;//checkbox
	uint8_t micinitvolume;
	uint8_t usbpower;//checkbox
	uint8_t headsetfilter;//checkbox
	uint8_t cryptkey[50][32];
	uint32_t buttonfunction[6][4]; //six buttons (left, right, ptt 1, ptt 2, extptt1, extptt2) with 4 functions (short click, medium click, long click, release)
	uint8_t audioreturn;
	uint32_t version;
} RADIO_STATE;
