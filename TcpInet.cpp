#include "TcpInet.h"
#include<time.h>

stu_udpsock m_stu_udp_sock;
TcpInet::TcpInet(Kernel * pker)//构造函数
{
	m_tcpsocket_listen = 0;//tcpsocket  管理连接
	FD_ZERO(&m_select);//处理登录的套接字
	m_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);//处理消息的完成端口
	m_kernel = pker;//处理到来的任务
}
TcpInet::~TcpInet()//析构函数
{

}

bool TcpInet::throwrecv(stu_udpsock* psock)
{
    psock->ntype = M_RECV;
    WSABUF we;//一个结构体
    we.buf = psock->buf;
    we.len = sizeof(psock->buf);
    DWORD lpNumberOfBytesRecvd;//实际接收到的个数
    DWORD qflog = false;
    psock->nlen = sizeof(psock->sockline);
    if (WSARecvFrom(psock->udp_winter,//winter套接字
        &we,//一个结构体，缓冲区，和缓冲区长度
        1,//结构体个数
        &lpNumberOfBytesRecvd,//实际接收到的个数
        &qflog,//是否可以修改recv行为，一般不修改
        (sockaddr*) & psock->sockline,
        &psock->nlen,
        &psock->my_olp,//用事件结构体发送消息
        0))//完成历程，这里不管放一个0
    {//返回0成功，非零失败
        if (WSAGetLastError() != WSA_IO_PENDING)//错误码是WSA_IO_PENDING说明成功了，
            //但是还要等一会，不算失败
        {
            return false;
        }
    }
    return true;
}

bool TcpInet::InitNet()//初始化网络
{
    //导入库
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;


    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {

        return false;
    }

    if (LOBYTE(wsaData.wVersion) != 2 ||
        HIBYTE(wsaData.wVersion) != 2)
    {
        //UnInitNetWork();
        return false;
    }


    //创建监听套接字
    m_tcpsocket_listen = socket(AF_INET, SOCK_STREAM, 0);
   /* int aaa;
    cout << "请输入端口号";
    cin >> aaa;*/
    sockaddr_in sockline;
    sockline.sin_port = htons(8899);
    sockline.sin_family = AF_INET;
    sockline.sin_addr.S_un.S_addr = 0;

    if (bind(m_tcpsocket_listen, (const sockaddr*)&sockline, sizeof(sockline)))//成功返回零
    {
        perror("绑定错误");
        WSACleanup();
        return 0;
    }

    if (listen(m_tcpsocket_listen, 10))//监听
    {
        perror("监听失败");
        return 0;
    }

    FD_SET(m_tcpsocket_listen, &m_select);//把监听套接字交给slect

    _beginthreadex(0, 0, &tcpthreadproc, this, 0, 0);//处理tcp的线程


    


    //*******************************************************************************
    //udp发送消息

    
        //stu_udpsock* udpsock = new stu_udpsock;
        m_stu_udp_sock.udp_winter = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);//创建监听套接字
        memset(&(m_stu_udp_sock.my_olp), 0, sizeof(OVERLAPPED));
        sockline.sin_port = htons(8899);
        int kl = bind(m_stu_udp_sock.udp_winter, (const sockaddr*)&sockline, sizeof(sockline));
        if (kl != 0)
        {
            perror("udp  bind  faild");
            return 0;
        }

        //5.31改，取消完成端口
        /*throwrecv(udpsock);
        m_iocp = CreateIoCompletionPort((HANDLE)udpsock->udp_winter,
            m_iocp, (ULONG_PTR)udpsock->udp_winter, 0);*/
    
    //交给完成端口管理
    
    //recvfrom(udpsock.udp_winter, udpsock.buf, sizeof(udpsock.buf), 0,0,0);
   
    //for (int i = 0; i < 10; i++)
    //{
        //多线程等待
       HANDLE ptheadhandle =  (HANDLE)_beginthreadex(0, 0, &ThreadProc2, this, 0, 0);

       //this->m_listhead.push_back(ptheadhandle);
    //}
    


    //连接代理服务器
    //m_messhan = (HANDLE)_beginthreadex(0, 0, &messThreadProc, this, 0, 0);








    return 1;


}


unsigned _stdcall TcpInet::ThreadProc2(void* lpvoid)
{
    TcpInet* pthis = (TcpInet*)lpvoid;
    DWORD dwNumberOfBytesTransferred;
    SOCKET sock;
    stu_udpsock* pSocket;
    BOOL bFlag;
    //提高效率 采用线程池的后进先出原则 线程1干完活又来一个活还让1干
    while (1)
    {
        //观察IOCP状态
        //bFlag = GetQueuedCompletionStatus(pthis->m_iocp,
        //    &dwNumberOfBytesTransferred,
        //    (PULONG_PTR)&sock,
        //    (LPOVERLAPPED*)&pSocket,
        //    INFINITE);//等待有事件发生

        ////校验参数
        //if (!sock || !pSocket)
        //    continue;
        //
        char buff[2000];
        int len = sizeof(SOCKADDR);
        recvfrom(m_stu_udp_sock.udp_winter, buff, 2000, 0, (SOCKADDR*)&(m_stu_udp_sock.sockline), &len);
        pthis->m_kernel->dealudpTask(buff,m_stu_udp_sock);
        //pthis->throwrecv(pSocket);

        //
        ////cout<<"接收到消息的，udp线程为 " << GetCurrentThreadId() << endl;
        ////cout << "接收到的消息为" <<  pSocket->buf << " ";
        //Sleep(100);

       
    }
    return 0;

}



unsigned _stdcall  TcpInet::tcpthreadproc(void* lpvoid)//处理TCP的线程
{
    TcpInet* pthis = (TcpInet*)lpvoid;
    timeval tv;//select等待的时间
    while (1)
    {
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        fd_set read_set = pthis->m_select;
        int num = select(0, &read_set, 0, 0, &tv);
        if (num)
        {
            if (FD_ISSET(pthis->m_tcpsocket_listen, &read_set))
            {
                SOCKET wockwinter = accept(pthis->m_tcpsocket_listen, NULL, 0);
                FD_SET(wockwinter, &pthis->m_select);
            }
            else
            {
                for (int i = 0; i < (int)pthis->m_select.fd_count; i++)
                {
                    if (FD_ISSET(pthis->m_select.fd_array[i], &read_set))
                    {
                        int a = 0;
                        char str[1024];
                        int recvnum = recv(pthis->m_select.fd_array[i], (char*)&a, 4, 0);
                        //send(pthis->m_select.fd_array[i], (char*)&a,4, 0);
                        //cout << a << endl;
                        if (recvnum > 0)
                        {
                            int cnt = recv(pthis->m_select.fd_array[i], str, a, 0);
                            cout << str << endl;

                            pthis->m_kernel->dealTask(str, pthis->m_select.fd_array[i]);
                        }
                        else
                        {
                            int fla = _FAILED_INET;//下线将标记改为下线
                            pthis->m_kernel->dealTask((char*)&fla, pthis->m_select.fd_array[i]);
                            FD_CLR(pthis->m_select.fd_array[i], &pthis->m_select);
                            
                           
                        }
                    }
                }
            }
        }
    }


    


}


unsigned _stdcall TcpInet::messThreadProc(void* lpvoid)
{
    TcpInet* pthis = (TcpInet*)lpvoid;
    pthis->m_messock = socket(AF_INET, SOCK_DGRAM, 0);
    //severmess ss;
    //ss.bianhao = -1;
    //ss.antcon = pthis->m_kernel->getconnectant();

    //sockaddr_in sockline;
    //sockline.sin_port = htons(8899);
    //sockline.sin_family = AF_INET;
    //sockline.sin_addr.S_un.S_addr = 0;

    //ss.ipaddr = sockline;

    ////cout << "请输入代理服务器IP地址，如127.0.0.1 " << endl;
    //    char str[100] = "127.0.0.1";
    //    //cin >> str;
    //    //cout << "请输入代理服务器端口号，如 8899" << endl;
    //    int duan = 8899;
    //    //cin >> duan;
    //    sockaddr_in sockline2;
    //    sockline2.sin_port = htons(duan);
    //    sockline2.sin_family = AF_INET;
    //    //inet_pton(AF_INET,str,&sockline2.sin_addr);
    //    InetPtonW(AF_INET, (const wchar_t*)str, &sockline2.sin_addr);
    //    
    //    pthis->insteadaddr = sockline2;
    //    
    //    while (1)
    //    {
    //        sendto(pthis->m_messock, (const char*)&ss, sizeof(ss), 0, (const sockaddr*)&sockline2, sizeof(sockline2));
    //        int num = recvfrom(pthis->m_messock, (char*)&ss.bianhao, sizeof(ss), 0, 0, 0);
    //        if (num > 0)
    //            break;
    //    }
    //    
    //while (1)
    //{
    //    ss.antcon = 
    //    sendto(pthis->m_messock, (const char*)&ss, sizeof(ss), 0, (const sockaddr*)&sockline2, sizeof(sockline2));
    //    Sleep(1);
    //}
    severmess ss;
    ss.bianhao = -1;
    ss.antcon = pthis->m_kernel->getconnectant();
    sockaddr_in sockline2;
    sockline2.sin_port = htons(8899);
    sockline2.sin_family = AF_INET;
    //inet_pton(AF_INET,str,&sockline2.sin_addr);
    InetPtonW(AF_INET, L"127.0.0.1", &sockline2.sin_addr);
    int num = sendto(pthis->m_messock, (const char*)&ss, sizeof(ss), 0, (const sockaddr*)&sockline2, sizeof(sockline2));
    //测试发送字节
    //cout << num << endl;

    int k = recvfrom(pthis->m_messock, (char*)&ss.bianhao, sizeof(ss.bianhao), 0, 0, 0);
    cout << ss.bianhao << endl;
    while (1)
    {
        ss.antcon = pthis->m_kernel->getconnectant();
        int num = sendto(pthis->m_messock, (const char*)&ss, sizeof(ss), 0, (const sockaddr*)&sockline2, sizeof(sockline2));
        cout << num << endl;
        Sleep(1000);
    }

    
}


bool TcpInet::SendData(SOCKET sockwinter, char* str, int nlen)//发送函数
{

    send(sockwinter, (char*)&nlen, sizeof(nlen), 0);

    if (send(sockwinter, str, nlen, 0))
    {
        return 1;
    }
    return 0;
}

bool TcpInet::Sendudpdate(stu_udpsock stu, char* str,int nlen)
{
    sendto(stu.udp_winter, str, nlen, 0, (const sockaddr*)&stu.sockline, sizeof(stu.sockline));
    return 1;
}
