#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include <queue>

#ifndef _PARSER_H_
#define _PARSER_H_
using namespace std;

struct offsets { 
    long  block_size;
    char  block_type[2]; // "lf" "il" "ri"
    short count;   
    long  first; 
    long  hash; 
};

struct key_block  { 
    long  block_size;
    char  block_type[2]; // "nk"
    char flag[2];
	long long time;
	char dummya[8];
    int   subkey_count;
    char  dummyb[4];
    int   subkeys;
    char  dummyc[4];
    int   value_count;
    int   offsets;
    char  dummyd[28];
    short len;
    short du;
    char  name; 
};

struct value_block {
    long  block_size;
    char  block_type[2]; // "vk"
    short name_len;
    long  size;
    long  offset;
    long  value_type;
    short flags;
    short dummy;
    char  name; 
};

struct
{
	char key[0x1000];
	long long time;
}typedef REGISTRY;

typedef queue<REGISTRY *> REGQUEUE;
void push(REGQUEUE* q, char * path, long long time);
void walk (char* path,   key_block* key, REGQUEUE *q, char *root, char *full);
void walk_nt( char* path,   key_block* key, REGQUEUE *q, char *root, char *full, char* name);
void chkKey(char *full, long long time, REGQUEUE *q);
void chkKey_nt(char *full, long long time, REGQUEUE *q, char* name);

#define WINDOWS_TICK 10000000
#define SEC_TO_UNIX_EPOCH 11644473600LL

unsigned WindowsTickToUnixSeconds(long long windowsTicks);

int parser(char* fpath, REGQUEUE *q);
int parser(char* fpath, REGQUEUE *q, char * name);

#endif