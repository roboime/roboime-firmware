#pragma once
#pragma once

#include <inttypes.h>
#include <hal/modem.h>
#include <hal/spi.h>
#include <hal/io_pin.h>
#include <utils/circularbuffer.h>
#include <initializer_list>
#include <radio/rf_power_ctrl.h>

class NRF24L01P: public MODEM{
public:
	NRF24L01P(SPI &spi, IO_Pin &SS_PIN, IO_Pin &CE_PIN, IO_Pin &NIRQ_PIN);

	uint8_t TxReady();
	void TxPackage(uint8_t *data, uint16_t size, uint32_t frequency, MODEM_POWER power);
	uint16_t RxSize();
	uint16_t RxData(uint8_t *data, uint16_t maxsize);
	void StartRX(uint16_t size, uint32_t freqHz);
	void ContinueRX(uint16_t size){}
	void GeneratePoll(){}
	void SetRXFrequency(uint32_t freqHz);
	uint8_t Busy(){return _busy;}
	void CW(uint8_t state);
	uint8_t GetRSSI();
	void Calibrate(uint32_t freq){}
	void Recovery(){}


	void InterruptCallback();
	void Init();
	void Config();
	uint8_t SelfTest();
	uint8_t Scan(uint8_t *buffer);
	uint8_t TxPackage_ESB(uint8_t channel, uint64_t address, uint8_t no_ack, uint8_t* data, uint16_t size);
	uint8_t TxPackage_ESB(uint8_t no_ack, uint8_t* data, uint16_t size);
	uint8_t StartRX_ESB(uint8_t channel, uint64_t address, uint16_t size, uint8_t en_auto_ack);
	uint8_t write_ack_payload(uint8_t *buffer, uint8_t size);

protected:
	uint8_t write_register(uint8_t address, uint8_t *buffer, uint8_t size);
	uint8_t write_register(uint8_t address, uint8_t value);

	uint8_t read_register(uint8_t address, uint8_t *buffer, uint8_t size);
	uint8_t read_register(uint8_t address, uint8_t *buffer);
	uint8_t read_register(uint8_t address);

	uint8_t read_rx_payload(uint8_t *buffer, uint8_t size);
	uint8_t write_tx_payload(uint8_t *buffer, uint8_t size);
	uint8_t write_tx_payload_no_ack(uint8_t *buffer, uint8_t size);
	uint8_t flush_tx();
	uint8_t flush_rx();
	uint8_t reuse_tx_pl();
	uint8_t read_rx_payload_width();
	uint8_t nop();

	uint8_t write(uint8_t cmd);
	uint8_t write(uint8_t command, uint8_t *buffer, uint8_t size);
	uint8_t read(uint8_t command, uint8_t *buffer, uint8_t size);

	SPI *_spi;
	IO_Pin *_SS_PIN;
	IO_Pin *_NIRQ_PIN;
	IO_Pin *_CE_PIN;
	CircularBuffer<uint8_t,false> _rxbuffer;
	CircularBuffer<uint8_t,false> _txbuffer;

	uint8_t _channel;
	uint32_t _retransmit_irq_count;
	uint32_t _receive_irq_count;
	uint32_t _transmit_irq_count;
	uint8_t _busy;

	typedef enum {
		R_REGISTER				=0x00,
		W_REGISTER				=0x20,
		R_RX_PAYLOAD			=0x61,
		W_TX_PAYLOAD			=0xA0,
		FLUSH_TX				=0xE1,
		FLUSH_RX				=0xE2,
		REUSE_TX_PL				=0xE3,
		R_RX_PL_WID				=0x60,
		W_ACK_PAYLOAD			=0xA8,
		W_TX_PAYLOAD_NOACK		=0xB0,
		NOP						=0xFF,
	} CMD;

	typedef union{
		struct {											//Configuration Register
			uint8_t PRIM_RX:1;								//RX/TX control
															//1: PRX, 0: PTX
			uint8_t PWR_UP:1;								//1: POWER UP, 0:POWER DOWN
			uint8_t CRCO:1;									//CRC encoding scheme
															//'0' - 1 byte
															//'1' – 2 bytes
			uint8_t EN_CRC:1;								//Enable CRC. Forced high if one of the bits in the
															//EN_AA is high
			uint8_t MASK_MAX_RT:1;							//Mask interrupt caused by MAX_RT
															//1: Interrupt not reflected on the IRQ pin
															//0: Reflect MAX_RT as active low interrupt on the
															//IRQ pin
			uint8_t MASK_TX_DS:1;							//Mask interrupt caused by TX_DS
															//1: Interrupt not reflected on the IRQ pin
															//0: Reflect TX_DS as active low interrupt on the IRQ
															//pin
			uint8_t MASK_RX_DR:1;							//Mask interrupt caused by RX_DR
															//1: Interrupt not reflected on the IRQ pin
															//0: Reflect RX_DR as active low interrupt on the
															//IRQ pin
			uint8_t Reserved:1;								//Only '0' allowed
		};
		uint8_t value;
	} NRF24L01_CONFIG;


	typedef union{
		struct {											//Enable ‘Auto Acknowledgment’ Function Disable
															//this functionality to be compatible with nRF2401,
															//see page 75
			uint8_t ENAA_P0:1;								//Enable auto acknowledgement data pipe 0
			uint8_t ENAA_P1:1;								//Enable auto acknowledgement data pipe 1
			uint8_t ENAA_P2:1;								//Enable auto acknowledgement data pipe 2
			uint8_t ENAA_P3:1;								//Enable auto acknowledgement data pipe 3
			uint8_t ENAA_P4:1;								//Enable auto acknowledgement data pipe 4
			uint8_t ENAA_P5:1;								//Enable auto acknowledgement data pipe 5
			uint8_t Reserved:2;								//Only '00' allowed

		};
		uint8_t value;
	} NRF24L01_EN_AA;

	typedef union{
		struct {											//Enabled RX Addresses
			uint8_t ERX_P0:1;								//Enable data pipe 0
			uint8_t ERX_P1:1;								//Enable data pipe 1
			uint8_t ERX_P2:1;								//Enable data pipe 2
			uint8_t ERX_P3:1;								//Enable data pipe 3
			uint8_t ERX_P4:1;								//Enable data pipe 4
			uint8_t ERX_P5:1;								//Enable data pipe 5
			uint8_t Reserved:2;								//Only '00' allowed
		};
		uint8_t value;
	} NRF24L01_EN_RXADDR;

	typedef union{
		struct {											//Setup of Address Widths
															//(common for all data pipes)
			uint8_t AW:2;									//RX/TX Address field width
															//'00' - Illegal
															//'01' - 3 bytes
															//'10' - 4 bytes
															//'11' – 5 bytes
															//LSByte is used if address width is below 5 bytes
			uint8_t Reserved:6;								//Only '00' allowed
		};
		uint8_t value;
	} NRF24L01_SETUP_AW;

	typedef union{
		struct {											//Setup of Automatic Retransmission
			uint8_t ARC:4;									//Auto Retransmit Count
															//‘0000’ –Re-Transmit disabled
															//‘0001’ – Up to 1 Re-Transmit on fail of AA
															//……
															//‘1111’ – Up to 15 Re-Transmit on fail of AA
			uint8_t ARD:4;									//Auto Retransmit Delay
															//‘0000’ – Wait 250μS
															//‘0001’ – Wait 500μS
															//‘0010’ – Wait 750μS
															//……..
															//‘1111’ – Wait 4000μS
															//(Delay defined from end of transmission to start of
															//next transmission)
															//Please take care when setting this parameter. If the ACK payload is more than 15 byte in 2Mbps mode the
															//ARD must be 500μS or more, if the ACK payload is more than 5byte in 1Mbps mode the ARD must be
															//500μS or more. In 250kbps mode (even when the payload is not in ACK) the ARD must be 500μS or more.
															//Please see section 7.4.2 on page 33 for more information.
															//This is the time the PTX is waiting for an ACK packet before a retransmit is made. The PTX is in RX mode
															//for 250μS (500μS in 250kbps mode) to wait for address match. If the address match is detected, it stays in
															//RX mode to the end of the packet, unless ARD elapses. Then it goes to standby-II mode for the rest of the
															//specified ARD. After the ARD it goes to TX mode and then retransmits the packet.
		};
		uint8_t value;
	} NRF24L01_SETUP_RETR;

	typedef union {
		struct {											//RF Channel
			uint8_t RF_CH:7;								//Sets the frequency channel nRF24L01+ operates
															//on
			uint8_t Reserved:1;								//Only '0' allowed
		};
		uint8_t value;
	} NRF24L01_RF_CH;


	typedef union {
		struct {											//RF Setup Register
			uint8_t Obsolete:1;								//Don’t care
			uint8_t RF_PWR:2;								//Set RF output power in TX mode
															//'00' – -18dBm
															//'01' – -12dBm
															//'10' – -6dBm
															//'11' – 0dBm
			uint8_t RF_DR_HIGH:1;							//Select between the high speed data rates. This bit
															//is don’t care if RF_DR_LOW is set.
															//Encoding:
															//[RF_DR_LOW, RF_DR_HIGH]:
															//‘00’ – 1Mbps
															//‘01’ – 2Mbps
															//‘10’ – 250kbps
															//‘11’ – Reserved
			uint8_t PLL_LOCK:1;								//Force PLL lock signal. Only used in test
			uint8_t RF_DR_LOW:1;							//Set RF Data Rate to 250kbps. See RF_DR_HIGH
															//for encoding.
			uint8_t Reserved:1;								//Only '0' allowed
			uint8_t CONT_WAVE:1;							//Enables continuous carrier transmit when high.
		};
		uint8_t value;
	} NRF24L01_RF_SETUP;


	typedef union{
		struct {											//Status Register (In parallel to the SPI command
															//word applied on the MOSI pin, the STATUS register
															//is shifted serially out on the MISO pin)
			uint8_t TX_FULL:1;								//TX FIFO full flag.
															//1: TX FIFO full.
															//0: Available locations in TX FIFO.
			uint8_t RX_P_NO:3;								//Data pipe number for the payload available for
															//reading from RX_FIFO
															//000-101: Data Pipe Number
															//110: Not Used
															//111: RX FIFO Empty
			uint8_t MAX_RT:1;								//Maximum number of TX retransmits interrupt
															//Write 1 to clear bit. If MAX_RT is asserted it must
															//be cleared to enable further communication.
			uint8_t TX_DS:1;								//Data Sent TX FIFO interrupt. Asserted when
															//packet transmitted on TX. If AUTO_ACK is activated,
															//this bit is set high only when ACK is
															//received.
															//Write 1 to clear bit.
			uint8_t RX_DR:1;								//Data Ready RX FIFO interrupt. Asserted when
															//new data arrives RX FIFO.
															//Write 1 to clear bit.
															//The RX_DR IRQ is asserted by a new packet arrival event. The procedure for handling this interrupt should
															//be: 1) read payload through SPI, 2) clear RX_DR IRQ, 3) read FIFO_STATUS to check if there are more
															//payloads available in RX FIFO, 4) if there are more data in RX FIFO, repeat from step 1).
			uint8_t Reserved:1;								//Only '0' allowed
		};
		uint8_t value;
	} NRF24L01_STATUS;

	typedef union{
		struct {											//Transmit observe register
			uint8_t ARC_CNT:1;								//Count retransmitted packets. The counter is reset
															//when transmission of a new packet starts. See
															//page 75.
			uint8_t PLOS_CNT:1;								//Count lost packets. The counter is overflow protected
															//to 15, and discontinues at max until reset.
															//The counter is reset by writing to RF_CH. See
															//page 75.
		};
		uint8_t value;
	} NRF24L01_OBSERVE_TX;

	typedef union{
		struct {											//
			uint8_t RPD:1;									//Received Power Detector. This register is called
															//CD (Carrier Detect) in the nRF24L01. The name is
															//different in nRF24L01+ due to the different input
															//power level threshold for this bit. See section 6.4
															//on page 25.
			uint8_t Reserved:7;								//
		};
		uint8_t value;
	} NRF24L01_RPD;


	typedef union{
		struct {											//Receive address data pipe 0. 5 Bytes maximum
															//length. (LSByte is written first. Write the number of
															//bytes defined by SETUP_AW)
			uint64_t RX_ADDR_P0:40;							//0xE7E7E7E7E7
		};
		uint8_t value[5];
	} NRF24L01_RX_ADDR_P0;

	typedef union{
		struct {											//Receive address data pipe 1. 5 Bytes maximum
															//length. (LSByte is written first. Write the number of
															//bytes defined by SETUP_AW)
			uint64_t RX_ADDR_P1:40;							//0xC2C2C2C2C2
		};
		uint8_t value[5];
	} NRF24L01_RX_ADDR_P1;

	typedef union {
		struct {											//Receive address data pipe 2. Only LSB. MSBytes
															//are equal to RX_ADDR_P1[39:8]
			uint8_t RX_ADDR_P2;								//0xC3
		};
		uint8_t value;
	} NRF24L01_RX_ADDR_P2;

	typedef union{
		struct {											//Receive address data pipe 3. Only LSB. MSBytes
															//are equal to RX_ADDR_P1[39:8]
			uint8_t RX_ADDR_P3;								//0xC4
		};
		uint8_t value;
	} NRF24L01_RX_ADDR_P3;

	typedef union{
		struct {											//Receive address data pipe 4. Only LSB. MSBytes
															//are equal to RX_ADDR_P1[39:8]
			uint8_t RX_ADDR_P4;								//0xC5
		};
		uint8_t value;
	} NRF24L01_RX_ADDR_P4;

	typedef union{
		struct {											//Receive address data pipe 5. Only LSB. MSBytes
															//are equal to RX_ADDR_P1[39:8]
			uint8_t RX_ADDR_P5;								//0xC6
		};
		uint8_t value;
	} NRF24L01_RX_ADDR_P5;

	typedef union{
		struct {											//Transmit address. Used for a PTX device only.
															//(LSByte is written first)
															//Set RX_ADDR_P0 equal to this address to handle
															//automatic acknowledge if this is a PTX device with
															//Enhanced ShockBurst™ enabled. See page 75.
			uint64_t TX_ADDR:40;							//0xE7E7E7E7E7
		};
		uint8_t value[5];
	} NRF24L01_TX_ADDR;

	typedef union{
		struct {											//
			uint8_t RX_PW_P0:6;								//Number of bytes in RX payload in data pipe 0 (1 to
															//32 bytes).
															//0 Pipe not used
															//1 = 1 byte
															//…
															//32 = 32 bytes
			uint8_t Reserved:2;								//Only '00' allowed
		};
		uint8_t value;
	} NRF24L01_RX_PW_P0;

	typedef union{
		struct {											//
			uint8_t RX_PW_P1:6;								//Number of bytes in RX payload in data pipe 1 (1 to
															//32 bytes).
															//0 Pipe not used
															//1 = 1 byte
															//…
															//32 = 32 bytes
			uint8_t Reserved:2;								//Only '00' allowed
		};
		uint8_t value;
	} NRF24L01_RX_PW_P1;

	typedef union{
		struct {											//
		uint8_t RX_PW_P2:6;								//Number of bytes in RX payload in data pipe 2 (1 to
														//32 bytes).
														//0 Pipe not used
														//1 = 1 byte
														//…
														//32 = 32 bytes
		uint8_t Reserved:2;								//Only '00' allowed
		};
		uint8_t value;
	} NRF24L01_RX_PW_P2;

	typedef union{
		struct {											//
		uint8_t RX_PW_P3:6;								//Number of bytes in RX payload in data pipe 3 (1 to
														//32 bytes).
														//0 Pipe not used
														//1 = 1 byte
														//…
														//32 = 32 bytes
		uint8_t Reserved:2;								//Only '00' allowed
		};
		uint8_t value;
	} NRF24L01_RX_PW_P3;

	typedef union{
		struct {											//
			uint8_t RX_PW_P4:6;								//Number of bytes in RX payload in data pipe 4 (1 to
															//32 bytes).
															//0 Pipe not used
															//1 = 1 byte
															//…
															//32 = 32 bytes
			uint8_t Reserved:2;								//Only '00' allowed
		};
		uint8_t value;
	} NRF24L01_RX_PW_P4;

	typedef union {
		struct {											//
			uint8_t RX_PW_P5:6;								//Number of bytes in RX payload in data pipe 5 (1 to
															//32 bytes).
															//0 Pipe not used
															//1 = 1 byte
															//…
															//32 = 32 bytes
			uint8_t Reserved:2;								//Only '00' allowed
		};
		uint8_t value;
	} NRF24L01_RX_PW_P5;

	typedef union{
		struct {											//
			uint8_t RX_EMPTY:1;								//RX FIFO empty flag.
															//1: RX FIFO empty.
															//0: Data in RX FIFO.
			uint8_t RX_FULL:1;								//RX FIFO full flag.
															//1: RX FIFO full.
															//0: Available locations in RX FIFO.
			uint8_t Reserved1:2;							//Only '00' allowed
			uint8_t TX_EMPTY:1;								//TX FIFO empty flag.
															//1: TX FIFO empty.
															//0: Data in TX FIFO.
			uint8_t TX_FULL:1;								//TX FIFO full flag. 1: TX FIFO full. 0: Available locations
															//in TX FIFO.
			uint8_t TX_REUSE:1;								//Used for a PTX device
															//Pulse the rfce high for at least 10μs to Reuse last
															//transmitted payload. TX payload reuse is active
															//until W_TX_PAYLOAD or FLUSH TX is executed.
															//TX_REUSE is set by the SPI command
															//REUSE_TX_PL, and is reset by the SPI commands
															//W_TX_PAYLOAD or FLUSH TX
			uint8_t Reserved:1;								//Only '0' allowed
		};
		uint8_t value;
	} NRF24L01_FIFO_STATUS;

	typedef union{
		struct {											//Enable dynamic payload length
			uint8_t DPL_P0:1;								//Enable dynamic payload length data pipe 0.
															//(Requires EN_DPL and ENAA_P0)
			uint8_t DPL_P1:1;								//Enable dynamic payload length data pipe 1.
															//(Requires EN_DPL and ENAA_P1)
			uint8_t DPL_P2:1;								//Enable dynamic payload length data pipe 2.
															//(Requires EN_DPL and ENAA_P2)
			uint8_t DPL_P3:1;								//Enable dynamic payload length data pipe 3.
															//(Requires EN_DPL and ENAA_P3)
			uint8_t DPL_P4:1;								//Enable dynamic payload length data pipe 4.
															//(Requires EN_DPL and ENAA_P4)
			uint8_t DPL_P5:1;								//Enable dynamic payload length data pipe 5.
															//(Requires EN_DPL and ENAA_P5)
			uint8_t Reserved:2;								//Only ‘00’ allowed
		};
		uint8_t value;
	} NRF24L01_DYNPD;

	typedef union{
		struct {											//Feature Register
			uint8_t EN_DYN_ACK:1;							//Enables the W_TX_PAYLOAD_NOACK command
			uint8_t EN_ACK_PAY:1;							//Enables Payload with ACK
															//If ACK packet payload is activated, ACK packets have dynamic payload lengths and the Dynamic Payload
															//Length feature should be enabled for pipe 0 on the PTX and PRX. This is to ensure that they receive the
															//ACK packets with payloads. If the ACK payload is more than 15 byte in 2Mbps mode the ARD must be
															//500μS or more, and if the ACK payload is more than 5 byte in 1Mbps mode the ARD must be 500μS or
															//more. In 250kbps mode (even when the payload is not in ACK) the ARD must be 500μS or more.
			uint8_t EN_DPL:1;								//Enables Dynamic Payload Length
			uint8_t Reserved:5;								//Only ‘00000’ allowed
		};
		uint8_t value;
	} NRF24L01_FEATURE;

	typedef struct{
		uint8_t CONFIG;				//0x00		//Configuration Register
		uint8_t EN_AA;				//0x01		//Enable ‘Auto Acknowledgment’ Function Disable
		uint8_t EN_RXADDR;			//0x02		//Enabled RX Addresses
		uint8_t SETUP_AW;			//0x03		//Setup of Address Widths (common for all data pipes)
		uint8_t SETUP_RETR;			//0x04		//Setup of Automatic Retransmission
		uint8_t RF_CH;				//0x05		//RF Channel
		uint8_t RF_SETUP;			//0x06		//RF Setup Register
		uint8_t STATUS;				//0x07		//Status Register (In parallel to the SPI command word applied on the MOSI pin, the STATUS register is shifted serially out on the MISO pin)
		uint8_t OBSERVE_TX;			//0x08		//Transmit observe register
		uint8_t RPD;				//0x09		//Received Power Detector
		uint8_t RX_ADDR_P0;			//0x0A		//Receive address data pipe 0. 5 Bytes maximum length. (LSByte is written first. Write the number of bytes defined by SETUP_AW)
		uint8_t RX_ADDR_P1;			//0x0B		//Receive address data pipe 1.
		uint8_t RX_ADDR_P2;			//0x0C		//Receive address data pipe 2. Only LSB. MSBytes are equal to RX_ADDR_P1[39:8]
		uint8_t RX_ADDR_P3;			//0x0D		//Receive address data pipe 3. Only LSB.
		uint8_t RX_ADDR_P4;			//0x0E		//Receive address data pipe 4. Only LSB.
		uint8_t RX_ADDR_P5;			//0x0F		//Receive address data pipe 5. Only LSB.
		uint8_t TX_ADDR;			//0x10		//Transmit address. Used for a PTX device only. (LSByte is written first)
		uint8_t RX_PW_P0;			//0x11		//Number of bytes in RX payload in data pipe 0 (1 to 32 bytes).
		uint8_t RX_PW_P1;			//0x12		//Number of bytes in RX payload in data pipe 1 (1 to 32 bytes).
		uint8_t RX_PW_P2;			//0x13		//Number of bytes in RX payload in data pipe 2 (1 to 32 bytes).
		uint8_t RX_PW_P3;			//0x14		//Number of bytes in RX payload in data pipe 3 (1 to 32 bytes).
		uint8_t RX_PW_P4;			//0x15		//Number of bytes in RX payload in data pipe 4 (1 to 32 bytes).
		uint8_t RX_PW_P5;			//0x16		//Number of bytes in RX payload in data pipe 5 (1 to 32 bytes).
		uint8_t FIFO_STATUS;		//0x17		//FIFO Status Register
		uint8_t DYNPD;				//0x1C		//Enable dynamic payload length
		uint8_t FEATURE;			//0x1D		//Feature Register

	} REG_ADDRS;
	static const REG_ADDRS REG_ADDR;

	typedef struct{
		NRF24L01_CONFIG 			CONFIG;					//0x00		//Configuration Register
		NRF24L01_EN_AA 				EN_AA;					//0x01		//Enable ‘Auto Acknowledgment’ Function Disable
		NRF24L01_EN_RXADDR 			EN_RXADDR;				//0x02		//Enabled RX Addresses
		NRF24L01_SETUP_AW			SETUP_AW;				//0x03		//Setup of Address Widths (common for all data pipes)
		NRF24L01_SETUP_RETR			SETUP_RETR;				//0x04		//Setup of Automatic Retransmission
		NRF24L01_RF_CH				RF_CH;					//0x05		//RF Channel
		NRF24L01_RF_SETUP			RF_SETUP;				//0x06		//RF Setup Register
		NRF24L01_STATUS				STATUS;					//0x07		//Status Register (In parallel to the SPI command word applied on the MOSI pin, the STATUS register is shifted serially out on the MISO pin)
		NRF24L01_OBSERVE_TX			OBSERVE_TX;				//0x08		//Transmit observe register
		NRF24L01_RPD				RPD;					//0x09		//Received Power Detector
		NRF24L01_RX_ADDR_P0			RX_ADDR_P0;				//0x0A		//Receive address data pipe 0. 5 Bytes maximum length. (LSByte is written first. Write the number of bytes defined by SETUP_AW)
		NRF24L01_RX_ADDR_P1			RX_ADDR_P1;				//0x0B		//Receive address data pipe 1.
		NRF24L01_RX_ADDR_P2			RX_ADDR_P2;				//0x0C		//Receive address data pipe 2. Only LSB. MSBytes are equal to RX_ADDR_P1[39:8]
		NRF24L01_RX_ADDR_P3			RX_ADDR_P3;				//0x0D		//Receive address data pipe 3. Only LSB.
		NRF24L01_RX_ADDR_P4			RX_ADDR_P4;				//0x0E		//Receive address data pipe 4. Only LSB.
		NRF24L01_RX_ADDR_P5			RX_ADDR_P5;				//0x0F		//Receive address data pipe 5. Only LSB.
		NRF24L01_TX_ADDR			TX_ADDR;				//0x10		//Transmit address. Used for a PTX device only. (LSByte is written first)
		NRF24L01_RX_PW_P0			RX_PW_P0;				//0x11		//Number of bytes in RX payload in data pipe 0 (1 to 32 bytes).
		NRF24L01_RX_PW_P1			RX_PW_P1;				//0x12		//Number of bytes in RX payload in data pipe 1 (1 to 32 bytes).
		NRF24L01_RX_PW_P2			RX_PW_P2;				//0x13		//Number of bytes in RX payload in data pipe 2 (1 to 32 bytes).
		NRF24L01_RX_PW_P3			RX_PW_P3;				//0x14		//Number of bytes in RX payload in data pipe 3 (1 to 32 bytes).
		NRF24L01_RX_PW_P4			RX_PW_P4;				//0x15		//Number of bytes in RX payload in data pipe 4 (1 to 32 bytes).
		NRF24L01_RX_PW_P5			RX_PW_P5;				//0x16		//Number of bytes in RX payload in data pipe 5 (1 to 32 bytes).
		NRF24L01_FIFO_STATUS		FIFO_STATUS;			//0x17		//FIFO Status Register
		NRF24L01_DYNPD				DYNPD;					//0x1C		//Enable dynamic payload length
		NRF24L01_FEATURE			FEATURE;				//0x1D		//Feature Register
	} NRF24_REG;

	NRF24_REG REG;
	static const NRF24_REG REG_DEFAULT;

};
