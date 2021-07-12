/* 运行：sudo ./lcd1602 [字符] 编译：gcc lcd1602.c -o lcd1602 -L lib -l wiringPi (需已安装wiringPi)
*/
#include <stdio.h>
# include <stdlib.h>
#include <stdint.h>
#include <wiringPi.h>
#include <iconv.h> 
#include <string.h> 
 
#define LCD_RS 10 //显示屏控制线
#define LCD_RW 12
#define LCD_EN 14
 

#define D5 24
#define D6 25
#define D7 26
#define D8 27

#define RELAY 5
 
char u2g_out[255]; 
 
/*===================================================================
功能：编码转换
输入：UTF8
输出：GB2312
====================================================================*/
int code_convert(char *from_charset,char *to_charset,char *inbuf,int inlen,char *outbuf,int outlen){
	iconv_t cd;
	int rc;
	char **pin = &inbuf;
	char **pout = &outbuf;
 
	cd = iconv_open(to_charset,from_charset);
	if (cd==0) return -1;
	memset(outbuf,0,outlen);
	if (iconv(cd,pin,&inlen,pout,&outlen)==-1) return -1;
	iconv_close(cd);
	return 0;
}
 
int u2g(char *inbuf,int inlen,char *outbuf,int outlen){ 
	return code_convert("utf-8","gb2312",inbuf,inlen,outbuf,outlen); 
} 
 
 /*===================================================================
功能：检查LCD忙状态                                                    
输入：无
输出：lcd_busy为1时，忙，等待。lcd-busy为0时,闲，可写指令与数据。      
====================================================================*/
void chk_busy(){//检查忙位
	digitalWrite(LCD_RS,0);
	digitalWrite(LCD_RW,1);
	digitalWrite(LCD_EN,1);
//	WriteData_LCD1602(0xff);
	pinMode(D8, INPUT);
	while(digitalRead(D8));
	pinMode(D8, OUTPUT);
	digitalWrite(LCD_EN,0);
}


 void send_to_lcd (char data, int rs)
{

	digitalWrite(LCD_RS,rs);  // rs = 1 for data, rs=0 for command
	digitalWrite(LCD_RW,0);
    digitalWrite(D5,((data>>0)&0x01));
	digitalWrite(D6,((data>>1)&0x01));
	digitalWrite(D7,((data>>2)&0x01));
	digitalWrite(D8,((data>>3)&0x01));
	
	digitalWrite(LCD_EN, 1);
	delay (20);
    digitalWrite(LCD_EN, 0);
	delay (20);
}

/*===================================================================
功能：总线写入
输入：十六进制数据
输出：无
====================================================================*/
void WriteCmd_LCD1602(char data,char BuysC){
	 
	char datatosend;
	 if (BuysC) chk_busy(); //根据需要检测忙 
	    /* send upper nibble first */
    datatosend = ((data>>4)&0x0f);
	send_to_lcd(datatosend,0); 
	
	    /* send Lower Nibble */
    datatosend = ((data)&0x0f);
	send_to_lcd(datatosend, 0);

}

/*===================================================================
功能：总线写入
输入：十六进制数据
输出：无
====================================================================*/
void WriteData_LCD1602(char data){
	
	char datatosend;
	
	 chk_busy(); //根据需要检测忙 
	
	    /* send upper nibble first */
    datatosend = ((data>>4)&0x0f);
	send_to_lcd(datatosend,1); 
	
	    /* send Lower Nibble */
    datatosend = ((data)&0x0f);
	send_to_lcd(datatosend, 1);

}


void LCDClear(void) //清屏
{
	WriteCmd_LCD1602(0x01,1); //显示清屏
	WriteCmd_LCD1602(0x34,1); // 显示光标移动设置
	WriteCmd_LCD1602(0x30,1); // 显示开及光标设置
}

void LCDFlash(void)	//闪烁效果
{
	WriteCmd_LCD1602(0x08,1); //显示清屏
	delay(400);
	WriteCmd_LCD1602(0x0c,1); // 显示开及光标设置
    delay(400);
    WriteCmd_LCD1602(0x08,1); //显示清屏
    delay(400);
    WriteCmd_LCD1602(0x0c,1); // 显示开及光标设置
	delay(400);
    WriteCmd_LCD1602(0x08,1); //显示清屏
	delay(400);

}


/*==========================================================================
功能：初始化LCD
输入：无
输出：无
===========================================================================*/
void Init_LCD1602(void){			//初始化LCD屏

	pinMode(D5, OUTPUT);
	pinMode(D6, OUTPUT);
	pinMode(D7, OUTPUT);
	pinMode(D8, OUTPUT);
 
	pinMode(LCD_RS, OUTPUT);
	pinMode(LCD_RW, OUTPUT);
	pinMode(LCD_EN, OUTPUT);
	
	WriteCmd_LCD1602(0x30,1); //显示模式设置,开始要求每次检测忙信号
    delay(40);
	WriteCmd_LCD1602(0x30,1); //显示模式设置,开始要求每次检测忙信号
    delay(10);
	WriteCmd_LCD1602(0x30,1); //显示模式设置,开始要求每次检测忙信号
    delay(10);
	WriteCmd_LCD1602(0x20,1); //显示模式设置,开始要求每次检测忙信号
	WriteCmd_LCD1602(0x28,1); //显示模式设置,开始要求每次检测忙信号
    WriteCmd_LCD1602(0x08,1); //显示模式设置,开始要求每次检测忙信号
	WriteCmd_LCD1602(0x01,1); //显示清屏
	WriteCmd_LCD1602(0x06,1); // 显示光标移动设置
	WriteCmd_LCD1602(0x0C,1); // 显示开及光标设置
}

//按指定位置显示一个字符
void DisplayOneChar(unsigned char X, unsigned char Y, unsigned char DData)
{
	if(Y<1)
	{
		Y=1;
	}
	
	if(Y>2)
	{
		Y=2;
	}
	X &= 0x0F; //限制X不能大于16，Y不能大于1
	switch(Y)
	{
		case 1:
		{
			X|=0X80;
			break;
		}
		case 2:
		{
			X|=0XC0;
			break;
		}
	
		default :
		{
			break;
		}
	}
	WriteCmd_LCD1602(X,0); //这里不检测忙信号，发送地址码
	WriteData_LCD1602(DData);
	delay(5);
}


//按指定位置显示一串字符
void DisplayListChar(unsigned char X, unsigned char Y, unsigned char  *DData)
{
	unsigned char ListLength,X2;
	ListLength = 0;
	X2=X;
	if(Y<1)
	{
		Y=1;
	}
	if(Y>2)
	{
		Y=2;
	}
	X &= 0x0F; //限制X不能大于16，Y在1-4之内
	switch(Y)
	{
		case 1:
		{
			X2|=0X80;
			break;	//根据行数来选择相应地址
		}
		case 2:
		{
			X2|=0XC0;
			break;
		}
	
		default :
		{
			break;
		}
	}
	WriteCmd_LCD1602(X2,1); //发送地址码
	while (DData[ListLength]>=0x0F) //若到达字串尾则退出
	{
		if (X <= 0x0F) //X坐标应小于0xF
		{
			WriteData_LCD1602(DData[ListLength]); //
			ListLength++;
			X++;
			delay(5);
		}
	}
}

 
 
int main (void){
	int i;
	char buff[20];
	wiringPiSetup();
	pinMode(RELAY, OUTPUT);
	Init_LCD1602();

while(1)
{  
	DisplayListChar(0,1, "heoll world");
	DisplayListChar(1,2, "hello world");


}

}