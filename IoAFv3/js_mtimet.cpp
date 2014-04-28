
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

#define TABLE_COUNT 4

unsigned int LIMITCOUNT_mtime = 50;

char *newValues_mtime[5];
int count_mtime = 0;
int num_mtime = 0;
char *newValues2_mtime[3] = {0,};
char dateString_mtime[32];

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

rm arr_mtime[400];
 static  rm temp_mtime;

static const char *Weekdays[] = {
    "Sun", "Mon", "Tue", "Wed",
    "Thu", "Fri", "Sat"
     };
static const char *monthdays[] = {
    "Jan", "Feb", "Mar", "Api", "May", "Jun",
    "July",   "Aug","Sep", "Oct", "Nov",    "Dec"
   };

void epochtimetolocaltime_mtime(char* wordtotime, char* dateString) {
    time_t curSec;
   struct tm *curDate;
   char *dayday;
   int tmp=atoi(wordtotime);
   curSec = (time_t)tmp;
    curDate = localtime(&curSec);
    sprintf(dateString_mtime,"%s %s %02d %02d %02d:%02d:%02d ", Weekdays[curDate->tm_wday], monthdays[curDate->tm_mon], curDate->tm_mday, curDate->tm_year+1900, curDate->tm_hour, curDate->tm_min, curDate->tm_sec);
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

static int callbackm(void *data, int nColumnCount, char **columnValues, char **columnNames){
   int  i, j ;
   char *action =  (char *)malloc(500);
   struct CallbackData *callbackData = (struct CallbackData *)data;
   FILE *fp = callbackData->fp;

   memset(action, 0x00, sizeof(char)*500);

   for(i=0; i<nColumnCount; i++) {
      newValues2_mtime[i] = replaceStr(columnValues[i], '\\');
    }
      memset(arr_mtime[num_mtime].rPath, 0x00, sizeof(char)*200);
      memset(arr_mtime[num_mtime].rTime, 0x00, sizeof(char)*200);
         strcpy(arr_mtime[num_mtime].rPath,newValues2_mtime[0]);
         strcpy(arr_mtime[num_mtime].rTime,newValues2_mtime[1]);
         strcpy(arr_mtime[num_mtime].rType,newValues2_mtime[2]);

      num_mtime++;
      
  for(i=0; i<nColumnCount; i++) {
      free(newValues2_mtime[i]);
   }
   
   return 0;
}

static int callback1(void *data, int nColumnCount, char **columnValues, char **columnNames){
   int i;   
   char *action =  (char *)malloc(500);
   struct CallbackData *callbackData = (struct CallbackData *)data;
   FILE *fp = callbackData->fp;

   memset(action, 0x00, sizeof(char)*500);

    for(i=0; i<nColumnCount; i++) {
      newValues_mtime[i] = replaceStr(columnValues[i], '\\');
    }
   if(atoi(newValues_mtime[4]) == 1){
      action = "install";
   }else if(atoi(newValues_mtime[4]) == 2){
         action = "open";
   }else if(newValues_mtime[4] == "3"){
         strcpy(action, "run");
   }else if(newValues_mtime[4] == "1"){
         strcpy(action, "uninstall");
   }
    
   if (callbackData->firstItem == 0) {
      epochtimetolocaltime_mtime(newValues_mtime[3], dateString_mtime);
      fprintf(fp,",\n        {'start': '%s',\n        'title': '%s detected %s' ,\n        'description': '%s',\n        }",
         dateString_mtime,newValues_mtime[0],action,newValues_mtime[1]);
   } else {
      epochtimetolocaltime_mtime(newValues_mtime[3], dateString_mtime);
      fprintf(fp,"        {'start': '%s',\n        'title': '%s detected %s',\n        'description': '%s',\n        }",
         dateString_mtime,newValues_mtime[0],action,newValues_mtime[1]);
      callbackData->firstItem = 0;
   }
   count_mtime = nColumnCount;
   
   return 0;
}

int js_mtime_create(int limit_count){

	LIMITCOUNT_mtime = limit_count;

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
     // fprintf(stderr, "Opened database successfully\n");
   }
   
   sql = "SELECT tool.name, tool.detail, match_Registry.key, match_Registry.mtime, match_Registry.action  from match_Registry INNER JOIN tool ON match_Registry.tool = tool.tool_num";

   fp = fopen("M_Data.js","w");
   fprintf(fp,"var M_Data = {\n'dateTimeFormat': 'Gregorian',\n'wikiURL': ""\"http://simile.mit.edu/shelf/""\",\n'wikiSection': ""\"Simile Cubism Timeline""\",");
   fprintf(fp,"\n");
   fprintf(fp,"'events' : [\n");

   callbackData.fp = fp;
   callbackData.firstItem = 1;
   rc = sqlite3_exec(db, sql, callback1, (void*)&callbackData, &zErrMsg);
   if( rc != SQLITE_OK ){
     // fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
    //  fprintf(stdout, "Operation done successfully\n");
   }
   
   sql = "SELECT tool.name, tool.detail, match_file.path, match_file.FN_mtime, match_file.action  from match_file INNER JOIN tool ON tool.tool_num = match_file.tool";

   rc = sqlite3_exec(db, sql, callback1, (void*)&callbackData, &zErrMsg);
   if( rc != SQLITE_OK ){
   //   fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
    //  fprintf(stdout, "Operation done successfully\n");
   }
   
   //match < limit LIMITCOUNT_mtime output
   num_mtime = 0;
   sprintf(buffer,"SELECT PATH,TIME, TYPE from Registry where TIME > %s ORDER BY TIME LIMIT %d",newValues_mtime[3],LIMITCOUNT_mtime);

   rc = sqlite3_exec(db, buffer, callbackm, (void*)&callbackData, &zErrMsg);
   if( rc != SQLITE_OK ){
  //    fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
     // fprintf(stdout, "Operation done successfully\n");
   }

   sprintf(buffer,"SELECT FULLPATH,  Sl_writeTm, TYPE from MFT where Sl_writeTm > %s ORDER BY Sl_writeTm LIMIT %d",newValues_mtime[3],LIMITCOUNT_mtime);

   rc = sqlite3_exec(db, buffer, callbackm, (void*)&callbackData, &zErrMsg);
   if( rc != SQLITE_OK ){
    //  fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
     // fprintf(stdout, "Operation done successfully\n");
   }

   sprintf(buffer,"SELECT URL, TIME, TYPE from HISTORY where TIME > %s ORDER BY TIME LIMIT %d",newValues_mtime[3],LIMITCOUNT_mtime);

   rc = sqlite3_exec(db, buffer, callbackm, (void*)&callbackData, &zErrMsg);
   if( rc != SQLITE_OK ){
   //   fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
     // fprintf(stdout, "Operation done successfully\n");
   }

    sprintf(buffer,"SELECT URL, TIME, TYPE from DOWNLOAD where TIME > %s ORDER BY TIME LIMIT %d",newValues_mtime[3],LIMITCOUNT_mtime);

   rc = sqlite3_exec(db, buffer, callbackm, (void*)&callbackData, &zErrMsg);
   if( rc != SQLITE_OK ){
    //  fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
      //fprintf(stdout, "Operation done successfully\n");
   }

   //bubble
      for(i=0;i<num_mtime;i++)
       {
         for(j=i+1;j<num_mtime;j++)
         {
            if(atoi(arr_mtime[i].rTime) > atoi(arr_mtime[j].rTime) )            
            {
               temp_mtime = arr_mtime[j];
               arr_mtime[j] = arr_mtime[i];
               arr_mtime[i] = temp_mtime;
            }
         }
      }
         
      for(i=0; i<LIMITCOUNT_mtime;i++){
      epochtimetolocaltime_mtime(arr_mtime[i].rTime, dateString_mtime);
      strcpy(arr_mtime[i].rTime, dateString_mtime);
      if( atoi(arr_mtime[i].rType)==1){
         strcpy(arr_mtime[i].rType,"IE");
      }
      }
      for(i=0; i<LIMITCOUNT_mtime ;i++){
         fprintf(fp,",\n        {'start': '%s',\n        'description': '%s',\n        'title': '%s',\n        }",arr_mtime[i].rTime,arr_mtime[i].rPath, arr_mtime[i].rType);
      }

      //match > limit LIMITCOUNT_mtime output
   num_mtime = 0;
   sprintf(buffer,"SELECT PATH,TIME, TYPE from Registry where TIME < %s ORDER BY TIME DESC LIMIT %d",newValues_mtime[3],LIMITCOUNT_mtime);

   rc = sqlite3_exec(db, buffer, callbackm, (void*)&callbackData, &zErrMsg);
   if( rc != SQLITE_OK ){
    //  fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
      //fprintf(stdout, "Operation done successfully\n");
   }

   sprintf(buffer,"SELECT FULLPATH, Sl_writeTm, TYPE from MFT where Sl_writeTm < %s ORDER BY Sl_writeTm DESC LIMIT %d",newValues_mtime[3],LIMITCOUNT_mtime);

   rc = sqlite3_exec(db, buffer, callbackm, (void*)&callbackData, &zErrMsg);
   if( rc != SQLITE_OK ){
   //   fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
      //fprintf(stdout, "Operation done successfully\n");
   }

   sprintf(buffer,"SELECT URL, TIME, TYPE from HISTORY where TIME < %s ORDER BY TIME DESC LIMIT %d",newValues_mtime[3],LIMITCOUNT_mtime);

   rc = sqlite3_exec(db, buffer, callbackm, (void*)&callbackData, &zErrMsg);
   if( rc != SQLITE_OK ){
     // fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
     // fprintf(stdout, "Operation done successfully\n");
   }

     sprintf(buffer,"SELECT URL, TIME, TYPE from DOWNLOAD where TIME < %s ORDER BY TIME DESC LIMIT %d",newValues_mtime[3],LIMITCOUNT_mtime);

   rc = sqlite3_exec(db, buffer, callbackm, (void*)&callbackData, &zErrMsg);
   if( rc != SQLITE_OK ){
    //  fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
     // fprintf(stdout, "Operation done successfully\n");
   }

   //bubble
      for(i=0;i<num_mtime;i++)
       {
         for(j=i+1;j<num_mtime;j++)
         {
            if(atoi(arr_mtime[i].rTime) < atoi(arr_mtime[j].rTime) )            
            {
               temp_mtime = arr_mtime[j];
               arr_mtime[j] = arr_mtime[i];
               arr_mtime[i] = temp_mtime;
            }
         }
      }
         
      for(i=0; i<LIMITCOUNT_mtime;i++){
      epochtimetolocaltime_mtime(arr_mtime[i].rTime, dateString_mtime);
      strcpy(arr_mtime[i].rTime, dateString_mtime);
      if( atoi(arr_mtime[i].rType)==1){
         strcpy(arr_mtime[i].rType,"IE");
      }
      }
      for(i=0; i<LIMITCOUNT_mtime ;i++){
         fprintf(fp,",\n        {'start': '%s',\n        'description': '%s',\n        'title': '%s',\n        }",arr_mtime[i].rTime,arr_mtime[i].rPath,arr_mtime[i].rType);
      }

   for(i=0; i<count_mtime; i++) {
      free(newValues_mtime[i]);
   }
   
    fprintf(fp,"\n");
   fprintf(fp,"]\n}");
   fprintf(fp,"\n;");
   fclose(fp);
 
   sqlite3_close(db);

    return 0;
}
