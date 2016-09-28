#pragma once
#include <usb/usb_device_class.h>
#include <utils/circularbuffer.h>

class USB_DEVICE_CLASS_TEMPLATE: public USB_DEVICE_CLASS{
public:
	USB_DEVICE_CLASS_TEMPLATE();
	void SendAudio(int16_t *audiobuffer, uint16_t samples);
	uint16_t GetAudio(int16_t *audiobuffer, uint16_t samples);
	uint16_t GetAudioAvailable();
protected:
	uint8_t Init(USB *usb);
	uint8_t DeInit(USB *usb);
	uint8_t Setup(USB *usb, USB::USB_SETUP_REQUEST* req);
	uint8_t EP0_TxSent(USB *usb);
	uint8_t EP0_RxReady(USB *usb);
	uint8_t  DataIn(USB *usb, uint8_t epnum);
	uint8_t  DataOut(USB *usb, uint8_t epnum, uint16_t xfer_count);
	uint8_t  SOF(USB *usb);
	uint8_t ISO_IN(USB *usb);
	uint8_t  ISO_OUT(USB *usb);
	uint8_t GetNumInterfaces();
	uint8_t GetNumEndpoints();
	uint16_t GetDescriptorSize();
	uint8_t* GetDescriptor(uint8_t firstinterface=0,  uint8_t firstendpoint=1, uint8_t firstdescriptorstring=10);

	static uint8_t desc_vendor[];
	CircularBuffer<int16_t> _audioinbuffer;
	CircularBuffer<int16_t> _audiooutbuffer;

	uint32_t cdcCmd;
	uint32_t cdcIndex;
	uint32_t cdcLen;
	uint8_t USB_AUDIO_Record;
	uint8_t USB_AUDIO_TransmitIncomplete;
	uint8_t CmdBuff[256];
	int16_t USB_AUDIO_ReceiveBuffer[16];
	int16_t USB_AUDIO_TransmitBuffer[16];
};

