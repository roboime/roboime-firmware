#pragma once
#include <inttypes.h>

class SERIAL_DATA_PROCESSOR{
public:
	virtual ~SERIAL_DATA_PROCESSOR();
	SERIAL_DATA_PROCESSOR(uint16_t buffersize);
	virtual void FeedData(uint8_t *data, uint16_t size);
	uint16_t DataAvailable();
	virtual uint16_t SpaceAvailable();
	uint16_t GetData(uint8_t *destbuffer, uint16_t size);
	virtual void Clear();
protected:
	uint8_t *buffer;
	uint16_t buffersize;
	uint16_t bufferpos;
	uint16_t bufferreservesize;
	uint16_t bufferbegindata;
	uint16_t bufferenddata;
};


#include <radio/radio_event.h>
class NRISP_DATA_PROCESSOR: public SERIAL_DATA_PROCESSOR{
public:
	~NRISP_DATA_PROCESSOR();
	NRISP_DATA_PROCESSOR(RADIO_EVENT &ev);
	void FeedData(uint8_t *data, uint16_t size);
	void Clear();
	uint16_t SpaceAvailable();
protected:
	typedef struct{
		uint8_t init;
		uint16_t Len;
		uint16_t Seq;
		uint8_t type;
		uint8_t begin_data;
	} MSG_STRUCT;
	uint8_t msgSOFreceived;
	uint8_t msgEOFreceived;
	uint32_t checksum;
	uint16_t msg_size;
	RADIO_EVENT *ev_ptr;
	uint16_t msg_id_pos;
	uint16_t checksum_pos;

	uint8_t datastart;
};
