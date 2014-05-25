#include"define.h"
#include "sql.h"
#include "exreg.h"

#pragma once
void mft_live_module(TCHAR *volName, TCHAR * CaseName);
void mft_image_module(TCHAR * ImagePath, TCHAR * CaseName);

void reg_live_module(TSK_FS_INFO *fs);
void reg_image_module(TSK_FS_INFO *fs);

void ie_hist_module(TSK_FS_INFO *fs);
int open_fs_live(TCHAR *volName, TSK_IMG_INFO **img,TSK_FS_INFO **fs );
int open_fs_image(TCHAR *ImagePath, int offset, TSK_IMG_INFO **img,TSK_FS_INFO **fs );
void img_fs_close(TSK_IMG_INFO **img,TSK_FS_INFO **fs );
void deepscan(TCHAR * CaseName);
void scan(TCHAR * CaseName);

void js_create_module(int Timeline_num);
void usb_info();