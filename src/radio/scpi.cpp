#include <radio/scpi.h>
#include <radio/bsp.h>

static uint8_t SCPI_calstate=1;
static uint8_t SCPI_output=0;

const scpi_interface_t SCPI::scpi_interface {
	/* error */ _SCPI_Error,
	/* write */ _SCPI_Write,
	/* control */ _SCPI_Control,
	/* flush */ _SCPI_Flush,
	/* reset */ _SCPI_Reset,
	/* test */ _SCPI_Test,
};

scpi_result_t SCPI_Freq(scpi_t * context) {
    (void) context;
    uint32_t freq=0;
    if(SCPI_ParamInt(context, (int32_t*)&freq, 1)){
    	modem.SetFrequency(freq);
    	if(SCPI_output){
    		modem.CW(0);
    		modem.CW(1);
    	}

        return SCPI_RES_OK;
    }
    return SCPI_RES_ERR;
}

scpi_result_t SCPI_FreqQ(scpi_t * context) {
    (void) context;
    uint32_t freq=modem.GetFrequency();
    SCPI_ResultInt(context, freq);
    return SCPI_RES_OK;
}

scpi_result_t SCPI_Power(scpi_t * context) {
    (void) context;
    double power=0;
    SCPI_ParamDouble(context, &power, 1);
    if(SCPI_calstate){
    	if(power<24.0) power=24.0;
    	if(power>35.0) power=35.0;
		rf_power_ctrl.SetPower(modem.GetFrequency(), power);
    } else {
        uint8_t trimpot=(uint8_t)power;
        if(trimpot<40) trimpot=40;
    	mcp4561_b.SetValue(trimpot);
    }
    return SCPI_RES_OK;
}

scpi_result_t SCPI_PowerQ(scpi_t * context) {
    (void) context;
    double power=0;

    if(SCPI_calstate){
		power=rf_power_ctrl.GetPower();
    } else {
    	power=mcp4561_b.GetValue();
    }
    SCPI_ResultInt(context, power);
    return SCPI_RES_OK;
}

scpi_result_t SCPI_Output(scpi_t * context) {
    (void) context;
    if(SCPI_ParamBool(context, (scpi_bool_t*)&SCPI_output, 1)){
    	if(SCPI_output){
    		rf_power_ctrl.Relax(0);
    		modem.CW(1);
    	} else {
    		rf_power_ctrl.Relax(1);
    		modem.CW(0);
    	}
        return SCPI_RES_OK;
    }
    return SCPI_RES_ERR;
}

scpi_result_t SCPI_OutputQ(scpi_t * context) {
    (void) context;
    SCPI_ResultBool(context, SCPI_output);
    return SCPI_RES_OK;
}

scpi_result_t SCPI_CalState(scpi_t * context) {
    (void) context;
    SCPI_ParamBool(context, (scpi_bool_t*)&SCPI_calstate , 1);
    return SCPI_RES_OK;
}

scpi_result_t SCPI_CalStateQ(scpi_t * context) {
    (void) context;
    SCPI_ResultBool(context, SCPI_calstate);
    return SCPI_RES_OK;
}

scpi_result_t SCPI_CorrState(scpi_t * context) {
    (void) context;
    SCPI_ParamBool(context, (scpi_bool_t*)&SCPI_calstate , 1);
    return SCPI_RES_OK;
}

scpi_result_t SCPI_CorrStateQ(scpi_t * context) {
    (void) context;
    SCPI_ResultBool(context, SCPI_calstate);
    return SCPI_RES_OK;
}

const scpi_command_t SCPI::scpi_commands_default[]{
		/* {"pattern", callback} */
		/* IEEE Mandated Commands (SCPI std V1999.0 4.1.1) */
		{"*CLS", SCPI_CoreCls},
		{"*ESE", SCPI_CoreEse},
		{"*ESE?", SCPI_CoreEseQ},
		{"*ESR?", SCPI_CoreEsrQ},
		{"*IDN?", SCPI_CoreIdnQ},
		{"*OPC", SCPI_CoreOpc},
		{"*OPC?", SCPI_CoreOpcQ},
		{"*RST", SCPI_CoreRst},
		{"*SRE", SCPI_CoreSre},
		{"*SRE?", SCPI_CoreSreQ},
		{"*STB?", SCPI_CoreStbQ},
		{"*TST?", SCPI_CoreTstQ},
		{"*WAI", SCPI_CoreWai},

		/* Required SCPI commands (SCPI std V1999.0 4.2.1) */
		{"SYSTem:ERRor?", SCPI_SystemErrorNextQ},
		{"SYSTem:ERRor:NEXT?", SCPI_SystemErrorNextQ},
		{"SYSTem:ERRor:COUNt?", SCPI_SystemErrorCountQ},
		{"SYSTem:VERSion?", SCPI_SystemVersionQ},

		{"STATus:QUEStionable?", SCPI_StatusQuestionableEventQ},
		{"STATus:QUEStionable:EVENt?", SCPI_StatusQuestionableEventQ},
		{"STATus:QUEStionable:ENABle", SCPI_StatusQuestionableEnable},
		{"STATus:QUEStionable:ENABle?", SCPI_StatusQuestionableEnableQ},

		{"STATus:PRESet", SCPI_StatusPreset},

		//[:SOURce]:FREQuency:CENTer frequency[units]
		{"[SOURce]:FREQuency:CENTer", SCPI_Freq},
		{"[SOURce]:FREQuency:CENTer?", SCPI_FreqQ},

		//[:SOURce]:POWer[:LEVel][:IMMediate][:AMPLitude] value<units>
		{"[:SOURce]:POWer[:LEVel][:IMMediate][:AMPLitude]", SCPI_Power},
		{"[:SOURce]:POWer[:LEVel][:IMMediate][:AMPLitude]?", SCPI_PowerQ},

		{"[:SOURce]:POWer:CORRection[:STATe]", SCPI_CorrState},
		{"[:SOURce]:POWer:CORRection[:STATe]?", SCPI_CorrStateQ},

		//:OUTPut[:STATe] <ON|OFF|0|1>
		{"OUTPut[:STATe]", SCPI_Output},
		{"OUTPut[:STATe]?", SCPI_OutputQ},

		{"CALibration:VALue",SCPI_Stub},
		{"CALibration:VALue?",SCPI_StubQ},

		{"CALibration:STATe",SCPI_CalState},
		{"CALibration:STATe?",SCPI_CalStateQ},

		SCPI_CMD_LIST_END
};


SCPI::SCPI(const char *idn[4]):
	_bufferout(0,1024),
	_scpi_context{
			/* cmdlist */ scpi_commands_default,
			/* buffer */ { /* length */ sizeof(scpi_input_buffer), /* position */ 0,  /* data */ scpi_input_buffer, },
			/* paramlist */ { /* cmd */ NULL, /* parameters */ NULL, /* length */ 0, },
			/* interface */ (scpi_interface_t*)&scpi_interface,
			/* output_count */ 0,
			/* input_count */ 0,
			/* cmd_error */ FALSE,
			/* error_queue */ NULL,
			/* registers */ scpi_regs,
			/* units */ scpi_units_def,
			/* special_numbers */ scpi_special_numbers_def,
			/* user_context */ NULL,
			/* idn */ {idn[0],idn[1], idn[2], idn[3]}
		}
{

	_scpi_context.user_context=this;
	SCPI_Init(&_scpi_context);
}

size_t SCPI::_SCPI_Write(scpi_t * context, const char * data, size_t len) {
	return ((SCPI*)context->user_context)->SCPI_Write(context, data, len);
}

scpi_result_t SCPI::_SCPI_Flush(scpi_t * context) {
	return ((SCPI*)context->user_context)->SCPI_Flush(context);
}

int SCPI::_SCPI_Error(scpi_t * context, int_fast16_t err) {
	return ((SCPI*)context->user_context)->SCPI_Error(context, err);
}

scpi_result_t SCPI::_SCPI_Control(scpi_t * context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val) {
	return ((SCPI*)context->user_context)->SCPI_Control(context, ctrl, val);
}

scpi_result_t SCPI::_SCPI_Test(scpi_t * context) {
	return ((SCPI*)context->user_context)->SCPI_Test(context);
}

scpi_result_t SCPI::_SCPI_Reset(scpi_t * context) {
	return ((SCPI*)context->user_context)->SCPI_Reset(context);
}


size_t SCPI::SCPI_Write(scpi_t* context, const char* data, size_t len) {
	return _bufferout.In(data, len);
}

int SCPI::SCPI_Error(scpi_t* context, int_fast16_t err) {
	char buffer[256];
	uint16_t len=sprintf(buffer, "**ERROR: %d, \"%s\"\r\n", (int) err, SCPI_ErrorTranslate(err));
	SCPI_Write(context, buffer, len);
	return 0;
}

scpi_result_t SCPI::SCPI_Control(scpi_t* context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val) {
	char buffer[256];
	uint16_t len=0;
	if (SCPI_CTRL_SRQ == ctrl) {
		len+=sprintf(buffer, "**SRQ: 0x%X (%d)\r\n", val, val);
	} else {
		len+=sprintf(buffer, "**CTRL %02x: 0x%X (%d)\r\n", ctrl, val, val);
	}
	SCPI_Write(context, buffer, len);
	return SCPI_RES_OK;
}

scpi_result_t SCPI::SCPI_Reset(scpi_t* context) {
	char buffer[256];
	uint16_t len=0;
	len+=sprintf(buffer, "**Reset\r\n");
	return SCPI_RES_OK;
}

scpi_result_t SCPI::SCPI_Test(scpi_t* context) {
	char buffer[256];
	uint16_t len=0;
	len+=sprintf(buffer, "**Test\r\n");
	return SCPI_RES_OK;
}


scpi_result_t SCPI::SCPI_Flush(scpi_t* context) {
	return SCPI_RES_OK;
}

int SCPI::Parse(char * data, size_t len) {
	return SCPI_Parse(&_scpi_context, data, len);
}

int SCPI::In(char* data, size_t len) {
	if(len){
		return SCPI_Input(&_scpi_context, data, len);
	} else {
		return 0;
	}
}

int SCPI::Out(char* data, size_t len) {
	return _bufferout.Out(data, len);
}

int SCPI::In(CircularBuffer<uint8_t>& in) {
	int len=in.Ocupied();
	In((char*)in.GetRearrangedBuffer(), len);
	in.Out(0,len);
	return len;
}
