#pragma once
#include"Kernel.h"

#include<WS2tcpip.h>
#include<time.h>



class inet
{
public:
	virtual ~inet()
	{}
	bool virtual InitNet() = 0;
	bool virtual SendData(SOCKET, char*, int nlen) = 0;
	bool virtual Sendudpdate(stu_udpsock, char*, int nlen) = 0;
	bool virtual throwrecv(stu_udpsock*) = 0;
};

