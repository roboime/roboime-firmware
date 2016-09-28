#include <radio/commands.h>
#include <radio/bsp.h>
#include <radio/serialnumber.h>

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
		modem.CW(0);
		rf_power_ctrl.Relax();
	} else {
		rf_power_ctrl.SetPower(freq, potdbm);
		modem.SetFrequency(freq);
		modem.CW(1);
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
		modem.CW(0);
	} else {
		modem.SetFrequency(1);
		modem.CW(1);
		size+=sprintf(buffer+size, "OK! Freq at %ld Hz!\r\n", freq);
	}
	return size;
}

uint16_t cmd_trimpot(uint16_t argc, uint8_t *argv8[]){
	const char **argv=(const char **)argv8;
	uint16_t size=0;
	char* buffer=(char*)argv[0];
	uint8_t pot=0;
	uint8_t syntax=1;
	if(argc==2){
		pot=atoi(argv[1]);
		if(pot<40){
			pot=40;
		} else {
			syntax=0;
		}
	}

	if(syntax){
		size+=sprintf(buffer+size, "Syntax: trimpot [value>50]\r\n");
	} else {
		mcp4561_b.SetValue(pot);
		size+=sprintf(buffer+size, "OK! TRIMPOT at %d!\r\n", pot);
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
			rf_power_ctrl.SetCalPot(calib);
			syntax=0;
			argc=2;
		}
	}
	if(argc==2){
		calib[0]=atoi(argv[1]);
		rf_power_ctrl.GetCalPot(calib);
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

uint16_t cmd_potv(uint16_t argc, uint8_t *argv8[]){
	const char **argv=(const char **)argv8;
	uint16_t size=0;
	char* buffer=(char*)argv[0];
	uint8_t channel=0;
	if(argc==2){
		channel=atoi(argv[1]);
	}
	if(channel==1 || channel==2){
		float res=ad7992.Read(channel);
		size+=sprintf(buffer+size, "%d", (uint16_t)(res));
		size+=sprintf(buffer+size, ".%03d\r\n", (uint16_t)(res*1000)%1000);
	} else {
		size+=sprintf(buffer+size, "Syntax: potv [1,2]\r\n");
	}
	return size;
}

uint16_t cmd_potd(uint16_t argc, uint8_t *argv8[]){
	const char **argv=(const char **)argv8;
	uint16_t size=0;
	char* buffer=(char*)argv[0];
	float calib[3];
	uint8_t syntax=1;
	if(argc==1){
		syntax=0;
	} else if(argc==4){
		calib[0]=atof(argv[1]);
		calib[1]=atof(argv[2]);
		calib[2]=atof(argv[3]);
		rf_power_ctrl.SetCalDetTrans(calib);
		syntax=0;
	}

	if(syntax){
		size+=sprintf(buffer+size, "Syntax: potd [power in dBm]\r\n");
	} else {
		float res=rf_power_ctrl.ReadPower();
	//	size+=sprintf(buffer+size, "%f", res);
		size+=sprintf(buffer+size, "%i", (int16_t)(res));
		size+=sprintf(buffer+size, ".%03d\r\n", (uint16_t)(res*1000)%1000);
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
		rf_power_ctrl.SaveCal();
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
		size+=sprintf(buffer+size, "IMBEL,TPP-1400,2016,1.3\r\n");
	} else {
		size+=sprintf(buffer+size, "Syntax: model\r\n");
	}
	return size;
}


extern uint8_t _gpsusbtestmode;
uint16_t cmd_gps(uint16_t argc, uint8_t *argv8[]){
	const char **argv=(const char **)argv8;
	uint16_t size=0;
	char* buffer=(char*)argv[0];

	if(argc==2){
		uint8_t gpsen=atoi(argv[1]);
		if(gpsen){
			_gpsusbtestmode=1;
		} else {
			_gpsusbtestmode=0;
		}
		size+=sprintf(buffer+size, "OK\r\n");
	} else {
		size+=sprintf(buffer+size, "Syntax: gps [0,1]\r\n");
	}
	return size;
}

CommandLine cmdline({"*IDN?", 	"cw", 	"trimpot", "potv", "serial", "model", "version", "calpot", "freq", "savecal", "potd", "gps"},
					{cmd_idn, 	cmd_cw,	cmd_trimpot, cmd_potv, cmd_serial, cmd_model, cmd_version, cmd_calpot, cmd_freq, cmd_savecal, cmd_potd, cmd_gps});