#include <usb/usb_device_class_cdc_vcp.h>

USB_DEVICE_CLASS_CDC_VCP::USB_DEVICE_CLASS_CDC_VCP(std::initializer_list<const char*> stringlist, uint8_t enabled):
	USB_DEVICE_CLASS(2, 2, 2, stringlist, enabled),
	_datainbuffer(0,2048),
	_dataoutbuffer(0,2048),
	_sendzerolen(0),
	_USB_Tx_State(0),
	_USB_Rx_State(0),
	_linecoding{ 115200, /* baud rate*/ 0x00,   /* stop bits-1*/ 0x00,   /* parity - none*/ 0x08    /* nb. of bits 8*/},
	cdcCmd(0xFF),
	cdcIndex(0),
	cdcLen(0),
	_sofcounter(0),
	_connected(0)
{
}

uint8_t USB_DEVICE_CLASS_CDC_VCP::Init(USB* usb) {
	_usb=usb;
	_usb->EP_Open(_inendpoints[0], 10, USB::USB_ENDPOINT_TYPE_BULK);
	_usb->EP_Open(_inendpoints[1], sizeof(USB_DATA_TransmitBuffer), USB::USB_ENDPOINT_TYPE_BULK);
	_usb->EP_Open(_outendpoints[1], sizeof(USB_DATA_ReceiveBuffer), USB::USB_ENDPOINT_TYPE_BULK);
	return 0;
}

uint8_t USB_DEVICE_CLASS_CDC_VCP::DeInit() {
	_usb->EP_Close(_inendpoints[0]);
	_usb->EP_Close(_inendpoints[1]);
	_usb->EP_Close(_outendpoints[1]);
	return 0;
}

uint8_t USB_DEVICE_CLASS_CDC_VCP::SetupCmd(uint16_t index, uint8_t cmd, uint8_t* buffer, uint16_t &len) {
	if(cmd==0x21){//GET_LINE_CODING:
		std::copy((uint8_t*)&_linecoding, ((uint8_t*)&_linecoding)+sizeof(_linecoding), buffer);
		_usb->CtlSendData(buffer, 7);
	} else if(cmd==0x20){//SET_LINE_CODING:
		std::copy(buffer, buffer+sizeof(_linecoding), (uint8_t*)&_linecoding);
		_usb->CtlSendData(buffer, 0);
	} else if(cmd==0x22){//SET_LINE_CONTROL_STATE
		uint16_t *valor=(uint16_t*)buffer;
		if(*valor & 1){
			_connected=1;
		} else {
			_connected=0;
		}
	} else {
		cmd++;
	}
	return 0;
}

uint8_t USB_DEVICE_CLASS_CDC_VCP::SOF() {
	if(_connected && !_USB_Tx_State && _datainbuffer.Ocupied()){
		uint16_t len=_datainbuffer.Out(USB_DATA_TransmitBuffer, sizeof(USB_DATA_TransmitBuffer));
		if(_usb) _usb->EP_Tx(_inendpoints[1], USB_DATA_TransmitBuffer, len);
		if(len==sizeof(USB_DATA_TransmitBuffer)) _sendzerolen=1;
		_USB_Tx_State=1;
		_sofcounter=0;
	}
    if(!_USB_Rx_State && _dataoutbuffer.Free()>=sizeof(USB_DATA_ReceiveBuffer)){
    	if(_usb){
    		_usb->EP_PrepareRx(_outendpoints[1], USB_DATA_ReceiveBuffer, sizeof(USB_DATA_ReceiveBuffer));
        	_USB_Rx_State=1;
    	}
    }

    if(_USB_Tx_State){
    	_sofcounter++;
		if(_sofcounter>=100){
			_sofcounter=0;
			_USB_Tx_State=0;
			if(_usb) {
	    		_usb->EP_Close(_inendpoints[1]);
	    		_usb->EP_Open(_inendpoints[1], sizeof(USB_DATA_TransmitBuffer), USB::USB_ENDPOINT_TYPE_BULK);
				_usb->EP_Flush(_inendpoints[1]);
			}
		}
    }
	return USB::USBD_OK;
}

uint8_t USB_DEVICE_CLASS_CDC_VCP::DataIn(uint8_t epnum) {
	if(epnum==(_inendpoints[1] & 0x7F)){
		uint16_t len=_datainbuffer.Out(USB_DATA_TransmitBuffer, sizeof(USB_DATA_TransmitBuffer));
		if(len){
			_usb->EP_Tx(_inendpoints[1], USB_DATA_TransmitBuffer, len);
			if(len==sizeof(USB_DATA_TransmitBuffer)) _sendzerolen=1;
		} else if(_sendzerolen){
			_usb->EP_Tx(_inendpoints[1], 0,0);
			_sendzerolen=0;
			_USB_Tx_State=0;
		} else {
			_USB_Tx_State=0;
		}
		_sofcounter=0;
	}
	return USB::USBD_OK;
}

uint8_t USB_DEVICE_CLASS_CDC_VCP::DataOut(uint8_t epnum, uint16_t xfer_count) {
	if(epnum==_outendpoints[1]){
	    _dataoutbuffer.In(USB_DATA_ReceiveBuffer, xfer_count);
	    if(_dataoutbuffer.Free()>=sizeof(USB_DATA_ReceiveBuffer)){
	    	_usb->EP_PrepareRx(_outendpoints[1], USB_DATA_ReceiveBuffer, sizeof(USB_DATA_ReceiveBuffer));
	    	_USB_Rx_State=1;
	    }
	}
	return USB::USBD_OK;
}

uint16_t USB_DEVICE_CLASS_CDC_VCP::GetDescriptorSize() {
	return 61;
}


uint8_t* USB_DEVICE_CLASS_CDC_VCP::GetDescriptor() {
	desc_vendor[7]=_firstdescriptorstring;

	desc_vendor[2]=_interfaces[0];
	desc_vendor[10]=_interfaces[0];

	desc_vendor[26]=_interfaces[1];

	desc_vendor[33]=_inendpoints[0];

	desc_vendor[40]=_interfaces[1];
	desc_vendor[49]=_outendpoints[1];
	desc_vendor[56]=_inendpoints[1];
	return (uint8_t*)desc_vendor;
}


uint8_t USB_DEVICE_CLASS_CDC_VCP::desc_vendor[]={
		//INTERFACE ASSOCIATION DESCRIPTOR
		0x08,																						//array index 0
		0x0B,	///bDescriptorType
		0x00,	///bFirstInterface																	//array index 2
		0x02,	///bInterfaceCount
		0x02,	///bFunctionClass
		0x00,	///bFunctionSubClass
		0x00,	///bFunctionProtocol
		0x00,	///iFunction
		//SIZE: 9 + 5 + 5 + 4 + 9 + 7 + 7 = 46

		///INTERFACE DESCRIPTOR(0)
		0x09,    ///bLength: Length of this descriptor												//array index 8
		0x04,    ///bDescriptorType: Interface Descriptor Type
		0x00,    ///bInterfaceNumber: Interface Number												//array index 10
		0x00,    ///bAlternateSetting: Alternate setting for this interface
		0x01,    ///bNumEndpoints: Number of endpoints in this interface excluding endpoint 0
		0x02,    ///iInterfaceClass: Class Code: COMMUNICATIONS_AND_CDC_CONTROL
		0x02,    ///iInterfaceSubClass: SubClass Code
		0x00,    ///bInterfaceProtocol: Protocol Code
		0x05,    ///iInterface: String index

		/*Header Functional Descriptor*/
		0x05,   /* bLength: Endpoint Descriptor size */												//array index 17
		0x24,   /* bDescriptorType: CS_INTERFACE */
		0x00,   /* bDescriptorSubtype: Header Func Desc */
		0x10,   /* bcdCDC: spec release number */
		0x01,

		/*Call Management Functional Descriptor*/
		0x05,   /* bFunctionLength */																//array index 22
		0x24,   /* bDescriptorType: CS_INTERFACE */
		0x01,   /* bDescriptorSubtype: Call Management Func Desc */
		0x03,   /* bmCapabilities: D0+D1 */
		0x01,   /* bDataInterface: 3 */																//array index 26

		/*ACM Functional Descriptor*/
		0x04,   /* bFunctionLength */																//array index 27
		0x24,   /* bDescriptorType: CS_INTERFACE */
		0x02,   /* bDescriptorSubtype: Abstract Control Management desc */
		0x00,   /* bmCapabilities */

//		/*Union Functional Descriptor*/
//		0x05,   /* bFunctionLength */
//		0x24,   /* bDescriptorType: CS_INTERFACE */
//		0x06,   /* bDescriptorSubtype: Union func desc */
//		0x00,   /* bMasterInterface: Communication class interface */
//		0x03,   /* bSlaveInterface0: Data Class Interface */

		///ENDPOINT DESCRIPTOR(0)
		0x07,    ///bLength: Length of this descriptor												//array index 31
		0x05,    ///bDescriptorType: Endpoint Descriptor Type
		0x81,    ///bEndpointAddress: Endpoint address (IN,EP1)										//array index 33
		0x03,    ///bmAttributes: Transfer Type: INTERRUPT_TRANSFER
		0x0A,    ///wMaxPacketSize: Endpoint Size
		0x00,    ///wMaxPacketSize: Endpoint Size
		0x01,    ///bIntervall: Polling Intervall

		/*Data class interface descriptor*/
		0x09,   /* bLength: Endpoint Descriptor size */												//array index 38
		0x04,  /* bDescriptorType: */
		0x01,   /* bInterfaceNumber: Number of Interface */											//array index 40
		0x00,   /* bAlternateSetting: Alternate setting */
		0x02,   /* bNumEndpoints: Two endpoints used */
		0x0A,   /* bInterfaceClass: CDC */
		0x00,   /* bInterfaceSubClass: */
		0x00,   /* bInterfaceProtocol: */
		0x00,   /* iInterface: */

		/*Endpoint OUT Descriptor*/
		0x07,   /* bLength: Endpoint Descriptor size */												//array index 47
		0x05,      /* bDescriptorType: Endpoint */
		0x02,                        /* bEndpointAddress */											//array index 49
		0x02,                              /* bmAttributes: Bulk */
		0x40,  /* wMaxPacketSize: */
		0x00,
		0x00,                              /* bInterval: ignore for Bulk transfer */

		/*Endpoint IN Descriptor*/
		0x07,   /* bLength: Endpoint Descriptor size */												//array index 54
		0x05,   /* bDescriptorType: Endpoint */
		0x82, 	/* bEndpointAddress */																//array index 56
		0x02,   /* bmAttributes: Bulk */
		0x40,  	/* wMaxPacketSize: */
		0x00,
		0x00,   /* bInterval: ignore for Bulk transfer */
};

uint16_t USB_DEVICE_CLASS_CDC_VCP::GetData(CircularBuffer<uint8_t> &CB, uint16_t len) {
	return _dataoutbuffer.Out(CB, len);
}

uint16_t USB_DEVICE_CLASS_CDC_VCP::GetData(uint8_t* buffer, uint16_t len) {
	return _dataoutbuffer.Out(buffer, len);
}

uint16_t USB_DEVICE_CLASS_CDC_VCP::SendData(uint8_t* buffer, uint16_t len) {
	return _datainbuffer.In(buffer, len);
}

uint16_t USB_DEVICE_CLASS_CDC_VCP::SendData(CircularBuffer<uint8_t> &CB) {
	return _datainbuffer.In(CB);
}
