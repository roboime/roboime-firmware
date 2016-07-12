/*
 * NOVO:
 * Usará o pino IRQ do nRF24 e interruptions para lidar com a recepção de mensagens
 * RECENTE:
 * Em vez de se basear em 3 funções diferentes, usar apenas um comando de SPI genérico
 * Mantém-se a configuração de SPI divida em duas partes:
 * 		uma parte fica no construtor da classe SPI e faz a inicialização do SCK,MOSI e MISO
 * 		a outra parte fica no construtor da classe NRF, e inicializa o CSN
 * o pino VDD do NRF fica ligado direto no VDD da discovery
 * a função NRF::begin() é chamada dentro do construtor do nRF
 *
 * Author: Renan Pícoli
 */

/* Includes */
#include "main.h"

#include "own_libraries/NRF24.h"
#include "own_libraries/CONFIG.h"
#include "own_libraries/SPI_interface.h"

uint32_t TimingDelay;

//void Delay (uint32_t nTime);

void TimingDelay_Decrement(void);

extern "C" {
 void SysTick_Handler(void);

 //handlers que podem ser chamados
 void EXTI0_IRQHandler();
 void EXTI1_IRQHandler();
 void EXTI2_IRQHandler();
 void EXTI3_IRQHandler();
 void EXTI4_IRQHandler();
 void EXTI9_5_IRQHandler();
 void EXTI15_10_IRQHandler();
}

uint8_t USB_receive_and_put(NRF* radio_ptr);

NRF* radio_ptr;

Robo robo;

int main(void)
{
  SysTick_Config(SystemCoreClock/1000);

  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
  STM_EVAL_LEDInit(LED5);
  STM_EVAL_LEDInit(LED6);

  NRF radio;//inicializa o NRF com os pinos default, deixa em POWER_UP
  radio_ptr=&radio;
  radio.RX_configure();
  radio.start_listen();

  while (1)
  {
		if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5)){
		  STM_EVAL_LEDOn(LED3);
		  STM_EVAL_LEDOff(LED4);
		  if(EXTI_GetITStatus(EXTI_Line5)!=RESET)
			  STM_EVAL_LEDOn(LED5);
		  else
			  STM_EVAL_LEDOff(LED5);
	  }
	  else{
		  STM_EVAL_LEDOff(LED3);
		  STM_EVAL_LEDOn(LED4);
	  }
  }
}
extern "C" {
void TIM6_DAC_IRQHandler(){
  if(TIM_GetITStatus(TIM6,TIM_IT_Update)){
    TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
  }
}
}

uint8_t USB_receive_and_put(NRF* radio_ptr){
	static uint8_t data[]={0,0,0,0,0};
	//passa por aqui
	if(radio_ptr->RECEIVE(data)){
		//TODO: testar se ainda há pacotes para ler, COMO O MANUAL MANDA
/*		if(radio_ptr->DATA_READY()){
			STM_EVAL_LEDOn(LED5);
		}*/
	    int dutys[4];
		for(int i2=0; i2<4; i2++){
			if(data[i2+1]<100){
			    dutys[i2]=10*data[i2+1];
			}
			else{
				dutys[i2]=-10*(data[i2+1]-100);
			}
		}
		robo.set_pwms(dutys);
		return 1;
	}
	else{
		return 0;
	}
}

void EXTI0_IRQHandler(){
	USB_receive_and_put(radio_ptr);
	EXTI_ClearITPendingBit(EXTI_Line0);
	STM_EVAL_LEDToggle(LED6);//indicador de sucesso
	return;
}

void EXTI1_IRQHandler(){
	USB_receive_and_put(radio_ptr);
	EXTI_ClearITPendingBit(EXTI_Line1);
	STM_EVAL_LEDToggle(LED6);//indicador de sucesso
	return;
}

void EXTI2_IRQHandler(){
	USB_receive_and_put(radio_ptr);
	EXTI_ClearITPendingBit(EXTI_Line2);
	STM_EVAL_LEDToggle(LED6);//indicador de sucesso
	return;
}

void EXTI3_IRQHandler(){
	USB_receive_and_put(radio_ptr);
	EXTI_ClearITPendingBit(EXTI_Line3);
	STM_EVAL_LEDToggle(LED6);//indicador de sucesso
	return;
}

void EXTI4_IRQHandler(){
	USB_receive_and_put(radio_ptr);
	EXTI_ClearITPendingBit(EXTI_Line4);
	STM_EVAL_LEDToggle(LED6);//indicador de sucesso
	return;
}

void EXTI9_5_IRQHandler(){
	//passa por aqui
	USB_receive_and_put(radio_ptr);
	//NÃO passa por aqui
	//contém 1 na posição correspondente às linhas que têm IT para tratar
	EXTI_ClearITPendingBit(EXTI_Line(radio_ptr->IRQ_Pin()));
	STM_EVAL_LEDToggle(LED6);//indicador de sucesso
	return;
}

void EXTI15_10_IRQHandler(){
	USB_receive_and_put(radio_ptr);
	//contém 1 na posição correspondente às linhas que têm IT para tratar
	EXTI_ClearITPendingBit(EXTI_Line(radio_ptr->IRQ_Pin()));
	STM_EVAL_LEDToggle(LED6);//indicador de sucesso
	return;
}

/*void Delay( uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0); //gustavo
}*/

/*
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  {
    TimingDelay--;
  }
}
extern "C" {
void SysTick_Handler(void)
{
  TimingDelay_Decrement();
}
}
*/

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
