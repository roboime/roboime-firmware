#pragma once
#include <inttypes.h>

#ifdef __cplusplus
extern "C"{
#endif

uint8_t delay_ms(uint32_t t_ms);
uint8_t delay_ticks(uint32_t ticks);

uint32_t GetLocalTime();

#ifdef __cplusplus
}
#endif
