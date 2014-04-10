#include <tchar.h>
#pragma once

int mft_image(_TCHAR *image);
int mft_live(char *path, char *casename);
int mft_image2db(char* case_path, char* case_name);
int iehist(char* dat_name, int mode);

int js_atime_create(int limit_count);
int js_ctime_create(int limit_count);
int js_mtime_create(int limit_count);

int js_detection_info(void);
int js_toolinfo(void);
