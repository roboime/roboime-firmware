#pragma once
#include <inttypes.h>
#include <radio/sound.h>

#include <radio/radio_event.h>
#include <hal/modem.h>
#include <radio/radio_fifo.h>
#include <radio/serial_data_processor.h>
#include <radio/configuration.h>
#include <radio/ptt.h>
#include <radio/buttons.h>
#include <utils/debounce.h>
#include <radio/packer.h>
#include <codec/cvsd.h>
#include <hal/memory.h>
#include <utils/iir_filter.h>
#include <radio/gps.h>
#include <radio/radio_state.h>

class RADIO{
public:
	RADIO(SOUND &snd=*(SOUND*)0, MODEM &modem=*(MODEM*)0, PTT &ptt=*(PTT*)0, Buttons &buttons=*(Buttons*)0, GPS &gps=*(GPS*)0, MEMORY &memory=*(MEMORY*)0);
	void Process();
	void SetNetwork(RadioNet &net);
	void SetTestMode(uint8_t testmode);
	void SaveState();
	void LoadState();
	typedef PACKER_APCO25::PACKAGE_TYPE PACKAGE_TYPE;

protected:
	typedef enum {
		RadioSoundNet1=1,
		RadioSoundNet2,
		RadioSoundNet3,
		RadioSoundNet4,
		RadioSoundNet5,
		RadioSoundNet6,
		RadioSoundNet7,
		RadioSoundNet8,
		RadioSoundNet9,
		RadioSoundNet10,
		RadioSoundNet11,
		RadioSoundNet12,
		RadioSoundNet13,
		RadioSoundNet14,
		RadioSoundNet15,
		RadioSoundClick,
		RadioSoundMediumClick,
		RadioSoundLongClick,
		RadioSound1,
		RadioSound2,
		RadioSound3,
		RadioSound4,
		RadioSound5,
		RadioSound6,
	} RadioSound;
	enum{
		RADIO_STATE_VERSION_0,
		RADIO_STATE_VERSION_1,
		RADIO_STATE_VERSION_2,
		RADIO_STATE_VERSION_3,
		RADIO_STATE_VERSION_4,
		RADIO_STATE_VERSION_5,
		RADIO_STATE_VERSION_6,
		RADIO_STATE_VERSION_7,
		RADIO_STATE_VERSION_8,
		RADIO_STATE_VERSION_9,
		RADIO_STATE_VERSION_PRE_LAST,
		RADIO_STATE_VERSION_LAST=RADIO_STATE_VERSION_PRE_LAST-1
	};

	void PlaySound(RadioSound radiosound, float volume=1.0f);
	void FunctionSpeakerVolumeUp();
	void FunctionSpeakerVolumeDown();
	void FunctionSpeakerVolumeCycleUp();
	void FunctionSpeakerVolumeCycleDown();
	void FunctionMicVolumeUp();
	void FunctionMicVolumeDown();
	void FunctionMicVolumeCycleUp();
	void FunctionMicVolumeCycleDown();
	void ProcessGPS();
	void ProcessKeys();
	void ProcessSwitch();
	void DeInitNet();
	void AudioTransmit();
	void PTTPressed(uint8_t netnumber, MODEM_POWER power=MODEM_POWER_NORMAL);
	void PTTReleased();
	void ProcessPackage(PACKAGE_TYPE type, uint8_t* receivebuffer, int16_t sizeunpacked);
	void ProcessPackageAudio(uint8_t* receivebuffer, int16_t sizeunpacked);
	void ProcessPackageDataSerial(uint8_t* receivebuffer, int16_t sizeunpacked);
	void ProcessPackageDataNet(uint8_t* receivebuffer, int16_t sizeunpacked);
	void ProcessReception();
	void ProcessUSBSerial();
	void ProcessUSBSerialTestmode();
	void ProcessUSBAudio();
	void ProcessUSBNet();
	void CheckState();

	uint32_t GetRandomWaitTime(uint32_t timems);
	uint8_t CanTransmit();
	void ChangeNet(uint8_t newnet);

	uint8_t _testmode;
	SOUND *_sound;
	MODEM *_modem;
	PTT *_ptt;
	Buttons *_buttons;
	GPS &_gps;
	MEMORY *_memory;
	Buttons::ButtonFunction _ButtonFunction;
	uint8_t _sw_pos_old;
	struct { uint8_t switchChanged:1;} _flags;
	Debounce<uint8_t> _switchesdebounce;
	Debounce<uint8_t> _keysdebounce;
	PACKER_APCO25 _packer;
	CVSD _cvsd;
	uint8_t _activeaudiochannel;
	RADIO_EVENT _event;
	RADIO_STATE _radio_state;
	uint32_t _packetreceivedtimestamp;
	uint32_t _transmitrandomwait;
	CircularBuffer<uint8_t,false> _cvsdbuffer;
	uint8_t _receivebuffer[2048];
	uint8_t _selectednetwork;
	uint8_t _pttnetwork;
	MODEM_POWER _pttpower;
	IIR_Filter _otf_filter;
	CircularBuffer<uint8_t> _usbserialbuffer;
	CircularBuffer<uint8_t> _gpsBuffer;
	Debounce<uint16_t> _usbserialbufferdebounce;
	CircularBuffer<uint8_t> _usbnetbuffer;
	uint8_t _usbnetbufferconsumed;
	uint32_t _lasttimereceived;
	uint8_t _gpsusb;
};

