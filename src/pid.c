#include "stm32f4xx_conf.h"

#include "pid.h"

#define MIN(A,B)    ((A)<(B)?(A):(B))
#define MAX(A,B)    ((A)<(B)?(B):(A))
#define ABS(A)      ((A)>(0)?(A):(-A))

void pidInit(CONTROLADOR_S* Controlador, float Kp, float Ki, float Kd, float saidaMax, float integMax)
{
    Controlador->Kp = Kp;
    Controlador->Ki = Ki;
    Controlador->Kd = Kd;
    Controlador->entrada = 0;
    Controlador->erro = 0;
    Controlador->erro_anterior = 0;
    Controlador->erro_dif = 0;
    Controlador->erro_integ = 0;
    Controlador->realimentacao = 0;
    Controlador->saida = 0;
    Controlador->saidaMax = saidaMax;
    Controlador->integMax = integMax;
}

void posInit(POSICAO_S* P)
{
    P->TETAesperado = 0;
    P->TETAreal = 0;
    P->Vteta = 0;
    P->Vx = 0;
    P->Vy = 0;
    P->Xreal = 0;
    P->Xesperado = 0;
    P->Yesperado = 0;
    P->Yreal = 0;
}

void pidService(CONTROLADOR_S* Controlador)
{
    // calcula o erro
    Controlador->erro = Controlador->entrada - Controlador->realimentacao;
    // calcula a integral do erro
    Controlador->erro_integ += Controlador->erro;
    // limita a integral do erro
    Controlador->erro_integ = MAX(Controlador->erro_integ, (-ABS(Controlador->integMax / Controlador->Ki)));
    Controlador->erro_integ = MIN(Controlador->erro_integ,  ABS(Controlador->integMax / Controlador->Ki));
    // calcula a derivada do erro
    Controlador->erro_dif = Controlador->erro - Controlador->erro_anterior;
    Controlador->erro_anterior = Controlador->erro;
    // calcula o PID
    Controlador->saida = (Controlador->Kp * Controlador->erro)
                         + (Controlador->Ki * Controlador->erro_integ)
                         + (Controlador->Kd * Controlador->erro_dif);
    // limita a saída
    Controlador->saida = MAX(Controlador->saida, -Controlador->saidaMax);
    Controlador->saida = MIN(Controlador->saida, Controlador->saidaMax);
}

void pidServiceAngulo(CONTROLADOR_S* Controlador)
{
    // calcula o erro
    Controlador->erro = Controlador->entrada - Controlador->realimentacao;
    //Erro sempre menor que 180 graus
    while (Controlador->erro < -3.14) Controlador->erro += 2 * PI;
    while (Controlador->erro >  3.14) Controlador->erro -= 2 * PI;
    // calcula a integral do erro
    Controlador->erro_integ += Controlador->erro;
    // limita a integral do erro
    Controlador->erro_integ = MAX(Controlador->erro_integ, (-ABS(Controlador->integMax / Controlador->Ki)));
    Controlador->erro_integ = MIN(Controlador->erro_integ,  ABS(Controlador->integMax / Controlador->Ki));
    // calcula a derivada do erro
    Controlador->erro_dif = Controlador->erro - Controlador->erro_anterior;
    Controlador->erro_anterior = Controlador->erro;
    // calcula o PID
    Controlador->saida = (Controlador->Kp * Controlador->erro)
                         + (Controlador->Ki * Controlador->erro_integ)
                         + (Controlador->Kd * Controlador->erro_dif);
    // limita a saída
    Controlador->saida = MAX(Controlador->saida, -Controlador->saidaMax);
    Controlador->saida = MIN(Controlador->saida, Controlador->saidaMax);
}



void NewPosition(POSICAO_S* P, float x, float y, float teta)
{
    P->Xreal = x;
    P->Yreal = y;
    P->TETAreal = teta;
}

