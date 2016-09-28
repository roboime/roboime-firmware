#pragma once
#include <hal/volume.h>
#include <hal/mcp4xxx.h>

class VOLUME_MCP4XXX: public VOLUME{
public:
	VOLUME_MCP4XXX(MCP4XXX &mcp4xxx):_mcp4xxx(&mcp4xxx){}
	void SetVolume(float volume){
		VOLUME::SetVolume(volume);
		if(_mcp4xxx) _mcp4xxx->SetValue((uint8_t)(volume*255.0f));
	}
	float GetVolume(){
		float volume=0.0f;
		if(_mcp4xxx) volume=((float)_mcp4xxx->GetValue())/255.0f;
		VOLUME::SetVolume(volume);
		return VOLUME::GetVolume();
	}
protected:
	MCP4XXX *_mcp4xxx;
};
