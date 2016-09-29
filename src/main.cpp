/**
 *****************************************************************************
 **
 **  File        : main.c
 **
 **  Abstract    : main function.
 **
 **  Functions   : main
 **
 **  Environment : Atollic TrueSTUDIO(R)
 **                STMicroelectronics STM32F4xx Standard Peripherals Library
 **
 **  Distribution: The file is distributed "as is", without any warranty
 **                of any kind.
 **
 **  (c)Copyright Atollic AB.
 **  You may use this file as-is or modify it according to the needs of your
 **  project. This file may only be built (assembled or compiled and linked)
 **  using the Atollic TrueSTUDIO(R) product. The use of this file together
 **  with other tools than Atollic TrueSTUDIO(R) is not permitted.
 **
 *****************************************************************************
 */

/* Includes */
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "GPIO.h"
#include "Pwm.h"
#include "Encoder.h"
#include "TimerTime.h"
#include "TimerTime2.h"
#include "Motor.h"
#include "Robo.h"
#include "my_spi.h"
#include "NRF24.h"
#include "adc.h"

static __IO uint32_t TimingDelay;

void TimingDelay_Decrement(void);
void Delay_ms(uint32_t time_ms);

//Robo robo;
//Timer_Time robo_irq_timer;

#include "radio/bsp.h"

CircularBuffer<uint8_t> _usbserialbuffer(0,2048);

int main(void)
{
	SysTick_Config(SystemCoreClock/1000);
	usb.Init();
	NRF24 radio;
//	radio.is_rx=true;
//	radio.Config();
//	radio.SetId(0);
//	radio.NRF_CE->Set();


	while(1){
		usb_device_class_cdc_vcp.GetData(_usbserialbuffer, 256);
		usb_device_class_cdc_vcp.SendData(_usbserialbuffer);
	}


	while (1){
//		robo.Receive();
	}
}

//extern "C" {
//void TIM6_DAC_IRQHandler(){
//	if(TIM_GetITStatus(TIM6,TIM_IT_Update)){
//		TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
//		robo.control_speed();
//	}
//}
//}
//
extern uint32_t LocalTime;
void SysTick_Handler(void){
//	TimingDelay_Decrement();
	LocalTime++;
}

//void TimingDelay_Decrement(void){
//	if(TimingDelay != 0x00){
//		TimingDelay--;
//	}
//}
//void Delay_ms(uint32_t time_ms)
//{
//	TimingDelay = time_ms;
//	while(TimingDelay != 0);
//}

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
