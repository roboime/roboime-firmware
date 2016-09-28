#pragma once

#include <inttypes.h>


class USB{
public:
	typedef enum{
		USB_ENDPOINT_TYPE_CONTROL=0,
		USB_ENDPOINT_TYPE_ISOC,
		USB_ENDPOINT_TYPE_BULK,
		USB_ENDPOINT_TYPE_INT,
	} USB_ENDPOINT_TYPE;
	typedef  struct  usb_setup_req {

	    uint8_t   bmRequest;
	    uint8_t   bRequest;
	    uint16_t  wValue;
	    uint16_t  wIndex;
	    uint16_t  wLength;
	} USB_SETUP_REQUEST;
	enum{
		USBD_OK											=0x00,
		USBD_BUSY										=0x01,
		USBD_FAIL										=0x02,


		/* Table 9-4. Standard Request Codes  */
		/* bRequest , Value */
		USB_REQ_GET_STATUS                             =0x00,
		USB_REQ_CLEAR_FEATURE                          =0x01,
		USB_REQ_SET_FEATURE                            =0x03,
		USB_REQ_SET_ADDRESS                            =0x05,
		USB_REQ_GET_DESCRIPTOR                         =0x06,
		USB_REQ_SET_DESCRIPTOR                         =0x07,
		USB_REQ_GET_CONFIGURATION                      =0x08,
		USB_REQ_SET_CONFIGURATION                      =0x09,
		USB_REQ_GET_INTERFACE                          =0x0A,
		USB_REQ_SET_INTERFACE                          =0x0B,
		USB_REQ_SYNCH_FRAME                            =0x0C,

		/* bmRequestType D6..5 Type */
		USB_REQ_TYPE_STANDARD                          =0x00,
		USB_REQ_TYPE_CLASS                             =0x20,
		USB_REQ_TYPE_VENDOR                            =0x40,
		USB_REQ_TYPE_RESERVED                          =0x60,
		USB_REQ_TYPE_MASK							   =0x60,




	};

	virtual void EP_Open(uint8_t endpoint, uint16_t size, USB_ENDPOINT_TYPE type)=0;
	virtual void EP_Close(uint8_t endpoint)=0;
	virtual void EP_Flush(uint8_t endpoint)=0;
	virtual void EP_PrepareRx(uint8_t endpoint, uint8_t* buffer, uint16_t size)=0;
	virtual void EP_Tx(uint8_t endpoint, uint8_t* buffer, uint16_t size)=0;
	virtual void CtlPrepareRx(uint8_t* buffer, uint16_t size)=0;
	virtual void CtlSendData(uint8_t* buffer, uint16_t size)=0;
	virtual void CtlError(USB_SETUP_REQUEST *req)=0;
protected:
	static USB *_usb;
};

