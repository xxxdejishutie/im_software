#pragma once

#ifndef PACKDEF_H
#define PACKDEF_H
#include<list>
#include<string>
#include<array>
using namespace std;

#define _DEF_PROTOCOL_BASE   10
//ע��
#define _DEF_PROTOCOL_REGISTER_RQ   _DEF_PROTOCOL_BASE +1
#define _DEF_PROTOCOL_REGISTER_RS   _DEF_PROTOCOL_BASE +2
//��¼
#define _DEF_PROTOCOL_LOGIN_RQ   _DEF_PROTOCOL_BASE +3
#define _DEF_PROTOCOL_LOGIN_RS   _DEF_PROTOCOL_BASE +4
//��ȡ�����б�
#define _DEF_PROTOCOL_GETFRIENDLIST_RQ   _DEF_PROTOCOL_BASE +5
#define _DEF_PROTOCOL_GETFRIENDLIST_RS   _DEF_PROTOCOL_BASE +6

//��ȡδ������Ϣ�б�
#define _DEF_PROTOCOL_GETUNRECVLIST_RQ	 _DEF_PROTOCOL_BASE +7
#define _DEF_PROTOCOL_GETUNRECVLIST_RS	 _DEF_PROTOCOL_BASE +8

//������Ϣ
#define _DEF_PROTOCOL_SENDMESSAGE_RQ	 _DEF_PROTOCOL_BASE +9
#define _DEF_PROTOCOL_SENDMESSAGE_RS	 _DEF_PROTOCOL_BASE +10

//��ȡδ���͵���Ϣ�б�
#define _DEF_PROTOCOL_GETNOTRECVMESS   _DEF_PROTOCOL_BASE + 11


//�ͻ�������
#define _FAILED_INET   _DEF_PROTOCOL_BASE + 12


//��Ӻ�������
#define _DEF_PROTOCOL_ADDFRIEND_RQ   _DEF_PROTOCOL_BASE + 13
//��Ӻ���ȷ��
#define _DEF_PROTOCOL_ADDFRIEND_ACK_RQ   _DEF_PROTOCOL_BASE + 14
//��Ӻ���ȷ�ϻظ�
#define _DEF_PROTOCOL_ADDFRIEND_ACK_RS   _DEF_PROTOCOL_BASE + 15

//��Ӻ�������ظ�
#define _DEF_PROTOCOL_ADDFRIEND_RS   _DEF_PROTOCOL_BASE + 16



//�����������ֵ
#define CHARMAX 100
#define MAXSIZE 512

//ע����
#define REGISTER_SUCCESS 0
#define REGISTER_FAILED 1

//��¼���
#define REGISTER_SUCCESS 0
#define REGISTER_FAILED_TELNOEXIST 1
#define REGISTER_FAILED_PASSWRONG 2

//��ȡ�����б�
#define GETFRIENDLIST_SUCCESS 0
#define GETFRIENDLIST_FAILED 1

//������Ϣ
#define SENDMESSAGE_SUCCESS 0
#define SENDMESSAGE_FAILED 1

//���ӻ��Ƿ�������
#define M_ACCEPT 0
#define M_RECV 1

//��Ӻ��ѽ��
#define ADD_REFUSE 0
#define ADD_ACCEPT 1
#define ADD_UNEXIT 2

//ע������
struct REGISTER_RQ
{
	int flags;
	long long tel;
	char name[CHARMAX];
	char password[CHARMAX];

};

//ע��ظ�
struct REGISTER_RS
{
	int flags;
	int result;
	long long id;
};

//��¼����
struct LOGIN_RQ
{
	int flags;
	long long tel;
	char password[CHARMAX];
};

//��¼�ظ�
struct LOGIN_RS
{
	int flags;
	int result;
	long long id;
	int ack;
	char name[CHARMAX];
};

//��ȡ�����б�
struct GETFRIENDLIST_RQ
{
	int flags;
	long long id;
};

//���Ѹ�����Ϣ
struct FriendsList
{
	long long id;
	char name[CHARMAX];
	char nickname[CHARMAX];
};

//�����б�ظ���
struct GETFRIENDLIST_RS
{
	int flags;
	int result;
	int acknum;
	//list<FriendsList> list_friends;
	array<FriendsList, 300> list_friends;
};

//������Ϣ
struct SENDMESSAGE_RQ
{
	int flags;
	int my_id;
	int fri_id;
	char szbuf[CHARMAX];
	int acknum;
};

//������Ϣ�ظ�
struct SENDMESSAGE_RS
{
	int flags;

	int result;
	long long m_id;
	long long f_id;
	int acknum;
};



//��ȡδ������Ϣ
struct GETNOTRECV
{
	int flags;
	long long m_id;
};


struct ADDFRIEND_RQ//��Ӻ�������
{
	int flags;
	int m_id;
	int f_id;
	long long f_tel;
	char nickname[CHARMAX];
};

struct ADDFRIEND_ACK_RQ//��Ӻ���ȷ��
{
	int flags;
	int m_id;
	int f_id;
	char nickname[CHARMAX];//û����Ҫ��ֵΪ��
};

struct ADDFRIEND_ACK_RS//��Ӻ���ȷ���ظ�
{
	int flags;
	int m_id;
	int f_id;
	int result;
	char f_nickname[CHARMAX];//û����Ҫ��ֵΪ��
	char m_nickname[CHARMAX];//û����Ҫ��ֵΪ��
};

struct ADDFRIEND_RS//��Ӻ�������ظ�
{
	int flags;
	int m_id;
	int f_id;
	int result;
	char f_name[CHARMAX];
};
struct getiprs//��ȡIP����ظ���
{
	//int flag;
	sockaddr_in addr;
};


struct nconnect//��������
{
	int flag;
	sockaddr_in addr;
	int ant;
};

struct severmess
{
	int bianhao;
	int flag;//����Ƿ񱾴α����ʹ�
	int antcon;//������
	sockaddr_in ipaddr;//��¼�Ķ�Ӧ��������IP��ַ�Ͷ˿ں�

	bool operator <(severmess s)//���ȶ��У��󶥶ѣ���Ҫ������
	{
		return this->antcon > s.antcon;
	}

};


#endif // PACKDEF_H
