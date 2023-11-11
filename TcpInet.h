#pragma once
#include <winsock2.h>
#include <windows.h>
#include <process.h>
#include <list>
#include <map>
#include<fstream>
#include "inet.h"
#include"TcpKernel.h"
#pragma comment(lib,"Ws2_32")
using namespace std;
extern struct  stu_udpsock;


class TcpInet :public inet
{
public:
	TcpInet(Kernel *pker);//构造函数
	~TcpInet();//析构函数
public:
	list <HANDLE> m_listhead;//多线程handle
	SOCKET m_tcpsocket_listen;//tcpsocket  管理连接
	fd_set m_select;//处理登录的套接字
	//stu_udpsock m_stu_udp_sock;

	HANDLE m_iocp;//处理消息的完成端口
	Kernel *m_kernel;//处理到来的任务


	HANDLE m_messhan;
	SOCKET m_messock;
	sockaddr_in insteadaddr;//代理服务器地址

public:
	bool InitNet();//初始化网络
	bool SendData(SOCKET, char*, int nlen);
    bool Sendudpdate(stu_udpsock, char*, int nlen);
	//bool RecvData();
	static unsigned _stdcall tcpthreadproc(void*);
	static unsigned _stdcall ThreadProc2(void* lpvoid);
	static unsigned _stdcall messThreadProc(void* lpvoid);//定时向服务器发送自己的信息
	//static unsigned _stdcall udpthreadproc(void*);


public:
	bool throwrecv(stu_udpsock*);//投递接收消息请求
};

