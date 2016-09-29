#include <hal/nrf24l01p.h>
#include <utils/time_functions.h>

NRF24L01P::NRF24L01P(SPI &spi, IO_Pin &SS_PIN, IO_Pin &NIRQ_PIN):
	MODEM(),
	_spi(&spi),
	_SS_PIN(&SS_PIN),
	_NIRQ_PIN(&NIRQ_PIN),
	_rxbuffer(0,2048),
	_txbuffer(0,2048),
	_channel(0),
	_flags(),
	_mode(ModeInit),
	_receivesize(2048),
	_receivecount(0),
	_receivesizegoing(0),
	_rssi(0)
{
}

void NRF24L01P::Init(){
}

uint8_t NRF24L01P::SelfTest(){
	return 1;
}

void NRF24L01P::Config(){
	_flags.INIT_STATUS=0;
	uint8_t *cmd_ptr=(uint8_t*)SI4464_CFG_256KBPS;
	uint8_t cmd_size=*cmd_ptr++;
	cmd_ptr+=cmd_size;
	cmd_size=*cmd_ptr++;

	while(cmd_size){
		write(cmd_ptr,cmd_size);
		cmd_ptr+=cmd_size;
		cmd_size=*cmd_ptr++;
		if(read_response(0,0,10)==-1){
			return;
		}
	}
	delay_ticks(10000);
	cmd({0xF1, 0xF0, 0x01, 0x0A});
	read_response(0,0,10);

	SetMode(ModeRX, _receivesize, 430000000);

//	uint8_t int_req[]={0x20, 0x00, 0x00, 0x00};
//	write(int_req,4);
	cmd({0x20, 0x00, 0x00, 0x00});
	_flags.INIT_STATUS=1;
}
#include <radio/bsp.h>

void NRF24L01P::cmd_GET_INT_STATUS(INTERRUPT_STATUS& intstatus) {
	uint8_t int_req[]={0x20, 0x00, 0x00, 0x00, 0x15, 0x02, 0x32};
	write(int_req,4);
	int_req[0]=read_response(intstatus.buffer,8,10);
	if(int_req[0]==10){
		return ; //error
	}
}

void NRF24L01P::InterruptCallback(){
//	if(_flags.INIT_STATUS){
//
//		INTERRUPT_STATUS int_status;
//		cmd_GET_INT_STATUS(int_status);
//
//		if(int_status.RX_FIFO_ALMOST_FULL_PEND){
//			uint8_t rxfifocount=get_RX_FIFO_COUNT();
//			if(rxfifocount>64) rxfifocount=64;
//			if(_receivecount>_receivesizegoing)
//				rxfifocount=0;
//			else if(rxfifocount>(_receivesizegoing-_receivecount))
//				rxfifocount=_receivesizegoing-_receivecount;
//			if(rxfifocount){
//				read_rx_fifo(_rxfifobuffer, rxfifocount);
//				_rxbuffer.In(_rxfifobuffer, rxfifocount);
//				_receivecount+=rxfifocount;
//			}
//			if(rxfifocount>32){
//				rxfifocount++;
//			}
//			if(_receivecount>=_receivesizegoing){
//				SetMode(ModeReady, _receivesize, _rxfrequency);
//			}
//		}
//
//		if(int_status.CMD_ERROR && int_status.CMD_ERROR_PEND){
//			SetMode(ModeRX, _receivesize, _rxfrequency);
//		}
//		if(int_status.FIFO_UNDERFLOW_OVERFLOW_ERROR_PEND){
//			_receivecount++;
//		}
//		if(int_status.PREAMBLE_DETECT_PEND){
//		}
//
//		if(int_status.INVALID_SYNC_PEND){
//		}
//
//		if(int_status.SYNC_DETECT_PEND){
//			_flags.RECEIVING=1;
//			MODEM_STATUS ms;
//			cmd(0x22);
//			read_response(ms.buffer,8,10);
//			_rssi=ms.LATCH_RSSI;
//		}
//
//		if(int_status.PACKET_SENT_PEND){
//			_txbuffer.Clear();
//			SetMode(ModeRX, _receivesize, _rxfrequency);
//		} else if(int_status.TX_FIFO_ALMOST_EMPTY_PEND){
//			uint16_t len=_txbuffer.Ocupied();
//			if(len!=0){
//				uint8_t txfifospace=get_TX_FIFO_SPACE();
//				if(txfifospace>64) txfifospace=64;
//				if(len>txfifospace){
//					len=txfifospace;
//				}
//				if(len){
//					_txbuffer.InBeginning(0x66); //Write TX FIFO cmd
//					_txbuffer.Out(_txfifobuffer, len+1);
//					write(_txfifobuffer, len+1);
//				}
//			}
//		}
//
//		if(int_status.STATE_CHANGE_PEND){
//		}
//
//		if(!_NIRQ_PIN->Read()){
////			cmd_GET_INT_STATUS(int_status);
////			if(int_status.CHIP_INT_STATUS_PEND || int_status.MODEM_INT_STATUS_PEND || int_status.PH_INT_STATUS_PEND){
//				GeneratePoll();
////			} else {
////			}
//		}
//	}
}
uint8_t NRF24L01P::GetRSSI() {
	return _rssi;
}

uint8_t NRF24L01P::TxReady(){
	return !_txbuffer.Ocupied();
}

void NRF24L01P::TxPackage(uint8_t* data, uint16_t size, uint32_t freqHz, MODEM_POWER power){
	if(size){
		_txbuffer.Clear();
		_txbuffer.In(data, size);
		uint16_t len;
		len=size;
		_NIRQ_PIN->Interrupt(0);
		SetMode(ModeTX, size, freqHz, power);
		if(len>64) len=64;
		_txbuffer.InBeginning(0x66);
		_txbuffer.Out(_txfifobuffer, len+1);
		write(_txfifobuffer, len+1);
		cmd_START_TX(0, 0x00, size);
		_NIRQ_PIN->Interrupt(1);
	}
}

void NRF24L01P::StartRX(uint16_t size, uint32_t freqHz) {
	_NIRQ_PIN->Interrupt(0);
	_rxbuffer.Clear();
	SetMode(ModeRX, size, freqHz);
	_NIRQ_PIN->Interrupt(1);
}

uint16_t NRF24L01P::RxSize(){
	return _rxbuffer.Ocupied();
}

uint16_t NRF24L01P::RxData(uint8_t *data, uint16_t maxsize){
	return _rxbuffer.Out(data, maxsize);
}

uint8_t NRF24L01P::Busy() {
	return (_flags.TRANSMITTING | _flags.RECEIVING);
}


void NRF24L01P::ContinueRX(uint16_t size) {
//	_receivesizegoing=size;
//	if(_receivecount<_receivesizegoing){
//		uint8_t sizealmostfull=0x30;
//		if((_receivesizegoing-_receivecount)<0x30){
//			sizealmostfull=_receivesizegoing-_receivecount;
//		}
//		_NIRQ_PIN->Interrupt(0);
//		cmd_SET_PKT_RX_THRESHOLD(sizealmostfull);
//		_NIRQ_PIN->Interrupt(1);
//	}
}




uint8_t NRF24L01P::get_RX_FIFO_COUNT() {
//	uint8_t cmd[]={0x15,0x00};
//	write(cmd,2);
//	if(read_response(cmd,2,10)==10){
//		return 0;
//	} else {
//		return cmd[0];
//	}
}
void NRF24L01P::clear_RX_FIFO() {
//	uint8_t cmd[]={0x15,0x02};
//	write(cmd,2);
//	read_response(cmd,2,10);
}

void NRF24L01P::clear_RX_TX_FIFO() {
//	uint8_t cmd[]={0x15,0x03};
//	write(cmd,2);
//	read_response(cmd,2,10);
}

void NRF24L01P::clear_TX_FIFO() {
//	uint8_t cmd[]={0x15,0x01};
//	write(cmd,2);
//	read_response(cmd,2,10);
}

uint8_t NRF24L01P::get_TX_FIFO_SPACE() {
//	uint8_t cmd[]={0x15,0x00};
//	write(cmd,2);
//	if(read_response(cmd,2,10)==10){
//		return 0;
//	} else {
//		return cmd[1];
//	}
}

void NRF24L01P::cmd_START_TX(uint8_t channel, uint8_t condition, uint16_t tx_len) {
//	uint8_t cmd[]={NRF24L01P_CMD_START_TX, channel, condition, (uint8_t)(tx_len>>8), (uint8_t)tx_len};
//	write(cmd,5);
//	read_response(0,1,10);
}
void NRF24L01P::cmd_START_RX(uint8_t channel, uint8_t condition, uint16_t rx_len, uint8_t rx_timeout_state, uint8_t rx_valid_state, uint8_t rx_invalid_state) {
//	uint8_t cmd[]={0x32, channel, condition, (uint8_t)(rx_len>>8), (uint8_t)rx_len, rx_timeout_state, rx_valid_state, rx_invalid_state};
//	write(cmd,8);
//	read_response(0,1,10);
}

void NRF24L01P::SetMode(NRF24L01P_Mode mode, uint16_t size, uint32_t freqHz, MODEM_POWER power){
////	static const uint8_t ChangeState_transTX[] = 		{ NRF24L01P_CMD_CHANGE_STATE, 0x07};
////	static const uint8_t ChangeState_Ready[] = 			{ NRF24L01P_CMD_CHANGE_STATE, 0x03};
////	static const uint8_t ChangeState_RX[] = 			{ NRF24L01P_CMD_CHANGE_STATE, 0x08};
//	static const uint8_t RF_GPIO_PIN_CFG_RX[] =			{ NRF24L01P_GPIO_PIN_CFG, 0x04, 0x00, 0x02, 0x03, 0x00, 0x00, 0x00};
////	static const uint8_t RF_GPIO_PIN_CFG_RX_PA_OFF[] =	{ NRF24L01P_GPIO_PIN_CFG, 0x04, 0x02, 0x02, 0x03, 0x00, 0x00, 0x00};
////	static const uint8_t RF_GPIO_PIN_CFG_RX_PA_ON[] =	{ NRF24L01P_GPIO_PIN_CFG, 0x04, 0x03, 0x02, 0x03, 0x00, 0x00, 0x00};
////	static const uint8_t RF_GPIO_PIN_CFG_TX_PA_OFF[]=	{ NRF24L01P_GPIO_PIN_CFG, 0x04, 0x02, 0x03, 0x02, 0x00, 0x00, 0x00};
//	static const uint8_t RF_GPIO_PIN_CFG_TX_PA_ON[]=	{ NRF24L01P_GPIO_PIN_CFG, 0x04, 0x03, 0x03, 0x02, 0x00, 0x00, 0x00};
//
//	switch(mode){
//	case ModeReady:
//		if(_mode!=ModeReady){
//			if(_mode!=ModeRX){
//				write((uint8_t*)RF_GPIO_PIN_CFG_RX,sizeof(RF_GPIO_PIN_CFG_RX));
//				read_response(0,1,10);
//			}
//			cmd({NRF24L01P_CMD_CHANGE_STATE, 0x06});
//			read_response(0,1,10);
//			clear_RX_FIFO();
//			cmd_SET_PKT_RX_THRESHOLD(0x20);
//		}
//		_mode=ModeReady;
//		_flags.TRANSMITTING=0;
//		_flags.RECEIVING=0;
//		_receivesize=size;
//		_receivecount=0;
//		_receivesizegoing=size;
//		break;
//	case ModeRX:
//		settxpower(MODEM_POWER_ZERO);
//		_rxfrequency=freqHz;
//		setfrequency(_rxfrequency);
//		SetMode(ModeReady, size, freqHz);
//		cmd_START_RX(0,0,size);
//		_mode=ModeRX;
//		break;
//	case ModeTX:
//		_txfrequency=freqHz;
//		setfrequency(freqHz);
//		settxpower(power);
//		if(_mode!=ModeTX){
//			write((uint8_t*)RF_GPIO_PIN_CFG_TX_PA_ON,8);
//			read_response(0,1,10);
//		}
//		cmd({ NRF24L01P_CMD_CHANGE_STATE, 0x05});
//		read_response(0,1,10);
//		clear_TX_FIFO();
//		_flags.TRANSMITTING=1;
//		_flags.RECEIVING=0;
//		_mode=ModeTX;
//		break;
//	case ModeInit:
//		break;
//	}
}

void NRF24L01P::CW(uint8_t state) {
//	cmd({NRF24L01P_CMD_CHANGE_STATE, 0x03});
//	read_response(0,2,10);
//	cmd({NRF24L01P_GPIO_PIN_CFG, 0x04, 0x03, 0x03, 0x02, 0x00, 0x00, 0x00});
//	read_response(0,2,10);
//	cmd({NRF24L01P_SET_PROPERTY, 0x20, 0x01, 0x00, 0x00});
//	read_response(0,2,10);
//	if(state){
//
//		cmd(NRF24L01P_CMD_START_TX);
//		read_response(0,2,10);
//	}
}

uint8_t NRF24L01P::cmd(std::initializer_list<uint8_t> c) {
	uint8_t resp=0;
//	_spi->Assert();
//	for (const uint8_t* i = c.begin(); i != c.end(); i++) {
//		resp+=_spi->WriteByte(*i);
//	}
//	_spi->Release();
	return resp;
}


uint8_t NRF24L01P::cmd(uint8_t cmd) {
	uint8_t resp;
//	_spi->Assert();
//	resp=_spi->WriteByte(cmd);
//	_spi->Release();
	return resp;
}

uint32_t NRF24L01P::write(uint8_t *data, uint16_t size){
	uint8_t resp;
//	_spi->Assert();
//	resp=_spi->WriteBuffer(data, size);
//	_spi->Release();
	return resp;
}

uint32_t NRF24L01P::read_rx_fifo(uint8_t *data, uint16_t size){
//	_spi->Assert();
//	_spi->WriteByte(0x77);
//	while(size--){
//		*data++=_spi->WriteByte(0xFF);
//	}
//	_spi->Release();
	return 0;
}

int8_t NRF24L01P::read_response(uint8_t *buffu, uint16_t size, uint8_t retries){
	uint8_t retries_tmp=0;
//	uint8_t resp=0;
//	uint8_t data=0;
//	while(resp!=0xFF && retries_tmp<retries){
//		_spi->Assert();
//		resp=_spi->WriteByte(0x44);
//		resp=_spi->WriteByte(0xFF);
//		if(resp==0xFF){
//			while(size--){
//				data=_spi->WriteByte(0xFF);
//				if(buffu)
//					(*buffu++)=data;
//			}
//		} else {
//			retries_tmp++;
//		}
//		_spi->Release();
//	}
//	if(retries_tmp==retries){
//		timeout();
//		return -1;
//	}
	return retries_tmp;
}

uint16_t NRF24L01P::timeout(){
	return 0;
}

void NRF24L01P::setfrequency(uint32_t freq){
//	uint8_t freqcmd[]={NRF24L01P_SET_PROPERTY, 0x40, 0x04, 0x00, 0x41, 0x09, 0x3B, 0x13};
//	uint8_t f_integ=freq/6500000-1; //65
//	uint64_t frac=(freq-f_integ*6500000);
//	frac<<=14;
//	frac/=203125;
//	freqcmd[4]=f_integ;
//	freqcmd[5]=(frac>>16) & 0xff;
//	freqcmd[6]=(frac>>8) & 0xff;
//	freqcmd[7]=frac & 0xff;
//
//	write(freqcmd,8);
//	read_response(0,1,10);
}

uint32_t NRF24L01P::getfrequency() {
//	uint8_t f_integ;
//	uint64_t frac;
	uint32_t freq=0;
//
//	uint8_t freqcmd[]={NRF24L01P_GET_PROPERTY, 0x40, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00};
//	write(freqcmd,4);
//	read_response(freqcmd+4,4,10);
//
//	f_integ=freqcmd[4];
//	frac=freqcmd[5]<<16;
//	frac|=freqcmd[6]<<8;
//	frac|=freqcmd[7];
//
//	frac*=203125;
//	frac>>=14;
//	freq=f_integ*6500000+1;
//	freq+=frac;
	return freq;
}

void NRF24L01P::settxpower(MODEM_POWER power){
	switch(power){
	case MODEM_POWER_HIGHEST:
		break;
	case MODEM_POWER_HIGH:
		break;
	case MODEM_POWER_MEDIUM:
		break;
	case MODEM_POWER_LOW:
		break;
	case MODEM_POWER_ZERO:
		break;
	default:
		return;
	}
}

void NRF24L01P::cmd_SET_PKT_RX_THRESHOLD(uint8_t size){
//	cmd({NRF24L01P_SET_PROPERTY, 0x12, 0x01, 0x0C, size});
//	read_response(0,1,10);
}

void NRF24L01P::GeneratePoll() {
}

void NRF24L01P::SetRXFrequency(uint32_t freq) {
}

void NRF24L01P::ircal(uint32_t freq) {
//	uint32_t ircalfreq=freq/26000000+1;
//	ircalfreq*=26000000;
//	ircalfreq+=26000000/32;
//	setfrequency(ircalfreq);
//	cmd({0x56, 0x10, 0xCA, 0xF0});
//	read_response(0,1,10);
//	cmd({NRF24L01P_GPIO_PIN_CFG, 0x10, 0xCA, 0xF0});
//	read_response(0,1,10);
//	setfrequency(freq);
}

void NRF24L01P::Calibrate(uint32_t freq) {
	ircal(freq);
}

void NRF24L01P::SetFrequency(uint32_t freq) {
	setfrequency(freq);
}

uint32_t NRF24L01P::GetFrequency() {
	return getfrequency();
}

void NRF24L01P::Recovery() {
}

