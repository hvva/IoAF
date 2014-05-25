#include<stdio.h>
#include<malloc.h>
#include<stdio.h>
#ifndef __PARSER_H_
#define __PARSER_H_
#include "parser.h"
#endif


void push(REGQUEUE* q, char * path, long long time)
{
	REGISTRY * reg;
	
	reg=(REGISTRY*)malloc(sizeof(REGISTRY));
	//memset(reg, '\x00', sizeof(REGISTRY));
	sprintf(reg->key, "%s\x00", path);
	reg->time=WindowsTickToUnixSeconds(time);
	q->push(reg);
}