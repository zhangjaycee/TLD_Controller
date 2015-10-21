#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include "sender.h"

extern char ctrlStr[20];
extern struct termios options ,oldoptions;
extern struct termios options ,oldoptions;
extern int fd;
extern int open_error_flag;

//gas:forward/backward
//dir:left/right

extern char * lock;
extern char * unlock;
extern int gasValue;
extern int dirValue;
extern char gasValueChars[2];
extern char dirValueChars[2];
extern char last2[2];
extern char ctrlStr[20];


void gasDeToHex(int gasValue)
{

    int wei[2];
    int i=0;
    wei[0]=gasValue/16;
    wei[1]=gasValue%16;
    for(i=0;i<2;i++){
        if(wei[i]>9){
            gasValueChars[i]='A'+wei[i]-10;
        }else{
            gasValueChars[i]='0'+wei[i];
        }
    }
}
void dirDeToHex(int dirValue)
{

    int wei[2];
    int i=0;
    wei[0]=dirValue/16;
    wei[1]=dirValue%16;
    for(i=0;i<2;i++){
        if(wei[i]>9){
            dirValueChars[i]='A'+wei[i]-10;
        }else{
            dirValueChars[i]='0'+wei[i];
        }
    }
}
void calLast2(char * str){
    int i,n,sum=0;
    int high,low;
    n=strlen(str);
//printf("strlen:%d\n",n=strlen(str));
    for(i=1;i<n;i++){
        sum+=str[i];
    }
    sum=256-(sum&0xff);
    high=(sum&0x0f0)/16;
    low=sum&0x0f;
    if(low<10)
        low+=48;
    else
        low+=55;
    if(high<10)
        high+=48;
    else
        high+=55;
    //printf("last2:%c%c\n",high,low);
    last2[0]=high;
    last2[1]=low;
}

void getGasValue(int dy)
{
    /*
   if(dy>0){
        if(dy<30){
            gasValue=dy*4;
        }else if(dy<60){
            gasValue=(dy-30)*1+120;
        }else if(dy<70){
            gasValue=(dy-60)*0.8+150;
        }else if(dy>=70){
            gasValue=160;
        }
    }else{
        gasValue=0;
    }*/
    if(dy>0){
        gasValue = dy;
    }
    else{
        gasValue = 128-dy;
    }
    gasDeToHex(gasValue);
}
void getDirValue(int dx)
{
    if(dx>0){
       dirValue=70;
        // if(dx>125)
          //  dx=125;
      //  dirValue=128+dx-20;
    }else{
     dirValue=197;
        //  if(dx<-125)
        //   dx=-125;
      // dirValue=-dx+20;
    }
    dirDeToHex(dirValue);
}
/*
void calControlStr(int gasValue,int dirValue)
{
    ctrlStr[0]='$';
    ctrlStr[1]=gasValue;
    ctrlStr[2]=dirValue;
<<<<<<< HEAD
    ctrlStr[3]=(gasValue+dirValue)&0xff;
    ctrlStr[4]='\0';
=======
    ctrlStr[3]='\0';
}*/
void calControlStr(int gasValue,int dirValue)
{
    ctrlStr[0]=':'; ctrlStr[1]='R';ctrlStr[2]='C';
    for(int i=3;i<=10;i++){
        ctrlStr[i]='0';
    }
    ctrlStr[5]=gasValueChars[0];ctrlStr[6]=gasValueChars[1];
    ctrlStr[7]=dirValueChars[0];
    ctrlStr[8]=dirValueChars[1];
    //crtlStr[9/10] is 0
    ctrlStr[11]='\0';
    calLast2(ctrlStr);
    ctrlStr[11]=last2[0];
    ctrlStr[12]=last2[1];
    ctrlStr[13]='/';
>>>>>>> 16cdf4fd7c5c699175333067b396f129f839d457
}


void senderInit(){
    fd = open("/dev/ttyUSB0", O_RDWR);
    if(fd <0){
        printf("fd=%d,open error!\n",fd);
        open_error_flag=1;
    }

    tcgetattr(fd, &oldoptions);
    tcgetattr(fd, &options);
    cfsetispeed(&options, B57600);
    cfsetospeed(&options, B57600);

    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_oflag &= ~OPOST;

    tcsetattr(fd, TCSANOW, &options);
}

void sendControlStr()
{
    if(!open_error_flag){
        write(fd,ctrlStr, strlen(ctrlStr));
        printf("[send success] %s\n",ctrlStr);
    }else
        printf("[send failed] %s\n",ctrlStr);
}
/*
int main()
{
    int dx=20;
    int dy=20;
    char q;
    senderInit();
    while(1){
        dx=(dx+1)%100;
        dy=(dy+1)%100;
        getGasValue(dy);
        getDirValue(dx);
        calControlStr(gasValue,dirValue);
        sendControlStr();
        q=getchar();
        if(q=='q'){
            break;
        } 
    }
    return 0;
}

*/
