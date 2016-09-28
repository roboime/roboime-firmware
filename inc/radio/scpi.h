#pragma once

#include <utils/circularbuffer.h>
#include <scpi/scpi.h>
#include <stdio.h>
#include <string.h>

class SCPI{
public:
	SCPI(const char *idn[4]);
	int Parse(char * data, size_t len);
	int In(CircularBuffer<uint8_t> &in);
	int In(char * data, size_t len);

	int Out(char * data, size_t len);

protected:
	CircularBuffer<char> _bufferout;
	static const scpi_command_t scpi_commands_default[];

	static size_t _SCPI_Write(scpi_t * context, const char * data, size_t len);
	static int _SCPI_Error(scpi_t * context, int_fast16_t err);
	static scpi_result_t _SCPI_Control(scpi_t * context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val);
	static scpi_result_t _SCPI_Reset(scpi_t * context);
	static scpi_result_t _SCPI_Test(scpi_t * context);
	static scpi_result_t _SCPI_Flush(scpi_t * context);
	static const scpi_interface_t scpi_interface;

	virtual size_t SCPI_Write(scpi_t * context, const char * data, size_t len);
	virtual int SCPI_Error(scpi_t * context, int_fast16_t err);
	virtual scpi_result_t SCPI_Control(scpi_t * context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val);
	virtual scpi_result_t SCPI_Reset(scpi_t * context);
	virtual scpi_result_t SCPI_Test(scpi_t * context);
	virtual scpi_result_t SCPI_Flush(scpi_t * context);

	char scpi_input_buffer[1024];
	scpi_reg_val_t scpi_regs[SCPI_REG_COUNT];
	scpi_t _scpi_context;

};
