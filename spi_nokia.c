#include <wiringPi.h>
#include <stdio.h>  
#include <wiringPiSPI.h>

#define LCD_DC  11     // 模式选择 1：写数据 0：写命令
#define LCD_DIN 12     // 串行数据线
#define LCD_CLK 14     // 串行时钟线
#define LCD_RST 5     // 复位信号  低电平有效
#define LCD_CE  10     // 芯片使能  低电平有效


#define u16 unsigned int 
#define u8  unsigned char

#define X_Col_addr  0x80  // 定位列 地址 0-83
#define Y_Row_addr  0x40  // 定位行 地址 0-5
				
u8  Font_[][6] = {
{0x00,0x00,0x00,0x00,0x00,0x00},// (0)
{0x00,0x00,0x00,0x4F,0x00,0x00},//!(1)
{0x00,0x00,0x07,0x00,0x07,0x00},//"(2)
{0x00,0x14,0x7F,0x14,0x7F,0x14},//#(3)
{0x00,0x24,0x2A,0x7F,0x2A,0x12},//$(4)
{0x00,0x23,0x13,0x08,0x64,0x62},//%(5)
{0x00,0x36,0x49,0x55,0x22,0x50},//&(6)
{0x00,0x00,0x05,0x03,0x00,0x00},//'(7)
{0x00,0x00,0x1C,0x22,0x41,0x00},//((8)
{0x00,0x00,0x41,0x22,0x1C,0x00},//)(9)
{0x00,0x14,0x08,0x3E,0x08,0x14},//*(10)
{0x00,0x08,0x08,0x3E,0x08,0x08},//+(11)
{0x00,0x00,0x50,0x30,0x00,0x00},//,(12)
{0x00,0x08,0x08,0x08,0x08,0x08},//-(13)
{0x00,0x00,0x60,0x60,0x00,0x00},//.(14)
{0x00,0x20,0x10,0x08,0x04,0x02},///(15)
{0x00,0x3E,0x51,0x49,0x45,0x3E},//0(16)
{0x00,0x00,0x42,0x7F,0x40,0x00},//1(17)
{0x00,0x42,0x61,0x51,0x49,0x46},//2(18)
{0x00,0x21,0x41,0x45,0x4B,0x31},//3(19)
{0x00,0x18,0x14,0x12,0x7F,0x10},//4(20)
{0x00,0x27,0x45,0x45,0x45,0x39},//5(21)
{0x00,0x3C,0x4A,0x49,0x49,0x30},//6(22)
{0x00,0x01,0x71,0x09,0x05,0x03},//7(23)
{0x00,0x36,0x49,0x49,0x49,0x36},//8(24)
{0x00,0x06,0x49,0x49,0x29,0x1E},//9(25)
{0x00,0x00,0x36,0x36,0x00,0x00},//:(26)
{0x00,0x00,0x56,0x36,0x00,0x00},//;(27)
{0x00,0x08,0x14,0x22,0x41,0x00},//<(28)
{0x00,0x14,0x14,0x14,0x14,0x14},//=(29)
{0x00,0x00,0x41,0x22,0x14,0x08},//>(30)
{0x00,0x02,0x01,0x51,0x09,0x06},//?(31)
{0x00,0x32,0x49,0x79,0x41,0x3E},//@(32)
{0x00,0x7E,0x11,0x11,0x11,0x7E},//A(33)
{0x00,0x7F,0x49,0x49,0x49,0x3E},//B(34)
{0x00,0x3E,0x41,0x41,0x41,0x22},//C(35)
{0x00,0x7F,0x41,0x41,0x22,0x1C},//D(36)
{0x00,0x7F,0x49,0x49,0x49,0x41},//E(37)
{0x00,0x7F,0x09,0x09,0x09,0x01},//F(38)
{0x00,0x3E,0x41,0x49,0x49,0x7A},//G(39)
{0x00,0x7F,0x08,0x08,0x08,0x7F},//H(40)
{0x00,0x00,0x41,0x7F,0x41,0x00},//I(41)
{0x00,0x20,0x40,0x41,0x3F,0x01},//J(42)
{0x00,0x7F,0x08,0x14,0x22,0x41},//K(43)
{0x00,0x7F,0x40,0x40,0x40,0x40},//L(44)
{0x00,0x7F,0x02,0x04,0x02,0x7F},//M(45)
{0x00,0x7F,0x04,0x08,0x10,0x7F},//N(46)
{0x00,0x3E,0x41,0x41,0x41,0x3E},//O(47)
{0x00,0x7F,0x09,0x09,0x09,0x06},//P(48)
{0x00,0x3E,0x41,0x51,0x21,0x5E},//Q(49)
{0x00,0x7F,0x09,0x19,0x29,0x46},//R(50)
{0x00,0x46,0x49,0x49,0x49,0x31},//S(51)
{0x00,0x01,0x01,0x7F,0x01,0x01},//T(52)
{0x00,0x3F,0x40,0x40,0x40,0x3F},//U(53)
{0x00,0x1F,0x20,0x40,0x20,0x1F},//V(54)
{0x00,0x3F,0x40,0x38,0x40,0x3F},//W(55)
{0x00,0x63,0x14,0x08,0x14,0x63},//X(56)
{0x00,0x03,0x04,0x78,0x04,0x03},//Y(57)
{0x00,0x61,0x51,0x49,0x45,0x43},//Z(58)
{0x00,0x00,0x7F,0x41,0x41,0x00},//[(59)
{0x00,0x15,0x16,0x7C,0x16,0x15},//\(60)
{0x00,0x00,0x41,0x41,0x7F,0x00},//](61)
{0x00,0x04,0x02,0x01,0x02,0x04},//^(62)
{0x00,0x40,0x40,0x40,0x40,0x40},//_(63)
{0x00,0x00,0x01,0x02,0x04,0x00},//`(64)
{0x00,0x20,0x54,0x54,0x54,0x78},//a(65)
{0x00,0x7F,0x48,0x44,0x44,0x38},//b(66)
{0x00,0x38,0x44,0x44,0x44,0x20},//c(67)
{0x00,0x38,0x44,0x44,0x48,0x7F},//d(68)
{0x00,0x38,0x54,0x54,0x54,0x18},//e(69)
{0x00,0x08,0x7E,0x09,0x01,0x02},//f(70)
{0x00,0x0C,0x52,0x52,0x52,0x3E},//g(71)
{0x00,0x7F,0x08,0x04,0x04,0x78},//h(72)
{0x00,0x00,0x44,0x7D,0x40,0x00},//i(73)
{0x00,0x20,0x40,0x44,0x3D,0x00},//j(74)
{0x00,0x7F,0x10,0x28,0x44,0x00},//k(75)
{0x00,0x00,0x41,0x7F,0x40,0x00},//l(76)
{0x00,0x7E,0x02,0x0C,0x02,0x7C},//m(77)
{0x00,0x7E,0x04,0x02,0x02,0x7C},//n(78)
{0x00,0x38,0x44,0x44,0x44,0x38},//o(79)
{0x00,0x7C,0x14,0x14,0x14,0x08},//p(80)
{0x00,0x08,0x14,0x14,0x18,0x7C},//q(81)
{0x00,0x7C,0x08,0x04,0x04,0x08},//r(82)
{0x00,0x48,0x54,0x54,0x54,0x20},//s(83)
{0x00,0x04,0x3F,0x44,0x40,0x20},//t(84)
{0x00,0x3C,0x40,0x40,0x20,0x7C},//u(85)
{0x00,0x1C,0x20,0x40,0x20,0x1C},//v(86)
{0x00,0x3C,0x40,0x30,0x40,0x3C},//w(87)
{0x00,0x44,0x28,0x10,0x28,0x44},//x(88)
{0x00,0x0C,0x50,0x50,0x50,0x3C},//y(89)
{0x00,0x44,0x64,0x54,0x4C,0x44},//z(90)
{0x00,0x00,0x08,0x36,0x41,0x00},//{(91)
{0x00,0x00,0x00,0x7F,0x00,0x00},//|(92)
{0x00,0x00,0x41,0x36,0x08,0x00},//}(93)
{0x00,0x08,0x04,0x08,0x10,0x08},//~(94)
{0x00,0x08,0x08,0x2A,0x1C,0x08},//→(127)
{0x00,0x08,0x1C,0x2A,0x08,0x08},//←(128)
{0x00,0x04,0x02,0x7F,0x02,0x04},//↑(129)
{0x00,0x10,0x20,0x7F,0x20,0x10},//↓(130)
{0x00,0x1C,0x2A,0x32,0x2A,0x1C},//笑面(131)
{0x00,0x1C,0x22,0x44,0x22,0x1C}};//爱心(132)
 
/*--------------------------------------------------------------*/

// 初始化GPIO 端口
void gpioInit(){
    pinMode(LCD_DC,OUTPUT);
    pinMode(LCD_RST,OUTPUT);
    pinMode(LCD_CE,OUTPUT);
}

// 往LCD写入数据
// wbyte: 需要写入的数据
// dat_cmd: 1-数据 0-命令 
void lcd_writeByte(u8 wbyte, u8 dat_cmd){
    digitalWrite(LCD_CE,LOW);
    if (dat_cmd == 1) { // 写数据
        digitalWrite(LCD_DC,HIGH);
    }
    else { // 写命令
        digitalWrite(LCD_DC,LOW);
    }
    u8 *tempData = &wbyte;
    wiringPiSPIDataRW(0, tempData, 1);
    digitalWrite (LCD_CE, HIGH);
}
// 液晶清屏
void lcd_clear(){
    u16 i;
    lcd_writeByte(X_Col_addr,0);
    lcd_writeByte(Y_Row_addr,0);
    // 84*48的LCD点阵液晶  那么84x48=504
    for(i = 0; i < 504; i ++){
        lcd_writeByte(0x00,1);
    }
}
// 显示字节定位
// x: x轴坐标 0-83
// y: y轴坐标 0-5
void lcd_pos_byte(u8 x,u8 y){
    x |= X_Col_addr;
    y |= Y_Row_addr;
    lcd_writeByte(x,0);
    lcd_writeByte(y,0);
}
// 清屏 某一行
void lcd_clear_row(u8 rowNum){
    u8 i;
    lcd_pos_byte(0,rowNum);
    for(i = 0; i < 84; i ++){
        lcd_writeByte(0x00,1);
    }
}
// 显示一个6*8字符
void lcd_show_char(u8 x, u8 y, u8 c_dat){
    u8 i;
    c_dat -= 32;
    x *= 6;
    lcd_pos_byte(x,y);
    for(i = 0; i < 6; i ++){
        lcd_writeByte(Font_[c_dat][i],1);
    }
}
// 显示字符串
void lcd_show_string(u8 x, u8 y, char *str){
    while(*str){
        lcd_show_char(x++, y, *str);
        str ++;
        if(x == 14){ x = 0; y ++;}
        if(y == 6){ y = 0;}
    }
}
// 初始化液晶
void lcd_init(){
    gpioInit();
    int isOK = wiringPiSPISetup(1, 1000000);
    if (isOK == -1) {
        printf("SPI设置失败\n");
    }
    else {
        printf("SPI设置OK == %d\n",isOK);
    }
    digitalWrite(LCD_RST,LOW); // 液晶复位
    delay(20);
    digitalWrite(LCD_RST,HIGH);
    delay(20);
    lcd_writeByte(0x21,0); // 工作模式 水平寻址  扩展指令
    lcd_writeByte(0x06,0); // VLCD温度系数2
    lcd_writeByte(0x13,0); // 设置偏置系统 1:48
    lcd_writeByte(0xc8,0); // 设置对比度
    lcd_writeByte(0x20,0); // 工作模式 水平寻址 常规指令
    lcd_writeByte(0x0c,0); // 普通模式
    lcd_writeByte(X_Col_addr,0);
    lcd_writeByte(Y_Row_addr,0); 
    lcd_clear(); // 清屏
}

int main (void)
{
    wiringPiSetup();
    lcd_init();
    lcd_show_string(0,0,"I Love You!");
	return 0;
}
