#include"parser.h"

void walk ( char* path,   key_block* key, REGQUEUE *q, char *root, char *full ) {	
	//printf("%lu\n", WindowsTickToUnixSeconds(key->time));
    //add current key name to printed path
	//char buf[0x1000];
	//memset(buf, '\x00', 0x1000);

	memcpy(path++,"/",1); 
	memcpy(path,&key->name,key->len); path+=key->len;
	*path = 0;
	//memset(path,'\x00',full+0x1000-path);
	chkKey(full, key->time, q);
	

    // for simplicity we can imagine keys as directories in filesystem and values
    // as files.
    // and since we already dumped values for this dir we will now iterate 
    // thru subdirectories in the same way

	//if((unsigned int)(key->subkeys) < 0x1000 || (unsigned int)(root+key->subkeys)>size)
	//	return ;
    offsets* item = (offsets*)(root+key->subkeys);

    for(int i=0;i<item->count;i++){
        // in case of too many subkeys this list contain just other lists
        offsets* subitem = (offsets*)((&item->first)[i]+root);

        // usual directory traversal  
        if(item->block_type[1]=='f'||item->block_type[1]=='h') {
            // for now we skip hash codes (used by regedit for faster search)
            walk(path,(key_block*)((&item->first)[i*2]+root), q, root, full);
        } else for(int j=0;j<subitem->count;j++) {
			// also ms had chosen to skip hashes altogether in this case 
			walk(path,(key_block*)((&subitem->first)[item->block_type[1]=='i'?j*2:j]+root), q, root, full);
        }
    }

}
void walk_nt( char* path,   key_block* key, REGQUEUE *q, char *root, char *full, char *name ) {	

	memcpy(path++,"/",1); 
	memcpy(path,&key->name,key->len); path+=key->len;
	*path = 0;
	//memset(path,'\x00',full+0x1000-path);
	chkKey_nt(full, key->time, q, name);
	

    // for simplicity we can imagine keys as directories in filesystem and values
    // as files.
    // and since we already dumped values for this dir we will now iterate 
    // thru subdirectories in the same way

	//if((unsigned int)(key->subkeys) < 0x1000 || (unsigned int)(root+key->subkeys)>size)
	//	return ;
    offsets* item = (offsets*)(root+key->subkeys);

    for(int i=0;i<item->count;i++){
        // in case of too many subkeys this list contain just other lists
        offsets* subitem = (offsets*)((&item->first)[i]+root);

        // usual directory traversal  
        if(item->block_type[1]=='f'||item->block_type[1]=='h') {
            // for now we skip hash codes (used by regedit for faster search)
            walk(path,(key_block*)((&item->first)[i*2]+root), q, root, full);
        } else for(int j=0;j<subitem->count;j++) {
			// also ms had chosen to skip hashes altogether in this case 
			walk(path,(key_block*)((&subitem->first)[item->block_type[1]=='i'?j*2:j]+root), q, root, full);
        }
    }

}
void chkKey(char *full, long long time, REGQUEUE *q)
{

	char SAM[]="/CMI-CreateHive{899121E8-11D8-44B6-ACEB-301713D5ED8C}";
	char SAM64[]="/CMI-CreateHive{C4E7BA2B-68E8-499C-B1A1-371AC8D717C7}";
	char SOFTWARE[]="/CMI-CreateHive{3D971F19-49AB-4000-8D39-A6D9C673D809}";
	char SOFTWARE64[]="/CMI-CreateHive{199DAFC2-6F16-4946-BF90-5A3FC3A60902}";
	char SYSTEM[]="/CMI-CreateHive{F10156BE-0E87-4EFB-969E-5DA29D131144}";
	char SYSTEM64[]="/CMI-CreateHive{2A7FB991-7BBE-4F9D-B91E-7CB51D4737F5}";
	char COMPONENTS[]="/CMI-CreateHive{0AF462BC-5E78-4490-BC7A-5FF5DE05A8F0}";
	char COMPONENTS64[]="/CMI-CreateHive{D9D79F5E-5CCC-4BEF-BA86-8462C417D5BE}";
	char SECURITY[]="/CMI-CreateHive{FE0DCB88-9AD4-44DC-AED8-DCE1C037E9E5}";
	char SECURITY64[]="/CMI-CreateHive{0297523D-E529-4E42-8BE7-E1AABC063C84}";
	char DEFAULT[]="/CMI-CreateHive{1F52C44A-F7D4-499D-9ADF-5B72EB7D8EA4}";
	char DEFAULT64[]="/CMI-CreateHive{BD6FA63F-599C-4F99-99DE-A05742AA2377}";

	char tmp[0x1000];
	memset(tmp, '\x00', 0x1000);
	//printf("%s %d\n", full+53, strlen(full+53));
	//printf("%s\n", full+53);
	if ( !strncmp(SAM, full, 53)){
		sprintf(tmp, "HKLM/sam%s", full+53);
		push(q, tmp, time);
	}
	else if ( !strncmp(SOFTWARE, full, 53) || !strncmp(SOFTWARE64, full, 53)){
		if ( !strncmp("/Classes", full+53, strlen("/Classes"))){
			sprintf(tmp, "HKCR/software%s", full+53);
			push(q, tmp, time);
		}
		memset(tmp, '\x00', sizeof(tmp));
		sprintf(tmp, "HKLM/SOFTWARE%s", full+53);
		push(q, tmp, time);
	}

	else if ( !strncmp(SYSTEM, full, 53) || !strncmp(SYSTEM64, full, 53)){
		sprintf(tmp, "HKLM/system%s", full+53);
		push(q, tmp, time);
	}
	else if (!strncmp(SECURITY, full, 53) || !strncmp(SECURITY64, full, 53)){
		sprintf(tmp, "HKLM/security%s", full+53);
		push(q, tmp, time);
	}
	else if (!strncmp(COMPONENTS, full, 53) || !strncmp(COMPONENTS64, full, 53)){
		sprintf(tmp, "HKLM/components%s", full+53);
		push(q, tmp, time);
	}
	else if (!strncmp(DEFAULT, full, 53) || !strncmp(DEFAULT64, full, 53)){
		sprintf(tmp, "HKU/.default%s", full+53);
		push(q, tmp, time);
	}
	else {
		push(q, full, time);
		return;
		
	}
	
}

void chkKey_nt(char *full, long long time, REGQUEUE *q, char * name)
{
	char NTUSER[]="/CMI-CreateHive{6A1C4018-979D-4291-A7DC-7AED1C75B67C}";
	char NTUSER64[]="/CMI-CreateHive{D43B12B8-09B5-40DB-B4F6-F6DFEB78DAEC}";
	char tmp[0x1000];
	memset(tmp, '\x00', 0x1000);
	//printf("%s\n", full+53);
	if ( !strncmp(NTUSER, full, 53) || !strncmp(NTUSER64, full, 53)){
		if ( !strncmp("/Classes", full+53, strlen("/Classes"))){
			sprintf(tmp, "HKCR/software%s", full+53);
			push(q, tmp, time);
		}
		memset(tmp, '\x00', sizeof(tmp));
		sprintf(tmp, "HKU/%s%s",name, full+53);
		push(q, tmp, time);
	}
	else {
		push(q, full, time);
	}

}