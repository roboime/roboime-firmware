#include "config.h"
#include "stm32f4xx_conf.h"
#include "protocolo.h"
//#include "rfm12.h"
#include "motores.h"
#include "chute.h"
#include "drible.h"
#include "leds.h"

#define SYNC_MSB 0x2D
#define SYNC_LSB 0xD4
#define PREAMBLE 0xAA

#define PACOTE_DATA_SIZE 47

#define PACOTE_TIPO 44

#define PROTOCOLO_RECEBENDO 1
#define PROTOCOLO_LIVRE 0

#define PAYLOAD_SIZE 44

unsigned int ultima_recepcao=0;

typedef struct {
	//we transmit the bytes beginning here
	u8 len;						//length byte - number of bytes in buffer
	u8 type;					//type field for airlab
	u8 source;					//origem do pacote
	u8 dest;					//destino do pacote
	u8 checksum;				//rx checksum
	u8 data[PACOTE_DATA_SIZE+10];	//armazena os dados que serão trasmitidos + checksum
	volatile uint8_t status;	//is the buffer free or occupied?
	uint8_t num_bytes; 			//number of bytes to transmit (size in buffer)
	uint8_t bytecount;   		//counter for the byte we are transmitting at the moment
} pacote_tx_s;

typedef struct {
	//we transmit the bytes beginning here
	u8 len;						//length byte - number of bytes in buffer
	u8 type;					//type field for airlab
	u8 source;					//origem do pacote
	u8 dest;					//destino do pacote
	u8 checksum;				//rx checksum
	u8 data[PACOTE_DATA_SIZE+10];	//armazena os dados que serão trasmitidos + checksum
	volatile uint8_t status;	//is the buffer free or occupied?
	uint8_t num_bytes; 			//number of bytes to transmit (size in buffer)
	uint8_t bytecount;   		//counter for the byte we are transmitting at the moment
} pacote_rx_s;


pacote_tx_s pacote_tx;
pacote_rx_s pacote_rx;

void protocolo_init(){
	//rfm12_receiver_mode();
}

void protocolo_transmitir(u8 type, u8 tam, u8 source, u8 dest, char *buf  ){
	u16 checksum=0;
	if(tam>PACOTE_DATA_SIZE){
		tam=PACOTE_DATA_SIZE;
		//TODO Evitar o truncamento
	}
	checksum=tam^source^dest^type^0xff;
	pacote_tx.len=tam;
	pacote_tx.source=source;
	pacote_tx.dest=dest;
	pacote_tx.type=type;
	pacote_tx.checksum=checksum;

	int i;
	checksum=0;
	for(i=0;i<tam;i++){
		pacote_tx.data[i]=buf[i];
		checksum^=buf[i];
	}
	pacote_tx.data[i]=checksum;
	pacote_tx.status=1;
	pacote_tx.num_bytes=tam+6;
	pacote_tx.bytecount=0;
	//rfm12_sendbuffer((char*)(&pacote_tx),pacote_tx.num_bytes);
	TM_NRF24L01_Transmit((char*)(&pacote_tx));
	pacote_tx.status=0;
}

void protocolo_analisar(char data[], u8 len){
	u8 found=0;
	u8 i=0;
	float val=0;
	if(len==PAYLOAD_SIZE){
		if(data[0]==PACOTE_TIPO){
			for(i=1;i<PAYLOAD_SIZE;i+=7){
				if(data[i]==ID_ROBO){
					found=1;
					val=(s8)data[i+1];
					//motor_velocidade(0,val);
					val=(s8)data[i+2];
					//motor_velocidade(1,val);
					val=(s8)data[i+3];
					//motor_velocidade(2,val);
					val=(s8)data[i+4];
					//motor_velocidade(3,val);
					drible(data[i+5]);
					val=(s8)data[i+6];

					//chutar((u32)(val*10/17));//val*150/255
					if(val>0)
					{
						chutar_baixo((u32)(val*10));
					}
					else if(val<0)
					{
						val=-val;
						chutar_alto((u32)(val*10));
					}
					return;
					}
				}

			if(!found) {
				motor_velocidade(0,20);
				motor_velocidade(1,20);
				motor_velocidade(2,20);
				motor_velocidade(3,20);
				}
			}
		}
}



unsigned char protocolo_get_robo_id(){
	return get_robot_id();
}

unsigned int protocolo_ultima_recepcao(){
	unsigned char up=ultima_recepcao;
	ultima_recepcao=0;
	return up;
}


void protocolo_poll() {
	static u8 status=PROTOCOLO_LIVRE;
	static u8 pos=0;
	uint8_t datain[32];
	static u8 checksum=0;
	u8 iterar;

	//while(rfm12_data_available()){
	while(TM_NRF24L01_DataReady()){
		ultima_recepcao=1;
		//c=rfm12_receive();
		TM_NRF24L01_GetData(datain);
		if(status==PROTOCOLO_LIVRE){
			Led_Status_on();
			status=PROTOCOLO_RECEBENDO;


			//pos0: local.checksum
			checksum = datain[0];

			//pos0: len
			pacote_rx.len = datain[0];

			//pos1: type
			pacote_rx.type = datain[1];
			checksum ^= datain[1];

			//pos2: source
			pacote_rx.source = datain[2];
			checksum ^= datain[2];

			//pos3: dest
			pacote_rx.dest = datain[3];
			checksum ^= datain[3];

			//pos4: rx.checksum
			pacote_rx.checksum = datain[4];
			checksum ^= 0xff;

			if(checksum!=pacote_rx.checksum){
					status=PROTOCOLO_LIVRE;
					//rfm12_accept_data();
					Led_Status_off();
					continue;
			}

			//>>zerar local.checksum
			checksum = 0;

			//pos5: data[5-5=0]
			for(pos = 5; pos < 32; pos++){
				pacote_rx.data[pos - 5] = datain[pos];
			}

			//continue;
		} else { //esse ja eh o segundo pacote! indice de rx.data agora eh 32
			for(pos = 32; pos < 64; pos++ ){
				pacote_rx.data[pos - 5] = datain[pos - 32];
				checksum ^= datain[pos - 32];
				if(pos >= (pacote_rx.len+5) || pos >= (PACOTE_DATA_SIZE+5)){
					status=PROTOCOLO_LIVRE;
					Led_Status_off();

					if(checksum == 0){
						protocolo_analisar((char*)pacote_rx.data, pacote_rx.len);
					}
					//rfm12_accept_data();
					break; // pra sair do for
				}
			}
		}
	}
}

void protocolo_poll_old(){
	static u8 status=PROTOCOLO_LIVRE;
	static u8 pos=0;
	char c;
	static u8 checksum=0;

	//while(rfm12_data_available()){
	while(TM_NRF24L01_DataReady()){
		ultima_recepcao=1;
		//c=rfm12_receive();

		if(status==PROTOCOLO_LIVRE){
			Led_Status_on();
			status=PROTOCOLO_RECEBENDO;
			pos=1;
			pacote_rx.len=c;
			checksum=c;
			continue;
		} else {//if(status==PROTOCOLO_RECEBENDO){
			switch(pos){
			case 1:
				pacote_rx.type=c;
				checksum^=c;
				pos++;
				break;
			case 2:
				pacote_rx.source=c;
				checksum^=c;
				pos++;
				break;
			case 3:
				pacote_rx.dest=c;
				checksum^=c;
				pos++;
				break;
			case 4:
				checksum^=0xff;
				pacote_rx.checksum=c;
				if(checksum!=pacote_rx.checksum){
					status=PROTOCOLO_LIVRE;
					rfm12_accept_data();
					Led_Status_off();

					continue;
				}
				pos++;
				checksum=0;
				break;
			default:
				pacote_rx.data[pos-5]=c;
				checksum^=c;
				if(pos>=(pacote_rx.len+5) || pos>=(PACOTE_DATA_SIZE+5)){
					status=PROTOCOLO_LIVRE;
					Led_Status_off();

					if(checksum==0){
						protocolo_analisar((char*)pacote_rx.data, pacote_rx.len);
					}
					rfm12_accept_data();
					continue;
				}
				pos++;
				break;
			}
		}
	}
}

void rfm12_receive_callback(u8 c){
	static u8 status=PROTOCOLO_LIVRE;
	static u8 pos=0;
	static u8 checksum=0;

	ultima_recepcao=1;
	if(status==PROTOCOLO_LIVRE){
		Led_Status_on();
		status=PROTOCOLO_RECEBENDO;
		pos=1;
		pacote_rx.len=c;
		checksum=c;
	} else {//if(status==PROTOCOLO_RECEBENDO){
		switch(pos){
		case 1:
			pacote_rx.type=c;
			checksum^=c;
			pos++;
			break;
		case 2:
			pacote_rx.source=c;
			checksum^=c;
			pos++;
			break;
		case 3:
			pacote_rx.dest=c;
			checksum^=c;
			pos++;
			break;
		case 4:
			checksum^=0xff;
			pacote_rx.checksum=c;
			if(checksum!=pacote_rx.checksum){
				status=PROTOCOLO_LIVRE;
				//rfm12_accept_data();
				Led_Status_off();
				break;
			}
			pos++;
			checksum=0;
			break;
		default:
			pacote_rx.data[pos-5]=c;
			checksum^=c;
			if(pos>=(pacote_rx.len+5) || pos>=(PACOTE_DATA_SIZE+5)){
				status=PROTOCOLO_LIVRE;
				Led_Status_off();
				if(pacote_rx.type==0 ||  checksum==0){
					protocolo_analisar((char*)pacote_rx.data, pacote_rx.len);
				}
				//rfm12_accept_data();
				break;
			}
			pos++;
			break;
		}
	}
}

