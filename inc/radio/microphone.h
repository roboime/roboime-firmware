#pragma once

#include <radio/audio_device.h>
#include <inttypes.h>

class MICROPHONE:public AUDIO_DEVICE{
public:
	MICROPHONE(VOLUME &volume):AUDIO_DEVICE(volume){}
};
