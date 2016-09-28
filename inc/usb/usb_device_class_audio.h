#pragma once
#include <usb/usb_device_class.h>
#include <utils/circularbuffer.h>

class USB_DEVICE_CLASS_AUDIO: public USB_DEVICE_CLASS{
public:
	USB_DEVICE_CLASS_AUDIO(uint8_t enabled=1);
	void SendAudio(int16_t *audiobuffer, uint16_t samples);
	uint16_t GetAudio(int16_t *audiobuffer, uint16_t samples);
	uint16_t GetAudioAvailable();
protected:
	uint8_t Init(USB *usb);
	uint8_t DeInit();
	uint8_t DataIn(uint8_t epnum);
	uint8_t DataOut(uint8_t epnum, uint16_t xfer_count);
	uint8_t SOF();
	uint16_t GetDescriptorSize();
	uint8_t* GetDescriptor();
	uint8_t InterfaceRequest(USB::USB_SETUP_REQUEST* req);
	static uint8_t descriptor[];
	CircularBuffer<int16_t> _audioinbuffer;
	CircularBuffer<int16_t> _audiooutbuffer;

	uint8_t USB_AUDIO_Record;
	uint8_t USB_AUDIO_TransmitIncomplete;
	int16_t USB_AUDIO_ReceiveBuffer[16];
	int16_t USB_AUDIO_TransmitBuffer[16];
};
