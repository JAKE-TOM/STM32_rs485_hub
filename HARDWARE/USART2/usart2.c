#include "sys.h"		    
#include "delay.h"
#include "usart2.h"


//接收缓存区 	
u16 USART2_RX_BUF[1000];  	//接收缓冲,最大1000个字节.
//接收到的数据长度
u16 USART2_RX_CNT=0;   		  
  
u16 U2_buf[1000]; 	
	

void USART2_GPIO_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
 	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能GPIOA,D时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//使能USART2时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				 //PD7端口配置
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	//PA2
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽
  GPIO_Init(GPIOA, &GPIO_InitStructure);
   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);  

 
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn; //使能串口2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //先占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //从优先级2级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能外部中断通道
	NVIC_Init(&NVIC_InitStructure); //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器
 
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断
	
	 USART2_TX_EN=0;			//默认为接收模式
	
}
//初始化IO 串口2
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率	  
void USART2_Init(u32 bound,u16 USART_Parity)
{  
   USART_InitTypeDef USART_InitStructure;
	

	USART_Cmd(USART2, DISABLE);                    //使能串口 
	
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8位数据长度
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity;///奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//收发模式

  USART_Init(USART2, &USART_InitStructure); ; //初始化串口
     
  USART_Cmd(USART2, ENABLE);                    //使能串口 

  USART2_TX_EN=0;			//默认为接收模式
 
}

//USART2发送len个字节.
//buf:发送区首地址
//len:发送的字节数(为了和本代码的接收匹配,这里建议不要超过64个字节)
void USART2_Send_Data(u16 *buf,u16 len)
{
	u16 t;
	USART2_TX_EN=1;			//设置为发送模式
  for(t=0;t<len;t++)		//循环发送数据
	{		   

		
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);	  
		USART_SendData(USART2,buf[t]);
	}	 


	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);		
	USART2_RX_CNT=0;	  
	USART2_TX_EN=0;				//设置为接收模式	
}
//USART2查询接收到的数据
//buf:接收缓存首地址
//len:读到的数据长度
void USART2_Receive_Data(u16 *buf,u16 *len)
{
	u16 rxlen=USART2_RX_CNT;
	u16 i=0;
	*len=0;				//默认为0
	delay_ms(10);		//等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束
	if(rxlen==USART2_RX_CNT&&rxlen)//接收到了数据,且接收完成了
	{

		for(i=0;i<rxlen;i++)
		{
			buf[i]=USART2_RX_BUF[i];	

		}			
		*len=USART2_RX_CNT;	//记录本次数据长度
		USART2_RX_CNT=0;		//清零
	}
}


void USART2_IRQHandler(void)
{
	u16 res;	    
 
 	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //接收到数据
	{	 
	 			 
		res =USART_ReceiveData(USART2); 	//读取接收到的数据
		if(USART2_RX_CNT<=1000)
		{
			USART2_RX_BUF[USART2_RX_CNT]=res;		//记录接收到的值
			USART2_RX_CNT++;						//接收数据增加1 

		} 
		
	}  											 
} 



















