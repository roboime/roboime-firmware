#pragma once

#include <radio/audio_device.h>
#include <inttypes.h>

class SPEAKER:public AUDIO_DEVICE{
public:
	SPEAKER(VOLUME &volume):AUDIO_DEVICE(volume){}
};
