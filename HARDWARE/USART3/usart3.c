#include "sys.h"		    
#include "delay.h"
#include "usart3.h"

//���ջ����� 	
u16 USART3_RX_BUF[1000];  	//���ջ���,���1000���ֽ�.
//���յ������ݳ���
u16 USART3_RX_CNT=0;   		  
  
u16 U3_buf[1000]; 	
	

void USART3_GPIO_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
 	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//ʹ��GPIOA,Dʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//ʹ��USART3ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;				 //PD7�˿�����
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	//PA2
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//��������
  GPIO_Init(GPIOB, &GPIO_InitStructure);
   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PA3
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //��������
  GPIO_Init(GPIOB, &GPIO_InitStructure);  

 
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn; //ʹ�ܴ���2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //��ռ���ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //�����ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure); //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
 
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�����ж�
	
	 USART3_TX_EN=0;			//Ĭ��Ϊ����ģʽ
	
}
//��ʼ��IO ����2
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������	  
void USART3_Init(u32 bound,u16 USART_Parity)
{  
   USART_InitTypeDef USART_InitStructure;
	

	USART_Cmd(USART3, DISABLE);                    //ʹ�ܴ��� 
	
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8λ���ݳ���
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity;///��żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//�շ�ģʽ

  USART_Init(USART3, &USART_InitStructure); ; //��ʼ������
     
  USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ��� 

  //USART3_TX_EN=0;			//Ĭ��Ϊ����ģʽ
 
}

//USART3����len���ֽ�.
//buf:�������׵�ַ
//len:���͵��ֽ���(Ϊ�˺ͱ�����Ľ���ƥ��,���ｨ�鲻Ҫ����64���ֽ�)
void USART3_Send_Data(u16 *buf,u16 len)
{
	u16 t;
	USART3_TX_EN=1;			//����Ϊ����ģʽ
  for(t=0;t<len;t++)		//ѭ����������
	{		   
		
		while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);	  
		USART_SendData(USART3,buf[t]);
	}	 

	while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);		
	USART3_RX_CNT=0;	  
	USART3_TX_EN=0;				//����Ϊ����ģʽ	
}
//USART3��ѯ���յ�������
//buf:���ջ����׵�ַ
//len:���������ݳ���
void USART3_Receive_Data(u16 *buf,u16 *len)
{
	u16 rxlen=USART3_RX_CNT;
	
	u16 i=0;
	*len=0;				//Ĭ��Ϊ0
	delay_ms(10);		//�ȴ�10ms,��������10msû�н��յ�һ������,����Ϊ���ս���
	if(rxlen==USART3_RX_CNT&&rxlen)//���յ�������,�ҽ��������
	{

		for(i=0;i<rxlen;i++)
		{
			buf[i]=USART3_RX_BUF[i];	

		}			
		*len=USART3_RX_CNT;	//��¼�������ݳ���
		USART3_RX_CNT=0;		//����
	}
}


void USART3_IRQHandler(void)
{
	u16 res;	    
 
 	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) //���յ�����
	{	 
	 			 
		res =USART_ReceiveData(USART3); 	//��ȡ���յ�������
		if(USART3_RX_CNT<=1000)
		{
			USART3_RX_BUF[USART3_RX_CNT]=res;		//��¼���յ���ֵ
			USART3_RX_CNT++;						//������������1 

		} 
		
	}  											 
} 



















