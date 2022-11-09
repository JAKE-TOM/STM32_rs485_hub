#include "sys.h"
#include "string.h"
#include "delay.h"
#include "usart1.h"
#include "usart2.h"
#include "usart3.h"
#include "oled.h"
#include "stmflash.h"

/* ------------------------------�汾˵�� RS485HUB-------------------------
															  2022-3-14
V2.0 ��д���
�޸��� USART2 USART3 ���������


------------------------------------------------------------------*/
	
/*��������*/
uint32_t Usart_band_talbe[] = {1200,2400,4800,9600,19200,38400,57600,115200};
uint32_t Usart1_band_Buff = 9600;
uint32_t Usart2_band_Buff = 9600;
uint32_t Usart3_band_Buff = 9600;

uint16_t Usart_parity_table[] = {USART_Parity_No,USART_Parity_Even,USART_Parity_Odd};
uint16_t Usart1_parity_Buff = USART_Parity_No;
uint16_t Usart2_parity_Buff = USART_Parity_No;
uint16_t Usart3_parity_Buff = USART_Parity_No;
/*��վ��ʱʱ��*/
uint32_t Time_Out_talbe[] = {100,200,500,1000,1500,2000};
uint32_t Time_Out_Buff = 1000;//Ĭ��1000ms

/*FLASH*/
uint16_t FLASH_BUFF[10];

u16 U1_RX_LEN=0;
u16 U2_RX_LEN=0;
u16 U3_RX_LEN=0;

u16 QT_ReadBuff[9] = {0};

u16 KEY_Flag = 0; //���������жϱ�ֵ ������ KEY_Flag == 1��������KEY_Flag == 2
char * strbuf;
u8 Connect_Ok = 0;//���ӳɹ���־λ
u16 Time_out_Num = 0;//��ʱʱ��

u8 OLED_Usart1_BandFlag=0,OLED_Usart2_BandFlag=1,OLED_Usart3_BandFlag=1;//��Щ��־λ��Ϊ�˺���OLEDˢ��ʹ�ã����ö�FLASH��ʡʱ��
u8 OLED_U1_ParityFlag=0,OLED_U2_ParityFlag=0,OLED_U3_ParityFlag=0;


void Connect_WintoRS485HUB();
void System_Initializes();
void OLED_485HUB_Show();
void USART1_USART23();
void USART23_USART1();

void USART_FLASH_Set()
{
	
	STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)datatemp,10);
	if(datatemp[7] == 65535 &&datatemp[8] == 65535 )//FLASHû��д������  ˢ�����һ������
	{
		USART1_Init(115200,USART_Parity_No);
		USART2_Init(9600,USART_Parity_No);
		USART3_Init(9600,USART_Parity_No);
	}
	else//��FLASH�ж�ȡ����
	{
	//	if(datatemp[1] == 0xC200)//��Ӧ�Ĳ�����115200
			USART1_Init(115200,datatemp[2]);
	//	else
	//		USART1_Init(datatemp[1],datatemp[2]);
		if(datatemp[3] == 0xC200)//��Ӧ�Ĳ�����115200
			USART2_Init(115200,datatemp[4]);
		else
			USART2_Init(datatemp[3],datatemp[4]);		
		if(datatemp[5] == 0xC200)//��Ӧ�Ĳ�����115200
			USART3_Init(115200,datatemp[6]);
		else
			USART3_Init(datatemp[5],datatemp[6]);				
	}	
}
/*
ϵͳ��ʼ������
�������� + ��Ļ
*/
void System_Initializes()
{
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	delay_init();	    		//��ʱ������ʼ��	 
		
	USART1_GPIO_Init();
	USART2_GPIO_Init();
	USART3_GPIO_Init();

	//��������	//	USART_FLASH_Set();	
	USART_FLASH_Set();
	
	OLED_Init();
}
//


void OLED_485HUB_Show()
{
	//�������� �������ڲ����� У��λ
	u8 i=0;

	OLED_CLS();
	STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)datatemp,10);
	delay_ms(20);

	OLED_ShowString(45,0,"485HUB",16);
	OLED_ShowString(0,2,"UART1:",16);
	OLED_ShowString(0,4,"UART2:",16);
	OLED_ShowString(0,6,"UART3:",16);
	// usart1 ������
	if(datatemp[1] == 0x4B0)
	{
		OLED_Usart1_BandFlag=1;
		OLED_ShowString(48,2,"1200,",16);
	}
	else if(datatemp[1] == 0x960)
	{
		OLED_Usart1_BandFlag=2;
		OLED_ShowString(48,2,"2400,",16);
	}
	else if(datatemp[1] == 0x12C0)
	{
		OLED_Usart1_BandFlag=3;
		OLED_ShowString(48,2,"4800,",16);
	}
	else if(datatemp[1] == 0x2580)
	{
		OLED_Usart1_BandFlag=4;
		OLED_ShowString(48,2,"9600,",16);
	}
	else if(datatemp[1] == 0x4B00)
	{
		OLED_Usart1_BandFlag=5;
		OLED_ShowString(48,2,"19200,",16);
	}
	else if(datatemp[1] == 0x9600)
	{
		OLED_Usart1_BandFlag=6;
		OLED_ShowString(48,2,"38400,",16);
	}
	else if(datatemp[1] == 0xE100)
	{
		OLED_Usart1_BandFlag=7;
		OLED_ShowString(48,2,"57600,",16);
	}
	else if(datatemp[1] == 0xC200)
	{
		OLED_Usart1_BandFlag=8;
		OLED_ShowString(48,2,"115200,",16);
	}
		// usart2 ������
	if(datatemp[3] == 0x4B0)
	{
		OLED_Usart2_BandFlag=1;
		OLED_ShowString(48,4,"1200,",16);
	}
	else if(datatemp[3] == 0x960)
	{
		OLED_Usart2_BandFlag=2;
		OLED_ShowString(48,4,"2400,",16);
	}
	else if(datatemp[3] == 0x12C0)
	{
		OLED_Usart2_BandFlag=3;
		OLED_ShowString(48,4,"4800,",16);
	}
	else if(datatemp[3] == 0x2580)
	{
		OLED_Usart2_BandFlag=4;
		OLED_ShowString(48,4,"9600,",16);
	}
	else if(datatemp[3] == 0x4B00)
	{
		OLED_Usart2_BandFlag=5;
		OLED_ShowString(48,4,"19200,",16);
	}
	else if(datatemp[3] == 0x9600)
	{
		OLED_Usart2_BandFlag=6;
		OLED_ShowString(48,4,"38400,",16);
	}
	else if(datatemp[3] == 0xE100)
	{
		OLED_Usart2_BandFlag=7;
		OLED_ShowString(48,4,"57600,",16);
	}
	else if(datatemp[3] == 0xC200)
	{
		OLED_Usart2_BandFlag=8;
		OLED_ShowString(48,4,"115200,",16);
	}
			// usart3 ������
	if(datatemp[5] == 0x4B0)
	{
		OLED_Usart3_BandFlag=1;
		OLED_ShowString(48,6,"1200,",16);
	}
	else if(datatemp[5] == 0x960)
	{
		OLED_Usart3_BandFlag=2;
		OLED_ShowString(48,4,"2400,",16);
	}
	else if(datatemp[5] == 0x12C0)
	{
		OLED_Usart3_BandFlag=3;
		OLED_ShowString(48,6,"4800,",16);
	}
	else if(datatemp[5] == 0x2580)
	{
		OLED_Usart3_BandFlag=4;
		OLED_ShowString(48,6,"9600,",16);
	}
	else if(datatemp[5] == 0x4B00)
	{
		OLED_Usart3_BandFlag=5;
		OLED_ShowString(48,6,"19200,",16);
	}
	else if(datatemp[5] == 0x9600)
	{
		OLED_Usart3_BandFlag=6;
		OLED_ShowString(48,6,"38400,",16);
	}
	else if(datatemp[5] == 0xE100)
	{
		OLED_Usart3_BandFlag=7;
		OLED_ShowString(48,6,"57600,",16);
	}
	else if(datatemp[5] == 0xC200)
	{
		OLED_Usart3_BandFlag=8;
		OLED_ShowString(48,6,"115200,",16);
	}

	//usart1 ��żУ��
	if(datatemp[2] == 0x0000)
	{
		OLED_U1_ParityFlag=1;
		OLED_ShowString(100,2,"8N1",16);
	}
	else if(datatemp[2] == 0x0400)
	{
		OLED_U1_ParityFlag=2;
		OLED_ShowString(100,2,"8E1",16);
	}	
	else if(datatemp[2] == 0x0600)
	{
		OLED_U1_ParityFlag=3;
		OLED_ShowString(100,2,"8O1",16);
	}	
		//usart2 ��żУ��
	if(datatemp[4] == 0x0000)
	{
		OLED_U2_ParityFlag=1;
		OLED_ShowString(100,4,"8N1",16);
	}
	else if(datatemp[4] == 0x0400)
	{
		OLED_U2_ParityFlag=1;
		OLED_ShowString(100,4,"8E1",16);
	}	
	else if(datatemp[4] == 0x0600)
	{
		OLED_U2_ParityFlag=1;
		OLED_ShowString(100,4,"8O1",16);
	}	
		//usart3 ��żУ��
	if(datatemp[6] == 0x0000)
	{
		OLED_U3_ParityFlag=1;
		OLED_ShowString(100,6,"8N1",16);
	}
	else if(datatemp[6] == 0x0400)
	{
		OLED_U3_ParityFlag=1;
		OLED_ShowString(100,6,"8E1",16);
	}	
	else if(datatemp[6] == 0x0600)
	{
		OLED_U3_ParityFlag=1;
		OLED_ShowString(100,6,"8O1",16);
	}	


}
int main(void)
{ 

	System_Initializes();
	
	Connect_WintoRS485HUB();
	OLED_485HUB_Show();
	USART_FLASH_Set();
	STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)datatemp,10);
	switch(datatemp[7]) //�ж������ֹ���ģʽ
	{
		case 0x01:KEY_Flag = 1;break;//������ģʽ
		case 0x02:KEY_Flag = 2;break;//������ģʽ
		default:KEY_Flag = 1;  break;		
	}
	switch(datatemp[8])//��վ��ʱʱ��
	{
		case 0x0064:Time_out_Num= 10; break;
		case 0x00C8:Time_out_Num= 20; break;
		case 0x01F4:Time_out_Num= 50; break;
		case 0x03E8:Time_out_Num= 100;break;
		case 0x05DC:Time_out_Num= 150;break;
		case 0x07D0:Time_out_Num= 200;break;
		default:Time_out_Num= 100;    break;	
	}

						FLASH_BUFF[0]=0XC1;				
						FLASH_BUFF[1]=115200;
						FLASH_BUFF[2]=Usart1_parity_Buff;
						FLASH_BUFF[3]=Usart2_band_Buff;
						FLASH_BUFF[4]=Usart2_parity_Buff;
						FLASH_BUFF[5]=Usart3_band_Buff;
						FLASH_BUFF[6]=Usart3_parity_Buff;
						FLASH_BUFF[7]=KEY_Flag;
						FLASH_BUFF[8]=Time_Out_Buff;
						
						STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)FLASH_BUFF,10);	
	
	while(1)
	{
	//����ģʽ ������  �Ͷ�����
			if(KEY_Flag == 1)//������ 
			{
				USART1_USART23();
								
			}
			if(KEY_Flag == 2)//������
			{
				USART23_USART1();				
			}

	}
}

void Connect_WintoRS485HUB()
{

//#define	 Set_time  300
	u16 Con_flag_time=0;
	u8 Set_Ok_Flag = 1;
	u8 index = 0;
	u8 Connect_Flag = 0;
//�ϵ� ����֮������  �����˳�  ������ʱ����
//���� ������������   
//���� ���ӹ���ѡ�� KEY_Flag == 1 ������  KEY_Flag == 2 ������ 
//������д��FLASH ���������FLASH�ж�ȡ
	
	USART1_Init(9600,USART_Parity_No);//����1 ��Ϊ������λ�� ����ǿ�ƻָ�9600
	
	OLED_CLS();
	OLED_ShowString(32,0,"485HUB",16);
	OLED_ShowString(0,2,"Connecting ... ",16);
	
	while(1)
	{
			USART1_Receive_Data(U1_buf,&U1_RX_LEN);
			//setting
			if(U1_buf[0] == 0XC2)//��ȡ����
			{
				Connect_Flag=1;
	//				switch(U1_buf[1])
					//��д���ܶ�
					QT_ReadBuff[0] = 0xC0;				
					STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)datatemp,10);
							//usart1 ������
					if(datatemp[1] == 0XC200)
					{
						QT_ReadBuff[1] = 8;
					}
					for(index = 0;index<8;index++)//usart1 У��λ
					{
						if(datatemp[1] == Usart_band_talbe[index] )
						{
								index = index+1;//��1��ʼ��
								QT_ReadBuff[1] =  index;							
						}
						
					}
					for(index = 0;index<3;index++)//usart2 У��λ
					{
						if(datatemp[2] == Usart_parity_table[index] )
						{
								index = index+1;//��1��ʼ��
								QT_ReadBuff[2] =  index;							
						}
						
					}			
							//usart2 ������
					if(datatemp[3] == 0XC200)
					{
						QT_ReadBuff[3] = 8;
					}
					for(index = 0;index<8;index++)
					{
						if(datatemp[3] == Usart_band_talbe[index] )
						{
								index = index+1;//��1��ʼ��
								QT_ReadBuff[3] =  index;							
						}
						
					}
					for(index = 0;index<3;index++)//usart2 У��λ
					{
						if(datatemp[4] == Usart_parity_table[index] )
						{
								index = index+1;//��1��ʼ��
								QT_ReadBuff[4] =  index;							
						}
						
					}
							// usart3 ������
					if(datatemp[5] == 0XC200)
					{
						QT_ReadBuff[5] = 8;
					}
					for(index = 0;index<8;index++)
					{
						if(datatemp[5] == Usart_band_talbe[index] )
						{
								index = index+1;//��1��ʼ��
								QT_ReadBuff[5] =  index;							
						}
						
					}
					for(index = 0;index<3;index++)//usart3 У��λ
					{
						if(datatemp[6] == Usart_parity_table[index] )
						{
								index = index+1;//��1��ʼ��
								QT_ReadBuff[6] =  index;							
						}
						
					}			
					if(datatemp[7] == 0x01)	//������ģʽѡ��
					{
						QT_ReadBuff[7] =0x01;				
					}
					if(datatemp[7] == 0x02)//������ģʽѡ��
					{
						QT_ReadBuff[7] =0x02;	
					}
					for(index = 0;index<6;index++)//��ʱʱ��
					{
						if(datatemp[8] == Time_Out_talbe[index] )
						{
								index = index+1;//��1��ʼ��
								QT_ReadBuff[8] =  index;							
						}					
					}					
					USART1_Send_Data(QT_ReadBuff,9); //���͸���λ��		
					//��ʾ  ����OK
						//
					OLED_ShowString(32,0,"485HUB",16);
					OLED_ShowString(0,2,"Connected OK  ",16);			
					
					*U1_buf=0;					
				
			}else if(U1_buf[0] == 0XC1)//д�����
			{
				//
					Connect_Flag=1;
					switch(U1_buf[1])
					{
						case 0x01 :Usart1_band_Buff = Usart_band_talbe[0]; break; //1200
						case 0x02 :Usart1_band_Buff = Usart_band_talbe[1]; break; //2400 
						case 0x03 :Usart1_band_Buff = Usart_band_talbe[2]; break; //4800 
						case 0x04 :Usart1_band_Buff = Usart_band_talbe[3]; break; //9600  
						case 0x05 :Usart1_band_Buff = Usart_band_talbe[4]; break; //19200 
						case 0x06 :Usart1_band_Buff = Usart_band_talbe[5]; break; //38400 
						case 0x07 :Usart1_band_Buff = Usart_band_talbe[6]; break; //57600 
						case 0x08 :Usart1_band_Buff = Usart_band_talbe[7]; break; //115200	
						default:	Set_Ok_Flag = 0;break;						
					}
								//��żУ��
					switch(U1_buf[2])
					{
						case 0x01:Usart1_parity_Buff = Usart_parity_table[0]; break;
						case 0x02:Usart1_parity_Buff = Usart_parity_table[1]; break;
						case 0x03:Usart1_parity_Buff = Usart_parity_table[2]; break;		
						default:Set_Ok_Flag = 0;break;													
					}
					
					switch(U1_buf[3])
					{
						case 0x01 :Usart2_band_Buff = Usart_band_talbe[0]; break; //1200
						case 0x02 :Usart2_band_Buff = Usart_band_talbe[1]; break; //2400 
						case 0x03 :Usart2_band_Buff = Usart_band_talbe[2]; break; //4800 
						case 0x04 :Usart2_band_Buff = Usart_band_talbe[3]; break; //9600  
						case 0x05 :Usart2_band_Buff = Usart_band_talbe[4]; break; //19200 
						case 0x06 :Usart2_band_Buff = Usart_band_talbe[5]; break; //38400 
						case 0x07 :Usart2_band_Buff = Usart_band_talbe[6]; break; //57600 
						case 0x08 :Usart2_band_Buff = Usart_band_talbe[7]; break; //115200	
						default:	Set_Ok_Flag = 0;break;						
					}
								//��żУ��
					switch(U1_buf[4])
					{
						case 0x01:Usart2_parity_Buff = Usart_parity_table[0]; break;
						case 0x02:Usart2_parity_Buff = Usart_parity_table[1]; break;
						case 0x03:Usart2_parity_Buff = Usart_parity_table[2]; break;		
						default:Set_Ok_Flag = 0;break;													
					}				
					switch(U1_buf[5])
					{
						case 0x01 :Usart3_band_Buff = Usart_band_talbe[0]; break; //1200
						case 0x02 :Usart3_band_Buff = Usart_band_talbe[1]; break; //2400 
						case 0x03 :Usart3_band_Buff = Usart_band_talbe[2]; break; //4800 
						case 0x04 :Usart3_band_Buff = Usart_band_talbe[3]; break; //9600  
						case 0x05 :Usart3_band_Buff = Usart_band_talbe[4]; break; //19200 
						case 0x06 :Usart3_band_Buff = Usart_band_talbe[5]; break; //38400 
						case 0x07 :Usart3_band_Buff = Usart_band_talbe[6]; break; //57600 
						case 0x08 :Usart3_band_Buff = Usart_band_talbe[7]; break; //115200	
						default:	Set_Ok_Flag = 0;break;						
					}
								//��żУ��
					switch(U1_buf[6])
					{
						case 0x01:Usart3_parity_Buff = Usart_parity_table[0]; break;
						case 0x02:Usart3_parity_Buff = Usart_parity_table[1]; break;
						case 0x03:Usart3_parity_Buff = Usart_parity_table[2]; break;		
						default:Set_Ok_Flag = 0; break;													
					}
					//���ö�����ģʽ
					switch(U1_buf[7])
					{
						case 0x01:KEY_Flag = 1; break;	
						case 0x02:KEY_Flag = 2; break;	
						default:Set_Ok_Flag = 0; break;	
					}
					switch(U1_buf[8])// ��վ ��ʱʱ��
					{
						case 0x01:Time_Out_Buff = Time_Out_talbe[0];break;	
						case 0x02:Time_Out_Buff = Time_Out_talbe[1];break;	
						case 0x03:Time_Out_Buff = Time_Out_talbe[2];break;	
						case 0x04:Time_Out_Buff = Time_Out_talbe[3];break;	
						case 0x05:Time_Out_Buff = Time_Out_talbe[4];break;	
						case 0x06:Time_Out_Buff = Time_Out_talbe[5];break;	
						default:Set_Ok_Flag = 0; break;	
					}
					if(Set_Ok_Flag == 1)
					{
						//���������ɹ���д��FLASH��
						FLASH_BUFF[0]=0XC1;				
						FLASH_BUFF[1]=Usart1_band_Buff;
						FLASH_BUFF[2]=Usart1_parity_Buff;
						FLASH_BUFF[3]=Usart2_band_Buff;
						FLASH_BUFF[4]=Usart2_parity_Buff;
						FLASH_BUFF[5]=Usart3_band_Buff;
						FLASH_BUFF[6]=Usart3_parity_Buff;
						FLASH_BUFF[7]=KEY_Flag;
						FLASH_BUFF[8]=Time_Out_Buff;
						
						STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)FLASH_BUFF,10);	
						U1_buf[0] = 0XC0;
						USART1_Send_Data(U1_buf,9); //���͸���λ��	
						OLED_ShowString(32,0,"485HUB",16);
						OLED_ShowString(0,2,"Set OK  ",16);
						break;
					}				
			}			
			else if(Connect_Flag == 0)
			{
					delay_ms(100); 
					Con_flag_time++;
					if(Con_flag_time >= 40)//����֮��û������ �Զ��˳�
					{
						Con_flag_time=0; 
						break;		
					}			
			}


	}		
}
void USART1_USART23() //������ ��-��
{
//	delay_ms(10);
//������1���������η��͵�����2 �ʹ���3 
	*U1_buf=0;	
	USART1_Receive_Data(U1_buf,&U1_RX_LEN);//�յ���λ������
//	OLED_ShowString(48,2,"Receive Data...",16);	
	if(U1_RX_LEN)
	{
		OLED_ShowString(48,4,"Send Data.",16);	
		OLED_ShowString(48,6,"Send Data.",16);	
		USART1_Send_Data(U1_buf,U1_RX_LEN);
		USART2_Send_Data(U1_buf,U1_RX_LEN);
		USART3_Send_Data(U1_buf,U1_RX_LEN);		
		*U1_buf=0;	

		switch(OLED_Usart2_BandFlag)
		{
			case 1:OLED_ShowString(48,4,"1200,  ",16);  break;
			case 2:OLED_ShowString(48,4,"2400,  ",16);  break;
			case 3:OLED_ShowString(48,4,"4800,  ",16);  break;
			case 4:OLED_ShowString(48,4,"9600,  ",16);  break;
			case 5:OLED_ShowString(48,4,"19200, ",16); break;
			case 6:OLED_ShowString(48,4,"38400, ",16); break;
			case 7:OLED_ShowString(48,4,"57600, ",16); break;
			case 8:OLED_ShowString(48,4,"115200,",16);break;
			default:break;
		}
		switch(OLED_U2_ParityFlag)
		{
			case 1:OLED_ShowString(100,4,"8N1",16);  break;
			case 2:OLED_ShowString(100,4,"8E1",16);  break;
			case 3:OLED_ShowString(100,4,"8O1",16);  break;
			default:break;
		}
		
		switch(OLED_Usart3_BandFlag)
		{
			case 1:OLED_ShowString(48,6,"1200,  ",16);  break;
			case 2:OLED_ShowString(48,6,"2400,  ",16);  break;
			case 3:OLED_ShowString(48,6,"4800,  ",16);  break;
			case 4:OLED_ShowString(48,6,"9600,  ",16);  break;
			case 5:OLED_ShowString(48,6,"19200, ",16); break;
			case 6:OLED_ShowString(48,6,"38400, ",16); break;
			case 7:OLED_ShowString(48,6,"57600, ",16); break;
			case 8:OLED_ShowString(48,6,"115200,",16);break;
			default:break;
		}
		switch(OLED_U3_ParityFlag)
		{
			case 1:OLED_ShowString(100,6,"8N1",16);  break;
			case 2:OLED_ShowString(100,6,"8E1",16);  break;
			case 3:OLED_ShowString(100,6,"8O1",16);  break;
			default:break;
		}
	}
	*U2_buf=0;
	USART2_Receive_Data(U2_buf,&U2_RX_LEN);
	if(U2_RX_LEN)
	{
		OLED_ShowString(48,2,"Send Data.",16);	
		USART1_Send_Data(U2_buf,U2_RX_LEN);  
		*U2_buf=0;		
		switch(OLED_Usart1_BandFlag)
		{
			case 1:OLED_ShowString(48,2,"1200,  ",16); break;
			case 2:OLED_ShowString(48,2,"2400,  ",16); break;
			case 3:OLED_ShowString(48,2,"4800,  ",16); break;
			case 4:OLED_ShowString(48,2,"9600,  ",16); break;
			case 5:OLED_ShowString(48,2,"19200, ",16); break;
			case 6:OLED_ShowString(48,2,"38400, ",16); break;
			case 7:OLED_ShowString(48,2,"57600, ",16); break;
			case 8:OLED_ShowString(48,2,"115200,",16); break;
			default:break;
		}
		switch(OLED_U1_ParityFlag)
		{
			case 1:OLED_ShowString(100,2,"8N1",16);  break;
			case 2:OLED_ShowString(100,2,"8E1",16);  break;
			case 3:OLED_ShowString(100,2,"8O1",16);  break;
			default:break;
		}
	}       
	*U3_buf=0;
	//delay_ms(10);
	USART3_Receive_Data(U3_buf,&U3_RX_LEN);	
	if(U3_RX_LEN)
	{
		u16 p[5] = {41,42,43,44,45};
		OLED_ShowString(48,2,"Send Data.",16);	
		USART1_Send_Data(U3_buf,U3_RX_LEN);
		USART2_Send_Data(U3_buf,U3_RX_LEN);
		USART3_Send_Data(U3_buf,U3_RX_LEN);	
		*U3_buf=0;	
		switch(OLED_Usart1_BandFlag)
		{
			case 1:OLED_ShowString(48,2,"1200,  ",16); break;
			case 2:OLED_ShowString(48,2,"2400,  ",16); break;
			case 3:OLED_ShowString(48,2,"4800,  ",16); break;
			case 4:OLED_ShowString(48,2,"9600,  ",16); break;
			case 5:OLED_ShowString(48,2,"19200, ",16); break;
			case 6:OLED_ShowString(48,2,"38400, ",16); break;
			case 7:OLED_ShowString(48,2,"57600, ",16); break;
			case 8:OLED_ShowString(48,2,"115200,",16); break;
			default:break;
		}
		switch(OLED_U1_ParityFlag)
		{
			case 1:OLED_ShowString(100,2,"8N1",16);  break;
			case 2:OLED_ShowString(100,2,"8E1",16);  break;
			case 3:OLED_ShowString(100,2,"8O1",16);  break;
			default:break;
		}

	}

}
void	USART23_USART1() //������ ��-��
{
	u8 Flag_Time=0;

	/*
	˼·
	 
	�ȼ�¼ u2 u3 �������ڳ�ʱʱ���ڣ�������ʱ���� ���������ŷ��ص�����2 ����3
	*/
	USART2_Receive_Data(U2_buf,&U2_RX_LEN);//���յ���λ��ָ��
	if(U2_RX_LEN)
	{
		OLED_ShowString(48,2,"Send Data.",16);	
		USART1_Send_Data(U2_buf,U2_RX_LEN);		// usart2->usart1 �·�ָ��
		*U2_buf=0;	
				switch(OLED_Usart1_BandFlag)
		{
			case 1:OLED_ShowString(48,2,"1200,  ",16); break;
			case 2:OLED_ShowString(48,2,"2400,  ",16); break;
			case 3:OLED_ShowString(48,2,"4800,  ",16); break;
			case 4:OLED_ShowString(48,2,"9600,  ",16); break;
			case 5:OLED_ShowString(48,2,"19200, ",16); break;
			case 6:OLED_ShowString(48,2,"38400, ",16); break;
			case 7:OLED_ShowString(48,2,"57600, ",16); break;
			case 8:OLED_ShowString(48,2,"115200,",16); break;
			default:break;
		}
		switch(OLED_U1_ParityFlag)
		{
			case 1:OLED_ShowString(100,2,"8N1",16);  break;
			case 2:OLED_ShowString(100,2,"8E1",16);  break;
			case 3:OLED_ShowString(100,2,"8O1",16);  break;
			default:break;
		}
		while(1)
		{
			USART1_Receive_Data(U1_buf,&U1_RX_LEN);
			
			if(U1_RX_LEN)// �յ��ⲿ�豸�ش�����
			{
				OLED_ShowString(48,4,"Send Data.",16);	
				USART2_Send_Data(U1_buf,U1_RX_LEN);//ת������λ��
				*U1_buf=0;	
				switch(OLED_Usart2_BandFlag)
				{
					case 1:OLED_ShowString(48,4,"1200,  ",16); break;
					case 2:OLED_ShowString(48,4,"2400,  ",16); break;
					case 3:OLED_ShowString(48,4,"4800,  ",16); break;
					case 4:OLED_ShowString(48,4,"9600,  ",16); break;
					case 5:OLED_ShowString(48,4,"19200, ",16); break;
					case 6:OLED_ShowString(48,4,"38400, ",16); break;
					case 7:OLED_ShowString(48,4,"57600, ",16); break;
					case 8:OLED_ShowString(48,4,"115200,",16); break;
					default:break;
				}
				switch(OLED_U2_ParityFlag)
				{
					case 1:OLED_ShowString(100,4,"8N1",16);  break;
					case 2:OLED_ShowString(100,4,"8E1",16);  break;
					case 3:OLED_ShowString(100,4,"8O1",16);  break;
					default:break;
				}
					break;			
			}
			Flag_Time++;
			if(Flag_Time >= Time_out_Num)//1�� ��ʱ��û���յ����� �˳�
			{
				Flag_Time=0;
				break;			
			}							
		}		
	}
	USART3_Receive_Data(U3_buf,&U3_RX_LEN);//���յ���λ��ָ��
	if(U3_RX_LEN)
	{
		OLED_ShowString(48,2,"Send Data.",16);	
		USART1_Send_Data(U3_buf,U3_RX_LEN);		// usart3->usart1 �·�ָ��
		*U3_buf=0;	
		switch(OLED_Usart1_BandFlag)
		{
			case 1:OLED_ShowString(48,2,"1200,  ",16); break;
			case 2:OLED_ShowString(48,2,"2400,  ",16); break;
			case 3:OLED_ShowString(48,2,"4800,  ",16); break;
			case 4:OLED_ShowString(48,2,"9600,  ",16); break;
			case 5:OLED_ShowString(48,2,"19200, ",16); break;
			case 6:OLED_ShowString(48,2,"38400, ",16); break;
			case 7:OLED_ShowString(48,2,"57600, ",16); break;
			case 8:OLED_ShowString(48,2,"115200,",16); break;
			default:break;
		}
		switch(OLED_U1_ParityFlag)
		{
			case 1:OLED_ShowString(100,2,"8N1",16);  break;
			case 2:OLED_ShowString(100,2,"8E1",16);  break;
			case 3:OLED_ShowString(100,2,"8O1",16);  break;
			default:break;
		}
		while(1)
		{
			USART1_Receive_Data(U1_buf,&U1_RX_LEN);
			
			if(U1_RX_LEN)// �յ��ⲿ�豸�ش�����
			{//
				OLED_ShowString(48,6,"Send Data.",16);
				USART3_Send_Data(U1_buf,U1_RX_LEN);//ת������λ��
				*U1_buf=0;	
				switch(OLED_Usart3_BandFlag)
				{
					case 1:OLED_ShowString(48,6,"1200,  ",16); break;
					case 2:OLED_ShowString(48,6,"2400,  ",16); break;
					case 3:OLED_ShowString(48,6,"4800,  ",16); break;
					case 4:OLED_ShowString(48,6,"9600,  ",16); break;
					case 5:OLED_ShowString(48,6,"19200, ",16); break;
					case 6:OLED_ShowString(48,6,"38400, ",16); break;
					case 7:OLED_ShowString(48,6,"57600, ",16); break;
					case 8:OLED_ShowString(48,6,"115200,",16); break;
					default:break;
				}
				switch(OLED_U3_ParityFlag)
				{
					case 1:OLED_ShowString(100,6,"8N1",16);  break;
					case 2:OLED_ShowString(100,6,"8E1",16);  break;
					case 3:OLED_ShowString(100,6,"8O1",16);  break;
					default:break;
				}
				break;			
			}	
			Flag_Time++;
			if(Flag_Time >= Time_out_Num)//1�� ��ʱ��û���յ����� �˳�
			{
				Flag_Time=0;
				break;			
			}							
		}		
	}	
	//
}