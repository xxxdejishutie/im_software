#pragma once
#include<iostream>
#include<sstream>
#include<vector>
#include "Kernel.h"
#include"TcpInet.h"
#include"CMySql.h"
#include"packdef.h"

using namespace std;

struct sockstruct
{
	SOCKET tcpsock;//检验是否在线
	SOCKET udpsock;//收发数据用的udp
};

struct  stu_udpsock
{
	OVERLAPPED my_olp;//这个是一个包含事件的结构体，必须写在第一个，
	//这个结构体的地址，是mysocket的首地址，返回值为这个结构体
	//可以带回其他数据，后面会解释
	SOCKET udp_winter;//套接字，连接
	char buf[1024];//一页字节大小的char数组，缓冲区
	int ntype;//标志是第一次连接，还是接收消息
	sockaddr_in sockline;
	int nlen;
};


struct messque//消息结构体
{
	long long acknum;//消息的确认号
	char cstr[1000];//消息内容
};

struct addfrimess//添加好友信息结构体
{
	long long id;
	string nickname;
};

struct notsendmessage//未发送信息结构体
{
	long long nhasid;//已经确认的id
	list<addfrimess> addfriend;//添加好友请求
	map<long long, list<messque>> idtomess;//好友id与他发过来的信息的映射
};


class TcpKernel :public Kernel
{
public:
	CMySql* m_mysql;
	inet* m_inet;
	map<int, sockstruct> m_map;//检验是否在线，通过id查找对应sock
	map<int, SOCKET> m_map2;//查找socket对应的，id;
	map<int, SOCKET> m_map3;
	mutex m_mutex;
	string path;

	map<long long, notsendmessage> m_idtomess;//通过id保存每个客户端已经确认过的id号，好友发来还没收到的信息
	
public:
	bool initKernel();
	void dealTask(char*,SOCKET);
	void dealudpTask(char*, stu_udpsock);

public:
	void register_rq(char*,SOCKET);
	void login_rq(char*, SOCKET);
	void getfriendlist_rq(char*, SOCKET);
	void addfriend_rq(char*, SOCKET);
	void addfriend_ack_rs(char*, SOCKET);


	void getnotrecv(char*, stu_udpsock);
	void sendmessage_rq(char*, stu_udpsock);
	void sendmessage_rs(char*, stu_udpsock);
	
	int getconnectant()
	{
		return m_map.size();
	}
};

