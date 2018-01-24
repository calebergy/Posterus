#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>          //needed to use ioctl
#include <stdint.h>

/*MPU6050 register addresses */

#define MPU6050_REG_POWER               0x6B
#define MPU6050_REG_ACCEL_CONFIG        0x1C
#define MPU6050_REG_GYRO_CONFIG         0x1B

/*These are the addresses of mpu6050 from which you will fetch accelerometer x,y,z high and low values */
#define MPU6050_REG_ACC_X_HIGH          0x3B
#define MPU6050_REG_ACC_X_LOW           0x3C
#define MPU6050_REG_ACC_Y_HIGH          0x3D
#define MPU6050_REG_ACC_Y_LOW           0x3E
#define MPU6050_REG_ACC_Z_HIGH          0x3F
#define MPU6050_REG_ACC_Z_LOW           0x40

/*These are the addresses of mpu6050 from which you will fetch gyro x,y,z high and low values */
#define MPU6050_REG_GYRO_X_HIGH          0x43
#define MPU6050_REG_GYRO_X_LOW           0x44
#define MPU6050_REG_GYRO_Y_HIGH          0x45
#define MPU6050_REG_GYRO_Y_LOW           0x46
#define MPU6050_REG_GYRO_Z_HIGH          0x47
#define MPU6050_REG_GYRO_Z_LOW           0x48

/*
 * Different full scale ranges for acc and gyro
 * refer table 6.2 and 6.3 in the document MPU-6000 and MPU-6050 Product Specification Revision 3.4
 *
 */
#define ACC_FS_SENSITIVITY_0					16384
#define ACC_FS_SENSITIVITY_1		            8192
#define ACC_FS_SENSITIVITY_2		            4096
#define ACC_FS_SENSITIVITY_3		            2048

#define GYR_FS_SENSITIVITY_0					 131
#define GYR_FS_SENSITIVITY_1					 65.5
#define GYR_FS_SENSITIVITY_2					 32.8
#define GYR_FS_SENSITIVITY_3				 	 16.4


/* This is the I2C slave address of mpu6050 sensor */
#define MPU6050_SLAVE_ADDR 0x68

#define MAX_VALUE 50

/* This is the linux OS device file for hte I2C3 controller of the SOC */
#define I2C_DEVICE_FILE   "/dev/i2c-2"

int fd;

/*write a 8bit "data" to the sensor at the address indicated by "addr" */
int mpu6050_write(uint8_t addr, uint8_t data)
{
	char buf[2];
	buf[0]=addr;
	buf[1]=data;

	int ret = write(fd,buf,2);

	if(ret<0){
		perror("Error writing to slave");
		return -1;
	}

	return 0;
}

/*read "len" many bytes from "addr" of the sensor in to the adresss indicated by "pBuffer" */
int mpu6050_read(uint8_t base_addr, char *pBuffer,uint32_t len)
{
	char buf[1];
	buf[0]=base_addr;
	int ret = write(fd,buf,1);

	if(ret<0){
		perror("Error trying to read from slave");
		return -1;
	}

	ret = read(fd,pBuffer,6);

	if(ret<0){
		perror("Error trying to read from slave");
	}
	return 0;
}


/* by default mpu6050 will in sleep mode, so disable its sleep mode and also configure
 * the full scale ranges for gyro and acc
 */
void mpu6050_init()
{
    mpu6050_write(MPU6050_REG_POWER, 0x00);
    usleep(500);

    // Adjust full scale values for gyro and acc
    mpu6050_write(MPU6050_REG_ACCEL_CONFIG, 0x00);
    usleep(500);
    mpu6050_write(MPU6050_REG_GYRO_CONFIG, 0x00);
    usleep(500);
}

/* read accelerometer values of x,y,z in to the buffer "pBuffer" */
void mpu6050_read_acc(short* pBuffer)
{
	char accBuffer[6];

	mpu6050_read(MPU6050_REG_ACC_X_HIGH,accBuffer,6);

	pBuffer[0]= (accBuffer[0]<<8) | accBuffer[1];
	pBuffer[1]= (accBuffer[2]<<8) | accBuffer[3];
	pBuffer[2]= (accBuffer[4]<<8) | accBuffer[5];


}

/* read gyro values of x,y,z in to the buffer "pBuffer" */
void mpu6050_read_gyro(short* pBuffer)
{
	char gyroBuffer[6];

	mpu6050_read(MPU6050_REG_GYRO_X_HIGH,gyroBuffer,6);

	pBuffer[0]= (gyroBuffer[0]<<8) | gyroBuffer[1];
	pBuffer[1]= (gyroBuffer[2]<<8) | gyroBuffer[3];
	pBuffer[2]= (gyroBuffer[4]<<8) | gyroBuffer[5];

}

int main(void)
{
	short accData[3],gyroData[3];

	fd=open(I2C_DEVICE_FILE,O_RDWR);
	if(fd<0){
		perror("You suck. The device file 'i2c-2' couldnt be opened");
		return -1;
	}

	int ret=ioctl(fd,I2C_SLAVE,MPU6050_SLAVE_ADDR);
	if(ret<0){
		perror("You suck. Error when trying to tell ioctl() the fd is in I2C_SLAVE mode and Slave address is: 0x68");
		return -1;
	}

	mpu6050_init();
	while(1){
		mpu6050_read_acc(accData);
		mpu6050_read_gyro(gyroData);

		int counter = 100;

		double accX,accY,accZ;


		accX=(double)accData[0]/ACC_FS_SENSITIVITY_0;
		accY=(double)accData[1]/ACC_FS_SENSITIVITY_0;
		accZ=(double)accData[2]/ACC_FS_SENSITIVITY_0;
		printf("AccX/AccY/AccZ:%0.2f,%0.2f,%0.2f\n",accX,accY,accZ);
		usleep(500);

	}

}
