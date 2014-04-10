#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>
#ifdef __cplusplus
#include <cstdio>
#else
#include <stdio.h>.
#endif
#include <time.h>
#include <Windows.h>
#include "define.h"


#define TABLE_COUNT 3

unsigned int LIMITCOUNT_ctime = 50;

char *newValues_ctime[5];
int count_ctime = 0;
int num_ctime = 0;
int division_ctime = 0;
char *newValues2_ctime[3] = {0,};
char dateString_ctime[32];

struct CallbackData {
	FILE *fp;
	int firstItem;
	int remain;
};
struct RemainData{
	char rTime[200];
	char rPath[200];
	char rType[200];
};
typedef struct RemainData rm;

static rm arr_ctime[400];
 static  rm temp_ctime;

static const char *Weekdays[] = {
    "Sun", "Mon", "Tue", "Wed",
    "Thu", "Fri", "Sat"
     };
static const char *monthdays[] = {
    "Jan", "Feb", "Mar", "Api", "May", "Jun",
    "July",	"Aug","Sep", "Oct", "Nov", 	"Dec"
   };

void epochtimetolocaltime_ctime(char* wordtotime, char* dateString) {
    time_t curSec;
	struct tm *curDate;
	char *dayday;
	int tmp=atoi(wordtotime);
	curSec = (time_t)tmp;
    curDate = localtime(&curSec);
    sprintf(dateString,"%s %s %02d %02d %02d:%02d:%02d ", Weekdays[curDate->tm_wday], monthdays[curDate->tm_mon], curDate->tm_mday, curDate->tm_year+1900, curDate->tm_hour, curDate->tm_min, curDate->tm_sec);
}

static char* replaceStr(char* orgStr, char replChar) {
	int replCharCount = 0;
	char *newData = NULL;
	char* tmp = orgStr;
	char* newDataPtr = NULL;

	while ( *tmp ) {
		if  ( *tmp == replChar ) {
			replCharCount ++;
		}
		*tmp++;
	}

	newData = (char*)malloc(strlen(orgStr)+replCharCount * sizeof(char) + 1);
	
	memset(newData, 0x00, strlen(orgStr)+replCharCount * sizeof(char) + 1);
	newDataPtr = newData;
	while ( *orgStr ) {
		if ( *orgStr == replChar) {
			*newDataPtr = *orgStr;
			*newDataPtr++;
			*newDataPtr = replChar;
		} else {
			*newDataPtr = *orgStr;
		}
		*newDataPtr++;
		*orgStr++;
	}
	return newData;

}

static int callbackc(void *data, int nColumnCount, char **columnValues, char **columnNames){
	int  i, j ;
	char *action =  (char *)malloc(500);
   struct CallbackData *callbackData = (struct CallbackData *)data;
   FILE *fp = callbackData->fp;

   memset(action, 0x00, sizeof(char)*500);

   for(i=0; i<nColumnCount; i++) {
	   newValues2_ctime[i] = replaceStr(columnValues[i], '\\');
    }

		memset(arr_ctime[num_ctime].rPath, 0x00, sizeof(char)*200);
		memset(arr_ctime[num_ctime].rTime, 0x00, sizeof(char)*200);
		   strcpy(arr_ctime[num_ctime].rPath,newValues2_ctime[0]);
		   strcpy(arr_ctime[num_ctime].rTime,newValues2_ctime[1]);
		   strcpy(arr_ctime[num_ctime].rType,newValues2_ctime[2]);

		num_ctime++;
		
  for(i=0; i<nColumnCount; i++) {
	   free(newValues2_ctime[i]);
   }
   
   return 0;
}

static int callback2(void *data, int nColumnCount, char **columnValues, char **columnNames){
	int i;	
	char *action =  (char *)malloc(500);
   struct CallbackData *callbackData = (struct CallbackData *)data;
   FILE *fp = callbackData->fp;

   memset(action, 0x00, sizeof(char)*500);

    for(i=0; i<nColumnCount; i++) {
	   newValues_ctime[i] = replaceStr(columnValues[i], '\\');
    }
	if(atoi(newValues_ctime[4]) == 1){
	   action = "install";
	}else if(atoi(newValues_ctime[4]) == 2){
		   action = "open";
	}else if(newValues_ctime[4] == "3"){
		   strcpy(action, "run");
	}else if(newValues_ctime[4] == "1"){
		   strcpy(action, "uninstall");
	}

	 
   if (callbackData->firstItem == 0) {
	   epochtimetolocaltime_ctime(newValues_ctime[3], dateString_ctime);
		fprintf(fp,",\n        {'start': '%s',\n        'title': '%s detected %s' ,\n        'description': '%s',\n        }",
			dateString_ctime,newValues_ctime[0],action,newValues_ctime[1]);
   } else {
	   epochtimetolocaltime_ctime(newValues_ctime[3], dateString_ctime);
		fprintf(fp,"        {'start': '%s',\n        'title': '%s detected %s',\n        'description': '%s',\n        }",
			dateString_ctime,newValues_ctime[0],action,newValues_ctime[1]);
		callbackData->firstItem = 0;
   }
   count_ctime = nColumnCount;
   
   return 0;
}

int js_ctime_create(int limit_count){

	LIMITCOUNT_ctime = limit_count;

	sqlite3 *db;
	char *zErrMsg = 0;
	int rc, i ,j;
	char *sql;
	FILE *fp;
	char *buffer = (char *)malloc(500);

	struct CallbackData callbackData;

	rc = sqlite3_open("info.db", &db);
   if( rc ){
    //  fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      exit(0);
   }else{
     // fprintf(stderr, "Opened database successfully\n");
   }
   
	sql = "SELECT tool.name, tool.detail, match_Registry.key, match_Registry.mtime, match_Registry.action  from match_Registry INNER JOIN tool ON match_Registry.tool = tool.tool_num";

	fp = fopen("C_Data.js","w");
	fprintf(fp,"var C_Data = {\n'dateTimeFormat': 'Gregorian',\n'wikiURL': ""\"http://simile.mit.edu/shelf/""\",\n'wikiSection': ""\"Simile Cubism Timeline""\",");
	fprintf(fp,"\n");
	fprintf(fp,"'events' : [\n");

	callbackData.fp = fp;
	callbackData.firstItem = 1;
   rc = sqlite3_exec(db, sql, callback2, (void*)&callbackData, &zErrMsg);
   if( rc != SQLITE_OK ){
    //  fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
    //  fprintf(stdout, "Operation done successfully\n");
   }
   
   sql = "SELECT tool.name, tool.detail, match_file.path, match_file.FN_mtime, match_file.action  from match_file INNER JOIN tool ON tool.tool_num = match_file.tool";

   rc = sqlite3_exec(db, sql, callback2, (void*)&callbackData, &zErrMsg);
   if( rc != SQLITE_OK ){
     // fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
     // fprintf(stdout, "Operation done successfully\n");
   }
   
   //match < limit LIMITCOUNT_ctime output
   num_ctime = 0;
   division_ctime = 1;
   sprintf(buffer,"SELECT PATH,TIME, TYPE from Registry where TIME > %s ORDER BY TIME LIMIT %d",newValues_ctime[3],LIMITCOUNT_ctime);

   rc = sqlite3_exec(db, buffer, callbackc, (void*)&callbackData, &zErrMsg);
   if( rc != SQLITE_OK ){
     // fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
    //  fprintf(stdout, "Operation done successfully\n");
   }

   sprintf(buffer,"SELECT FULLPATH,  SI_createTm, TYPE from MFT where SI_createTm > %s ORDER BY SI_createTm LIMIT %d",newValues_ctime[3],LIMITCOUNT_ctime);

   rc = sqlite3_exec(db, buffer, callbackc, (void*)&callbackData, &zErrMsg);
   if( rc != SQLITE_OK ){
      //fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
     // fprintf(stdout, "Operation done successfully\n");
   }

   sprintf(buffer,"SELECT URL, TIME, TYPE from HISTORY where TIME > %s ORDER BY TIME LIMIT %d",newValues_ctime[3],LIMITCOUNT_ctime);

   rc = sqlite3_exec(db, buffer, callbackc, (void*)&callbackData, &zErrMsg);
   if( rc != SQLITE_OK ){
     // fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
      //fprintf(stdout, "Operation done successfully\n");
   }

   sprintf(buffer,"SELECT Location, TIME, TYPE from DOWNLOAD where TIME > %s ORDER BY TIME LIMIT %d",newValues_ctime[3],LIMITCOUNT_ctime);
 
   rc = sqlite3_exec(db, buffer, callbackc, (void*)&callbackData, &zErrMsg);
   if( rc != SQLITE_OK ){
     // fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
    //  fprintf(stdout, "Operation done successfully\n");
   }
   //bubble
		for(i=0;i<num_ctime;i++)
		 {
			for(j=i+1;j<num_ctime;j++)
			{
				if(atoi(arr_ctime[i].rTime) > atoi(arr_ctime[j].rTime) )            
				{
					temp_ctime = arr_ctime[j];
					arr_ctime[j] = arr_ctime[i];
					arr_ctime[i] = temp_ctime;
				}
			}
		}
   		
		for(i=0; i<LIMITCOUNT_ctime;i++){
		epochtimetolocaltime_ctime(arr_ctime[i].rTime, dateString_ctime);
		strcpy(arr_ctime[i].rTime, dateString_ctime);
		if( atoi(arr_ctime[i].rType)==1){
			strcpy(arr_ctime[i].rType,"IE");
		}
		}
		for(i=0; i<LIMITCOUNT_ctime ;i++){
			fprintf(fp,",\n        {'start': '%s',\n        'description': '%s',\n        'title': '%s',\n        }",arr_ctime[i].rTime,arr_ctime[i].rPath, arr_ctime[i].rType);
		}

		//match > limit LIMITCOUNT_ctime output
   num_ctime = 0;
   sprintf(buffer,"SELECT PATH,TIME, TYPE from Registry where TIME < %s ORDER BY TIME DESC LIMIT %d",newValues_ctime[3],LIMITCOUNT_ctime);

   rc = sqlite3_exec(db, buffer, callbackc, (void*)&callbackData, &zErrMsg);
   if( rc != SQLITE_OK ){
      //fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
     // fprintf(stdout, "Operation done successfully\n");
   }

   sprintf(buffer,"SELECT FULLPATH, SI_createTm, TYPE from MFT where SI_createTm < %s ORDER BY SI_createTm DESC LIMIT %d",newValues_ctime[3],LIMITCOUNT_ctime);

   rc = sqlite3_exec(db, buffer, callbackc, (void*)&callbackData, &zErrMsg);
   if( rc != SQLITE_OK ){
    ///  fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
     // fprintf(stdout, "Operation done successfully\n");
   }

   sprintf(buffer,"SELECT URL, TIME, TYPE from HISTORY where TIME < %s ORDER BY TIME DESC LIMIT %d",newValues_ctime[3],LIMITCOUNT_ctime);

   rc = sqlite3_exec(db, buffer, callbackc, (void*)&callbackData, &zErrMsg);
   if( rc != SQLITE_OK ){
     // fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
     // fprintf(stdout, "Operation done successfully\n");
   }

   sprintf(buffer,"SELECT Location, TIME, TYPE from DOWNLOAD where TIME < %s ORDER BY TIME DESC LIMIT %d",newValues_ctime[3],LIMITCOUNT_ctime);
 
   rc = sqlite3_exec(db, buffer, callbackc, (void*)&callbackData, &zErrMsg);
   if( rc != SQLITE_OK ){
     // fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
      //fprintf(stdout, "Operation done successfully\n");
   }
   //bubble
		for(i=0;i<num_ctime;i++)
		 {
			for(j=i+1;j<num_ctime;j++)
			{
				if(atoi(arr_ctime[i].rTime) < atoi(arr_ctime[j].rTime) )            
				{
					temp_ctime = arr_ctime[j];
					arr_ctime[j] = arr_ctime[i];
					arr_ctime[i] = temp_ctime;
				}
			}
		}
   		
		for(i=0; i<LIMITCOUNT_ctime;i++){
		epochtimetolocaltime_ctime(arr_ctime[i].rTime, dateString_ctime);
		strcpy(arr_ctime[i].rTime, dateString_ctime);
		if( atoi(arr_ctime[i].rType)==1){
			strcpy(arr_ctime[i].rType,"IE");
		}
		}
		for(i=0; i<LIMITCOUNT_ctime ;i++){
			fprintf(fp,",\n        {'start': '%s',\n        'description': '%s',\n        'title': '%s',\n        }",arr_ctime[i].rTime,arr_ctime[i].rPath,arr_ctime[i].rType);
		}

   for(i=0; i<count_ctime; i++) {
	   free(newValues_ctime[i]);
   }
   
    fprintf(fp,"\n");
	fprintf(fp,"]\n}");
	fprintf(fp,"\n;");
	fclose(fp);
 
	sqlite3_close(db);

    return 0;
}
