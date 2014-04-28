#include<stdio.h>
#include<queue>
#include<tchar.h>
using namespace std;
#ifndef __MATCH_H_
#define __MATCH_H_
struct info_file{
	char path[0x1000];
	char name[0x1000];
	long long SI_Mtime;
	long long SI_Atime;
	long long SI_Ctime;
	long long FN_Mtime;
	long long FN_Atime;
	long long FN_Ctime;
};

struct info_Registry{
	char key[0x1000];
	long long time;
};

struct match_file{
	int action;
	int tool;
	char path[0x1000];
	long long SI_Mtime;
	long long SI_Atime;
	long long SI_Ctime;
	long long FN_Mtime;
	long long FN_Atime;
	long long FN_Ctime;
};

struct match_registry{
	int action;
	int tool;
	char key[0x1000];
	long long time;
};

struct sig_file{
	int action;
	int tool;
	char path[0x1000];
	char file[0x1000];
};

struct sig_registry{
	int action;
	int tool;
	char key[0x1000];
};
struct USB
{
	char PATH[0x1000];
	long long time;
};


typedef queue<match_file*> MATCHFILEQ;
typedef queue<match_registry*> MATCHREGQ;
typedef queue<sig_file*> SIGFILEQ;
typedef queue<sig_registry*> SIGREGQ;
typedef queue<USB*> USBq;

#endif

void get_signature_REG(SIGREGQ * q);
void get_signature_FILE(SIGFILEQ * q);
void matching_REG(SIGREGQ *q, MATCHREGQ *mq);
void matching_FILE(SIGFILEQ *q, MATCHFILEQ *mq);
void Deep_matching_REG(SIGREGQ *q, MATCHREGQ *mq);
void Deep_matching_FILE(SIGFILEQ *q, MATCHFILEQ *mq);
void REG2SQL(MATCHREGQ * mq);
void FILE2SQL(MATCHFILEQ * mq);
void tool_info();
void get_USB(USBq * q);
void USBG2SQL(USBq * q);