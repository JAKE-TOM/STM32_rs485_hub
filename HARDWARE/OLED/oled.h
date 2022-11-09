#ifndef _oled_H
#define _oled_H
#include "stm32f10x.h"
#include "stm32f10x_it.h"

#define SYS_CLOCK_FREQ_HZ   72000000
#define OLEDIIC
/********************************
	����
	OledCS  ��CS--->PB12
	OledRS  : DC--->PB14
	OledRst ��RES-->PB13,����ֱ�ӽӵ�Ƭ��RST����
	OledSck : D0--->PB0
	OledSdin: D1--->PB1
	

******************************************/
#ifdef  OLEDIIC  
#define  I2CSCLPort         GPIOB
#define  I2CSDAPort         GPIOB
#define  I2CSCLPin          GPIO_Pin_6
#define  I2CSDAPin          GPIO_Pin_7
#else

//OLEDģʽ����
//0: SPI����ģʽ  ��ģ���BS1��BS2����GND��
//1: ����8080ģʽ ��ģ���BS1��BS2����VCC��
#define  OLED_MODE 	        0
//SPIģʽ����
//0: 4�ߴ���ģʽ 8 bit data send
//1: 3�ߴ���ģʽ 9 bit data send
#define  OLED_SPI_X         0

#define  OledCsPort         GPIOB
#define  OledCsPin          GPIO_Pin_15

#define  OledRstPort        GPIOB
#define  OledRstPin         GPIO_Pin_13

#define  OledRSPort         GPIOB
#define  OledRSPin          GPIO_Pin_14

#define  OledCs_H()         OledCsPort->BSRR = OledCsPin
#define  OledCs_L()         OledCsPort->BRR  = OledCsPin  

#define  OledRst_H()        OledRstPort->BSRR = OledRstPin
#define  OledRst_L()        OledRstPort->BRR  = OledRstPin  

#if OLED_MODE ==1
#define  OledWRPort         GPIOB
#define  OledWRPin          GPIO_Pin_15

#define  OledRDPort         GPIOB
#define  OledRDPin          GPIO_Pin_11

#define  OledWR_H()         OledWRPort->BSRR = OledWRPin
#define  OledWR_L()         OledWRPort->BRR  = OledWRPin

#define  OledRD_H()         OledRDPort->BSRR = OledRDPin
#define  OledRD_L()         OledRDPort->BRR  = OledRDPin

#define  DATAOUT(x)         GPIO_Write(GPIOA,x);//���  //PA0~7,��Ϊ������

#else
#define  OledSckPort        GPIOB
#define  OledSckPin         GPIO_Pin_0

#define  OledSdinPort       GPIOB
#define  OledSdinPin        GPIO_Pin_1

#define  OledSck_H()        OledSckPort->BSRR = OledSckPin
#define  OledSck_L()        OledSckPort->BRR  = OledSckPin

#define  OledSdin_H()       OledSdinPort->BSRR = OledSdinPin
#define  OledSdin_L()       OledSdinPort->BRR  = OledSdinPin
#endif

#define  OledRS_H()         OledRSPort->BSRR = OledRSPin
#define  OledRS_L()         OledRSPort->BRR  = OledRSPin

#endif

#define  LedPort            GPIOC
#define  LedPIN             GPIO_Pin_13

#define  LedState(State)    State?GPIO_SetBits(LedPort,LedPIN):GPIO_ResetBits(LedPort,LedPIN);
#define  LedXor(PIN)        LedPort->ODR ^= PIN

#define  OLED_ADDRESS       0x78    //IICģʽ�µĵ�ַ,��Ҫ���Ӳ��
#define  OLED_CMD           0x00	//д����
#define  OLED_DATA          0x01	//д����

void RCC_Configuration(void);
//void delay_ms(u16 time);
//void delay_us(u16 time);

void OLED_ON(void);
void OLED_CLS(void);
void OLED_OFF(void);
void OLED_Init(void);
void WriteDat(u8 I2C_Data);
void OLED_SetPos(u8 x, u8 y);
void OLED_Fill(u8 fill_Data);
void I2C_Configuration(void);
void WriteCmd(u8 I2C_Command);
void OLED_ShowCN(u8 x, u8 y, u8 N);
void OLED_ShowCHinese(u8 x,u8 y,u8 no);
void I2C_WriteByte(uint8_t addr,uint8_t data);
void OLED_ShowStr(u8 x, u8 y, u8 ch[], u8 TextSize);
void OLED_DrawBMP(u8 x0,u8 y0,u8 x1,u8 y1,u8 BMP[]);
void OLED_ShowString(u8 x,u8 y,char *chr,u8 Char_Size);

#endif



