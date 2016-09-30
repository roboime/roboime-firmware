#include <radio/bsp.h>
#include <hal/nrf24l01p.h>
#include <utils/time_functions.h>
NRF24_REG REG;

NRF24L01P::NRF24L01P(SPI &spi, IO_Pin &SS_PIN, IO_Pin &NIRQ_PIN):
	MODEM(),
	_spi(&spi),
	_SS_PIN(&SS_PIN),
	_NIRQ_PIN(&NIRQ_PIN),
	_rxbuffer(0,2048),
	_txbuffer(0,2048),
	_channel(0)
{
}



uint8_t NRF24L01P::SelfTest(){
	return 1;
}

void NRF24L01P::Config(){
}

void NRF24L01P::InterruptCallback(){
}


const NRF24_REG REG_DEFAULT={
		.CONFIG={{
				.Reserved=0,
				.MASK_RX_DR=0,
				.MASK_TX_DS=0,
				.MASK_MAX_RT=0,
				.EN_CRC=1,
				.CRCO=0,
				.PWR_UP=0,
				.PRIM_RX=0
		}},
		.EN_AA={{
				.Reserved=0,
				.ENAA_P5=1,
				.ENAA_P4=1,
				.ENAA_P3=1,
				.ENAA_P2=1,
				.ENAA_P1=1,
				.ENAA_P0=1,
		}},
		.EN_RXADDR={{
				.Reserved=0,
				.ERX_P5=0,
				.ERX_P4=0,
				.ERX_P3=0,
				.ERX_P2=0,
				.ERX_P1=1,
				.ERX_P0=1,
		}},
		.SETUP_AW={{
				.Reserved=0,
				.AW=0b11,
		}},
		.SETUP_RETR={{
				.ARD=0,
				.ARC=0b0011,
		}},
		.RF_CH={{
				.Reserved=0,
				.RF_CH=0b0000010,
		}},
		.RF_SETUP={{
				.CONT_WAVE=0,
				.Reserved=0,
				.RF_DR_LOW=0,
				.PLL_LOCK=0,
				.RF_DR_HIGH=1,
				.RF_PWR=0b11,
				.Obsolete=0,
		}},
		.STATUS={{
				.Reserved=0,
				.RX_DR=0,
				.TX_DS=0,
				.MAX_RT=0,
				.RX_P_NO=0b111,
				.TX_FULL=0,
		}},
		.OBSERVE_TX={{
				.PLOS_CNT=0,
				.ARC_CNT=0,
		}},
		.RPD={{
				.Reserved=0,
				.RPD=0,
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
				.Reserved=0,
				.RX_PW_P0=0,
		}},
		.RX_PW_P1={{
				.Reserved=0,
				.RX_PW_P1=0,
		}},
		.RX_PW_P2={{
				.Reserved=0,
				.RX_PW_P2=0,
		}},
		.RX_PW_P3={{
				.Reserved=0,
				.RX_PW_P3=0,
		}},
		.RX_PW_P4={{
				.Reserved=0,
				.RX_PW_P4=0,
		}},
		.RX_PW_P5={{
				.Reserved=0,
				.RX_PW_P5=0,
		}},
		.FIFO_STATUS={{
				.Reserved=0,
				.TX_REUSE=0,
				.TX_FULL=0,
				.TX_EMPTY=1,
				.Reserved1=0,
				.RX_FULL=0,
				.RX_EMPTY=1,
		}},
		.DYNPD={{
				.Reserved=0,
				.DPL_P5=0,
				.DPL_P4=0,
				.DPL_P3=0,
				.DPL_P2=0,
				.DPL_P1=0,
				.DPL_P0=0,
		}},
		.FEATURE={{
				.Reserved=0,
				.EN_DPL=0,
				.EN_ACK_PAY=0,
				.EN_DYN_ACK=0,
		}},
};



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

uint8_t NRF24L01P::read_register(uint8_t address, uint8_t* buffer, uint8_t size) {
	return read(CMD::R_REGISTER | address, buffer, size);
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
	NRF24L01_CONFIG status;
	REG.CONFIG=REG_DEFAULT.CONFIG;
	REG.CONFIG.value=0;
	status.value=read_register(NRF24L01_REGISTER_ADDR.CONFIG, &REG.CONFIG.value, 1);
	status.value=nop();
	status.value=nop();
	status.value=read_rx_payload_width();

	if(status.value==0){
		status.value++;
	}
}
