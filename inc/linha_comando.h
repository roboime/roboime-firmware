#ifndef LINHA_COMANDO_H
#define LINHA_COMANDO_H


#define LINHA_COMANDO_STRING_COMANDO_INVALIDO "\r\nComando Inválido"



typedef enum  {
	LINHA_COMANDO_DISPONIVEL=0,
	LINHA_COMANDO_OCUPADO=1
} linha_comando_estado_e;



typedef struct{
	char *comando;
	char *descricao;
	void (*funcao)(void *lcs);
	struct linha_comando_cmd_s *parent;
	struct linha_comando_cmd_s *sub;
} linha_comando_cmd_s;


typedef struct{
	unsigned char codigo;
	linha_comando_estado_e estado;
	unsigned char echo_local;
	void (*callback_putc)(char c);
	unsigned int buffer_size;
	unsigned int buffer_pos;
	unsigned char nr_parametros;
	unsigned char modo_matlab;
	char *buffer;
	linha_comando_cmd_s *lista_cmd;
} linha_comando_s;

unsigned char linha_comando_processar_char(char c, linha_comando_s *lcs);
void linha_comando_puts(char *s, linha_comando_s *lcs);
void display_matlab(float vel0, float vel1, float vel2, float vel3);

#endif

