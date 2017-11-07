/*
******************************************************************************
File:     main.cpp
Info:     Generated by Atollic TrueSTUDIO(R) 7.1.0   2017-10-29

The MIT License (MIT)
Copyright (c) 2009-2017 Atollic AB

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

******************************************************************************
*/


/* Includes */
#include "stm32f4xx.h"
#include "proto/grSim_Commands.pb.h"
#include "proto/pb_decode.h"
#include "proto/pb_encode.h"
#include "radio/commands.h"
#include "hal_stm32/interrupt_stm32.h"

#include "radio/bsp.h"


void vTaskLed1( void *pvParameters){
	//const char *pcTaskName = "Task 1 is running \r\n";
	//volatile uint32_t ul;

	//configASSERT( ( ( uint32_t ) pvParameters ) == 1 );
	//a tarefa tem implementada nela um loop infinito.

	for(;;){
		int delay;
		delay = pdMS_TO_TICKS(200);
		int nMotor=ulTaskNotifyTake( pdTRUE, portMAX_DELAY);
		if(nMotor==5){
			for(int i=0; i<4; i++){
				int j;
				float velRodas[]={0, 0, 0, 0};
				for(j=0; j<10; j++){
					velRodas[i] = (float)j/2.8;
					EuRobo.set_speed(velRodas);
					vTaskDelay(delay);
				}
				for(j=10; j>-10; j--){
					velRodas[i] = (float)j/2.8;
					EuRobo.set_speed(velRodas);
					vTaskDelay(delay);
				}
				for(j=-10; j<1; j++){
					velRodas[i] = (float)j/2.8;
					EuRobo.set_speed(velRodas);
					vTaskDelay(delay);
				}
			}
		}
		else{
			int j;
			float velRodas[]={0, 0, 0, 0};
			for(j=0; j<10; j++){
				velRodas[nMotor] = (float)j/2.8;
				EuRobo.set_speed(velRodas);
				vTaskDelay(delay);
			}
			for(j=10; j>-10; j--){
				velRodas[nMotor] = (float)j/2.8;
				EuRobo.set_speed(velRodas);
				vTaskDelay(delay);
			}
			for(j=-10; j<1; j++){
				velRodas[nMotor] = (float)j/2.8;
				EuRobo.set_speed(velRodas);
				vTaskDelay(delay);
			}
		}
		//GPIOD->ODR ^= GPIO_Pin_13;
		led_laranja.Toggle();
	}
}

void vTaskNRF24RX( void *pvParameters){
	NRF24L01P *_nrf24=(NRF24L01P*)pvParameters;
	_nrf24->Init();
	_nrf24->Config();
	uint8_t channel;
	uint64_t address;

	channel=92;
	address=0xE7E7E7E700;
	_nrf24->StartRX_ESB(channel, address | EuRobo.GetId(), 32, 1);

	for(;;){
		int delay = pdMS_TO_TICKS(100);
		vTaskDelay(delay);
		_nrf24->InterruptCallback();

		if(_nrf24->RxSize()){
			_nrf24->StartRX_ESB(channel, address | EuRobo.GetId(), 32, 1);
			uint8_t rxsize=_nrf24->RxSize();
			if(rxsize>32) rxsize=32;
			uint8_t buffer[32];
			_nrf24->RxData(buffer, rxsize);
			led_azul.Toggle();
			grSim_Robot_Command* robotcmd = new grSim_Robot_Command;
			pb_istream_t istream=pb_istream_from_buffer(buffer, rxsize);
			int status=pb_decode(&istream, grSim_Robot_Command_fields, robotcmd);
			if(status){
				xQueueSendToBack(fila_vel, &robotcmd, portMAX_DELAY);
			}
		}

	}
}

void vTaskRobo( void *pvParameters){
	Robo* _EuRobo=(Robo*)pvParameters;
	int delay = pdMS_TO_TICKS(100);
	INTERRUPT_STM32 timer_robot(TIM6_DAC_IRQn, 0x0C, 0x0C, ENABLE);
	grSim_Robot_Command* robotcmd;
	float motor_speeds[4];
	for(;;){
		if(uxQueueMessagesWaiting(fila_vel)){
			xQueueReceive(fila_vel, &robotcmd, portMAX_DELAY);
			float arrayVel[] = {robotcmd->wheel1, robotcmd->wheel2, robotcmd->wheel3, robotcmd->wheel4};
			_EuRobo->set_speed(robotcmd->veltangent, robotcmd->velnormal, robotcmd->velangular);
			_EuRobo->set_speed(arrayVel);
			/*if(robotcmd.kickspeedx!=0)
				robo.ChuteBaixo(robotcmd.kickspeedx);
			if(robotcmd.kickspeedz!=0)
				robo.HighKick(robotcmd.kickspeedz);*/
			delete robotcmd;
		}
		ulTaskNotifyTake( pdTRUE, delay);
		if(_EuRobo->bitcontrol){
			_EuRobo->control_speed();
		}
	}
}

void vTaskCmdLine( void *pvParameters){
	for(;;){
		int delay;
		delay = pdMS_TO_TICKS(100);
		vTaskDelay(delay);
		usb_device_class_cdc_vcp.GetData(_usbserialbuffer, 1024);
		cmdline.In(_usbserialbuffer);
		cmdline.Out(_usbserialbuffer);
		if(_usbserialbuffer.Ocupied()){
			usb_device_class_cdc_vcp.SendData(_usbserialbuffer);
		}
	}
}

int main(void)
{
	LIS3DSH_CSN.Set();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	//int i = 0;
	usb.Init();
	// robo.Init();

	SysTick_Config(SystemCoreClock/1000);

	xTaskCreate(	vTaskLed1, //ponteiro para a fun��o que implementa a tarefa
		  "Task Led1", 	//nome da fun��o. Para facilitar o debug.
		  150, 		//stack depth
		  NULL, 		//nao usa task parameter
		  2,			//prioridade 1
		  &t2);
	/*xTaskCreate(	vTaskNRF24TX, //ponteiro para a fun��o que implementa a tarefa
		  "Task NRF24TX", 	//nome da fun��o. Para facilitar o debug.
		  700, 		//stack depth
		  (void*)&nrf24, 		//usa task parameter
		  1,			//prioridade 2
		  NULL);*/
	/*xTaskCreate(	vTaskNRF24RX, //ponteiro para a fun��o que implementa a tarefa
			"Task NRF24RX", 	//nome da fun��o. Para facilitar o debug.
   	 	 700, 		//stack depth
   	 	 (void*)&nrf24, 		//nao usa task parameter
   	 	 2,			//prioridade 1
   	 	 NULL);*/
	xTaskCreate(	vTaskCmdLine, 	//ponteiro para a fun��o que implementa a tarefa
		  "Task CmdLine", 			//nome da fun��o. Para facilitar o debug.
		  700, 						//stack depth
		  NULL, 					//nao usa task parameter
		  2,						//prioridade 1
		  NULL);
	xTaskCreate(	vTaskRobo, 	//ponteiro para a fun��o que implementa a tarefa
   		  "Task Motor", 			//nome da fun��o. Para facilitar o debug.
   		  700, 						//stack depth
  		  (void*)&EuRobo, 		//usa task parameter
 		  1,						//prioridade 1
		  &t1 );
	vTaskStartScheduler();
	while (1)
	{
		//i++;
	}
}

extern "C" void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName ){
	( void ) pcTaskName;
	( void ) pxTask;
	for( ;; );
}

extern "C" void vApplicationIdleHook( void ){
	volatile size_t xFreeStackSpace;
	xFreeStackSpace = xPortGetFreeHeapSize();
}

extern "C" void vApplicationTickHook( void ){
}

extern "C" void vApplicationMallocFailedHook( void ){
	for( ;; );
}
