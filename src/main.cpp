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
#include "flash.h"

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
int ShowNumber(int number){

	uint8_t numero[17];
	numero[16]=0x0;
	numero[0]=0x77;//10+20+40+04+02+01
	numero[1]=0x06;//02+04
	numero[2]=0xB5;//10+20+04+01+80
	numero[3]=0x97;//01+02+04+80+10
	numero[4]=0xC6;//40+80+04+02
	numero[5]=0xD3;//01+02+10+40+80
	numero[6]=0xF3;//01+02+10+20+40+80
	numero[7]=0x07;//01+02+04
	numero[8]=0xF7;//todos menos o 0x08
	numero[9]=0xD7;//01+02+08+10+40+04
	numero[10]=0xE7;
	numero[11]=0xF2;
	numero[12]=0x71;
	numero[13]=0xB6;
	numero[14]=0xF1;
	//numero[15]=0xE1;
	//int k=0;
	uint8_t a;
	//int i=0;

	a=numero[number];

	i2c_a.WriteRegByte(0x82, 0x04, ~0x04);
	i2c_a.WriteRegByte(0x82, 0x17, a);
	i2c_a.WriteRegByte(0x82, 0x13, a);
	i2c_a.WriteRegByte(0x82, 0x10, a);

	return 0;
}
int main(void){

	int flagIDchange = 0;
	uint32_t time_display_on = GetLocalTime();
	uint32_t time_debounce = GetLocalTime();
	uint32_t x2 = GetLocalTime();
	//ShowNumber(robo.GetId());
	uint8_t bitStatus3 = (uint8_t)Bit_RESET;//adicionado pra tentar a logica do botão
	uint8_t bitStatus2 = (uint8_t)Bit_SET;//adicionado pra tentar a logica do botão
	uint8_t bitStatus= (uint8_t)Bit_SET;//adicionado pra tentar a logica do botão

	LIS3DSH_CSN.Set();
	//robo.drible->Set_Vel(0); não da certo
	SysTick_Config(SystemCoreClock/1000);
	usb.Init();
	robo.init();
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOC ,&GPIO_InitStructure);


	uint32_t last_charge_en=0;

	IO_Pin_STM32 CT(IO_Pin::IO_Pin_Mode_OUT, GPIOD, GPIO_Pin_8, GPIO_PuPd_UP, GPIO_OType_PP);//tem que confirmar
	//mudanca de plca: CA -> CT e CT -> CA


	uint32_t FLASH_SECTOR_ADDRESS;
	FLASH_SECTOR_ADDRESS = ADDR_FLASH_SECTOR_7;
	uint32_t NEXT_FLASH_SECTOR_ADDRESS;
	NEXT_FLASH_SECTOR_ADDRESS = ADDR_FLASH_SECTOR_8;

//	if(!ID_Button.Read()){
	//		DeleteFlashSector(FLASH_SECTOR_ADDRESS);
		//	WriteFlash(FLASH_SECTOR_ADDRESS, NEXT_FLASH_SECTOR_ADDRESS, (uint32_t) 1);
	//}



	//TESTE STMPE
		uint8_t i2c_ta_ai = i2c_a.ReadRegByte(0x82, 0x00);
		i2c_ta_ai++;

	//TESTE MPU
		uint8_t buf[]={0xF5,0XFF};
		spi_mpu.Assert();
		spi_mpu.WriteBuffer(buf,1);
		uint8_t resp_mpu = spi_mpu.WriteBuffer(buf+1,1);
		spi_mpu.Release();
		resp_mpu++;

	//sets the previous written id on the flash on the robot.
		int id = ReadID(FLASH_SECTOR_ADDRESS, NEXT_FLASH_SECTOR_ADDRESS);
		if (id>10) id = 0;
		robo.SetId(id);

		//ShowNumber(robo.GetId()); //shows id

		while (GetLocalTime() - x2 < 800){
			ShowNumber(robo.GetId());
		}
		ShowNumber(16);

	while(1){

		if(id==16){
			ShowNumber(14); //letter "e" for error.
		}

		if(GetLocalTime() - time_display_on>2000){
			ShowNumber(16); //turns off display
			time_display_on = GetLocalTime();
		}

		//else{
		//	ShowNumber(robo.GetId());
		//}

		//robo.drible->Set_Vel(0);
		//TIM_SetCompare4(TIM8,500);
		/* inicio de tentativa de logica para o botão*/


		if(GetLocalTime() - time_debounce>100){
			bitStatus=ID_Button.Read();
			if(bitStatus==bitStatus3){
				if(bitStatus!=bitStatus2){
					if(bitStatus==(uint8_t)Bit_RESET){
						if(robo.GetId()<10) {
							id++;
							//robo.IncId();
							//ShowNumber(robo.GetId());
						}
						else {
							id=0;
							//robo.ZeraId();
							//ShowNumber(robo.GetId());
						}
						flagIDchange = 1;
					}
					bitStatus2=bitStatus;
				}
			}
			bitStatus3=bitStatus;
			time_debounce = GetLocalTime();
		}

		if(flagIDchange){
			WriteFlash(FLASH_SECTOR_ADDRESS, NEXT_FLASH_SECTOR_ADDRESS, (uint32_t) id);
			robo.SetId(id);
			flagIDchange = 0;
			ShowNumber(robo.GetId());
		}
		//ShowNumber(robo.GetId());

		//ShowNumber(ReadID(FLASH_SECTOR_ADDRESS, NEXT_FLASH_SECTOR_ADDRESS)<10);

		if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11)){
			led_c.On();
			robo.drible->Set_Vel(0);
		}
		else
			led_c.Off();
		robo._nrf24->InterruptCallback();
		usb_device_class_cdc_vcp.GetData(_usbserialbuffer, 1024);
		CT.Set();

		if(GetLocalTime() - last_charge_en > 6000){
			last_charge_en=GetLocalTime();
			CT.Reset();
		}

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
