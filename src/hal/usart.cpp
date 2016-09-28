#include <hal/usart.h>

uint8_t USART::ReadBuffer(uint8_t *buffer, uint16_t size){
	uint8_t c=0;
	uint8_t sizer=0;
	do{
		c=ReadByte();
		*buffer++=c;
	} while(c && --size && sizer++);
	return sizer;
}

void USART::WriteBuffer(uint8_t *buffer, uint16_t size){
	while(size--){
		WriteByte(*buffer++);
	}
}

void USART::WriteString(const char* buffer) {
	while(*buffer){
		WriteByte(*buffer++);
	}
}
