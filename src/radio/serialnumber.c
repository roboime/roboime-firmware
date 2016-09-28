#include <radio/serialnumber.h>

char *SerialNumberGetHexaString(){
	static char serial[25];
	uint8_t i=0;

	for(i=0;i<12;i++){
		serial[i*2]=(DeviceID_8[i]>>4)+'0';
		if(serial[i*2]>'9') serial[i*2]+=7;
		serial[i*2+1]=(DeviceID_8[i] & 0x0F)+'0';
		if(serial[i*2+1]>'9') serial[i*2+1]+=7;
	}
	serial[24]=0;
	return serial;
}
