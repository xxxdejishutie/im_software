#include "TcpInet.h"
#include<time.h>

string ProxyIp = "127.0.0.1";
int  ProxyPort = 8899;
string LocalIp = "127.0.0.1";
int LocalPort = 7899;

stu_udpsock m_stu_udp_sock;
TcpInet::TcpInet(Kernel * pker)//���캯��
{
	m_tcpsocket_listen = 0;//tcpsocket  ��������
	FD_ZERO(&m_select);//�����¼���׽���
	m_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);//������Ϣ����ɶ˿�
	m_kernel = pker;//������������

    RunFlag = true;
}
TcpInet::~TcpInet()//��������
{

}

bool TcpInet::throwrecv(stu_udpsock* psock)
{
    psock->ntype = M_RECV;
    WSABUF we;//һ���ṹ��
    we.buf = psock->buf;
    we.len = sizeof(psock->buf);
    DWORD lpNumberOfBytesRecvd;//ʵ�ʽ��յ��ĸ���
    DWORD qflog = false;
    psock->nlen = sizeof(psock->sockline);
    if (WSARecvFrom(psock->udp_winter,//winter�׽���
        &we,//һ���ṹ�壬���������ͻ���������
        1,//�ṹ�����
        &lpNumberOfBytesRecvd,//ʵ�ʽ��յ��ĸ���
        &qflog,//�Ƿ�����޸�recv��Ϊ��һ�㲻�޸�
        (sockaddr*) & psock->sockline,
        &psock->nlen,
        &psock->my_olp,//���¼��ṹ�巢����Ϣ
        0))//������̣����ﲻ�ܷ�һ��0
    {//����0�ɹ�������ʧ��
        if (WSAGetLastError() != WSA_IO_PENDING)//��������WSA_IO_PENDING˵���ɹ��ˣ�
            //���ǻ�Ҫ��һ�ᣬ����ʧ��
        {
            return false;
        }
    }
    return true;
}

bool TcpInet::InitNet()//��ʼ������
{
    //�����
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


    //���������׽���
    m_tcpsocket_listen = socket(AF_INET, SOCK_STREAM, 0);
   /* int aaa;
    cout << "������˿ں�";
    cin >> aaa;*/
    sockaddr_in sockline;
    sockline.sin_port = htons(LocalPort);
    sockline.sin_family = AF_INET;
    //socklineLoad.sin_addr.S_un.S_addr = "127.0.0.1";
    wchar_t* wc = new wchar_t[LocalIp.size()];
    swprintf(wc, 100, L"%S", LocalIp.c_str());
    InetPtonW(AF_INET, wc, &sockline.sin_addr);

    if (bind(m_tcpsocket_listen, (const sockaddr*)&sockline, sizeof(sockline)))//�ɹ�������
    {
        perror("�󶨴���");
        WSACleanup();
        return 0;
    }

    if (listen(m_tcpsocket_listen, 10))//����
    {
        perror("����ʧ��");
        return 0;
    }

    FD_SET(m_tcpsocket_listen, &m_select);//�Ѽ����׽��ֽ���slect

    _beginthreadex(0, 0, &tcpthreadproc, this, 0, 0);//����tcp���߳�


    


    //*******************************************************************************
    //udp������Ϣ

    
        //stu_udpsock* udpsock = new stu_udpsock;
        m_stu_udp_sock.udp_winter = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_IP, NULL, 0, WSA_FLAG_OVERLAPPED);//���������׽���
        memset(&(m_stu_udp_sock.my_olp), 0, sizeof(OVERLAPPED));
        //sockline.sin_port = htons(8899);
        int kl = bind(m_stu_udp_sock.udp_winter, (const sockaddr*)&sockline, sizeof(sockline));
        if (kl != 0)
        {
            perror("udp  bind  faild");
            return 0;
        }

        //5.31�ģ�ȡ����ɶ˿�
        /*throwrecv(udpsock);
        m_iocp = CreateIoCompletionPort((HANDLE)udpsock->udp_winter,
            m_iocp, (ULONG_PTR)udpsock->udp_winter, 0);*/
    
    //������ɶ˿ڹ���
    
    //recvfrom(udpsock.udp_winter, udpsock.buf, sizeof(udpsock.buf), 0,0,0);
   
    //for (int i = 0; i < 10; i++)
    //{
        //���̵߳ȴ�
       HANDLE ptheadhandle =  (HANDLE)_beginthreadex(0, 0, &ThreadProc2, this, 0, 0);

       //this->m_listhead.push_back(ptheadhandle);
    //}
    


    //���Ӵ��������
    m_messhan = (HANDLE)_beginthreadex(0, 0, &messThreadProc, this, 0, 0);








    return 1;


}


unsigned _stdcall TcpInet::ThreadProc2(void* lpvoid)
{
    TcpInet* pthis = (TcpInet*)lpvoid;
    DWORD dwNumberOfBytesTransferred;
    SOCKET sock;
    stu_udpsock* pSocket;
    BOOL bFlag;
    //���Ч�� �����̳߳صĺ���ȳ�ԭ�� �߳�1���������һ�����1��
    while (1)
    {
        //�۲�IOCP״̬
        //bFlag = GetQueuedCompletionStatus(pthis->m_iocp,
        //    &dwNumberOfBytesTransferred,
        //    (PULONG_PTR)&sock,
        //    (LPOVERLAPPED*)&pSocket,
        //    INFINITE);//�ȴ����¼�����

        ////У�����
        //if (!sock || !pSocket)
        //    continue;
        //
        char buff[2000];
        int len = sizeof(SOCKADDR);
        recvfrom(m_stu_udp_sock.udp_winter, buff, 2000, 0, (SOCKADDR*)&(m_stu_udp_sock.sockline), &len);
        pthis->m_kernel->dealudpTask(buff,m_stu_udp_sock);
        //pthis->throwrecv(pSocket);

        //
        ////cout<<"���յ���Ϣ�ģ�udp�߳�Ϊ " << GetCurrentThreadId() << endl;
        ////cout << "���յ�����ϢΪ" <<  pSocket->buf << " ";
        //Sleep(100);

       
    }
    return 0;

}



unsigned _stdcall  TcpInet::tcpthreadproc(void* lpvoid)//����TCP���߳�
{
    TcpInet* pthis = (TcpInet*)lpvoid;
    timeval tv;//select�ȴ���ʱ��
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
                            int fla = _FAILED_INET;//���߽���Ǹ�Ϊ����
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

    sockaddr_in socklineLoad;
    socklineLoad.sin_port = htons(ProxyPort);
    socklineLoad.sin_family = AF_INET;
    //socklineLoad.sin_addr.S_un.S_addr = "127.0.0.1";
    wchar_t* wc = new wchar_t[ProxyIp.size()];
    swprintf(wc, 100, L"%S", ProxyIp.c_str());
    InetPtonW(AF_INET, wc, &socklineLoad.sin_addr);
    //InetPtonW(AF_INET, L"127.0.0.1", &socklineLoad.sin_addr);
    while (pthis->RunFlag)
    {
        stringstream ss;
        ss << "0*" << pthis->m_kernel->getconnectant();
        sendto(m_stu_udp_sock.udp_winter, ss.str().c_str(), ss.str().length(), 0, (const sockaddr*)&socklineLoad, sizeof(socklineLoad));
        Sleep(4000);
        cout << "send to load cnt message" << ss.str() << endl;
    }
    return 0;
}


bool TcpInet::SendData(SOCKET sockwinter, char* str, int nlen)//���ͺ���
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
