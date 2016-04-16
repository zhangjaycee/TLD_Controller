#define PID_P 1.2
#define PID_D 3.0
#define PID_I 0.01

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
extern int flag_found;
extern int last_dx;
extern int last_dy;
extern int ddx;
extern int ddy;
extern int pid_xsum;
extern int pid_ysum;
extern float adjust_k;
extern int fly_status;
extern int landing_width;

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
    pid_ysum += dy;
	//printf("data [PID]I_ysum%d\n",pid_ysum);
    ddy = dy - last_dy;
    //printf("data [PID]dy = %d,last_dy = %d,ddy = %d",dy , last_dy, ddy); 
    last_dy = dy; 
    if(fly_status != 3){
        dy = adjust_k * (PID_P * dy + PID_D * ddy + PID_I * pid_ysum);
    }
    if(dy<=-128){
        dy =-125;
    }
    if(dy>=127){
        dy = 125;
    }
    //printf("[PID OK] dy = %d\n",dy);
    if(dy<=0){
        gasValue = - dy;
    }
    else{
        gasValue = 128 + dy;
    }
    switch(fly_status){
        case 1:
            printf("data [状态1:初次调整] ");
            break;
        case 2:
            printf("data [状态2:尝试下降] ");
            break;
        case 3:
            printf("data [状态3:再次调整] ");
            break;
        case 4:
            printf("data [状态4:检查调整] ");
            break;
        case 5:
            printf("data [状态5:直接下降] ");
            break;
    }
    //printf("data [adjust_k]= %.4f  [gas]= %d  [y_isum]= %d",adjust_k, dy, pid_ysum);
    printf("data [adjust_k]= %.4f [Y_gas]= %d ",adjust_k, dy);
    gasDeToHex(gasValue);
}
void getDirValue(int dx)
{
    pid_xsum += dx;
	//printf("data [PID]I_xsum%d\n",pid_xsum);
    ddx = dx - last_dx;
    //printf("data [PID]dx = %d,last_dx = %d,ddx = %d",dx , last_dx, ddx); 
    last_dx = dx;
    if(fly_status!=3){
    	dx = adjust_k * (PID_P * dx + PID_D * ddx + PID_I * pid_xsum);
    }
    if(dx<=-128){
        dx =-125;
    }
    if(dx>=127){
        dx = 125;
    }
    //printf("[PID OK] dx = %d\n",dx);
    if(dx>=0){
        dirValue = dx;
    }
    else{
        dirValue = 128-dx;
    }
	printf("data [X_gas]= %d\n", dx);
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
void calControlStr()
{
    //  0   1/2  3/4  5/6   7/8  9/10 11/12  13
    //[ :   RC   xx   oo    xx   oo   xx     / ] 
    //           ^    ^     ^    ^    ^      ^
    //           gas  pitch roll yaw  
    //           高低 前后  左右 偏航 校验
    //           dir  gas        land?
    ctrlStr[0]=':'; ctrlStr[1]='R';ctrlStr[2]='C';
    for(int i=3;i<=10;i++){
        ctrlStr[i]='0';
    }//现在:[: RC 00 00 00 00]
    if(flag_found){ //如果当前帧中目标没有丢失，对字符串赋值，否则保持0
        if(fly_status == 2 || fly_status == 5){//状态2/5:降落[: RC 00 00 00 10 ]
            ctrlStr[9] = '0';
            ctrlStr[10] = 'A';
        }
        else if (fly_status == 1 || fly_status == 3) {//状态1/3:不降落[: RC xx xx 00 00 xx /]
                ctrlStr[5]=dirValueChars[0];
                ctrlStr[6]=dirValueChars[1];
                ctrlStr[7]=gasValueChars[0];
                ctrlStr[8]=gasValueChars[1];
                //crtlStr[9/10]不变
        }
        //else{} //状态4悬停检查
    }else if(flag_found == 0 && fly_status == 5){//没有目标时只有状态5降落
            ctrlStr[9] = '0';
            ctrlStr[10] = 'A';
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
{/*
    switch(fly_status){
        case 1:
            printf("data [状态1:初次调整] ");
            break;
        case 2:
            printf("data [状态2:尝试下降] ");
            break;
        case 3:
            printf("data [状态3:再次调整] ");
            break;
        case 4:
            printf("data [状态4:检查调整] ");
            break;
        case 5:
            printf("data [状态5:直接下降] ");
            break;
    }
    */
    if(!open_error_flag){
        write(fd,ctrlStr, strlen(ctrlStr));
        printf("data [send success: %s] \n",ctrlStr);
    }else{
        printf("data [send failed: %s] \n",ctrlStr);
    }
}
