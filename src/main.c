#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>

//#include "rtc.h"
//#include "usart.h"
#include "motores.h"
#include "adc.h"
#include "timer.h"
#include "leds.h"
#include "drible.h"
#include "chute.h"
#include "sensor_bola.h"
#include "rfm12.h"
#include "protocolo.h"

s32 time_ms=0;

u8 led_bat_en = 0;
void SysTick_Handler(){
	s32 tempo = time_ms % 100;
	if(led_bat_en == 1 && tempo == 0){
		Led_Pwr_Toggle();
	}
	time_ms++;
}

void PORT_Init(){
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(	RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB |
			RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD |
			RCC_AHB1Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

}

void wait_ms(u32 ms){
	u32 tempo_ini=time_ms;
	while((tempo_ini+ms)>time_ms);
}

void teste(){
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);

//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);

	while(1){
		GPIOC->ODR=GPIO_Pin_1 | GPIO_Pin_0;
	}

}


int main(void)

{
	int tempo_ultima_recepcao=-5000;
	SystemInit();
	SysTick_Config(SystemCoreClock / 1000);
	Leds_init();

	Led_Pwr_on();


	motor_inicializar();
	adc_init();
	chute_init();
	drible_init();
	sensor_bola_init();



	Timer_Init(1000);

	wait_ms(500);
	rfm12_init();
	protocolo_init();

	Led_Pwr_off();
	wait_ms(500);

	unsigned char robo_id=protocolo_get_robo_id();
	unsigned char temp;
	for(temp=0;temp<robo_id;temp++){
		Led_Pwr_on();
		wait_ms(200);
		Led_Pwr_off();
		wait_ms(200);
	}



	while(1){
		//    	wait_ms(1000);
		//    	protocolo_transmitir(0,6,0xfe,0,"LRBoot");

		protocolo_poll();

		if(protocolo_ultima_recepcao()){
			tempo_ultima_recepcao=time_ms;
		} else if((time_ms-tempo_ultima_recepcao)>500){
			motor_parar(0);
			motor_parar(1);
			motor_parar(2);
			motor_parar(3);
			drible(0);
		}

		if(adc_getConversion(4) < 2420){
			led_bat_en = 1;
		}
		else{
			led_bat_en = 0;
			if(sensor_bola(0)==0 || sensor_bola(1)==0){
				Led_Pwr_on();
			} else {
				Led_Pwr_off();
			}
		}
	}

	//return 0;
}
