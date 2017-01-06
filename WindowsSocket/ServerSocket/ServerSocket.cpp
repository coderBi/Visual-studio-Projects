// ServerSocket.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <vector>
#include <WinSock2.h>
#include <iostream>
#include <string>
#include <process.h> //_beginethread 等
#pragma comment(lib, "ws2_32")

//最大等待连接客户端数量
#define MAX_CLIENT_SEQUENCE 100

CRITICAL_SECTION cs; //临界区

class ServerSocket{
private :
	std::string ip = "127.0.0.1";
	int port = 8080;
	SOCKET so;
	static ServerSocket * serverSocket;  //存贮单例对象
	int clientItems = 0; //当前连接客户端的数目 
	//初始化sock调用所需环境
	void initSocketCall(){ 
		WSADATA wsData; //直译wsadata的意思是： windows socket asychronize data
		//WSAStartup, 第一个参数word低位是需要的socket主版本 高位是次版本号。 这个函数跟操作系统底层dll约定调用哪一版本的dll。 返回值 0 表示成功, 否则返回错误代码
		if (0 != WSAStartup(MAKEWORD(2, 2), &wsData)){
			std::cout << "socket dll 初始化失败" << std::endl;
			exit(-1);
		}
	}
	ServerSocket(){
		initSocketCall();
		//socket函数第一个参数表示通信协议族，如果协议使用的是tcp或者udp这里恒为pf_inet(或者也可以写成af_inet)
		//socket函数第二个参数表示要创建的套接字类型，如果是tcp这里是sock_stram 如果是udp这里是 sock_dgram
		//socket函数第三个参与表示要使用的协议，跟第二个参数相匹配
		//socket函数返回socket套接字描述，如果失败返回 -1，并且填充getlasterror
		if (-1 == (so = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP))){
			//实际开发要做更细致的处理，这里暂时只是打印输出
			std::cout << "创建socket失败， 错误代码为： " << WSAGetLastError() << std::endl;
			exit(-1);
		}
		//绑定服务器端端口与ip
		sockaddr_in sockaddrIn;
		ZeroMemory(&sockaddrIn, sizeof(sockaddrIn)); //也可以写sizeof(socketaddr_in) 他们的大小是一样的。
		//inet_ntoa: 将unsigned long 的ip转换成字符串  inet_addr; 将字符串类型的ip转换成 unsigned long
		sockaddrIn.sin_addr.S_un.S_addr = inet_addr(this->ip.c_str()); //ip
		//这里的htons是将本地的字节序列变成网络字节序列（大头朝向，如果本地是小头朝上（如x86cpu）把字节的序列倒过来存放), 提高兼容性
		sockaddrIn.sin_port = htons(this->port);  //端口 这里需要提供网络字节序的端口号
		sockaddrIn.sin_family = PF_INET; //协议族

		//第二个参数需要从sockaddr_in 类型转换过来，有历史原因。 sockaddr 其实与 sockaddr_in 都占16个字节，其中都是前两个自己表示
		//协议族。 在sockaddr里面，接下来14个字节将ip跟接口放在一起表示 "1.1.1.1:80"，但是在对像"1.1.1.1:80"这样的结构直接赋值到
		//sockaddr很不方便，没有合适的转换函数。 而sockaddr_in将后面的14字节拆分细化，前两个自己表示端口，然后4个字节表示ip，后面
		//多余的8字节不用。这样就方便了对ip跟端口分别赋值。
		//sockaddr类型可以表示各种类型的ip， sockaddr_in表示ipv4， 类似的有sockaddr_in6来表示ipv6.
		//bind成功返回 0   失败返回-1 并且会设置 lasterror
		if (-1 == bind(so, (sockaddr*)&sockaddrIn, sizeof(sockaddr))){
			std::cout << "socket bind 失败， 错误代码为： " << WSAGetLastError() << std::endl;
			exit(-1);
		}
		listen(so, MAX_CLIENT_SEQUENCE); //监听 请求队列最大长度 
	}
public:
	static ServerSocket & getInstance(){ 
		EnterCriticalSection(&cs); //进入临界区
		if (nullptr == serverSocket)
			serverSocket = new ServerSocket;
		LeaveCriticalSection(&cs); //离开临界区
		return * serverSocket;
	}

	int getClientItems(){
		return this->clientItems;
	}

	//从队里中取得一个请求，并且处理客户端请求
	bool openClient(){
		//创建客户端对象
		ServerSocket::ClientSocket * client = new ServerSocket::ClientSocket;
		//处理通信，开一个线程维护当前的client通信
		client->runHandle = (HANDLE)_beginthreadex(nullptr, 0, ClientSocket::run, client, 0, nullptr);
		if (nullptr == client->runHandle){ //线程创建失败
			delete client;
			return false;
		}
		//添加客户端对象到客户端列表
		serverSocket->clientList.push_back(client);
		//添加句柄到列表
		serverSocket->subThreadHandles.push_back(client->runHandle);
		ServerSocket::serverSocket->clientItems++;
		return true;
	}

	class ClientSocket {
	private:
		SOCKET client;
	public:
		HANDLE handles[2]; //保存接受跟发送线程句柄
		HANDLE runHandle; //保存调用run函数产生的新句柄
		ClientSocket(){
			sockaddr socka; //用来存新接受套接字地址
			int socklen = sizeof(sockaddr); //存套接字地址结构的长度 
			this->client = accept(serverSocket->so, &socka, &socklen); //服务器端调用accept从连接队列取出来第一个。客户端可调用connect进行连接
		}
		~ClientSocket(){
			closesocket(this->client); //关闭客户端的socket
			//在析构里面将当前对象从客户端列表中移除
			for (auto it = serverSocket->clientList.begin(); it != serverSocket->clientList.end(); ++it){
				if (*it == this){
					serverSocket->clientList.erase(it);
					break;
				}
			}
			//将本线程相关的run函数调用产生的线程句柄从服务器对象保存的句柄列表中移除
			for (auto it = serverSocket->subThreadHandles.begin(); it != serverSocket->subThreadHandles.end(); ++it){
				if (*it == this->runHandle){
					serverSocket->subThreadHandles.erase(it);
					break;
				}
			}
		}

		static unsigned int __stdcall run(void * parameters){
			ClientSocket * pClientSocket = (ClientSocket*)parameters;
			//开一个线程发送消息
			if (nullptr == (pClientSocket->handles[1] = (HANDLE)_beginthreadex(nullptr, 0, &ClientSocket::sendMsg2Client, pClientSocket, 0, nullptr))){
				std::cout << "创建发送线程失败，错误代码" << GetLastError() << std::endl;
				return -1;
			}
			//开一个线程接受消息
			if (nullptr == (pClientSocket->handles[0] = (HANDLE)_beginthreadex(nullptr, 0, &ClientSocket::recvMsgFromClient, pClientSocket, 0, nullptr))){
				std::cout << "创建写入线程失败，错误代码" << GetLastError() << std::endl;
				return -1;
			}
			//当前线程等待输入输出线程
			WaitForMultipleObjects(2, pClientSocket->handles, FALSE, INFINITE);

			//关闭子线程句柄
			for (int i = 0; i < 2; ++i){
				CloseHandle(pClientSocket->handles[i]);
			}
			//释放客户端对象
			delete pClientSocket;
			return 0;
		}

		//接受客户端消息并打印到控制台
		static unsigned int __stdcall recvMsgFromClient(void * parameters){
			//处理参数
			ClientSocket * cs = (ClientSocket*)parameters;

			//获取并打印
			char buff[1024 + 1] = { 0 };
			int len = 0;
			do{
				int len = recv(cs->client, buff, 1024, 0);
				//以下异常处理在实际开发会更细致，这只是做了打印处理
				if (len == SOCKET_ERROR){
					std::cout << "与客户端进行数据传输时出现错误，错误代码为：" << WSAGetLastError() << std::endl;
					break;
				}
				else if (len == 0){ //如果len == 0 表示连接中断
					std::cout << "与客户端的连接中断...." << std::endl;
					break;
				}
				buff[len] = '\0'; ///实际发现需要自己追加这个\0
				std::cout << buff;
			} while (true);
			return 0;
		}

		//从输入流中得到输入字符串传给客户端socket
		static unsigned int __stdcall sendMsg2Client(void * parameters){
			//处理参数
			ClientSocket * cs = (ClientSocket*)parameters;

			//读入并发送
			char buff[1024 + 1] = { 0 };
			while (true){
				int i;
				for (i = 0; i < 1024; ++i){
					std::cin.get(buff[i]);
					if (buff[i] == '\n'){
						i++;
						break;
					}
				}
				buff[i] = '\0';
				if (SOCKET_ERROR == send(cs->client, buff, strlen(buff), 0)){ //函数返回发送的字节数，失败会设置getlasterror
					std::cout << "与客户端进行数据传输时出现错误，错误代码为：" << WSAGetLastError() << std::endl;
					break;
				}
			}
			return 0;
		}

		//发送消息到当前客户端
		bool sendMsg(std::string & msg){
			if (SOCKET_ERROR == send(this->client, msg.c_str(), msg.length(), 0)){ //函数返回发送的字节数，失败会设置getlasterror
				std::cout << "与客户端进行数据传输时出现错误，错误代码为：" << WSAGetLastError() << std::endl;
				return false;
			}
			return true;
		}
	};
	//已连接的客户端对象列表
	std::vector<ClientSocket *> clientList;
	//子线程句柄集合，每连接一个客户端就保存相应的线程句柄
	std::vector<HANDLE> subThreadHandles;

	//析构，中间清空clientList，防止内容泄露
	~ServerSocket(){
		closesocket(this->so); //关闭服务器端的socket
		clientList.clear(); //清空
		clientList.swap(std::vector<ClientSocket *>()); //利用跟空对象交换回收内存。因为上面的clear虽然清空了，但是物理上那些内存还是可使用的
		WSACleanup(); //释放socket资源
	}

	//关闭，需要手动调用
	void close(){
		if (!subThreadHandles.empty()){
			std::cout << "尚有客户端连接到这台机器，请确认是否关闭服务器(y:yes  n:no) : " << std::endl;
			std::string s;
			std::cin >> s;
			if (s == "n" || s == "N")
				return;
		}
		delete serverSocket;
	}
};

ServerSocket * ServerSocket::serverSocket = nullptr; //类中静态变量在需要在外面初始化

int _tmain(int argc, _TCHAR* argv[])
{
	InitializeCriticalSection(&cs); //初始化临界区变量
	ServerSocket & serverSocket = ServerSocket::getInstance();

	//这里只是单线程通信，如果要多线程，得把run方法里面修改成只有接受线程。并且将服务器端写的函数放到ServerSocket类里面。
	if (serverSocket.openClient()){
		std::cout << "新的客户端连接到服务器..." << std::endl;
		std::cout << "当前连接客户端数量为: " << serverSocket.getClientItems() << std::endl;
	}
	/*
	通过测试，用死循环阻止程序推出是不合适的。发现这个程序会过度占用cpu资源，也就是导致cpu空转。
		while (!serverSocket.clientList.empty()) {}
	*/
	
	while (!serverSocket.clientList.empty()) {}
	std::cout << "当前没有客户端连接到服务器，即将关闭服务器..." << std::endl;
	return 0;
}