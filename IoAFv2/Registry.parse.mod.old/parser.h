#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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
	unsigned char time[8];
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

void walk ( char* path,   key_block* key );

