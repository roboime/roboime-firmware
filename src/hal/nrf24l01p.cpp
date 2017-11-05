#include <radio/bsp.h>
#include <hal/nrf24l01p.h>
#include <utils/time_functions.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "StackMacros.h"
#include "projdefs.h"

NRF24L01P::NRF24L01P(SPI &spi, IO_Pin &SS_PIN, IO_Pin &CE_PIN, IO_Pin &NIRQ_PIN):
	MODEM(),
	_spi(&spi),
	_SS_PIN(&SS_PIN),
	_NIRQ_PIN(&NIRQ_PIN),
	_CE_PIN(&CE_PIN),
	_rxbuffer(0,2048),
	_txbuffer(0,2048),
	_channel(0),
	_retransmit_irq_count(0),
	_receive_irq_count(0),
	_transmit_irq_count(0),
	_busy(1),
	REG(REG_DEFAULT)
{
}


uint8_t NRF24L01P::SelfTest(){
	return 1;
}


uint8_t NRF24L01P::TxReady() {
	REG.STATUS.value=nop();
	return !REG.STATUS.TX_FULL;
}

void NRF24L01P::TxPackage(uint8_t* data, uint16_t size, uint32_t frequency, MODEM_POWER power) {
	if(REG.STATUS.MAX_RT || REG.STATUS.TX_FULL){
		return;
	}
	if(size>32){
		size=32;
	}
	REG.STATUS.value=nop();
	SetRXFrequency(frequency);

	write_tx_payload(data, size);
	_CE_PIN->Set();
}


uint16_t NRF24L01P::RxSize() {
	return _rxbuffer.Ocupied();
}

uint16_t NRF24L01P::RxData(uint8_t* data, uint16_t maxsize) {
	return _rxbuffer.Out(data, maxsize);
}

void NRF24L01P::SetRXFrequency(uint32_t freqHz) {
	REG.RF_CH.RF_CH=freqHz;
	write_register(REG_ADDR.RF_CH, REG.RF_CH.value);
}

uint8_t NRF24L01P::GetRSSI() {
	return read_register(REG_ADDR.RPD);
}

uint8_t NRF24L01P::read(uint8_t command, uint8_t *buffer, uint8_t size) {
	uint8_t status;
	_spi->Assert();
	status=_spi->WriteByte(command);
	while(size--){
		*buffer++=_spi->WriteByte(0x00);
	}
	_spi->Release();
	return status;
}


uint8_t NRF24L01P::write(uint8_t command, uint8_t *buffer, uint8_t size) {
	uint8_t status;
	_spi->Assert();
	status=_spi->WriteByte(command);
	if(size){
		_spi->WriteBuffer(buffer, size);
	}
	_spi->Release();
	return status;
}

uint8_t NRF24L01P::write(uint8_t cmdval) {
	return write(cmdval, 0, 0);
}

uint8_t NRF24L01P::write_register(uint8_t address, uint8_t* buffer, uint8_t size) {
	return write(CMD::W_REGISTER | address, buffer, size);
}

uint8_t NRF24L01P::write_register(uint8_t address, uint8_t value) {
	return write(CMD::W_REGISTER | address, &value, 1);
}

uint8_t NRF24L01P::read_register(uint8_t address, uint8_t* buffer, uint8_t size) {
	return read(CMD::R_REGISTER | address, buffer, size);
}

uint8_t NRF24L01P::read_register(uint8_t address) {
	uint8_t value;
	read(CMD::R_REGISTER | address, &value, 1);
	return value;
}

uint8_t NRF24L01P::read_register(uint8_t address, uint8_t *buffer) {
	return read(CMD::R_REGISTER | address, buffer, 1);
}

uint8_t NRF24L01P::read_rx_payload(uint8_t* buffer, uint8_t size) {
	return read(CMD::R_RX_PAYLOAD, buffer, size);
}

uint8_t NRF24L01P::write_tx_payload(uint8_t* buffer, uint8_t size) {
	return write(CMD::W_TX_PAYLOAD, buffer, size);
}

uint8_t NRF24L01P::write_tx_payload_no_ack(uint8_t* buffer, uint8_t size) {
	return write(CMD::W_TX_PAYLOAD_NOACK, buffer, size);
}

uint8_t NRF24L01P::write_ack_payload(uint8_t* buffer, uint8_t size) {
	return write(CMD::W_ACK_PAYLOAD, buffer, size);
}

uint8_t NRF24L01P::flush_tx() {
	return write(NRF24L01P::CMD::FLUSH_TX);
}

uint8_t NRF24L01P::flush_rx() {
	return write(NRF24L01P::CMD::FLUSH_RX);
}

uint8_t NRF24L01P::reuse_tx_pl() {
	return write(NRF24L01P::CMD::REUSE_TX_PL);
}

uint8_t NRF24L01P::read_rx_payload_width() {
	uint8_t size=0;
	read(NRF24L01P::CMD::R_RX_PL_WID,&size,1);
	return size;
}

uint8_t NRF24L01P::nop() {
	return write(NRF24L01P::CMD::NOP);
}

void NRF24L01P::Init(){
	_CE_PIN->Reset();
	_SS_PIN->Set();

	vTaskDelay(pdMS_TO_TICKS(5));
	REG=REG_DEFAULT;

	uint8_t config=read_register(REG_ADDR.CONFIG);
	if(config==REG.CONFIG.value){
		//OK
	}
	config=nop();

	//As the chip lacks of reset pin, write default values to all registers
	write_register(REG_ADDR.CONFIG, REG.CONFIG.value);
	write_register(REG_ADDR.EN_AA, REG.EN_AA.value);
	write_register(REG_ADDR.SETUP_AW, REG.SETUP_AW.value);
	write_register(REG_ADDR.SETUP_RETR, REG.SETUP_RETR.value);
	write_register(REG_ADDR.RF_CH, REG.RF_CH.value);
	write_register(REG_ADDR.RF_SETUP, REG.RF_SETUP.value);
	write_register(REG_ADDR.STATUS, REG.STATUS.value);

	write_register(REG_ADDR.RX_ADDR_P0, REG.RX_ADDR_P0.value, 5);
	write_register(REG_ADDR.RX_ADDR_P1, REG.RX_ADDR_P1.value, 5);
	write_register(REG_ADDR.RX_ADDR_P2, REG.RX_ADDR_P2.value);
	write_register(REG_ADDR.RX_ADDR_P3, REG.RX_ADDR_P3.value);
	write_register(REG_ADDR.RX_ADDR_P4, REG.RX_ADDR_P4.value);
	write_register(REG_ADDR.RX_ADDR_P5, REG.RX_ADDR_P5.value);

	write_register(REG_ADDR.TX_ADDR, REG.TX_ADDR.value, 5);
	write_register(REG_ADDR.RX_PW_P0, REG.RX_PW_P0.value);
	write_register(REG_ADDR.RX_PW_P1, REG.RX_PW_P1.value);
	write_register(REG_ADDR.RX_PW_P2, REG.RX_PW_P2.value);
	write_register(REG_ADDR.RX_PW_P3, REG.RX_PW_P3.value);
	write_register(REG_ADDR.RX_PW_P4, REG.RX_PW_P4.value);
	write_register(REG_ADDR.RX_PW_P5, REG.RX_PW_P5.value);

	write_register(REG_ADDR.DYNPD, REG.DYNPD.value);
	write_register(REG_ADDR.FEATURE, REG.FEATURE.value);

	REG.SETUP_RETR.ARC=0;
	REG.SETUP_RETR.ARD=0x11;
	write_register(REG_ADDR.SETUP_RETR, REG.SETUP_RETR.value);

	flush_rx();
	flush_tx();
}

void NRF24L01P::Config(){
	REG.FEATURE.EN_DPL=1;
	REG.FEATURE.EN_ACK_PAY=1;
	REG.FEATURE.EN_DYN_ACK=1;
	write_register(REG_ADDR.FEATURE, REG.FEATURE.value);

	REG.DYNPD.DPL_P0=1;
	REG.DYNPD.DPL_P1=1;
	write_register(REG_ADDR.DYNPD, REG.DYNPD.value);

	REG.CONFIG.PWR_UP=1;
	write_register(REG_ADDR.CONFIG, REG.CONFIG.value);
	_busy=0;
}

uint8_t NRF24L01P::Scan(uint8_t *buffer){
	uint8_t i, j;
	REG.CONFIG=REG_DEFAULT.CONFIG;
	REG.CONFIG.PRIM_RX=1;
	REG.CONFIG.PWR_UP=1;
	write_register(REG_ADDR.CONFIG, REG.CONFIG.value);

	for(i=0;i<126;i++){
		buffer[i]=0;
	}
	_CE_PIN->Set();
	for(j=0;j<100;j++){
		for(i=0;i<126;i++){
			REG.RF_CH.RF_CH=i;
			write_register(REG_ADDR.RF_CH, REG.RF_CH.value);
			nop();
			buffer[i]+=read_register(REG_ADDR.RPD);
		}
	}
	_CE_PIN->Reset();
	return 1;
}

void NRF24L01P::CW(uint8_t state){
	if(state){
		REG.CONFIG=REG_DEFAULT.CONFIG;
		REG.CONFIG.PRIM_RX=0;
		REG.CONFIG.PWR_UP=1;
		write_register(REG_ADDR.CONFIG, 2);

		REG.RF_CH.RF_CH=1;
		write_register(REG_ADDR.RF_CH, REG.RF_CH.value);

		REG.RF_SETUP.CONT_WAVE=1;
		REG.RF_SETUP.PLL_LOCK=1;
		write_register(REG_ADDR.RF_SETUP, 0x9F);
		_CE_PIN->Set();
	} else {
		_CE_PIN->Reset();
		REG.CONFIG=REG_DEFAULT.CONFIG;
		REG.RF_SETUP=REG_DEFAULT.RF_SETUP;
		write_register(REG_ADDR.CONFIG, REG.CONFIG.value);
		write_register(REG_ADDR.RF_SETUP, REG.RF_SETUP.value);
	}
}


void NRF24L01P::StartRX(uint16_t size, uint32_t freqHz) {
	REG.CONFIG.PRIM_RX=1;
	REG.CONFIG.PWR_UP=1;
	write_register(REG_ADDR.CONFIG, REG.CONFIG.value);
	_CE_PIN->Set();
}

uint8_t NRF24L01P::StartRX_ESB(uint8_t channel, uint64_t address, uint16_t size, uint8_t en_auto_ack) {
	uint8_t error=0;
	REG.CONFIG.value=read_register(REG_ADDR.CONFIG);
	REG.CONFIG.PRIM_RX=1;
	REG.CONFIG.PWR_UP=1;
	write_register(REG_ADDR.CONFIG, REG.CONFIG.value);

	REG.STATUS.value=nop();
	REG.STATUS.MAX_RT=1;
	REG.STATUS.RX_DR=1;
	REG.STATUS.TX_DS=1;
	write_register(REG_ADDR.STATUS, REG.STATUS.value);

	if(REG.RF_CH.RF_CH!=channel){
		REG.RF_CH.RF_CH=channel;
		write_register(REG_ADDR.RF_CH, REG.RF_CH.value);
	}

	if(REG.RX_ADDR_P0.RX_ADDR_P0 != address){
		REG.RX_ADDR_P0.RX_ADDR_P0 = address;
		write_register(REG_ADDR.RX_ADDR_P0, REG.RX_ADDR_P0.value, 5);
	}

	if(!REG.EN_RXADDR.ERX_P0){
		REG.EN_RXADDR.ERX_P0=1;
		write_register(REG_ADDR.EN_RXADDR, REG.EN_RXADDR.value);
	}

	if(REG.RX_PW_P0.RX_PW_P0 != size && !REG.DYNPD.DPL_P0){
		REG.RX_PW_P0.RX_PW_P0 = size;
		write_register(REG_ADDR.RX_PW_P0, REG.RX_PW_P0.value);
	}

	if(REG.EN_AA.ENAA_P0 != (en_auto_ack!=0)){
		REG.EN_AA.ENAA_P0=(en_auto_ack!=0);
		write_register(REG_ADDR.EN_AA, REG.EN_AA.value);
	}
	flush_rx();
	flush_tx();

	_CE_PIN->Set();
	delay_ticks(21840);//130us
	return error;
}

uint8_t NRF24L01P::TxPackage_ESB(uint8_t no_ack, uint8_t* data, uint16_t size){
	return TxPackage_ESB(REG.RF_CH.RF_CH, REG.TX_ADDR.TX_ADDR, no_ack, data, size);
}

uint8_t NRF24L01P::TxPackage_ESB(uint8_t channel, uint64_t address, uint8_t no_ack, uint8_t* data, uint16_t size) {
	uint8_t error=0;
	_CE_PIN->Reset();
	_busy=1;
	REG.CONFIG.value=read_register(REG_ADDR.CONFIG);
	REG.CONFIG.PRIM_RX=0;
	REG.CONFIG.PWR_UP=1;
	write_register(REG_ADDR.CONFIG, REG.CONFIG.value);
	delay_ticks(25200);//150us

	if(size>32){
		size=32;
	}

	if(REG.RF_CH.RF_CH!=channel){
		REG.RF_CH.RF_CH=channel;
		write_register(REG_ADDR.RF_CH, REG.RF_CH.value);
	}
	if(REG.TX_ADDR.TX_ADDR!=address){
		REG.TX_ADDR.TX_ADDR=address;
		write_register(REG_ADDR.TX_ADDR, REG.TX_ADDR.value, 5);
	}

	if(REG.RX_ADDR_P0.RX_ADDR_P0!=address){
		REG.RX_ADDR_P0.RX_ADDR_P0=address;
		write_register(REG_ADDR.RX_ADDR_P0, REG.RX_ADDR_P0.value, 5);
	}
// Erro quando transmite para outro robo

	if(no_ack){
		write_tx_payload_no_ack(data, size);
	} else {
		write_tx_payload(data, size);
	}
	_CE_PIN->Set();
	delay_ticks(2520); //15us
	return error;
}


void NRF24L01P::InterruptCallback(){
	static uint32_t led_a_time=0;
	static uint32_t led_b_time=0;
	static uint32_t led_c_time=0;
	static uint32_t led_d_time=0;

//	if(_NIRQ_PIN->Read()){
		REG.STATUS.value=nop();
		REG.FIFO_STATUS.value=read_register(REG_ADDR.FIFO_STATUS);
		if(REG.STATUS.MAX_RT){
			flush_tx();
			REG.STATUS.value=0;
			REG.STATUS.MAX_RT=1;
			write_register(REG_ADDR.STATUS, REG.STATUS.value);
			REG.STATUS.value=nop();
			_retransmit_irq_count++;
			_busy=0;
			//MUDAR OS NOMES DOS OBJETOS DOS LEDS
			//led_c.On();
			//led_c_time=GetLocalTime();
		}
		if(REG.STATUS.RX_DR||(!REG.FIFO_STATUS.RX_EMPTY)){
			_receive_irq_count++;
			uint8_t payloadsize=read_rx_payload_width();
			if(payloadsize>32) {
				flush_rx();
				//led_c.On();
				//led_c_time=GetLocalTime();
			} else {
				uint8_t buffer[32];
				read_rx_payload(buffer, payloadsize);
				_rxbuffer.In(buffer, payloadsize);
				//led_d.On();
				//led_d_time=GetLocalTime();
			}
			if(REG.STATUS.RX_DR){
				REG.STATUS.value=0;
				REG.STATUS.RX_DR=1;
				write_register(REG_ADDR.STATUS, REG.STATUS.value);
			}
			REG.STATUS.value=nop();
		}
		if(REG.STATUS.TX_DS||(REG.FIFO_STATUS.TX_EMPTY&&(_busy==1))){
			_transmit_irq_count++;
			_CE_PIN->Reset();
			if(REG.STATUS.RX_DR){
				REG.STATUS.value=0;
				REG.STATUS.TX_DS=1;
				write_register(REG_ADDR.STATUS, REG.STATUS.value);
			}
			REG.STATUS.value=nop();
			_busy=0;
			//led_a.On();
			//led_a_time=GetLocalTime();
		}
		if(REG.STATUS.TX_FULL||REG.FIFO_STATUS.TX_FULL){
			nrf24.flush_tx();
			REG.STATUS.value=nop();
			_busy=0;
			//led_b.On();
			//led_b_time=GetLocalTime();
		}
		//if(led_a_time && (GetLocalTime()-led_a_time)>50) {led_a.Off(); led_a_time=0;}
		//if(led_b_time && (GetLocalTime()-led_b_time)>50) {led_b.Off(); led_b_time=0;}
		//if(led_c_time && (GetLocalTime()-led_c_time)>50) {led_c.Off(); led_c_time=0;}
		//if(led_d_time && (GetLocalTime()-led_d_time)>50) {led_d.Off(); led_d_time=0;}
//	}
}


const NRF24L01P::REG_ADDRS NRF24L01P::REG_ADDR ={	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
													0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13,
													0x14, 0x15, 0x16, 0x17, 0x1C, 0x1D};

const NRF24L01P::NRF24_REG NRF24L01P::REG_DEFAULT={
		.CONFIG={{
				.PRIM_RX=0,
				.PWR_UP=0,
				.CRCO=0,
				.EN_CRC=1,
				.MASK_MAX_RT=0,
				.MASK_TX_DS=0,
				.MASK_RX_DR=0,
				.Reserved=0,
		}},
		.EN_AA={{
				.ENAA_P0=1,
				.ENAA_P1=1,
				.ENAA_P2=1,
				.ENAA_P3=1,
				.ENAA_P4=1,
				.ENAA_P5=1,
				.Reserved=0,
		}},
		.EN_RXADDR={{
				.ERX_P0=1,
				.ERX_P1=1,
				.ERX_P2=0,
				.ERX_P3=0,
				.ERX_P4=0,
				.ERX_P5=0,
				.Reserved=0,
		}},
		.SETUP_AW={{
				.AW=0b11,
				.Reserved=0,
		}},
		.SETUP_RETR={{
				.ARC=0b0011,
				.ARD=0b0011,
		}},
		.RF_CH={{
				.RF_CH=0b0000010,
				.Reserved=0,
		}},
		.RF_SETUP={{
				.Obsolete=0,
				.RF_PWR=0b11,
				.RF_DR_HIGH=1,
				.PLL_LOCK=0,
				.RF_DR_LOW=0,
				.Reserved=0,
				.CONT_WAVE=0,
		}},
		.STATUS={{
				.TX_FULL=0,
				.RX_P_NO=0b111,
				.MAX_RT=0,
				.TX_DS=0,
				.RX_DR=0,
				.Reserved=0,
		}},
		.OBSERVE_TX={{
				.ARC_CNT=0,
				.PLOS_CNT=0,
		}},
		.RPD={{
				.RPD=0,
				.Reserved=0,
		}},
		.RX_ADDR_P0={{
				.RX_ADDR_P0=0xE7E7E7E7E7,
		}},
		.RX_ADDR_P1={{
				.RX_ADDR_P1=0xC2C2C2C2C2,
		}},
		.RX_ADDR_P2={{
				.RX_ADDR_P2=0xC3,
		}},
		.RX_ADDR_P3={{
				.RX_ADDR_P3=0xC4,
		}},
		.RX_ADDR_P4={{
				.RX_ADDR_P4=0xC5,
		}},
		.RX_ADDR_P5={{
				.RX_ADDR_P5=0xC6,
		}},
		.TX_ADDR={{
				.TX_ADDR=0xE7E7E7E7E7,
		}},
		.RX_PW_P0={{
				.RX_PW_P0=0,
				.Reserved=0,
		}},
		.RX_PW_P1={{
				.RX_PW_P1=0,
				.Reserved=0,
		}},
		.RX_PW_P2={{
				.RX_PW_P2=0,
				.Reserved=0,
		}},
		.RX_PW_P3={{
				.RX_PW_P3=0,
				.Reserved=0,
		}},
		.RX_PW_P4={{
				.RX_PW_P4=0,
				.Reserved=0,
		}},
		.RX_PW_P5={{
				.RX_PW_P5=0,
				.Reserved=0,
		}},
		.FIFO_STATUS={{
				.RX_EMPTY=1,
				.RX_FULL=0,
				.Reserved1=0,
				.TX_EMPTY=1,
				.TX_FULL=0,
				.TX_REUSE=0,
				.Reserved=0,
		}},
		.DYNPD={{
				.DPL_P0=0,
				.DPL_P1=0,
				.DPL_P2=0,
				.DPL_P3=0,
				.DPL_P4=0,
				.DPL_P5=0,
				.Reserved=0,
		}},
		.FEATURE={{
				.EN_DYN_ACK=0,
				.EN_ACK_PAY=0,
				.EN_DPL=0,
				.Reserved=0,
		}},
};
