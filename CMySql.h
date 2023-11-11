#pragma once
#include"C:\\Program Files\\MySQL\\MySQL Server 5.7\\include\\mysql.h"
//#include <WinSock2.h>
//#pragma comment(lib,"E:\\lianxichengxu\\wangpan4.23\\网盘\\libmysql.lib")
#pragma comment(lib,"..\\即时通信软件\\libmysql.lib")
//
#include <list>
#include <string>
using namespace std;

class CMySql
{
public:
    CMySql(void);
    ~CMySql(void);
public:
    bool  ConnectMySql(const char *host,const char *user,const char *pass,const char *db);
    void  DisConnect();
    bool  SelectMySql(const char* szSql,int nColumn,list<string>& lstStr);
   
    //更新：删除、插入、修改
    bool  UpdateMySql(const char* szSql);
 
private:
    MYSQL *sock;   
	MYSQL_RES *results;   
	MYSQL_ROW record; 
   
};

