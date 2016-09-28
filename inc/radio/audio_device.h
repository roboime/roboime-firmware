#pragma once

#include <inttypes.h>
#include <hal/volume.h>

class AUDIO_DEVICE{
public:
	AUDIO_DEVICE(VOLUME &volume):_volume(&volume){}
	virtual void Start()=0;
	virtual void Stop()=0;
	virtual void Poll()=0;
	virtual int16_t* GetBuffer()=0;
	virtual uint16_t GetBufferSize()=0;
	virtual void SetVolume(uint8_t volume){	if(_volume) _volume->SetVolume(((float)volume)/10.0f);}
	virtual uint8_t GetVolume(){if(_volume) return (uint8_t)(_volume->GetVolume()*10.0f); return 0;}
protected:
	VOLUME *_volume;
};
