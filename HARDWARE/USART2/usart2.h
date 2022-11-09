#ifndef __USART2_H
#define __USART2_H	

#include "sys.h"	 								  
//////////////////////////////////////////////////////////////////////////////////	 

#define USART2_TX_EN		PAout(4)	//485模式控制.0,接收;1,发送.  		 


extern u16 USART2_RX_BUF[1000]; 		//接收缓冲,最大64个字节
extern u16 USART2_RX_CNT;   			//接收到的数据长度
extern u16 U2_buf[1000]; 	

void USART2_Init(u32 bound,u16 USART_Parity);
void USART2_Send_Data(u16* buf,u16 len);
void USART2_Receive_Data(u16 *buf,u16 *len);
void USART2_GPIO_Init();

#endif	   
















