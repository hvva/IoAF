
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

unsigned int LIMITCOUNT = 50;

char *newValues[5];
int count = 0;
int num = 0;
int division = 0;
char *newValues2[3] = {0,};
char dateString[32];

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

 static rm arr_atime[400];
 static rm temp_atime;

static const char *Weekdays[] = {
    "Sun", "Mon", "Tue", "Wed",
    "Thu", "Fri", "Sat"
     };
static const char *monthdays[] = {
    "Jan", "Feb", "Mar", "Api", "May", "Jun",
    "July",	"Aug","Sep", "Oct", "Nov", 	"Dec"
   };

void epochtimetolocaltime(char* wordtotime, char* dateString) {
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

static int callbacka(void *data, int nColumnCount, char **columnValues, char **columnNames){
	int  i, j ;
	char *action =  (char *)malloc(500);
   struct CallbackData *callbackData = (struct CallbackData *)data;
   FILE *fp = callbackData->fp;

   memset(action, 0x00, sizeof(char)*500);

   for(i=0; i<nColumnCount; i++) {
	   newValues2[i] = replaceStr(columnValues[i], '\\');
    }

		memset(arr_atime[num].rPath, 0x00, sizeof(char)*200);
		memset(arr_atime[num].rTime, 0x00, sizeof(char)*200);
		   strcpy(arr_atime[num].rPath,newValues2[0]);
		   strcpy(arr_atime[num].rTime,newValues2[1]);
		   strcpy(arr_atime[num].rType,newValues2[2]);
	
		num++;
		
  for(i=0; i<nColumnCount; i++) {
	   free(newValues2[i]);
   }
   
   return 0;
}

static int callback3(void *data, int nColumnCount, char **columnValues, char **columnNames){
	int i;	
	char *action =  (char *)malloc(500);
   struct CallbackData *callbackData = (struct CallbackData *)data;
   FILE *fp = callbackData->fp;

   memset(action, 0x00, sizeof(char)*500);

    for(i=0; i<nColumnCount; i++) {
	   newValues[i] = replaceStr(columnValues[i], '\\');
    }
	if(atoi(newValues[4]) == 1){
	   action = "install";
	}else if(atoi(newValues[4]) == 2){
		   action = "open";
	}else if(newValues[4] == "3"){
		   strcpy(action, "run");
	}else if(newValues[4] == "1"){
		   strcpy(action, "uninstall");
	}

	 
   if (callbackData->firstItem == 0) {
	   epochtimetolocaltime(newValues[3], dateString);
		fprintf(fp,",\n        {'start': '%s',\n        'title': '%s detected %s' ,\n        'description': '%s',\n        }",
			dateString,newValues[0],action,newValues[1]);
   } else {
	   epochtimetolocaltime(newValues[3], dateString);
		fprintf(fp,"        {'start': '%s',\n        'title': '%s detected %s',\n        'description': '%s',\n        }",
			dateString,newValues[0],action,newValues[1]);
		callbackData->firstItem = 0;
   }
   count = nColumnCount;
   
   return 0;
}

int js_atime_create(int limit_count){

	LIMITCOUNT = limit_count;

	sqlite3 *db;
	char *zErrMsg = 0;
	int rc, i ,j;
	char *sql;
	FILE *fp;
	char *buffer = (char *)malloc(500);

	struct CallbackData callbackData;

	rc = sqlite3_open("info.db", &db);
   if( rc ){
     // fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      exit(0);
   }else{
      //fprintf(stderr, "Opened database successfully\n");
   }
   
	sql = "SELECT tool.name, tool.detail, match_Registry.key, match_Registry.mtime, match_Registry.action  from match_Registry INNER JOIN tool ON match_Registry.tool = tool.tool_num";

	fp = fopen("A_Data.js","w");
	fprintf(fp,"var A_Data = {\n'dateTimeFormat': 'Gregorian',\n'wikiURL': ""\"http://simile.mit.edu/shelf/""\",\n'wikiSection': ""\"Simile Cubism Timeline""\",");
	fprintf(fp,"\n");
	fprintf(fp,"'events' : [\n");

	callbackData.fp = fp;
	callbackData.firstItem = 1;
   rc = sqlite3_exec(db, sql, callback3, (void*)&callbackData, &zErrMsg);
   if( rc != SQLITE_OK ){
    //  fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
     // fprintf(stdout, "Operation done successfully\n");
   }
   
   sql = "SELECT tool.name, tool.detail, match_file.path, match_file.FN_mtime, match_file.action  from match_file INNER JOIN tool ON tool.tool_num = match_file.tool";

   rc = sqlite3_exec(db, sql, callback3, (void*)&callbackData, &zErrMsg);
   if( rc != SQLITE_OK ){
    //  fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
    //  fprintf(stdout, "Operation done successfully\n");
   }
   
   //match < limit LIMITCOUNT output
   num = 0;
   division = 1;
   sprintf(buffer,"SELECT PATH,TIME, TYPE from Registry where TIME > %s ORDER BY TIME LIMIT %d",newValues[3],LIMITCOUNT);

   rc = sqlite3_exec(db, buffer, callbacka, (void*)&callbackData, &zErrMsg);
   if( rc != SQLITE_OK ){
    //  fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
      //fprintf(stdout, "Operation done successfully\n");
   }

   sprintf(buffer,"SELECT FULLPATH,  SI_accessTm, TYPE from MFT where SI_accessTm > %s ORDER BY SI_accessTm LIMIT %d",newValues[3],LIMITCOUNT);

   rc = sqlite3_exec(db, buffer, callbacka, (void*)&callbackData, &zErrMsg);
   if( rc != SQLITE_OK ){
     // fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
    //  fprintf(stdout, "Operation done successfully\n");
   }

   sprintf(buffer,"SELECT URL, TIME, TYPE from HISTORY where TIME > %s ORDER BY TIME LIMIT %d",newValues[3],LIMITCOUNT);

   rc = sqlite3_exec(db, buffer, callbacka, (void*)&callbackData, &zErrMsg);
   if( rc != SQLITE_OK ){
     // fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
    //  fprintf(stdout, "Operation done successfully\n");
   }

   sprintf(buffer,"SELECT Location, TIME, TYPE from DOWNLOAD where TIME > %s ORDER BY TIME LIMIT %d",newValues[3],LIMITCOUNT);
 
   rc = sqlite3_exec(db, buffer, callbacka, (void*)&callbackData, &zErrMsg);
   if( rc != SQLITE_OK ){
    //  fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
     // fprintf(stdout, "Operation done successfully\n");
   }
   //bubble
		for(i=0;i<num;i++)
		 {
			for(j=i+1;j<num;j++)
			{
				if(atoi(arr_atime[i].rTime) > atoi(arr_atime[j].rTime) )            
				{
					temp_atime = arr_atime[j];
					arr_atime[j] = arr_atime[i];
					arr_atime[i] = temp_atime;
				}
			}
		}
   		
		for(i=0; i<LIMITCOUNT;i++){
		epochtimetolocaltime(arr_atime[i].rTime, dateString);
		strcpy(arr_atime[i].rTime, dateString);
		if( atoi(arr_atime[i].rType)==1){
			strcpy(arr_atime[i].rType,"IE");
		}
		}
		for(i=0; i<LIMITCOUNT ;i++){
			fprintf(fp,",\n        {'start': '%s',\n        'description': '%s',\n        'title': '%s',\n        }",arr_atime[i].rTime,arr_atime[i].rPath, arr_atime[i].rType);
		}

		//match > limit LIMITCOUNT output
   num = 0;
   sprintf(buffer,"SELECT PATH,TIME, TYPE from Registry where TIME < %s ORDER BY TIME DESC LIMIT %d",newValues[3],LIMITCOUNT);

   rc = sqlite3_exec(db, buffer, callbacka, (void*)&callbackData, &zErrMsg);
   if( rc != SQLITE_OK ){
    //  fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
      //fprintf(stdout, "Operation done successfully\n");
   }

   sprintf(buffer,"SELECT FULLPATH, SI_accessTm, TYPE from MFT where SI_accessTm < %s ORDER BY SI_accessTm DESC LIMIT %d",newValues[3],LIMITCOUNT);

   rc = sqlite3_exec(db, buffer, callbacka, (void*)&callbackData, &zErrMsg);
   if( rc != SQLITE_OK ){
     // fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
    //  fprintf(stdout, "Operation done successfully\n");
   }

   sprintf(buffer,"SELECT URL, TIME, TYPE from HISTORY where TIME < %s ORDER BY TIME DESC LIMIT %d",newValues[3],LIMITCOUNT);

   rc = sqlite3_exec(db, buffer, callbacka, (void*)&callbackData, &zErrMsg);
   if( rc != SQLITE_OK ){
   //   fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
     // fprintf(stdout, "Operation done successfully\n");
   }
   sprintf(buffer,"SELECT Location, TIME, TYPE from DOWNLOAD where TIME < %s ORDER BY TIME DESC LIMIT %d",newValues[3],LIMITCOUNT);
 
   rc = sqlite3_exec(db, buffer, callbacka, (void*)&callbackData, &zErrMsg);
   if( rc != SQLITE_OK ){
     // fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
    //  fprintf(stdout, "Operation done successfully\n");
   }
   //bubble
		for(i=0;i<num;i++)
		 {
			for(j=i+1;j<num;j++)
			{
				if(atoi(arr_atime[i].rTime) < atoi(arr_atime[j].rTime) )            
				{
					temp_atime = arr_atime[j];
					arr_atime[j] = arr_atime[i];
					arr_atime[i] = temp_atime;
				}
			}
		}
   		
		for(i=0; i<LIMITCOUNT;i++){
		epochtimetolocaltime(arr_atime[i].rTime, dateString);
		strcpy(arr_atime[i].rTime, dateString);
		if( atoi(arr_atime[i].rType)==1){
			strcpy(arr_atime[i].rType,"IE");
		}
		}
		for(i=0; i<LIMITCOUNT ;i++){
			fprintf(fp,",\n        {'start': '%s',\n        'description': '%s',\n        'title': '%s',\n        }",arr_atime[i].rTime,arr_atime[i].rPath,arr_atime[i].rType);
		}

   for(i=0; i<count; i++) {
	   free(newValues[i]);
   }
   
    fprintf(fp,"\n");
	fprintf(fp,"]\n}");
	fprintf(fp,"\n;");
	fclose(fp);
 
	sqlite3_close(db);

    return 0;
}