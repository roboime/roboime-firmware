#pragma once
#include <usb/usb.h>
#include <usbh_core.h>
#include <utils/circularbuffer.h>
#include <list>
#include <usb/usb_device_class.h>

class USB_STM32: public USB{
public:
	USB_STM32(uint16_t vid, uint16_t pid, const char* manufacturerstring=0,  const char* productstring=0,  const char* serialnumberstring=0);
	void Init();
	void EP_Open(uint8_t endpoint, uint16_t size, USB_ENDPOINT_TYPE type);
	void EP_Close(uint8_t endpoint);
	void EP_Flush(uint8_t endpoint);
	void EP_PrepareRx(uint8_t endpoint, uint8_t* buffer, uint16_t size);
	void EP_Tx(uint8_t endpoint, uint8_t* buffer, uint16_t size);
	void CtlPrepareRx(uint8_t* buffer, uint16_t size);
	void CtlSendData(uint8_t* buffer, uint16_t size);
	void CtlError(USB_SETUP_REQUEST *req);
protected:
	static uint8_t *  USBD_STM32_DeviceDescriptor( uint8_t speed , uint16_t *length);
	static uint8_t *  USBD_STM32_LangIDStrDescriptor( uint8_t speed , uint16_t *length);
	static uint8_t *  USBD_STM32_ManufacturerStrDescriptor( uint8_t speed , uint16_t *length);
	static uint8_t *  USBD_STM32_ProductStrDescriptor( uint8_t speed , uint16_t *length);
	static uint8_t *  USBD_STM32_SerialStrDescriptor( uint8_t speed , uint16_t *length);
	static uint8_t *  USBD_STM32_ConfigStrDescriptor( uint8_t speed , uint16_t *length);
	static uint8_t *  USBD_STM32_InterfaceStrDescriptor( uint8_t speed , uint16_t *length);
	static uint8_t *  usbd_stm32_GetUsrStrDescriptor (uint8_t speed, uint8_t idx , uint16_t *length);
	static uint8_t  usbd_stm32_Setup (void  *pdev, USB_SETUP_REQ *req);
	static uint8_t  usbd_stm32_Init (void  *pdev, uint8_t cfgidx);
	static uint8_t  usbd_stm32_DeInit (void  *pdev, uint8_t cfgidx);
	static uint8_t  usbd_stm32_DataOut (void *pdev, uint8_t epnum);
	static uint8_t  usbd_stm32_DataIn (void *pdev, uint8_t epnum);
	static uint8_t  usbd_stm32_IsoINIncomplete (void *pdev);
	static uint8_t  usbd_stm32_IsoOUTIncomplete (void *pdev);
	static uint8_t  usbd_stm32_SOF (void *pdev);
	static uint8_t  *usbd_stm32_GetConfigDescriptor (uint8_t speed, uint16_t *length);
	static uint8_t  *usbd_stm32_GetOtherConfigDescriptor (uint8_t speed, uint16_t *length);
	static uint8_t  usbd_stm32_EP0_TxSent  (void *pdev );
	static uint8_t  usbd_stm32_EP0_RxReady (void *pdev );
	static void usbd_usr_stm32_Init();
	static void usbd_usr_stm32_DeviceReset(uint8_t speed);
	static void usbd_usr_stm32_DeviceConfigured();
	static void usbd_usr_stm32_DeviceSuspended();
	static void usbd_usr_stm32_DeviceResumed();
	static void usbd_usr_stm32_DeviceConnected();
	static void usbd_usr_stm32_DeviceDisconnected();

	static uint16_t _vid;
	static uint16_t _pid;

	static const USBD_DEVICE usbd_device_stm32;
	static const USBD_Class_cb_TypeDef usbd_class_stm32_cb;
	static const USBD_Usr_cb_TypeDef usbd_usr_stm32_cb;

	static uint8_t USBD_StrDesc[];
	static uint8_t _setupRequestLen;
	static uint8_t *_manufacturerstring;
	static uint8_t *_productstring;
	static uint8_t *_serialnumberstring;
	static uint8_t *_configurationstring;
	static uint8_t *_interfacestring;
	static CircularBuffer<uint8_t> _descriptorbuffer;
	static uint8_t _setupbuffer[];
	static uint16_t _setupcmdlen;
	static uint8_t _setupcmd;
	static uint16_t _setupcmdindex;


};
