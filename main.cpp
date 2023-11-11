#include"TcpInet.h"
#include"TcpKernel.h"
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif



int main()
{	
	TcpKernel* ker = new TcpKernel;
	ker->initKernel();
	system("pause");
	return 0;
}