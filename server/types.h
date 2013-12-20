/*
 * ******************************************
 * * module name: types header file
 * * author: PuMaoyang
 * * e-mail: pumaoyang1988@163.com
 * * create date: 2013-09-18
 * * modify date: 
 * * modify log: 
 * * ***************************************
 * */

#ifndef _TYPES_H_2013_09_17
#define _TYPES_H_2013_09_17


#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <pthread.h>

#define u_char unsigned char
#define u_int unsigned int
#define u_short unsigned short

#pragma pack(1)

typedef struct tagData
{
	int iSize;
	char pData[0];
}tagData;
const short g_hdDataSize = sizeof(tagData);

#pragma pack()

#endif
