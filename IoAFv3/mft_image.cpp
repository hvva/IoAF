
#include <stdio.h>
#include <tsk\tsk_tools_i.h>
#include <tsk\libtsk.h>
#include <locale.h>
#include <time.h>
#include "define.h"


static TSK_TCHAR *progname;

/*
	작동 순서
	OpenImage -> FindFilesInImg -> ProcessFile() ==> Callback ( FilterFs / FilterVol)
*/
class TskGetTimes:public TskAuto {
public:
    TskGetTimes(int32_t);
	TskGetTimes(int32_t, bool);
    virtual TSK_RETVAL_ENUM processFile(TSK_FS_FILE * fs_file, const char *path);
    virtual TSK_FILTER_ENUM filterVol(const TSK_VS_PART_INFO * vs_part);
    virtual TSK_FILTER_ENUM filterFs(TSK_FS_INFO * fs_info);
    virtual uint8_t handleError();
    
private:
    int m_curVolAddr;
    int32_t m_secSkew;
	bool m_compute_hash;
};

TskGetTimes::TskGetTimes(int32_t a_secSkew)
{
    m_curVolAddr = -1;
    m_secSkew = a_secSkew;
	m_compute_hash = false;
}

TskGetTimes::TskGetTimes(int32_t a_secSkew, bool a_compute_hash)
{
    m_curVolAddr = -1;
    m_secSkew = a_secSkew;
	m_compute_hash = a_compute_hash;
}

// Print errors as they are encountered
uint8_t TskGetTimes::handleError() 
{
    fprintf(stderr, "%s", tsk_error_get());
    return 0;
}


TSK_RETVAL_ENUM TskGetTimes::processFile(TSK_FS_FILE * fs_file, const char *path)
{
    return TSK_OK;
}


TSK_FILTER_ENUM 
TskGetTimes::filterFs(TSK_FS_INFO * fs_info)
{
    TSK_TCHAR volName[32];
    if (m_curVolAddr > -1)
        TSNPRINTF(volName, 32, _TSK_T("vol%d/"),m_curVolAddr);
    else 
        volName[0] = '\0';

	TSK_FS_FLS_FLAG_ENUM fls_flags = (TSK_FS_FLS_FLAG_ENUM)(TSK_FS_FLS_MAC | TSK_FS_FLS_DIR | TSK_FS_FLS_FILE | TSK_FS_FLS_FULL);
	
	if(m_compute_hash){
		fls_flags = (TSK_FS_FLS_FLAG_ENUM)(fls_flags | TSK_FS_FLS_HASH);
	}

	// 실제 파싱 수행 Auto 클래스 .
	if (tsk_fs_fls(fs_info, (TSK_FS_FLS_FLAG_ENUM)(fls_flags),
		fs_info->root_inum, (TSK_FS_DIR_WALK_FLAG_ENUM)(TSK_FS_DIR_WALK_FLAG_ALLOC | TSK_FS_DIR_WALK_FLAG_UNALLOC | TSK_FS_DIR_WALK_FLAG_RECURSE), volName, m_secSkew)) {
	}


    return TSK_FILTER_SKIP;
}


TSK_FILTER_ENUM
TskGetTimes::filterVol(const TSK_VS_PART_INFO * vs_part)
{
    m_curVolAddr = vs_part->addr;
    return TSK_FILTER_CONT;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////

int mft_image(_TCHAR *image)
{
	int argc = 1;
	//_TCHAR *argv1[2] = {0,};
	//argv1[0] = L"test"; 
	//argv1[1] = imaTge;
	//printf("%s, %s\n\n", argv1[0], argv1[1]);

    TSK_IMG_TYPE_ENUM imgtype = TSK_IMG_TYPE_DETECT;
	// Use autodetection methods.
	TSK_TCHAR *argv[1] = {0, };
    unsigned int ssize = 0;
    //TSK_TCHAR *cp;
    int32_t sec_skew = 0;
	bool do_hash = false;

#ifdef TSK_WIN32
    // On Windows, get the wide arguments (mingw doesn't support wmain)
    //argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	argv[0] = image;

    if (argv == NULL) {
        fprintf(stderr, "Error getting wide arguments\n");
        exit(1);
    }
#else
    argv = (TSK_TCHAR **) argv1;
#endif

    progname = argv[0];
    setlocale(LC_ALL, "");
	// 경로명에 한글이 포함되면 안되는 문제를 해결함


    TskGetTimes tskGetTimes(sec_skew, do_hash);
    if (tskGetTimes.openImage(argc, &argv[0], imgtype, ssize)) {
        tsk_error_print(stderr);
        exit(1);
    }
	/*
	http://www.sleuthkit.org/sleuthkit/docs/api-docs/classTskAuto.html#a6b8742f6c15472e822b2226c4cfb2187

		uint8_t TskAuto::openImage	(	int 	a_numImg,
										const TSK_TCHAR *const 	a_images[],
										TSK_IMG_TYPE_ENUM 	a_imgType,
										unsigned int 	a_sSize 
		)	
			a_numImg	The number of images to open (will be > 1 for split images).
			a_images	The path to the image files (the number of files must be equal to num_img and they must be in a sorted order)
			a_imgType	The disk image type (can be autodetection)
			a_sSize	Size of device sector in bytes (or 0 for default)
     */

    
    if (tskGetTimes.findFilesInImg()) {
        // we already logged the errors
        exit(1);
    }
    
}
