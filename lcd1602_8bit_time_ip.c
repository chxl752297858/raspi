/* 运行：sudo ./lcd1602 [字符] 编译：gcc lcd1602.c -o lcd1602 -L lib -l wiringPi (需已安装wiringPi)
*/
#include <stdio.h>
# include <stdlib.h>
#include <stdint.h>
#include <wiringPi.h>
#include <iconv.h> 
#include <string.h> 
#include <time.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ifaddrs.h>

 
#define LCD_RS 10 //显示屏控制线
#define LCD_RW 12
#define LCD_EN 14
 
#define D1 30 //显示屏数据线
#define D2 21
#define D3 22
#define D4 23
#define D5 24
#define D6 25
#define D7 26
#define D8 27

#define RELAY 5
 
char u2g_out[255]; 
char buf_ip [32] ;
char ip[3][INET_ADDRSTRLEN];
int n;

 
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
功能：总线写入
输入：十六进制数据
输出：无
====================================================================*/
void bus_write(unsigned char data){
	int t[10];
	int f=0,i=0,d=data;
 
	//进制转换
	for(i=0;i<8;i++){
		t[i]=data%2;
		data=data/2;
	}
 
	//输出
	digitalWrite(D1,t[0]);
	digitalWrite(D2,t[1]);
	digitalWrite(D3,t[2]);
	digitalWrite(D4,t[3]);
	digitalWrite(D5,t[4]);
	digitalWrite(D6,t[5]);
	digitalWrite(D7,t[6]);
	digitalWrite(D8,t[7]);
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
	bus_write(0xff);
	pinMode(D8, INPUT);
	while(digitalRead(D8));
	pinMode(D8, OUTPUT);
	digitalWrite(LCD_EN,0);
}
/*====================================================================
功能：写命令
输入：8位数据
输出：无
=====================================================================*/
void WriteCmd_LCD1602(unsigned char cmdcode){
	chk_busy();
	digitalWrite(LCD_RS,0);
	digitalWrite(LCD_RW,0);
	digitalWrite(LCD_EN,1);
	delay(5);
	bus_write(cmdcode);
	digitalWrite(LCD_EN,0);
	delay(5);
}
/*====================================================================
功能：写数据
输入：8位数据
输出：无
=====================================================================*/
void WriteData_LCD1602(unsigned char Dispdata){
	chk_busy();
	digitalWrite(LCD_RS,1);
	digitalWrite(LCD_RW,0);
	digitalWrite(LCD_EN,1);
	delay(5);
	bus_write(Dispdata);
	digitalWrite(LCD_EN,0);
	delay(5);
}


void LCDClear(void) //清屏
{
	WriteCmd_LCD1602(0x01); //显示清屏
    WriteCmd_LCD1602(0x06); // 显示光标移动设置
	WriteCmd_LCD1602(0x0C); // 显示开及光标设置
}

void LCDFlash(void)	//闪烁效果
{
	WriteCmd_LCD1602(0x08); //显示清屏
	delay(400);
	WriteCmd_LCD1602(0x0c); // 显示开及光标设置
    delay(400);
    WriteCmd_LCD1602(0x08); //显示清屏
    delay(400);
    WriteCmd_LCD1602(0x0c); // 显示开及光标设置
	delay(400);
    WriteCmd_LCD1602(0x08); //显示清屏
	delay(400);

}


/*==========================================================================
功能：初始化LCD
输入：无
输出：无
===========================================================================*/
void Init_LCD1602(void){			//初始化LCD屏
	pinMode(D1, OUTPUT);	//设置GPIO
	pinMode(D2, OUTPUT);
	pinMode(D3, OUTPUT);
	pinMode(D4, OUTPUT);
	pinMode(D5, OUTPUT);
	pinMode(D6, OUTPUT);
	pinMode(D7, OUTPUT);
	pinMode(D8, OUTPUT);
 
	pinMode(LCD_RS, OUTPUT);
	pinMode(LCD_RW, OUTPUT);
	pinMode(LCD_EN, OUTPUT);
	
	WriteCmd_LCD1602(0x38); //显示模式设置,开始要求每次检测忙信号
    delay(10);
	WriteCmd_LCD1602(0x38); //显示模式设置,开始要求每次检测忙信号
    delay(10);
	WriteCmd_LCD1602(0x38); //显示模式设置,开始要求每次检测忙信号
    delay(10);
	WriteCmd_LCD1602(0x30); //显示模式设置,开始要求每次检测忙信号
	WriteCmd_LCD1602(0x38); //显示模式设置,开始要求每次检测忙信号
    WriteCmd_LCD1602(0x08); //显示模式设置,开始要求每次检测忙信号
	WriteCmd_LCD1602(0x01); //显示清屏
	WriteCmd_LCD1602(0x06); // 显示光标移动设置
	WriteCmd_LCD1602(0x0C); // 显示开及光标设置
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
	WriteCmd_LCD1602(X); //这里不检测忙信号，发送地址码
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
	WriteCmd_LCD1602(X2); //发送地址码
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






int get_local_ip(char *ip_list) {
        struct ifaddrs *ifAddrStruct;

        void *tmpAddrPtr;

        char ip[INET_ADDRSTRLEN];

        int n = 0;

        getifaddrs(&ifAddrStruct);

        while (ifAddrStruct != NULL) {
                if (ifAddrStruct->ifa_addr->sa_family==AF_INET) {
                        tmpAddrPtr=&((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;

                        inet_ntop(AF_INET, tmpAddrPtr, ip, INET_ADDRSTRLEN);

                        if (strcmp(ip, "127.0.0.1") != 0) {
//                              printf("%s IP Address:%s\n", ifAddrStruct->ifa_name, ip);

                                if (n == 0){
                                        memcpy(ip_list, ip, INET_ADDRSTRLEN);

                                } else {
                                        memcpy(ip_list+INET_ADDRSTRLEN, ip, INET_ADDRSTRLEN);

                                }

                                n++;

                        }

                }

                ifAddrStruct=ifAddrStruct->ifa_next;

        }

        //free ifaddrs

        freeifaddrs(ifAddrStruct);

        return n;

}


 

 
 
int main (void){
	int i;
	char buff[20];
	struct tm *t ;
    time_t tim ;

	wiringPiSetup();
	pinMode(RELAY, OUTPUT);
	Init_LCD1602();

	DisplayListChar(0,1, "  now times    ");
	DisplayListChar(0,2, "  ip adress    ");
	delay(2000);
    LCDClear();//清屏

while(1)

{  
	
    tim = time (NULL) ;
    t = localtime (&tim) ;
    sprintf (buff, " %02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec) ;
   memset(ip, 0, sizeof(ip));
     for (n=get_local_ip(*ip); n>0; n--) {
               // printf("IP:%s\n", ip[n-1]);
				sprintf(buf_ip,"%s\n", ip[n-1]);

        }
		
	DisplayListChar(0,1, buff);
	DisplayListChar(0,2, buf_ip);

}

}