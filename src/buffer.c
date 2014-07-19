#include "stm32f4xx_conf.h"
#include "buffer.h"

#include <stdint.h>

#define BUFFER_SIZE 30 /*TODO verificar tamanho  - Substituí por #define (Renault)*/

unsigned char buffer[BUFFER_SIZE];

/* Aponta para o ultimo dado q entrou*/
static int in = 0;

/* Aponta para o dado que vai ser enviado*/
static int out = -1;

/* Conta quantos dados estão disponíveis no buffer */
static int contador = 0;

void buffer_write(unsigned char dado)
{
    /* Verifica se o buffer está cheio*/
    if (BUFFER_SIZE == contador)
    {
        /* TODO: faz alguma coisa se tiver cheio */
    }
    else
    {
        /* Caso especial para quando o buffer estiver vazio*/
        if (contador == 0)
        {
            out++;
        }
        buffer[in] = dado;
        in = (in + 1) % BUFFER_SIZE;
        contador++;
    }
}

unsigned char buffer_read(void)
{
    if (buffer_dataIndication() == 1)
    {
        unsigned char dado = buffer[out];
        out = (out + 1) % BUFFER_SIZE;
        contador--;
        return dado;
    }
    else
    {
        /* TODO: faz alguma coisa se tiver fazio */
        return 0;
    }
}

/*
 * Indica se há dado disponível no buffer
 */
unsigned char buffer_dataIndication(void)
{
    return (contador > 0) ?  1 : 0;
}
