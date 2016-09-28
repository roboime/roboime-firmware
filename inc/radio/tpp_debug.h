#pragma once
#include <stdio.h>

#ifdef DEBUG

#define DEBUG_LEVEL_ALL     0x00
#define DEBUG_LEVEL_OFF     LWIP_DBG_LEVEL_ALL /* compatibility define only */
#define DEBUG_LEVEL_WARNING 0x01 /* bad checksums, dropped packets, ... */
#define DEBUG_LEVEL_SERIOUS 0x02 /* memory allocation failures, ... */
#define DEBUG_LEVEL_SEVERE  0x03
#define DEBUG_MASK_LEVEL    0x03

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL DEBUG_LEVEL_ALL
#endif
#define DEBUG_PROCESS() DEBUG_Process_UDP()
void DEBUG_Process();
void DEBUG_Process_UDP();
#define DEBUG_Printf(...) if(DEBUG_LEVEL<=DEBUG_LEVEL_ALL) printf(__VA_ARGS__)
#define DEBUG_Warning(...) if(DEBUG_LEVEL<=DEBUG_LEVEL_WARNING) printf(__VA_ARGS__)
#define DEBUG_Error(...) if(DEBUG_LEVEL<=DEBUG_LEVEL_SERIOUS) printf(__VA_ARGS__)



#else

#define DEBUG_PROCESS()
#define DEBUG_Printf(...)
#define DEBUG_Warning(...)
#define DEBUG_Error(...)



#endif
