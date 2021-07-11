
#include <wiringPi.h>
#include <wiringPiI2C.h>
 
// LED Pin - wiringPi pin 0 是 BCM_GPIO 17。  ssd1306
//利用 wiringPiSetupSys 进行初始化时必须使用 BCM 编号
//选择其他 pin 编号时，请同时使用 BCM 编号
//更新 Property Pages - Build Events - Remote Post-Build Event 命令
//它使用 gpio 导出进行 wiringPiSetupSys 的设置
#define	LED	17
 
#define I2C_ADDR 0x3c
unsigned char DataBuffer[8][128];/*全屏大小 8page页 * 128col列  row64行  每页有8格，通过高低列组合出0~127的数据，寻找起始列进行从左向右填充*/
 
int i2cHand = 0;
 
 
void WriteCmd(int fd, unsigned char I2C_Command)//写命令
{
	wiringPiI2CWriteReg8(fd, 0x00, I2C_Command);
}
 
 
void OLED_Init(int fd)
{
 
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
	WriteCmd(fd, (unsigned char)(0xb0 + x));  /*设置开始页*/
//	WriteCmd(fd, (unsigned char)(0x40 + x));  /*设置开始行*/
	WriteCmd(fd, ((y & 0x0f) | 0x00));//LOW
	WriteCmd(fd, (((y & 0xf0) >> 4) | 0x10));//HIGHT
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
 
void OLED_Full(void)//全屏填充
{
	int i, j;
	for (i = 0; i <8; i++)
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
	for (i = 0; i <8; i++)
	{
		for (j = 0; j < 128; j++)
		{
			DataBuffer[i][j] = 0x00;
		}
	}
	Write_DataBuffer();
}
 
int main(void)
{
	
	delay(1000); // 毫秒
 
	wiringPiSetupSys();
 
	pinMode(LED, OUTPUT);
 
	i2cHand = wiringPiI2CSetup(I2C_ADDR);	/*加载i2c设备*/
	OLED_Init(i2cHand);
 
	while (1)
	{
 
		digitalWrite(LED, HIGH);  //开
		delay(500); // 毫秒
		digitalWrite(LED, LOW);	  //关
		delay(500);
 
		OLED_Full();
		delay(1000);
 
		OLED_CLS();
		delay(1000); // 毫秒
 
 
	}
 
	return 0;
}


