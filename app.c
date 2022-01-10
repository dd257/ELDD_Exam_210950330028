#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/types.h>
#include<asm/ioctl.h>
#include"ioctl.h"

struct stats
{
    int size;
    char buff[100];
    int r_w;
};

int main(int argc, char *argv[])
{
    int fd,result;
    char Ubuff[100],Kbuff[100]={};

    printf("Enter User buffer: ");
    scanf("%s",Ubuff);

    fd=open("/dev/lddexam",O_RDWR|O_CREAT,0777);

    if(fd<0)
    {
        perror("Error");
        return fd;
    }

    write(fd,Ubuff,sizeof(Ubuff));
    if(Ubuff=="GETSTATS")
        result=ioctl(fd,GETSTATS);
    else
        result=ioctl(fd,DEFAULT);

    if(result<0)
    {
        perror("Error");
        return result;
    }

    read(fd,(struct stat *)Kbuff,sizeof(struct stat));
    printf("Status: %s",Kbuff);
}
