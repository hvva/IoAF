#include<stdio.h>
#include<stdlib.h>
#include<sqlite3.h>
#include<malloc.h>
#include<string.h>
#include<tchar.h>
#include "match.h"


void get_signature_REG(SIGREGQ * q)
{

	sqlite3 *sigDB = NULL;


	sqlite3_stmt *sigSTMT = NULL;
	char *sql;
	//int rc; 
	char* errorMsg = NULL;
	if(sqlite3_open("sig.db", &sigDB) != SQLITE_OK)
	{
		fprintf(stderr, "DB접근이 어렵습니다. (오류 %s)\n", sqlite3_errmsg(sigDB));
	}

	sql = "select * from Registry;";
	if( sqlite3_exec(sigDB, sql, NULL, NULL, NULL) == SQLITE_OK) { 

	} else {

		exit(1);
	}


	if(sqlite3_prepare_v2(sigDB, sql, strlen(sql), &sigSTMT, NULL) == SQLITE_OK) 
	{
		//puts(">> Prepared Statement is ready : Succeeded!\n");
	}
	else
	{
		puts("테이블 값 입력에 실패하였습니다.");
	}

	int row = 0;
	while (1) {
		int s;


		s = sqlite3_step (sigSTMT);
		if (s == SQLITE_ROW) {
			sig_registry * tmp = (sig_registry * ) malloc(sizeof(sig_registry));
			memset(tmp, 0, sizeof(sig_registry));
			int bytes;
			const unsigned char * text;
			////////////////////////////////////////////////////////////////////
			tmp->action = sqlite3_column_int(sigSTMT, 0);
			tmp->tool = sqlite3_column_int (sigSTMT, 1);
			sprintf(tmp->key, "%s", (char * )sqlite3_column_text(sigSTMT, 2));
			////////////////////////////////////////////////////////////////////
			q->push(tmp);
			row++;
		}
		else if (s == SQLITE_DONE) {
			break;
		}
		else {
			fprintf (stderr, "get registry signature Failed.\n");
			exit (1);
		}
	}
	sqlite3_finalize(sigSTMT);
	sqlite3_close(sigDB);
}
void get_signature_FILE(SIGFILEQ * q)
{

	sqlite3 *sigDB = NULL;


	sqlite3_stmt *sigSTMT = NULL;
	char *sql;
	int rc; 
	char* errorMsg = NULL;

	if(sqlite3_open("sig.db", &sigDB) != SQLITE_OK)
	{
		fprintf(stderr, "DB접근이 어렵습니다. (오류 %s)\n", sqlite3_errmsg(sigDB));
	}

	sql = "select * from file;";
	if( sqlite3_exec(sigDB, sql, NULL, NULL, NULL) == SQLITE_OK) { 

	} else {

		exit(1);
	}


	if(sqlite3_prepare_v2(sigDB, sql, strlen(sql), &sigSTMT, NULL) == SQLITE_OK) 
	{
		//puts(">> Prepared Statement is ready : Succeeded!\n");
	}
	else
	{
		puts("테이블 값 입력에 실패하였습니다.");
	}

	int row = 0;
	while (1) {
		int s;


		s = sqlite3_step (sigSTMT);
		if (s == SQLITE_ROW) {
			sig_file * tmp = (sig_file * ) malloc(sizeof(sig_file));
			memset(tmp, 0, sizeof(sig_file));
			int bytes;
			const unsigned char * text;
			////////////////////////////////////////////////////////////////////
			tmp->action = sqlite3_column_int(sigSTMT, 1);
			tmp->tool = sqlite3_column_int (sigSTMT, 0);
			sprintf(tmp->file, "%s", (char * )sqlite3_column_text(sigSTMT, 2));
			sprintf(tmp->path, "%s", (char * )sqlite3_column_text(sigSTMT, 3));
			////////////////////////////////////////////////////////////////////
			q->push(tmp);
			row++;
		}
		else if (s == SQLITE_DONE) {
			break;
		}
		else {
			fprintf (stderr, "get signatrue file Failed.\n");
			exit (1);
		}
	}
	sqlite3_finalize(sigSTMT);
	sqlite3_close(sigDB);
}
void matching_REG(SIGREGQ *q, MATCHREGQ *mq)
{
	sqlite3 *infoDB = NULL;
	sqlite3_stmt *infoSTMT = NULL;
	char *sql;
	int rc; 
	char* errorMsg = NULL;

	if(sqlite3_open("info.db", &infoDB) != SQLITE_OK)
	{
		fprintf(stderr, "DB접근이 어렵습니다. (오류 %s)\n", sqlite3_errmsg(infoDB));
	}
	if( sqlite3_exec(infoDB, "create table IF NOT EXISTS Registry(action NUMERIC, tool NUMERIC, key TEXT, mtime NUMERIC);", NULL, NULL, NULL) == SQLITE_OK) { 
	} else {
		exit(1);
	}
	while(!q->empty()){
		sig_registry * stmp;
		stmp=q->front();
		//printf("%s\n", q->front()->key);
		char sql[0x1000];
		memset(sql, 0, 0x1000);
		sprintf(sql, "select * from Registry where PATH like \"%%%s\";", q->front()->key);
		if(sqlite3_prepare_v2(infoDB, sql, strlen(sql), &infoSTMT, NULL) == SQLITE_OK) 
		{
		}
		while (1) {
			int row = 0;
			int s;
			s = sqlite3_step (infoSTMT);
			if (s == SQLITE_ROW) {
				match_registry * mtmp = (match_registry * )malloc(sizeof(match_registry));
				memset(mtmp, 0, sizeof(match_registry));
				////////////////////////////////////////////////////////////////////
				mtmp->action = stmp->action;
				sprintf(mtmp->key, "%s", (char * )sqlite3_column_text(infoSTMT, 0));
				mtmp->time =  sqlite3_column_int64 (infoSTMT, 1);
				mtmp->tool = stmp->tool;
				////////////////////////////////////////////////////////////////////
				printf("%d : %s : %d : %d\n", stmp->action, mtmp->key, mtmp->time, mtmp->tool);
				mq->push(mtmp);
				row++;

			}
			else if (s == SQLITE_DONE) {
				break;
			}
			else {
				fprintf (stderr, "Matching reg Failed.\n");
				exit (1);
			}

		}
		sqlite3_finalize(infoSTMT);
		q->pop();
		free(stmp);
	}
	sqlite3_close(infoDB);
}
void Deep_matching_REG(SIGREGQ *q, MATCHREGQ *mq)
{
	sqlite3 *infoDB = NULL;
	sqlite3_stmt *infoSTMT = NULL;
	char *sql;
	int rc; 
	char* errorMsg = NULL;

	if(sqlite3_open("info.db", &infoDB) != SQLITE_OK)
	{
		fprintf(stderr, "DB접근이 어렵습니다. (오류 %s)\n", sqlite3_errmsg(infoDB));
	}

	while(!q->empty()){
		sig_registry * stmp;
		stmp=q->front();
		//printf("%s\n", q->front()->key);
		char sql[0x1000];
		memset(sql, 0, 0x1000);
		sprintf(sql, "select * from Registry where PATH like \"%%%s%%\";", q->front()->key);//modify
		if(sqlite3_prepare_v2(infoDB, sql, strlen(sql), &infoSTMT, NULL) == SQLITE_OK) 
		{
		}
		while (1) {
			int row = 0;
			int s;
			s = sqlite3_step (infoSTMT);
			if (s == SQLITE_ROW) {
				match_registry * mtmp = (match_registry * )malloc(sizeof(match_registry));
				memset(mtmp, 0, sizeof(match_registry));
				////////////////////////////////////////////////////////////////////
				mtmp->action = stmp->action;
				sprintf(mtmp->key, "%s", (char * )sqlite3_column_text(infoSTMT, 0));
				mtmp->time =  sqlite3_column_int64 (infoSTMT, 1);
				mtmp->tool = stmp->tool;
				////////////////////////////////////////////////////////////////////
				printf("%d : %s : %d : %d\n", stmp->action, mtmp->key, mtmp->time, mtmp->tool);
				mq->push(mtmp);
				row++;

			}
			else if (s == SQLITE_DONE) {
				break;
			}
			else {
				fprintf (stderr, "Matching reg Failed.\n");
				exit (1);
			}

		}
		sqlite3_finalize(infoSTMT);
		q->pop();
		free(stmp);
	}

	sqlite3_close(infoDB);
}
void matching_FILE(SIGFILEQ *q, MATCHFILEQ *mq)
{
	sqlite3 *infoDB = NULL;
	sqlite3_stmt *infoSTMT = NULL;
	char *sql;
	int rc; 
	char* errorMsg = NULL;

	if(sqlite3_open("info.db", &infoDB) != SQLITE_OK)
	{
		fprintf(stderr, "DB접근이 어렵습니다. (오류 %s)\n", sqlite3_errmsg(infoDB));
	}

	while(!q->empty()){
		sig_file * stmp;
		stmp=q->front();
		printf("%s\n", q->front()->path);
		char sql[0x1000];
		memset(sql, 0, 0x1000);
		sprintf(sql, "select * from mft where FULLPATH like \"%%%s\";", q->front()->path, q->front()->file);
		//printf("%s\n", sql);
		if(sqlite3_prepare_v2(infoDB, sql, strlen(sql), &infoSTMT, NULL) == SQLITE_OK) 
		{
		}
		while (1) {
			int row = 0;
			int s;
			s = sqlite3_step (infoSTMT);
			if (s == SQLITE_ROW) {
				match_file * mtmp = (match_file * )malloc(sizeof(match_file));
				memset(mtmp, 0, sizeof(match_file));
				////////////////////////////////////////////////////////////////////
				mtmp->action = stmp->action;
				sprintf(mtmp->path, "%s", (char * )sqlite3_column_text(infoSTMT, 0));
				mtmp->SI_Atime =  sqlite3_column_int64 (infoSTMT, 1);
				mtmp->SI_Mtime =  sqlite3_column_int64 (infoSTMT, 2);
				mtmp->SI_Ctime =  sqlite3_column_int64 (infoSTMT, 3);
				mtmp->FN_Atime =  sqlite3_column_int64 (infoSTMT, 4);
				mtmp->FN_Mtime =  sqlite3_column_int64 (infoSTMT, 5);
				mtmp->FN_Ctime =  sqlite3_column_int64 (infoSTMT, 6);
				mtmp->tool = stmp->tool;
				////////////////////////////////////////////////////////////////////
				//printf("%d : %s : %d : %d\n", stmp->action, mtmp->key, mtmp->time, mtmp->tool);
				mq->push(mtmp);
				row++;

			}
			else if (s == SQLITE_DONE) {
				break;
			}
			else {
				fprintf (stderr, "Matching file Failed.\n");
				exit (1);
			}

		}
		sqlite3_finalize(infoSTMT);
		q->pop();
		free(stmp);
	}

	sqlite3_close(infoDB);
}
void Deep_matching_FILE(SIGFILEQ *q, MATCHFILEQ *mq)
{
	sqlite3 *infoDB = NULL;
	sqlite3_stmt *infoSTMT = NULL;
	char *sql;
	int rc; 
	char* errorMsg = NULL;

	if(sqlite3_open("info.db", &infoDB) != SQLITE_OK)
	{
		fprintf(stderr, "DB접근이 어렵습니다. (오류 %s)\n", sqlite3_errmsg(infoDB));
	}

	while(!q->empty()){
		sig_file * stmp;
		stmp=q->front();
		//printf("%s\n", q->front()->path);
		char sql[0x1000];
		memset(sql, 0, 0x1000);
		sprintf(sql, "select * from mft where FULLPATH like \"%%%s%%\";", q->front()->file);
		//printf("%s\n", sql);
		if(sqlite3_prepare_v2(infoDB, sql, strlen(sql), &infoSTMT, NULL) == SQLITE_OK) 
			while (1) {
				int row = 0;
				int s;
				s = sqlite3_step (infoSTMT);
				if (s == SQLITE_ROW) {
					match_file * mtmp = (match_file * )malloc(sizeof(match_file));
					memset(mtmp, 0, sizeof(match_file));
					////////////////////////////////////////////////////////////////////
					mtmp->action = stmp->action;
					sprintf(mtmp->path, "%s", (char * )sqlite3_column_text(infoSTMT, 0));
					mtmp->SI_Atime =  sqlite3_column_int64 (infoSTMT, 1);
					mtmp->SI_Mtime =  sqlite3_column_int64 (infoSTMT, 2);
					mtmp->SI_Ctime =  sqlite3_column_int64 (infoSTMT, 3);
					mtmp->FN_Atime =  sqlite3_column_int64 (infoSTMT, 4);
					mtmp->FN_Mtime =  sqlite3_column_int64 (infoSTMT, 5);
					mtmp->FN_Ctime =  sqlite3_column_int64 (infoSTMT, 6);
					mtmp->tool = stmp->tool;
					////////////////////////////////////////////////////////////////////
					//printf("%d : %s : %d : %d\n", stmp->action, mtmp->key, mtmp->time, mtmp->tool);
					mq->push(mtmp);
					row++;

				}
				else if (s == SQLITE_DONE) {
					break;
				}
				else {
					fprintf (stderr, "Failed.\n");
					exit (1);
				}

			}
			sqlite3_finalize(infoSTMT);
			q->pop();
			free(stmp);
	}

	sqlite3_close(infoDB);
}
void REG2SQL(MATCHREGQ * mq)
{
	sqlite3 *db = NULL; 
	sqlite3_stmt *stmt = NULL;
	char *sql;
	int rc; 
	unsigned int i;
	char *buffer = (char *)malloc(500); 
	char* errorMsg = NULL;

	memset(buffer, 0x00, sizeof(char)*500); 


	if(sqlite3_open("info.db", &db) != SQLITE_OK)
	{
		fprintf(stderr, "DB접근이 어렵습니다. (오류 %s)\n", sqlite3_errmsg(db));
	}

	if( sqlite3_exec(db, "create table IF NOT EXISTS match_Registry(action NUMERIC, tool NUMERIC, key TEXT, mtime NUMERIC);", NULL, NULL, NULL) == SQLITE_OK) { 
	} else {
		exit(1);
	}

	rc = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, &errorMsg); 

	sprintf (buffer,"INSERT INTO match_Registry(action, tool, key, mtime) VALUES ( ?1, ?2, ?3, ?4)");

	if(sqlite3_prepare_v2(db, buffer, strlen(buffer), &stmt, NULL) == SQLITE_OK) 
	{
	//	puts(">> Prepared Statement is ready : Succeeded!\n");
	}
	else
	{
		puts("테이블 값 입력에 실패하였습니다.");
	}

	while (!mq->empty())
	{
		match_registry * tmp;
		tmp=mq->front();
		sqlite3_bind_int(stmt, 1, mq->front()->action);
		sqlite3_bind_int(stmt, 2, mq->front()->tool);
		sqlite3_bind_text(stmt, 3, mq->front()->key, strlen(mq->front()->key), SQLITE_STATIC);
		sqlite3_bind_int64(stmt, 4, mq->front()->time);
		mq->pop();
		if ( sqlite3_step(stmt) != SQLITE_DONE )  {
		}
		sqlite3_reset(stmt); 
		free(tmp);
	}
	rc = sqlite3_exec(db, "COMMIT TRANSACTION;", NULL, NULL, &errorMsg);
	//fprintf(stderr, " Commit result : %s\n", errorMsg);
	sqlite3_finalize(stmt);
	sqlite3_close(db);

}
void FILE2SQL(MATCHFILEQ * mq)
{

	sqlite3 *db = NULL; 
	sqlite3_stmt *stmt = NULL;
	char *sql;
	int rc; 
	unsigned int i;
	char *buffer = (char *)malloc(500); 
	char* errorMsg = NULL;

	memset(buffer, 0x00, sizeof(char)*500); 


	if(sqlite3_open("info.db", &db) != SQLITE_OK)
	{
		fprintf(stderr, "DB접근이 어렵습니다. (오류 %s)\n", sqlite3_errmsg(db));
	}

	if( sqlite3_exec(db, "create table IF NOT EXISTS match_file(action NUMERIC, tool NUMERIC, path TEXT, FN_mtime NUMERIC, FN_ctime NUMERIC, FN_atime NUMERIC, SI_mtime NUMERIC, SI_atime NUMERIC, SI_ctime NUMERIC);", NULL, NULL, NULL) == SQLITE_OK) { 
	} else {
		exit(1);
	}

	rc = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, &errorMsg); 

	sprintf (buffer,"INSERT INTO match_file(action, tool, path, FN_mtime, FN_ctime, FN_atime, SI_mtime, SI_ctime, SI_atime) VALUES ( ?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9)");

	if(sqlite3_prepare_v2(db, buffer, strlen(buffer), &stmt, NULL) == SQLITE_OK) 
	{
		//puts(">> Prepared Statement is ready : Succeeded!\n");
	}
	else
	{
		puts("테이블 값 입력에 실패하였습니다.");
	}

	while (!mq->empty())
	{
		match_file * tmp;
		tmp=mq->front();
		sqlite3_bind_int(stmt, 1, mq->front()->action);
		sqlite3_bind_int(stmt, 2, mq->front()->tool);
		sqlite3_bind_text(stmt, 3, mq->front()->path, strlen(mq->front()->path), SQLITE_STATIC);
		sqlite3_bind_int64(stmt, 4, mq->front()->FN_Mtime);
		sqlite3_bind_int64(stmt, 5, mq->front()->FN_Ctime);
		sqlite3_bind_int64(stmt, 6, mq->front()->FN_Atime);
		sqlite3_bind_int64(stmt, 7, mq->front()->SI_Mtime);
		sqlite3_bind_int64(stmt, 8, mq->front()->SI_Ctime);
		sqlite3_bind_int64(stmt, 9, mq->front()->FN_Atime);
		mq->pop();
		if ( sqlite3_step(stmt) != SQLITE_DONE )  {
		}
		sqlite3_reset(stmt); 
		free(tmp);
	}
	rc = sqlite3_exec(db, "COMMIT TRANSACTION;", NULL, NULL, &errorMsg);
	//fprintf(stderr, " Commit result : %s\n", errorMsg);
	sqlite3_finalize(stmt);
	sqlite3_close(db);
}

void tool_info()
{

	sqlite3 *infodb = NULL; 
	sqlite3 *sigdb = NULL;
	sqlite3_stmt *infostmt = NULL;
	sqlite3_stmt *sigstmt = NULL;
	char *sql;
	int rc; 
	unsigned int i;
	char buffer[500]; 
	char* errorMsg = NULL;

	memset(buffer, 0x00, sizeof(char)*500); 


	if(sqlite3_open("info.db", &infodb) != SQLITE_OK)
	{
		fprintf(stderr, "DB접근이 어렵습니다. (오류 %s)\n", sqlite3_errmsg(infodb));
	}
	if(sqlite3_open("sig.db", &sigdb) != SQLITE_OK)
	{
		fprintf(stderr, "DB접근이 어렵습니다. (오류 %s)\n", sqlite3_errmsg(sigdb));
	}
	if( sqlite3_exec(infodb, "create table IF NOT EXISTS tool(detail TEXT, name TEXT, tool_num NUMERIC, type TEXT);", NULL, NULL, NULL) == SQLITE_OK) { 
	} else {
		exit(1);
	}

	rc = sqlite3_exec(infodb, "BEGIN TRANSACTION;", NULL, NULL, &errorMsg); 

	sprintf (buffer,"INSERT INTO tool(detail, name, tool_num, type) VALUES ( ?4, ?1, ?2, ?3)");

	if(sqlite3_prepare_v2(infodb, buffer, strlen(buffer), &infostmt, NULL) == SQLITE_OK);// printf("prepare success infodb\n");

	memset(buffer, 0x00, sizeof(char)*500);
    sprintf (buffer,"SELECT detail, name, tool_num, type from tool;");
	//sprintf (buffer,"SELECT * from tool;");
	if(sqlite3_prepare_v2(sigdb, buffer, strlen(buffer), &sigstmt, NULL) == SQLITE_OK); //printf("prepared success sigdb\n");

	int row = 0;
	while (1) {
		int s;


		s = sqlite3_step (sigstmt);
		if (s == SQLITE_ROW) {

			int bytes;
			char text[0x1000];
			char text1[0x1000];
			char text2[0x1000];
			memset(text, 0, 0x1000);
			memset(text1, 0, 0x1000);
			memset(text2, 0, 0x1000);
			sprintf(text, "%s", (char*)sqlite3_column_text(sigstmt, 1));
			//printf("%s : ", text);
			sqlite3_bind_text(infostmt, 1, text, strlen(text), SQLITE_STATIC);

			//sprintf(text1, "%s", (char*)sqlite3_column_text(sigstmt, 2));
			//printf("%s : ", text1);
			//sqlite3_bind_text(infostmt, 2, text1, strlen(text1), SQLITE_STATIC);
			sqlite3_bind_int(infostmt,2,sqlite3_column_int(sigstmt,2));

			sprintf(text1, "%s", (char*)sqlite3_column_text(sigstmt, 3));
			//printf("%s : ", text1);
			sqlite3_bind_text(infostmt, 3, text1, strlen(text1), SQLITE_STATIC);
		
			sprintf(text2, "%s", (char*)sqlite3_column_text(sigstmt, 0));
			//printf("%s\n", text2);
			sqlite3_bind_text(infostmt, 4, text2, strlen(text2), SQLITE_STATIC);
			sqlite3_step(infostmt);
			sqlite3_reset(infostmt);
//			row++;
		}
		else if (s == SQLITE_DONE) {
			break;
		}
		else {
			fprintf (stderr, "sig Failed.\n");
			exit (1);

		}
		 
	}

	rc = sqlite3_exec(infodb, "COMMIT TRANSACTION;", NULL, NULL, &errorMsg);
	//fprintf(stderr, " Commit result : %s\n", errorMsg);
	sqlite3_finalize(sigstmt);
	sqlite3_close(sigdb);

	sqlite3_finalize(infostmt);
	sqlite3_close(infodb);

}

void get_USB(USBq * q)
{

	sqlite3 *sigDB = NULL;


	sqlite3_stmt *sigSTMT = NULL;
	char *sql;
	//int rc; 
	char* errorMsg = NULL;
	if(sqlite3_open("info.db", &sigDB) != SQLITE_OK)
	{
		fprintf(stderr, "DB접근이 어렵습니다. (오류 %s)\n", sqlite3_errmsg(sigDB));
	}

	sql = "select * from Registry where PATH like \"%%/Enum/USBSTOR%%\";";
	if( sqlite3_exec(sigDB, sql, NULL, NULL, NULL) == SQLITE_OK) { 

	} else {

		exit(1);
	}


	if(sqlite3_prepare_v2(sigDB, sql, strlen(sql), &sigSTMT, NULL) == SQLITE_OK) 
	{
		//puts(">> Prepared Statement is ready : Succeeded!\n");
	}
	else
	{
		//puts("테이블 값 입력에 실패하였습니다.");
	}

	int row = 0;
	while (1) {
		int s;


		s = sqlite3_step (sigSTMT);
		if (s == SQLITE_ROW) {
			USB * tmp = (USB * ) malloc(sizeof(USB));
			memset(tmp, 0, sizeof(USB));
			int bytes;
			const unsigned char * text;
			////////////////////////////////////////////////////////////////////
			tmp->time = sqlite3_column_int(sigSTMT, 1);
			sprintf(tmp->PATH, "%s", (char * )sqlite3_column_text(sigSTMT, 0));
			////////////////////////////////////////////////////////////////////
			q->push(tmp);
			row++;
		}
		else if (s == SQLITE_DONE) {
			break;
		}
		else {
			fprintf (stderr, "Failed.\n");
			exit (1);
		}
	}
	sqlite3_finalize(sigSTMT);
	sqlite3_close(sigDB);
}

void USBG2SQL(USBq * q)
{
	 sqlite3 *db = NULL; 
    sqlite3_stmt *stmt = NULL;
    char *sql;
    int rc; 
    unsigned int i;
    char *buffer = (char *)malloc(500); 
    char* errorMsg = NULL;

    memset(buffer, 0x00, sizeof(char)*500); 


    if(sqlite3_open("info.db", &db) != SQLITE_OK)
    {
        fprintf(stderr, "DB접근이 어렵습니다. (오류 %s)\n", sqlite3_errmsg(db));
    }

	if( sqlite3_exec(db, "create table IF NOT EXISTS USB(PATH TEXT, time NUMERIC);", NULL, NULL, NULL) == SQLITE_OK) { 
	} else {
		exit(1);
	}

    rc = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, &errorMsg); 
  
	sprintf (buffer,"INSERT INTO USB(PATH, time) VALUES ( ?1, ?2)");

    if(sqlite3_prepare_v2(db, buffer, strlen(buffer), &stmt, NULL) == SQLITE_OK) 
    {
        puts(">> Prepared Statement is ready : Succeeded!\n");
    }
    else
    {
        puts("테이블 값 입력에 실패하였습니다.");
    }

	 while (!q->empty())
    {
		USB * tmp;
		tmp=q->front();
		sqlite3_bind_text(stmt, 1, q->front()->PATH, strlen(q->front()->PATH), SQLITE_STATIC);
		sqlite3_bind_int64(stmt, 2, q->front()->time);
        q->pop();
		if ( sqlite3_step(stmt) != SQLITE_DONE )  {
        }
        sqlite3_reset(stmt); 
		free(tmp);
    }
    rc = sqlite3_exec(db, "COMMIT TRANSACTION;", NULL, NULL, &errorMsg);
    //fprintf(stderr, " Commit result : %s\n", errorMsg);
    sqlite3_finalize(stmt);
    sqlite3_close(db);

}
