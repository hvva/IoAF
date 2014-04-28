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
#include "define.h"

char *newValues_detection[5];
int count_detection = 0;
int num_detection = 0;
int division_detection = 0;
char *newValues2_detection[2] = {0,};
char dateString_detection[32];

struct CallbackData {
	FILE *fp;
	int firstItem;
	int remain;
};
struct RemainData{
	char rTime[200];
	char rPath[200];
};
typedef struct RemainData rm;

char *newValues_file[10];
char dateString1[32];
char dateString2[32];
char dateString3[32];
char dateString4[32];
char dateString5[32];
char dateString6[32];


 static rm arr_detection[21];
 static  rm temp_detection;

static const char *Weekdays[] = {
    "Sun", "Mon", "Tue", "Wed",
    "Thu", "Fri", "Sat"
     };
static const char *monthdays[] = {
    "Jan", "Feb", "Mar", "Api", "May", "Jun",
    "July",	"Aug","Sep", "Oct", "Nov", 	"Dec"
   };

void epochtimetolocaltime_detection(char* wordtotime, char* dateString) {
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
static int calld(void *data, int nColumnCount, char **columnValues, char **columnNames){
   int i;   
   char *action =  (char *)malloc(500);
   struct CallbackData *callbackData = (struct CallbackData *)data;
   FILE *fp = callbackData->fp;

   memset(action, 0x00, sizeof(char)*500);

    for(i=0; i<nColumnCount; i++) {
      newValues_file[i] = replaceStr(columnValues[i], '\\');
    }
   if(atoi(newValues_file[2]) == 1){
      action = "install";
   }else if(atoi(newValues_file[2]) == 2){
         action = "open";
   }else if(newValues_file[2] == "3"){
         strcpy(action, "run");
   }else if(newValues_file[2] == "1"){
         strcpy(action, "uninstall");
   }
 
   if (callbackData->firstItem == 0) {
      epochtimetolocaltime_detection(newValues_file[4], dateString1);
      epochtimetolocaltime_detection(newValues_file[5], dateString2);
      epochtimetolocaltime_detection(newValues_file[6], dateString3);
      epochtimetolocaltime_detection(newValues_file[7], dateString4);
      epochtimetolocaltime_detection(newValues_file[8], dateString5);
      epochtimetolocaltime_detection(newValues_file[9], dateString6);
      fprintf(fp,",\n        {'name': 'Detected Action : %s %s'ed,\n         'PATH' : 'Detected Signature : %s',\n         'FN_mtime' : 'Detected Time : %s' \n         'FN_ctime' : 'Detected Time : %s' \n         'FN_atime' : 'Detected Time : %s' \n         'SI_mtime' : 'Detected Time : %s' \n         'SI_atime' : 'Detected Time : %s' \n         'SI_ctime' : 'Detected Time : %s' \n        }",   
         newValues_file[0], action,newValues_file[3],dateString1,dateString2,dateString3,dateString4,dateString5,dateString6);
   } else {
      epochtimetolocaltime_detection(newValues_file[4], dateString1);
      epochtimetolocaltime_detection(newValues_file[5], dateString2);
      epochtimetolocaltime_detection(newValues_file[6], dateString3);
      epochtimetolocaltime_detection(newValues_file[7], dateString4);
      epochtimetolocaltime_detection(newValues_file[8], dateString5);
      epochtimetolocaltime_detection(newValues_file[9], dateString6);

      fprintf(fp,"        {'name': 'Detected Action : %s %s',\n         'PATH' : 'Detected Signature : %s',\n         'FN_mtime' : 'Detected Time : %s' \n         'FN_ctime' : 'Detected Time : %s' \n         'FN_atime' : 'Detected Time : %s' \n         'SI_mtime' : 'Detected Time : %s' \n         'SI_atime' : 'Detected Time : %s' \n         'SI_ctime' : 'Detected Time : %s' \n        }", 
         newValues_file[0], action,newValues_file[3],dateString1,dateString2,dateString3,dateString4,dateString5,dateString6);
      callbackData->firstItem = 0;
   }
   count_detection = nColumnCount;
      for(i=0; i<count_detection; i++) {
      free(newValues_file[i]);
   }
   
   return 0;
}
static int callback(void *data, int nColumnCount, char **columnValues, char **columnNames){
	int i;	
	char *action =  (char *)malloc(500);
   struct CallbackData *callbackData = (struct CallbackData *)data;
   FILE *fp = callbackData->fp;

   memset(action, 0x00, sizeof(char)*500);

    for(i=0; i<nColumnCount; i++) {
	   newValues_detection[i] = replaceStr(columnValues[i], '\\');
    }
	if(atoi(newValues_detection[4]) == 1){
	   action = "install";
	}else if(atoi(newValues_detection[4]) == 2){
		   action = "open";
	}else if(newValues_detection[4] == "3"){
		   strcpy(action, "run");
	}else if(newValues_detection[4] == "1"){
		   strcpy(action, "uninstall");
	}
 
   if (callbackData->firstItem == 0) {
	  epochtimetolocaltime_detection(newValues_detection[3], dateString_detection);
	  fprintf(fp,",\n        {'name': 'Detected Action : %s %s'ed,\n         'PATH' : 'Detected Signature : %s',\n         'Time' : 'Detected Time : %s' \n        }",   newValues_detection[0],action,newValues_detection[2],dateString_detection);
   } else {
	   epochtimetolocaltime_detection(newValues_detection[3], dateString_detection);
	  fprintf(fp,"        {'name': 'Detected Action : %s %s',\n         'PATH' : 'Detected Signature : %s',\n         'Time' : 'Detected Time : %s' \n        }", newValues_detection[0],action,newValues_detection[2],dateString_detection);
	   callbackData->firstItem = 0;
   }
   count_detection = nColumnCount;
   
   return 0;
}

int js_detection_info(){
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc, i ,j;
	char *sql;
	FILE *fp;
	char *buffer = (char *)malloc(500);

	struct CallbackData callbackData;

	rc = sqlite3_open("info.db", &db);
   if( rc ){
  ///    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      exit(0);
   }else{
   //   fprintf(stderr, "Opened database successfully\n");
   }
   
	sql = "SELECT tool.name, tool.detail, match_Registry.key, match_Registry.mtime, match_Registry.action  from match_Registry INNER JOIN tool ON match_Registry.tool = tool.tool_num";

	fp = fopen("Detection_info.js","w");
	fprintf(fp,"var Data2 = {");
	fprintf(fp,"\n");
	fprintf(fp,"'events' : [\n");

	callbackData.fp = fp;
	callbackData.firstItem = 1;
   rc = sqlite3_exec(db, sql, callback, (void*)&callbackData, &zErrMsg);
   if( rc != SQLITE_OK ){
    //  fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
     // fprintf(stdout, "Operation done successfully\n");
   }
  
   sql = "SELECT tool.name, tool.detail, match_file.action, match_file.path, match_file.FN_mtime, match_file.FN_ctime, match_file.FN_atime,  match_file.SI_mtime, match_file.SI_atime, match_file.SI_ctime  from match_file INNER JOIN tool ON match_file.tool = tool.tool_num";
   rc = sqlite3_exec(db, sql, calld, (void*)&callbackData, &zErrMsg);
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
      fprintf(stdout, "Operation done successfully\n");
   }

   for(i=0; i<count_detection; i++) {
	   free(newValues_detection[i]);
   }
   
    fprintf(fp,"\n");
	fprintf(fp,"]\n}");
	fprintf(fp,"\n;");
	fclose(fp);
 
	sqlite3_close(db);

    return 0;
}










