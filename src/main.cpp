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
#include "hal/ina220.h"
#include "hal_stm32/i2c_stm32.h"
#include "hal_stm32/io_pin_stm32.h"
#include <utils/time_functions.h>

static __IO uint32_t TimingDelay;

void TimingDelay_Decrement(void);
void Delay_ms(uint32_t time_ms);

#include <stm32f4xx_wwdg.h>

int main(void){

	SysTick_Config(SystemCoreClock/1000);

	delay_ms(3);
	/*IO_Pin_STM32 i2c_scl(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOB, GPIO_Pin_8);

	IO_Pin_STM32 i2c_sda(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOB, GPIO_Pin_9);

	I2C_STM32 my_i2c(i2c_sda, i2c_scl, I2C1, 20000, 5000002);
	INA220 mina(my_i2c, (uint8_t) 0b1000101);*/

	IO_Pin_STM32 I2C_A_SDA_PIN(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOB, GPIO_Pin_9, GPIO_PuPd_NOPULL, GPIO_OType_OD, GPIO_AF_I2C1);
	IO_Pin_STM32 I2C_A_SCL_PIN(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOB, GPIO_Pin_8, GPIO_PuPd_NOPULL, GPIO_OType_OD, GPIO_AF_I2C1);
	I2C_STM32 i2c_a(I2C_A_SDA_PIN, I2C_A_SCL_PIN, I2C1, 100000, 0x4000);

	INA220 mina220(i2c_a, 0x80);

	delay_ms(3);
	mina220.SelfTest();
	while(1){


	}

}

extern uint32_t LocalTime;

extern "C" void SysTick_Handler(void){
	//	TimingDelay_Decrement();
	LocalTime++;
}

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
