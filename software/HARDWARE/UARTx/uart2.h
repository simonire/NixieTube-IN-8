#ifndef UART2_H
#define UART2_H

#include "sys.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"

#define USART2_REC_LEN  			200  	//�����������ֽ��� 200
	  	
extern u8  USART2_RX_BUF[USART2_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART2_RX_STA;         		//����״̬���	

void uart2_Init(u32 baudrate);
void uart2_test(void);
void u2_printf(char *fmt,...);

#endif
