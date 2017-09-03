#include <radio/commands.h>
#include <radio/bsp.h>
#include <radio/serialnumber.h>
#include "control/Robo.h"

uint16_t cmd_cw(uint16_t argc, uint8_t *argv8[]){
	const char **argv=(const char **)argv8;
	uint16_t size=0;
	char* buffer=(char*)argv[0];
	uint32_t freq=0;
	uint8_t syntax=1;
	uint8_t pot=100;
	static uint8_t potdbm=24;
	if(argc==2 || argc==3){
		freq=atoi(argv[1]);
		if(argc==3){
			potdbm=atoi(argv[2]);
		}
		if(freq && potdbm>=24 && potdbm<=35){
			syntax=0;
		}
	}

	if(syntax){
		size+=sprintf(buffer+size, "Syntax: cw [freq in Hz] [pot in dBm]\r\n");
//		modem.CW(0);
//		rf_power_ctrl.Relax();
	} else {
//		rf_power_ctrl.SetPower(freq, potdbm);
//		modem.SetFrequency(freq);
//		modem.CW(1);
		size+=sprintf(buffer+size, "OK! CW at %ld Hz Pot at %d dBm Trimpot at %d!\r\n", freq, potdbm, pot);
	}
	return size;
}

uint16_t cmd_freq(uint16_t argc, uint8_t *argv8[]){
	const char **argv=(const char **)argv8;
	uint16_t size=0;
	char* buffer=(char*)argv[0];
	uint32_t freq=0;
	uint8_t syntax=1;
	if(argc==2){
		freq=atoi(argv[1]);
		if(freq){
			syntax=0;
		}
	}

	if(syntax){
		size+=sprintf(buffer+size, "Syntax: freq [freq in Hz]\r\n");
//		modem.CW(0);
	} else {
//		modem.SetFrequency(1);
//		modem.CW(1);
		size+=sprintf(buffer+size, "OK! Freq at %ld Hz!\r\n", freq);
	}
	return size;
}



uint16_t cmd_calpot(uint16_t argc, uint8_t *argv8[]){
	const char **argv=(const char **)argv8;
	uint16_t size=0;
	char* buffer=(char*)argv[0];
	uint8_t pot=0;
	uint8_t syntax=1;
	uint8_t calib[50];
	if(argc>3){
		pot=atoi(argv[1]);
		if(pot<24 || pot>35){
		} else {
			calib[0]=pot;
			calib[1]=argc-2;;
			for(uint8_t i=2;i<argc;i++){
				calib[i]=atoi(argv[i]);
			}
//			rf_power_ctrl.SetCalPot(calib);
			syntax=0;
			argc=2;
		}
	}
	if(argc==2){
		calib[0]=atoi(argv[1]);
//		rf_power_ctrl.GetCalPot(calib);
		if(calib[0]<24 || calib[0]>35){
		} else {
			for(uint8_t i=0;i<(calib[1]+2);i++){
				if(i!=1){
					size+=sprintf(buffer+size, "%03d ", calib[i]);
				}
			}
			size+=sprintf(buffer+size, "\r\n");
			syntax=0;
		}
	}
	if(syntax){
		size+=sprintf(buffer+size, "Syntax: calpot [pot] [val(350MHz)] ... [val(450MHz)]\r\n");
	}
	return size;
}




uint16_t cmd_potd(uint16_t argc, uint8_t *argv8[]){
	const char **argv=(const char **)argv8;
	uint16_t size=0;
	char* buffer=(char*)argv[0];
	//float calib[3];
	uint8_t syntax=1;
	if(argc==1){
		syntax=0;
	} else if(argc==4){
//		calib[0]=atof(argv[1]);
//		calib[1]=atof(argv[2]);
//		calib[2]=atof(argv[3]);
//		rf_power_ctrl.SetCalDetTrans(calib);
		syntax=0;
	}

	if(syntax){
		size+=sprintf(buffer+size, "Syntax: potd [power in dBm]\r\n");
	} else {
//		float res=rf_power_ctrl.ReadPower();
	//	size+=sprintf(buffer+size, "%f", res);
//		size+=sprintf(buffer+size, "%i", (int16_t)(res));
//		size+=sprintf(buffer+size, ".%03d\r\n", (uint16_t)(res*1000)%1000);
	}
	return size;
}

uint16_t cmd_serial(uint16_t argc, uint8_t *argv8[]){
	const char **argv=(const char **)argv8;
	uint16_t size=0;
	char* buffer=(char*)argv[0];
	if(argc==1){
		uint8_t* serial=(uint8_t*)SerialNumberGetHexaString();
		for(uint16_t i=0;i<25;i++){
			buffer[i]=serial[i];
		}
		buffer[24]='\r';
		buffer[25]='\n';
		size+=26;
	} else {
		size+=sprintf(buffer+size, "Syntax: serial\r\n");
	}
	return size;
}

uint16_t cmd_model(uint16_t argc, uint8_t *argv8[]){
	const char **argv=(const char **)argv8;
	uint16_t size=0;
	char* buffer=(char*)argv[0];
	if(argc==1){
		buffer[0]='T';
		buffer[1]='P';
		buffer[2]='P';
		buffer[3]='-';
		buffer[4]='1';
		buffer[5]='4';
		buffer[6]='0';
		buffer[7]='0';
		buffer[8]='\r';
		buffer[9]='\n';
		size+=10;
	} else {
		size+=sprintf(buffer+size, "Syntax: model\r\n");
	}
	return size;
}



uint16_t cmd_version(uint16_t argc, uint8_t *argv8[]){
	const char **argv=(const char **)argv8;
	uint16_t size=0;
	char* buffer=(char*)argv[0];
	if(argc==1){
		size+=sprintf(buffer+size, "Build %04d\r\n", 1);
	} else {
		size+=sprintf(buffer+size, "Syntax: version\r\n");
	}
	return size;
}

uint16_t cmd_savecal(uint16_t argc, uint8_t *argv8[]){
	const char **argv=(const char **)argv8;
	uint16_t size=0;
	char* buffer=(char*)argv[0];
	if(argc==1){
//		rf_power_ctrl.SaveCal();
		size+=sprintf(buffer+size, "OK!\r\n");
	} else {
		size+=sprintf(buffer+size, "Syntax: savecal\r\n");
	}
	return size;
}



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


uint16_t cmd_gps(uint16_t argc, uint8_t *argv8[]){
	const char **argv=(const char **)argv8;
	uint16_t size=0;
	char* buffer=(char*)argv[0];

	if(argc==2){
		uint8_t gpsen=atoi(argv[1]);
		if(gpsen){
//			_gpsusbtestmode=1;
		} else {
//			_gpsusbtestmode=0;
		}
		size+=sprintf(buffer+size, "OK\r\n");
	} else {
		size+=sprintf(buffer+size, "Syntax: gps [0,1]\r\n");
	}
	return size;
}

uint16_t cmd_chute(uint16_t argc, uint8_t *argv8[]){
	const char **argv=(const char **)argv8;
	uint16_t size=0;
	char* buffer=(char*)argv[0];
	uint8_t pot=0;

	if(argc==2 && (pot=atoi(argv[1]))<=2 && pot){
		size+=sprintf(buffer+size, "OK!\r\n");
		switch(pot){
		case 1:
			robo.ChuteBaixo((float) 100);
			break;
		case 2:
			robo.HighKick((float) 100);
			break;
		default:
			break;
		}

	} else {
		size+=sprintf(buffer+size, "Syntax: chute [1=baixo 2=alto]\r\n");
	}
	return size;
}

uint16_t cmd_robv(uint16_t argc, uint8_t *argv8[]){
	const char **argv=(const char **)argv8;
	uint16_t size=0;
	char* buffer=(char*)argv[0];
	uint8_t motnr=0;
	if(argc==4 && motnr<5){
		float v[3];
		v[0]=atof(argv[1]);
		v[1]=atof(argv[2]);
		v[2]=atof(argv[3]);
		robo.set_speed(v[0], v[1], v[2]);
		size+=sprintf(buffer+size, "OK\r\n");
	} else {
		size+=sprintf(buffer+size, "Syntax: robv velt velr vela \r\n");
	}
	return size;
}

uint16_t cmd_pid(uint16_t argc, uint8_t *argv8[]){
	const char **argv=(const char **)argv8;
	uint16_t size=0;
	char* buffer=(char*)argv[0];
	uint8_t motnr=0;
	if(argc==4 && motnr<5){
		float c[3];
		c[0]=atof(argv[1]);
		c[1]=atof(argv[2]);
		c[2]=atof(argv[3]);
		Motor::SetPID(c[0], c[1], c[2]);
		size+=sprintf(buffer+size, "OK\r\n");
	} else if(argc==1){
		float pid_cte[3];
		Motor::GetPID(pid_cte);
		size+=sprintf(buffer+size, "cp: %.4f \r\n", pid_cte[0]);
		size+=sprintf(buffer+size, "ci: %.4f \r\n", pid_cte[1]);
		size+=sprintf(buffer+size, "cd: %.4f \r\n", pid_cte[2]);
	} else {
		size+=sprintf(buffer+size, "Syntax: pid \r\n");
		size+=sprintf(buffer+size, "        pid cp ci cd \r\n");
	}
	return size;
}


uint16_t cmd_motv(uint16_t argc, uint8_t *argv8[]){
	const char **argv=(const char **)argv8;
	uint16_t size=0;
	char* buffer=(char*)argv[0];
	uint8_t motnr=0;
	if(argc==2){//com 1 argumento, motv imprime a velocidade da roda
		robo.get_wheel_speeds(robo.real_wheel_speed);
		motnr=atoi(argv[1]);
		size+=sprintf(buffer+size, "Roda %d: %.3f m/s\r\n",motnr,robo.real_wheel_speed[motnr]);
	} else if(argc==3 && motnr<5){
		motnr=atoi(argv[1]);
		float vel=atof(argv[2]);
		robo.set_motor_speed(motnr, vel);
		size+=sprintf(buffer+size, "OK\r\n");
	} else if(argc==6){
		float v[5];
		v[0]=atof(argv[1]);
		v[1]=atof(argv[2]);
		v[2]=atof(argv[3]);
		v[3]=atof(argv[4]);

		robo.set_speed(v);
		size+=sprintf(buffer+size, "OK\r\n");
	} else {
		size+=sprintf(buffer+size, "Syntax: motv 0..4 vel \r\n");
		size+=sprintf(buffer+size, "        motv 0..4 \r\n");
		size+=sprintf(buffer+size, "        motv vel0 vel1 vel2 vel3 vel4\r\n");
	}
	return size;
}

uint16_t cmd_testmode(uint16_t argc, uint8_t *argv8[]){
	const char **argv=(const char **)argv8;
	uint16_t size=0;
	char* buffer=(char*)argv[0];
	if(argc==2){
		robo.SetTestMode(atoi(argv[1]));
		size+=sprintf(buffer+size, "OK\r\n");
	} else {
		size+=sprintf(buffer+size, "Syntax: testmode [0, 1]\r\n");
	}
	return size;
}
uint16_t cmd_writefoo(uint16_t argc, uint8_t *argv8[]){
	const char **argv=(const char **)argv8;
	uint16_t size=0;
	char* buffer=(char*)argv[0];
	if(argc==2){
		size+=sprintf(buffer+size, "OK\r\n");
	} else {
		int x=7;
		size+=sprintf(buffer+size, "Hello picoli %d \r\n", x);
	}
	return size;
}

// imprime as velocidades de cada RODA
uint16_t cmd_readv(uint16_t argc, uint8_t *argv8[]){
	const char **argv=(const char **)argv8;
	uint16_t size=0;
	char* buffer=(char*)argv[0];
	if(argc==1){
		float v[4];
		robo.get_wheel_speeds(v);
		for(int i=0; i<4;i++){
			size+=sprintf(buffer+size, "Roda %d: %.2f m/s\r\n",i,v[i]);
		}
		size+=sprintf(buffer+size, "OK\r\n");
	} else {
		size+=sprintf(buffer+size, "Syntax: readv\r\n");
	}
	return size;
}

uint16_t cmd_sendv(uint16_t argc, uint8_t *argv8[]){
	const char **argv=(const char **)argv8;
	uint16_t size=0;
	char* buffer=(char*)argv[0];
	if(argc==1){//com 1 argumento, motv imprime a velocidade da roda
		robo.printv = true;
		} else {
		size+=sprintf(buffer+size, "Syntax: sendv\r\n");
	}
	return size;
}

uint16_t cmd_n_sendv(uint16_t argc, uint8_t *argv8[]){
	const char **argv=(const char **)argv8;
	uint16_t size=0;
	char* buffer=(char*)argv[0];
	if(argc==1){//com 1 argumento, motv imprime a velocidade da roda
		robo.printv = false;
		} else {
		size+=sprintf(buffer+size, "Syntax: n_sendv\r\n");
	}
	return size;
}

uint16_t cmd_step(uint16_t argc, uint8_t *argv8[]){
	const char **argv=(const char **)argv8;
	uint16_t size=0;
	char* buffer=(char*)argv[0];
	uint8_t motnr=0;
	if(argc==3){
		motnr=atoi(argv[1]);
		int vel=atoi(argv[2]);
		robo.motors[motnr]->SetDutyCycle(vel);
		for(int i=0; i<4; i++){
			robo.dutycycles[i]=(int16_t) vel;
		}
		size+=sprintf(buffer+size, "OK, %d %d\r\n", motnr, vel);
	} else if(argc==5){
		float v[4];
		v[0]=atoi(argv[1]);
		v[1]=atoi(argv[2]);
		v[2]=atoi(argv[3]);
		v[3]=atoi(argv[4]);

		robo.motors[0]->SetDutyCycle(v[0]);
		robo.motors[1]->SetDutyCycle(v[1]);
		robo.motors[2]->SetDutyCycle(v[2]);
		robo.motors[3]->SetDutyCycle(v[3]);
		for(int i=0; i<4; i++){
			robo.dutycycles[i]=(int16_t) v[i];
		}
		size+=sprintf(buffer+size, "OK\r\n");
	} else {
		size+=sprintf(buffer+size, "Syntax: step motor dutycycle \r\n");
		size+=sprintf(buffer+size, "        step dutycycle0 dutycycle1 dutycycle2 dutycycle3\r\n");
	}
	return size;
}

uint16_t cmd_ctl_or_step(uint16_t argc, uint8_t *argv8[]){
	const char **argv=(const char **)argv8;
	uint16_t size=0;
	char* buffer=(char*)argv[0];
	if(argc==1){//com 1 argumento, h� step e n�o control
		if (robo.stepbit){
			robo.stepbit = false;
			robo.controlbit = true;
		}
		else {
			robo.stepbit = true;
			robo.controlbit = false;
		}
		//size+=sprintf(buffer+size, "Syntax: step\r\n");
	}
	else {
		size+=sprintf(buffer+size, "Syntax: control_or_step\r\n");
	}
	return size;
}

uint16_t cmd_sendI(uint16_t argc, uint8_t *argv8[]){
	const char **argv=(const char **)argv8;
	uint16_t size=0;
	char* buffer=(char*)argv[0];
	if(argc==1){//com 1 argumento, printI imprime a corrente da roda
		robo.printI = true;
		} else {
		size+=sprintf(buffer+size, "Syntax: sendI\r\n");
	}
	return size;
}

uint16_t cmd_n_sendI(uint16_t argc, uint8_t *argv8[]){
	const char **argv=(const char **)argv8;
	uint16_t size=0;
	char* buffer=(char*)argv[0];
	if(argc==1){//com 1 argumento, printT imprime a corrente da roda
		robo.printI = false;
		} else {
		size+=sprintf(buffer+size, "Syntax: n_sendI\r\n");
	}
	return size;
}

CommandLine cmdline({"*IDN?", "testmode",	"pid", 	"chute", "readv", "motv", "writefoo", "robv", "serial", "model", "version", "calpot", "freq", "savecal", "potd", "gps", "sendv", "n_sendv", "step", "ctl_or_step", "sendI", "n_sendI"},
					{cmd_idn,	cmd_testmode, 	cmd_pid,	cmd_chute, cmd_readv, cmd_motv, cmd_writefoo, cmd_robv, cmd_serial, cmd_model, cmd_version, cmd_calpot, cmd_freq, cmd_savecal, cmd_potd, cmd_gps, cmd_sendv, cmd_n_sendv, cmd_step, cmd_ctl_or_step, cmd_sendI, cmd_n_sendI});
