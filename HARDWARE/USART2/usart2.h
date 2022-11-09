#ifndef __USART2_H
#define __USART2_H	

#include "sys.h"	 								  
//////////////////////////////////////////////////////////////////////////////////	 

#define USART2_TX_EN		PAout(4)	//485ģʽ����.0,����;1,����.  		 


extern u16 USART2_RX_BUF[1000]; 		//���ջ���,���64���ֽ�
extern u16 USART2_RX_CNT;   			//���յ������ݳ���
extern u16 U2_buf[1000]; 	

void USART2_Init(u32 bound,u16 USART_Parity);
void USART2_Send_Data(u16* buf,u16 len);
void USART2_Receive_Data(u16 *buf,u16 *len);
void USART2_GPIO_Init();

#endif	   
















