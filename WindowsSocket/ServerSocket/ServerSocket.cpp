// ServerSocket.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <vector>
#include <WinSock2.h>
#include <iostream>
#pragma comment(lib, "ws2_32")

CRITICAL_SECTION cs; //临界区
class ServerSocket{
private :
	std::string ip = "127.0.0.1";
	int port = 8080;
	SOCKET so;
	static ServerSocket * serverSocket;  //存贮单例对象
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
		std::cout << "网络端口号 ：" << sockaddrIn.sin_port << "  " << this->port << std::endl;
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
		listen(so, 10); //监听 请求队列设置最大长度位 10
	}
public:
	static ServerSocket & getInstance(){ 
		EnterCriticalSection(&cs); //进入临界区
		if (nullptr == serverSocket)
			serverSocket = new ServerSocket;
		LeaveCriticalSection(&cs); //离开临界区
		return * serverSocket;
	}

	//从队里中取得一个请求，并且处理客户端请求
	void openClient(){
		ServerSocket::ClientSocket * client = new ServerSocket::ClientSocket;
		//新开一个线程来处理与当前client socket的通信
	}

	class ClientSocket {
	private:
		SOCKET client;
	public:
		ClientSocket(){
			sockaddr socka; //用来存新接受套接字地址
			int socklen = sizeof(sockaddr); //存套接字地址结构的长度 
			this->client = accept(serverSocket->so, &socka, &socklen); //服务器端调用accept从连接队列取出来第一个。客户端可调用connect进行连接
			serverSocket->clientList.push_back(this);
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
		}
		//创建线程调用函数
		static void run(ClientSocket * pClientSocket){
			pClientSocket->client;
			//开一个线程接受客户端输入
			//开一个线程接受服务器端输入并且发送给客户端
		}
		//接受客户端消息并打印到控制台
		void printMsgFromClient(std::ostream & os){
			char buff[1024 + 1] = { 0 };
			int len = 0;
			do{
				int len = recv(this->client, buff, 1024, 0);
				//以下异常处理在实际开发会更细致，这只是做了打印处理
				if (len == SOCKET_ERROR){
					os << "与客户端进行数据传输时出现错误，错误代码为：" << WSAGetLastError() << std::endl;
				}
				else if (len == 0){ //如果len == 0 表示连接中断
					os << "与客户端的连接中断...." << std::endl;
					break;
				}
			} while (true);
		}

		//从输入流中得到输入字符串传给客户端socket
		void sendMsg2Client(std::istream & is){
			char buff[1024 + 1] = { 0 };
			while (!is.eof()){
				is >> buff;
				if (SOCKET_ERROR == (this->client, buff, strlen(buff), 0)){ //函数返回发送的字节数，失败会设置getlasterror
					std::cout << "与客户端进行数据传输时出现错误，错误代码为：" << WSAGetLastError() << std::endl;
				}
			}
		}
	};
	std::vector<ClientSocket *> clientList;

	//析构，中间清空clientList，防止内容泄露
	~ServerSocket(){
		closesocket(this->so); //关闭服务器端的socket
		clientList.clear(); //清空
		clientList.swap(std::vector<ClientSocket *>()); //利用跟空对象交换回收内存。因为上面的clear虽然清空了，但是物理上那些内存还是可使用的
		WSACleanup(); //释放socket资源
	}

	//关闭，需要手动调用
	void close(){
		delete serverSocket;
	}
};

ServerSocket * ServerSocket::serverSocket = nullptr; //类中静态变量在需要在外面初始化

int _tmain(int argc, _TCHAR* argv[])
{
	InitializeCriticalSection(&cs); //初始化临界区变量
	ServerSocket serverSocket = ServerSocket::getInstance();
	serverSocket.openClient();
	return 0;
}

