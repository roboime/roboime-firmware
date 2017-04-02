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
#include <control/motor.h>


extern I2C_STM32 i2c_a;
extern I2C_STM32 i2c_b;
extern STMPE stmpe_a;
extern STMPE stmpe_b;
extern PTT ptt;
extern LED led_a;
extern LED led_b;
extern LED led_c;
extern LED led_d;
extern GPS gps;
extern INA220 ina220;
extern FM24V10 radio_memory;
extern TMP102 tmp102;
extern MCP4XXX mcp4561_a;
extern MCP4XXX mcp4561_b;
extern CS43L22 cs43l22;
extern USART_STM32 usart_hdq;
extern SOUND sound;
extern RADIO radio;
extern SPI_STM32 spi;
extern IO_Pin_STM32 PA_VDD_EN;
extern IO_Pin_STMPE V5V_EN_PIN;
extern IO_Pin_STMPE USB_5V_EN_PIN;
extern USB_STM32 usb;
extern USB_DEVICE_CLASS_CDC_VCP usb_device_class_cdc_vcp;
extern USB_DEVICE_CLASS_AUDIO usb_device_class_audio;
extern USB_DEVICE_CLASS_CDC_RNDIS usb_device_class_cdc_rndis;
//extern IP_STACK ip_stack;
extern AD7992 ad7992;
extern DS2413 ds2413;
extern SI446X modem;
extern IO_Pin_STMPE PTT_EMI_PIN;
extern IO_Pin_STM32 ONEWIRE_PULL_UP;
extern RF_POWER_CTRL rf_power_ctrl;
//extern NULLMODEM modem;
extern SCPI SCPI_USB;

extern NRF24L01P nrf24;
extern IO_Pin_STM32 Botao;
extern IO_Pin_STM32 LIS3DSH_CSN;
extern Motor motor0;
extern Motor motor1;
extern Motor motor2;
extern Motor motor3;
extern INA220 mina220;
extern INA220 mina221;
extern INA220 mina223;
extern INA220 mina22d;
extern CircularBuffer<uint8_t> _usbserialbuffer;

