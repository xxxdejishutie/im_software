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
	TcpInet(Kernel *pker);//���캯��
	~TcpInet();//��������
public:
	list <HANDLE> m_listhead;//���߳�handle
	SOCKET m_tcpsocket_listen;//tcpsocket  ��������
	fd_set m_select;//�����¼���׽���
	//stu_udpsock m_stu_udp_sock;

	HANDLE m_iocp;//������Ϣ����ɶ˿�
	Kernel *m_kernel;//������������


	HANDLE m_messhan;
	SOCKET m_messock;
	sockaddr_in insteadaddr;//�����������ַ

public:
	bool InitNet();//��ʼ������
	bool SendData(SOCKET, char*, int nlen);
    bool Sendudpdate(stu_udpsock, char*, int nlen);
	//bool RecvData();
	static unsigned _stdcall tcpthreadproc(void*);
	static unsigned _stdcall ThreadProc2(void* lpvoid);
	static unsigned _stdcall messThreadProc(void* lpvoid);//��ʱ������������Լ�����Ϣ
	//static unsigned _stdcall udpthreadproc(void*);


public:
	bool throwrecv(stu_udpsock*);//Ͷ�ݽ�����Ϣ����
};

