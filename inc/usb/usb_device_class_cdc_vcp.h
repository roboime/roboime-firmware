#pragma once

#pragma once
#include <usb/usb_device_class.h>
#include <utils/circularbuffer.h>
#include <vector>
#include <string>

class USB_DEVICE_CLASS_CDC_VCP: public USB_DEVICE_CLASS{
public:
	USB_DEVICE_CLASS_CDC_VCP(std::initializer_list<const char*> stringlist={}, uint8_t enabled=1);
	uint8_t Init(USB *usb);
	uint8_t DeInit();
	uint8_t DataIn(uint8_t epnum);
	uint8_t DataOut(uint8_t epnum, uint16_t xfer_count);
	uint8_t SOF();
	uint16_t GetDescriptorSize();
	uint8_t* GetDescriptor();
	uint16_t GetData(uint8_t *buffer, uint16_t len);
	uint16_t GetData(CircularBuffer<uint8_t> &CB, uint16_t len);
	uint16_t SendData(uint8_t *buffer, uint16_t len);
	uint16_t SendData(CircularBuffer<uint8_t> &CB);
	uint8_t SetupCmd(uint16_t index, uint8_t cmd, uint8_t* buffer, uint16_t &len);
protected:
	typedef struct
	{
	  uint32_t bitrate;
	  uint8_t  format;
	  uint8_t  paritytype;
	  uint8_t  datatype;
	}LINE_CODING;

	static uint8_t desc_vendor[];
	CircularBuffer<uint8_t,false> _datainbuffer;
	CircularBuffer<uint8_t,false> _dataoutbuffer;
	uint8_t _sendzerolen;
	uint8_t _USB_Tx_State;
	uint8_t _USB_Rx_State;
	LINE_CODING _linecoding;

	uint32_t cdcCmd;
	uint32_t cdcIndex;
	uint32_t cdcLen;
	uint8_t CmdBuff[256];
	uint8_t USB_DATA_ReceiveBuffer[64];
	uint8_t USB_DATA_TransmitBuffer[64];
	uint16_t _sofcounter;
	uint8_t _connected;

};

