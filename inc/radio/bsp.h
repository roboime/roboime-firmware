#pragma once

#pragma once

#include <hal/stmpe.h>
#include <hal_stm32/i2c_stm32.h>
#include <hal/ina220.h>
#include <radio/ptt.h>
#include <hal_stm32/io_pin_stm32.h>
#include <hal_stm32/usart_stm32.h>
#include <hal/io_pin_stmpe.h>
#include <hal/ina220.h>
#include <radio/gps.h>
#include <hal/led.h>
#include <hal/fm24v10.h>
#include <hal/tmp102.h>
#include <hal/mcp4xxx.h>
#include <hal/cs43l22.h>
#include <hal_stm32/i2s_stm32.h>
#include <radio/radio.h>
#include <hal/speaker_cs43l22.h>
#include <hal/microphone_adc.h>
#include <hal_stm32/adc_stm32.h>
#include <hal_stm32/spi_stm32.h>
#include <hal/si446x.h>
#include <radio/buttons.h>
#include <hal/volume_mcp4xxx.h>
#include <hal_stm32/usb_stm32.h>
#include <usb/usb_device_class_audio.h>
#include <usb/usb_device_class_cdc_vcp.h>
#include <usb/usb_device_class_cdc_rndis.h>
#include <hal/nullmodem.h>
//#include <network/ip_stack.h>
#include <radio/rf_power_ctrl.h>
#include <hal/ad7992.h>
#include <hal/ds2413.h>
#include <hal/onewire.h>
#include <hal/io_pin_ds2413.h>
#include <hal_stm32/flash_memory_stm32.h>
#include <radio/scpi.h>
#include <hal/nrf24l01p.h>

#include "control/adc.h"
#include "control/Encoder.h"
#include "control/GPIO.h"
#include "control/Motor.h"
#include "control/Pwm.h"
#include "control/Robo.h"
#include "control/Switch.h"

extern "C"{
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
}

extern I2C_STM32 i2c_a;
extern LED led_azul;
extern LED led_laranja;
extern LED led_verde;
extern LED led_vermelho;
extern INA220 ina220;
extern CS43L22 cs43l22;
extern SPI_STM32 spi;

extern USB_STM32 usb;
extern USB_DEVICE_CLASS_CDC_VCP usb_device_class_cdc_vcp;

extern Robo EuRobo;


extern NRF24L01P nrf24;
extern IO_Pin_STM32 LIS3DSH_CSN;
extern CircularBuffer<uint8_t> _usbserialbuffer;
extern TaskHandle_t t1;
extern TaskHandle_t t2;
extern QueueHandle_t fila_vel;
