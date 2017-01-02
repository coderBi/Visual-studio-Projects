// ClientSocket.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <WinSock2.h>
#pragma comment(lib,"ws2_32")
#include <iostream>
#include <process.h>
using std::cout;
using std::cin;
using std::endl;

class ClientSocket{
private:
	SOCKET s;
public:
	HANDLE handles[2]; //读线程句柄  写线程句柄
	static void initSocket(){
		WSADATA wsaData;
		if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData)){
			cout << "socket dll 初始化失败" << endl;
			exit(-1);
		}
	}
	ClientSocket(){
		initSocket();
		s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (-1 == s){
			cout << "创建socket出错，错误代码为： " << WSAGetLastError() << endl;
			exit(-1);
		}
		sockaddr_in si;
		ZeroMemory(&si, sizeof(si));
		si.sin_family = PF_INET;
		si.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
		si.sin_port = htons(8080);

		int connectRes = -1;
		do
		{
			connectRes = connect(s, (sockaddr*)&si, sizeof(sockaddr));
			if (-1 == connectRes){
				cout << "连接服务器失败，等待重新链接..." << endl;
			}
			else if (0 == connectRes){
				break;
			}
		} while (true);
		cout << "连接服务器成功..." << endl;
	}

	//接受端消息并打印到控制台
	static unsigned int __stdcall recvMsg(void * parameters){
		//处理参数
		ClientSocket * p = (ClientSocket*)parameters;

		//获取并打印
		char buff[1024 + 1] = { 0 };
		int len = 0;
		do{
			int len = recv(p->s, buff, 1024, 0);
			//以下异常处理在实际开发会更细致，这只是做了打印处理
			if (len == SOCKET_ERROR){
				cout << "与服务器进行数据传输时出现错误，错误代码为：" << WSAGetLastError() << std::endl;
				break;
			}
			else if (len <= 0){ //如果len <= 0 表示连接中断
				cout << "与服务器的连接中断...." << std::endl;
				break;
			}
			buff[len] = '\0'; ///实际发现需要自己追加这个\0
			cout << buff;
		} while (true);

		//_endthreadex结束当前线程，参数是返回值
		//msdn上面的例子这里调用了_endthreadex,但是同时它又支出函数return之后会自动调用endthreadex，
		//如果自己在里面调会导致函数意外终止，会导致线程析构不执行可能产生泄露。所以最好是不在线程内部调用endthreadex，让函数正常返回。
		//_endthreadex(0);
		return 0;
	}

	//发送消息
	static unsigned int __stdcall sendMsg(void * parameters){
		//处理参数
		ClientSocket * p = (ClientSocket*)parameters;

		//读入并发送
		char buff[1024 + 1] = { 0 };
		while (true){
			int i = 0;
			for (i = 0; i < 1024; ++i){
				cin.get(buff[i]);
				if ('\n' == buff[i]){ //输入了换行，直接发送消息
					i++;
					break;
				}
			}
			buff[i] = '\0';
			if (SOCKET_ERROR == send(p->s, buff, strlen(buff), 0)){ //函数返回发送的字节数，失败会设置getlasterror
				std::cout << "与服务器进行数据传输时出现错误，错误代码为：" << WSAGetLastError() << std::endl;
				break;
			}
		}
		return 0;
	}

	void run(){
		//开一个线程发送消息
		if (nullptr == (handles[1] = (HANDLE)_beginthreadex(nullptr, 0, &ClientSocket::sendMsg, this, 0, nullptr))){
			std::cout << "创建发送线程失败，错误代码" << GetLastError() << endl;
		}
		//开一个线程接受消息
		if(nullptr == (handles[0] = (HANDLE)_beginthreadex(nullptr, 0, &ClientSocket::recvMsg, this, 0, nullptr))){
			std::cout << "创建写入线程失败，错误代码" << GetLastError() << endl;
		}
	}
	~ClientSocket(){
		closesocket(s);
		WSACleanup();
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	ClientSocket cs;
	cs.run();

	//waitformultipleobjects 第三个参数表示是否等待所有。如果是true表示等待所有子线程结束，
	//如果是false任意一个结束就停止主线程的等待并且返回值减去wait_object_0得到的是触发了事件的最小handles里面的脚标。（可能有多个同时触发）
	DWORD dwWait = WaitForMultipleObjects(2, cs.handles, FALSE, INFINITE);

	//关闭子线程句柄
	for (int i = 0; i < 2; ++i){
		CloseHandle(cs.handles[i]);
	}

	//输出是哪一个线程导致程序结束的
	switch (dwWait - WAIT_OBJECT_0)
	{
	case 0:
		cout << "客户端接受消息线程意外终止，程序即将一同结束..." << endl;
		break;
	case 1:
		cout << "客户端发送消息线程意外终止，程序即将一同结束..." << endl;
		break;
	default:
		cout << "客户端与服务器连接出现不可预料问题，程序即将终止..." << endl;
		break;
	}
	return 0;
}

