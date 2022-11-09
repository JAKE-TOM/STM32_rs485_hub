#ifndef _oled_H
#define _oled_H
#include "stm32f10x.h"
#include "stm32f10x_it.h"

#define SYS_CLOCK_FREQ_HZ   72000000
#define OLEDIIC
/********************************
	定义
	OledCS  ：CS--->PB12
	OledRS  : DC--->PB14
	OledRst ：RES-->PB13,可以直接接单片机RST引脚
	OledSck : D0--->PB0
	OledSdin: D1--->PB1
	

******************************************/
#ifdef  OLEDIIC  
#define  I2CSCLPort         GPIOB
#define  I2CSDAPort         GPIOB
#define  I2CSCLPin          GPIO_Pin_6
#define  I2CSDAPin          GPIO_Pin_7
#else

//OLED模式设置
//0: SPI串行模式  （模块的BS1，BS2均接GND）
//1: 并行8080模式 （模块的BS1，BS2均接VCC）
#define  OLED_MODE 	        0
//SPI模式设置
//0: 4线串行模式 8 bit data send
//1: 3线串行模式 9 bit data send
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

#define  DATAOUT(x)         GPIO_Write(GPIOA,x);//输出  //PA0~7,作为数据线

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

#define  OLED_ADDRESS       0x78    //IIC模式下的地址,需要结合硬件
#define  OLED_CMD           0x00	//写命令
#define  OLED_DATA          0x01	//写数据

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



