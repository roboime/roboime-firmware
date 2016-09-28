#include <usb/usb_device_class_cdc_rndis.h>
#include <radio/serialnumber.h>
#include <utils/tpp_debug.h>

const uint32_t USB_DEVICE_CLASS_CDC_RNDIS::response[]={
		RNDIS_RESPONSE_AVAILABLE,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
};

USB_DEVICE_CLASS_CDC_RNDIS::USB_DEVICE_CLASS_CDC_RNDIS(uint8_t enabled):
	USB_DEVICE_CLASS(2, 2, 2, {"TPP-1400 RNDIS"}, enabled),
	_inbuffer(0,2048),
	_outbuffer(0,2048),
	rndis_oid_gen_xmit_ok(0),
	rndis_oid_gen_rcv_ok(0),
	usbd_cdc_rndis_AltSet(0),
	_packetreceived(0),
	_startrxout(0)
{
	_inbuffer.In(0);
}

uint8_t USB_DEVICE_CLASS_CDC_RNDIS::Init(USB* usb) {
	_usb=usb;
	_usb->EP_Open(_inendpoints[0], 8, USB::USB_ENDPOINT_TYPE_INT);
	_usb->EP_Open(_inendpoints[1], _maxendpointsize, USB::USB_ENDPOINT_TYPE_BULK);
	_usb->EP_Open(_outendpoints[1], _maxendpointsize, USB::USB_ENDPOINT_TYPE_BULK);
	_usb->EP_PrepareRx(_outendpoints[1], _receivebuffer, _maxendpointsize);
	return 0;
}

uint8_t USB_DEVICE_CLASS_CDC_RNDIS::DeInit() {
	_usb->EP_Close(_inendpoints[0]);
	_usb->EP_Close(_inendpoints[1]);
	_usb->EP_Close(_outendpoints[1]);
	return 0;
}

uint8_t USB_DEVICE_CLASS_CDC_RNDIS::DataIn(uint8_t epnum) {
	if(epnum==(_inendpoints[1] & 0x7F)){
		_inbuffer.Clear();
	}
	return USB::USBD_OK;
}

uint8_t USB_DEVICE_CLASS_CDC_RNDIS::DataOut(uint8_t epnum, uint16_t xfer_count) {
	if(epnum==_outendpoints[1] && !_packetreceived){
	    _outbuffer.In(_receivebuffer, xfer_count);
		if(xfer_count==_maxendpointsize){
			_usb->EP_PrepareRx(_outendpoints[1], _receivebuffer, _maxendpointsize);
		} else {
			REMOTE_NDIS_PACKET_MSG_STRUCT_T *msg=(REMOTE_NDIS_PACKET_MSG_STRUCT_T*)_outbuffer.GetRearrangedBuffer();
			if(msg->MessageType==RNDIS_MSG_PACKET){
//				uint16_t dataLength=msg->DataLength;
				uint16_t dataOffset=msg->DataOffset+8;
				_outbuffer.Out(0, dataOffset);
//				if(dataLength<_outbuffer.Ocupied()){
//					_outbuffer.OutEnd(0,_outbuffer.Ocupied()-dataLength);
//				}
				_packetreceived=1;
			} else {
				_outbuffer.Clear();
				_usb->EP_PrepareRx(_outendpoints[1], _receivebuffer, _maxendpointsize);
			}

		}
	}
	return USB::USBD_OK;
}


uint16_t USB_DEVICE_CLASS_CDC_RNDIS::GetDescriptorSize() {
	return 47;
}

uint8_t* USB_DEVICE_CLASS_CDC_RNDIS::GetDescriptor() {
	_descriptor[7]=_firstdescriptorstring;

	_descriptor[2]=_interfaces[0];
	_descriptor[10]=_interfaces[0];
	_descriptor[19]=_inendpoints[0];

	_descriptor[26]=_interfaces[1];
	_descriptor[35]=_inendpoints[1];
	_descriptor[42]=_outendpoints[1];

	return (uint8_t*)_descriptor;
}

uint8_t USB_DEVICE_CLASS_CDC_RNDIS::_descriptor[]={
		//INTERFACE ASSOCIATION DESCRIPTOR
		0x08,																							//array index 0
		0x0B,	///bDescriptorType
		0x00,	///bFirstInterface																		//array index 2
		0x02,	///bInterfaceCount
		0xE0,    ///iInterfaceClass: Class Code: Wireless
		0x01,    ///iInterfaceSubClass: RF Controller
		0x03,    ///bInterfaceProtocol: Remote NDIS
//		0x02,	///bFunctionClass
//		0x02,	///bFunctionSubClass
//		0xFF,	///bFunctionProtocol
		0x00,	///iFunction																			//array index 7

		//SIZE: 9 + 7 + 9 + 7 + 7 = 39
		///INTERFACE DESCRIPTOR(0)
		0x09,    ///bLength: Length of this descriptor													//array index 8
		0x04,    ///bDescriptorType: Interface Descriptor Type
		0x00,    ///bInterfaceNumber: Interface Number													//array index 10
		0x00,    ///bAlternateSetting: Alternate setting for this interface
		0x01,    ///bNumEndpoints: Number of endpoints in this interface excluding endpoint 0
		0xE0,    ///iInterfaceClass: Class Code: Wireless
		0x01,    ///iInterfaceSubClass: RF Controller
		0x03,    ///bInterfaceProtocol: Remote NDIS
//		0x02,    ///iInterfaceClass: Class Code: COMMUNICATIONS_AND_CDC_CONTROL
//		0x02,    ///iInterfaceSubClass: SubClass Code
//		0xFF,    ///bInterfaceProtocol: Protocol Code
		0x00,    ///iInterface: String index

		///ENDPOINT DESCRIPTOR(0)
		0x07,    ///bLength: Length of this descriptor													//array index 17
		0x05,    ///bDescriptorType: Endpoint Descriptor Type
		0x81,    ///bEndpointAddress: Endpoint address (IN,EP3)											//array index 19
		0x03,    ///bmAttributes: Transfer Type: INTERRUPT_TRANSFER
		0x08,    ///wMaxPacketSize: Endpoint Size
		0x00,    ///wMaxPacketSize: Endpoint Size
		0x01,    ///bIntervall: Polling Intervall

		///INTERFACE DESCRIPTOR(5)
		0x09,    ///bLength: Length of this descriptor													//array index 24
		0x04,    ///bDescriptorType: Interface Descriptor Type
		0x01,    ///bInterfaceNumber: Interface Number													//array index 26
		0x00,    ///bAlternateSetting: Alternate setting for this interface
		0x02,    ///bNumEndpoints: Number of endpoints in this interface excluding endpoint 0
		0x0A,    ///iInterfaceClass: Class Code: CDC_DATA
		0x00,    ///iInterfaceSubClass: SubClass Code
		0x00,    ///bInterfaceProtocol: Protocol Code
		0x00,    ///iInterface: String index

		///ENDPOINT DESCRIPTOR(1)
		0x07,    ///bLength: Length of this descriptor													//array index 33
		0x05,    ///bDescriptorType: Endpoint Descriptor Type
		0x82,    ///bEndpointAddress: Endpoint address (IN,EP1)											//array index 35
		0x02,    ///bmAttributes: Transfer Type: BULK_TRANSFER
		0x40,    ///wMaxPacketSize: Endpoint Size
		0x00,    ///wMaxPacketSize: Endpoint Size
		0x00,    ///bIntervall: Polling Intervall

		///ENDPOINT DESCRIPTOR(2)
		0x07,    ///bLength: Length of this descriptor													//array index 40
		0x05,    ///bDescriptorType: Endpoint Descriptor Type
		0x02,    ///bEndpointAddress: Endpoint address (OUT,EP1)										//array index 42
		0x02,    ///bmAttributes: Transfer Type: BULK_TRANSFER
		0x40,    ///wMaxPacketSize: Endpoint Size
		0x00,    ///wMaxPacketSize: Endpoint Size
		0x00,    ///bIntervall: Polling Intervall														//array index 46
		/*---------------------------------------------------------------------------*/

};


uint8_t USB_DEVICE_CLASS_CDC_RNDIS::SOF() {
	return 0;
}

uint8_t USB_DEVICE_CLASS_CDC_RNDIS::SetupCmd(uint16_t index, uint8_t cmd, uint8_t* buffer, uint16_t &len){
	uint32_t *buf32=(uint32_t *)buffer;
	int pos=0;
	switch (cmd)
	{
	case SEND_ENCAPSULATED_COMMAND:
		rndis_data.MessageType=buf32[0];
		rndis_data.RequestId=buf32[2];
		if(buf32[0]==RNDIS_MSG_INIT){
			TPP_DEBUGF(RNDIS_DEBUG | TPP_DBG_TRACE,("SEC RNDIS_MSG_INIT\n"));
			rndis_data.MajorVersion=buf32[3];
			rndis_data.MinorVersion=buf32[4];
			rndis_data.MaxTransferSize=buf32[5];
			_usb->EP_Tx(_inendpoints[0], (uint8_t*)response,8);
		} else if(buf32[0]==RNDIS_MSG_HALT){
			TPP_DEBUGF(RNDIS_DEBUG | TPP_DBG_TRACE,("SEC RNDIS_MSG_HALT\n"));
		} else if(buf32[0]==RNDIS_MSG_QUERY){
			TPP_DEBUGF(RNDIS_DEBUG | TPP_DBG_TRACE,("SEC RNDIS_MSG_QUERY\n"));
			rndis_data.Oid=buf32[3];
			rndis_data.InformationBufferLength=buf32[4];
			rndis_data.InformationBufferOffset=buf32[5];
			rndis_data.DeviceVcHandle=buf32[6];
			_usb->EP_Tx(_inendpoints[0], (uint8_t*)response,8);
		} else if(buf32[0]==RNDIS_MSG_SET){
			TPP_DEBUGF(RNDIS_DEBUG | TPP_DBG_TRACE,("SEC RNDIS_MSG_SET\n"));
			_usb->EP_Tx(_inendpoints[0], (uint8_t*)response,8);
		} else if(buf32[0]==RNDIS_MSG_RESET){
			TPP_DEBUGF(RNDIS_DEBUG | TPP_DBG_TRACE,("SEC RNDIS_MSG_RESET\n"));
			_usb->EP_Tx(_inendpoints[0], (uint8_t*)response,8);
		} else if(buf32[0]==RNDIS_MSG_KEEPALIVE){
			TPP_DEBUGF(RNDIS_DEBUG | TPP_DBG_TRACE,("SEC RNDIS_MSG_KEEPALIVE\n"));
			_usb->EP_Tx(_inendpoints[0], (uint8_t*)response,8);
		}
		break;
	case GET_ENCAPSULATED_RESPONSE:
		len=0;
		if(rndis_data.MessageType==RNDIS_MSG_INIT){
			TPP_DEBUGF(RNDIS_DEBUG | TPP_DBG_TRACE,("GER RNDIS_MSG_INIT\n"));
			buf32[pos++]=RNDIS_MSG_INIT_C;							//MessageType			Specifies the type of message being sent. Set to 0x80000002.
			pos++;
			buf32[pos++]=rndis_data.RequestId;						//RequestId				Specifies the Remote NDIS message ID value. This value is used to match messages sent by the host with device responses.
			buf32[pos++]=RNDIS_STATUS_SUCCESS;						//Status				Specifies RNDIS_STATUS_SUCCESS if the device initialized successfully; otherwise, it specifies an error code that indicates the failure.
			buf32[pos++]=1;											//MajorVersion			Specifies the highest Remote NDIS major protocol version supported by the device.
			buf32[pos++]=0;											//MinorVersion			Specifies the highest Remote NDIS minor protocol version supported by the device.
			buf32[pos++]=RNDIS_DF_CONNECTIONLESS;					//DeviceFlags			Specifies the miniport driver type as either connectionless or connection-oriented. This value can be one of the following:
																	//						RNDIS_DF_CONNECTIONLESS 0x00000001
																	//						RNDIS_DF_CONNECTION_ORIENTED 0x00000002
			buf32[pos++]=RNDIS_MEDIUM_802_3;						//Medium				Specifies the medium supported by the device. Set to RNDIS_MEDIUM_802_3 (0x00000000)
			buf32[pos++]=1;											//MaxPacketsPerMessage	Specifies the maximum number of Remote NDIS data messages that the device can handle in a single transfer to it. This value should be at least one.
			buf32[pos++]=1580;										//MaxTransferSize		Specifies the maximum size in bytes of any single bus data transfer that the device expects to receive from the host.
			buf32[pos++]=0;											//PacketAlignmentFactor	Specifies the byte alignment that the device expects for each Remote NDIS message that is part of a multimessage transfer to it. This value is specified in powers of 2. For example, this value is set to three to indicate 8-byte alignment. This value has a maximum setting of seven, which specifies 128-byte alignment.
			buf32[pos++]=0;											//AFListOffset			Reserved for connection-oriented devices. Set value to zero.
			buf32[pos++]=0;											//AFListSize			Reserved for connection-oriented devices. Set value to zero.
		} else if(rndis_data.MessageType==RNDIS_MSG_QUERY){
			TPP_DEBUGF(RNDIS_DEBUG | TPP_DBG_TRACE,("GER RNDIS_MSG_QUERY OID: %lx\n", rndis_data.Oid));
			uint32_t temp=0;

			buf32[pos++]=RNDIS_MSG_QUERY_C;
			pos++;
			buf32[pos++]=rndis_data.RequestId;
			buf32[pos++]=RNDIS_STATUS_SUCCESS;

			switch(rndis_data.Oid){
			case RNDIS_OID_GEN_SUPPORTED_LIST:
				temp=OID_GEN_SUPPORTED.size()*sizeof(OID_GEN_SUPPORTED.begin());
				buf32[pos++]=temp;
				buf32[pos++]=16;
				std::copy(OID_GEN_SUPPORTED.begin(), OID_GEN_SUPPORTED.end(), buf32+pos);
				pos+=temp;
				break;
			case RNDIS_OID_GEN_PHYSICAL_MEDIUM:
				buf32[pos++]=4;
				buf32[pos++]=16;
				buf32[pos++]=0;
				break;
			case RNDIS_OID_GEN_MAXIMUM_FRAME_SIZE:
				buf32[pos++]=4;
				buf32[pos++]=16;
				buf32[pos++]=1500;
				break;
			case RNDIS_OID_GEN_LINK_SPEED:
				buf32[pos++]=4;
				buf32[pos++]=16;
				buf32[pos++]=100000/100;
				break;
			case RNDIS_OID_GEN_MEDIA_CONNECT_STATUS:
				buf32[pos++]=4;
				buf32[pos++]=16;
				buf32[pos++]=NdisMediaStateConnected;
				break;
			case RNDIS_OID_802_3_MAXIMUM_LIST_SIZE:
				buf32[pos++]=4;
				buf32[pos++]=16;
				buf32[pos++]=1;
				break;
			case RNDIS_OID_802_3_CURRENT_ADDRESS:
				buf32[pos++]=6;
				buf32[pos++]=16;
				buf32[pos++]=0x00757840 | (DeviceID_8[0]<<24);
				buf32[pos++]=DeviceID_8[2]<<8 | DeviceID_8[1];
				len=buf32[1]=pos*4-2;
				break;
			case RNDIS_OID_802_3_PERMANENT_ADDRESS:
				buf32[pos++]=6;
				buf32[pos++]=16;
				buf32[pos++]=0xDD757840;
				buf32[pos++]=0xFFEE;
				len=buf32[1]=pos*4-2;
				break;
			case RNDIS_OID_GEN_MAXIMUM_TOTAL_SIZE:
				buf32[pos++]=4;
				buf32[pos++]=16;
				buf32[pos++]=1558;
				break;
			case RNDIS_OID_GEN_XMIT_OK:
				buf32[pos++]=4;
				buf32[pos++]=16;
				buf32[pos++]=rndis_oid_gen_xmit_ok & 0xffffffff;
				break;
			case RNDIS_OID_GEN_RCV_OK:
				buf32[pos++]=4;
				buf32[pos++]=16;
				buf32[pos++]=rndis_oid_gen_rcv_ok & 0xffffffff;
				break;
			case RNDIS_OID_GEN_RCV_ERROR:
				buf32[pos++]=4;
				buf32[pos++]=16;
				buf32[pos++]=0;
				break;
			case RNDIS_OID_GEN_RCV_NO_BUFFER:
				buf32[pos++]=4;
				buf32[pos++]=16;
				buf32[pos++]=0;
				break;
			case RNDIS_OID_GEN_XMIT_ERROR:
				buf32[pos++]=4;
				buf32[pos++]=16;
				buf32[pos++]=0;
				break;
			case RNDIS_OID_GEN_VENDOR_ID:
				buf32[pos++]=3;
				buf32[pos++]=20;
				buf32[pos++]=0x00757840;
				len=buf32[1]=pos*4-1;
				break;
			case RNDIS_OID_GEN_VENDOR_DESCRIPTION:
				static const char nome[]="IMBEL TPP-1400";
				buf32[pos++]=sizeof(nome);
				buf32[pos++]=16;
				std::copy(nome, nome+sizeof(nome), (char*)&buf32[pos++]);
				len=buf32[1]=pos*4+sizeof(nome);
				break;
			default:
				buf32[pos++]=0;
				buf32[pos++]=20;
				break;
			}
		} else if(rndis_data.MessageType==RNDIS_MSG_SET){
			TPP_DEBUGF(RNDIS_DEBUG | TPP_DBG_TRACE,("GER RNDIS_MSG_SET OID: %lx\n", rndis_data.Oid));
			buf32[pos++]=RNDIS_MSG_SET_C;
			pos++;
			buf32[pos++]=rndis_data.RequestId;
			buf32[pos++]=RNDIS_STATUS_SUCCESS;
			switch(rndis_data.Oid){
			case RNDIS_OID_GEN_MEDIA_CONNECT_STATUS:
				break;
			case RNDIS_OID_GEN_MAXIMUM_TOTAL_SIZE:
				break;
			default:
				break;
			}
		} else if(rndis_data.MessageType==RNDIS_MSG_RESET){
			TPP_DEBUGF(RNDIS_DEBUG | TPP_DBG_TRACE,("GER RNDIS_MSG_RESET\n"));
			buf32[pos++]=RNDIS_MSG_RESET_C;
			pos++;
			buf32[pos++]=RNDIS_STATUS_SUCCESS;
			buf32[pos++]=0;
		} else if(rndis_data.MessageType==RNDIS_MSG_KEEPALIVE){
			TPP_DEBUGF(RNDIS_DEBUG | TPP_DBG_TRACE,("GER RNDIS_MSG_KEEPALIVE\n"));
			if(_inbuffer.Ocupied()==1){
				TPP_DEBUGF(RNDIS_DEBUG | TPP_DBG_TRACE,("INBUFFER CLEAR!\n"));
				_inbuffer.Clear();
			}
			buf32[pos++]=RNDIS_MSG_KEEPALIVE_C;
			pos++;
			buf32[pos++]=rndis_data.RequestId;
			buf32[pos++]=RNDIS_STATUS_SUCCESS;
		}
		if(!len) len=buf32[1]=pos*4;
		_usb->CtlSendData(buffer, len);
		break;
	default:
		break;
	}
	return len;
}


const std::list<uint32_t> USB_DEVICE_CLASS_CDC_RNDIS::OID_GEN_SUPPORTED{
			RNDIS_OID_GEN_SUPPORTED_LIST,
			RNDIS_OID_GEN_HARDWARE_STATUS,
			RNDIS_OID_GEN_MEDIA_SUPPORTED,
			RNDIS_OID_GEN_MEDIA_IN_USE,
			RNDIS_OID_GEN_MAXIMUM_FRAME_SIZE,
			RNDIS_OID_GEN_LINK_SPEED,
			RNDIS_OID_GEN_TRANSMIT_BLOCK_SIZE,
			RNDIS_OID_GEN_RECEIVE_BLOCK_SIZE,
			RNDIS_OID_GEN_VENDOR_ID,
			RNDIS_OID_GEN_VENDOR_DESCRIPTION,
			RNDIS_OID_GEN_VENDOR_DRIVER_VERSION,
			RNDIS_OID_GEN_CURRENT_PACKET_FILTER,
			RNDIS_OID_GEN_MAXIMUM_TOTAL_SIZE,
			RNDIS_OID_GEN_MEDIA_CONNECT_STATUS,
			RNDIS_OID_GEN_PHYSICAL_MEDIUM,
			RNDIS_OID_GEN_XMIT_OK,
			RNDIS_OID_GEN_RCV_OK,
//			RNDIS_OID_GEN_XMIT_ERROR,
//			RNDIS_OID_GEN_RCV_ERROR,
//			RNDIS_OID_GEN_RCV_NO_BUFFER,
			RNDIS_OID_802_3_PERMANENT_ADDRESS,
			RNDIS_OID_802_3_CURRENT_ADDRESS,
			RNDIS_OID_802_3_MULTICAST_LIST,
			RNDIS_OID_802_3_MAC_OPTIONS,
			RNDIS_OID_802_3_MAXIMUM_LIST_SIZE,
			RNDIS_OID_802_3_RCV_ERROR_ALIGNMENT,
			RNDIS_OID_802_3_XMIT_ONE_COLLISION,
			RNDIS_OID_802_3_XMIT_MORE_COLLISIONS,
 };


uint8_t USB_DEVICE_CLASS_CDC_RNDIS::InterfaceRequest(USB::USB_SETUP_REQUEST* req) {
	switch (req->bRequest)		{
	case USB::USB_REQ_GET_INTERFACE :
		_usb->CtlSendData ((uint8_t *)&usbd_cdc_rndis_AltSet, 1);
		break;
	case USB::USB_REQ_SET_INTERFACE :
		usbd_cdc_rndis_AltSet = (uint8_t)(req->wValue);
		break;
	default:
		_usb->CtlError (req);
		break;
	}
	return 0;
}

uint16_t USB_DEVICE_CLASS_CDC_RNDIS::GetData(CircularBuffer<uint8_t> &CB, uint16_t len) {
	if(_packetreceived){
		len=_outbuffer.Out(CB, len);
		_outbuffer.Clear();
		rndis_oid_gen_xmit_ok++;
		_packetreceived=0;
		_usb->EP_PrepareRx(_outendpoints[1], _receivebuffer, _maxendpointsize);
		return len;
	}
	return 0;
}

uint16_t USB_DEVICE_CLASS_CDC_RNDIS::GetData(uint8_t* buffer, uint16_t len) {
	if(_packetreceived){
		len=_outbuffer.Out(buffer, len);
		_outbuffer.Clear();
		rndis_oid_gen_xmit_ok++;
		_packetreceived=0;
		_usb->EP_PrepareRx(_outendpoints[1], _receivebuffer, _maxendpointsize);
		return len;
	}
	return 0;
}
uint16_t USB_DEVICE_CLASS_CDC_RNDIS::CanSendData() {
	if(_inbuffer.Ocupied()){
		return 0;
	} else {
		return _inbuffer.Free();
	}
}
uint16_t USB_DEVICE_CLASS_CDC_RNDIS::SendData(uint8_t* buffer, uint16_t len) {
	uint32_t buffer_temp[11];

	buffer_temp[0]=0x00000001;	//MessageType
	buffer_temp[1]=len+44;		//MessageLength
	buffer_temp[2]=36;			//DataOffset
	buffer_temp[3]=len;			//DataLength
	buffer_temp[4]=0;			//OOBDataOffset
	buffer_temp[5]=0;			//OOBDataLength
	buffer_temp[6]=0;			//NumOOBDataElements
	buffer_temp[7]=0;			//PerPacketInfoOffset
	buffer_temp[8]=0;			//PerPacketInfoLength
	buffer_temp[9]=0;			//VcHandle
	buffer_temp[10]=0;			//Reserved

	_inbuffer.Clear();
	_inbuffer.In((uint8_t*)buffer_temp, sizeof(buffer_temp));
	len=_inbuffer.In(buffer, len);
	if(_inbuffer.Ocupied()%_maxendpointsize==0){
		_inbuffer.In(0);
	}
	if(_usb) _usb->EP_Tx(_inendpoints[1], _inbuffer.GetRearrangedBuffer(), _inbuffer.Ocupied());
	return len;
}
