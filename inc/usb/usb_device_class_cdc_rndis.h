#pragma once
#include <usb/rndis.h>
#include <usb/usb_device_class.h>
#include <utils/circularbuffer.h>

class USB_DEVICE_CLASS_CDC_RNDIS: public USB_DEVICE_CLASS{
public:
	USB_DEVICE_CLASS_CDC_RNDIS(uint8_t enabled=1);
	uint16_t GetData(uint8_t *buffer, uint16_t len);
	uint16_t GetData(CircularBuffer<uint8_t> &CB, uint16_t len);

	uint16_t SendData(uint8_t *buffer, uint16_t len);
	uint16_t CanSendData();

protected:
	typedef struct {
		uint32_t MessageType;						//Specifies the type of message being sent.
		uint32_t RequestId;							//Specifies the Remote NDIS message ID value. This value is used to match messages sent by the host with device responses.
		uint32_t MajorVersion;						//Specifies the Remote NDIS protocol version implemented by the host. The current specification uses RNDIS_MAJOR_VERSION = 1.
		uint32_t MinorVersion;						//Specifies the Remote NDIS protocol version implemented by the host. The current specification uses RNDIS_MINOR_VERSION = 0.
		uint32_t MaxTransferSize;					//Specifies the maximum size in bytes of any single bus data transfer that the host expects to receive from the device. Typically, each bus data transfer accommodates a single Remote NDIS message. However, the device may bundle several Remote NDIS messages that contain data packets into a single transfer (see REMOTE_NDIS_PACKET_MSG).
		uint32_t Oid;								//Specifies the NDIS OID that identifies the parameter being queried.
		uint32_t InformationBufferLength;			//Specifies in bytes the length of the input data for the query. Set to zero when there is no OID input buffer.
		uint32_t InformationBufferOffset;			//Specifies the byte offset, from the beginning of the RequestId field, at which input data for the query is located. Set to zero if there is no OID input buffer.
		uint32_t DeviceVcHandle;					//Reserved for connection-oriented devices. Set to zero.
	} RNDIS_DATA;


	uint8_t Init(USB *usb);
	uint8_t DeInit();
	uint8_t  DataIn(uint8_t epnum);
	uint8_t  DataOut(uint8_t epnum, uint16_t xfer_count);
	uint16_t GetDescriptorSize();
	uint8_t* GetDescriptor();
	uint8_t SOF();
	uint8_t SetupCmd(uint16_t index, uint8_t cmd, uint8_t* buffer, uint16_t &len);
	uint8_t InterfaceRequest(USB::USB_SETUP_REQUEST* req);

	static uint8_t _descriptor[];
	CircularBuffer<uint8_t,false> _inbuffer;
	CircularBuffer<uint8_t,false> _outbuffer;

	uint8_t _receivebuffer[_maxendpointsize];
	uint8_t _transmitbuffer[_maxendpointsize];
	uint16_t _receivebufferremaining;
	uint16_t _transmitbufferremaining;

	RNDIS_DATA rndis_data;
	uint64_t rndis_oid_gen_xmit_ok;
	uint64_t rndis_oid_gen_rcv_ok;
	uint32_t usbd_cdc_rndis_AltSet;
	uint8_t _packetreceived;
	uint8_t _startrxout;
	uint8_t _starttxin;



	enum{
		SEND_ENCAPSULATED_COMMAND               =0x00,
		GET_ENCAPSULATED_RESPONSE               =0x01,
	};

	static const std::list<uint32_t> OID_GEN_SUPPORTED;
	static const uint32_t response[];
};

