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
	SOCKET tcpsock;//�����Ƿ�����
	SOCKET udpsock;//�շ������õ�udp
};

struct  stu_udpsock
{
	OVERLAPPED my_olp;//�����һ�������¼��Ľṹ�壬����д�ڵ�һ����
	//����ṹ��ĵ�ַ����mysocket���׵�ַ������ֵΪ����ṹ��
	//���Դ����������ݣ���������
	SOCKET udp_winter;//�׽��֣�����
	char buf[1024];//һҳ�ֽڴ�С��char���飬������
	int ntype;//��־�ǵ�һ�����ӣ����ǽ�����Ϣ
	sockaddr_in sockline;
	int nlen;
};


struct messque//��Ϣ�ṹ��
{
	long long acknum;//��Ϣ��ȷ�Ϻ�
	char cstr[1000];//��Ϣ����
};

struct addfrimess//��Ӻ�����Ϣ�ṹ��
{
	long long id;
	string nickname;
};

struct notsendmessage//δ������Ϣ�ṹ��
{
	long long nhasid;//�Ѿ�ȷ�ϵ�id
	list<addfrimess> addfriend;//��Ӻ�������
	map<long long, list<messque>> idtomess;//����id��������������Ϣ��ӳ��
};


class TcpKernel :public Kernel
{
public:
	CMySql* m_mysql;
	inet* m_inet;
	map<int, sockstruct> m_map;//�����Ƿ����ߣ�ͨ��id���Ҷ�Ӧsock
	map<int, SOCKET> m_map2;//����socket��Ӧ�ģ�id;
	map<int, SOCKET> m_map3;
	mutex m_mutex;
	string path;

	map<long long, notsendmessage> m_idtomess;//ͨ��id����ÿ���ͻ����Ѿ�ȷ�Ϲ���id�ţ����ѷ�����û�յ�����Ϣ
	
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

