#include "oled.h" 
#include "codetab.h"
//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 
#ifdef OLEDIIC 

#define  OledSCL_H()        I2CSCLPort->BSRR = I2CSCLPin  
#define  OledSCL_L()        I2CSCLPort->BRR  = I2CSCLPin  
#define  OledSDA_H()        I2CSDAPort->BSRR = I2CSDAPin  
#define  OledSDA_L()        I2CSDAPort->BRR  = I2CSDAPin 

#define  OledSDA_Read       I2CSDAPort->IDR  & I2CSDAPin  

/**
**  设置SDA为输出
**/
void SDA_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructer;
    GPIO_InitStructer.GPIO_Pin= I2CSDAPin;
    GPIO_InitStructer.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_InitStructer.GPIO_Mode=GPIO_Mode_Out_PP;
    GPIO_Init(I2CSDAPort, &GPIO_InitStructer);
}


/**
**  设置SDA为输入
**/
void SDA_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStructer;
    GPIO_InitStructer.GPIO_Pin= I2CSDAPin;
    GPIO_InitStructer.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_InitStructer.GPIO_Mode=GPIO_Mode_IPU;
    GPIO_Init(I2CSDAPort, &GPIO_InitStructer);
}

//开始信号
void IIC_Start(void)
{
    SDA_OUT();
    OledSDA_H();
    OledSCL_H();
    delay_us(2);
    OledSDA_L();
    delay_us(2);
    OledSCL_L();
    delay_us(2);
}

void IIC_Stop(void)
{
    OledSCL_H();
    OledSDA_L();
    delay_us(2);
    OledSDA_H();
    delay_us(2);
}


/*
*   返回1--应答出错
*   放回0--应答正确
*/
u8 IIC_Wait_Ask(void)
{
    int count=0;
    SDA_IN();
    OledSCL_H();
    delay_us(2);
    while(OledSDA_Read)
    {
        count++;
        if(count>250)
        {
            IIC_Stop();
            return 1;
        }   
    }
    OledSCL_L();
    delay_us(2);
    return 0;
}

//写一个字节
void IIC_WriteByte(u8 data)
{
    u8 i;
    SDA_OUT();
    for(i=0;i<8;i++)
    {
        OledSCL_L();
        delay_us(2);
        if(data & 0x80)     //MSB,从高位开始一位一位传输
            OledSDA_H();
        else
            OledSDA_L();
        OledSCL_H();
        delay_us(2);
        OledSCL_L();
        data<<=1;

    }
}

u8 IIC_ReadByte(void)
{
    u8 data,i;
    OledSDA_H();
    delay_us(3);
    for(i=0;i<8;i++)
    {
        data<<=1;
        OledSCL_L();
        delay_us(1);
        OledSCL_H();
        delay_us(1);
        if(GPIO_ReadInputDataBit(I2CSDAPort, I2CSDAPin))
            data=data | 0x01;
        else 
            data=data & 0xFE;

    }
    OledSCL_L();
    delay_us(3);
    return data;

}
 
void WriteCmd(u8 command)
{
    IIC_Start();
    IIC_WriteByte(0x78);//OLED地址
    IIC_Wait_Ask();
    IIC_WriteByte(0x00);//寄存器地址
    IIC_Wait_Ask();
    IIC_WriteByte(command);
    IIC_Wait_Ask();
    IIC_Stop();
}

void WriteDat(u8 data)
{
    IIC_Start();
    IIC_WriteByte(0x78);//OLED地址
    IIC_Wait_Ask();
    IIC_WriteByte(0x40);//寄存器地址
    IIC_Wait_Ask();
    IIC_WriteByte(data);
    IIC_Wait_Ask();
    IIC_Stop();
}

#else
#if OLED_MODE==1	//8080并口 
/*****************************************************************
    向SSD1306写入一个字节
    dat:要写入的数据/命令
    cmd:数据/命令标志 0,表示命令;1,表示数据
*****************************************************************/
void OLED_WR_Byte(u8 dat,u8 cmd)
{
	DATAOUT(dat);	    
    if(cmd)
        OledRS_H();
    else
        OledRS_L();
	OledCs_L();	   
	OledWR_L(); 
    OledWR_H();    
	OledCs_H();	  
	OledRS_H();	 
} 	    	    
#else
void OLED_WR_Byte(u8 dat,u8 cmd)
{
    char i;
    OledCs_L();
#if OLED_SPI_X == 1
    OledSck_L();
    if(cmd)OledSdin_H();
    else OledSdin_L();
    OledSck_H();
#else
    if(cmd)
        OledRS_H();
    else
        OledRS_L();
#endif
	for(i=0;i<8;i++)
	{			  
		OledSck_L();
		if(dat&0x80)OledSdin_H();
		else OledSdin_L();
		OledSck_H();
		dat<<=1;   
	}    
    OledCs_H();
#if OLED_SPI_X == 0
    OledRS_H();
#endif
     
}
#endif
void WriteCmd(u8 command)
{
	OLED_WR_Byte(command,OLED_CMD);
}
void WriteDat(u8 data)
{
	OLED_WR_Byte(data,OLED_DATA);
}
#endif

// 公共部分

/*****************************************************************
    将OLED从休眠中唤醒
*****************************************************************/
void OLED_ON(void)
{
    WriteCmd(0X8D);  //设置电荷泵
    WriteCmd(0X14);  //开启电荷泵
    WriteCmd(0XAF);  //OLED唤醒
}

/*****************************************************************
  设置光标
  x,光标x位置 0-127
  y,光标y位置 0-7
*****************************************************************/
void OLED_SetPos(u8 x, u8 y)
{ 
    WriteCmd(0xb0+y);
    WriteCmd(((x&0xf0)>>4)|0x10);
    WriteCmd((x&0x0f)|0x01);
}

/*****************************************************************
  填充整个屏幕
  fill_Data:要填充的数据
*****************************************************************/
void OLED_Fill(u8 fill_Data)
{
    u8 m,n;
    for(m=0;m<8;m++)
    {
        WriteCmd(0xb0+m);       //page0-page1
        WriteCmd(0x00);     //low column start address
        WriteCmd(0x10);     //high column start address
        for(n=0;n<128;n++)
        {
            WriteDat(fill_Data);
        }
    }
}

void OLED_CLS(void)//清屏
{
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		WriteCmd (0xb0+i);
		WriteCmd (0x00);
		WriteCmd (0x10);   
		for(n=0;n<128;n++)
        WriteDat(0); 
	}
}

/*****************************************************************
  显示codetab.h中的ASCII字符,有6*8和8*16可选择
  x,y : 起始点坐标(x:0~127, y:0~7);
                     ch[] :- 要显示的字符串; 
                     TextSize : 字符大小(1:6*8 ; 2:8*16)
*****************************************************************/
void OLED_ShowStr(u8 x, u8 y, u8 ch[], u8 TextSize)
{
    u8 c = 0,i = 0,j = 0;
    switch(TextSize)
    {
        case 1:
        {
            while(ch[j] != '\0')
            {
                c = ch[j] - 32;
                if(x > 126)
                {
                    x = 0;
                    y++;
                }
                OLED_SetPos(x,y);
                for(i=0;i<6;i++)
                    WriteDat(F6x8[c][i]);
                x += 6;
                j++;
            }
        }break;
        case 2:
        {
            while(ch[j] != '\0')
            {
                c = ch[j] - 32;
                if(x > 120)
                {
                    x = 0;
                    y++;
                }
                OLED_SetPos(x,y);
                for(i=0;i<8;i++)
                    WriteDat(F8X16[c*16+i]);
                OLED_SetPos(x,y+1);
                for(i=0;i<8;i++)
                    WriteDat(F8X16[c*16+i+8]);
                x += 8;
                j++;
            }
        }break;
    }
}

void OLED_ShowChar(u8 x,u8 y,char chr,u8 Char_Size)
{      	
	u8 c=0,i=0;	
	c=chr-' ';		
	if(x>128-1){x=0;y=y+2;}
	if(Char_Size ==16)
	{
		OLED_SetPos(x,y);	
		for(i=0;i<8;i++)
		WriteDat(F8X16[c*16+i]);
		OLED_SetPos(x,y+1);
		for(i=0;i<8;i++)
		WriteDat(F8X16[c*16+i+8]);
	}
	else 
	{	
		OLED_SetPos(x,y);
		for(i=0;i<6;i++)
		{
			WriteDat(F6x8[c][i]);
		}
	}
}
/*****************************************************************
    显示字符串
    示例:OLED_ShowString(0,0,"4Wire SPI Runing",16);
*****************************************************************/
void OLED_ShowString(u8 x,u8 y,char *chr,u8 Char_Size)
{
	u8 j=0;
	while (chr[j]!='\0')
	{
		OLED_ShowChar(x,y,chr[j],Char_Size);
		x+=8;
		if(x>120){x=0;y+=2;}
		{
			j++;
		}
	}
}
/*****************************************************************
    显示中文
    no  :中文数组Hzk[] 编号
    示例:OLED_ShowCHinese(32,4,2);
*****************************************************************/
void OLED_ShowCHinese(u8 x,u8 y,u8 no)
{      			    
	u8 t,adder=0;
	OLED_SetPos(x,y);	
    for(t=0;t<16;t++)
    {
        WriteDat(Hzk[2*no][t]);
        adder+=1;
     }	
    OLED_SetPos(x,y+1);	
    for(t=0;t<16;t++)
    {	
        WriteDat(Hzk[2*no+1][t]);
        adder+=1;
    }					
}
/*****************************************************************
    OLED初始化
*****************************************************************/
void OLED_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    delay_ms(100);
    
#ifdef OLEDIIC 
    
    GPIO_InitStructure.GPIO_Pin=I2CSCLPin | I2CSDAPin;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;

    GPIO_Init(I2CSCLPort, &GPIO_InitStructure);
#else    
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = OledCsPin;
 	GPIO_Init(OledCsPort, &GPIO_InitStructure);
    GPIO_SetBits(OledCsPort,OledCsPin);
    
    GPIO_InitStructure.GPIO_Pin = OledRstPin;
 	GPIO_Init(OledRstPort, &GPIO_InitStructure);
    GPIO_SetBits(OledRstPort,OledRstPin);
    
    GPIO_InitStructure.GPIO_Pin =OledRSPin ;
 	GPIO_Init(OledRSPort, &GPIO_InitStructure);
    GPIO_SetBits(OledRSPort,OledRSPin);
    
#if OLED_MODE==1
 
 	GPIO_InitStructure.GPIO_Pin =0xFF; //PA0~7 OUT推挽输出
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOA,0xFF); //PA0~7输出高
	
	GPIO_InitStructure.GPIO_Pin = OledWRPin;
 	GPIO_Init(OledWRPort, &GPIO_InitStructure);
    GPIO_SetBits(OledWRPort,OledWRPin);

    GPIO_InitStructure.GPIO_Pin = OledRDPin;
 	GPIO_Init(OledRDPort, &GPIO_InitStructure);
    GPIO_SetBits(OledRDPort,OledRDPin);

#else
 	GPIO_InitStructure.GPIO_Pin = OledSckPin;
 	GPIO_Init(OledSckPort, &GPIO_InitStructure);
 	GPIO_SetBits(OledSckPort,OledSckPin);

 	GPIO_InitStructure.GPIO_Pin = OledSdinPin;
 	GPIO_Init(OledSdinPort, &GPIO_InitStructure);
 	GPIO_SetBits(OledSdinPort,OledSdinPin);

#endif
  							  
	OledCs_H();
    OledRS_H();
	OledRst_L();
	delay_ms(100);
	OledRst_H();
#endif    
    
    WriteCmd(0xAE); //display off
    WriteCmd(0x20); //Set Memory Addressing Mode    
    WriteCmd(0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
    WriteCmd(0xb0); //Set Page Start Address for Page Addressing Mode,0-7
    WriteCmd(0xc8); //Set COM Output Scan Direction
    WriteCmd(0x00); //---set low column address
    WriteCmd(0x10); //---set high column address
    WriteCmd(0x40); //--set start line address
    WriteCmd(0x81); //--set contrast control register
    WriteCmd(0xdf); //亮度调节 0x00~0xff
    WriteCmd(0xa1); //--set segment re-map 0 to 127
    WriteCmd(0xa6); //--set normal display
    WriteCmd(0xa8); //--set multiplex ratio(1 to 64)
    WriteCmd(0x3F); //
    WriteCmd(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    WriteCmd(0xd3); //-set display offset
    WriteCmd(0x00); //-not offset
    WriteCmd(0xd5); //--set display clock divide ratio/oscillator frequency
    WriteCmd(0xf0); //--set divide ratio
    WriteCmd(0xd9); //--set pre-charge period
    WriteCmd(0x22); //
    WriteCmd(0xda); //--set com pins hardware configuration
    WriteCmd(0x12);
    WriteCmd(0xdb); //--set vcomh
    WriteCmd(0x20); //0x20,0.77xVcc
    WriteCmd(0x8d); //--set DC-DC enable
    WriteCmd(0x14); //
    WriteCmd(0xaf); //--turn on oled panel
    OLED_CLS();
}

