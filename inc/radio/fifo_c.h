#pragma once
#include "inttypes.h"

typedef struct{
	uint8_t *Buffer;
	uint16_t Size;
	uint16_t Head;
	uint16_t Tail;
	uint8_t Allocated;
	uint8_t Initiated;
} FIFO_Structure;


uint16_t FIFO_Free(FIFO_Structure *fifo);
uint16_t FIFO_Ocupied(FIFO_Structure *fifo);
void FIFO_Init(FIFO_Structure *fifo, uint8_t *buffer, uint16_t size);
uint16_t FIFO_In(FIFO_Structure *fifo, uint8_t* buffer, uint16_t size);
uint16_t FIFO_In_Item(FIFO_Structure *fifo, uint8_t item);
uint16_t FIFO_Out(FIFO_Structure *fifo, uint8_t* buffer, uint16_t size);
uint8_t FIFO_Out_Item(FIFO_Structure *fifo);
void FIFO_Flush(FIFO_Structure *fifo) ;
