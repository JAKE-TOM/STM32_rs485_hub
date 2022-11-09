#include "sys.h"		    
#include "delay.h"
#include "usart2.h"


//���ջ����� 	
u16 USART2_RX_BUF[1000];  	//���ջ���,���1000���ֽ�.
//���յ������ݳ���
u16 USART2_RX_CNT=0;   		  
  
u16 U2_buf[1000]; 	
	

void USART2_GPIO_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;
 	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//ʹ��GPIOA,Dʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//ʹ��USART2ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;				 //PD7�˿�����
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	//PA2
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);
   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);  

 
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn; //ʹ�ܴ���2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //��ռ���ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //�����ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure); //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
 
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�
	
	 USART2_TX_EN=0;			//Ĭ��Ϊ����ģʽ
	
}
//��ʼ��IO ����2
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������	  
void USART2_Init(u32 bound,u16 USART_Parity)
{  
   USART_InitTypeDef USART_InitStructure;
	

	USART_Cmd(USART2, DISABLE);                    //ʹ�ܴ��� 
	
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8λ���ݳ���
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity;///��żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//�շ�ģʽ

  USART_Init(USART2, &USART_InitStructure); ; //��ʼ������
     
  USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ��� 

  USART2_TX_EN=0;			//Ĭ��Ϊ����ģʽ
 
}

//USART2����len���ֽ�.
//buf:�������׵�ַ
//len:���͵��ֽ���(Ϊ�˺ͱ�����Ľ���ƥ��,���ｨ�鲻Ҫ����64���ֽ�)
void USART2_Send_Data(u16 *buf,u16 len)
{
	u16 t;
	USART2_TX_EN=1;			//����Ϊ����ģʽ
  for(t=0;t<len;t++)		//ѭ����������
	{		   

		
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);	  
		USART_SendData(USART2,buf[t]);
	}	 


	while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);		
	USART2_RX_CNT=0;	  
	USART2_TX_EN=0;				//����Ϊ����ģʽ	
}
//USART2��ѯ���յ�������
//buf:���ջ����׵�ַ
//len:���������ݳ���
void USART2_Receive_Data(u16 *buf,u16 *len)
{
	u16 rxlen=USART2_RX_CNT;
	u16 i=0;
	*len=0;				//Ĭ��Ϊ0
	delay_ms(10);		//�ȴ�10ms,��������10msû�н��յ�һ������,����Ϊ���ս���
	if(rxlen==USART2_RX_CNT&&rxlen)//���յ�������,�ҽ��������
	{

		for(i=0;i<rxlen;i++)
		{
			buf[i]=USART2_RX_BUF[i];	

		}			
		*len=USART2_RX_CNT;	//��¼�������ݳ���
		USART2_RX_CNT=0;		//����
	}
}


void USART2_IRQHandler(void)
{
	u16 res;	    
 
 	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //���յ�����
	{	 
	 			 
		res =USART_ReceiveData(USART2); 	//��ȡ���յ�������
		if(USART2_RX_CNT<=1000)
		{
			USART2_RX_BUF[USART2_RX_CNT]=res;		//��¼���յ���ֵ
			USART2_RX_CNT++;						//������������1 

		} 
		
	}  											 
} 



















