#pragma once
#include "stdafx.h"
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

/**
	Socket�����װ�࣬�����ṩWinSocket�ļ򵥲�����
	@auth edwin
	@since 2017��1��7��20:12:57
	@version v1.0
*/
class WrapperSocket
{
private:
	SOCKET winSocket;
	WrapperSocket();
public:
	static WrapperSocket & getWrapperSocket();
	static WrapperSocket & getWrapperSocket(SOCKET winSocket);
	virtual ~WrapperSocket();
};

