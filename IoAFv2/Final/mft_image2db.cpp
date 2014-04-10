#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <sqlite3.h>
#include "define.h"

char seps[] = "|\n";
char *token;

struct MFT {
	char PATH[260];
	unsigned int FN_atime, FN_mtime, FN_ctime, FN_crtime;
	unsigned int SI_atime, SI_mtime, SI_ctime, SI_crtime;
};
typedef struct MFT test;


int MFTtest(struct MFT  *u3, int countresult, char* case_name)
{
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;
    char *sql;
    int rc;
	char path[_MAX_PATH] = "info.db";
    int j;
    unsigned int i;

    char *buffer = (char *)malloc(500);
    memset(buffer, 0x00, sizeof(char)*500);

	//sprintf(path, ".\/%s\/info.db", case_name);

    //int error = sqlite3_open("./case/info.db", &db);
    //fprintf(stdout, "DB연결 완료.\n");
	// case를 -n인자로 받은 폴더 명 수정 필요.

	if(sqlite3_open("info.db", &db) != SQLITE_OK)
    //if(sqlite3_open("./case/info.db", &db) != SQLITE_OK)
    {
        fprintf(stderr, "DB접근이 어렵습니다. (오류 %s)\n", sqlite3_errmsg(db));
    }

    //MFT 테이블 생성
    //sql = "CREATE TABLE IF NOT EXISTS MFT (FULLPATH TEXT ,AccessTm INT, ModifiedTm INT, ChangeTm INT, CreatedTm INT);";
    sql = "CREATE TABLE IF NOT EXISTS MFT (FULLPATH TEXT , FN_AccessTm INT, FN_ModifiedTm INT, FN_ChangeTm INT, FN_CreatedTm INT, SI_AccessTm INT, SI_ModifiedTm INT, SI_ChangeTm INT, SI_CreatedTm INT, TYPE TEXT);";
	if( sqlite3_exec(db, sql, NULL, NULL, NULL) == SQLITE_OK) {
        //fprintf(stderr, ">> SQLite Table creation Succeeded!\n");
    } else {
        puts("테이블 생성에 실패했습니다.");
        exit(1);
    }

    //데이터 추가 코드.
    char* errorMsg = NULL;
    rc = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, &errorMsg);
    fprintf(stderr, " Commit begin result : %s\n", errorMsg);
	sprintf (buffer,"INSERT INTO MFT(FULLPATH  , FN_AccessTm, FN_ModifiedTm, FN_ChangeTm, FN_CreatedTm, SI_AccessTm, SI_ModifiedTm, SI_ChangeTm, SI_CreatedTm, TYPE) VALUES ( ?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, \"FILE\")");

    if(sqlite3_prepare_v2(db, buffer, strlen(buffer), &stmt, NULL) == SQLITE_OK)
    {
        puts(">> Prepared Statement is ready : Succeeded!\n");
    }
    else
    {
        puts("테이블 값 입력에 실패하였습니다.");
    }

    for( i=0; i<countresult; i++){
		sqlite3_bind_text(stmt, 1, u3[i].PATH, strlen(u3[i].PATH), SQLITE_STATIC);
		sqlite3_bind_int(stmt, 2, (int)(u3[i].FN_atime));
        sqlite3_bind_int(stmt, 3, (int)(u3[i].FN_mtime));
        sqlite3_bind_int(stmt, 4, (int)(u3[i].FN_ctime));
		sqlite3_bind_int(stmt, 5, (int)(u3[i].FN_crtime));
		sqlite3_bind_int(stmt, 6, (int)(u3[i].SI_atime));
        sqlite3_bind_int(stmt, 7, (int)(u3[i].SI_mtime));
        sqlite3_bind_int(stmt, 8, (int)(u3[i].SI_ctime));
		sqlite3_bind_int(stmt, 9, (int)(u3[i].SI_crtime));

        if ( sqlite3_step(stmt) != SQLITE_DONE )  {
            fprintf(stderr, ">> SQLite Insert failed! \n");
        }
        sqlite3_reset(stmt);
    }
    rc = sqlite3_exec(db, "COMMIT TRANSACTION;", NULL, NULL, &errorMsg);
    //fprintf(stderr, " Commit result : %s\n", errorMsg);
    sqlite3_finalize(stmt);

    sqlite3_close(db);
   
    return 0;
 }
 
int mft_image2db(char* case_path, char* case_name)
{
	FILE *f;
	char buf[256];
	int i,j, count;
	test *u3;

	//u3 = (test *)malloc(sizeof(test) * 1000000);
	u3 = (test *)calloc(1000000, sizeof(test));

	if(u3==NULL){
		puts("Malloc Failed...");
		exit(1);
	}

	// case는 -n인자로 받은 폴더명, 수정필요.
	//f = fopen("./case/image.mft","rt");
	f = fopen(case_path, "rt");
	//int FindDeletedFile = 0;

	if(f!=NULL){

		for(i=0;f!=NULL;i++){

			FindDeleted_enty:   
			//FindDeletedFile = 0;

			if(fgets(buf,256,f)==NULL){
				break;
			}
			token = strtok( buf, seps );

			for(j=0; token != NULL;j++)
			{
				if(j==1){
					// strcpy(u3[i].PATH,token);

				}else if(j==7){
					if( atoi(token) == 0 )
						goto FindDeleted_enty;

					u3[i].FN_atime = atoi(token);
				}else if(j==8){
					u3[i].FN_mtime = atoi(token);
				}else if(j==9){
					u3[i].FN_ctime = atoi(token);
				}else if(j==10){
					u3[i].FN_crtime = atoi(token);
				}
				token = strtok( NULL, seps );
			}

			//if(FindDeletedFile == 1)


			if(fgets(buf,256,f)==NULL){
				break;
			}
			token = strtok( buf, seps );

			for(j=0; token != NULL;j++)
			{
				if(j==1){
					 strcpy(u3[i].PATH,token);
				}else if(j==7){
					u3[i].SI_atime = atoi(token);
				}else if(j==8){
					u3[i].SI_mtime = atoi(token);
				}else if(j==9){
					u3[i].SI_ctime = atoi(token);
				}else if(j==10){
					u3[i].SI_crtime = atoi(token);
				}
				token = strtok( NULL, seps );
			}

			if(f!=NULL){
				count = i;
			}

		}
		fclose(f);
	}

	MFTtest(u3,count,case_name);
	free(u3);

	return 0;
}
