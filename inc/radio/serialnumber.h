#pragma once

#include <inttypes.h>
#define DeviceID_8 ((uint8_t*)0x1FFF7A10)
#define DeviceID_16 ((uint16_t*)0x1FFF7A10)
#define DeviceID_32 ((uint32_t*)0x1FFF7A10)

#ifdef __cplusplus
extern "C" {
#endif

char *SerialNumberGetHexaString();

#ifdef __cplusplus
}
#endif
