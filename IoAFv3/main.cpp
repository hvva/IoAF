#include <stdio.h>
#include <Windows.h>
#include <tchar.h>
#include <tsk\libtsk.h>
#include <tsk\tsk_tools_i.h>
#include <direct.h>  
#include "sql.h"
#include "module.h"

static TSK_TCHAR *progname;

static uint8_t print_bytes = 0;
static uint8_t recurse = 0;

static int recurse_cnt = 0;
static TSK_DADDR_T recurse_list[64];
static void print_header(const TSK_VS_INFO * vs);
static TSK_WALK_RET_ENUM part_act(TSK_VS_INFO * vs, const TSK_VS_PART_INFO * part, void *ptr);
int info_offset(TCHAR *drive);
void usage();
void drive_info(TCHAR *drive);
void drive_info_live(void);

void _tmain(int argc, TSK_TCHAR **argv)
{
	system("mode con:cols=110 lines=50");

	int opt=0;
	int deepchk=0;
	int live=1;
	TSK_TCHAR *ImagePath = NULL;
	TSK_TCHAR *CaseName = NULL;
	TSK_TCHAR *DriveName = NULL;
	TSK_TCHAR *VolumeName = NULL;

	int Timeline_num = 50;
	int offset = -1;

	int set_offset = 0;


	/*   
		Live Analysis Mode:

			C:\>IOAF.exe -n "case_name" -v "Volume_name" 
			ex) IOAF.exe -n "Case123" -v "C:"


		Image Analysis Mode:

			C:\>IOAF.exe -n "case_name" -i "Disk Image Path" -v "C:" -o "Offset" 
	
			ex) IOAF.exe -n "Case123" -i "C:\saple.dd" 
				IOAF.exe -n "Case123" -i "C:\sample.dd" -o "0204"


		options:
			-n : Case Name 입력              (*Default : Computer Name )
			-v : 라이브모드 분석할 논리 드라이브명 입력 
			-i : 디스크 이미지 경로 입력
			-d : Deep Scan 모드           
			-o : 디스크 이미지 Offset 설정		 (-i 옵션만 사용 시 Offset정보 확인 가능)			
			-t : TIMELINE 로그 갯수 지정	 (*Default : 50 )
	*/

    if(argc < 2){
		usage();
        exit(0);
    }


	TCHAR computerName[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD size = sizeof(computerName) / sizeof(computerName[0]);
	GetComputerName(computerName, &size);

	CaseName = computerName;

	//while(-1 != (opt = GETOPT(argc, argv, _TSK_T("h:v:lo:d:n:"))))
	while ((opt = GETOPT(argc, argv, _TSK_T("n:v:i:o:d:t:l"))) != -1) 
	{
		switch(opt) 
		{ 

		case _TSK_T('n') :
			CaseName = OPTARG;
			break;

		case _TSK_T('v') :	// 라이브 분석 옵션
			VolumeName = OPTARG;
			break;

		case _TSK_T('i') :	// 디스크 이미지 분석 옵션
			live = 0;  // 라이브모드 사용 안함 설정
			ImagePath = OPTARG;
			break;

		case _TSK_T('o') :	// 디스크 이미지 분석 옵션
			set_offset = 1;
			offset = _ttoi(OPTARG);
			break;

		case _TSK_T('d') :
			deepchk = 1;
			break;

		case _TSK_T('t') : 
			Timeline_num = _ttoi(OPTARG);
			break;

		case _TSK_T('l') :
			drive_info_live();
			break;

		default : 
			printf("Invalid Argument(Default).\n");
			usage();
		}
	}

	/////////////////////////  옵션 예외처리 작업 부분.
	
	char* casename_char;
	int len; 
	len = WideCharToMultiByte(CP_ACP, 0, CaseName, -1, NULL, 0, NULL,NULL);
	casename_char = new char[len];
	WideCharToMultiByte(CP_ACP, 0, CaseName, -1, casename_char, len, NULL, NULL);

	mkdir(casename_char);

	if(live == 0 && set_offset == 0)	// -i 옵션만 사용 시 디스크 이미지 정보 출력.
	{
		drive_info(ImagePath);
	}


	if( access("info.db", 0) == NULL){
		if ( remove("info.db") == -1 )
			perror ( "Could not delete info.db\n");
	}

	if( access("image.mft", 0) == NULL){
		if ( remove("image.mft") == -1)
			perror ( "Could not delete image.mft\n");
	}

	if( access("tmp",0)== NULL){
		if ( remove("tmp") == -1 )
			perror ( "Could not delete tmp file\n");
	}

	//////////////////// 분석 기능 작동 .

	if ( live == 1 && VolumeName != NULL && CaseName != NULL)
	{
		system("cls");
		TCHAR tmp[0x100];
		TSK_IMG_INFO *img;
		TSK_FS_INFO *fs;

		memset(tmp, 0, 0x100);
		printf("Extracting MFT ...\n");

		mft_live_module(VolumeName, CaseName);
		memset(tmp, 0, 0x100);
		wsprintf(tmp, L"\\\\.\\%s", VolumeName);

		if(!open_fs_live(tmp, &img, &fs)){
			printf("Extracting Registry ...\n");
			reg_live_module(fs);

			printf("Extracting ie history ...\n");
			ie_file(fs);
			//usb_info();
			img_fs_close(&img, &fs);


		}

	}
	
	//else if( live == 0 && ImagePath != NULL && CaseName != NULL)
	else if( live == 0 && ImagePath != NULL && offset != -1 && CaseName != NULL)
	{
		system("cls");
		
		TCHAR tmp[0x100];
		TSK_IMG_INFO * img;
		TSK_FS_INFO * fs;

		if(!open_fs_image(ImagePath, offset, &img, &fs)){
			printf("Extracting MFT ...\n");
			mft_image_module(ImagePath, CaseName);
			printf("Extracting Registry ...\n");
			reg_image_module(fs);
			printf("Extracting ie history ...\n");
			ie_file(fs);
			img_fs_close(&img, &fs);
			usb_info();

		}
	}

	else
	{
		printf("Invaild Operation Mode.\n");
		usage();
	}


	system("cls");
	
	if ( deepchk == 1)
	{
		printf("\t\t @@@ Detected Pattern @@@ \t\t\n");
		printf("\n");

		deepscan(CaseName);
		js_create_module(Timeline_num);
	}

	else
	{
		printf("Detected Pattern\n");
		printf("\n\n");
		scan(CaseName);
		js_create_module(Timeline_num);
	}

	// STDOUT 스트림 출력을 방지하기 위해서.
   FILE *stream ;
   if((stream = freopen("file.txt", "w", stdout)) == NULL)
	   exit(-1);


	// CaseName 폴더로 파일들 이동!..
	WCHAR text1[100];
	GetCurrentDirectory(100, text1);

	char* text1_char;
	int len11; 
	len11 = WideCharToMultiByte(CP_ACP, 0, text1, -1, NULL, 0, NULL,NULL);
	text1_char = new char[len11];
	WideCharToMultiByte(CP_ACP, 0, text1, -1, text1_char, len11, NULL, NULL);

    char *buffer = (char *)malloc(500);
	buffer = (char *)malloc(500);
    memset(buffer, 0x00, sizeof(char)*500);
	sprintf(buffer, "robocopy %s\\timeline_ajax %s\\%s\\timeline_ajax /MIR", text1_char, text1_char, casename_char );
	system(buffer);

	buffer = (char *)malloc(500);
    memset(buffer, 0x00, sizeof(char)*500);
	sprintf(buffer, "robocopy %s\\import_files %s\\%s\\import_files /MIR", text1_char, text1_char, casename_char );
	system(buffer);

	buffer = (char *)malloc(500);
    memset(buffer, 0x00, sizeof(char)*500);
	sprintf(buffer, "copy Report.html %s", casename_char);
	system(buffer);
	free(buffer);


//////////////////////////////////////////////////////////////////////////////

	buffer = (char *)malloc(500);
    memset(buffer, 0x00, sizeof(char)*500);
	sprintf(buffer, "move info.db %s", casename_char);
	system(buffer);
	free(buffer);

	// JS 모듈 이동

	buffer = (char *)malloc(500);
    memset(buffer, 0x00, sizeof(char)*500);
	sprintf(buffer, "move A_data.js %s", casename_char);
	system(buffer);
	free(buffer);

		buffer = (char *)malloc(500);
    memset(buffer, 0x00, sizeof(char)*500);
	sprintf(buffer, "move C_Data.js %s", casename_char);
	system(buffer);
	free(buffer);

		buffer = (char *)malloc(500);
    memset(buffer, 0x00, sizeof(char)*500);
	sprintf(buffer, "move M_data.js %s", casename_char);
	system(buffer);
	free(buffer);

		buffer = (char *)malloc(500);
    memset(buffer, 0x00, sizeof(char)*500);
	sprintf(buffer, "move Detection_info.js %s", casename_char);
	system(buffer);
	free(buffer);

		buffer = (char *)malloc(500);
    memset(buffer, 0x00, sizeof(char)*500);
	sprintf(buffer, "move jstopmake.js %s", casename_char);
	system(buffer);
	free(buffer);

	//close(stream);
	fclose(stream);

	if ( remove("file.txt") == -1 )
		perror ( "Could not delete info.db\n");

}

void usage()
{
	//printf("-h : help\n-n : case name\n-o : offset\n-i <disk>: disk info\n-I <image path>: image mode \n-v : volume \n\n");
	printf("\n\n");
	printf("         ueeeeeeeeee           eeeeeeeeeeX                   ee              eeeeeeeeeeeeeeeeeeeee  \n");
	printf("         eeeeeeeeeee        eeeeee       eee               ,eeeX             eeeeeeeeeee            \n");
	printf("         eeeeeeeeeee      eeeeeeee         ee             Geeeee             eeeeeeeeee             \n");
	printf("         eeeeeeeeee     eeeeeeeeeE          9e           eeeeeeee           Weeeeeeeeee             \n");
	printf("        ueeeeeeeeee    eeeeeeeeee            eW         eeeeeeeeeu          eeeeeeeeeeeeeeeeeeeee   \n");
	printf("        eeeeeeeeeee   eeeeeeeeeee            ee       Geeeeeeeeeee          eeeeeeeeee              \n");
	printf("        eeeeeeeeee,   eeeeeeeeeee            ee      eDeeeeeeeeeeee         eeeeeeeeee              \n");
	printf("        eeeeeeeeee   eeeeeeeeeee             eD    ueu  eeeeeeeeeee        Eeeeeeeeeee              \n");
	printf("       eeeeeeeeeee   eeeeeeeeeee             e    ee    eeeeeeeeeeee       eeeeeeeeeeK              \n");
	printf("       eeeeeeeeeez   eeeeeeeeeee            eK   ee      eeeeeeeeeeeE      eeeeeeeeee               \n");
	printf("       eeeeeeeeee    WeeeeeeeeeD           e9   eK       zeeeeeeeeeee     Xeeeeeeeeee               \n");
	printf("      #eeeeeeeeee     eeeeeeeee          ze,  Xe,  Kuuuu  eeeeeeeeeeee    eeeeeeeeeeG               \n");
	printf("      eeeeeeeeeee      eeeeeeee        Eee   eeGeeeeeeeeee9eeeeeeeeeeey   eeeeeeeeee                \n");
	printf("      eeeeeeeeee         eeeeeG    ,eeez    ee             eeeeeeeeeeee  eeeeeeeeeee                \n");
	printf("      9       y              9eeeeeDK   yX 9           e   W                   E   K                \n");
	printf("     eeKe e e e e e9 W,eyee e  e,  ee  Xe   ee  e e eDee  ee  eu ee eue9e e e  e e                  \n");
	printf("     eDee9eue e e e  e,e eDGe We   ee  ee ,5 eeee e e eG  ee  e  ee e  eez    BOB 2nd Contest stage \n");
	printf("\n\n");
	printf("====================================================================================================\n");
	
	printf("\n");
	printf("Default Options:\n");
	printf("	-n : Case Name 입력              (* Default : Computer Name )\n");
	printf("	-d : Deep Scan 모드         \n");
	printf("	-t : TIMELINE 로그 갯수 지정	 (* Default : 50 )\n\n");

	printf("Live Options:\n");
	printf("	-l : 라이브 드라이브 정보 출력 \n");
	printf("	-v : 분석할 논리 드라이브명 입력 \n\n");
	printf("	C:\> IOAF.exe -n \"case_name\" -v \"Volume_name\"\n");
	printf("	ex) IOAF.exe -n \"Case123\" -v \"C:\"\n\n");

	printf("Image Options:\n");
	printf("	-i : 디스크 이미지 경로 입력\n");
	printf("	-o : Offset 설정		 (-i 옵션만 사용 시 Offset정보 확인 가능)\n\n");
	printf("	C:\> IOAF.exe -n \"case_name\" -i \"Disk Image Path\" -o \"Offset\"\n");
	printf("	ex) IOAF.exe -n \"Case123\" -i \"C:\saple.dd\"\n");
	printf("	    IOAF.exe -n \"Case123\" -i \"C:\sample.dd\" -o \"2048\"\n\n");

	printf("====================================================================================================\n");
	
	exit(0);
}


static TSK_WALK_RET_ENUM part_act(TSK_VS_INFO * vs, const TSK_VS_PART_INFO * part, void *ptr)
{
	if (part->flags & TSK_VS_PART_FLAG_META)
		tsk_printf("%.2" PRIuPNUM ":  Meta    ", part->addr);

	/* Neither table or slot were given */
	else if ((part->table_num == -1) && (part->slot_num == -1))
		tsk_printf("%.2" PRIuPNUM ":  -----   ", part->addr);

	/* Table was not given, but slot was */
	else if ((part->table_num == -1) && (part->slot_num != -1))
		tsk_printf("%.2" PRIuPNUM ":  %.2" PRIu8 "      ",
		part->addr, part->slot_num);

	/* The Table was given, but slot wasn't */
	else if ((part->table_num != -1) && (part->slot_num == -1))
		tsk_printf("%.2" PRIuPNUM ":  -----   ", part->addr);

	/* Both table and slot were given */
	else if ((part->table_num != -1) && (part->slot_num != -1))
		tsk_printf("%.2" PRIuPNUM ":  %.2d:%.2d   ",
		part->addr, part->table_num, part->slot_num);

	if (print_bytes) {
		TSK_OFF_T size;
		char unit = 'B';
		size = part->len * part->vs->block_size;

		if (size > 1024) {
			size /= 1024;
			unit = 'K';
		}

		if (size > 1024) {
			size /= 1024;
			unit = 'M';
		}

		if (size > 1024) {
			size /= 1024;
			unit = 'G';
		}

		if (size > 1024) {
			size /= 1024;
			unit = 'T';
		}


		/* Print the layout */
		tsk_printf("%.10" PRIuDADDR "   %.10" PRIuDADDR "   %.10" PRIuDADDR
			"   %.4" PRIuOFF "%c   %s\n", part->start,
			(TSK_DADDR_T) (part->start + part->len - 1), part->len, size,
			unit, part->desc);
	}
	else {
		/* Print the layout */
		tsk_printf("%.10" PRIuDADDR "   %.10" PRIuDADDR "   %.10" PRIuDADDR
			"   %s\n", part->start,
			(TSK_DADDR_T) (part->start + part->len - 1), part->len,
			part->desc);
	}

	if ((recurse) && (vs->vstype == TSK_VS_TYPE_DOS)
		&& (part->flags == TSK_VS_PART_FLAG_ALLOC)) {
			if (recurse_cnt < 64)
				recurse_list[recurse_cnt++] = part->start * part->vs->block_size;
	}

	return TSK_WALK_CONT;
}

static void print_header(const TSK_VS_INFO * vs)
{
	tsk_printf("%s\n", tsk_vs_type_todesc(vs->vstype));
	tsk_printf("Offset Sector: %" PRIuDADDR "\n",
		(TSK_DADDR_T) (vs->offset / vs->block_size));
	tsk_printf("Units are in %d-byte sectors\n\n", vs->block_size);
	if (print_bytes)
		tsk_printf
		("     Slot    Start        End          Length       Size    Description\n");
	else
		tsk_printf
		("     Slot    Start        End          Length       Description\n");
}


int info_offset(TCHAR *drive)
{
	TSK_VS_INFO *vs;
	int ch;
	TSK_OFF_T imgaddr = 0;
	int flags = 0;
	TSK_IMG_INFO *img;
	TSK_IMG_TYPE_ENUM imgtype = TSK_IMG_TYPE_DETECT;
	TSK_VS_TYPE_ENUM vstype = TSK_VS_TYPE_DETECT;
	uint8_t hide_meta = 0;
	TSK_TCHAR **argv;
	unsigned int ssize = 0;
	TSK_TCHAR *cp;



	progname = drive;

	/* open the image */
	img = tsk_img_open(1, &progname ,imgtype, ssize);

	if (img == NULL) {
		tsk_error_print(stderr);
		exit(1);
	}
	if ((imgaddr * img->sector_size) >= img->size) {
		tsk_fprintf(stderr,
			"Sector offset supplied is larger than disk image (maximum: %"
			PRIu64 ")\n", img->size / img->sector_size);
		exit(1);
	}

	/* process the partition tables */
	vs = tsk_vs_open(img, imgaddr * img->sector_size, vstype);
	if (vs == NULL) {
		tsk_error_print(stderr);
		if (tsk_error_get_errno() == TSK_ERR_VS_UNSUPTYPE)
			tsk_vs_type_print(stderr);
		tsk_img_close(img);
		exit(1);
	}

	print_header(vs);

	if (tsk_vs_part_walk(vs, 0, vs->part_count - 1,
		(TSK_VS_PART_FLAG_ENUM) flags, part_act, NULL)) {
			tsk_error_print(stderr);
			tsk_vs_close(vs);
			tsk_img_close(img);
			exit(1);
	}

	if ((recurse) && (vs->vstype == TSK_VS_TYPE_DOS)) {
		int i;
		/* disable recursing incase we hit another DOS partition
		* future versions may support more layers */
		recurse = 0;

		for (i = 0; i < recurse_cnt; i++) {
			TSK_VS_INFO *vs2;
			vs2 = tsk_vs_open(img, recurse_list[i], TSK_VS_TYPE_DETECT);
			if (vs2 != NULL) {
				tsk_printf("\n\n");
				print_header(vs2);
				if (tsk_vs_part_walk(vs2, 0, vs2->part_count - 1,
					(TSK_VS_PART_FLAG_ENUM) flags, part_act, NULL)) {
						tsk_error_reset();
				}
				tsk_vs_close(vs2);
			}
			else {
				/* Ignore error in this case and reset */
				tsk_error_reset();
			}
		}
	}

	tsk_vs_close(vs);
	tsk_img_close(img);
	exit(0);
}

void drive_info(TCHAR * drive)
{
	printf(" into the Dirve_info = %S", drive);
	if (drive != NULL)
		info_offset(drive);

	exit(1);
}


void drive_info_live()
{
	printf("# The Physical Drives of this Machine : \n");
	system("wmic diskdrive get Caption, Name");
	printf("\n");

	printf("# The Logical Drives of this Machine : \n");
	system("wmic logicaldisk get Description, Name, FileSystem, SystemName");

	exit(1);
}