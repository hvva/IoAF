#include "parser.h"

int main(int argc, char** argv) {
    char path[0x1000]={0}, *data; FILE* f; int size;  
    
    if(argc<2||!(f=fopen(argv[1],"rb"))) return printf("hive path err");
    
    fseek(f,0,SEEK_END); 
    if(!(size=ftell(f))) return printf("empty file");
    
    rewind(f); data=(char*)malloc(size); 
    fread(data,size,1,f); 
    fclose(f);

    // we just skip 1k header and start walking root key tree
    walk(path,(key_block*)(data+0x1020));
    free(data);
    return 0;
}
