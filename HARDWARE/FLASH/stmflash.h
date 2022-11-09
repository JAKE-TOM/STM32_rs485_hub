#ifndef __STMFLASH_H__
#define __STMFLASH_H__

 #include "stdint.h"
extern uint16_t datatemp[10];          //Flash相关数组
//用户根据自己的需要设置
#define STM32_FLASH_SIZE 64 	 		//所选STM32的FLASH容量大小(单位为K)
#define STM32_FLASH_WREN 1              //使能FLASH写入(0，不是能;1，使能)

//FLASH起始地址
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH的起始地址
#define FLASH_SAVE_ADDR  0X0800E000 	//设置FLASH 保存地址(必须为偶数，且其值要大于本代码所占用FLASH的大小+0X08000000)
 
 //FLASH解锁键值
#define FLASH_KEY1               0X45670123
#define FLASH_KEY2               0XCDEF89AB

void STMFLASH_Unlock(void);					  	//FLASH解锁
void STMFLASH_Lock(void);					  	//FLASH上锁
uint8_t STMFLASH_GetStatus(void);				  	//获得状态
uint8_t STMFLASH_WaitDone(uint16_t time);				  	//等待操作结束
uint8_t STMFLASH_ErasePage(uint32_t paddr);			  	//擦除页
uint8_t STMFLASH_WriteHalfWord(uint32_t faddr, uint16_t dat);	//写入半字
uint16_t STMFLASH_ReadHalfWord(uint32_t faddr);		    //读出半字  
void STMFLASH_WriteLenByte(uint32_t WriteAddr,uint32_t DataToWrite,uint16_t Len);	//指定地址开始写入指定长度的数据
uint32_t STMFLASH_ReadLenByte(uint32_t ReadAddr,uint16_t Len);						//指定地址开始读取指定长度数据
void STMFLASH_Write(uint32_t WriteAddr,uint16_t *pBuffer,uint16_t NumToWrite);		//从指定地址开始写入指定长度的数据
void STMFLASH_Read(uint32_t ReadAddr,uint16_t *pBuffer,uint16_t NumToRead);   		//从指定地址开始读出指定长度的数据


void Test_Write(uint32_t WriteAddr,uint16_t WriteData);//测试写入
void Flash_Read(void);	//读取Flash数据
void Flash_Write(void);	//写入Flash数据

#endif


