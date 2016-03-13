#define PID_P 1.0
#define PID_D 3.0
#define PID_I 0.015

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
extern int flag_landing;
extern int flag_adjust;
extern int flag_found;
extern int last_dx;
extern int last_dy;
extern int ddx;
extern int ddy;
extern int pid_xsum;
extern int pid_ysum;

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
    if(dy<=-128){
        dy =-125;
    }
    if(dy>=127){
        dy = 125;
    }
/************************/
    pid_ysum += dy;
	printf("data [PID]I_ysum%d\n",pid_ysum);
    ddy = dy - last_dy;
    printf("data [PID]dy = %d,last_dy = %d,ddy = %d",dy , last_dy, ddy); 
    last_dy = dy; 
    dy = PID_P * dy + PID_D * ddy + PID_I * pid_ysum;
    if(dy<=-128){
        dy =-125;
    }
    if(dy>=127){
        dy = 125;
    }
    printf("[PID OK] dy = %d\n",dy);
    if(dy<=0){
        gasValue = - dy;
    }
    else{
        gasValue = 128 + dy;
    }
/************************/
    gasDeToHex(gasValue);
}
void getDirValue(int dx)
{
    if(dx<=-128){
        dx =-125;
    }
    if(dx>=127){
        dx = 125;
    }
/************************/
    pid_xsum += dx;
	printf("data [PID]I_xsum%d\n",pid_xsum);
    ddx = dx - last_dx;
    printf("data [PID]dx = %d,last_dx = %d,ddx = %d",dx , last_dx, ddx); 
    last_dx = dx;
    dx = PID_P * dx + PID_D * ddx + PID_I * pid_xsum;
    if(dx<=-128){
        dx =-125;
    }
    if(dx>=127){
        dx = 125;
    }
    printf("[PID OK] dx = %d\n",dx);
/***.............modify*************/
    if(dx>=0){
        dirValue = dx;
    }
    else{
        dirValue = 128-dx;
    }
/************************/
    dirDeToHex(dirValue);
}
/*
void calControlStr(int gasValue,int dirValue)
{
    ctrlStr[0]='$';
    ctrlStr[1]=gasValue;
    ctrlStr[2]=dirValue;
    ctrlStr[3]='\0';
}*/
void calControlStr(int gasValue,int dirValue)
{
    //  0   1/2  3/4  5/6   7/8  9/10 11/12  13
    //[ :   RC   xx   oo    xx   oo   xx     / ] 
    //           ^    ^     ^    ^    ^      ^
    //           gas  pitch roll yaw  
    //           高低 前后  左右 偏航 校验
    //           dir  gas        land?
    /*************v4 移动到了TLD.cpp控制这些flag*****************
    int tmp_gas = gasValue;
    int tmp_dir = dirValue;
    if (gasValue > 128){
        tmp_gas -= 128;
    }
    if (dirValue > 128){
        tmp_dir -= 128;
    }
    printf("data [gas]abs(dy)=%d,[dir]abs(dx)=%d\n",tmp_gas,tmp_dir);
    //现在tmp_dir和tmp_gas都为dx或dy的绝对值
    if (tmp_dir < 5 && tmp_gas < 5){
        flag_landing = 1;
        
    }
    *************v4 移动到了TLD.cpp控制这些flag******************/
    ctrlStr[0]=':'; ctrlStr[1]='R';ctrlStr[2]='C';
    for(int i=3;i<=10;i++){
        ctrlStr[i]='0';
    }//现在:[: RC 00 00 00 00]
    if(flag_landing == 1 && flag_adjust == 0){//状态2：降落标志为1，并且没有处于调整状态:[: RC 00 00 00 10 ]
        ctrlStr[9] = '0';
        ctrlStr[10] = 'A';
    }
    else if (flag_landing == 0 || flag_adjust == 1) {//状态1与状态3:降落标志是0:[: RC xx xx 00 00 xx /]
        if(flag_found){
            ctrlStr[5]=dirValueChars[0];
            ctrlStr[6]=dirValueChars[1];
            ctrlStr[7]=gasValueChars[0];
            ctrlStr[8]=gasValueChars[1];
            //crtlStr[9/10] is 0
        }
    }
    ctrlStr[11]='\0';
    calLast2(ctrlStr);
    ctrlStr[11]=last2[0];
    ctrlStr[12]=last2[1];
    ctrlStr[13]='/';
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
        printf("data [send success] %s\n",ctrlStr);
    }else
        printf("data [send failed] %s\n",ctrlStr);
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
