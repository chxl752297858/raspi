
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ifaddrs.h>

#include "codetab.h"
 
// LED Pin - wiringPi pin 0 是 BCM_GPIO 17。  ssd1306
//利用 wiringPiSetupSys 进行初始化时必须使用 BCM 编号
//选择其他 pin 编号时，请同时使用 BCM 编号
//更新 Property Pages - Build Events - Remote Post-Build Event 命令
//它使用 gpio 导出进行 wiringPiSetupSys 的设置
#define	LED	0
 
#define I2C_ADDR 0x3c
unsigned char DataBuffer[8][128];/*全屏大小 8page页 * 128col列  row64行  每页有8格，通过高低列组合出0~127的数据，寻找起始列进行从左向右填充*/
 
int i2cHand = 0;
char buf [32] ;
char buf1 [32] ;
char buf_ip [32] ;
char ip[3][INET_ADDRSTRLEN];
int n;



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


 
 
void WriteCmd(int fd, unsigned char I2C_Command)//写命令
{
	wiringPiI2CWriteReg8(fd, 0x00, I2C_Command);
}
 
 
void OLED_Init(int fd)
{
	delay(100); // 毫秒
 
	WriteCmd(fd, 0xAE); //display off
 
	WriteCmd(fd, 0x20);	//Set Memory Addressing Mode	/*默认0x02，从左向右刷新 */
 
	WriteCmd(fd, 0x10);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	
	WriteCmd(fd, 0xb0);	//Set Page Start Address for Page Addressing Mode,0-7
	
	WriteCmd(fd, 0xc8);	//Set COM Output Scan Direction
 
	WriteCmd(fd, 0x00); //---set low column address /*起始位置低位列0x00~0x0f */
 
	WriteCmd(fd, 0x10); //---set high column address /*起始位置高位列0x10~0x1f */
 
	WriteCmd(fd, 0x40); //--set start line address  /*起始行0x40~0x7F  正好是0~63*/
 
	WriteCmd(fd, 0x81); //--set contrast control register
	WriteCmd(fd, 0xff); //亮度调节 0x00~0xff
 
	WriteCmd(fd, 0xa1); //--set segment re-map 0 to 127
 
	WriteCmd(fd, 0xa6); //--set normal display
 
	WriteCmd(fd, 0xa8); //--set multiplex ratio(1 to 64)
	WriteCmd(fd, 0x3F); // /*设置页，0x3f为默认，有效值为0~14 */
 
	WriteCmd(fd, 0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	
	WriteCmd(fd, 0xd3); //-set display offset
	WriteCmd(fd, 0x00); //-not offset 0x00为默认
 
	WriteCmd(fd, 0xd5); //--set display clock divide ratio/oscillator frequency
	WriteCmd(fd, 0xf0); //--set divide ratio
 
	WriteCmd(fd, 0xd9); //--set pre-charge period
	WriteCmd(fd, 0x22); //
 
	WriteCmd(fd, 0xda); //--set com pins hardware configuration
	WriteCmd(fd, 0x12);
 
	WriteCmd(fd, 0xdb); //--set vcomh
	WriteCmd(fd, 0x20); //0x20,0.77xVcc 默认
 
	WriteCmd(fd, 0x8d); //--set DC-DC enable  
	WriteCmd(fd, 0x14); //  0x14 enable  0x10 disabled（sleep）  一般用于交流电设置
 
	WriteCmd(fd, 0xAF); //--turn on oled panel
	

}
 
void WriteData(int fd, unsigned char I2C_Data)//写数据
{
	wiringPiI2CWriteReg8(fd, 0x40, I2C_Data);
}
 
 


void OLED_SetPos(int fd, unsigned char x, unsigned char y) //设置起始点坐标
{

	WriteCmd( fd,0xb0+y);
	WriteCmd( fd,((x&0xf0)>>4)|0x10);
	WriteCmd( fd,(x&0x0f)|0x01);
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
 

 
 void OLED_Fill(int fd,unsigned char fill_Data)//全屏填充
{
	unsigned char m,n;
	for(m=0;m<8;m++)
	{
		WriteCmd(fd,0xb0+m);		//page0-page1
		WriteCmd(fd,0x00);		//low column start address
		WriteCmd(fd,0x10);		//high column start address
		for(n=0;n<128;n++)
			{
				WriteData(fd,fill_Data);
			}
	}
}



//--------------------------------------------------------------
// Prototype      : void OLED_ON(void)
// Calls          : 
// Parameters     : none
// Description    : 将OLED从休眠中唤醒
//--------------------------------------------------------------
void OLED_ON(int fd)
{
	WriteCmd(fd,0X8D);  //设置电荷泵
	WriteCmd(fd,0X14);  //开启电荷泵
	WriteCmd(fd,0XAF);  //OLED唤醒
}

//--------------------------------------------------------------
// Prototype      : void OLED_OFF(void)
// Calls          : 
// Parameters     : none
// Description    : 让OLED休眠 -- 休眠模式下,OLED功耗不到10uA
//--------------------------------------------------------------
void OLED_OFF(int fd)
{
	WriteCmd(fd,0X8D);  //设置电荷泵
	WriteCmd(fd,0X10);  //关闭电荷泵
	WriteCmd(fd,0XAE);  //OLED休眠
}

//--------------------------------------------------------------
// Prototype      : void OLED_ShowChar(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
// Calls          : 
// Parameters     : x,y -- 起始点坐标(x:0~127, y:0~7); ch[] -- 要显示的字符串; TextSize -- 字符大小(1:6*8 ; 2:8*16)
// Description    : 显示codetab.h中的ASCII字符,有6*8和8*16可选择
//--------------------------------------------------------------
void OLED_ShowStr(int fd,unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
{
	unsigned char c = 0,i = 0,j = 0;
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
				OLED_SetPos(fd,x,y);
				for(i=0;i<6;i++)
					WriteData(fd,F6x8[c][i]);
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
				OLED_SetPos(fd,x,y);
				for(i=0;i<8;i++)
					WriteData(fd,F8X16[c*16+i]);
				OLED_SetPos(fd,x,y+1);
				for(i=0;i<8;i++)
					WriteData(fd,F8X16[c*16+i+8]);
				x += 8;
				j++;
			}
		}break;
	}
}


//--------------------------------------------------------------
// Prototype      : void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N)
// Calls          : 
// Parameters     : x,y -- 起始点坐标(x:0~127, y:0~7); N:汉字在codetab.h中的索引
// Description    : 显示codetab.h中的汉字,16*16点阵
//--------------------------------------------------------------
void OLED_ShowCN(int fd,unsigned char x, unsigned char y, unsigned char *data)
{
	unsigned char wm=0;
	OLED_SetPos(fd,x , y);
	for(wm = 0;wm < 16;wm++)
	{
		WriteData(fd,data[wm]);
	}
	OLED_SetPos(fd,x,y + 1);
	for(wm = 16;wm < 32;wm++)
	{
		WriteData(fd,data[wm]);
	}
}
//--------------------------------------------------------------
// Prototype      : void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N)
// Calls          : 
// Parameters     : x,y -- 起始点坐标(x:0~127, y:0~7); N:汉字在codetab.h中的索引
// Description    : 显示codetab.h中的汉字,16*16点阵
//--------------------------------------------------------------
void OLED_Show_HZ(int fd,unsigned char x, unsigned char y, unsigned char *HZ)
{
	unsigned int i = 0,j = 0,k = 0;
	unsigned int p_x = x;
	while(HZ[j] != 0)
	{
		while(Hz_16X16_Lib[k].Index[0] != 0)
		{
			
			if(HZ[j] == Hz_16X16_Lib[k].Index[0] && HZ[j + 1] == Hz_16X16_Lib[k].Index[1])//在字库中找到该字
			{
				OLED_SetPos(fd,p_x , y);
				for(i = 0;i < 32;i ++)
				{
					if(i == 16)
					{
						OLED_SetPos(fd,p_x , y + 1);
					}
					WriteData(fd,Hz_16X16_Lib[k].Msk[i]);
				}
				break;
			}
			k ++;
		}
		j += 2;
		p_x += 16;
	}
}

void OLED_16x16CN(int fd,unsigned char x, unsigned char y, unsigned char N)
{
	unsigned char wm=0;
	unsigned int adder=32*N;
	OLED_SetPos(fd,x , y);
	for(wm = 0;wm < 16;wm++)
	{
		WriteData(fd,F16x16[adder]);
		adder += 1;
	}
	OLED_SetPos(fd,x,y + 1);
	for(wm = 0;wm < 16;wm++)
	{
		WriteData(fd,F16x16[adder]);
		adder += 1;
	}
}
//--------------------------------------------------------------
// Prototype      : void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);
// Calls          : 
// Parameters     : x0,y0 -- 起始点坐标(x0:0~127, y0:0~7); x1,y1 -- 起点对角线(结束点)的坐标(x1:1~128,y1:1~8)
// Description    : 显示BMP位图
//--------------------------------------------------------------
void OLED_DrawBMP(int fd,unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[])
{
	unsigned int j=0;
	unsigned char x,y;

  if(y1%8==0)
		y = y1/8;
  else
		y = y1/8 + 1;
	for(y=y0;y<y1;y++)
	{
		OLED_SetPos(fd,x0,y);
    for(x=x0;x<x1;x++)
		{
			WriteData(fd,BMP[j++]);
		}
	}
}


 
int main(void)
{
  int i;
  struct tm *t ;
  time_t tim ;

  
 
	wiringPiSetupSys();
 
	pinMode(LED, OUTPUT);
 
	i2cHand = wiringPiI2CSetup(I2C_ADDR);	/*加载i2c设备*/
	OLED_Init(i2cHand);
	OLED_Fill(i2cHand,0xff);
    delay(1000);
	OLED_Fill(i2cHand,0x00);
	delay(1000);
		 
		        



	while (1)
	{
 	tim = time (NULL) ;
    t = localtime (&tim) ;
    memset(ip, 0, sizeof(ip));
     for (n=get_local_ip(*ip); n>0; n--) {
               // printf("IP:%s\n", ip[n-1]);
				sprintf(buf_ip,"%s\n", ip[n-1]);

        }
		
    sprintf (buf, " %02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec) ;
	sprintf (buf1, "%04d-%02d-%02d", t->tm_year+1900,t->tm_mon, t->tm_mday ) ;

	delay(100); // 毫秒
 
	
		// OLED_Fill(i2cHand,0xff);
		 // delay(1000);
		 	// OLED_Fill(i2cHand,0x00);
		 // delay(1000);
		
		OLED_ShowStr(i2cHand,0, 0, buf, 2);
		OLED_ShowStr(i2cHand,0, 2, buf1, 2);
		OLED_ShowStr(i2cHand,0, 4, buf_ip, 2);
		


 

 
	}
 
	return 0;
}


