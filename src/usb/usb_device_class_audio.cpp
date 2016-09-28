#include <usb/usb_device_class_audio.h>

USB_DEVICE_CLASS_AUDIO::USB_DEVICE_CLASS_AUDIO(uint8_t enabled):
	USB_DEVICE_CLASS(3,1,1,{"TPP-1400 Audio"}, enabled),
	_audioinbuffer(0,512),
	_audiooutbuffer(0,512),
	USB_AUDIO_Record(0),
	USB_AUDIO_TransmitIncomplete(0)
{
}

uint8_t USB_DEVICE_CLASS_AUDIO::Init(USB* usb) {
	_usb=usb;
	_usb->EP_Open(_inendpoints[0], 32, USB::USB_ENDPOINT_TYPE_ISOC);
	_usb->EP_Open(_outendpoints[0], 32, USB::USB_ENDPOINT_TYPE_ISOC);
	_usb->EP_PrepareRx(_outendpoints[0], (uint8_t*)USB_AUDIO_ReceiveBuffer, 32);
	return 0;
}

uint8_t USB_DEVICE_CLASS_AUDIO::DeInit() {
	_usb->EP_Close(_inendpoints[0]);
	_usb->EP_Close(_outendpoints[0]);
	return 0;
}

uint8_t USB_DEVICE_CLASS_AUDIO::InterfaceRequest(USB::USB_SETUP_REQUEST* req) {
	switch (req->bRequest){
	case USB::USB_REQ_SET_INTERFACE :
		if(req->wIndex==_interfaces[2]){
			if(req->wValue){
				USB_AUDIO_Record=1;
				USB_AUDIO_TransmitIncomplete=1;
				_audioinbuffer.Clear();
			} else {
				USB_AUDIO_Record=0;
			}
		}
		break;
	}
	return USB::USBD_OK;
}

uint8_t USB_DEVICE_CLASS_AUDIO::DataIn(uint8_t epnum) {
	if(epnum==(_inendpoints[0] & 0x7F)){
		if(USB_AUDIO_Record){
			uint16_t len=_audioinbuffer.Out(USB_AUDIO_TransmitBuffer, 16);
			if(len){
				_usb->EP_Flush(_inendpoints[0]);
				_usb->EP_Tx(_inendpoints[0], (uint8_t*)USB_AUDIO_TransmitBuffer, len*sizeof(*USB_AUDIO_TransmitBuffer));
			} else {
				USB_AUDIO_TransmitIncomplete=1;
			}
		}
	}
	return USB::USBD_OK;
}

uint8_t USB_DEVICE_CLASS_AUDIO::DataOut(uint8_t epnum, uint16_t xfer_count) {
	if(epnum==_outendpoints[0]){
	    _audiooutbuffer.In(USB_AUDIO_ReceiveBuffer, xfer_count/sizeof(*USB_AUDIO_ReceiveBuffer));
		_usb->EP_PrepareRx(_outendpoints[0], (uint8_t*)USB_AUDIO_ReceiveBuffer, 32);
	}
	return USB::USBD_OK;
}

uint8_t USB_DEVICE_CLASS_AUDIO::SOF() {
	if(USB_AUDIO_Record && USB_AUDIO_TransmitIncomplete){
		if(_audioinbuffer.Ocupied()>16*4){
			uint16_t len= _audioinbuffer.Out(USB_AUDIO_TransmitBuffer, 16);
			if(_usb) _usb->EP_Flush(_inendpoints[0]);
			if(_usb) _usb->EP_Tx(_inendpoints[0], (uint8_t*)USB_AUDIO_TransmitBuffer, len*sizeof(*USB_AUDIO_TransmitBuffer));
			USB_AUDIO_TransmitIncomplete=0;
		}
	}
	return USB::USBD_OK;
}


uint16_t USB_DEVICE_CLASS_AUDIO::GetDescriptorSize() {
	return 173;
}

uint8_t* USB_DEVICE_CLASS_AUDIO::GetDescriptor() {
	descriptor[7]=_firstdescriptorstring;

	descriptor[2]=_interfaces[0];
	descriptor[10]=_interfaces[0];
	descriptor[25]=_interfaces[1];
	descriptor[26]=_interfaces[2];

	descriptor[71]=_interfaces[1];
	descriptor[80]=_interfaces[1];
	descriptor[107]=_outendpoints[0];

	descriptor[123]=_interfaces[2];
	descriptor[132]=_interfaces[2];
	descriptor[159]=_inendpoints[0];
	return (uint8_t*)descriptor;
}

void USB_DEVICE_CLASS_AUDIO::SendAudio(int16_t *audiobuffer, uint16_t samples) {
	int16_t data[samples];
	for(uint16_t i=0;i<samples;i++){
		data[i]=audiobuffer[i]-2048;
	}
	_audioinbuffer.In(data, samples);
}
uint16_t USB_DEVICE_CLASS_AUDIO::GetAudio(int16_t* audiobuffer, uint16_t samples)
{
	return _audiooutbuffer.Out(audiobuffer, samples);
}


uint16_t USB_DEVICE_CLASS_AUDIO::GetAudioAvailable() {
	return _audiooutbuffer.Ocupied();
}

uint8_t USB_DEVICE_CLASS_AUDIO::descriptor[]={
	//INTERFACE ASSOCIATION DESCRIPTOR
	0x08,												//array index 0
	0x0B,	///bDescriptorType
	0x00,	///bFirstInterface							//array index 2
	0x03,	///bInterfaceCount
	0x01,	///bFunctionClass
	0x00,	///bFunctionSubClass
	0x00,	///bFunctionProtocol
	0x00,	///iFunction

	//Size: 9 + 9 + 12 + 9 + 9 + 9 + 9 + 7 + 11 + 9 + 7 = 100
	0x09, // bLength (9)								//array index 8
	0x04, // bDescriptorType (INTERFACE)
	0x00, // bInterfaceNumber 							//array index 10
	0x00, // bAlternateSetting (none)
	0x00, // bNumEndpoints (none)
	0x01, // bInterfaceClass (AUDIO)
	0x01, // bInterfaceSubClass (AUDIO_CONTROL)
	0x00, // bInterfaceProtocol (none)
	0x00, // iInterface (none)

	0x0A, // bLength (10)								//array index 17
	0x24, // bDescriptorType (CS_INTERFACE)
	0x01, // bDescriptorSubtype (HEADER)
	0x00,
	0x01, // bcdADC (1.0)
	10+12+9+12+9,
	0x00, // wTotalLength (43)
	0x02, // bInCollection (2 streaming interfaces)
	0x01, // baInterfaceNr (interface 1 is stream)  	//array index 25
	0x02, // baInterfaceNr (interface 2 is stream)		//array index 26

	0x0C, // bLength (12)								//array index 27
	0x24, // bDescriptorType (CS_INTERFACE)
	0x02, // bDescriptorSubtype (INPUT_TERMINAL)
	0x01, // bTerminalID (1)
	0x01,
	0x01, // wTerminalType (USB Streaming)
	0x00, // bAssocTerminal (none)
	0x01, // bNrChannels (1)
	0x00,
	0x00, // wChannelConfig (mono)
	0x00, // iChannelNames (none)
	0x00, // iTerminal (none)

	0x09, // bLength (9)								//array index 39
	0x24, // bDescriptorType (CS_INTERFACE)
	0x03, // bDescriptorSubtype (OUTPUT_TERMINAL)
	0x02, // bTerminalID (2)
	0x01,
	0x03, // wTerminalType (speaker)
	0x00, // bAssocTerminal (none)
	0x01, // bSourceID (input 1)
	0x00, // iTerminal (none)

	0x0C, // bLength (12)								//array index 48
	0x24, // bDescriptorType (CS_INTERFACE)
	0x02, // bDescriptorSubtype (INPUT_TERMINAL)
	0x03, // bTerminalID (3)
	0x01,
	0x02, // wTerminalType (Microphone)
	0x00, // bAssocTerminal (none)
	0x01, // bNrChannels (1)
	0x00,
	0x00, // wChannelConfig (mono)
	0x00, // iChannelNames (none)
	0x00, // iTerminal (none)

	0x09, // bLength (9)								//array index 60
	0x24, // bDescriptorType (CS_INTERFACE)
	0x03, // bDescriptorSubtype (OUTPUT_TERMINAL)
	0x04, // bTerminalID (4)
	0x01,
	0x01, // wTerminalType (USB Streaming)
	0x00, // bAssocTerminal (none)
	0x03, // bSourceID (input 3)
	0x00, // iTerminal (none)

	0x09, // bLength (9)								//array index 69
	0x04, // bDescriptorType (INTERFACE)
	0x01, // bInterfaceNumber 							//array index 71
	0x00, // bAlternateSetting (0)
	0x00, // bNumEndpoints (0)
	0x01, // bInterfaceClass (AUDIO)
	0x02, // bInterfaceSubClass (AUDIO_STREAMING)
	0x00, // bInterfaceProtocol (none)
	0x00, // iInterface (none)

	0x09, // bLength (9)								//array index 78
	0x04, // bDescriptorType (INTERFACE)
	0x01, // bInterfaceNumber							//array index 80
	0x01, // bAlternateSetting (1)
	0x01, // bNumEndpoints (1)
	0x01, // bInterfaceClass (AUDIO)
	0x02, // bInterfaceSubClass (AUDIO_STREAMING)
	0x00, // bInterfaceProtocol (none)
	0x00, // iInterface (none)

	0x07, // bLength (7)								//array index 87
	0x24, // bDescriptorType (CS_INTERFACE)
	0x01, // bDescriptorSubtype (AS_GENERAL)
	0x01, // bTerminalLink (terminal 1)
	0x01, // bDelay (none)
	0x01,
	0x00, // wFormatTag (PCM format)

	0x0B, // bLength (11)								//array index 94
	0x24, // bDescriptorType (CS_INTERFACE)
	0x02, // bDescriptorSubtype (FORMAT_TYPE)
	0x03, // bFormatType (TYPE_I)
	0x01, // bNrChannels (1)
	0x02, // bSubFrameSize (2)
	// The next field should be 10, but 16 works with more standard software
	0x10, // bBitResolution (16)
	0x01, // bSamFreqType (1 sampling frequency)
	0x80, // 16,000 Hz (byte 0)
	0x3E, // 16,000 Hz (byte 1)
	0x00, // 16,000 Hz (byte 2)

	0x09, // bLength (9)								//array index 105
	0x05, // bDescriptorType (ENDPOINT)
	0x01, // bEndpointAddress							//array index 107
	0x01, // bmAttributes (asynchronous)
	0x20,
	0x00, // wMaxPacketSize (32)
	0x01, // bInterval (1 millisecond)
	0x00, // bRefresh (0)
	0x00, // bSynchAddress (no synchronization)

	0x07, // bLength (7)								//array index 114
	0x25, // bDescriptorType (CS_ENDPOINT)
	0x01, // bDescriptorSubtype (EP_GENERAL)
	0x00, // bmAttributes (none)
	0x00, // bLockDelayUnits (PCM samples)
	0x00,
	0x00, // wLockDelay (0)

	0x09, // bLength (9)								//array index 121
	0x04, // bDescriptorType (INTERFACE)
	0x02, // bInterfaceNumber							//array index 123
	0x00, // bAlternateSetting (0)
	0x00, // bNumEndpoints (0)
	0x01, // bInterfaceClass (AUDIO)
	0x02, // bInterfaceSubClass (AUDIO_STREAMING)
	0x00, // bInterfaceProtocol (none)
	0x00, // iInterface (none)

	0x09, // bLength (9)								//array index 130
	0x04, // bDescriptorType (INTERFACE)
	0x02, // bInterfaceNumber							//array index 132
	0x01, // bAlternateSetting (1)
	0x01, // bNumEndpoints (1)
	0x01, // bInterfaceClass (AUDIO)
	0x02, // bInterfaceSubClass (AUDIO_STREAMING)
	0x00, // bInterfaceProtocol (none)
	0x00, // iInterface (none)

	0x07, // bLength (7)								//array index 139
	0x24, // bDescriptorType (CS_INTERFACE)
	0x01, // bDescriptorSubtype (AS_GENERAL)
	0x04, // bTerminalLink (terminal 4)
	0x00, // bDelay (none)
	0x01,
	0x00, // wFormatTag (PCM format)

	0x0B, // bLength (11)								//array index 146
	0x24, // bDescriptorType (CS_INTERFACE)
	0x01, // bDescriptorSubtype (FORMAT_TYPE)
	0x01, // bFormatType (TYPE_I)
	0x01, // bNrChannels (1)
	0x02, // bSubFrameSize (2)
	// The next field should be 10, but 16 works with more standard software
	0x10, // bBitResolution (16)
	0x01, // bSamFreqType (1 sampling frequency)
	0x80, // 16,000 Hz (byte 0)
	0x3E, // 16,000 Hz (byte 1)
	0x00, // 16,000 Hz (byte 2)

	0x09, // bLength (9)								//array index 157
	0x05, // bDescriptorType (ENDPOINT)
	0x81, // bEndpointAddress							//array index 159
	0x01, // bmAttributes (asynchronous)
	0x20,
	0x00, // wMaxPacketSize (32)
	0x01, // bInterval (1 millisecond)
	0x00, // bRefresh (0)
	0x00, // bSynchAddress (no synchronization)

	0x07, // bLength (7)								//array index 166
	0x25, // bDescriptorType (CS_ENDPOINT)
	0x01, // bDescriptorSubtype (EP_GENERAL)
	0x00, // bmAttributes (none)
	0x00, // bLockDelayUnits (PCM samples)
	0x00,
	0x00, // wLockDelay (0)								//array index 172
		};

