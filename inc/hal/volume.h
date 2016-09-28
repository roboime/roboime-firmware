#pragma once


class VOLUME{
public:
	VOLUME():_volume(1.0f){}
	virtual void SetVolume(float volume){_volume=volume;}
	virtual float GetVolume(){return _volume;}
protected:
	float _volume;
};
