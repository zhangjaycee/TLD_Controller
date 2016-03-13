#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>


void gasDeToHex(int gasValue);
void dirDeToHex(int dirValue);
void calLast2(char * str);
void getGasValue(int dy);
void getDirValue(int dx);
void calControlStr();
void senderInit();
void sendControlStr();
