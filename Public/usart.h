#ifndef __usart_H
#define __usart_H
#include "stm32f10x.h"
#include "stdio.h" 

void USART1_Init(u32 bound);
void USART2_Init(u32 bound);
void USART1_on(void);

#endif


