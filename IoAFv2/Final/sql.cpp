#include "sql.h"

int reg2sql(REGQUEUE *q)
{
    //database 파일 생성코드
    sqlite3 *db = NULL; 
    sqlite3_stmt *stmt = NULL; //sqlite3 statement 
    char *sql; //쿼리문 들어가는 것
    int rc; //sqlite3 함수를 사용해서 나온 값을 저장하는 부분인데. 그냥 실행시킬때 받는 변수라 생각하면 될듯.
    unsigned int i; //for 문 돌릴 때 변수
    char *buffer = (char *)malloc(500);  //sql로 해도 되지만 insert부분의 쿼리를 넣기 위한 변수로 새로 만들었음
    char* errorMsg = NULL; //error 생겼을 때 출력 위한 변수

    memset(buffer, 0x00, sizeof(char)*500); //buffer부분의 메모리 초기화

	//여기서부터 테이블 생성 전까지는 db파일 생성부분
    if(sqlite3_open("info.db", &db) != SQLITE_OK)
    {
        fprintf(stderr, "DB접근이 어렵습니다. (오류 %s)\n", sqlite3_errmsg(db));
    }

    //MFT 테이블 생성
    sql = "CREATE TABLE IF NOT EXISTS Registry (PATH TEXT ,TIME INTEGER, TYPE TEXT);";
    if( sqlite3_exec(db, sql, NULL, NULL, NULL) == SQLITE_OK) { //멘토님께서 디버깅 편하시려고 수정해주신 부분. 연결되면 콘솔에 succeeded 출력됨.
        //fprintf(stderr, ">> SQLite Table creation Succeeded!\n");
    } else {
       // puts("테이블 생성에 실패했습니다.");
        exit(1);
    }

    //데이터 추가 코드.
    rc = sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, &errorMsg); //insert 속도 개선 위해 사용한 코드. insert문 끝날 때 끝내는 문장도 있음.
    //fprintf(stderr, " Commit begin result : %s\n", errorMsg); //디버깅 편하게 하기 위해 콘솔에 출력해주는 부분. 나중에는 지워도 될듯
	sprintf (buffer,"INSERT INTO Registry(PATH, TIME, TYPE) VALUES ( ?1, ?2, \"Registry\")");
  //buffer라는 변수에 insert 쿼리문을 넣는 것. 쿼리문에 변수에 맞게 넣고. values에 넣으면됨. 1,2,3,4,5,6~~ 숫자는 아래 sqlite3_bind_text stmt, 숫자부분을 바꿔주면 됨. 숫자다음은 저장되있는 변수값들)
    if(sqlite3_prepare_v2(db, buffer, strlen(buffer), &stmt, NULL) == SQLITE_OK) //디버깅 확인 위한 if문
    {
      //  puts(">> Prepared Statement is ready : Succeeded!\n");
    }
    else
    {
        puts("테이블 값 입력에 실패하였습니다.");
    }

	int size = q->size();

	 while (!q->empty())
    {
		sqlite3_bind_text(stmt, 1, q->front()->key, strlen(q->front()->key), SQLITE_STATIC);
		sqlite3_bind_int(stmt, 2, q->front()->time);
        q->pop();
		if ( sqlite3_step(stmt) != SQLITE_DONE )  {
        }
        sqlite3_reset(stmt); 
    }
    rc = sqlite3_exec(db, "COMMIT TRANSACTION;", NULL, NULL, &errorMsg);//위에 insert문 속도빠르게 하기위한거 끝마치는것.
 //   fprintf(stderr, " Commit result : %s\n", errorMsg);//디버깅편하기 위해 콘솔에 끝났다고 표시
    sqlite3_finalize(stmt);//sqlite3작동 끝내는 함수.

    sqlite3_close(db);//db 닫음.
    return 0;
 }