#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usbd_cdc_vcp.h"
#include "usb_dcd_int.h"
#include "usbd_cdc_core.h"
#include "usbd_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usbd_cdc_vcp.h"
#include "usb_dcd_int.h"
#include "NRF24.h"

static __IO uint32_t TimingDelay;

void TimingDelay_Decrement(void);
void Delay_ms(uint32_t time_ms);

extern "C" {
 void SysTick_Handler(void);
 void OTG_FS_IRQHandler(void);
 void OTG_FS_WKUP_IRQHandler(void);
}

__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;

int main(void){
  SysTick_Config(SystemCoreClock/1000);
  USBD_Init(&USB_OTG_dev, USB_OTG_FS_CORE_ID, &USR_desc, &USBD_CDC_cb, &USR_cb);
  STM_EVAL_LEDInit(LED5);
  STM_EVAL_LEDInit(LED6);
  NRF24 radio;
  radio.is_rx=false;
  radio.Config();
  radio.NRF_CE->Set();
  while (1)
  {
	int i=0;
	uint8_t buf[] = {0,0,0,0,0};
	uint8_t symbol;
	while(i<6){
	  if(VCP_get_char(&symbol)){
		if((symbol=='a')&&(i==0)){
	      i=1;
		}
		else if(i>0){
		  buf[i-1]=symbol;
		  i=i+1;
		}
	  }
	}
	VCP_send_buffer(buf, 5);
	radio.NRF_CE->Set();
    radio.WritePayload(buf, 5);
    int counter;
    while(radio.TxEmpty()!=0){
      counter++;
      if(counter==0xeeee2)
    	radio.FlushTx();
    }
    if(radio.DataSent()){
	  radio.CleanDataSent();
	  STM_EVAL_LEDToggle(LED6);
	}
	if(radio.MaxRt()){
	  radio.CleanMaxRt();
	  radio.FlushTx();
	  STM_EVAL_LEDToggle(LED5);
	}
	radio.NRF_CE->Reset();
  }
}

extern "C"{
  void SysTick_Handler(void){
    TimingDelay_Decrement();
  }
}

void TimingDelay_Decrement(void){
  if(TimingDelay != 0x00){
    TimingDelay--;
  }
}
void Delay_ms(uint32_t time_ms){
  TimingDelay = time_ms;
  while(TimingDelay != 0);
}
void OTG_FS_IRQHandler(void){
  USBD_OTG_ISR_Handler (&USB_OTG_dev);
}

void OTG_FS_WKUP_IRQHandler(void){
  if(USB_OTG_dev.cfg.low_power)
  {
    *(uint32_t *)(0xE000ED10) &= 0xFFFFFFF9 ;
    SystemInit();
    USB_OTG_UngateClock(&USB_OTG_dev);
  }
  EXTI_ClearITPendingBit(EXTI_Line18);
}

/*
 * Callback used by stm32f4_discovery_audio_codec.c.
 * Refer to stm32f4_discovery_audio_codec.h for more info.
 */
extern "C" void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size){
  /* TODO, implement your code here */
  return;
}

/*
 * Callback used by stm324xg_eval_audio_codec.c.
 * Refer to stm324xg_eval_audio_codec.h for more info.
 */
extern "C" uint16_t EVAL_AUDIO_GetSampleCallBack(void){
  /* TODO, implement your code here */
  return -1;
}
