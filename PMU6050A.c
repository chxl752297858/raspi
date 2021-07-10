#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

//****************************************
// 定义MPU6050内部地址
//****************************************
#define	SMPLRT_DIV		0x19	//陀螺仪采样率，典型值：0x07(125Hz)
#define	CONFIG			0x1A	//低通滤波频率，典型值：0x06(5Hz)
#define	GYRO_CONFIG		0x1B	//陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
#define	ACCEL_CONFIG	0x1C	//加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)
#define	ACCEL_XOUT_H	0x3B
#define	ACCEL_XOUT_L	0x3C
#define	ACCEL_YOUT_H	0x3D
#define	ACCEL_YOUT_L	0x3E
#define	ACCEL_ZOUT_H	0x3F
#define	ACCEL_ZOUT_L	0x40
#define	TEMP_OUT_H		0x41
#define	TEMP_OUT_L		0x42
#define	GYRO_XOUT_H		0x43
#define	GYRO_XOUT_L		0x44	
#define	GYRO_YOUT_H		0x45
#define	GYRO_YOUT_L		0x46
#define	GYRO_ZOUT_H		0x47
#define	GYRO_ZOUT_L		0x48
#define	PWR_MGMT_1		0x6B	//电源管理，典型值：0x00(正常启用)
#define	WHO_AM_I		0x75	//IIC地址寄存器(默认数值0x68，只读)
#define	SlaveAddress	0x68	//MPU6050器件地址

int fd;

// 初始化 mpu6050
void initMPU6050(){
    wiringPiSetup () ;
    fd = wiringPiI2CSetup (SlaveAddress);  
    if (fd >= 0) { // fd 为负数，说明IIC连接失败
        printf("fd = %d\n",fd);
        wiringPiI2CWriteReg8(fd,PWR_MGMT_1,0x00); // 开启温度检测 关闭休眠
        wiringPiI2CWriteReg8(fd,SMPLRT_DIV, 0x07);
        wiringPiI2CWriteReg8(fd,CONFIG, 0x06);
        wiringPiI2CWriteReg8(fd,GYRO_CONFIG, 0x18);
        wiringPiI2CWriteReg8(fd,ACCEL_CONFIG, 0x01);
    }
    else {
        printf("IIC初始化失败");
    }
}
// 读取制定寄存器中的数据
int getData(int reg_address){
   return (wiringPiI2CReadReg8(fd,reg_address) << 8) + wiringPiI2CReadReg8(fd,reg_address + 1);
}


int main (void)
{ 
    initMPU6050();
    for(;;){
        printf("ACCEL_XOUT_data = %d\n",getData(ACCEL_XOUT_H));
        printf("ACCEL_YOUT_data = %d\n",getData(ACCEL_YOUT_H));
        printf("ACCEL_ZOUT_data = %d\n",getData(ACCEL_ZOUT_H));

        printf("GYRO_XOUT_data = %d\n",getData(GYRO_XOUT_H));
        printf("GYRO_YOUT_data = %d\n",getData(GYRO_YOUT_H));
        printf("GYRO_DOUT_data = %d\n",getData(GYRO_ZOUT_H));
		
		// 这里温度计算公式 还没找到具体的出处，百度的，不可靠
        printf("TEMP_OUT_data = %f\n",((double)getData(TEMP_OUT_H)/340 + 36.53)); 
        delay(5000); // 间隔5秒 打印一次数据
    }
	return 0 ;
}