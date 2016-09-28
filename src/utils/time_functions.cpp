#include <utils/time_functions.h>


volatile uint32_t LocalTime = 0; /* this variable is used to create a time reference incremented by 1ms */

uint32_t timingdelay;


uint32_t GetLocalTime(){
	return LocalTime;
}

uint8_t delay_ms(uint32_t t_ms){
	uint32_t tempo_ms_now=GetLocalTime();
	while((GetLocalTime()-tempo_ms_now)<t_ms){
//		IWDG_ReloadCounter();
	}
	return 1;
}

uint8_t delay_ticks(uint32_t ticks) {
	while(ticks--);
	return 1;
}
