#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>


#include "motores.h"
#include "adc.h"
#include "timer.h"
#include "leds.h"
#include "drible.h"
#include "chute.h"
#include "sensor_bola.h"
#include "rfm12.h"
#include "TM/defines.h"
#include "TM/tm_stm32f4_nrf24l01.h"
#include "TM/tm_stm32f4_spi.h"
#include "protocolo.h"
#include "dip_switch.h"

#define PI	3.14159265358979f



/* Receiver address */
uint8_t nrf_address_transmissor[] = {	0xE7,	0xE7,	0xE7,	0xE7,	0xE7};

/* My address */
uint8_t nrf_address_robo[] = {	0x7E,	0x7E,	0x7E,	0x7E,	0x7E};

int nrf_canal = 109;
int nrf_payload = 32;

s32 time_ms=0;
u8 led_bat_en = 0;
int bateria=0;


void NRF24L01_init() {
	TM_NRF24L01_Init(nrf_canal, nrf_payload);
	TM_NRF24L01_SetRF(TM_NRF24L01_DataRate_2M, TM_NRF24L01_OutputPower_M18dBm);

	TM_NRF24L01_SetMyAddress(nrf_address_robo);
	TM_NRF24L01_SetTxAddress(nrf_address_transmissor);
}

void SysTick_Handler(){
	s32 tempo = time_ms % 200;
	if(led_bat_en == 1 && tempo == 0){
		//Led_Pwr_Toggle();
	}
	time_ms++;
}


void wait_ms(u32 ms){
	u32 tempo_ini=time_ms;
	while((tempo_ini+ms)>time_ms);
}




int main(void)
{
	SystemInit();
	int tempo_ultima_recepcao=-5000;

	SysTick_Config(SystemCoreClock/1000);
	Leds_init();

	Led_Pwr_on();


    motor_inicializar();
    adc_init();
	chute_init();
	drible_init();
	sensor_bola_init();
	dip_switch_init();
	Timer_Init(10000);
	wait_ms(500);

	NRF24L01_init();
	//rfm12_init();
	protocolo_init();
	botao_init();
	EXTILine0_Config();
	Led_Pwr_off();
	wait_ms(500);

	unsigned char robo_id=protocolo_get_robo_id();
	unsigned char temp;
	for(temp=0;temp<robo_id;temp++){
		Led_Pwr_on();
		Led_Status_on();
		wait_ms(200);
		Led_Pwr_off();
		Led_Status_off();
		wait_ms(200);
	}

//	while(1) {
//		motor_velocidade(0, 40);
//		motor_velocidade(1, 40);
//		motor_velocidade(2, 40);
//		motor_velocidade(3, 40);
//		wait_ms(2500);
//
//		motor_velocidade(0, 0);
//		motor_velocidade(1, 0);
//		motor_velocidade(2, 0);
//		motor_velocidade(3, 0);
//		wait_ms(1000);
//
//		motor_velocidade(0, -40);
//		motor_velocidade(1, -40);
//		motor_velocidade(2, -40);
//		motor_velocidade(3, -40);
//		wait_ms(2500);
//
//		motor_velocidade(0, 0);
//		motor_velocidade(1, 0);
//		motor_velocidade(2, 0);
//		motor_velocidade(3, 0);
//		wait_ms(1000);
//	}

 /*    drible(1);
	motor_velocidade(0,80);
	motor_velocidade(1,80);
	motor_velocidade(2,80);
	motor_velocidade(3,80);
	while(1){
		//chutar_alto(255);
		//wait_ms(3000);
		//chutar_baixo(255);
		//wait_ms(255);
	}*/
	int led_bat_toggle=0;

	while(1){
		//    	wait_ms(1000);


		protocolo_poll_new_NRF24L01();
		protocolo_poll_new_NRF24L01();

		if(protocolo_ultima_recepcao())
		{
			tempo_ultima_recepcao=time_ms;
		}
//		else if((time_ms-tempo_ultima_recepcao)>50)
//		{
//			motor_parar(0);
//			motor_parar(1);
//			motor_parar(2);
//			motor_parar(3);
//			drible(0);
//		}




		if(sensor_bola(0)==0 || sensor_bola(1)==0){
					Led_Pwr_on();
					}
					else
					{
						Led_Pwr_off();
					}


		int a1,a2,a3,a4,a5,a6,a7;
		a1=adc_getConversion(0);
		a2=adc_getConversion(1);
		a3=adc_getConversion(2);
		a4=adc_getConversion(3);
		a5=adc_getConversion(4);
		a6=adc_getConversion(5);
		a7=adc_getConversion(6);



		bateria=adc_getConversion(2);
		if(bateria>3000)
		{
			Led_Acender(0);
			Led_Acender(1);
			Led_Acender(2);
			Led_Acender(3);

		}

		if(bateria<3000 && bateria>2900)
		{
			Led_Apagar(0);
			Led_Acender(1);
			Led_Acender(2);
			Led_Acender(3);

		}

		if(bateria<2900 && bateria>2800)
		{
			Led_Apagar(0);
			Led_Apagar(1);
			Led_Acender(2);
			Led_Acender(3);

		}

		if(bateria<2800 && bateria>2700)
		{
			Led_Apagar(0);
			Led_Apagar(1);
			Led_Apagar(2);
			Led_Acender(3);

		}

		if(bateria<2700)
		{
			if (led_bat_toggle==0 && !(time_ms%200)){
			Led_Apagar(0);
			Led_Apagar(1);
			Led_Apagar(2);
			Led_Apagar(3);
			led_bat_toggle=1;}
			else if(led_bat_toggle==1 && !(time_ms%200))
			{
				Led_Acender(0);
				Led_Acender(1);
				Led_Acender(2);
				Led_Acender(3);
				led_bat_toggle=0;
			}
		}



		if(adc_getConversion(4) < 2420)
		{
			led_bat_en = 1;
		}
		else
		{
			led_bat_en = 0;
			if(sensor_bola(0)==1 && sensor_bola(1)==1){
			Led_Pwr_on();
			}
			else
			{
				Led_Pwr_off();
			}

		}

	}
	return 0;
}
