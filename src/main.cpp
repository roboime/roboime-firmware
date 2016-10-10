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

//#include "GPIO.h"
//#include "Pwm.h"
//#include "Encoder.h"
#include "TimerTime.h"
//#include "TimerTime2.h"
//#include "Motor.h"
#include "Robo.h"
//#include "my_spi.h"
//#include "NRF24.h"
//#include "adc.h"

static __IO uint32_t TimingDelay;

void TimingDelay_Decrement(void);
void Delay_ms(uint32_t time_ms);

Timer_Time robo_irq_timer;

#include "radio/bsp.h"

CircularBuffer<uint8_t> _usbserialbuffer(0,2048);

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
	stream.bytes_left = circularbuffer->Ocupied();
	if(stream.bytes_left==0){
		stream.bytes_left=1; //keep it alive for pb_decode(...) function
	}
	stream.errmsg = NULL;
	return stream;
}


int main(void){
	uint8_t channel=100;
	uint64_t address=0xE7E7E7E700;
	LIS3DSH_CSN.Set();

	SysTick_Config(SystemCoreClock/1000);
	usb.Init();
	nrf24.Init();
	nrf24.Config();

	nrf24.TxPackage_ESB(channel, address + robo.GetId(), 0,(uint8_t*) "TESTE", 5);
	while(nrf24.Busy()){
		nrf24.InterruptCallback();
	}
	nrf24.StartRX_ESB(channel, address + robo.GetId(), 32, 1);

	bool status;


	grSim_Robot_Command robotcmd;
	pb_istream_t istream = pb_istream_from_circularbuffer(&_usbserialbuffer);
	while(1){
		nrf24.InterruptCallback();

		usb_device_class_cdc_vcp.GetData(_usbserialbuffer, 1024);

		if(robo.InTestMode()){
			cmdline.In(_usbserialbuffer);
			cmdline.Out(_usbserialbuffer);
			if(_usbserialbuffer.Ocupied()){
				usb_device_class_cdc_vcp.SendData(_usbserialbuffer);
			}
		} else {
			status=0;
			if(_usbserialbuffer.Ocupied()){
				status=pb_decode(&istream, grSim_Robot_Command_fields, &robotcmd);
				if(status){
					uint8_t robotid=robotcmd.id;
					uint8_t buffer[32];
					pb_ostream_t ostream=pb_ostream_from_buffer(buffer, sizeof(buffer));
					pb_encode(&ostream, grSim_Robot_Command_fields, &robotcmd);
					uint8_t size=ostream.bytes_written;
					nrf24.TxPackage_ESB(channel, address | robotid, 0, buffer, size);
					while(nrf24.Busy()){
						nrf24.InterruptCallback();
					}
				}
			} else if(nrf24.RxSize()){
				nrf24.StartRX_ESB(channel, address + robo.GetId(), 32, 1);
				uint8_t rxsize=nrf24.RxSize();
				if(rxsize>32) rxsize=32;
				uint8_t buffer[32];
				nrf24.RxData(buffer, rxsize);
				usb_device_class_cdc_vcp.SendData(buffer, rxsize);

				pb_istream_t istream=pb_istream_from_buffer(buffer, rxsize);
				status=pb_decode(&istream, grSim_Robot_Command_fields, &robotcmd);
			}
			if(status){
				if(robotcmd.id==robo.GetId()){
					robo.set_speed(robotcmd.veltangent, robotcmd.velnormal, robotcmd.velangular);
				}
			}

		}
	}


	while (1){
		//		robo.Receive();
	}
}

extern "C" {
void TIM6_DAC_IRQHandler(){
	if(TIM_GetITStatus(TIM6,TIM_IT_Update)){
		TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
		robo.control_speed();
	}
}
}

extern uint32_t LocalTime;

extern "C" void SysTick_Handler(void){
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
