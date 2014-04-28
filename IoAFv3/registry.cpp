#include "parser.h"

int parser(char* fpath,  REGQUEUE *q) {
    char path[0x1000]={0}, *data; FILE* f; int size;  
    //REGQUEUE q;

    if(!(f=fopen(fpath,"rb"))) return printf("hive path err");
    
    fseek(f,0,SEEK_END); 
    if(!(size=ftell(f))) return printf("empty file");
    
    rewind(f); data=(char*)malloc(size); 
    fread(data,size,1,f); 
    fclose(f);
	//int s=strlen(data);
    // we just skip 1k header and start walking root key tree
	char *root;
	char *full;

	root = data+0x1000;
	full = path;
    walk(path, (key_block*)(data+0x1020), q, root, full);
    free(data);
	/*while (!q->empty())
    {
        printf ("%s : %u\n" , q->front()->key, q->front()->time );
        q->pop();
    }*/
	
    return 0;
}

int parser(char* fpath,  REGQUEUE *q, char * name) {
    char path[0x1000]={0}, *data; FILE* f; int size;  
    //REGQUEUE q;

    if(!(f=fopen(fpath,"rb"))) return printf("hive path err");
    
    fseek(f,0,SEEK_END); 
    if(!(size=ftell(f))) return printf("empty file");
    
    rewind(f); data=(char*)malloc(size); 
    fread(data,size,1,f); 
    fclose(f);
	//int s=strlen(data);
    // we just skip 1k header and start walking root key tree
	char *root;
	char *full;

	root = data+0x1000;
	full = path;
    walk_nt(path, (key_block*)(data+0x1020), q, root, full, name);
    free(data);
	/*while (!q->empty())
    {
        printf ("%s : %u\n" , q->front()->key, q->front()->time );
        q->pop();
	}*/
	
    return 0;
}