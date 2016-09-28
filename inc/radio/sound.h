#pragma once
#include <inttypes.h>
#include <radio/tpp_peripheral.h>
#include <radio/speaker.h>
#include <radio/microphone.h>
#include <utils/circularbuffer.h>
#include <queue>
#include <utils/iir_filter.h>

template <class T> class SoundBuffer: protected CircularBuffer<T>{
public:
	SoundBuffer(T* buffer, uint16_t size, uint32_t samplerate=16000, uint8_t isconstant=0):
		CircularBuffer<T>(buffer,size, size),
		_samplerate(samplerate),
		_isconstant(isconstant),
		_gain(1.0f)
	{
	}
	uint8_t IsConstant(){return _isconstant;}
	uint32_t GetSampleRate(){return _samplerate;}
	uint16_t GetSamples(int16_t *buffer, uint16_t size, int16_t gain){return 0;}
	uint16_t GetSamplesAdd(int16_t *buffer, uint16_t size, uint32_t samplerate=0){
		if(!samplerate) samplerate=_samplerate;
		float rate=(float)samplerate/(float)_samplerate;//6
		uint16_t i;
		int32_t value;
		if(CircularBuffer<T>::Ocupied()<(size/rate)) size=CircularBuffer<T>::Ocupied()*rate;
		for(i=0;i<size;i++){
			value=((int16_t)((*this)[(uint16_t)((float)i/rate)]))*_gain;
			value=value>32767?32767:value;
			value=value<-32768?-32768:value;
			*buffer+=(int16_t)value;
			buffer++;
		}
		CircularBuffer<T>::Out(0,(uint32_t)((float)size/rate));
		return size;
	}
	uint16_t Remaining(){return CircularBuffer<T>::Ocupied();}
	void Rewind(){
		CircularBuffer<T>::_contentsize=CircularBuffer<T>::_buffersize;
		CircularBuffer<T>::_tail=0;
		CircularBuffer<T>::_head=CircularBuffer<T>::_contentsize%CircularBuffer<T>::_buffersize;
	}
	void SetGain(float gain){_gain=gain;}
	T* GetBuffer(){ return CircularBuffer<T>::_buffer;}
	T* operator=(T* buffer){
		std::copy(buffer, buffer+CircularBuffer<T>::_buffersize, CircularBuffer<T>::_buffer);
		return buffer;
	}

private:
	uint32_t _samplerate;
	uint8_t _isconstant;
	float _gain;
};

class SOUND{
protected:
	friend SoundBuffer<int16_t>;
	friend SoundBuffer<int8_t>;
	uint32_t _samplerate;
	SPEAKER *_speaker;
	MICROPHONE *_microphone;
	uint32_t _speakertimeoutsamples;
	uint32_t _speakertimeoutsamplesacc;
	CircularBuffer<int16_t> _micbuffer;
public:
	SOUND(uint32_t samplerate, SPEAKER &speaker=*(SPEAKER*)0, MICROPHONE &microphone=*(MICROPHONE*)0, uint32_t speakertimeoutms=0, uint8_t nrchannels=4);

	void Play(SoundBuffer<int16_t> &soundbuffer, uint8_t channel=0, float gain=1.0f);
	void Play(SoundBuffer<int8_t> &soundbuffer, float gain=1.0f);

	void Poll();
	void SetSpeakerVolume(uint8_t vol);
	uint8_t GetSpeakerVolume();
	void ProcessSpeaker();
	void SetMicrophoneVolume(uint8_t vol);
	uint8_t GetMicrophoneVolume();
	void StartMicrophone();
	void StopMicrophone();
	void ProcessMicrophone();
	uint16_t GetMicrophoneSamples(int16_t* buffer, uint16_t size);
	uint16_t PutMicrophoneSamples(int16_t* buffer, uint16_t size);
	uint16_t GetMicrophoneAvailable();
	uint8_t GetNrChannels();
	uint8_t GetNextChannel();
	uint8_t GetReturnChannel();
	uint8_t GetUSBAudioChannel();
	uint8_t GetChannelQueueSize(uint8_t channel);

	static SoundBuffer<int8_t> snd_beinghailed_sb;
	static SoundBuffer<int8_t> snd_alerta_sb;
	static SoundBuffer<int8_t> snd_mensagem_sb;
	static SoundBuffer<int8_t> snd_rede_sb;
	static SoundBuffer<int8_t> snd_um_sb;
	static SoundBuffer<int8_t> snd_dois_sb;
	static SoundBuffer<int8_t> snd_tres_sb;
	static SoundBuffer<int8_t> snd_quatro_sb;
	static SoundBuffer<int8_t> snd_cinco_sb;
	static SoundBuffer<int8_t> snd_seis_sb;
	static SoundBuffer<int8_t> snd_sete_sb;
	static SoundBuffer<int8_t> snd_oito_sb;
	static SoundBuffer<int8_t> snd_nove_sb;
	static SoundBuffer<int8_t> snd_dez_sb;
	static SoundBuffer<int8_t> snd_onze_sb;
	static SoundBuffer<int8_t> snd_doze_sb;
	static SoundBuffer<int8_t> snd_treze_sb;
	static SoundBuffer<int8_t> snd_quatorze_sb;
	static SoundBuffer<int8_t> snd_quinze_sb;
	static SoundBuffer<int8_t> snd_bateria_sb;
	static SoundBuffer<int8_t> snd_fraca_sb;
	static SoundBuffer<int8_t> snd_nav_sb;
protected:
	static const uint8_t _extrachannels=2;
	uint16_t CallBack(int16_t *buffer, uint16_t consume_size);
	const uint8_t _nrchannels;
	std::queue<SoundBuffer<int16_t>*> *_soundint16;
	std::queue<SoundBuffer<int8_t>*> _soundint8;
	IIR_Filter _bpfilter;
	uint8_t _activeaudiochannel;
};
