//gcc -Wall -o motor motor.c -lwiringPi//
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdlib.h>
#include <stdint.h>
#include <iconv.h> 
#include "codetab.h"
 
#define I2C_ADDR 0x3c //显示屏控制线
#define XLevelL			0x00
#define XLevelH			0x10
#define XLevel	    ((XLevelH&0x0F)*16+XLevelL)
#define Max_Column	128
#define Max_Row			64
#define	Brightness	0xCF 
#define X_WIDTH 		128
#define Y_WIDTH 		64



int i2cHand;
unsigned char DataBuffer[8][128];

void WriteCmd(int fd,unsigned char I2C_Command)//写命令
{
	wiringPiI2CWriteReg8(fd,0x00, I2C_Command);
}

void WriteData(int fd,unsigned char I2C_Data)//写数据
{
	wiringPiI2CWriteReg8(fd,0x40, I2C_Data);
}
 
 
 
void OLED_Init(int fd)
{
 
	WriteCmd(fd,0xAE); //display off
	WriteCmd(fd, 0x20);	//Set Memory Addressing Mode	
	WriteCmd(fd, 0x10);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	WriteCmd(fd, 0xb0);	//Set Page Start Address for Page Addressing Mode,0-7
	WriteCmd(fd, 0xc8);	//Set COM Output Scan Direction
	WriteCmd(fd, 0x00); //---set low column address
	WriteCmd(fd, 0x10); //---set high column address
	WriteCmd(fd, 0x40); //--set start line address
	WriteCmd(fd, 0x81); //--set contrast control register
	WriteCmd(fd, 0xff); //亮度调节 0x00~0xff
	WriteCmd(fd, 0xa1); //--set segment re-map 0 to 127
	WriteCmd(fd, 0xa6); //--set normal display
	WriteCmd(fd, 0xa8); //--set multiplex ratio(1 to 64)
	WriteCmd(fd, 0x3F); //
	WriteCmd(fd, 0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	WriteCmd(fd, 0xd3); //-set display offset
	WriteCmd(fd, 0x00); //-not offset
	WriteCmd(fd, 0xd5); //--set display clock divide ratio/oscillator frequency
	WriteCmd(fd, 0xf0); //--set divide ratio
	WriteCmd(fd, 0xd9); //--set pre-charge period
	WriteCmd(fd, 0x22); //
	WriteCmd(fd, 0xda); //--set com pins hardware configuration
	WriteCmd(fd, 0x12);
	WriteCmd(fd, 0xdb); //--set vcomh
	WriteCmd(fd, 0x20); //0x20,0.77xVcc
	WriteCmd(fd, 0x8d); //--set DC-DC enable
	WriteCmd(fd, 0x14); //
	WriteCmd(fd, 0xaf); //--turn on oled panel

}



/*写 缓存数据*/
void Write_DataBuffer(void)//这个是将DataBuffer数组里面的值，全部写进屏里去
{
	unsigned char i, j;
 
	for (i = 0; i < 8; i++)
	{
		OLED_SetPos(i2cHand, i, 0); //设置起始点坐标
		for (j = 0; j < 128; j++)
		{
			WriteData(i2cHand, DataBuffer[i][j]);//写数据
		}
	}

}



void OLED_SetPos(int fd,unsigned char x, unsigned char y) //设置起始点坐标
{
	WriteCmd(fd, (unsigned char)(0xb0 + x));
	WriteCmd(fd,((y & 0x0f) | 0x00));//LOW
	WriteCmd(fd,(((y & 0xf0) >> 4) | 0x10));//HIGHT
}


void OLED_Fill(void)//全屏填充
{
	int i, j;
	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 128; j++)
		{
			DataBuffer[i][j] = 0xff;
		}
	}
	Write_DataBuffer();
}
 
 
void OLED_CLS(void)//清屏
{
	unsigned char i, j;
	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 128; j++)
		{
			DataBuffer[i][j] = 0x00;
		}
	}
	Write_DataBuffer();
	
	}
	
	
	void OLED_8x16Str(int fd,unsigned char x, unsigned char y, unsigned char ch[])
{
	unsigned char c=0,i=0,j=0;
	while (ch[j]!='\0')
	{
		c =ch[j]-32;
		if(x>120)
		{
			x=0;y++;
		}
		OLED_SetPos(fd,x,y);
		for(i=0;i<8;i++)
		{
			WriteData(fd,F8X16[c*16+i]);
		}
		OLED_SetPos(fd,x,y+1);
		for(i=0;i<8;i++)
		{
			WriteData(fd,F8X16[c*16+i+8]);
		}
		x+=8;
		j++;
	}
}


void OLED_6x8Str(int fd,unsigned char x, unsigned char y, unsigned char ch[])
{
	unsigned char c=0,i=0,j=0;
	while (ch[j]!='\0')
	{
		c = ch[j]-32;
		if(x>126)
		{
			x=0;y++;
		}
		OLED_SetPos(fd,x,y);
		for(i=0;i<6;i++)
		{
			WriteData(fd,F6x8[c][i]);
		}
		x+=6;
		j++;
	}
}

	
	
	int main(void)
{
 
 
	delay(1000); // 毫秒
 
	wiringPiSetupSys();
 
	i2cHand=wiringPiI2CSetup(I2C_ADDR);	/*加载i2c设备*/
	OLED_Init(i2cHand);
 
	while (1)
	{
 
		OLED_Fill();
		delay(5000);
        OLED_CLS();
		delay(5000); // 毫秒
		
	   OLED_6x8Str(i2cHand,0, 0, "1234567890");
        delay(5000); // 毫秒
 
	}
}

