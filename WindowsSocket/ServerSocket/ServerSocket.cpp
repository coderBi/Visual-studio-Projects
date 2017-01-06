// ServerSocket.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <vector>
#include <WinSock2.h>
#include <iostream>
#include <string>
#include <process.h> //_beginethread ��
#pragma comment(lib, "ws2_32")

//���ȴ����ӿͻ�������
#define MAX_CLIENT_SEQUENCE 100

CRITICAL_SECTION cs; //�ٽ���

class ServerSocket{
private :
	std::string ip = "127.0.0.1";
	int port = 8080;
	SOCKET so;
	static ServerSocket * serverSocket;  //������������
	int clientItems = 0; //��ǰ���ӿͻ��˵���Ŀ 
	//��ʼ��sock�������軷��
	void initSocketCall(){ 
		WSADATA wsData; //ֱ��wsadata����˼�ǣ� windows socket asychronize data
		//WSAStartup, ��һ������word��λ����Ҫ��socket���汾 ��λ�Ǵΰ汾�š� �������������ϵͳ�ײ�dllԼ��������һ�汾��dll�� ����ֵ 0 ��ʾ�ɹ�, ���򷵻ش������
		if (0 != WSAStartup(MAKEWORD(2, 2), &wsData)){
			std::cout << "socket dll ��ʼ��ʧ��" << std::endl;
			exit(-1);
		}
	}
	ServerSocket(){
		initSocketCall();
		//socket������һ��������ʾͨ��Э���壬���Э��ʹ�õ���tcp����udp�����Ϊpf_inet(����Ҳ����д��af_inet)
		//socket�����ڶ���������ʾҪ�������׽������ͣ������tcp������sock_stram �����udp������ sock_dgram
		//socket���������������ʾҪʹ�õ�Э�飬���ڶ���������ƥ��
		//socket��������socket�׽������������ʧ�ܷ��� -1���������getlasterror
		if (-1 == (so = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP))){
			//ʵ�ʿ���Ҫ����ϸ�µĴ���������ʱֻ�Ǵ�ӡ���
			std::cout << "����socketʧ�ܣ� �������Ϊ�� " << WSAGetLastError() << std::endl;
			exit(-1);
		}
		//�󶨷������˶˿���ip
		sockaddr_in sockaddrIn;
		ZeroMemory(&sockaddrIn, sizeof(sockaddrIn)); //Ҳ����дsizeof(socketaddr_in) ���ǵĴ�С��һ���ġ�
		//inet_ntoa: ��unsigned long ��ipת�����ַ���  inet_addr; ���ַ������͵�ipת���� unsigned long
		sockaddrIn.sin_addr.S_un.S_addr = inet_addr(this->ip.c_str()); //ip
		//�����htons�ǽ����ص��ֽ����б�������ֽ����У���ͷ�������������Сͷ���ϣ���x86cpu�����ֽڵ����е��������), ��߼�����
		sockaddrIn.sin_port = htons(this->port);  //�˿� ������Ҫ�ṩ�����ֽ���Ķ˿ں�
		sockaddrIn.sin_family = PF_INET; //Э����

		//�ڶ���������Ҫ��sockaddr_in ����ת������������ʷԭ�� sockaddr ��ʵ�� sockaddr_in ��ռ16���ֽڣ����ж���ǰ�����Լ���ʾ
		//Э���塣 ��sockaddr���棬������14���ֽڽ�ip���ӿڷ���һ���ʾ "1.1.1.1:80"�������ڶ���"1.1.1.1:80"�����Ľṹֱ�Ӹ�ֵ��
		//sockaddr�ܲ����㣬û�к��ʵ�ת�������� ��sockaddr_in�������14�ֽڲ��ϸ����ǰ�����Լ���ʾ�˿ڣ�Ȼ��4���ֽڱ�ʾip������
		//�����8�ֽڲ��á������ͷ����˶�ip���˿ڷֱ�ֵ��
		//sockaddr���Ϳ��Ա�ʾ�������͵�ip�� sockaddr_in��ʾipv4�� ���Ƶ���sockaddr_in6����ʾipv6.
		//bind�ɹ����� 0   ʧ�ܷ���-1 ���һ����� lasterror
		if (-1 == bind(so, (sockaddr*)&sockaddrIn, sizeof(sockaddr))){
			std::cout << "socket bind ʧ�ܣ� �������Ϊ�� " << WSAGetLastError() << std::endl;
			exit(-1);
		}
		listen(so, MAX_CLIENT_SEQUENCE); //���� ���������󳤶� 
	}
public:
	static ServerSocket & getInstance(){ 
		EnterCriticalSection(&cs); //�����ٽ���
		if (nullptr == serverSocket)
			serverSocket = new ServerSocket;
		LeaveCriticalSection(&cs); //�뿪�ٽ���
		return * serverSocket;
	}

	int getClientItems(){
		return this->clientItems;
	}

	//�Ӷ�����ȡ��һ�����󣬲��Ҵ���ͻ�������
	bool openClient(){
		//�����ͻ��˶���
		ServerSocket::ClientSocket * client = new ServerSocket::ClientSocket;
		//����ͨ�ţ���һ���߳�ά����ǰ��clientͨ��
		client->runHandle = (HANDLE)_beginthreadex(nullptr, 0, ClientSocket::run, client, 0, nullptr);
		if (nullptr == client->runHandle){ //�̴߳���ʧ��
			delete client;
			return false;
		}
		//��ӿͻ��˶��󵽿ͻ����б�
		serverSocket->clientList.push_back(client);
		//��Ӿ�����б�
		serverSocket->subThreadHandles.push_back(client->runHandle);
		ServerSocket::serverSocket->clientItems++;
		return true;
	}

	class ClientSocket {
	private:
		SOCKET client;
	public:
		HANDLE handles[2]; //������ܸ������߳̾��
		HANDLE runHandle; //�������run�����������¾��
		ClientSocket(){
			sockaddr socka; //�������½����׽��ֵ�ַ
			int socklen = sizeof(sockaddr); //���׽��ֵ�ַ�ṹ�ĳ��� 
			this->client = accept(serverSocket->so, &socka, &socklen); //�������˵���accept�����Ӷ���ȡ������һ�����ͻ��˿ɵ���connect��������
		}
		~ClientSocket(){
			closesocket(this->client); //�رտͻ��˵�socket
			//���������潫��ǰ����ӿͻ����б����Ƴ�
			for (auto it = serverSocket->clientList.begin(); it != serverSocket->clientList.end(); ++it){
				if (*it == this){
					serverSocket->clientList.erase(it);
					break;
				}
			}
			//�����߳���ص�run�������ò������߳̾���ӷ��������󱣴�ľ���б����Ƴ�
			for (auto it = serverSocket->subThreadHandles.begin(); it != serverSocket->subThreadHandles.end(); ++it){
				if (*it == this->runHandle){
					serverSocket->subThreadHandles.erase(it);
					break;
				}
			}
		}

		static unsigned int __stdcall run(void * parameters){
			ClientSocket * pClientSocket = (ClientSocket*)parameters;
			//��һ���̷߳�����Ϣ
			if (nullptr == (pClientSocket->handles[1] = (HANDLE)_beginthreadex(nullptr, 0, &ClientSocket::sendMsg2Client, pClientSocket, 0, nullptr))){
				std::cout << "���������߳�ʧ�ܣ��������" << GetLastError() << std::endl;
				return -1;
			}
			//��һ���߳̽�����Ϣ
			if (nullptr == (pClientSocket->handles[0] = (HANDLE)_beginthreadex(nullptr, 0, &ClientSocket::recvMsgFromClient, pClientSocket, 0, nullptr))){
				std::cout << "����д���߳�ʧ�ܣ��������" << GetLastError() << std::endl;
				return -1;
			}
			//��ǰ�̵߳ȴ���������߳�
			WaitForMultipleObjects(2, pClientSocket->handles, FALSE, INFINITE);

			//�ر����߳̾��
			for (int i = 0; i < 2; ++i){
				CloseHandle(pClientSocket->handles[i]);
			}
			//�ͷſͻ��˶���
			delete pClientSocket;
			return 0;
		}

		//���ܿͻ�����Ϣ����ӡ������̨
		static unsigned int __stdcall recvMsgFromClient(void * parameters){
			//�������
			ClientSocket * cs = (ClientSocket*)parameters;

			//��ȡ����ӡ
			char buff[1024 + 1] = { 0 };
			int len = 0;
			do{
				int len = recv(cs->client, buff, 1024, 0);
				//�����쳣������ʵ�ʿ������ϸ�£���ֻ�����˴�ӡ����
				if (len == SOCKET_ERROR){
					std::cout << "��ͻ��˽������ݴ���ʱ���ִ��󣬴������Ϊ��" << WSAGetLastError() << std::endl;
					break;
				}
				else if (len == 0){ //���len == 0 ��ʾ�����ж�
					std::cout << "��ͻ��˵������ж�...." << std::endl;
					break;
				}
				buff[len] = '\0'; ///ʵ�ʷ�����Ҫ�Լ�׷�����\0
				std::cout << buff;
			} while (true);
			return 0;
		}

		//���������еõ������ַ��������ͻ���socket
		static unsigned int __stdcall sendMsg2Client(void * parameters){
			//�������
			ClientSocket * cs = (ClientSocket*)parameters;

			//���벢����
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
				if (SOCKET_ERROR == send(cs->client, buff, strlen(buff), 0)){ //�������ط��͵��ֽ�����ʧ�ܻ�����getlasterror
					std::cout << "��ͻ��˽������ݴ���ʱ���ִ��󣬴������Ϊ��" << WSAGetLastError() << std::endl;
					break;
				}
			}
			return 0;
		}

		//������Ϣ����ǰ�ͻ���
		bool sendMsg(std::string & msg){
			if (SOCKET_ERROR == send(this->client, msg.c_str(), msg.length(), 0)){ //�������ط��͵��ֽ�����ʧ�ܻ�����getlasterror
				std::cout << "��ͻ��˽������ݴ���ʱ���ִ��󣬴������Ϊ��" << WSAGetLastError() << std::endl;
				return false;
			}
			return true;
		}
	};
	//�����ӵĿͻ��˶����б�
	std::vector<ClientSocket *> clientList;
	//���߳̾�����ϣ�ÿ����һ���ͻ��˾ͱ�����Ӧ���߳̾��
	std::vector<HANDLE> subThreadHandles;

	//�������м����clientList����ֹ����й¶
	~ServerSocket(){
		closesocket(this->so); //�رշ������˵�socket
		clientList.clear(); //���
		clientList.swap(std::vector<ClientSocket *>()); //���ø��ն��󽻻������ڴ档��Ϊ�����clear��Ȼ����ˣ�������������Щ�ڴ滹�ǿ�ʹ�õ�
		WSACleanup(); //�ͷ�socket��Դ
	}

	//�رգ���Ҫ�ֶ�����
	void close(){
		if (!subThreadHandles.empty()){
			std::cout << "���пͻ������ӵ���̨��������ȷ���Ƿ�رշ�����(y:yes  n:no) : " << std::endl;
			std::string s;
			std::cin >> s;
			if (s == "n" || s == "N")
				return;
		}
		delete serverSocket;
	}
};

ServerSocket * ServerSocket::serverSocket = nullptr; //���о�̬��������Ҫ�������ʼ��

int _tmain(int argc, _TCHAR* argv[])
{
	InitializeCriticalSection(&cs); //��ʼ���ٽ�������
	ServerSocket & serverSocket = ServerSocket::getInstance();

	//����ֻ�ǵ��߳�ͨ�ţ����Ҫ���̣߳��ð�run���������޸ĳ�ֻ�н����̡߳����ҽ���������д�ĺ����ŵ�ServerSocket�����档
	if (serverSocket.openClient()){
		std::cout << "�µĿͻ������ӵ�������..." << std::endl;
		std::cout << "��ǰ���ӿͻ�������Ϊ: " << serverSocket.getClientItems() << std::endl;
	}
	/*
	ͨ�����ԣ�����ѭ����ֹ�����Ƴ��ǲ����ʵġ����������������ռ��cpu��Դ��Ҳ���ǵ���cpu��ת��
		while (!serverSocket.clientList.empty()) {}
	*/
	
	while (!serverSocket.clientList.empty()) {}
	std::cout << "��ǰû�пͻ������ӵ��������������رշ�����..." << std::endl;
	return 0;
}