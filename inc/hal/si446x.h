#pragma once

#include <inttypes.h>
#include <hal/modem.h>
#include <hal/spi.h>
#include <hal/io_pin.h>
#include <utils/circularbuffer.h>
#include <initializer_list>
#include <radio/rf_power_ctrl.h>

class SI446X: public MODEM{
public:
	SI446X(SPI &spi, IO_Pin &VCC_PIN, IO_Pin &SDN_PIN,  IO_Pin &NIRQ_PIN, RF_POWER_CTRL &rf_power_ctrl);
	void InterruptCallback();
	void Init();
	void Config();
	uint8_t SelfTest();
	uint8_t TxReady();
	void TxPackage(uint8_t *data, uint16_t size, uint32_t freqHz, MODEM_POWER power);
	uint16_t RxSize();
	uint16_t RxData(uint8_t *data, uint16_t maxsize);
	void GeneratePoll();
	void SetRXFrequency(uint32_t freq);
	void VCC(uint8_t state);
	void Shutdown(uint8_t state);
	void StartRX(uint16_t size, uint32_t freqHz);
	uint8_t Busy();
	void ContinueRX(uint16_t size);
	void CW(uint8_t state);
	uint8_t GetRSSI();
	void Calibrate(uint32_t freq);
	void SetFrequency(uint32_t freq);
	uint32_t GetFrequency();
	void Recovery();
protected:
	typedef enum{
		ModeInit,
		ModeRX,
		ModeTX,
		ModeReady,
	} SI446X_Mode;
	typedef union {
		uint32_t flag;
		struct {
			uint8_t INIT_STATUS :1;
			uint8_t TRANSMITTING:1;
			uint8_t RECEIVING:1;

		};
	} FLAGS;
	typedef union {
		uint8_t buffer[8];
		struct {
			uint8_t MODEM_PEND;
			uint8_t MODEM_STATUS;
			uint8_t CURR_RSSI;
			uint8_t LATCH_RSSI;
			uint8_t ANT1_RSSI;
			uint8_t ANT2_RSSI;
			uint8_t AFC_FREQ_OFFSET;
			uint8_t AFC_FREQ_OFFSET1;
		};
	} MODEM_STATUS;

	typedef union {
		uint8_t buffer[8];
		struct {
			uint8_t PH_INT_STATUS_PEND :1;
			uint8_t MODEM_INT_STATUS_PEND :1;
			uint8_t CHIP_INT_STATUS_PEND :1;
			uint8_t INT_PEND_XXXXX :5;

			uint8_t PH_INT_STATUS :1;
			uint8_t MODEM_INT_STATUS :1;
			uint8_t CHIP_INT_STATUS :1;
			uint8_t INT_STATUS_XXXXX :5;

			uint8_t RX_FIFO_ALMOST_FULL_PEND :1;
			uint8_t TX_FIFO_ALMOST_EMPTY_PEND :1;
			uint8_t PH_PEND_X :1;
			uint8_t CRC_ERROR_PEND :1;
			uint8_t PACKET_RX_PEND :1;
			uint8_t PACKET_SENT_PEND :1;
			uint8_t FILTER_MISS_PEND :1;
			uint8_t FILTER_MATCH_PEND :1;

			uint8_t RX_FIFO_ALMOST_FULL :1;
			uint8_t TX_FIFO_ALMOST_EMPTY :1;
			uint8_t PH_STATUS_X :1;
			uint8_t CRC_ERROR :1;
			uint8_t PACKET_RX :1;
			uint8_t PACKET_SENT :1;
			uint8_t FILTER_MISS :1;
			uint8_t FILTER_MATCH :1;

			uint8_t SYNC_DETECT_PEND :1;
			uint8_t PREAMBLE_DETECT_PEND :1;
			uint8_t INVALID_PREAMBLE_PEND :1;
			uint8_t RSSI_PEND :1;
			uint8_t RSSI_JUMP_PEND :1;
			uint8_t INVALID_SYNC_PEND :1;
			uint8_t MODEM_PEND_XX :2;

			uint8_t SYNC_DETECT  :1;
			uint8_t PREAMBLE_DETECT :1;
			uint8_t INVALID_PREAMBLE :1;
			uint8_t RSSI :1;
			uint8_t RSSI_JUMP :1;
			uint8_t INVALID_SYNC :1;
			uint8_t MODEM_STATUS_XX :2;

			uint8_t WUT_PEND :1;
			uint8_t LOW_BATT_PEND :1;
			uint8_t CHIP_READY_PEND :1;
			uint8_t CMD_ERROR_PEND :1;
			uint8_t STATE_CHANGE_PEND :1;
			uint8_t FIFO_UNDERFLOW_OVERFLOW_ERROR_PEND :1;
			uint8_t CAL_PEND :2;

			uint8_t WUT :1;
			uint8_t LOW_BATT :1;
			uint8_t CHIP_READY :1;
			uint8_t CMD_ERROR :1;
			uint8_t STATE_CHANGE :1;
			uint8_t FIFO_UNDERFLOW_OVERFLOW_ERROR :1;
			uint8_t CAL :2;
		};
	} INTERRUPT_STATUS;

	void SetMode(SI446X_Mode mode, uint16_t size, uint32_t freqHz, MODEM_POWER power=MODEM_POWER_NORMAL);
	uint32_t read_rx_fifo(uint8_t *data, uint16_t size);
	int8_t read_response(uint8_t *buffu, uint16_t size, uint8_t retries);
	uint32_t write(uint8_t *data, uint16_t size);
	uint8_t cmd(uint8_t cmd);
	uint16_t timeout();
	void setfrequency(uint32_t freq);
	uint32_t getfrequency();
	void settxpower(MODEM_POWER power);
	uint8_t get_RX_FIFO_COUNT();
	uint8_t get_TX_FIFO_SPACE();
	void clear_TX_FIFO();
	void clear_RX_FIFO();
	void clear_RX_TX_FIFO();
	void cmd_START_TX(uint8_t channel=0, uint8_t condition=0, uint16_t tx_len=0);
	void cmd_START_RX(uint8_t channel=0, uint8_t condition=0, uint16_t rx_len=2048, uint8_t rx_timeout_state=0, uint8_t rx_valid_state=0, uint8_t rx_invalid_state=0);
	void cmd_GET_INT_STATUS(INTERRUPT_STATUS &intstatus);
	void cmd_SET_PKT_RX_THRESHOLD(uint8_t size);
	uint8_t cmd(std::initializer_list<uint8_t> c);
	void ircal(uint32_t freq);


	SPI *_spi;
	IO_Pin *_VCC_PIN;
	IO_Pin *_SDN_PIN;
	IO_Pin *_NIRQ_PIN;
	CircularBuffer<uint8_t,false> _rxbuffer;
	CircularBuffer<uint8_t,false> _txbuffer;
	uint8_t _rxfifobuffer[64];
	uint8_t _txfifobuffer[65];
	uint32_t _frequency;
	FLAGS _flags;
	static const uint8_t SI4464_CFG_256KBPS[];
	SI446X_Mode _mode;
	uint16_t _receivesize;
	uint16_t _receivecount;
	uint16_t _receivesizegoing;
	uint32_t _rxfrequency;
	uint32_t _txfrequency;
	RF_POWER_CTRL &_rf_power_ctrl;
	uint8_t _rssi;

};

#define SI446X_CMD_POWER_UP 		0x02
#define SI446X_SET_PROPERTY 		0x11
#define SI446X_GET_PROPERTY 		0x12
#define SI446X_GPIO_PIN_CFG 		0x13
#define SI446X_CMD_START_TX 		0x31
#define SI446X_CMD_CHANGE_STATE		0x34


