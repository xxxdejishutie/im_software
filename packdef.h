#pragma once

#ifndef PACKDEF_H
#define PACKDEF_H
#include<list>
#include<string>
#include<array>
using namespace std;

#define _DEF_PROTOCOL_BASE   10
//注册
#define _DEF_PROTOCOL_REGISTER_RQ   _DEF_PROTOCOL_BASE +1
#define _DEF_PROTOCOL_REGISTER_RS   _DEF_PROTOCOL_BASE +2
//登录
#define _DEF_PROTOCOL_LOGIN_RQ   _DEF_PROTOCOL_BASE +3
#define _DEF_PROTOCOL_LOGIN_RS   _DEF_PROTOCOL_BASE +4
//获取好友列表
#define _DEF_PROTOCOL_GETFRIENDLIST_RQ   _DEF_PROTOCOL_BASE +5
#define _DEF_PROTOCOL_GETFRIENDLIST_RS   _DEF_PROTOCOL_BASE +6

//获取未接收消息列表
#define _DEF_PROTOCOL_GETUNRECVLIST_RQ	 _DEF_PROTOCOL_BASE +7
#define _DEF_PROTOCOL_GETUNRECVLIST_RS	 _DEF_PROTOCOL_BASE +8

//发送消息
#define _DEF_PROTOCOL_SENDMESSAGE_RQ	 _DEF_PROTOCOL_BASE +9
#define _DEF_PROTOCOL_SENDMESSAGE_RS	 _DEF_PROTOCOL_BASE +10

//获取未发送的消息列表
#define _DEF_PROTOCOL_GETNOTRECVMESS   _DEF_PROTOCOL_BASE + 11


//客户端下线
#define _FAILED_INET   _DEF_PROTOCOL_BASE + 12


//添加好友请求
#define _DEF_PROTOCOL_ADDFRIEND_RQ   _DEF_PROTOCOL_BASE + 13
//添加好友确认
#define _DEF_PROTOCOL_ADDFRIEND_ACK_RQ   _DEF_PROTOCOL_BASE + 14
//添加好友确认回复
#define _DEF_PROTOCOL_ADDFRIEND_ACK_RS   _DEF_PROTOCOL_BASE + 15

//添加好友请求回复
#define _DEF_PROTOCOL_ADDFRIEND_RS   _DEF_PROTOCOL_BASE + 16



//名字密码最大值
#define CHARMAX 100
#define MAXSIZE 512

//注册结果
#define REGISTER_SUCCESS 0
#define REGISTER_FAILED 1

//登录结果
#define REGISTER_SUCCESS 0
#define REGISTER_FAILED_TELNOEXIST 1
#define REGISTER_FAILED_PASSWRONG 2

//获取好友列表
#define GETFRIENDLIST_SUCCESS 0
#define GETFRIENDLIST_FAILED 1

//发送信息
#define SENDMESSAGE_SUCCESS 0
#define SENDMESSAGE_FAILED 1

//连接还是发送数据
#define M_ACCEPT 0
#define M_RECV 1

//添加好友结果
#define ADD_REFUSE 0
#define ADD_ACCEPT 1
#define ADD_UNEXIT 2

//注册请求
struct REGISTER_RQ
{
	int flags;
	long long tel;
	char name[CHARMAX];
	char password[CHARMAX];

};

//注册回复
struct REGISTER_RS
{
	int flags;
	int result;
	long long id;
};

//登录请求
struct LOGIN_RQ
{
	int flags;
	long long tel;
	char password[CHARMAX];
};

//登录回复
struct LOGIN_RS
{
	int flags;
	int result;
	long long id;
	int ack;
	char name[CHARMAX];
};

//获取好友列表
struct GETFRIENDLIST_RQ
{
	int flags;
	long long id;
};

//好友个人信息
struct FriendsList
{
	long long id;
	char name[CHARMAX];
	char nickname[CHARMAX];
};

//好友列表回复包
struct GETFRIENDLIST_RS
{
	int flags;
	int result;
	int acknum;
	//list<FriendsList> list_friends;
	array<FriendsList, 300> list_friends;
};

//发送信息
struct SENDMESSAGE_RQ
{
	int flags;
	int my_id;
	int fri_id;
	char szbuf[CHARMAX];
	int acknum;
};

//发送信息回复
struct SENDMESSAGE_RS
{
	int flags;

	int result;
	long long m_id;
	long long f_id;
	int acknum;
};



//获取未接收消息
struct GETNOTRECV
{
	int flags;
	long long m_id;
};


struct ADDFRIEND_RQ//添加好友请求
{
	int flags;
	int m_id;
	int f_id;
	long long f_tel;
	char nickname[CHARMAX];
};

struct ADDFRIEND_ACK_RQ//添加好友确定
{
	int flags;
	int m_id;
	int f_id;
	char nickname[CHARMAX];//没有需要赋值为空
};

struct ADDFRIEND_ACK_RS//添加好友确定回复
{
	int flags;
	int m_id;
	int f_id;
	int result;
	char f_nickname[CHARMAX];//没有需要赋值为空
	char m_nickname[CHARMAX];//没有需要赋值为空
};

struct ADDFRIEND_RS//添加好友请求回复
{
	int flags;
	int m_id;
	int f_id;
	int result;
	char f_name[CHARMAX];
};
struct getiprs//获取IP请求回复包
{
	//int flag;
	sockaddr_in addr;
};


struct nconnect//连接数量
{
	int flag;
	sockaddr_in addr;
	int ant;
};

struct severmess
{
	int bianhao;
	int flag;//标记是否本次被访问过
	int antcon;//连接数
	sockaddr_in ipaddr;//记录的对应服务器的IP地址和端口号

	bool operator <(severmess s)//优先队列，大顶堆，需要反过来
	{
		return this->antcon > s.antcon;
	}

};


#endif // PACKDEF_H
