#include <inttypes.h>

class CRC_CCIT{
protected:
	static uint16_t crc_tabccitt[];
	static uint8_t crc_tabccitt_init;
	static void init_crcccitt_tab(void);
	static uint16_t update_crc_ccitt(uint16_t crc, char c);
public:
	static uint16_t Calculate(uint8_t* buffer, uint16_t size);
};


class CRC_32{
	static uint8_t init;
public:
	static uint32_t Calculate(uint32_t* buffer, uint16_t size);
	static uint32_t Calculate(uint8_t* buffer, uint16_t size);
};

