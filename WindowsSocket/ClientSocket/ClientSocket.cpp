// ClientSocket.cpp : �������̨Ӧ�ó������ڵ㡣
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
	HANDLE handles[2]; //���߳̾��  д�߳̾��
	static void initSocket(){
		WSADATA wsaData;
		if (0 != WSAStartup(MAKEWORD(2, 2), &wsaData)){
			cout << "socket dll ��ʼ��ʧ��" << endl;
			exit(-1);
		}
	}
	ClientSocket(){
		initSocket();
		s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (-1 == s){
			cout << "����socket�����������Ϊ�� " << WSAGetLastError() << endl;
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
				cout << "���ӷ�����ʧ�ܣ��ȴ���������..." << endl;
			}
			else if (0 == connectRes){
				break;
			}
		} while (true);
		cout << "���ӷ������ɹ�..." << endl;
	}

	//���ܶ���Ϣ����ӡ������̨
	static unsigned int __stdcall recvMsg(void * parameters){
		//�������
		ClientSocket * p = (ClientSocket*)parameters;

		//��ȡ����ӡ
		char buff[1024 + 1] = { 0 };
		int len = 0;
		do{
			int len = recv(p->s, buff, 1024, 0);
			//�����쳣������ʵ�ʿ������ϸ�£���ֻ�����˴�ӡ����
			if (len == SOCKET_ERROR){
				cout << "��������������ݴ���ʱ���ִ��󣬴������Ϊ��" << WSAGetLastError() << std::endl;
				break;
			}
			else if (len <= 0){ //���len <= 0 ��ʾ�����ж�
				cout << "��������������ж�...." << std::endl;
				break;
			}
			buff[len] = '\0'; ///ʵ�ʷ�����Ҫ�Լ�׷�����\0
			cout << buff;
		} while (true);

		//_endthreadex������ǰ�̣߳������Ƿ���ֵ
		//msdn������������������_endthreadex,����ͬʱ����֧������return֮����Զ�����endthreadex��
		//����Լ���������ᵼ�º���������ֹ���ᵼ���߳�������ִ�п��ܲ���й¶����������ǲ����߳��ڲ�����endthreadex���ú����������ء�
		//_endthreadex(0);
		return 0;
	}

	//������Ϣ
	static unsigned int __stdcall sendMsg(void * parameters){
		//�������
		ClientSocket * p = (ClientSocket*)parameters;

		//���벢����
		char buff[1024 + 1] = { 0 };
		while (true){
			int i = 0;
			for (i = 0; i < 1024; ++i){
				cin.get(buff[i]);
				if ('\n' == buff[i]){ //�����˻��У�ֱ�ӷ�����Ϣ
					i++;
					break;
				}
			}
			buff[i] = '\0';
			if (SOCKET_ERROR == send(p->s, buff, strlen(buff), 0)){ //�������ط��͵��ֽ�����ʧ�ܻ�����getlasterror
				std::cout << "��������������ݴ���ʱ���ִ��󣬴������Ϊ��" << WSAGetLastError() << std::endl;
				break;
			}
		}
		return 0;
	}

	void run(){
		//��һ���̷߳�����Ϣ
		if (nullptr == (handles[1] = (HANDLE)_beginthreadex(nullptr, 0, &ClientSocket::sendMsg, this, 0, nullptr))){
			std::cout << "���������߳�ʧ�ܣ��������" << GetLastError() << endl;
		}
		//��һ���߳̽�����Ϣ
		if(nullptr == (handles[0] = (HANDLE)_beginthreadex(nullptr, 0, &ClientSocket::recvMsg, this, 0, nullptr))){
			std::cout << "����д���߳�ʧ�ܣ��������" << GetLastError() << endl;
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

	//waitformultipleobjects ������������ʾ�Ƿ�ȴ����С������true��ʾ�ȴ��������߳̽�����
	//�����false����һ��������ֹͣ���̵߳ĵȴ����ҷ���ֵ��ȥwait_object_0�õ����Ǵ������¼�����Сhandles����Ľűꡣ�������ж��ͬʱ������
	DWORD dwWait = WaitForMultipleObjects(2, cs.handles, FALSE, INFINITE);

	//�ر����߳̾��
	for (int i = 0; i < 2; ++i){
		CloseHandle(cs.handles[i]);
	}

	//�������һ���̵߳��³��������
	switch (dwWait - WAIT_OBJECT_0)
	{
	case 0:
		cout << "�ͻ��˽�����Ϣ�߳�������ֹ�����򼴽�һͬ����..." << endl;
		break;
	case 1:
		cout << "�ͻ��˷�����Ϣ�߳�������ֹ�����򼴽�һͬ����..." << endl;
		break;
	default:
		cout << "�ͻ�������������ӳ��ֲ���Ԥ�����⣬���򼴽���ֹ..." << endl;
		break;
	}
	return 0;
}

