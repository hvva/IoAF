#include"exreg.h"
#include <tchar.h>
#include<tsk/tsk_tools_i.h>
#include<tsk/fs/tsk_fs_i.h>
#include "parser.h"
#include "sql.h"

int c=0;
int carving_hive(TCHAR * path, TSK_FS_INFO *fs, TSK_INUM_T inode)
{

	char buf[0x2000];
	memset(buf, 0, 0x2000);

	WideCharToMultiByte(CP_ACP, 0, path, TSTRLEN(path) , buf, TSTRLEN(path) , NULL, NULL);
	if(tsk_fs_path2inum(fs, buf, &inode, NULL)){
		tsk_error_print(stderr);
		return 0;
	}

	if(hive_extract(fs, inode, TSK_FS_ATTR_TYPE_DEFAULT, NULL, NULL,NULL, (TSK_FS_FILE_WALK_FLAG_ENUM) 0))
		return 0;
	return 1;
}



uint8_t
	hive_extract(TSK_FS_INFO * fs, TSK_INUM_T inum,
	TSK_FS_ATTR_TYPE_ENUM type, uint8_t type_used,
	uint16_t id, uint8_t id_used, TSK_FS_FILE_WALK_FLAG_ENUM flags)
{
	TSK_FS_FILE *fs_file;

#ifdef TSK_WIN32
	if (-1 == _setmode(_fileno(stdout), 0x8000)) {
		tsk_error_reset();
		tsk_error_set_errno(TSK_ERR_FS_WRITE);
		tsk_error_set_errstr
			("icat_lib: error setting stdout to binary: %s",
			strerror(errno));
		return 1;
	}
#endif

	fs_file = tsk_fs_file_open_meta(fs, NULL, inum);
	if (!fs_file) {
		return 1;
	}

	if (type_used) {
		if (id_used == 0) {
			flags = TSK_FS_FILE_WALK_FLAG_NOID;
		}
		if (tsk_fs_file_walk_type(fs_file, type, id, flags, hive_action,
			NULL)) {
				tsk_fs_file_close(fs_file);
				return 1;
		}
	}
	else {
		if (tsk_fs_file_walk(fs_file, flags, hive_action, NULL)) {
			tsk_fs_file_close(fs_file);
			return 1;
		}
	}


	tsk_fs_file_close(fs_file);

	return 0;
}

TSK_WALK_RET_ENUM
	hive_action(TSK_FS_FILE * fs_file, TSK_OFF_T a_off, TSK_DADDR_T addr,
	char *buf, size_t size, TSK_FS_BLOCK_FLAG_ENUM flags, void *ptr)
{


	if (size == 0){
		return TSK_WALK_CONT;

	}
	FILE *fp = fopen("tmp", "ab+");
	if(fp==NULL)
	{
		printf("file open fail\n");
		exit(-1);
	}
	if (fwrite(buf, size, 1, fp) != 1) {
		tsk_error_reset();
		tsk_error_set_errno(TSK_ERR_FS_WRITE);
		tsk_error_set_errstr("icat_action: error writing to stdout: %s",
			strerror(errno));
		fclose(fp);
		return TSK_WALK_ERROR;
	}
	fclose(fp);
	return TSK_WALK_CONT;
}

void ntuser_hive(TSK_FS_INFO *fs)
{
	TSK_INUM_T inode;
	void * ptr= NULL;
	//	TSK_FS_DIR * dir = tsk_fs_dir_open  (fs,  "/user");
	tsk_fs_path2inum(fs, "/Users", &inode, NULL);
	printf("%d\n", (int)inode);
	tsk_fs_dir_walk(fs, inode, TSK_FS_DIR_WALK_FLAG_NONE, callback, ptr);


}
TSK_WALK_RET_ENUM callback(TSK_FS_FILE * fs_file, const char* path, void * ptr)
{
	char tmp[0x100];
	memset(tmp, 0, 0x100);
	TCHAR filename[0x100];
	memset(filename, 0, 0x100);
	REGQUEUE nq;
	if (fs_file->name->type==TSK_FS_NAME_TYPE_DIR && c>1){
		//printf("%s %s\n", fs_file->name->name, path);
		sprintf(tmp, "/Users/%s/NTUSER.DAT", fs_file->name->name);
		MultiByteToWideChar(CP_ACP, 0, tmp, strlen(tmp), filename, strlen(tmp));
		printf("Starting extract \"%s\"\n", tmp);
		if(carving_hive(filename, fs_file->fs_info, NULL)){
			parser("tmp", &nq, fs_file->name->name);
			unlink("tmp");
			reg2sql(&nq);
		}
	}
	c++;

	reg2sql(&nq);
	return TSK_WALK_CONT;
}
void ie_file(TSK_FS_INFO *fs)
{
	TSK_INUM_T inode;
	void * ptr= NULL;
	tsk_fs_path2inum(fs, "/Users", &inode, NULL);
	tsk_fs_dir_walk(fs, inode, TSK_FS_DIR_WALK_FLAG_NONE, callback2, ptr);


}

TSK_WALK_RET_ENUM callback2(TSK_FS_FILE * fs_file, const char* path, void * ptr)
{
	char tmp[0x1000];
	memset(tmp, 0, 0x1000);
	TCHAR filename[0x1000];
	memset(filename, 0, 0x1000);
	REGQUEUE nq;
	if (fs_file->name->type==TSK_FS_NAME_TYPE_DIR && c>1){
		//printf("%s %s\n", fs_file->name->name, path);

		sprintf(tmp, "/Users/%s/AppData/Local/Microsoft/Windows/History/History.IE5/index.dat", fs_file->name->name);
		MultiByteToWideChar(CP_ACP, 0, tmp, strlen(tmp), filename, strlen(tmp));
		printf("Starting extract \"%s\"\n", tmp);

		if(carving_hive(filename, fs_file->fs_info, NULL)){
			iehist("tmp", 1);
			unlink("tmp");
		}

		memset(tmp, 0, 0x1000);
		memset(filename, 0, 0x1000);
		
		sprintf(tmp, "/Users/%s/AppData/Local/Microsoft/Windows/Temporary Internet Files/Content.IE5/index.dat", fs_file->name->name);
		MultiByteToWideChar(CP_ACP, 0, tmp, strlen(tmp), filename, strlen(tmp));
		printf("Starting extract \"%s\"\n", tmp);

		if(carving_hive(filename, fs_file->fs_info, NULL)){
			iehist("tmp", 1);
			unlink("tmp");
		}

		memset(tmp, 0, 0x1000);
		memset(filename, 0, 0x1000);
		
		sprintf(tmp, "/Users/%s/AppData/Roaming/Microsoft/Windows/Cookies/index.dat", fs_file->name->name);
		MultiByteToWideChar(CP_ACP, 0, tmp, strlen(tmp), filename, strlen(tmp));
		printf("Starting extract \"%s\"\n", tmp);

		if(carving_hive(filename, fs_file->fs_info, NULL)){
			iehist("tmp", 1);
			unlink("tmp");
		}
		memset(tmp, 0, 0x1000);
		memset(filename, 0, 0x1000);

		sprintf(tmp, "/Users/%s/AppData/Roaming/Microsoft/Winodws/IEDownloadHistory/index.dat", fs_file->name->name);
		MultiByteToWideChar(CP_ACP, 0, tmp, strlen(tmp), filename, strlen(tmp));
		printf("Starting extract \"%s\"\n", tmp);

		if(carving_hive(filename, fs_file->fs_info, NULL)){
			iehist("tmp", 2);
			unlink("tmp");
		}

	}
	c++;

	return TSK_WALK_CONT;
}