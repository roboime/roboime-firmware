#pragma once

#include <usb/usb.h>
#include <inttypes.h>
#include <vector>
#include <initializer_list>
#include <list>

class USB_STM32;


class USB_DEVICE_CLASS{
public:
protected:
	friend class USB_STM32;
	USB_DEVICE_CLASS(uint8_t interfaces=1, uint8_t inendpoints=0, uint8_t outendpoints=0, std::initializer_list<const char*> stringlist={}, uint8_t enabled=1);
	virtual uint8_t Init(USB *usb){return USB::USBD_OK;}
	virtual uint8_t DeInit(){return USB::USBD_OK;}
	virtual uint8_t Setup(USB::USB_SETUP_REQUEST* req){return USB::USBD_OK;}
	virtual uint8_t EP0_TxSent(){return USB::USBD_OK;}
	virtual uint8_t EP0_RxReady(){return USB::USBD_OK;}
	virtual uint8_t DataIn(uint8_t epnum){return USB::USBD_OK;}
	virtual uint8_t DataOut(uint8_t epnum, uint16_t xfer_count){return USB::USBD_OK;}
	virtual uint8_t SOF(){return USB::USBD_OK;}
	virtual uint8_t ISO_IN(){return USB::USBD_OK;}
	virtual uint8_t ISO_OUT(){return USB::USBD_OK;}
	virtual uint8_t* GetDescriptor(){return 0;}
	virtual uint16_t GetDescriptorSize(){return 0;}
	virtual uint8_t InterfaceRequest(USB::USB_SETUP_REQUEST* req){return 0;}
	virtual uint8_t SetupCmd(uint16_t index, uint8_t cmd, uint8_t* buffer, uint16_t &len){ return 0;}
	uint8_t* GetDescriptorString(uint8_t index);
	uint8_t InterfaceBelongs(uint8_t interface){
		if(!_interfaces.size()) return 0;
		return (interface>=_interfaces[0] && interface<=_interfaces[_interfaces.size()-1]);
	}
	static uint8_t GetNumInterfaces(){return _numinterfaces;}
public:
	static std::list<USB_DEVICE_CLASS*> &GetDeviceClassesList();
protected:
	static uint8_t _numinterfaces;
	static uint8_t _numinendpoints;
	static uint8_t _numoutendpoints;
	static uint8_t _numdescriptorstrings;
	static std::list<USB_DEVICE_CLASS*> _usb_device_classes_list;

	std::vector<uint8_t> _interfaces;
	std::vector<uint8_t> _inendpoints;
	std::vector<uint8_t> _outendpoints;

	static const uint8_t _maxendpointsize=64;
	std::vector<uint8_t> _replacedescriptorvalue;
	const std::vector<const char*> _stringlist;
	uint8_t _firstdescriptorstring;
	USB *_usb;
};

