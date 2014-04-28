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

struct CallbackData {
   FILE *fp;
   int firstItem;
};

static const char *Weekdays[] = {
    "Sun", "Mon", "Tue", "Wed",
    "Thu", "Fri", "Sat"
     };
static const char *monthdays[] = {
    "Jan", "Feb", "Mar", "Api", "May", "Jun",
    "July",   "Aug","Sep", "Oct", "Nov",    "Dec"
   };

void epochtimetolocaltime_toolinfo(char* wordtotime, char* dateString) {
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

static int call(void *data, int nColumnCount, char **columnValues, char **columnNames){
   int i;
   char *about[4];
   char dateString[32];
 
   // status message
   struct CallbackData *callbackData = (struct CallbackData *)data;
   FILE *fp = callbackData->fp;
 
    for(i=0; i<nColumnCount; i++) {
      about[i] = replaceStr(columnValues[i], '\\');
    }
 
   if (callbackData->firstItem == 0) {
      fprintf(fp,",\n        {'name': '%s',\n         'description' :  '%s' ,\n        }",
         about[0],about[1],about[2],about[3]);
   } else {
      fprintf(fp,"        {'name': '%s',\n         'description' : '%s',\n        }",
         about[0],about[1],about[2],about[3]);
      callbackData->firstItem = 0;
   }
 
   for(i=0; i<nColumnCount; i++) {
      free(about[i]);
   }
 
   return 0;
}

int js_toolinfo(){
   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
   char *sql;
   char **result, **result1;
   int nrows, ncols, i,j;
   char *nzErr;
   FILE *fp;

   struct CallbackData callbackData;

   rc = sqlite3_open("info.db", &db);
   if( rc ){
      //fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      exit(0);
   }else{
     // fprintf(stderr, "Opened database successfully\n");
   }
   
   sql = "SELECT tool.name, tool.detail, match_Registry.action from match_Registry INNER JOIN tool ON match_Registry.tool = tool.tool_num";

   fp = fopen("jstopmake.js","w");
   fprintf(fp,"var Data = {");
   fprintf(fp,"\n");
   fprintf(fp,"'events' : [\n");

   callbackData.fp = fp;
   callbackData.firstItem = 1;
   rc = sqlite3_exec(db, sql, call, (void*)&callbackData, &zErrMsg);
   if( rc != SQLITE_OK ){
     // fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
    //  fprintf(stdout, "Operation done successfully\n");
   }
   
   sql = "SELECT tool.name, tool.detail, match_file.action from match_file INNER JOIN tool ON tool.tool_num = match_file.tool";

   rc = sqlite3_exec(db, sql, call, (void*)&callbackData, &zErrMsg);
   if( rc != SQLITE_OK ){
     //fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
      //fprintf(stdout, "Operation done successfully\n");
   }

   fprintf(fp,"\n");
   fprintf(fp,"]\n}");
   fprintf(fp,"\n;");
   fclose(fp);

   sqlite3_close(db);

    return 0;
}