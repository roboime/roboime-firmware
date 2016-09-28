#include <usb/usb_device_class.h>

uint8_t USB_DEVICE_CLASS::_numinterfaces=0;
uint8_t USB_DEVICE_CLASS::_numinendpoints=1;
uint8_t USB_DEVICE_CLASS::_numoutendpoints=1;
uint8_t USB_DEVICE_CLASS::_numdescriptorstrings=6;
std::list<USB_DEVICE_CLASS*> USB_DEVICE_CLASS::_usb_device_classes_list;

uint8_t* USB_DEVICE_CLASS::GetDescriptorString(uint8_t index) {
	int8_t ind = index - _firstdescriptorstring;
	if (ind >= 0 && ind < (int8_t) (_stringlist.size())) {
		return (uint8_t*) (_stringlist[ind]);
	}
	return 0;
}
