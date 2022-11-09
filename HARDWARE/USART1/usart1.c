#include "sys.h"		    
#include "delay.h"
#include "usart1.h"


//���ջ����� 	
u16 USART1_RX_BUF[1000];  	//���ջ���,���1000���ֽ�.
//���յ������ݳ���
u16 USART1_RX_CNT=0;   		  
  
u16 U1_buf[1000]; 	
	

void USART1_GPIO_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
 	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//ʹ��GPIOA,Dʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ʹ��USART1ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;				 //PD7�˿�����
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	//PA2
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);
   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA3
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);  

 
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; //ʹ�ܴ���2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //��ռ���ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //�����ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure); //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
 
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
	
	 USART1_TX_EN=0;			//Ĭ��Ϊ����ģʽ
	
}
//��ʼ��IO ����2
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������	  
void USART1_Init(u32 bound,u16 USART_Parity)
{  
   USART_InitTypeDef USART_InitStructure;
	

	USART_Cmd(USART1, DISABLE);                    //ʹ�ܴ��� 
	
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8λ���ݳ���
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity;///��żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//�շ�ģʽ

  USART_Init(USART1, &USART_InitStructure); ; //��ʼ������
     
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 

  USART1_TX_EN=0;			//Ĭ��Ϊ����ģʽ
 
}

//USART1����len���ֽ�.
//buf:�������׵�ַ
//len:���͵��ֽ���(Ϊ�˺ͱ�����Ľ���ƥ��,���ｨ�鲻Ҫ����64���ֽ�)
void USART1_Send_Data(u16 *buf,u16 len)
{
	u16 t;
	USART1_TX_EN=1;			//����Ϊ����ģʽ
  for(t=0;t<len;t++)		//ѭ����������
	{		   
		
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);	  
		USART_SendData(USART1,buf[t]);
	}	 

	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);		
	USART1_RX_CNT=0;	  
	USART1_TX_EN=0;				//����Ϊ����ģʽ	
}
//USART1��ѯ���յ�������
//buf:���ջ����׵�ַ
//len:���������ݳ���
void USART1_Receive_Data(u16 *buf,u16 *len)
{
	u16 rxlen=USART1_RX_CNT;
	u16 i=0;
	*len=0;				//Ĭ��Ϊ0
	delay_ms(10);		//�ȴ�10ms,��������10msû�н��յ�һ������,����Ϊ���ս���
	if(rxlen==USART1_RX_CNT&&rxlen)//���յ�������,�ҽ��������
	{

		for(i=0;i<rxlen;i++)
		{
			buf[i]=USART1_RX_BUF[i];	

		}			
		*len=USART1_RX_CNT;	//��¼�������ݳ���
		USART1_RX_CNT=0;		//����
	}
}


void USART1_IRQHandler(void)
{
	u16 res;	    
 
 	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) //���յ�����
	{	 
	 			 
		res =USART_ReceiveData(USART1); 	//��ȡ���յ�������
		if(USART1_RX_CNT<=1000)
		{
			USART1_RX_BUF[USART1_RX_CNT]=res;		//��¼���յ���ֵ
			USART1_RX_CNT++;						//������������1 

		} 
		
	}  											 
} 



















