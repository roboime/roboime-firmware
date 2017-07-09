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

TaskHandle_t t1;
SemaphoreHandle_t xSPISemaphore;
QueueHandle_t xQueue1, xQueue2;

typedef struct {
	uint8_t size;
	uint8_t buffer[32];
} msgStructure;

static void prvReceiveTask( void *pvParameters )
{
	uint8_t channel=100;
	uint64_t address=0xE7E7E7E700;
	msgStructure msg;
	char ackBuffer[20];
	int ackSize=sprintf(ackBuffer, "test \n");
	for( ;; )
	{
		ulTaskNotifyTake( pdTRUE, portMAX_DELAY);
		xSemaphoreTake(xSPISemaphore, portMAX_DELAY);
		nrf24.InterruptCallback();
		if(nrf24.RxSize()){
			nrf24.StartRX_ESB(channel, address + robo.GetId(), 32, 1);
			msg.size=nrf24.RxSize();
			if(msg.size>32) msg.size=32;
			nrf24.RxData(msg.buffer, msg.size);
			xQueueSend(xQueue1, ( void * ) &msg, portMAX_DELAY);
			nrf24.write_ack_payload((uint8_t *) ackBuffer, ackSize);
		}
		xSemaphoreGive(xSPISemaphore);

		STM_EVAL_LEDOff(LED4);
	}
}
bool status=0;
grSim_Robot_Command robotcmd;

static void prvUSBTask( void *pvParameters )
{
	uint8_t buffer[32];
	for( ;; )
	{
		usb_device_class_cdc_vcp.GetData(_usbserialbuffer, 1024);
		if(robo.InTestMode()){
			cmdline.In(_usbserialbuffer);
			cmdline.Out(_usbserialbuffer);
			if(_usbserialbuffer.Ocupied()){
				usb_device_class_cdc_vcp.SendData(_usbserialbuffer);
			}
		} else {
			uint8_t size=_usbserialbuffer.Out(buffer, 32);//escreve em buffer o que recebeu
			pb_istream_t istream = pb_istream_from_buffer(buffer,size);
			status=pb_decode(&istream, grSim_Robot_Command_fields, &robotcmd);//preenche robotcmd
			if(status){//caso haja sucesso na decodificação
				uint8_t robotid=robotcmd.id;//extrai o  id do pacote
				uint8_t buffer[32];
				pb_ostream_t ostream=pb_ostream_from_buffer(buffer, sizeof(buffer));
				pb_encode(&ostream, grSim_Robot_Command_fields, &robotcmd);//escreve em ostream os dados de robotcmd
				uint8_t size=ostream.bytes_written;

//				xSemaphoreTake(xSPISemaphore, portMAX_DELAY);
//				nrf24.TxPackage_ESB(channel, address | robotid, 0, buffer, size);
//				xSemaphoreGive(xSPISemaphore);
			} else {
				_usbserialbuffer.Clear();
			}
		}
	}
}
static void prvRoboTask( void *pvParameters )
{
    bool status=0;
    pb_istream_t istream;
    msgStructure msg;
	for( ;; )
	{
		if(xQueueReceive(xQueue1, (void*)&msg, 100)==pdTRUE){
			istream=pb_istream_from_buffer(msg.buffer, msg.size);
			status=pb_decode(&istream, grSim_Robot_Command_fields, &robotcmd);
			if(status){
				if(robotcmd.id==robo.GetId()){
					robo.controlbit = true;
					robo.set_speed(robotcmd.veltangent, robotcmd.velnormal, robotcmd.velangular);
					if(robotcmd.kickspeedx!=0)
						robo.ChuteBaixo();
					if(robotcmd.kickspeedz!=0)
						robo.HighKick();
					if(robotcmd.spinner)
						robo.drible->Set_Vel(100);					}
			}
		} else {
			robo.controlbit = false;
		}
	}
}


int main(void){
	//LIS3DSH_CSN.Set();

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	usb.Init();
	robo.init();

	STM_EVAL_LEDInit(LED3);
	STM_EVAL_LEDInit(LED4);
	STM_EVAL_LEDInit(LED5);
	STM_EVAL_LEDInit(LED6);
	STM_EVAL_LEDOn(LED3);
	STM_EVAL_LEDOn(LED4);
	STM_EVAL_LEDOn(LED5);
	STM_EVAL_LEDOn(LED6);

	xSPISemaphore=xSemaphoreCreateBinary();
	xQueue1=xQueueCreate( 2, sizeof( msgStructure ) );

	xTaskCreate(prvReceiveTask, (const char * ) "Rx", configMINIMAL_STACK_SIZE, NULL, 1, &t1 );
	xTaskCreate(prvUSBTask, 	(const char * ) "USB", configMINIMAL_STACK_SIZE, NULL, 1, NULL );
	xTaskCreate(prvRoboTask, 	(const char * ) "Robo", configMINIMAL_STACK_SIZE, NULL, 1, NULL );

	vTaskStartScheduler();

	while(1);

}

extern uint32_t LocalTime;

//extern "C" void SysTick_Handler(void){
//	//	TimingDelay_Decrement();
//	LocalTime++;
//}

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

extern "C" void vApplicationStackOverflowHook(){
}

extern "C" void vApplicationTickHook(){
	LocalTime++;
}

extern "C" void vApplicationIdleHook( void ){

}

extern "C" void vApplicationMallocFailedHook( void ){

}

extern "C" void EXTI9_5_IRQHandler(){
	if(EXTI_GetITStatus(EXTI_Line5)){
		EXTI_ClearITPendingBit(EXTI_Line5);
		STM_EVAL_LEDOn(LED4);
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		xTaskNotifyFromISR(t1, 500, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
		portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
	}
}
