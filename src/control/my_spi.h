#ifndef MY_SPI
#define MY_SPI
#include "stm32f4xx.h"
#include "GPIO.h"
class MySpi{
public:
  MySpi();
  int Transfer(uint8_t *Data_IN, uint8_t *Data_OUT, int size);
  GPIO *CS_Gpio;
private:
  uint32_t Spi_TimeOut;
};
#endif
