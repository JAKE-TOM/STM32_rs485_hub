#ifndef __USART1_H
#define __USART1_H	

#include "sys.h"	 								  
//////////////////////////////////////////////////////////////////////////////////	 

#define USART1_TX_EN		PAout(11)	//485ģʽ����.0,����;1,����.  		 


extern u16 USART1_RX_BUF[1000]; 		//���ջ���,���64���ֽ�
extern u16 USART1_RX_CNT;   			//���յ������ݳ���
extern u16 U1_buf[1000]; 	

void USART1_Init(u32 bound,u16 USART_Parity);
void USART1_Send_Data(u16* buf,u16 len);
void USART1_Receive_Data(u16 *buf,u16 *len);
void USART1_GPIO_Init();

#endif	   
















