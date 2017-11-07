#include <radio/commands.h>
#include <radio/bsp.h>
#include "proto/grSim_Commands.pb.h"

uint16_t cmd_idn(uint16_t argc, uint8_t *argv8[]){
	const char **argv=(const char **)argv8;
	uint16_t size=0;
	char* buffer=(char*)argv[0];

	if(argc==1){
		size+=sprintf(buffer+size, "IME,RoboIME,2016,0.1\r\n");
	} else {
		size+=sprintf(buffer+size, "Syntax: model\r\n");
	}
	return size;
}

uint16_t cmd_motv(uint16_t argc, uint8_t *argv8[]){
	const char **argv=(const char **)argv8;
	uint16_t size=0;
	char* buffer=(char*)argv[0];
	uint8_t motnr=0;
	if(argc==6){
		EuRobo.bitcontrol = true;
		grSim_Robot_Command* robotcmd;
		robotcmd = new grSim_Robot_Command;
		robotcmd->wheel1=atof(argv[1]);
		robotcmd->wheel2=atof(argv[2]);
		robotcmd->wheel3=atof(argv[3]);
		robotcmd->wheel4=atof(argv[4]);
		xQueueSendToBack(fila_vel, &robotcmd, portMAX_DELAY);
		size+=sprintf(buffer+size, "OK\r\n");
	}if(argc==5){
		EuRobo.bitcontrol = false;
		for(int i = 0; i<4; i++){
			EuRobo.motors[i]->SetDutyCycle(atoi(argv[i+1]));
		}
		size+=sprintf(buffer+size, "OK\r\n");
	}
	else {
		size+=sprintf(buffer+size, "        motv vel0 vel1 vel2 vel3 vel4\r\n");
	}
	return size;
}

uint16_t cmd_ctl(uint16_t argc, uint8_t *argv8[]){
	const char **argv=(const char **)argv8;
	uint16_t size=0;
	char* buffer=(char*)argv[0];

	if(argc==2){
		EuRobo.bitcontrol = true;
		xTaskNotify(t2, atoi(argv[1]), eSetValueWithOverwrite);
		size+=sprintf(buffer+size, "Teste dos motores\r\n");
	} else {
		size+=sprintf(buffer+size, "Syntax: model\r\n");
	}
	return size;
}

CommandLine cmdline({"*IDN?", "motv", "ctl"},
					{cmd_idn, cmd_motv, cmd_ctl});
