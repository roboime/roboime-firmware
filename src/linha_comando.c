#include "stm32f4xx_conf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linha_comando.h"
#include "strings.h"
#include "motores.h"
#include "usart.h"
#include "timer.h"
#include "leds.h"
#include "chute.h"
#include "drible.h"

#define TRUE 1
#define FALSE 0

#define DEBUG_LC 0

char *proximo_parametro(char *buf){
	while(*buf!=' '){
		if(*buf==0){
			return 0;
		}
		buf++;
	}
	return buf+1;
}

u8 parametro_numero(char *buf){
	if(*buf=='-' || *buf=='+'){
		buf++;
	}
	if(*buf==0){
		return 0;
	}
	while(*buf!=' ' && *buf!=0){
		if(*buf<'0' || *buf>'9'){
			return 0;
		}
		buf++;
	}
	return 1;
}

u8 contar_parametros(char *buf){
	u8 nr_parametros=1;
	while(*buf){
		if(*buf++==' ')
			nr_parametros++;
	}
	return nr_parametros;
}

extern linha_comando_s lc_usart1;
extern linha_comando_s lc_usart3;

void display_matlab(float veloc0, float veloc1, float veloc2, float veloc3){
	char buf[50];
//	Led_Status_Toggle();
	sprintf(buf,"!%c%c",(((s16)(veloc0*10)) & 0xff),((s16)(veloc0*10)/256) & 0xff);
	sprintf(buf+3,"%c%c",(((s16)(veloc1*10)) & 0xff),((s16)(veloc1*10)/256) & 0xff);
	sprintf(buf+5,"%c%c",(((s16)(veloc2*10)) & 0xff),((s16)(veloc2*10)/256) & 0xff);
	sprintf(buf+7,"%c%c\r\n",(((s16)(veloc3*10)) & 0xff),((s16)(veloc3*10)/256) & 0xff);

	u8 i=0;
	for(i=0;i<11;i++){
		if(lc_usart1.modo_matlab)
			USART1_PrintChar(buf[i]);

		if(lc_usart3.modo_matlab)
			USART3_PrintChar(buf[i]);
	}
}

void comando_matlab(void *lcst){
	linha_comando_s *lcs=lcst;
	char *parametro=lcs->buffer;
	static u8 echo_local=0;
	do{
		if(lcs->nr_parametros==2){
			parametro=proximo_parametro(parametro);
			if(!parametro || !parametro_numero(parametro)){
				break;
			}
			u16 freq=atoi(parametro);
			if(freq){
				if(!lcs->modo_matlab){
					echo_local=lcs->echo_local;
					lcs->echo_local=0;
					lcs->modo_matlab=1;
				}
			} else {
				if(lcs->modo_matlab){
					lcs->echo_local=echo_local;
					lcs->modo_matlab=0;
				}
			}
			linha_comando_puts("OK",lcs);
			return;
		}
	} while(0);
	linha_comando_puts("Erro!\r\nFormato: matlab <freq>",lcs);
	return;
}

void comando_encoder(void *lcst){
	linha_comando_s *lcs=lcst;
	if(lcs->nr_parametros==1){
		sprintf(lcs->buffer,"Encoder[0]=%i\r\nEncoder[1]=%i\r\n",motor_get_posicao(0),motor_get_posicao(1));
		linha_comando_puts(lcs->buffer,lcs);
		sprintf(lcs->buffer,"Encoder[2]=%i\r\nEncoder[3]=%i",motor_get_posicao(2),motor_get_posicao(3));
		linha_comando_puts(lcs->buffer,lcs);
	}
}

void comando_spectrum_analyzer(void *lcst){
	linha_comando_s *lcs=lcst;
	u16 freq;
	u8 rssi;
	char *b;

	for(freq=40;freq<3900;freq+=40){
		rssi=rfm12_spectrum_analyzer(freq);
		sprintf(lcs->buffer,"[%03i]", freq/40);
		sprintf(lcs->buffer+rssi+5,"\r\n");
		while(rssi){
			lcs->buffer[4+rssi]='*';
			rssi--;
		}

		linha_comando_puts(lcs->buffer,lcs);
	}
	if(lcs->nr_parametros==1){
	}
}




void comando_chutar(void *lcst){
	linha_comando_s *lcs=lcst;
	char *parametro=lcs->buffer;
	if(lcs->nr_parametros==2){
		parametro=proximo_parametro(parametro);
		if(parametro && parametro_numero(parametro)){
			chutar(atoi(parametro));
			linha_comando_puts("OK",lcs);
			return;
		}
	}
	linha_comando_puts("Erro!\r\nFormato: chutar <tempo>",lcs);
	return;

}

void comando_driblar(void *lcst){
	linha_comando_s *lcs=lcst;
	char *parametro=lcs->buffer;
	if(lcs->nr_parametros==2){
		parametro=proximo_parametro(parametro);
		if(parametro && parametro_numero(parametro)){
			drible(atoi(parametro));
			linha_comando_puts("OK",lcs);
			return;
		}
	}
	linha_comando_puts("Erro!\r\nFormato: driblar <estado>",lcs);
	return;

}

void comando_motor(void *lcst){
	linha_comando_s *lcs=lcst;
	char *parametro=lcs->buffer;
	do{
		if(lcs->nr_parametros==3){
			parametro=proximo_parametro(parametro);
			if(!parametro || !parametro_numero(parametro)){
				break;
			}
			u8 nr_motor=atoi(parametro);
			parametro=proximo_parametro(parametro);
			if(!parametro || !parametro_numero(parametro)){
				break;
			}
			s16 valor_motor=atoi(parametro);
			motor_tensao(nr_motor,valor_motor);
			sprintf(lcs->buffer,"Motor[%d]=%d",nr_motor,valor_motor);
			linha_comando_puts(lcs->buffer,lcs);
			return;
		} else if(lcs->nr_parametros==1){
			sprintf(lcs->buffer,"Motor[0]=%d\r\nMotor[1]=%d\r\n",motor_get_tensao(0),motor_get_tensao(1));
			linha_comando_puts(lcs->buffer,lcs);
			sprintf(lcs->buffer,"Motor[2]=%d\r\nMotor[3]=%d",motor_get_tensao(2),motor_get_tensao(3));
			linha_comando_puts(lcs->buffer,lcs);
			return;
		}
	} while(0);
	linha_comando_puts("Erro!\r\nFormato: motor <numero> <valor>",lcs);
	return;

}

void comando_vel(void *lcst){
	linha_comando_s *lcs=lcst;
	char *parametro=lcs->buffer;
	do{
		if(lcs->nr_parametros==2){
			parametro=proximo_parametro(parametro);
			if(!parametro || !parametro_numero(parametro)){
				break;
			}
			s16 valor_motor=atoi(parametro);
			motor_velocidade(0,(float)valor_motor);
			motor_velocidade(1,(float)valor_motor);
			motor_velocidade(2,(float)valor_motor);
			motor_velocidade(3,(float)valor_motor);
			sprintf(lcs->buffer,"Vel[0]=%d\r\n",valor_motor);
			linha_comando_puts(lcs->buffer,lcs);
			sprintf(lcs->buffer,"Vel[1]=%d\r\n",valor_motor);
			linha_comando_puts(lcs->buffer,lcs);
			sprintf(lcs->buffer,"Vel[2]=%d\r\n",valor_motor);
			linha_comando_puts(lcs->buffer,lcs);
			sprintf(lcs->buffer,"Vel[3]=%d",valor_motor);
			linha_comando_puts(lcs->buffer,lcs);
			return;
		} else if(lcs->nr_parametros==3){
			parametro=proximo_parametro(parametro);
			if(!parametro || !parametro_numero(parametro)){
				break;
			}
			u8 nr_motor=atoi(parametro);
			parametro=proximo_parametro(parametro);
			if(!parametro || !parametro_numero(parametro)){
				break;
			}
			s16 valor_motor=atoi(parametro);
			motor_velocidade(nr_motor,(float)valor_motor);
			sprintf(lcs->buffer,"Vel[%d]=%d",nr_motor,valor_motor);
			linha_comando_puts(lcs->buffer,lcs);
			return;
		} else if(lcs->nr_parametros==1){
			sprintf(lcs->buffer,"Vel[0]=%d\r\nVel[1]=%d\r\n",(s16)(motor_get_velocidade(0)*10),(s16)(motor_get_velocidade(1)*10));
			linha_comando_puts(lcs->buffer,lcs);
			sprintf(lcs->buffer,"Vel[2]=%d\r\nVel[3]=%d",(s16)(motor_get_velocidade(2)*10),(s16)(motor_get_velocidade(3)*10));
			linha_comando_puts(lcs->buffer,lcs);
			return;
		}
	} while(0);
	linha_comando_puts("Erro!\r\nFormato: vel <numero> <valor>",lcs);
	linha_comando_puts("Formato: vel <valor>",lcs);
	return;
}



void comando_usart(void *lcst){
	linha_comando_s *lcs=lcst;
	char *parametro=lcs->buffer;
	do{
		if(lcs->nr_parametros==3){
			parametro=proximo_parametro(parametro);
			if(!parametro || !parametro_numero(parametro)){
				break;
			}
			u8 nr_usart=atoi(parametro);
			parametro=proximo_parametro(parametro);
			if(!parametro || !parametro_numero(parametro)){
				break;
			}
			u32 baudrate=atoi(parametro);
			if(baudrate==0){
				switch(nr_usart){
					case 3:
						USART3_Debug(1);
						break;
					case 4:
						UART4_Debug(1);
						break;
				}
			} else{
				switch(nr_usart){
					case 1:
						USART1_Baudrate(baudrate);
						break;
					case 3:
						USART3_Baudrate(baudrate);
						break;
					case 4:
						UART4_Baudrate(baudrate);
						break;
					case 2:
						//TODO USART3_Baudrate(baudrate);
						break;
					default:
						linha_comando_puts("Erro!\r\nFormato: usart <numero> <baudrate>",lcs);
						return;
				}
			}
			sprintf(lcs->buffer,"Usart[%d] Baudrate=%ld",nr_usart,baudrate);
			linha_comando_puts(lcs->buffer,lcs);
			return;
		} else if(lcs->nr_parametros==1){
			sprintf(lcs->buffer,"Usart[0] Baudrate=%d%03d\r\n",(u16)(USART1_Get_Baudrate()/1000),(u16)(USART1_Get_Baudrate()%1000));
			linha_comando_puts(lcs->buffer,lcs);
			sprintf(lcs->buffer,"Usart[3] Baudrate=%d%03d\r\n",(u16)(USART3_Get_Baudrate()/1000),(u16)(USART3_Get_Baudrate()%1000));
			linha_comando_puts(lcs->buffer,lcs);
			sprintf(lcs->buffer,"Usart[4] Baudrate=%d%03d",(u16)(UART4_Get_Baudrate()/1000),(u16)(UART4_Get_Baudrate()%1000));
			linha_comando_puts(lcs->buffer,lcs);
			return;
		}
	} while(0);
	linha_comando_puts("Erro!\r\nFormato: usart <numero> <baudrate>",lcs);
	return;

}

void comando_ls(void *lcst){
	linha_comando_s *lcs=lcst;
	linha_comando_puts("Comando LS",lcs);
}

void comando_parar(void *lcst){
	linha_comando_s *lcs=lcst;
	motor_velocidade(0,0);
	motor_velocidade(1,0);
	motor_velocidade(2,0);
	motor_velocidade(3,0);
	linha_comando_puts("OK",lcs);
}


void comando_bateria(void *lcst){
	extern volatile uint16_t ADC_Conversao[3];
	uint16_t ADC_Bateria=ADC_Conversao[0];
	linha_comando_s *lcs=lcst;

	u32 tensao=ADC_Bateria*3630/4095;
	sprintf(lcs->buffer,"Bateria=%d.%02d",(u16)tensao/100,(u16)tensao%100);
	linha_comando_puts(lcs->buffer,lcs);
}

void comando_corrente(void *lcst){
	linha_comando_s *lcs=lcst;
	static s16 correcao_corrente[]={10,10,10,10};
	extern volatile uint16_t ADC_Conversao[3];

	if(lcs->nr_parametros==1){
		s32 tensao=(ADC_Conversao[1]-correcao_corrente[0])*610/4095;
		if(tensao<0){
			tensao=0;
		}
		sprintf(lcs->buffer,"Corrente Motor[0]=%d.%02d\r\n",(u16)tensao/100,(u16)tensao%100);
		linha_comando_puts(lcs->buffer,lcs);
		tensao=(ADC_Conversao[2]-correcao_corrente[1])*610/4095;
		if(tensao<0){
			tensao=0;
		}
		sprintf(lcs->buffer,"Corrente Motor[1]=%d.%02d\r\n",(u16)tensao/100,(u16)tensao%100);
		linha_comando_puts(lcs->buffer,lcs);
		tensao=(ADC_Conversao[3]-correcao_corrente[2])*610/4095;
		if(tensao<0){
			tensao=0;
		}
		sprintf(lcs->buffer,"Corrente Motor[2]=%d.%02d\r\n",(u16)tensao/100,(u16)tensao%100);
		linha_comando_puts(lcs->buffer,lcs);
		tensao=(ADC_Conversao[4]-correcao_corrente[3])*610/4095;
		if(tensao<0){
			tensao=0;
		}
		sprintf(lcs->buffer,"Corrente Motor[3]=%d.%02d",(u16)tensao/100,(u16)tensao%100);
		linha_comando_puts(lcs->buffer,lcs);
		return;
	} else if(lcs->nr_parametros==2){
		char *par=proximo_parametro(lcs->buffer);
		if(par && parametro_numero(par) && atoi(par)==0){
			correcao_corrente[0]=ADC_Conversao[1];
			correcao_corrente[1]=ADC_Conversao[2];
			correcao_corrente[2]=ADC_Conversao[3];
			correcao_corrente[3]=ADC_Conversao[4];
			linha_comando_puts("OK",lcs);
			return;
		}
	}
	linha_comando_puts("ERRO!",lcs);
}

void comando_echo(void *lcst){
	linha_comando_s *lcs=lcst;
	char *parametro=lcs->buffer;
	if(lcs->nr_parametros==2){
		parametro=proximo_parametro(parametro);
		if(parametro && parametro_numero(parametro)){
			lcs->echo_local=atoi(parametro);
			linha_comando_puts("OK",lcs);
			return;
		}
	} else if(lcs->nr_parametros==1){
		sprintf(lcs->buffer, "Echo=%d", lcs->echo_local);
		linha_comando_puts(lcs->buffer,lcs);
		return;
	}
	linha_comando_puts("Erro!\r\nFormato: echo <estado>",lcs);
	return;
}

void comando_help(void *lcst){
	linha_comando_s *lcs=lcst;
	int i;
	for(i=0;lcs->lista_cmd[i].comando; i++){
		linha_comando_puts("\r\n",lcs);
		linha_comando_puts(lcs->lista_cmd[i].comando,lcs);
	}
}


linha_comando_cmd_s comandos[]={
		{
			"ls",
			"lista todos os arquivos",
			comando_ls,
			0,
			0
		},
		{
			"vel",
			"velociade do motor",
			comando_vel,
			0,
			0
		},
		{
			"help",
			"ajuda",
			comando_help,
			0,
			0
		},
		{
			"?",
			"ajuda",
			comando_help,
			0,
			0
		},
		{
			"motor",
			"comandos do motor",
			comando_motor,
			0,
			0
		},
		{
			"encoder",
			"encoder",//TODO descrição comando encoder
			comando_encoder,
			0,
			0
		},
		{
			"usart",
			"usart",//TODO descrição comando usart
			comando_usart,
			0,
			0
		},
		{
			"echo",
			"echo",//TODO descrição comando echo
			comando_echo,
			0,
			0
		},
		{
			"parar",
			"parar",//TODO descrição comando parar
			comando_parar,
			0,
			0
		},
		{
			"bateria",
			"bateria",//TODO descrição comando bateria
			comando_bateria,
			0,
			0
		},
		{
			"corrente",
			"corrente",//TODO descrição comando corrente
			comando_corrente,
			0,
			0
		},
		{
			"matlab",
			"matlab",//TODO descrição comando matlab
			comando_matlab,
			0,
			0
		},
		{
			"chutar",
			"chutar",//TODO descrição comando chutar
			comando_chutar,
			0,
			0
		},
		{
			"driblar",
			"driblar",//TODO descrição comando driblar
			comando_driblar,
			0,
			0
		},
		{
			"sa",
			"Spectrum Analyzer",//TODO descrição comando sa
			comando_spectrum_analyzer,
			0,
			0
		},
		{
				0,0,0,0,0
		}
};

void linha_comando_puts(char *s, linha_comando_s *lcs){
	while(*s){
		if(lcs->callback_putc){
			(*lcs->callback_putc)(*s++);
		}
	}
}

unsigned char linha_comando_processar_char(char c, linha_comando_s *lcs){
	static u8 pos=0;
	static u16 valor[4];
	if(lcs->modo_matlab){
		if(pos==0 && c=='!'){
			pos++;
		} else if(pos==1){
			valor[0]=c;
			pos++;
		} else if(pos==2){
			valor[0]+=c*256;
			pos++;
		} else if(pos==3){
			valor[1]=c;
			pos++;
		} else if(pos==4){
			valor[1]+=c*256;
			pos++;
		} else if(pos==5){
			valor[2]=c;
			pos++;
		} else if(pos==6){
			valor[2]+=c*256;
			pos++;
		} else if(pos==7){
			valor[3]=c;
			pos++;
		} else if(pos==8){
			valor[3]+=c*256;
			pos++;
		} else if(c=='\r' && pos==9){
			motor_velocidade(0,(((float)(s16)valor[0])/10.0));
			motor_velocidade(1,(((float)(s16)valor[1])/10.0));
			motor_velocidade(2,(((float)(s16)valor[2])/10.0));
			motor_velocidade(3,(((float)(s16)valor[3])/10.0));
			pos=0;
		} else {
			pos=0;
		}
	} else if(c=='\r' || c=='\n'){
		if(!lcs->buffer){
			return FALSE;
		}

		int i;

		//remove os espaços duplicados
		lcs->buffer_pos=remove_espacos_multiplos(lcs->buffer,lcs->buffer_pos);

		int comando=-1;
		int comandos_encontrados=0;
		for(i=0;lcs->lista_cmd[i].comando; i++){
			if(strstr( lcs->buffer, lcs->lista_cmd[i].comando)== lcs->buffer){
				char f=lcs->buffer[strlen(lcs->lista_cmd[i].comando)];
				if(f==' ' || f==0){
					comando=i;
					comandos_encontrados++;
				}
			}
		}

		if(comandos_encontrados==1){
			//TODO Tratar string do comando

			if(comando!=-1 && lcs->lista_cmd[comando].funcao!=0){
				lcs->nr_parametros=contar_parametros(lcs->buffer);
				linha_comando_puts("\r\n",lcs);
				lcs->lista_cmd[comando].funcao(lcs);
			}
		} else if(lcs->buffer_pos==0){
		} else {
			linha_comando_puts("\r\n",lcs);
			linha_comando_puts(lcs->buffer,lcs);
			linha_comando_puts(LINHA_COMANDO_STRING_COMANDO_INVALIDO,lcs);
		}
#if DEBUG_LC
		char b[1000];
		linha_comando_puts("\r\nComando: ",lcs);
		for(i=0;i<lcs->buffer_pos;i++){
			sprintf(b,"%d ",lcs->buffer[i]);
			linha_comando_puts(b,lcs);
		}
		linha_comando_puts("\r\n",lcs);
#endif
		lcs->buffer_pos=0;

		sprintf(lcs->buffer,"\r\n%s> ","cmd");
		linha_comando_puts(lcs->buffer,lcs);

	} else if(c=='\b'){
		if(!lcs->buffer){
			return FALSE;
		}

		if(lcs->estado!=LINHA_COMANDO_DISPONIVEL){
			return FALSE;
		}

		if(lcs->buffer_pos>0){
			lcs->buffer_pos--;
			if(lcs->echo_local){
				(*lcs->callback_putc)('\b');
				(*lcs->callback_putc)(' ');
				(*lcs->callback_putc)('\b');
			}
		}
	} else {
		if(!lcs->buffer)
			return FALSE;

		if(lcs->buffer_pos>=lcs->buffer_size){
			return FALSE;
		}

		if(lcs->estado!=LINHA_COMANDO_DISPONIVEL){
			return FALSE;
		}

		if(lcs->echo_local){
			(*lcs->callback_putc)(c);
		}
		lcs->buffer[lcs->buffer_pos++]=c;

	}
	return TRUE;
}
