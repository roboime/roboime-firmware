#include <hal_stm32/usb_stm32.h>
#include <stm32f4xx_rcc.h>

extern "C" {
#include <usbd_req.h>
#include <usbd_usr.h>
#include <usbd_ioreq.h>
}

USB_OTG_CORE_HANDLE USB_OTG_dev;
USBH_HOST USB_Host;

uint16_t USB_STM32::_vid=0x29BC;
uint16_t USB_STM32::_pid=0xFFFF;
uint8_t USB_STM32::USBD_StrDesc[128];
uint8_t* USB_STM32::_manufacturerstring((uint8_t*)"IMBEL");
uint8_t* USB_STM32::_productstring((uint8_t*)"TPP-1400");
uint8_t* USB_STM32::_serialnumberstring((uint8_t*)"0000");
uint8_t* USB_STM32::_configurationstring((uint8_t*)"Configuration");
uint8_t* USB_STM32::_interfacestring((uint8_t*)"Interface");
CircularBuffer<uint8_t> USB_STM32::_descriptorbuffer(0,1024);
uint8_t USB_STM32::_setupbuffer[256];
uint16_t USB_STM32::_setupcmdlen=0;
uint8_t USB_STM32::_setupcmd=0;
uint16_t USB_STM32::_setupcmdindex=0;


USB_STM32::USB_STM32(uint16_t vid, uint16_t pid, const char* manufacturerstring,  const char* productstring,  const char* serialnumberstring)
{
	_usb=this;
	if(manufacturerstring)
		_manufacturerstring=(uint8_t*)manufacturerstring;
	if(productstring)
		_productstring=(uint8_t*)productstring;
	if(serialnumberstring)
		_serialnumberstring=(uint8_t*)serialnumberstring;
	_vid=vid;
	_pid=pid;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_PWR, ENABLE);
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_OTG_FS, ENABLE) ;
}

void USB_STM32::Init() {
	USBD_Init(&USB_OTG_dev, USB_OTG_CORE_ID, (USBD_DEVICE*)&usbd_device_stm32, (USBD_Class_cb_TypeDef*) &usbd_class_stm32_cb, (USBD_Usr_cb_TypeDef*)&usbd_usr_stm32_cb);
}

void USB_STM32::EP_Open(uint8_t endpoint, uint16_t size, USB_ENDPOINT_TYPE type) {
	DCD_EP_Open(&USB_OTG_dev, endpoint, size, type);
}

void USB_STM32::EP_Close(uint8_t endpoint) {
	DCD_EP_Close(&USB_OTG_dev, endpoint);
}

void USB_STM32::EP_Flush(uint8_t endpoint) {
	DCD_EP_Flush(&USB_OTG_dev, endpoint);
}

void USB_STM32::EP_PrepareRx(uint8_t endpoint, uint8_t* buffer, uint16_t size) {
	DCD_EP_PrepareRx(&USB_OTG_dev, endpoint, buffer, size);
}

void USB_STM32::EP_Tx(uint8_t endpoint, uint8_t* buffer, uint16_t size) {
	DCD_EP_Tx(&USB_OTG_dev, endpoint, buffer, size);
}

void USB_STM32::CtlPrepareRx(uint8_t* buffer, uint16_t size) {
	USBD_CtlPrepareRx(&USB_OTG_dev, buffer, size);
}

void USB_STM32::CtlSendData(uint8_t* buffer, uint16_t size) {
	USBD_CtlSendData(&USB_OTG_dev, buffer, size);
}

void USB_STM32::CtlError(USB_SETUP_REQUEST* req) {
	USBD_CtlError(&USB_OTG_dev, (USB_SETUP_REQ*) req);

}

uint8_t* USB_STM32::USBD_STM32_DeviceDescriptor(uint8_t speed, uint16_t* length) {
	static const uint8_t descriptor[]=
	{
			0x12,                       /*bLength */
			0x01, /*bDescriptorType*/
			0x00,                       /*bcdUSB */
			0x02,
			0xEF,                       /*bDeviceClass*/
			0x02,                       /*bDeviceClass*/
			0x01,                       /*bDeviceClass*/
			USB_OTG_MAX_EP0_SIZE,      /*bMaxPacketSize*/
			(uint8_t)_vid,           /*idVendor Low Byte*/
			(uint8_t)(_vid>>8),           /*idVendor High Byte*/
			(uint8_t)_pid,           /*idProduct Low Byte*/
			(uint8_t)(_pid>>8),           /*idProduct High Byte*/
			0x00,                       /*bcdDevice rel. 2.00*/
			0x00,
			USBD_IDX_MFC_STR,           /*Index of manufacturer  string*/
			USBD_IDX_PRODUCT_STR,       /*Index of product string*/
			USBD_IDX_SERIAL_STR,        /*Index of serial number string*/
			0x01            /*bNumConfigurations*/
	} ; /* USB_DeviceDescriptor */
	*length=sizeof(descriptor);
	return (uint8_t*)descriptor;
}

uint8_t* USB_STM32::USBD_STM32_LangIDStrDescriptor(uint8_t speed, uint16_t* length) {
	static const uint8_t langid[]={0x04, 0x03, 0x09, 0x04};
	*length=4;
	return (uint8_t*)langid;
}

uint8_t* USB_STM32::USBD_STM32_ManufacturerStrDescriptor(uint8_t speed, uint16_t* length) {
	USBD_GetString (_manufacturerstring, USBD_StrDesc, length);
	return USBD_StrDesc;
}

uint8_t* USB_STM32::USBD_STM32_ProductStrDescriptor(uint8_t speed, uint16_t* length) {
	USBD_GetString (_productstring, USBD_StrDesc, length);
	return USBD_StrDesc;
}

uint8_t* USB_STM32::USBD_STM32_SerialStrDescriptor(uint8_t speed, uint16_t* length) {
	USBD_GetString (_serialnumberstring, USBD_StrDesc, length);
	return USBD_StrDesc;
}

uint8_t* USB_STM32::USBD_STM32_ConfigStrDescriptor(uint8_t speed, uint16_t* length) {
	USBD_GetString (_configurationstring, USBD_StrDesc, length);
	return USBD_StrDesc;
}


uint8_t* USB_STM32::USBD_STM32_InterfaceStrDescriptor(uint8_t speed, uint16_t* length) {
	USBD_GetString (_interfacestring, USBD_StrDesc, length);
	return USBD_StrDesc;
}

const USBD_DEVICE USB_STM32::usbd_device_stm32={
		USBD_STM32_DeviceDescriptor,
		USBD_STM32_LangIDStrDescriptor,
		USBD_STM32_ManufacturerStrDescriptor,
		USBD_STM32_ProductStrDescriptor,
		USBD_STM32_SerialStrDescriptor,
		USBD_STM32_ConfigStrDescriptor,
		USBD_STM32_InterfaceStrDescriptor,
};

const USBD_Class_cb_TypeDef  USB_STM32::usbd_class_stm32_cb ={
		usbd_stm32_Init,
		usbd_stm32_DeInit,
		usbd_stm32_Setup,
		usbd_stm32_EP0_TxSent,
		usbd_stm32_EP0_RxReady,
		usbd_stm32_DataIn,
		usbd_stm32_DataOut,
		usbd_stm32_SOF,
		usbd_stm32_IsoINIncomplete,
		usbd_stm32_IsoOUTIncomplete,
		usbd_stm32_GetConfigDescriptor,
//		usbd_stm32_GetOtherConfigDescriptor,
		usbd_stm32_GetUsrStrDescriptor,
 };

const USBD_Usr_cb_TypeDef  USB_STM32::usbd_usr_stm32_cb ={
		usbd_usr_stm32_Init,
		usbd_usr_stm32_DeviceReset,
		usbd_usr_stm32_DeviceConfigured,
		usbd_usr_stm32_DeviceSuspended,
		usbd_usr_stm32_DeviceResumed,
		usbd_usr_stm32_DeviceConnected,
		usbd_usr_stm32_DeviceDisconnected,
};

uint8_t USB_STM32::usbd_stm32_Setup(void* pdev, USB_SETUP_REQ* req) {
	std::list<USB_DEVICE_CLASS*> *_usb_device_classes_list(&USB_DEVICE_CLASS::GetDeviceClassesList());
	if(_usb_device_classes_list){
		for(auto device =_usb_device_classes_list->begin(); device!=_usb_device_classes_list->end(); device++){
			if((*device)->InterfaceBelongs(req->wIndex)){
				switch (req->bmRequest & USB_REQ_TYPE_MASK)	{
				/* CDC Class Requests -------------------------------*/
				case USB_REQ_TYPE_CLASS :
					/* Check if the request is a data setup packet */
					if (req->wLength)
					{
						/* Check if the request is Device-to-Host */
						if (req->bmRequest & 0x80){
							(*device)->SetupCmd(req->wIndex, req->bRequest, _setupbuffer, _setupcmdlen);
						} else {/* Host-to-Device requeset */
							/* Set the value of the current command to be processed */
							_setupcmd = req->bRequest;
							_setupcmdlen = req->wLength;
							_setupcmdindex = req->wIndex;

							/* Prepare the reception of the buffer over EP0
					  	  	   Next step: the received data will be managed in usbd_cdc_EP0_TxSent()
					  	  	   function. */
							USBD_CtlPrepareRx ((USB_OTG_CORE_HANDLE*)pdev, _setupbuffer, req->wLength);
						}
					} else { /* No Data request */
						/* Transfer the command to the interface layer */
						_setupcmdlen=0;
						(*device)->SetupCmd(req->wIndex, req->bRequest, (uint8_t*)&(req->wValue), _setupcmdlen);
					}
					break;
					/* Standard Requests -------------------------------*/
				case USB_REQ_TYPE_STANDARD:
					(*device)->InterfaceRequest((USB::USB_SETUP_REQUEST*)req);
					break;
				default:
					USBD_CtlError ((USB_OTG_CORE_HANDLE*)pdev, req);
					return USB::USBD_FAIL;
					break;
				}
			}
		}
	}
	return 0;
}

uint8_t USB_STM32::usbd_stm32_Init(void* pdev, uint8_t cfgidx) {
	std::list<USB_DEVICE_CLASS*> *_usb_device_classes_list(&USB_DEVICE_CLASS::GetDeviceClassesList());
	if(_usb_device_classes_list){
		for(auto device =_usb_device_classes_list->begin(); device!=_usb_device_classes_list->end(); device++){
			(*device)->Init(_usb);
		}
	}
	return 0;
}

uint8_t USB_STM32::usbd_stm32_DeInit(void* pdev, uint8_t cfgidx) {
	std::list<USB_DEVICE_CLASS*> *_usb_device_classes_list(&USB_DEVICE_CLASS::GetDeviceClassesList());
	if(_usb_device_classes_list){
		for(auto device =_usb_device_classes_list->begin(); device!=_usb_device_classes_list->end(); device++){
			(*device)->DeInit();
		}
	}
	return 0;
}

uint8_t USB_STM32::usbd_stm32_DataOut(void* pdev, uint8_t epnum) {
	std::list<USB_DEVICE_CLASS*> *_usb_device_classes_list(&USB_DEVICE_CLASS::GetDeviceClassesList());
    ((USB_OTG_CORE_HANDLE*)pdev)->dev.out_ep[epnum].even_odd_frame =(((USB_OTG_CORE_HANDLE*)pdev)->dev.out_ep[epnum].even_odd_frame)? 0:1;
	if(_usb_device_classes_list){
		for(auto device =_usb_device_classes_list->begin(); device!=_usb_device_classes_list->end(); device++){
			(*device)->DataOut(epnum, ((USB_OTG_CORE_HANDLE*)pdev)->dev.out_ep[epnum].xfer_count);
		}
	}
	return 0;
}

uint8_t USB_STM32::usbd_stm32_DataIn(void* pdev, uint8_t epnum) {
	std::list<USB_DEVICE_CLASS*> *_usb_device_classes_list(&USB_DEVICE_CLASS::GetDeviceClassesList());
	if(_usb_device_classes_list){
		for(auto device =_usb_device_classes_list->begin(); device!=_usb_device_classes_list->end(); device++){
			(*device)->DataIn(epnum);
		}
	}
	return 0;
}

uint8_t USB_STM32::usbd_stm32_IsoINIncomplete(void* pdev) {
	std::list<USB_DEVICE_CLASS*> *_usb_device_classes_list(&USB_DEVICE_CLASS::GetDeviceClassesList());
	if(_usb_device_classes_list){
		for(auto device =_usb_device_classes_list->begin(); device!=_usb_device_classes_list->end(); device++){
			(*device)->ISO_IN();
		}
	}
	return 0;
}

uint8_t USB_STM32::usbd_stm32_IsoOUTIncomplete(void* pdev) {
	std::list<USB_DEVICE_CLASS*> *_usb_device_classes_list(&USB_DEVICE_CLASS::GetDeviceClassesList());
	if(_usb_device_classes_list){
		for(auto device =_usb_device_classes_list->begin(); device!=_usb_device_classes_list->end(); device++){
			(*device)->ISO_OUT();
		}
	}
	return 0;
}

uint8_t USB_STM32::usbd_stm32_SOF(void* pdev) {
	std::list<USB_DEVICE_CLASS*> *_usb_device_classes_list(&USB_DEVICE_CLASS::GetDeviceClassesList());
	if(_usb_device_classes_list){
		for(auto device =_usb_device_classes_list->begin(); device!=_usb_device_classes_list->end(); device++){
			(*device)->SOF();
		}
	}
	return 0;
}

uint8_t USB_STM32::usbd_stm32_EP0_TxSent(void* pdev) {
	std::list<USB_DEVICE_CLASS*> *_usb_device_classes_list(&USB_DEVICE_CLASS::GetDeviceClassesList());
	if(_usb_device_classes_list){
		for(auto device =_usb_device_classes_list->begin(); device!=_usb_device_classes_list->end(); device++){
			if((*device)->InterfaceBelongs(_setupcmdindex)){
				(*device)->EP0_TxSent();
			}
		}
	}
	return 0;
}

uint8_t USB_STM32::usbd_stm32_EP0_RxReady(void* pdev) {
	std::list<USB_DEVICE_CLASS*> *_usb_device_classes_list(&USB_DEVICE_CLASS::GetDeviceClassesList());
	if(_usb_device_classes_list){
		for(auto device =_usb_device_classes_list->begin(); device!=_usb_device_classes_list->end(); device++){
			if((*device)->InterfaceBelongs(_setupcmdindex)){
				(*device)->SetupCmd(_setupcmdindex, _setupcmd, _setupbuffer, _setupcmdlen);
			}
			(*device)->EP0_RxReady();
		}
	}
	return 0;
}


uint8_t* USB_STM32::usbd_stm32_GetConfigDescriptor(uint8_t speed, uint16_t* length) {
	_descriptorbuffer.Clear();
	static const uint8_t desc[]=
	{
			0x09,                       /*bLength */
			0x02, 						/*bDescriptorType*/
			0x00,						/*bConfigurationValue*/
			0x00,						/*bConfigurationValue*/
			0x00,						/*NumInterfaces*/
			0x01,						/*bConfigurationValue*/
			USBD_IDX_CONFIG_STR,
			0x80,
			0xFA,
	};
	static const uint8_t desc_vendor[]={
			//INTERFACE ASSOCIATION DESCRIPTOR
			0x08,												//array index 0
			0x0B,	///bDescriptorType
			0x00,	///bFirstInterface
			0x01,	///bInterfaceCount
			0x00,	///bFunctionClass
			0x00,	///bFunctionSubClass
			0x00,	///bFunctionProtocol
			USBD_IDX_PRODUCT_STR,	///iFunction

			0x09,    ///bLength: Length of this descriptor
			0x04,    ///bDescriptorType: Interface Descriptor Type
			0x00,    ///bInterfaceNumber: Interface Number
			0x00,    ///bAlternateSetting: Alternate setting for this interface
			0x00,    ///bNumEndpoints: Number of endpoints in this interface excluding endpoint 0
			0xFF,    ///iInterfaceClass: Class Code: Vendor Specific
			0x00,    ///iInterfaceSubClass: SubClass Code
			0x00,    ///bInterfaceProtocol: Protocol Code
			USBD_IDX_INTERFACE_STR,    ///iInterface: String index
	};

	_descriptorbuffer.In(desc, sizeof(desc));

	std::list<USB_DEVICE_CLASS*> *_usb_device_classes_list(&USB_DEVICE_CLASS::GetDeviceClassesList());
	if(_usb_device_classes_list){
		for(auto device =_usb_device_classes_list->begin(); device!=_usb_device_classes_list->end(); device++){
			_descriptorbuffer.In((*device)->GetDescriptor(), (*device)->GetDescriptorSize());
		}
	}
	_descriptorbuffer[4]=USB_DEVICE_CLASS::GetNumInterfaces();
	if(_descriptorbuffer[4]==0){
		_descriptorbuffer.In(desc_vendor, sizeof(desc_vendor));
		_descriptorbuffer[4]=1;
	}
	_descriptorbuffer[2]=(uint8_t)_descriptorbuffer.Ocupied();
	_descriptorbuffer[3]=(uint8_t)(_descriptorbuffer.Ocupied()>>8);
	*length=_descriptorbuffer.Ocupied();
	return _descriptorbuffer.GetRearrangedBuffer();
}

uint8_t* USB_STM32::usbd_stm32_GetOtherConfigDescriptor(uint8_t speed, uint16_t* length) {
	return usbd_stm32_GetConfigDescriptor(speed, length);
}

uint8_t* USB_STM32::usbd_stm32_GetUsrStrDescriptor(uint8_t speed, uint8_t idx, uint16_t* length) {
	uint8_t *string=0;
	std::list<USB_DEVICE_CLASS*> *_usb_device_classes_list(&USB_DEVICE_CLASS::GetDeviceClassesList());
	if(_usb_device_classes_list){
		for(auto device =_usb_device_classes_list->begin(); device!=_usb_device_classes_list->end(); device++){
			string=(*device)->GetDescriptorString(idx);
			if(string){
				USBD_GetString (string, USBD_StrDesc, length);
				return USBD_StrDesc;
			}
		}
	}
	return 0;
}

void USB_STM32::usbd_usr_stm32_Init() {
}

void USB_STM32::usbd_usr_stm32_DeviceReset(uint8_t speed) {
}

void USB_STM32::usbd_usr_stm32_DeviceConfigured() {
}

void USB_STM32::usbd_usr_stm32_DeviceSuspended() {
}

void USB_STM32::usbd_usr_stm32_DeviceResumed() {
}

void USB_STM32::usbd_usr_stm32_DeviceConnected() {
}

void USB_STM32::usbd_usr_stm32_DeviceDisconnected() {
}

uint8_t USBD_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] =
{
		USB_LEN_DEV_QUALIFIER_DESC,
		USB_DESC_TYPE_DEVICE_QUALIFIER,
		0x00,
		0x02,
		0x00,
		0x00,
		0x00,
		0x40,
		0x01,
		0x00,
};

//void BSP_Init(void);

extern "C" void USB_OTG_BSP_Init (USB_OTG_CORE_HANDLE *pdev){};
extern "C" void USB_OTG_BSP_uDelay (const uint32_t usec){
	uint32_t count = 0;
	const uint32_t utime = (120 * usec / 7);
	do
	{
		if ( ++count > utime )
		{
			return ;
		}
	}
	while (1);
};
extern "C" void USB_OTG_BSP_mDelay (const uint32_t msec){
	USB_OTG_BSP_uDelay(msec * 1000);
};
extern "C" void USB_OTG_BSP_EnableInterrupt (USB_OTG_CORE_HANDLE *pdev){};

void USB_OTG_BSP_ConfigVBUS(USB_OTG_CORE_HANDLE *pdev){};
void USB_OTG_BSP_DriveVBUS(USB_OTG_CORE_HANDLE *pdev,uint8_t state){};


