#pragma once

#include <hal_stm32/i2c_stm32.h>
#include <hal/ina220.h>
#include <hal_stm32/io_pin_stm32.h>
#include <hal/ina220.h>
#include <hal/led.h>
#include <hal/cs43l22.h>
#include <hal_stm32/adc_stm32.h>
#include <hal_stm32/spi_stm32.h>
#include <hal_stm32/usb_stm32.h>
#include <usb/usb_device_class_cdc_vcp.h>
#include <hal_stm32/flash_memory_stm32.h>
#include <hal/nrf24l01p.h>
#include <Robo.h>

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

extern Robo robo;
extern NRF24L01P nrf24;
extern IO_Pin_STM32 LIS3DSH_CSN;
extern CircularBuffer<uint8_t> _usbserialbuffer;
extern TaskHandle_t t1;
extern QueueHandle_t fila_vel;

