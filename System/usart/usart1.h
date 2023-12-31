#ifndef __usart1_H
#define __usart1_H	 
#include "stm32f10x.h"
#include <stdbool.h>

#define USART1_RXBUFF_SIZE   88 

extern unsigned int  RxCounter;          //外部声明，其他文件可以调用该变量
extern unsigned char Usart1RecBuf[USART1_RXBUFF_SIZE]; //外部声明，其他文件可以调用该变量
extern bool USART_RX_FLAG;

void uart1_Init(u32 bound);
void uart1_SendStr(char*SendBuf);
void uart1_send(unsigned char *bufs,unsigned char len);
		 				    
#endif

