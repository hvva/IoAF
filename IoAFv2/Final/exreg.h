#include<stdio.h>
#include<tsk\tsk_tools_i.h>
#include<tsk\libtsk.h>
#include "define.h"

#pragma once

int carving_hive(TCHAR * path, TSK_FS_INFO *fs, TSK_INUM_T inode);

uint8_t hive_extract(TSK_FS_INFO * fs, TSK_INUM_T inum,
    TSK_FS_ATTR_TYPE_ENUM type, uint8_t type_used,
    uint16_t id, uint8_t id_used, TSK_FS_FILE_WALK_FLAG_ENUM flags);

static TSK_WALK_RET_ENUM
	hive_action(TSK_FS_FILE * fs_file, TSK_OFF_T a_off, TSK_DADDR_T addr,
    char *buf, size_t size, TSK_FS_BLOCK_FLAG_ENUM flags, void *ptr);

void ntuser_hive(TSK_FS_INFO *fs);

TSK_WALK_RET_ENUM callback(TSK_FS_FILE * fs_file, const char* path, void * ptr);

void ie_file(TSK_FS_INFO *fs);
TSK_WALK_RET_ENUM callback2(TSK_FS_FILE * fs_file, const char* path, void * ptr);