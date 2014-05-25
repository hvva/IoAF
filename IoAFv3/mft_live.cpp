#include <NTFS.h>
#include <stdio.h>
#include <windows.h>
#include "time.h"
#include <iostream>
#include <Setupapi.h>
#include <Ntddstor.h>
#include <iostream>
#include <string>
#include <sqlite3.h>
#include "define.h"
//#include <string.h>

#pragma comment( lib, "setupapi.lib" )

using namespace std;

void FileTimeToUnixTime(LPFILETIME pft, unsigned __int64 * pt) {
    LONGLONG ll; // 64 bit value
    ll = (((LONGLONG)(pft->dwHighDateTime)) << 32) + pft->dwLowDateTime;
    *pt = (time_t)((ll - 116444736000000000L) / 10000000L);
}

struct mftstruct{
   ULONGLONG entry;
   ULONGLONG ParentRef;
   char FILENAME[MAX_PATH]; 
   char FULLPATH[MAX_PATH];
   unsigned __int64 SI_writeTm, SI_createTm, SI_accessTm, SI_mftTm;
   unsigned __int64 FN_writeTm, FN_createTm, FN_accessTm, FN_mftTm;
};
typedef struct mftstruct MFTSTRUCT;

unsigned int entry_count;

int totalfiles = 0;
int totaldirs = 0;

char fullPath_[MAX_PATH];
char root[10] = "$ROOT";
char BackSlash[MAX_PATH] = "\\";


char *getFullPath(int entry, MFTSTRUCT *mftStruct, int saved_entry)
{
   //static char buff[MAX_PATH] = "";

   if ( entry == 5 )
   {   
      return root;
   }

   if ( entry != 5 )
   {
      //sprintf(mftStruct[saved_entry].FULLPATH, "\\%s", getFullPath(mftStruct[entry].ParentRef, mftStruct, saved_entry));

      strcat(mftStruct[saved_entry].FULLPATH,  getFullPath(mftStruct[entry].ParentRef, mftStruct, saved_entry));
      char BackSlash[MAX_PATH] = "\/";
      strcat(BackSlash, mftStruct[entry].FILENAME);
      //return mftStruct[entry].FILENAME;
      return BackSlash;
   }
}

void printStruct(MFTSTRUCT *mftStruct)
{
   // for(int i=303 ;i<404 ; i++)
   for(int i=0 ;i<entry_count ; i++)
   {
      //printf(" [i] entry values\n ");
      printf(" FILENAME = %s\n", mftStruct[i].FILENAME);
      //printf(" W_TIME = %u\n", mftStruct[i].FN_accessTm);
      //printf(" A_TIME = %u\n", mftStruct[i].FN_createTm);
      //printf(" C_TIME = %u\n", mftStruct[i].FN_mftTm);
      //printf(" Entry Num = %u\n", mftStruct[i].entry);
      //printf(" ParentReg Num = %u\n\n", mftStruct[i].ParentRef);
   }
}

void mft_live_usage()
{
   printf("\n# Invalid parameter\n");
   printf("# Usage: mftparset.exe c:\n");
}

// get volume name 'C', 'D', ...
// *ppath -> "c:\program files\common files"
char getvolume(char **ppath)
{
   char *p = *ppath;
   char volname;

   // skip leading blank and "
   while (*p)
   {
      if (*p == ' ' || *p == '"')
         p++;
      else
         break;
   }
   if (*p == '\0')
      return '\0';
   else
   {
      volname = *p;
      p++;
   }

   // skip blank
   while(*p)
   {
      if (*p == ' ')
         p++;
      else
         break;
   }
   if (*p == '\0')
      return '\0';

   if (*p != ':')
      return '\0';

   // forward to '\' or string end
   while (*p)
   {
      if (*p != '\\')
         p++;
      else
         break;
   }
   // forward to not '\' and not ", or string end
   while (*p)
   {
      if (*p == '\\' || *p == '"')
         p++;
      else
         break;
   }

   *ppath = p;
   return volname;
}

int MFTtest_live(struct mftstruct  *u3, char *casename)
{
    sqlite3 *db = NULL; 
    sqlite3_stmt *stmt = NULL; //sqlite3 statement 
	char path[MAX_PATH] = "info.db";
    char *sql; 
    int rc;
    unsigned int i;
    char *buffer = (char *)malloc(500);
 
    memset(buffer, 0x00, sizeof(char)*500);

	//sprintf(path, ".\/%s\/info.db", casename);

    //int error = sqlite3_open("./case/info.db", &db);
   // fprintf(stdout, "DB연결 완료.\n");


	if(sqlite3_open("info.db", &db) != SQLITE_OK)
    //if(sqlite3_open("./case/info.db", &db) != SQLITE_OK)
    {
        fprintf(stderr, "DB접근이 어렵습니다. (오류 %s)\n", sqlite3_errmsg(db));
    }

    //MFT 테이블 생성
    sql = "CREATE TABLE IF NOT EXISTS MFT (FILENAME TEXT, FULLPATH TEXT, entry INT, ParentRef INT, Sl_writeTm INT, SI_createTm INT, SI_accessTm INT, SI_mftTm INT, FN_writeTm INT, FN_createTm INT, FN_accessTm INT, FN_mftTm INT, TYPE TEXT);";
    if( sqlite3_exec(db, sql, NULL, NULL, NULL) == SQLITE_OK) {
       // fprintf(stderr, ">> SQLite Table creation Succeeded!\n");
    } else {
        puts("테이블 생성에 실패했습니다.");
        exit(1);
    }
    char* errorMsg = NULL;
    rc = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, &errorMsg);
    //fprintf(stderr, " Commit begin result : %s\n", errorMsg);
    sprintf (buffer,"INSERT INTO MFT(FILENAME, FULLPATH, entry, ParentRef, Sl_writeTm, SI_createTm, SI_accessTm, SI_mftTm, FN_writeTm, FN_createTm, FN_accessTm, FN_mftTm, TYPE) VALUES ( ?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?10, ?11, ?12, \"FILE\")");
    //sprintf (buffer,"INSERT INTO MFT(FILENAME, entry, ParentRef, Sl_writeTm, SI_createTm, SI_accessTm, SI_mftTm, FN_writeTm, FN_createTm, FN_accessTm, FN_mftTm) VALUES ( ?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?10, ?11)", , , , u3[i]., u3[i]., u3[i]., u3[i]., u3[i]., u3[i]., u3[i]., u3[i].);

    if(sqlite3_prepare_v2(db, buffer, strlen(buffer), &stmt, NULL) == SQLITE_OK)
    {
        //puts(">> Prepared Statement is ready : Succeeded!\n");
    }
    else
    {
        puts("테이블 값 입력에 실패하였습니다.");
    }

    for( i=0; i<entry_count; i++){

        //sprintf (buffer,"INSERT INTO MFT(FILENAME, entry, ParentRef, Sl_writeTm, SI_createTm, SI_accessTm, SI_mftTm, FN_writeTm, FN_createTm, FN_accessTm, FN_mftTm) VALUES ( \"%s\", \"%d\", \"%d\", \"%d\", \"%d\", \"%d\", \"%d\", \"%d\", \"%d\", \"%d\", \"%d\")", u3[i].FILENAME, (int)u3[i].entry, (int)u3[i].ParentRef, u3[i].SI_writeTm, u3[i].SI_createTm, u3[i].SI_accessTm, u3[i].SI_mftTm, u3[i].FN_writeTm, u3[i].FN_createTm, u3[i].FN_accessTm, u3[i].FN_mftTm);

      
      sqlite3_bind_text(stmt, 1, u3[i].FILENAME, strlen(u3[i].FILENAME), SQLITE_STATIC);
      sqlite3_bind_text(stmt, 2, u3[i].FULLPATH, strlen(u3[i].FULLPATH), SQLITE_STATIC);
      sqlite3_bind_int(stmt, 3, (int)(u3[i].entry));
	  sqlite3_bind_int(stmt, 4, (int)(u3[i].ParentRef));
	  sqlite3_bind_int(stmt, 5, (int)(u3[i].SI_writeTm));
      sqlite3_bind_int(stmt, 6, (int)(u3[i].SI_createTm));
	  sqlite3_bind_int(stmt, 7, (int)(u3[i].SI_accessTm));
      sqlite3_bind_int(stmt, 8, (int)(u3[i].SI_mftTm));
      sqlite3_bind_int(stmt, 9, (int)(u3[i].FN_writeTm));
      sqlite3_bind_int(stmt, 10, (int)(u3[i].FN_createTm));
	  sqlite3_bind_int(stmt, 11, (int)(u3[i].FN_accessTm));
      sqlite3_bind_int(stmt, 12, (int)(u3[i].FN_mftTm));


        if ( sqlite3_step(stmt) != SQLITE_DONE )  {
            fprintf(stderr, ">> SQLite Insert failed! \n");
            fprintf(stderr, ">> Values : %s, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u\n", u3[i].FILENAME, u3[i].entry, u3[i].ParentRef, u3[i].SI_writeTm, u3[i].SI_createTm, u3[i].SI_accessTm, u3[i].SI_mftTm, u3[i].FN_writeTm, u3[i].FN_createTm, u3[i].FN_accessTm, u3[i].FN_mftTm);
            //exit(1);
        }

        //sqlite3_finalize(stmt);
        sqlite3_reset(stmt);
    }
    rc = sqlite3_exec(db, "COMMIT TRANSACTION;", NULL, NULL, &errorMsg);
  //  fprintf(stderr, " Commit result : %s\n", errorMsg);
    sqlite3_finalize(stmt);

    sqlite3_close(db);
    
    return 0;
 }

int mft_live(char *path, char *casename)
{
   clock_t start, end; 

   char volname;
  
   volname = getvolume(&path); // Double Pointer !

   if (!volname)
   {
      mft_live_usage();
      return -1;
   }
   
   
   CNTFSVolume volume(volname);
   if (!volume.IsVolumeOK())
   {
      printf("Cannot get NTFS BPB from boot sector of volume %c\n", volname);
      return -1;
   }


   entry_count = volume.GetRecordsCount();   // ULONGLONG GetRecordsCount() const
   printf("MFT Records Count = %d\n\n", entry_count);

   MFTSTRUCT *u3; 
   //u3 = (MFTSTRUCT *)malloc(sizeof(MFTSTRUCT) * entry_count);
   u3 = (MFTSTRUCT *)calloc(entry_count, sizeof(MFTSTRUCT));
   if(u3 ==NULL){
      puts("Malloc Failed...");
      exit(1);
   }


   ULONGLONG  mft_addr;
   mft_addr = volume.GetMFTAddr();

   
   CFileRecord fr(&volume);


   fr.SetAttrMask(MASK_STANDARD_INFORMATION | MASK_FILE_NAME);

   //------------------------------------------------------------------------
   if (!fr.ParseFileRecord(MFT_IDX_ROOT)) 
   {
      printf("Cannot read root directory of volume %c\n", volname);
      return -1;
   }

   if (!fr.ParseAttrs())
   {
      printf("Cannot parse attributes\n");
      return -1;
   }


   CIndexEntry ie;
   
   // 파일의 MAC타임 정보 저장.
   FILETIME SI_writeTm;
   FILETIME SI_createTm;
   FILETIME SI_accessTm;
   FILETIME SI_mftTm;

   FILETIME FN_writeTm;
   FILETIME FN_createTm;
   FILETIME FN_accessTm;
   FILETIME FN_mftTm;


   //char fn[MAX_PATH];
   _TCHAR fn[MAX_PATH];
   
   start = clock(); 

   //for(int i=303 ;i<404 ; i++)
   for(int i=0 ;i<entry_count ; i++)
   {
      fr.ParseFileRecord(i);

      if (!fr.ParseAttrs())  
      {
         //printf("Entry NUM %d Cannot parse attributes\n", i);
         continue;
      }

      int fnlen = fr.GetFileName(fn, MAX_PATH);
      if (fnlen > 0)
      {
         fr.GetFileTime(&SI_writeTm, &SI_createTm, &SI_accessTm, &SI_mftTm, &FN_writeTm, &FN_createTm, &FN_accessTm, &FN_mftTm);

         unsigned __int64 SI_writeTm_s;
         unsigned __int64 SI_createTm_s;
         unsigned __int64 SI_accessTm_s;
         unsigned __int64 SI_mftTm_s;

         unsigned __int64 FN_writeTm_s;
         unsigned __int64 FN_createTm_s;
         unsigned __int64 FN_accessTm_s;
         unsigned __int64 FN_mftTm_s;

         FileTimeToUnixTime(&SI_writeTm, &SI_writeTm_s);
         FileTimeToUnixTime(&SI_createTm, &SI_createTm_s);
         FileTimeToUnixTime(&SI_accessTm, &SI_accessTm_s);
         FileTimeToUnixTime(&SI_mftTm, &SI_mftTm_s);

         FileTimeToUnixTime(&FN_writeTm, &FN_writeTm_s);
         FileTimeToUnixTime(&FN_createTm, &FN_createTm_s);
         FileTimeToUnixTime(&FN_accessTm, &FN_accessTm_s);
         FileTimeToUnixTime(&FN_mftTm, &FN_mftTm_s);

         if (fr.IsDirectory())
            totaldirs ++;
         else
            totalfiles ++;

	// K
		 int len; 
		 len = WideCharToMultiByte(CP_ACP, 0, fn, -1, NULL, 0, NULL,NULL);
		 char* ctmp = new char[len];

		 WideCharToMultiByte(CP_ACP, 0, fn, -1, ctmp, len, NULL, NULL);


         strcpy(u3[i].FILENAME, ctmp);

         u3[i].SI_writeTm = SI_writeTm_s;
         u3[i].SI_createTm = SI_createTm_s;
         u3[i].SI_accessTm = SI_accessTm_s;
         u3[i].SI_mftTm = SI_mftTm_s;
         
         u3[i].FN_writeTm = FN_writeTm_s;
         u3[i].FN_createTm = FN_createTm_s;
         u3[i].FN_accessTm = FN_accessTm_s;
         u3[i].FN_mftTm = FN_mftTm_s;

         u3[i].entry = i;
         u3[i].ParentRef = fr.GetParentRef();

         
      
         if (0)
         {
            printf("************************************************************\n\n");
            printf("Current MFT Entry NUM : %u\n", i);
            printf("MFT Parent Reference : %u\n", fr.GetParentRef());

            printf("SI_WRITE TIME : %d\n", SI_writeTm_s);
            printf("SI_CREATE TIME : %d\n", SI_createTm_s);
            printf("SI_ACCESS TIME : %d\n", SI_accessTm_s);
            printf("SI_MFT TIME : %d\n", SI_mftTm_s);

            printf("FN_WRITE TIME : %d\n", FN_writeTm_s);
            printf("FN_CREATE TIME : %d\n", FN_createTm_s);
            printf("FN_ACCESS TIME : %d\n", FN_accessTm_s);
            printf("FN_MFT TIME : %d\n", FN_mftTm_s);

            printf("File TYPE : \t%s", fr.IsDirectory()?"<DIR>\n":"<FILE>\n");

            if (!fr.IsDirectory())
               printf("filesize = %I64u\n", fr.GetFileSize());
            else
               printf("\t");

            printf("<%c%c%c%c>\t%s\n", fr.IsReadOnly()?'R':' ',
               fr.IsHidden()?'H':' ', fr.IsSystem()?'S':' ',fr.IsDeleted()?'D':' ',fn);

           //printf("filename = %s\n", fn);
         }
      }
      
   }


   //printStruct(u3);

   //for(int k=303 ;k<404 ; k++)
   for (int k = 0 ; k < entry_count ; k++)
   {
      getFullPath(k, u3, k);
	  strcat(u3[k].FULLPATH, "\/");
	  strcat(u3[k].FULLPATH, u3[k].FILENAME);
      //printf( "entry %d = %s\n\n", k, u3[k].FULLPATH ); 
   }


   end = clock();
   //printf("\n##### 전체 소요시간 : %5.2f초 #####\n", (float)(end-start)/CLOCKS_PER_SEC);
   //printf("Files: %d, Directories: %d\n", totalfiles, totaldirs);

   MFTtest_live(u3, casename);

   free(u3);
   return 0;
}

