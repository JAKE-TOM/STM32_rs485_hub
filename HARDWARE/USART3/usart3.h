#ifndef __USART3_H
#define __USART3_H	

#include "sys.h"	 								  
//////////////////////////////////////////////////////////////////////////////////	 

#define USART3_TX_EN		PBout(12)	//485ģʽ����.0,����;1,����.  		 


extern u16 USART3_RX_BUF[1000]; 		//���ջ���,���64���ֽ�
extern u16 USART3_RX_CNT;   			//���յ������ݳ���
extern u16 U3_buf[1000]; 	

void USART3_Init(u32 bound,u16 USART_Parity);
void USART3_Send_Data(u16* buf,u16 len);
void USART3_Receive_Data(u16 *buf,u16 *len);
void USART3_GPIO_Init();

#endif	   
















