#include <stm32f4xx_gpio.h>
#include <stm32f4xx_usart.h>

#include <radio/bsp.h>
#include <radio/serialnumber.h>
#include <hal_stm32/interrupt_stm32.h>
#include <radio/version.h>
#include <list>
//#include <control/Switch.h>
#include "TimerTime.h"
#include "proto/grSim_Commands.pb.h"


extern "C"{
	#include "usb_dcd_int.h"
	#include "usb_hcd_int.h"
}
//------------setando os leds----------------//
LED led_verde(new IO_Pin_STM32 (IO_Pin::IO_Pin_Mode_OUT, GPIOD, GPIO_Pin_12)); //led verde
LED led_laranja(new IO_Pin_STM32 (IO_Pin::IO_Pin_Mode_OUT, GPIOD, GPIO_Pin_13)); //led laranja
LED led_vermelho(new IO_Pin_STM32 (IO_Pin::IO_Pin_Mode_OUT, GPIOD, GPIO_Pin_14)); //led vermelho
LED led_azul(new IO_Pin_STM32 (IO_Pin::IO_Pin_Mode_OUT, GPIOD, GPIO_Pin_15)); //led azul
//-------------setando o botão--------------//
//IO_Pin_STM32 Botao(IO_Pin::IO_Pin_Mode_IN, GPIOA, GPIO_Pin_0, GPIO_PuPd_NOPULL);

//-------------setando o usb--------------//
IO_Pin_STM32 USB_DP(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOA, GPIO_Pin_12, GPIO_PuPd_NOPULL, GPIO_OType_PP, GPIO_AF_OTG_FS);
IO_Pin_STM32 USB_DM(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOA, GPIO_Pin_11, GPIO_PuPd_NOPULL, GPIO_OType_PP, GPIO_AF_OTG_FS);

//USB_DEVICE_CLASS_CDC_RNDIS usb_device_class_cdc_rndis(1);

std::list<USB_DEVICE_CLASS*> USB_DEVICE_CLASS::_usb_device_classes_list;
USB_DEVICE_CLASS_CDC_VCP usb_device_class_cdc_vcp({"RoboIME Serial Port"},1);
USB_STM32 usb(0x29BC, 0x2000, "IME", "RoboIME", SerialNumberGetHexaString());

//-------------setando o nrf--------------//
IO_Pin_STM32 SPI_SCK_PIN(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOA, GPIO_Pin_5, GPIO_PuPd_NOPULL, GPIO_OType_PP, GPIO_AF_SPI1);
IO_Pin_STM32 SPI_MISO_PIN(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOA, GPIO_Pin_6, GPIO_PuPd_NOPULL, GPIO_OType_PP, GPIO_AF_SPI1);
IO_Pin_STM32 SPI_MOSI_PIN(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOA, GPIO_Pin_7, GPIO_PuPd_NOPULL, GPIO_OType_PP, GPIO_AF_SPI1);
IO_Pin_STM32 NRF24_SS_PIN(IO_Pin::IO_Pin_Mode_OUT, GPIOA, GPIO_Pin_4, GPIO_PuPd_NOPULL, GPIO_OType_PP);
IO_Pin_STM32 NRF24_CE_PIN(IO_Pin::IO_Pin_Mode_OUT, GPIOA, GPIO_Pin_3, GPIO_PuPd_NOPULL, GPIO_OType_PP);
IO_Pin_STM32 NRF24_IRQN_PIN(IO_Pin::IO_Pin_Mode_IN, GPIOC, GPIO_Pin_5, GPIO_PuPd_UP, GPIO_OType_OD);

SPI_STM32 spi(SPI1, NRF24_SS_PIN);

NRF24L01P nrf24(spi, NRF24_SS_PIN, NRF24_CE_PIN, NRF24_IRQN_PIN);

IO_Pin_STM32 LIS3DSH_CSN(IO_Pin::IO_Pin_Mode_IN, GPIOE, GPIO_Pin_3, GPIO_PuPd_NOPULL, GPIO_OType_OD);

//------------PARA USAR QND TIVER COM OS MOTORES---------------
//INTERRUPT_STM32 nrf24_irqn_exti_interrupt(NRF24_IRQN_PIN.GetIRQChannel(), 0x0C, 0x0C, DISABLE);

IO_Pin_STM32 I2C_A_SDA_PIN(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOB, GPIO_Pin_9, GPIO_PuPd_NOPULL, GPIO_OType_OD, GPIO_AF_I2C1);
IO_Pin_STM32 I2C_A_SCL_PIN(IO_Pin::IO_Pin_Mode_SPECIAL, GPIOB, GPIO_Pin_8, GPIO_PuPd_NOPULL, GPIO_OType_OD, GPIO_AF_I2C1);
I2C_STM32 i2c_a(I2C_A_SDA_PIN, I2C_A_SCL_PIN, I2C1, 100000, 0x4000);

Pwm ahpwm0(GPIOC, GPIO_Pin_9, TIM8, GPIO_PinSource9, GPIO_AF_TIM8, 4, false);
GPIO algpio0(GPIOE, GPIO_Pin_5);
Pwm bhpwm0(GPIOC, GPIO_Pin_7, TIM8, GPIO_PinSource7, GPIO_AF_TIM8, 2, false);
GPIO blgpio0(GPIOC, GPIO_Pin_13);
Encoder encoder0(GPIOB, GPIOB, GPIO_Pin_4, GPIO_Pin_5, TIM3, GPIO_PinSource4, GPIO_PinSource5, GPIO_AF_TIM3);
INA220 mina220(i2c_a, 0x80);
Motor motor0(&ahpwm0, &algpio0, &bhpwm0, &blgpio0, &encoder0, &mina220);

Pwm ahpwm1(GPIOA, GPIO_Pin_8, TIM1, GPIO_PinSource8, GPIO_AF_TIM1, 1, false);
GPIO algpio1(GPIOE, GPIO_Pin_6);
Pwm bhpwm1(GPIOC, GPIO_Pin_8, TIM8, GPIO_PinSource8, GPIO_AF_TIM8, 3, false);
GPIO blgpio1(GPIOE, GPIO_Pin_4);
Encoder encoder1(GPIOA, GPIOB, GPIO_Pin_15, GPIO_Pin_3, TIM2, GPIO_PinSource15, GPIO_PinSource3, GPIO_AF_TIM2);
INA220 mina221(i2c_a, 0x82);//trocado
Motor motor1(&ahpwm1, &algpio1, &bhpwm1, &blgpio1, &encoder1, &mina221);

Pwm ahpwm2(GPIOC, GPIO_Pin_6, TIM8, GPIO_PinSource6, GPIO_AF_TIM8, 1, false);
GPIO algpio2(GPIOC, GPIO_Pin_2);
Pwm bhpwm2(GPIOE, GPIO_Pin_11, TIM1, GPIO_PinSource11, GPIO_AF_TIM1, 2, false);
GPIO blgpio2(GPIOB, GPIO_Pin_1);
Encoder encoder2(GPIOA, GPIOA, GPIO_Pin_0, GPIO_Pin_1, TIM5, GPIO_PinSource0, GPIO_PinSource1, GPIO_AF_TIM5);
INA220 mina22d(i2c_a, 0x86);//trocado
Motor motor2(&ahpwm2, &algpio2, &bhpwm2, &blgpio2, &encoder2, &mina22d);

Pwm ahpwm3(GPIOE, GPIO_Pin_14, TIM1, GPIO_PinSource14, GPIO_AF_TIM1, 4, false);
GPIO algpio3(GPIOB, GPIO_Pin_12);
Pwm bhpwm3(GPIOE, GPIO_Pin_13, TIM1, GPIO_PinSource13, GPIO_AF_TIM1, 3, false);
GPIO blgpio3(GPIOB, GPIO_Pin_11);
Encoder encoder3(GPIOB, GPIOB, GPIO_Pin_6, GPIO_Pin_7, TIM4, GPIO_PinSource6, GPIO_PinSource7, GPIO_AF_TIM4);
INA220 mina223(i2c_a, 0x8A);
Motor motor3(&ahpwm3, &algpio3, &bhpwm3, &blgpio3, &encoder3, &mina223);
//*///------------------------/////////

CircularBuffer<uint8_t> _usbserialbuffer(0,2048);
CircularBuffer<uint8_t> _usbserialbuffer2(0,2048);

//--------------------cria a queue p envio dos pacotes------------
QueueHandle_t fila_vel = xQueueCreate(12, sizeof(grSim_Robot_Command*));

QueueHandle_t fila_pck = xQueueCreate(12, sizeof(grSim_Robot_Command*));

TaskHandle_t t1; //handler para vTaskMotor
//SemaphoreHandle_t xBinarySemaphore = xSemaphoreCreateBinary();

Timer_Time robo_irq_timer;
//

INTERRUPT_STM32 usb_otg_fs_interrupt(OTG_FS_IRQn, 0x0D, 0x0D, ENABLE);

extern USB_OTG_CORE_HANDLE USB_OTG_dev;

extern "C" void TIM6_DAC_IRQHandler(){
	if(TIM_GetITStatus(TIM6,TIM_IT_Update)){
		TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		//xSemaphoreGiveFromISR( xBinarySemaphore, &xHigherPriorityTaskWoken );
		xTaskNotifyFromISR(t1, 500, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
		portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);//TODO: pesquisar diferença entre portEND e portaYield
	}
}

extern "C" void OTG_FS_IRQHandler(void){
	USBD_OTG_ISR_Handler (&USB_OTG_dev);
	USBH_OTG_ISR_Handler (&USB_OTG_dev);
}

extern "C" void OTG_FS_WKUP_IRQHandler(void){
	if(USB_OTG_dev.cfg.low_power){
		*(uint32_t *)(0xE000ED10) &= 0xFFFFFFF9 ;
		SystemInit();
		USB_OTG_UngateClock(&USB_OTG_dev);
	}
	EXTI_ClearITPendingBit(EXTI_Line18);
}
