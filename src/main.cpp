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
#include "proto/grSim_Commands.pb.h"
#include "proto/pb_decode.h"
#include "proto/pb_encode.h"
#include "radio/commands.h"
#include "Robo.h"

extern "C"{
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
}

static __IO uint32_t TimingDelay;

void TimingDelay_Decrement(void);
void Delay_ms(uint32_t time_ms);

#include "radio/bsp.h"

uint8_t scanned[256];



#include <stm32f4xx_wwdg.h>


bool pb_circularbuffer_read(pb_istream_t *stream, pb_byte_t *buf, size_t count){
	bool result=false;
	CircularBuffer<uint8_t> *circularbuffer=(CircularBuffer<uint8_t> *)(stream->state);
	if(circularbuffer->Out(buf, count)==count){
		result=true;
	}
	stream->bytes_left=circularbuffer->Ocupied();
	if(stream->bytes_left==0){
		stream->bytes_left=1; //keep it alive for pb_decode(...) function
	}
	return result;
}
pb_istream_t pb_istream_from_circularbuffer(CircularBuffer<uint8_t> *circularbuffer)
{
	pb_istream_t stream;
	/* Cast away the const from buf without a compiler error.  We are
	 * careful to use it only in a const manner in the callbacks.
	 */
	union {
		void *state;
		const void *c_state;
	} state;
	stream.callback = &pb_circularbuffer_read;
	state.c_state = circularbuffer;
	stream.state = state.state;
	stream.bytes_left = 1;
	if(stream.bytes_left==0){
		stream.bytes_left=1; //keep it alive for pb_decode(...) function
	}
	stream.errmsg = NULL;
	return stream;
}

int main(void){
	LIS3DSH_CSN.Set();

	SysTick_Config(SystemCoreClock/1000);
	usb.Init();
	robo.init();
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOA ,&GPIO_InitStructure);

	mina220.SelfTest();
	mina22d.SelfTest();
	mina221.SelfTest();
	mina223.SelfTest();
	mina220.ReadCurrent();
	robo.motors[0]->mina22->ReadCurrent();

	while(1){
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2))
			led_c.On();
		else
			led_c.Off();
		robo._nrf24->InterruptCallback();
		usb_device_class_cdc_vcp.GetData(_usbserialbuffer, 1024);

		if(robo.InTestMode()){
			robo.interruptTestMode();
			delay_ms(2);
		}
		else {
			if(_usbserialbuffer.Ocupied()){
				robo.interruptTransmitter();
			}
			if(nrf24.RxSize()){
				robo.interruptReceive();
				robo.interruptAckPayload();
				robo.last_packet_ms = GetLocalTime();
				robo.controlbit = true;
			}
			if((GetLocalTime()-robo.last_packet_ms)>100){
				robo.controlbit = false;
			}
		}
	}
}

extern uint32_t LocalTime;
/*
extern "C" void SysTick_Handler(void){
	//	TimingDelay_Decrement();
	LocalTime++;
}*/

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

extern "C" void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName ){

	while(1);
}
extern "C" void vApplicationTickHook( void ){
	//Aqui vai o código que iria no SysTickHandler

}
