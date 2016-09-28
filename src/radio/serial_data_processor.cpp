#include <radio/serial_data_processor.h>

SERIAL_DATA_PROCESSOR::SERIAL_DATA_PROCESSOR(uint16_t buffersize_t){
	buffersize=buffersize_t;
	bufferreservesize=buffersize/10;
	buffer=new uint8_t[buffersize_t];
	Clear();
}


SERIAL_DATA_PROCESSOR::~SERIAL_DATA_PROCESSOR(){
	if(buffer!=0){
		delete[] buffer;
		buffer=0;
		buffersize=0;
		Clear();
	}
}

void SERIAL_DATA_PROCESSOR::FeedData(uint8_t *data, uint16_t size){
	if(size>SpaceAvailable()){
		size=SpaceAvailable();
	}
	while(size--){
		buffer[bufferpos++]=*data++;
	}
	bufferenddata=bufferpos;
}

uint16_t SERIAL_DATA_PROCESSOR::DataAvailable(){
	return bufferenddata-bufferbegindata;
}

uint16_t SERIAL_DATA_PROCESSOR::SpaceAvailable(){
	uint16_t aval=buffersize-bufferpos-bufferreservesize;
	return aval;
}

uint16_t SERIAL_DATA_PROCESSOR::GetData(uint8_t *destbuffer, uint16_t size){
	if(size>DataAvailable()){
		size=DataAvailable();
	}
	uint16_t sizeread=size;
	while(size--){
		uint8_t c=buffer[bufferbegindata++];
		if(destbuffer){
			*destbuffer++=c;
		}
	}
	if(bufferbegindata==bufferenddata){
		Clear();
	}
	return sizeread;
}

void SERIAL_DATA_PROCESSOR::Clear(){
	bufferbegindata=0;
	bufferenddata=0;
	bufferpos=0;
}

NRISP_DATA_PROCESSOR::NRISP_DATA_PROCESSOR(RADIO_EVENT &ev):SERIAL_DATA_PROCESSOR(2048){
	bufferreservesize=640;
	ev_ptr=&ev;
	Clear();
}

void NRISP_DATA_PROCESSOR::Clear(){
	SERIAL_DATA_PROCESSOR::Clear();
	msgSOFreceived=0;
	msgEOFreceived=0;
	msg_size=0;
	checksum=0;
}

void NRISP_DATA_PROCESSOR::FeedData(uint8_t *data, uint16_t size){
	if(size>SpaceAvailable()){
		size=SpaceAvailable();
	}
	while(size){
		size--;
		if(*data==0x80){
			Clear();
			msgSOFreceived=1;
		}
		if(msgSOFreceived){
			buffer[bufferpos]=*data;
			if(*data==0x81){
				msgEOFreceived=1;
				break;
			} else if(*data!=0x82 && *data!=0x80){
				buffer[buffersize-bufferreservesize+msg_size]=*data;
				if(*(data-1)==0x82){
					buffer[buffersize-bufferreservesize+msg_size]+=0x80;
				}
				uint16_t len=0xffff;
				if(msg_size>1){
					len=buffer[buffersize-bufferreservesize]*256+buffer[buffersize-bufferreservesize+1];
				}
				if(msg_size==4){
					msg_id_pos=bufferpos;
				}
				if(msg_size==(len+4)){
					checksum_pos=bufferpos;
				}
				if(msg_size<(len+4)){
					checksum+=buffer[buffersize-bufferreservesize+msg_size];
				}
				msg_size++;
			}
			bufferpos++;
		}
		data++;
	}
	if(msgEOFreceived){
		uint16_t msg_len_rcvd=buffer[buffersize-bufferreservesize]*256+buffer[buffersize-bufferreservesize+1];
//		uint16_t seq=buffer[buffersize-bufferreservesize+2]*256+buffer[buffersize-bufferreservesize+3];
		uint32_t checksumreceived=
				buffer[buffersize-bufferreservesize+4+msg_len_rcvd]*256*256+
				buffer[buffersize-bufferreservesize+4+msg_len_rcvd+1]*256+
				buffer[buffersize-bufferreservesize+4+msg_len_rcvd+2];
		if(checksum!=checksumreceived){
			Clear();
		} else {
			uint8_t *msg_ptr=&buffer[buffersize-bufferreservesize+4];
			switch(*msg_ptr){
			default:
				break;
			case 0x05:
				buffer[msg_id_pos]=0x85;
				buffer[2]=2;
				checksum+=0x81;
				buffer[checksum_pos++]=93;
				ev_ptr->Set(RADIO_EVENT::EVENT_NRISP_REPLY);
				goto calculaCRC;
				break;
			case 0x07:
				switch(buffer[msg_id_pos+1]){
				case 0:
					ev_ptr->Set(RADIO_EVENT::EVENT_MENSAGEM);
					break;
				case 1:
					ev_ptr->Set(RADIO_EVENT::EVENT_ALERTA);
					break;
				case 2:
					ev_ptr->Set(RADIO_EVENT::EVENT_BATERIA_COMPUTADOR_BAIXA);
					break;
				}
				Clear();
				break;
				case 0x04:
					buffer[msg_id_pos]=0x84;
					checksum+=0x80;
					ev_ptr->Set(RADIO_EVENT::EVENT_NRISP_REPLY);
					goto calculaCRC;
					break;
				case 0x83:
					buffer[msg_id_pos]=0x03;
					checksum-=0x80;
					goto calculaCRC;
					break;
				case 0x03:
					buffer[msg_id_pos]=0x83;
					checksum+=0x80;
					goto calculaCRC;
					break;
				calculaCRC:
					msg_ptr=&buffer[checksum_pos];
					*msg_ptr=(checksum>>16) & 0xff;
					if(*msg_ptr==0x80 || *msg_ptr==0x81 || *msg_ptr==0x82){
						msg_ptr[1]=msg_ptr[0]-0x80;
						msg_ptr[0]=0x82;
						msg_ptr++;
					}
					msg_ptr++;
					*msg_ptr=(checksum>>8) & 0xff;
					if(*msg_ptr==0x80 || *msg_ptr==0x81 || *msg_ptr==0x82){
						msg_ptr[1]=msg_ptr[0]-0x80;
						msg_ptr[0]=0x82;
						msg_ptr++;
					}
					msg_ptr++;
					*msg_ptr=checksum & 0xff;
					if(*msg_ptr==0x80 || *msg_ptr==0x81 || *msg_ptr==0x82){
						msg_ptr[1]=msg_ptr[0]-0x80;
						msg_ptr[0]=0x82;
						msg_ptr++;
					}
					msg_ptr++;
					*msg_ptr++=0x81;
					bufferbegindata=0;
					bufferenddata=msg_ptr-buffer;
					break;
			}
		}
	}
}

uint16_t NRISP_DATA_PROCESSOR::SpaceAvailable(){
	uint16_t aval=buffersize-bufferpos-bufferreservesize;
	if(bufferenddata-bufferbegindata){
		aval=0;
	}
	return aval;
}

NRISP_DATA_PROCESSOR::~NRISP_DATA_PROCESSOR(){
}
//TIMER_THING
//static volatile uint32_t transmit_timer=0;
//static uint16_t len_old=0;
//len=USBH_PL2303_GetDataCount();
//if(len && (len!=len_old)){
//	len_old=len;
//	transmit_timer=GetLocalTime();
//}
//if((len && (GetLocalTime()-transmit_timer)>200) || len>=GetMaximumPayloadSize()){
//	transmit_timer=GetLocalTime();
//	len=USBH_PL2303_GetData(buffer, GetMaximumPayloadSize());
//	event.Set(RADIO_EVENT::EVENT_VCP_DATA_TRANSMIT);
//} else {
//	len=0;
//}
