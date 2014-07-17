#include "rfm12.h"
#include "stm32f4xx_gpio.h"

#include "stm32f4xx_conf.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_exti.h"
#include "misc.h"


#define PWRMGT_DEFAULT (RFM12_PWRMGT_EX)
#define FREQ 433432500UL
#define DATARATE_VALUE RFM12_DATARATE_CALC_HIGH(20000.0)
//default fiforeset is as follows:
//2 Byte Sync Pattern,
//disable sensitive reset, Fifo filled interrupt at 8 bits
//default fiforeset register value to accept data
#define ACCEPT_DATA (RFM12_CMD_FIFORESET | RFM12_FIFORESET_DR | (8<<4) | RFM12_FIFORESET_FF)

//default fiforeset register value to clear fifo
#define CLEAR_FIFO (RFM12_CMD_FIFORESET | RFM12_FIFORESET_DR | (8<<4))

#define MODE_RX 0
#define MODE_TX 1
u8 rfm12_mode;

void rfm12_init() {


	RCC_APB1PeriphClockCmd(RFM12_RCC_SPIPeriph, ENABLE);
	RCC_AHB1PeriphClockCmd(RFM12_RCC_PORTPeriph, ENABLE);
	RCC_AHB1PeriphClockCmd(RFM12_NIRQ_RCC_PORTPeriph, ENABLE);


	GPIO_InitTypeDef GPIO_InitStructure;

	//MOSI
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;

	GPIO_InitStructure.GPIO_Pin = RFM12_MOSI_PIN;
	GPIO_Init(RFM12_PORT, &GPIO_InitStructure);

	//MISO
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = RFM12_MISO_PIN;
	GPIO_Init(RFM12_PORT, &GPIO_InitStructure);

	//SCK
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = RFM12_SCK_PIN;
	GPIO_Init(RFM12_PORT, &GPIO_InitStructure);

	//SS
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = RFM12_SS_PIN;
	GPIO_Init(RFM12_SS_PORT, &GPIO_InitStructure);
	GPIO_SetBits(RFM12_SS_PORT, RFM12_SS_PIN);

	GPIO_PinAFConfig(RFM12_PORT, RFM12_SCK_PIN_AF, RFM12_SPI_AF);
	GPIO_PinAFConfig(RFM12_PORT, RFM12_MISO_PIN_AF, RFM12_SPI_AF);
	GPIO_PinAFConfig(RFM12_PORT, RFM12_MOSI_PIN_AF, RFM12_SPI_AF);


	//FFS
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStructure.GPIO_Pin = RFM12_FFS_PIN;
//	GPIO_Init(RFM12_PORT, &GPIO_InitStructure);
//	GPIO_SetBits(RFM12_PORT, RFM12_FFS_PIN);

	//NIRQ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = RFM12_NIRQ_PIN;
	GPIO_Init(RFM12_NIRQ_PORT, &GPIO_InitStructure);

	//SPI
	SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_CRCPolynomial = 0;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
	SPI_Init(RFM12_SPI, &SPI_InitStructure);
	SPI_CalculateCRC(RFM12_SPI, DISABLE);
	SPI_Cmd(RFM12_SPI, ENABLE);

	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Configure EXTI Line3 to generate an interrupt on rising or falling edge */
	EXTI_InitStructure.EXTI_Line = EXTI_Line3;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//	EXTI_Init(&EXTI_InitStructure);

	/* Configure and enable EXTI3 interrupt */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);

	rfm12_config();
}

void rfm12_ss_assert() {
	GPIO_ResetBits(RFM12_SS_PORT, RFM12_SS_PIN);
}

void rfm12_ss_release() {
	GPIO_SetBits(RFM12_SS_PORT, RFM12_SS_PIN);
}

u16 rfm12_cmd(u16 cmd) {
	u16 resp;
	rfm12_ss_assert();
	SPI_I2S_SendData(RFM12_SPI, (cmd >> 8) & 0xff);
	while (SPI_I2S_GetFlagStatus(RFM12_SPI, SPI_I2S_FLAG_BSY) == SET);
	while (SPI_I2S_GetFlagStatus(RFM12_SPI, SPI_I2S_FLAG_RXNE) == RESET);
	resp = SPI_I2S_ReceiveData(RFM12_SPI) << 8;
	SPI_I2S_SendData(RFM12_SPI, cmd & 0xff);
	while (SPI_I2S_GetFlagStatus(RFM12_SPI, SPI_I2S_FLAG_BSY) == SET);
	while (SPI_I2S_GetFlagStatus(RFM12_SPI, SPI_I2S_FLAG_RXNE) == RESET);
	resp |= SPI_I2S_ReceiveData(RFM12_SPI);
//	while (SPI_I2S_GetFlagStatus(RFM12_SPI, SPI_I2S_FLAG_BSY) == SET);//necessário para dar um delay antes do release do SS sem isso não funciona
	u8 i;
	for(i = 0; i < 100; i++);
	rfm12_ss_release();
	return resp;
}

u16 rfm12_send(u8 c) {
	while (GPIO_ReadInputDataBit(RFM12_NIRQ_PORT, RFM12_NIRQ_PIN) == SET);
	return rfm12_cmd(RFM12_CMD_TX | c);
}

void rfm12_config() {
	rfm12_cmd(RFM12_CMD_LBDMCD | 0x40);//clock out divider and low battery detector
	rfm12_cmd(RFM12_CMD_CFG | RFM12_CFG_EL | RFM12_CFG_EF | RFM12_BAND_433| RFM12_XTAL_12PF);//enable internal data register and fifo
	rfm12_cmd(RFM12_CMD_DATAFILTER | RFM12_DATAFILTER_AL | 5);//automatic clock lock control(AL), digital Filter(!S), Data quality detector value 3
	rfm12_cmd(RFM12_CMD_FIFORESET | RFM12_FIFORESET_DR | (8 << 4));//2 Byte Sync Pattern, Start fifo fill when sychron pattern received, disable sensitive reset, Fifo filled interrupt at 8 bits
	rfm12_cmd(RFM12_CMD_WAKEUP);//disable wake up timer
	rfm12_cmd(RFM12_CMD_DUTYCYCLE);//disable low duty cycle
	rfm12_cmd(RFM12_CMD_AFC | RFM12_AFC_AUTO_KEEP | RFM12_AFC_LIMIT_4| RFM12_AFC_FI | RFM12_AFC_OE | RFM12_AFC_EN);//set AFC to automatic, (+4 or -3)*2.5kHz Limit, fine mode, active and enabled
	rfm12_cmd(RFM12_CMD_PWRMGT | PWRMGT_DEFAULT | RFM12_PWRMGT_ER| RFM12_PWRMGT_EBB);//enable receiver
	rfm12_cmd(RFM12_CMD_STATUS);
	rfm12_cmd(RFM12_CMD_FREQUENCY | (RFM12_FREQUENCY_CALC_433(FREQ)));
	rfm12_cmd(RFM12_CMD_TXCONF | RFM12_TXCONF_POWER_0| RFM12_TXCONF_FS_CALC(90000));//set TX Power to -0dB, frequency shift = +-90kHz
	rfm12_cmd(RFM12_CMD_RXCTRL | RFM12_RXCTRL_P16_VDI | RFM12_RXCTRL_VDI_MEDIUM	| RFM12_RXCTRL_BW_200 | RFM12_RXCTRL_LNA_6 | RFM12_RXCTRL_RSSI_103);
	//aumentar ganho LNA
	rfm12_cmd(RFM12_CMD_SYNCPATTERN | 0xD4);//SYNC=2DD4;
	rfm12_cmd(RFM12_CMD_DATARATE | DATARATE_VALUE);
	rfm12_cmd(CLEAR_FIFO);
	rfm12_cmd(ACCEPT_DATA);
}

u8 rfm12_receive() {
	u16 c;
//	while (GPIO_ReadInputDataBit(RFM12_NIRQ_PORT, RFM12_NIRQ_PIN) == SET);
	rfm12_cmd(RFM12_CMD_STATUS);
	c = rfm12_cmd(RFM12_CMD_READ);
	return c & 0xff;
}

void rfm12_receiver() {
	u8 i, c;
	rfm12_cmd(RFM12_CMD_PWRMGT | PWRMGT_DEFAULT | RFM12_PWRMGT_ER | RFM12_PWRMGT_EBB); //turn off the transmitter and enable receiver
	rfm12_cmd(RFM12_CMD_TX | 0xAA);//load a dummy byte to clear int status.
	rfm12_cmd(CLEAR_FIFO);
	while (1) {
		char b[16] = {0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5};
		rfm12_cmd(ACCEPT_DATA);
		c = rfm12_cmd(RFM12_CMD_STATUS);
		for (i = 0; i < 16; i++) {
			b[i] = rfm12_receive();
		}
		rfm12_cmd(CLEAR_FIFO);
	}
}

void rfm12_receiver_mode() {
	rfm12_cmd(RFM12_CMD_PWRMGT | PWRMGT_DEFAULT | RFM12_PWRMGT_ER | RFM12_PWRMGT_EBB); //turn off the transmitter and enable receiver
	rfm12_cmd(RFM12_CMD_TX | 0xAA);//load a dummy byte to clear int status.
	rfm12_cmd(CLEAR_FIFO);

	rfm12_cmd(ACCEPT_DATA);
	rfm12_cmd(RFM12_CMD_STATUS);
}

void rfm12_accept_data(){
	rfm12_cmd(CLEAR_FIFO);
	rfm12_cmd(ACCEPT_DATA);
	rfm12_cmd(RFM12_CMD_STATUS);
}

void rfm12_sendbuffer(char *buf, u8 tam) {
	u16 c;
	c = rfm12_cmd(RFM12_CMD_STATUS);//read status register
	c = rfm12_cmd(RFM12_CMD_PWRMGT | PWRMGT_DEFAULT | RFM12_PWRMGT_ET	| RFM12_PWRMGT_ES);//set ET in power register to enable transmission
	c = rfm12_send(0xAA);//PREAMBLE
	c = rfm12_send(0xAA);//PREAMBLE
	c = rfm12_send(0xAA);//PREAMBLE
	c = rfm12_send(0x2D);//SYNC HI BYTE
	c = rfm12_send(0xD4);//SYNC LOW BYTE
	while (tam--) {
		c = rfm12_send(*buf++);//DATA BYTE 0 ...
	}
	c = rfm12_send(0xBB);//DUMMY BYTE
	c = rfm12_send(0xBB);//DUMMY BYTE
	c = rfm12_send(0xBB);//DUMMY BYTE
	c = rfm12_cmd(RFM12_CMD_PWRMGT | PWRMGT_DEFAULT | RFM12_PWRMGT_ER	| RFM12_PWRMGT_EBB); //turn off the transmitter and enable receiver
	c = rfm12_cmd(CLEAR_FIFO);
	c = rfm12_cmd(RFM12_CMD_STATUS);//read status register
}

u8 rfm12_data_available(){
	if(GPIO_ReadInputDataBit(RFM12_NIRQ_PORT, RFM12_NIRQ_PIN)==RESET){
		return 1;
	} else {
		return 0;
	}
}

void EXTI3_IRQHandler(void) {
	/* If RFM12 has a data */
	if (EXTI_GetITStatus(EXTI_Line3) != RESET) {
		if (rfm12_mode == MODE_RX) {
			static u8 checksum;
			u8 data = rfm12_receive();
			checksum = data;
			//len byte + type byte + source byte + dest byte + checksum
			//buffer_rx.num_bytes = checksum + 5;
			//TODO: falta continuar
		}
		EXTI_ClearITPendingBit(EXTI_Line3);
	}
}

const u16 rssi_level[]={
		RFM12_RSSI_LNA_TH_73_NEG,
		RFM12_RSSI_LNA_TH_79_NEG,
		RFM12_RSSI_LNA_TH_85_NEG,
		RFM12_RSSI_LNA_TH_87_NEG,
		RFM12_RSSI_LNA_TH_91_NEG,
		RFM12_RSSI_LNA_TH_93_NEG,
		RFM12_RSSI_LNA_TH_97_NEG,
		RFM12_RSSI_LNA_TH_99_NEG,
		RFM12_RSSI_LNA_TH_103_NEG,
		RFM12_RSSI_LNA_TH_105_NEG,
		RFM12_RSSI_LNA_TH_109_NEG,
		RFM12_RSSI_LNA_TH_111_NEG,
		RFM12_RSSI_LNA_TH_117_NEG,
		RFM12_RSSI_LNA_TH_123_NEG,
};

u8 rfm12_spectrum_analyzer(u16 freq){
	u8 rssi_l=0;
	u16 c;
	int i;
	rfm12_cmd(RFM12_CMD_FREQUENCY | freq);
	rfm12_receiver_mode();
	for(rssi_l=0; rssi_l<14;rssi_l++){
		c = rfm12_cmd(RFM12_CMD_RXCTRL | RFM12_RECEIVER_BASEBAND_BANDWIDTH_64_KHZ  | RFM12_VDI_RESPONSE_MEDIUM | RFM12_P16_FUNC_VDI_OUTPUT| rssi_level[rssi_l]);
		for(i=20000;i;i--){

		}
		c = rfm12_cmd(RFM12_CMD_STATUS);//read status register
		if(c & RFM12_STATUS_RSSI){
			break;
		} else {
			continue;
		}
	}
	return rssi_l;
}


