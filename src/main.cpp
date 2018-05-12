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
#include "mpu.h"

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

uint8_t read_register(uint8_t address);

int main(void){
	LIS3DSH_CSN.Set();
/*
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
*/
	//setando pino de CS do mpu9250
	/*GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_Init(GPIOC ,&GPIO_InitStructure);*/

	IO_Pin_STM32 MPU9250_CE_PIN(IO_Pin::IO_Pin_Mode_OUT, GPIOA, GPIO_Pin_15, GPIO_PuPd_UP, GPIO_OType_PP);
	IO_Pin_STM32 SDCARD_CE_PIN(IO_Pin::IO_Pin_Mode_OUT, GPIOC, GPIO_Pin_12, GPIO_PuPd_UP, GPIO_OType_PP);
	IO_Pin_STM32 ID_BUTTON(IO_Pin::IO_Pin_Mode_IN, GPIOE, GPIO_Pin_2, GPIO_PuPd_UP, GPIO_OType_OD);


	IO_Pin_STM32 SPI_SCK_PIN(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOA, GPIO_Pin_5, GPIO_PuPd_NOPULL, GPIO_OType_PP, GPIO_AF_SPI1);
	IO_Pin_STM32 SPI_MISO_PIN(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOA, GPIO_Pin_6, GPIO_PuPd_NOPULL, GPIO_OType_PP, GPIO_AF_SPI1);
	IO_Pin_STM32 SPI_MOSI_PIN(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOA, GPIO_Pin_7, GPIO_PuPd_NOPULL, GPIO_OType_PP, GPIO_AF_SPI1);

	led_c.On();
	led_d.On();

	uint32_t last_time;

	//MPU9250_CE_PIN.Reset();

	SPI_STM32 spi_mpu(SPI1, MPU9250_CE_PIN);
	SPI_STM32 spi_sdcard(SPI1, SDCARD_CE_PIN);

	uint8_t i2c_ta_ai = i2c_a.ReadRegByte(0x82, 0x00);
	i2c_a.WriteRegByte(0x82, 0x04, ~0x04);
	i2c_a.WriteRegByte(0x82, 0x17, 0x02);
	i2c_a.WriteRegByte(0x82, 0x13, 0x02);
	i2c_a.WriteRegByte(0x82, 0x10, 0x02);

	i2c_ta_ai++;

	/*uint8_t buf[]={0xF5,0xFF}; //who_am_i é o reg 0x75 + 0b10000000 (0x80) de read = 0xF5
	Assert();
	lixo = wb(buf[0],1);
	resp = wb(buf[1],1);
	deassert();*/

	uint8_t buf[]={0xF5,0XFF};
	spi_mpu.Assert();
	spi_mpu.WriteBuffer(buf,1);
	uint8_t resp = spi.WriteBuffer(buf+1,1);
	spi_mpu.Release();
	resp++;
/*
	while(0){
		uint8_t buf_pre_sed[]={0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
		spi_sdcard.Release();
		spi_sdcard.WriteBuffer(buf_pre_sed, 11);
		spi_sdcard.WriteBuffer(buf_pre_sed, 11);
		spi_sdcard.WriteBuffer(buf_pre_sed, 11);
		uint8_t buf_sd[]={0x40, 0x00, 0x00, 0x00, 0x00, 0x95, 0xFF, 0xFF};
		spi_sdcard.Assert();
		spi_sdcard.WriteBuffer(buf_sd,6);
		resp = spi.WriteBuffer(buf_sd+6,2);
		spi_sdcard.Release();
		resp++;
	}*/
	while(1){

		//led_d.Toggle();

		//if (GPIOA->IDR & (1<<15)) led_d.Toggle(); //chip select funcionou.
		//led_c.Toggle();
		delay_ms(250);

		if(ID_BUTTON.Read())
			led_c.On();
		else
			led_c.Off();
		//spi_sdcard.Assert();
		//if (GPIOC->IDR & (1<<12)) led_d.Toggle(); //chip select funcionou.
		//spi_sdcard.Release();
		//WHO_AM_I_MPU9250;
		//NRF24L01P.read_register(WHO_AM_I_MPU9250);
		/*
		 * if((GetLocalTime()-last_time)>1000){
			led_c.Toggle();
			led_d.Toggle();
			last_time = GetLocalTime();
		}
		*/

		/*
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
				//robo.controlbit = false;
			}
		}*/
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
