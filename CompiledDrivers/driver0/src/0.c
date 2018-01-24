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

#define GPIO_PATH "/sys/class/gpio/gpio"
const char* p="/sys/class/gpio/gpio46/direction";
const char* partialDir="/sys/class/gpio/gpio";

int fd;
int main(int argc,char* argv[]){

	//concatinate GPIO_PATH with gpio number of pin being used
	//initialize gpio direction under GPIO_PATH/direction to out
	//write to value file under GPIO_PATH and trigger 1 and 0 in a loop with N iterations
	char src[50], dest[50],gpioValue[50],gpioValueStartPath[50];
	strcpy(gpioValueStartPath,"/sys/class/gpio/gpio");
	strcpy(src,  "/sys/class/gpio/gpio");
    char* gpioNum="46";
    sprintf(dest,"%s/direction",gpioNum);
    sprintf(gpioValue,"%s/value",gpioNum);
    strcat(src, dest);
    strcat(gpioValueStartPath,gpioValue);

    printf("Final destination string : |%s|", gpioValueStartPath);

	fd = open(src,O_RDWR);
	printf("Return value for file open: %d \n",fd);
	char* writeBuf="out";
	int ret = write(fd,writeBuf,4);
	printf("Return value: %d \n",ret);
	close(fd);

	fd=open(gpioValueStartPath,O_RDWR);
	if(fd>0){
		char* gpioOn="1";
		char* gpioOff="0";
		while(1){
			write(fd,gpioOn,2);
			usleep(1000*1000);
			write(fd,gpioOff,2);
			usleep(1000*1000);
		}
	}else{
		printf("File failed to open: %d",fd);
	}
	return 0;


}
