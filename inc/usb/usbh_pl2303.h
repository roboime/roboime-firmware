#pragma once

#include "inttypes.h"
#include "usbh_core.h"

#define SET_LINE_REQUEST_TYPE           0x21
#define SET_LINE_REQUEST                0x20

#define SET_CONTROL_REQUEST_TYPE        0x21
#define SET_CONTROL_REQUEST             0x22
#define CONTROL_DTR                     0x01
#define CONTROL_RTS                     0x02

#define BREAK_REQUEST_TYPE              0x21
#define BREAK_REQUEST                   0x23
#define BREAK_ON                        0xffff
#define BREAK_OFF                       0x0000

#define GET_LINE_REQUEST_TYPE           0xa1
#define GET_LINE_REQUEST                0x21

#define VENDOR_WRITE_REQUEST_TYPE       0x40
#define VENDOR_WRITE_REQUEST            0x01

#define VENDOR_READ_REQUEST_TYPE        0xc0
#define VENDOR_READ_REQUEST             0x01

#define UART_STATE                      0x08
#define UART_STATE_TRANSIENT_MASK       0x74
#define UART_DCD                        0x01
#define UART_DSR                        0x02
#define UART_BREAK_ERROR                0x04
#define UART_RING                       0x08
#define UART_FRAME_ERROR                0x10
#define UART_PARITY_ERROR               0x20
#define UART_OVERRUN_ERROR              0x40
#define UART_CTS                        0x80

#define USBH_PL2303_MPS_SIZE            0x40

typedef struct _PL2303_State{
	uint8_t	PL2303State;
} USBH_PL2303_State_TypeDef;

typedef struct{
	uint8_t              hc_num_in;
	uint8_t              hc_num_out;
	uint8_t              hc_num_int_in;

	uint8_t              MSBulkOutEp;
	uint16_t             MSBulkOutEpSize;
	uint8_t              MSBulkInEp;
	uint16_t             MSBulkInEpSize;
	uint8_t              MSBulkIntInEp;
	uint16_t             MSBulkIntInEpSize;
	uint8_t              buff[USBH_PL2303_MPS_SIZE];
	uint8_t              maxLun;
} PL2303_Machine_TypeDef;

typedef enum {
	USBH_PL2303_OK = 0,
	USBH_PL2303_FAIL = 1,
	USBH_PL2303_PHASE_ERROR = 2,
	USBH_PL2303_BUSY = 3
} USBH_PL2303_Status_TypeDef;

enum pl2303_type {
	type_0,         /* don't know the difference between type 0 and */
	type_1,         /* type 1, until someone from prolific tells us... */
	HX,             /* HX version of the pl2303 chip */
};

struct pl2303_serial_private {
	enum pl2303_type type;
};

struct pl2303_private {
	uint8_t line_control;
	uint8_t line_status;
};

typedef enum{
  USBH_PL2303_INIT_STATE = 0,
  USBH_PL2303_INIT_STATEA,
  USBH_PL2303_INIT_STATEB,
  USBH_PL2303_INIT_STATEC,
  USBH_PL2303_INIT_STATED,
  USBH_PL2303_INIT_STATE0,
  USBH_PL2303_INIT_STATE1,
  USBH_PL2303_INIT_STATE2,
  USBH_PL2303_INIT_STATE3,
  USBH_PL2303_INIT_STATE4,
  USBH_PL2303_INIT_STATE5,
  USBH_PL2303_INIT_STATE6,
  USBH_PL2303_INIT_STATE7,
  USBH_PL2303_INIT_STATE8,
  USBH_PL2303_INIT_STATE9,
  USBH_PL2303_INIT_STATE10,
  USBH_PL2303_INIT_STATE11,
  USBH_PL2303_INIT_OK,

  USBH_PL2303_ERROR,
} PL2303_State;

#define PL2303_STOPBITS_1		0
#define PL2303_STOPBITS_1_5		1
#define PL2303_STOPBITS_2		2

#define PL2303_PARITY_NONE		0
#define PL2303_PARITY_ODD		1
#define PL2303_PARITY_EVEN		2
#define PL2303_PARITY_MARK		3
#define PL2303_PARITY_SPACE		4

extern USBH_Class_cb_TypeDef  USBH_PL2303_cb;

#ifdef __cplusplus
extern "C" {
#endif

uint16_t USBH_PL2303_SendData(uint8_t *buffer, uint16_t len);
uint8_t USBH_PL2303_SendAvailable();
uint16_t USBH_PL2303_GetData(uint8_t *buffer, uint16_t len);
uint16_t USBH_PL2303_GetDataCount();

#ifdef __cplusplus
}
#endif
