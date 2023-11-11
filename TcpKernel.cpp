#include "TcpKernel.h"
bool TcpKernel::initKernel()
{
	m_mysql = new CMySql;
	if (!m_mysql->ConnectMySql("localhost", "root", "151430asd", "wechat"))
	{
		cout << "mysql connect faild" << endl;
	}

	m_inet = new TcpInet(this);
	m_inet->InitNet();

	path = "E:/memory/";
	
	return 1;
}

void  TcpKernel::dealTask(char* cstr, SOCKET sock)
{
	switch (*(int*)cstr)
	{
	case _DEF_PROTOCOL_REGISTER_RQ:
		register_rq(cstr,sock);
		break;
	case _DEF_PROTOCOL_LOGIN_RQ:
		login_rq(cstr,sock);
		break;
	case _DEF_PROTOCOL_GETFRIENDLIST_RQ:
		getfriendlist_rq(cstr, sock);
		break;
	case _DEF_PROTOCOL_ADDFRIEND_RQ:
		addfriend_rq(cstr, sock);
		break;
	case _DEF_PROTOCOL_ADDFRIEND_ACK_RS:
		addfriend_ack_rs(cstr, sock);
		break;
	case _FAILED_INET:
	{
		int a = (int)m_map2[(int)sock];
		m_map2.erase((int)sock);
		m_map.erase(a);
		break;
	}
	default:
		break;
	}


}

void TcpKernel::dealudpTask(char* cstr, stu_udpsock stuck_udpsock)//发送消息
{
	
	if (*(int*)cstr == _DEF_PROTOCOL_SENDMESSAGE_RQ)
	{
		sendmessage_rq(cstr,stuck_udpsock);
	}
	else if (*(int*)cstr == _DEF_PROTOCOL_SENDMESSAGE_RS)
	{
		sendmessage_rs(cstr, stuck_udpsock);
	}
	else if(*(int*)cstr == _DEF_PROTOCOL_GETNOTRECVMESS)
	{
		getnotrecv(cstr, stuck_udpsock);
	}

	//this->m_inet->throwrecv(&stuck_udpsock);
}



void TcpKernel::register_rq(char* cstr, SOCKET sock)//处理注册请求
{
	REGISTER_RQ* rq = (REGISTER_RQ*)cstr;//获取注册请求信息
	REGISTER_RS rs;//注册请求回复
	rs.flags = _DEF_PROTOCOL_REGISTER_RS;


 	stringstream ss;
	ss << "select u_id from user where u_tel =" << rq->tel << ";";
	list<string> lis;

	m_mysql->SelectMySql(ss.str().c_str(),1,lis);//查询手机号是否被注册过
	if (!lis.empty())//列表非空说明已经存在
	{
		rs.result = REGISTER_FAILED;
	}
	else
	{
		rs.result = REGISTER_SUCCESS;

		stringstream ss2;
		ss2 << "insert into user(u_name, u_password, u_tel) values(" << "\"" << rq->name << "\",\"" <<
			rq->password << "\"," << rq->tel << ");";//拼接语句

		if (!m_mysql->UpdateMySql(ss2.str().c_str()))//更新数据库注册
		{
			cout << "updata faild register" << endl;
			return;
		}
		m_mysql->SelectMySql(ss.str().c_str(), 1, lis);//获取id
		rs.id = atoi(lis.front().c_str());
		//以文件id创建存储位置
		ofstream ff;
		stringstream ss;
		ss << path << rs.id;
		ff.open(ss.str());
		ff.close();
		
	}
	m_inet->SendData(sock, (char*)&rs, sizeof(rs));
}

void TcpKernel::login_rq(char* cstr, SOCKET sock)
{
	LOGIN_RQ* rq = (LOGIN_RQ*)cstr;
	LOGIN_RS rs;
	rs.flags = _DEF_PROTOCOL_LOGIN_RS;
	stringstream ss;
	ss << "select u_password from user where u_tel =" << rq->tel << ";";
	list<string> lis;
	m_mysql->SelectMySql(ss.str().c_str(), 1, lis);//查询tel是否存在

	if (lis.empty())//不存在，没有注册过
	{
		rs.result = REGISTER_FAILED_TELNOEXIST;
	}
    else if (lis.front() == rq->password)//密码正确，成功登录
	{
		rs.result = REGISTER_SUCCESS;
		stringstream ss;
		ss << "select u_id,u_name from user where u_tel =" << rq->tel << ";";
		lis.pop_front();
		m_mysql->SelectMySql(ss.str().c_str(), 2, lis);
		rs.id = atoi(lis.front().c_str());

		lis.pop_front();

		strcpy_s(rs.name, lis.front().c_str());

		m_mutex.lock();

		if (!m_map.count((int)rs.id))
		{//服务器运行期间未登录过
			
			m_map[(int)rs.id].tcpsock = sock;//将登录成功的id与tcp映射
			m_map2[(int)sock] = (int)rs.id;//tcp 与 id映射
			m_map3[rs.id] = sock;
			m_idtomess[rs.id].nhasid = 0;
		}
		rs.ack = m_idtomess[rs.id].nhasid;
		m_mutex.unlock();
	}
	else//密码错误
	{
		rs.result = REGISTER_FAILED_PASSWRONG;
	}
	
	m_inet->SendData(sock, (char*)&rs, sizeof(rs));
}

void TcpKernel::getfriendlist_rq(char* cstr, SOCKET sock)
{
	GETFRIENDLIST_RQ* rq = (GETFRIENDLIST_RQ*)cstr;
	//select f_id from friend where m_id = 1;
	GETFRIENDLIST_RS rs;
	rs.flags = _DEF_PROTOCOL_GETFRIENDLIST_RS;
	
	

	stringstream ss;
	ss << "select f_id,nickname,f_name from myview2 where u_id = " << rq->id << ";";
	list<string> lis;
	m_mysql->SelectMySql(ss.str().c_str(),3,lis);//查询好友信息
	
	for (int i = 0;i < lis.size();i ++)
	{
		FriendsList fri;
		fri.id = atoi(lis.front().c_str());//取出id
		lis.pop_front();
		//cout << " 8888888888888888888888888" << endl;
		cout << "fri.id  ：" << fri.id << endl;
		//取出昵称
		strcpy_s(fri.nickname, lis.front().c_str());
		lis.pop_front();
		//取出好友名
		strcpy_s(fri.name, lis.front().c_str());
		lis.pop_front();
		//rs.list_friends.push_back(fri);
		rs.list_friends[i] = fri;

		list<messque> lis;//给储存未发送的消息初始化

		//6.12 更改，只有第一次登录的时候才初始化
		if (!m_idtomess[rq->id].idtomess.count(fri.id))
		{
			m_mutex.lock();
			m_idtomess[rq->id].idtomess[fri.id] = lis;
			m_mutex.unlock();
		}
	    //用户id和它未收到的消息的映射   来自哪个好友id
	}
	if (rs.list_friends.front().id == 0)
	{
		rs.list_friends[0].id = 0;
	}
	rs.acknum = 0;    //5.31 取消使用，登录时即赋予
	rs.result = 90909;//5.29   定义了但没用上
	m_inet->SendData(sock, (char*)&rs, sizeof(rs));
}

void TcpKernel::addfriend_rq(char* cstr, SOCKET sock)
{
	ADDFRIEND_RQ* rq = (ADDFRIEND_RQ*)cstr;
	//查看对方是否存在,对方name也查出来
	stringstream sna;
	sna << "select u_name,u_id from user where u_tel = " << rq->f_tel << ';';
	list<string> lis;
	m_mysql->SelectMySql(sna.str().c_str(), 2, lis);
	if (lis.size() == 0)//不存在直接返回
	{//对方不存在
		ADDFRIEND_RS rs;
		//rs.f_id = rq->f_id;
		rs.m_id = rq->m_id;
		rs.result = ADD_UNEXIT;

		m_inet->SendData(sock, (char*)&rs, sizeof(rs));
		return;
	}


	if (rq->nickname != "")
	{
		strcpy_s(rq->nickname, lis.front().c_str());
	}
	lis.pop_front();

	//把消息放到未发送列表中
	addfrimess mess;
	mess.id = rq->m_id;
	mess.nickname = rq->nickname;

	//加入到对方未收到的好友请求
	int add_f_id = atoi(lis.front().c_str());
	m_mutex.lock();
	m_idtomess[add_f_id].addfriend.push_front(mess);
	m_mutex.unlock();
	

	rq->f_id = add_f_id;

	//查看对方是否在线
	m_mutex.lock();
	if(m_map.count(rq->f_id))
	{//在线发送
		ADDFRIEND_ACK_RQ ack_rq;
		
		ack_rq.f_id = (int)m_idtomess[rq->f_id].addfriend.front().id;
		
		ack_rq.m_id = rq->f_id;
		ack_rq.flags = _DEF_PROTOCOL_ADDFRIEND_ACK_RQ;

		
		int nlen = (int)m_idtomess[rq->f_id].addfriend.front().nickname.length();
		

		//加密昵称
		/*for (int i = 0; i < nlen; i++)
		{
			rq->nickname[i] ++;
		}*/
		 strcpy_s(ack_rq.nickname, rq->nickname);
		 //加密

		if (m_inet->SendData(sock, (char*)&ack_rq, sizeof(ack_rq)))//将请求发送给目标
		{
			//需要加锁
			//m_idtomess[rq->f_id].addfriend.pop_front();
		}
	}
	m_mutex.unlock();

}

void TcpKernel::addfriend_ack_rs(char* cstr, SOCKET sock)
{
	ADDFRIEND_ACK_RS* ackrs = (ADDFRIEND_ACK_RS*)cstr;

	ADDFRIEND_RS rs;
	rs.flags = _DEF_PROTOCOL_ADDFRIEND_RS;
	rs.f_id = ackrs->f_id;
	rs.m_id = ackrs->m_id;
	rs.result = ackrs->result;
	m_mutex.lock();
	//将添加好友从未发送的离线消息中删除
	m_idtomess[rs.f_id].addfriend.pop_front();
	//获取下一个好友请求列表
	//if (!m_idtomess[rs.f_id].addfriend.empty())
	//{
	//	//内存泄露
	//	ADDFRIEND_RQ* rq = new ADDFRIEND_RQ;
	//	rq->flags = _DEF_PROTOCOL_ADDFRIEND_RQ;
	//	int m_id = ackrs->m_id;
	//	int f_id = m_idtomess[ackrs->m_id].addfriend.front().id;
	//	if (strlen(m_idtomess[ackrs->m_id].addfriend.front().nickname.c_str()))
	//	{
	//		strcpy_s(rq->nickname, m_idtomess[ackrs->m_id].addfriend.front().nickname.c_str());
	//	}
	//	this->dealTask((char*)rq, sock);
	//}
	m_mutex.unlock();
	cout << " ackrs->result  " << ackrs->result << endl;
	if (ackrs->result == ADD_ACCEPT)
	{//同意添加

		//ackrs->result;
		if (ackrs->m_nickname != "")
		{//如果没填昵称，就用名字
			stringstream s3;
			s3 << "select u_name from user where u_id = " << ackrs->m_id << ';';
			list<string> lis;
			m_mysql->SelectMySql(s3.str().c_str(), 1, lis);
			cout << "select u_name from user where u_id ; u_name =  " << lis.front().c_str() << endl;
			strcpy_s(ackrs->m_nickname, lis.front().c_str());
		}

		int nlen = (int)strlen(ackrs->f_nickname);
		/*for (int i = 0; i < nlen; i++)
		{
			ackrs->f_nickname[i] --;
		}*/

		
		stringstream ss;
		ss << "insert into  friend(m_id,f_id,nickname) values(" <<
			ackrs->m_id << "," << ackrs->f_id << "," << "\"" <<
			ackrs->f_nickname << "\"" << ");";
		cout  << "add friend sql  " << ss.str().c_str() << endl;
		if (!m_mysql->UpdateMySql(ss.str().c_str()))
		{
			cout << "add friend update failed  1" << endl;
			return;
		}
		stringstream s2;
		s2 << "insert into  friend(m_id,f_id,nickname) values(" <<
			ackrs->f_id << "," << ackrs->m_id << "," << "\"" <<
			ackrs->m_nickname << "\"" << ");";
		cout << "add friend sql" << s2.str().c_str() << endl;
		if(!m_mysql->UpdateMySql(s2.str().c_str()))
		{
			cout << "add friend update failed  2" << endl;
			return;
		}
		//rs.result = ADD_ACCEPT;
	}
	else
	{
		//rs.result = ADD_REFUSE;
	}
	cout << " rs->result  " << ackrs->result << endl;
	m_inet->SendData(sock, (char*)&rs, sizeof(rs));
	
	//6.9添加，向选择添加好友的一方发送获取好友列表请求回复
	GETFRIENDLIST_RQ rq;
	rq.flags = _DEF_PROTOCOL_GETFRIENDLIST_RQ;
	rq.id = rs.f_id;
	//6.12修改，之前socket选错了
	this->dealTask((char*)&rq, m_map3[rq.id]);
}

void TcpKernel::sendmessage_rq(char* cstr, stu_udpsock udp)
{
	m_mutex.lock();
	SENDMESSAGE_RQ* rq = (SENDMESSAGE_RQ*)cstr;
	SENDMESSAGE_RQ rq2;
	rq2.flags = _DEF_PROTOCOL_SENDMESSAGE_RQ;
	rq2.fri_id = rq->fri_id;
	rq2.my_id = rq->my_id;

	if (m_idtomess[rq->my_id].nhasid == rq->acknum)//如果这条消息之前已经收到过确认，重新发确认包
	{//向客户端发送确认id
		SENDMESSAGE_RS rs;
		rs.flags = _DEF_PROTOCOL_SENDMESSAGE_RS;
		rs.acknum = rq->acknum;
		rs.f_id = rq->fri_id;
		rs.m_id = rq->my_id;

		m_inet->Sendudpdate(udp, (char*)&rs, sizeof(rs));
		m_mutex.unlock();
		return;
	}



	//存在对应列表里
	//将这条消息加入消息列表（需要锁）
	messque qq;
	qq.acknum = rq->acknum;
	strcpy_s(qq.cstr, rq->szbuf);
	m_idtomess[rq->fri_id].idtomess[rq->my_id].push_back(qq);
	//加入好友的来自发消息的人的未接收消息列表
	//在收到消息确认的时候才会把消息移除


	//只要成功收到消息，发送确认序号
	m_idtomess[rq->my_id].nhasid = rq->acknum;//将这条消息确认
	SENDMESSAGE_RS rs;//发送消息接收回复包
	rs.flags = _DEF_PROTOCOL_SENDMESSAGE_RS;
	rs.acknum = rq->acknum;
	m_inet->Sendudpdate(udp, (char*)&rs, sizeof(rs));

	//goto aaa;

	//转发到另一客户端
	if (!m_map.count((int)rq->fri_id))//好友未在线
	{

	}
	else
	{
		//aaa:
		//cout << "haoyou in string" << endl;
		sockaddr_in sockline;
		int nl = sizeof(sockline);
		if (getpeername(m_map[(int)rq->fri_id].tcpsock, (struct sockaddr*)&sockline, &nl))
		{
			cout << "通过sock获取朋友id失败" << endl;
			return;
		}

		rq2.flags = _DEF_PROTOCOL_SENDMESSAGE_RQ;
		strcpy_s(rq2.szbuf, m_idtomess[rq->fri_id].idtomess[rq->my_id].front().cstr);
		//strcpy_s(rq2.szbuf, "你好啊");
		//rq2.szbuf = m_idtomess[rq->my_id].idtomess[rq->fri_id].front().cstr;
		//发送来自该好友未接收消息的第一个
		rq2.acknum = m_idtomess[rq->fri_id].idtomess[rq->my_id].front().acknum;
		//sendto(udp.udp_winter, cstr, sizeof(cstr), 0, (const sockaddr*)&sockline, nl);
		udp.sockline = sockline;
		udp.nlen = nl;


		//转发给目标客户端
		m_inet->Sendudpdate(udp, (char*)&rq2, sizeof(rq2));
	}

	m_mutex.unlock();

}
void TcpKernel::sendmessage_rs(char* cstr, stu_udpsock udp)
{
	SENDMESSAGE_RS* rs = (SENDMESSAGE_RS*)cstr;
	m_mutex.lock();
	if (rs->acknum == -1)
	{

	}
	else if (m_idtomess[rs->m_id].idtomess[rs->f_id].front().acknum == rs->acknum)
	{//序列号对，就把这个消息移除，发送下一个消息
		m_idtomess[rs->m_id].idtomess[rs->f_id].pop_front();
		//需要加锁
	}
	
	else
	{
		m_mutex.unlock();
		return;
	}
	//发送新的消息
	if (!m_idtomess[rs->m_id].idtomess[rs->f_id].empty())
	{//在队列中找第一个发送
		SENDMESSAGE_RQ rq;
		rq.flags = _DEF_PROTOCOL_SENDMESSAGE_RQ;
		rq.fri_id = rs->m_id;
		rq.my_id = rs->f_id;
		rq.acknum = (int)m_idtomess[rs->m_id].idtomess[rs->f_id].front().acknum;
		//rq.szbuf = m_idtomess[rs->m_id].idtomess[rs->f_id].front().cstr;
		strcpy_s(rq.szbuf, m_idtomess[rs->m_id].idtomess[rs->f_id].front().cstr);
		m_inet->Sendudpdate(udp, (char*)&rq, sizeof(rq));
	}
	m_mutex.unlock();







}

void TcpKernel::getnotrecv(char* cstr, stu_udpsock sock)//获取未接收的消息
{//应该再单独开一个线程
	GETNOTRECV* nr = (GETNOTRECV*)cstr;
	m_mutex.lock();
	if (m_idtomess.count(nr->m_id))//如果存在
	{
		//把好友列表发过去
		while (!m_idtomess[(int)nr->m_id].addfriend.empty() && m_map.count((int)nr->m_id))
		{//未收到的好友列表不为空
			ADDFRIEND_ACK_RQ rq;
			rq.f_id = (int)m_idtomess[nr->m_id].addfriend.front().id;
			rq.m_id = (int)nr->m_id;
			strcpy_s(rq.nickname, m_idtomess[nr->m_id].addfriend.front().nickname.c_str());

			if (m_inet->SendData(m_map[(int)nr->m_id].tcpsock, (char*)&rq, sizeof(rq)))
			{
				m_idtomess[nr->m_id].addfriend.pop_front();
			}
		}


		for (auto ite : m_idtomess[nr->m_id].idtomess)//遍历所有好友，看谁有消息没发就发一遍
		{//可能存在并发问题，正在发消息，这面又读到了，再次发送，需要锁
			if (!ite.second.empty())
			{//发送这个好友的第一个消息，然后等待消息回复发消息就行了
				SENDMESSAGE_RQ rq;
				rq.flags = _DEF_PROTOCOL_SENDMESSAGE_RQ;
				rq.fri_id = ite.first;
				rq.my_id = nr->m_id;

				rq.acknum = (int)m_idtomess[nr->m_id].idtomess[ite.first].front().acknum;
				//rq.szbuf = m_idtomess[nr->m_id].idtomess[ite.first].front().cstr;
				strcpy_s(rq.szbuf, m_idtomess[nr->m_id].idtomess[ite.first].front().cstr);


				m_inet->Sendudpdate(sock, (char*)&rq, sizeof(rq));
			}
		}

	}
	else
	{
		m_mutex.unlock();
		return;
	}
	m_mutex.unlock();
}