#pragma once
#include "stdafx.h"
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

/**
	Socket对象包装类，里面提供WinSocket的简单操作。
	@auth edwin
	@since 2017年1月7日20:12:57
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

