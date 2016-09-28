#include <utils/crc.h>
#include <stm32f4xx_crc.h>
uint8_t CRC_CCIT::crc_tabccitt_init=0;
uint16_t CRC_CCIT::crc_tabccitt[256];

uint16_t CRC_CCIT::Calculate(uint8_t* buffer, uint16_t size) {
	uint16_t crc = 0xFFFF;
	while (size--) {
		crc = update_crc_ccitt(crc, *buffer++);
	}
	return crc;
}

uint16_t CRC_CCIT::update_crc_ccitt(uint16_t crc, char c) {
	uint16_t tmp, short_c;
	short_c = 0x00ff & (uint16_t) (c);
	if (!crc_tabccitt_init)
		init_crcccitt_tab();

	tmp = (crc >> 8) ^ short_c;
	crc = (crc << 8) ^ crc_tabccitt[tmp];
	return crc;
}

void CRC_CCIT::init_crcccitt_tab(void) {
	int i, j;
	uint16_t crc, c;
	for (i = 0; i < 256; i++) {
		crc = 0;
		c = ((uint16_t) (i)) << 8;
		for (j = 0; j < 8; j++) {
			if ((crc ^ c) & 0x8000)
				crc = (crc << 1) ^ 0x1021;
			else
				crc = crc << 1;

			c = c << 1;
		}
		crc_tabccitt[i] = crc;
	}
	crc_tabccitt_init = 1;
}



uint8_t CRC_32::init=0;
uint32_t CRC_32::Calculate(uint32_t* buffer, uint16_t size) {
	if(!init){
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
		init=1;
	}
	CRC_ResetDR();
	return CRC_CalcBlockCRC(buffer, size)^0xFFFFFFFF;
}

uint32_t CRC_32::Calculate(uint8_t* buffer, uint16_t size) {
	if(!init){
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);
		init=1;
	}
	CRC_ResetDR();
	for(uint16_t i=0;i<size;i++){
		CRC_CalcCRC(*buffer++);
	}
	return CRC_GetCRC()^0xFFFFFFFF;

	uint16_t size32=size/4;
	CRC_ResetDR();
	CRC_CalcBlockCRC((uint32_t*)buffer, size32);
	uint32_t val=*((uint32_t*)(&buffer[size32*4]));
	size32=size%4;
	if(size32){
		val&=(0xFFFFFFFF>>((4-size32)*8));
		CRC_CalcCRC(val);
	}
	return CRC_GetCRC()^0xFFFFFFFF;
}
