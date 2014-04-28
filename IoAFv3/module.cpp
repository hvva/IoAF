#include "module.h"
#include "sql.h"
#include <tsk\tsk_tools_i.h>
#include "parser.h"
#include "exreg.h"
#include "define.h"
#include "match.h"
/*
void ie_hist_module(TSK_FS_INFO *fs)
{
	if (carving_hive(L"index_history.dat", fs, NULL))
	{
		iehist("index_history.dat", 1);
		unlink("tmp");
	}
	if (carving_hive(L"index_cache.dat", fs, NULL))
	{
		iehist("index_history.dat", 1);
		unlink("tmp");
	}
	if (carving_hive(L"index_cookie.dat", fs, NULL))
	{
		iehist("index_history.dat", 1);
		unlink("tmp");
	}
	if (carving_hive(L"index_download.dat", fs, NULL))
	{
		iehist("index_history.dat", 1);
		unlink("tmp");
	}
}
*/

void js_create_module(int Timeline_num)
{
	js_atime_create(Timeline_num);
	js_mtime_create(Timeline_num);
	js_ctime_create(Timeline_num);
	js_detection_info();
	js_toolinfo();
}


void mft_live_module(TCHAR *volName, TCHAR *CaseName)
{
	char* ctmp;
	int len; 
	len = WideCharToMultiByte(CP_ACP, 0, volName, -1, NULL, 0, NULL,NULL);
	ctmp = new char[len];
	WideCharToMultiByte(CP_ACP, 0, volName, -1, ctmp, len, NULL, NULL);

	char* ctmp2;
	int len2; 
	len2 = WideCharToMultiByte(CP_ACP, 0, CaseName, -1, NULL, 0, NULL,NULL);
	ctmp2 = new char[len2];
	WideCharToMultiByte(CP_ACP, 0, CaseName, -1, ctmp2, len2, NULL, NULL);

	mft_live(ctmp, ctmp2);   // mft_live("Volname", "CaseName");
}

void mft_image_module(TCHAR * ImagePath, TCHAR * CaseName)
{

	char* ctmp;
	int len; 
	len = WideCharToMultiByte(CP_ACP, 0, CaseName, -1, NULL, 0, NULL,NULL);
	ctmp = new char[len];
	WideCharToMultiByte(CP_ACP, 0, CaseName, -1, ctmp, len, NULL, NULL);

	//char case_path[MAX_PATH] = {0,};
	//sprintf(case_path, ".\/%s\/image.mft", ctmp);

	if (freopen("image.mft", "w", stdout) == NULL)
			fprintf(stderr, "error redirecting stdout\n");
	mft_image(ImagePath);
	fclose (stdout);
	freopen("CON", "w", stdout);
	//printf("image.mft 생성 완료\n");

	mft_image2db("image.mft", ctmp);
	
	if ( remove("image.mft") == -1 )
		perror ( "Could not delete image.mft file\n");

}
int open_fs_live(TCHAR *volName, TSK_IMG_INFO **img,TSK_FS_INFO **fs )
{
	TSK_IMG_TYPE_ENUM imgtype = TSK_IMG_TYPE_DETECT;
	TSK_FS_TYPE_ENUM fstype = TSK_FS_TYPE_DETECT;
	TSK_TCHAR *drivename;
	TSK_INUM_T inode=0;
//	TSK_FS_NAME *fName;
	UINT sectorSize=0;
	TSK_FS_ATTR_TYPE_ENUM TYPE = TSK_FS_ATTR_TYPE_DEFAULT;
	char path[]="tmp";
//	fName=(TSK_FS_NAME*)malloc(sizeof(TSK_FS_NAME));

	drivename = volName;
	
	if((*img = tsk_img_open(1, &drivename, imgtype, sectorSize)) == NULL ){
		tsk_error_print(stderr);
		exit(EXIT_FAILURE);
	}

	if((*fs=tsk_fs_open_img(*img, 0, fstype)) == NULL){
		tsk_error_print(stderr);
		if(tsk_error_get_errno()==TSK_ERR_FS_UNSUPTYPE)
			tsk_fs_type_print(stderr);
		(*img)->close(*img);
		exit(EXIT_FAILURE);
	}

	return 0;
}

int open_fs_image(TCHAR *ImagePath, int offset, TSK_IMG_INFO **img,TSK_FS_INFO **fs )
{
	
	TSK_IMG_TYPE_ENUM imgtype = TSK_IMG_TYPE_DETECT;
	TSK_FS_TYPE_ENUM fstype = TSK_FS_TYPE_DETECT;
	TSK_TCHAR *drivename;
	TSK_INUM_T inode=0;
	UINT sectorSize=0;
	TSK_FS_ATTR_TYPE_ENUM TYPE = TSK_FS_ATTR_TYPE_DEFAULT;
	char path[]="tmp";
	drivename = ImagePath;
	
	if((*img = tsk_img_open(1, &drivename, imgtype, sectorSize)) == NULL ){
		tsk_error_print(stderr);
		exit(EXIT_FAILURE);
	}

	if((*fs=tsk_fs_open_img(*img, offset*(*img)->sector_size, fstype)) == NULL){
		tsk_error_print(stderr);
		if(tsk_error_get_errno()==TSK_ERR_FS_UNSUPTYPE)
			tsk_fs_type_print(stderr);
		(*img)->close(*img);
		exit(EXIT_FAILURE);
	}
	return 0;
}


void reg_live_module(TSK_FS_INFO *fs)
{
	
	//TCHAR parser_path[256];
	//memset(parser_path, '\x00',wcslen(parser_path));
	//_stprintf(parser_path, _TSK_T("%s\\tmp"), CaseName);
	REGQUEUE q;
	//char Ppath[256];
	//memset(Ppath, '\x00', strlen(Ppath));
	//WideCharToMultiByte(CP_ACP, 0, parser_path, wcslen(parser_path), Ppath, wcslen(parser_path), NULL, NULL);
	ntuser_hive(fs);
	if(carving_hive(_TSK_T("/Windows/System32/config/SYSTEM"), fs, NULL))
	{
		parser("tmp", &q);
		unlink("tmp");
	}
	if(	carving_hive(_TSK_T("/Windows/System32/config/SOFTWARE"), fs, NULL))
	{
		parser("tmp", &q);
		unlink("tmp");
	}
	if (carving_hive(_TSK_T("/Windows/System32/config/DEFAULT"), fs, NULL))
	{
		parser("tmp", &q);
		unlink("tmp");
	}
	if (carving_hive(_TSK_T("/Windows/System32/config/SAM"), fs, NULL))
	{
		parser("tmp", &q);
		unlink("tmp");
	}
	if (carving_hive(_TSK_T("/Windows/System32/config/SECURITY"), fs, NULL))
	{
		parser("tmp", &q);
		unlink("tmp");
	}

	if (!q.empty()) 	reg2sql(&q);
}
void img_fs_close(TSK_IMG_INFO **img,TSK_FS_INFO **fs )
{
	(*fs)->close(*fs);
	(*img)->close(*img);
}

void reg_image_module(TSK_FS_INFO *fs)
{
	
	//TCHAR parser_path[256];
	//memset(parser_path, '\x00',wcslen(parser_path));
	//_stprintf(parser_path, _TSK_T("%s\\tmp"), CaseName);

	REGQUEUE q;
	//char Ppath[256];
	//memset(Ppath, '\x00', strlen(Ppath));
	//WideCharToMultiByte(CP_ACP, 0, parser_path, wcslen(parser_path), Ppath, wcslen(parser_path), NULL, NULL);
	ntuser_hive(fs);
	if(carving_hive(_TSK_T("/Windows/System32/config/SYSTEM"), fs, NULL))
	{
		parser("tmp", &q);
		unlink("tmp");
	}
	if(	carving_hive(_TSK_T("/Windows/System32/config/SOFTWARE"), fs, NULL))
	{
		parser("tmp", &q);
		unlink("tmp");
	}
	if (carving_hive(_TSK_T("/Windows/System32/config/DEFAULT"), fs, NULL))
	{
		parser("tmp", &q);
		unlink("tmp");
	}
	if (carving_hive(_TSK_T("/Windows/System32/config/SAM"), fs, NULL))
	{
		parser("tmp", &q);
		unlink("tmp");
	}
	if (carving_hive(_TSK_T("/Windows/System32/config/SECURITY"), fs, NULL))
	{
		parser("tmp", &q);
		unlink("tmp");
	}

	if (!q.empty()) 	reg2sql(&q);

}
void deepscan(TCHAR * CaseName)
{
	SIGREGQ rq;
	SIGFILEQ fq;
	MATCHREGQ mq;
	MATCHFILEQ mfq;

	get_signature_REG(&rq);
	get_signature_FILE(&fq);
	Deep_matching_REG(&rq, &mq);
	Deep_matching_FILE(&fq, &mfq);
	REG2SQL(&mq);
	FILE2SQL(&mfq);
	tool_info();
}
void scan(TCHAR * CaseName)
{
	SIGREGQ rq;
	SIGFILEQ fq;
	MATCHREGQ mq;
	MATCHFILEQ mfq;

	get_signature_REG(&rq);
	get_signature_FILE(&fq);
	matching_REG(&rq, &mq);
	matching_FILE(&fq, &mfq);
	REG2SQL(&mq);
	FILE2SQL(&mfq);
	tool_info();
}

void usb_info()
{
	USBq q;
	get_USB(&q);
	USBG2SQL(&q);
}