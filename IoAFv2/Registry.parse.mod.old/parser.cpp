#include "parser.h"
int count=0;
void walk ( char* path,   key_block* key ) {
    static  char* root=(char*)key-0x20, *full=path;
    
    memcpy(path++,"/",2); memcpy(path,&key->name,key->len); path+=key->len;
	offsets* item = (offsets*)(root+key->subkeys);
    for(int i=0;i<item->count;i++){
        // in case of too many subkeys this list contain just other lists
        offsets* subitem = (offsets*)((&item->first)[i]+root);

        // usual directory traversal  
        if(item->block_type[1]=='f'||item->block_type[1]=='h') {
            // for now we skip hash codes (used by regedit for faster search)
            walk(path,(key_block*)((&item->first)[i*2]+root));
        } else for(int j=0;j<subitem->count;j++) {
            // also ms had chosen to skip hashes altogether in this case 
            walk(path,(key_block*)((&subitem->first)[item->block_type[1]=='i'?j*2:j]+root));
        }
		printf("%d : %s time : %x%x%x%x%x%x%x%x \n", ++count, full, key->time[0],key->time[1],key->time[2],key->time[3],key->time[4],key->time[5],key->time[6], key->time[7]);
	}
}