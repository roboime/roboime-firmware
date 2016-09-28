#pragma once
#include <radio/speaker.h>
#include <hal/cs43l22.h>
#include <inttypes.h>

class SPEAKER_CS43L22: public CS43L22, public SPEAKER{
public:
	SPEAKER_CS43L22(CS43L22 &cs43l22, VOLUME &volume=*(VOLUME*)0):CS43L22(cs43l22),SPEAKER(volume),_volumel(255),_statechanged(0),_speakerstatus(1),_volumechanged(0){};
	void Start();
	void Stop();
	void SetVolume(uint8_t volume);
	uint8_t GetVolume();
	void Poll();
	int16_t* GetBuffer(){return _i2s->GetBuffer();}
	uint16_t GetBufferSize(){return _i2s->GetBufferSize();}
protected:
	uint8_t _volumel;
	uint8_t _statechanged;
	uint8_t _speakerstatus;
	uint8_t _volumechanged;
};
