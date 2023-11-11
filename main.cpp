#include"TcpInet.h"
#include"TcpKernel.h"
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif



int main()
{
	//REGISTER_RQ* rq = new REGISTER_RQ;
	//rq->tel = 1678;
	//rq->name = "disk";
	//rq->password = "1234556";
	//rq->tel = 189;
	//stringstream ss;
	///*ss << "insert into user(u_name, u_password, u_tel) values(" << "\"" << rq->name << "\",\"" <<
	//	rq->password << "\"," << rq->tel << ");";*/
	//
	//ss << "select u_tel from user where u_tel =" << rq->tel << ";";

	//cout << ss.str() << endl;

	

	/*TcpKernel* ker = new TcpKernel;
	
	ker->initKernel();
	stringstream ss;
	ss << "select f_id,nickname from myview where u_id = " << 2 << ";";
	list<string> lis;
	cout << ss.str().c_str() << endl;
 	ker->m_mysql->SelectMySql(ss.str().c_str(), 2, lis);*/

	TcpKernel* ker = new TcpKernel;
	ker->initKernel();

	//GETFRIENDLIST_RQ rq;//获取好友列表测试
	//rq.flags = _DEF_PROTOCOL_GETFRIENDLIST_RQ;
	//rq.id = 1;
	//stringstream ss;
	//string str = "aaaa";
	//ss << "insert into  friend(m_id,f_id,nickname) values(" <<
	//	3 << "," << 2 << "," << "\"" <<
	//	str << "\"" << ");";
	//ss << "select f_id,nickname,f_name from myview2 where u_id = " << rq.id << ";";
	//cout << ss.str() << endl;

	//string path = "E:/memory/";//注册文件测试
	//ofstream ff;
	//stringstream ss;
	//ss << path << "999999" << '/' << "999999";
	//ff.open(ss.str());
	//ff << path << endl;
	//ff.close();


	//string str = "";
	//ifstream f2;
	//f2.open(ss.str());
	//f2 >> str;
	//f2.close();


	//LOGIN_RQ rq;
	//rq.flags = _DEF_PROTOCOL_LOGIN_RQ;
	//rq.tel = 123;
	//rq.password = "2131";

	//ker->dealTask((char*)&rq, 0);

	system("pause");
	return 0;
}