#pragma once
#include <winsock2.h>
#pragma comment(lib,"Ws2_32")
#include<mutex>
struct  stu_udpsock;
class Kernel
{
public:
	bool virtual initKernel() = 0;
	void virtual dealTask(char*,SOCKET) = 0;
	void virtual dealudpTask(char*, stu_udpsock) = 0;
	int virtual getconnectant() = 0;
};

