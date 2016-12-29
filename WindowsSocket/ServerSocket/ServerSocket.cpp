// ServerSocket.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <vector>
#include <WinSock2.h>
#include <iostream>
#pragma comment(lib, "ws2_32")

CRITICAL_SECTION cs; //�ٽ���
class ServerSocket{
private :
	std::string ip = "127.0.0.1";
	int port = 8080;
	SOCKET so;
	static ServerSocket * serverSocket;  //������������
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
		std::cout << "����˿ں� ��" << sockaddrIn.sin_port << "  " << this->port << std::endl;
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
		listen(so, 10); //���� �������������󳤶�λ 10
	}
public:
	static ServerSocket & getInstance(){ 
		EnterCriticalSection(&cs); //�����ٽ���
		if (nullptr == serverSocket)
			serverSocket = new ServerSocket;
		LeaveCriticalSection(&cs); //�뿪�ٽ���
		return * serverSocket;
	}

	//�Ӷ�����ȡ��һ�����󣬲��Ҵ���ͻ�������
	void openClient(){
		ServerSocket::ClientSocket * client = new ServerSocket::ClientSocket;
		//�¿�һ���߳��������뵱ǰclient socket��ͨ��
	}

	class ClientSocket {
	private:
		SOCKET client;
	public:
		ClientSocket(){
			sockaddr socka; //�������½����׽��ֵ�ַ
			int socklen = sizeof(sockaddr); //���׽��ֵ�ַ�ṹ�ĳ��� 
			this->client = accept(serverSocket->so, &socka, &socklen); //�������˵���accept�����Ӷ���ȡ������һ�����ͻ��˿ɵ���connect��������
			serverSocket->clientList.push_back(this);
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
		}
		//�����̵߳��ú���
		static void run(ClientSocket * pClientSocket){
			pClientSocket->client;
			//��һ���߳̽��ܿͻ�������
			//��һ���߳̽��ܷ����������벢�ҷ��͸��ͻ���
		}
		//���ܿͻ�����Ϣ����ӡ������̨
		void printMsgFromClient(std::ostream & os){
			char buff[1024 + 1] = { 0 };
			int len = 0;
			do{
				int len = recv(this->client, buff, 1024, 0);
				//�����쳣������ʵ�ʿ������ϸ�£���ֻ�����˴�ӡ����
				if (len == SOCKET_ERROR){
					os << "��ͻ��˽������ݴ���ʱ���ִ��󣬴������Ϊ��" << WSAGetLastError() << std::endl;
				}
				else if (len == 0){ //���len == 0 ��ʾ�����ж�
					os << "��ͻ��˵������ж�...." << std::endl;
					break;
				}
			} while (true);
		}

		//���������еõ������ַ��������ͻ���socket
		void sendMsg2Client(std::istream & is){
			char buff[1024 + 1] = { 0 };
			while (!is.eof()){
				is >> buff;
				if (SOCKET_ERROR == (this->client, buff, strlen(buff), 0)){ //�������ط��͵��ֽ�����ʧ�ܻ�����getlasterror
					std::cout << "��ͻ��˽������ݴ���ʱ���ִ��󣬴������Ϊ��" << WSAGetLastError() << std::endl;
				}
			}
		}
	};
	std::vector<ClientSocket *> clientList;

	//�������м����clientList����ֹ����й¶
	~ServerSocket(){
		closesocket(this->so); //�رշ������˵�socket
		clientList.clear(); //���
		clientList.swap(std::vector<ClientSocket *>()); //���ø��ն��󽻻������ڴ档��Ϊ�����clear��Ȼ����ˣ�������������Щ�ڴ滹�ǿ�ʹ�õ�
		WSACleanup(); //�ͷ�socket��Դ
	}

	//�رգ���Ҫ�ֶ�����
	void close(){
		delete serverSocket;
	}
};

ServerSocket * ServerSocket::serverSocket = nullptr; //���о�̬��������Ҫ�������ʼ��

int _tmain(int argc, _TCHAR* argv[])
{
	InitializeCriticalSection(&cs); //��ʼ���ٽ�������
	ServerSocket serverSocket = ServerSocket::getInstance();
	serverSocket.openClient();
	return 0;
}

