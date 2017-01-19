// GetSystemOperationInfo.cpp : 定义控制台应用程序的入口点。
//
/*
	本程序获取并打印一些常见的操作系统相关的属性
	本程序获取一个计算机的名称以及当前使用的用户名称
*/

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <stdio.h>
//忽略GetVersionEx这里的安全检查
#pragma warning(disable: 4996)

void getSystemInfo(){
	PSTR pstrComputerName = (PSTR)calloc(MAXBYTE, sizeof(char)), pstrUserName = (PSTR)calloc(MAXBYTE, sizeof(char)); //calloc会自动初始位全0
	DWORD nSize = MAXBYTE;

	//得到计算机名称, 函数成功返回非0 失败返回0 并且会设置getlasterror
	if (GetComputerName(pstrComputerName, &nSize) == 0 && GetLastError() == ERROR_BUFFER_OVERFLOW){
		
		realloc(pstrComputerName, nSize * sizeof(char) + 1);
		GetComputerName(pstrComputerName, &nSize);
	}
	std::cout << "computer name : " << pstrComputerName << std::endl;
	free(pstrComputerName); 
	pstrComputerName = nullptr;

	//得到当前用户名
	nSize = MAXBYTE; //这里需要将nSize重置，因为它是inout类型的，上面GetComputerName可能给他赋值为很小导致这里放不下。
	if (GetUserName(pstrUserName, &nSize) == 0 && GetLastError() == ERROR_BUFFER_OVERFLOW){
		realloc(pstrUserName, nSize * sizeof(char) + 1);
		GetUserName(pstrUserName, &nSize);
	}
	std::cout << "user name : " << pstrUserName << std::endl;
	free(pstrUserName);
	pstrUserName = nullptr;

	//或者操作系统相关
	OSVERSIONINFO osVersionInfo;
	osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO); //设置结构体的大小
	GetVersionEx(&osVersionInfo);
	//dwPlatformId可以取得三个值： VER_PLATFORM_WIN32_NT表示windowsNT VER_PLATFORM_WIN32_WINDOWS表示win98或win95（如果是
	//win95,dwMinorVersion为0， 如果是win98dwMinorVersion大于0） VER_PLATFORM_WIN32s 表示windows3.1。 
	if (osVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT){
		if (osVersionInfo.dwMajorVersion == 5){
			if (osVersionInfo.dwMinorVersion == 1){ //xp
				std::cout << "System : xp " << osVersionInfo.dwMajorVersion << "." << osVersionInfo.dwMinorVersion << std::endl;
			}
			else if (osVersionInfo.dwMinorVersion == 0){ //win 2000
				std::cout << "System : win 2000 " << osVersionInfo.dwMajorVersion << "." << osVersionInfo.dwMinorVersion << std::endl;
			}
		}
		else if(osVersionInfo.dwMajorVersion > 5) {
			std::cout << "System : above xp  " << osVersionInfo.dwMajorVersion << "." << osVersionInfo.dwMinorVersion << std::endl;
		}
		else {
			std::cout << "System : win 98 " << osVersionInfo.dwMajorVersion << "." << osVersionInfo.dwMinorVersion << std::endl;
		}
	}
	else if (osVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS){
		if (osVersionInfo.dwMajorVersion > 4){
			std::cout << "system : win98 " << osVersionInfo.dwMajorVersion << "." << osVersionInfo.dwMinorVersion << std::endl;
		}
		else if (osVersionInfo.dwMajorVersion == 4){
			if (osVersionInfo.dwMinorVersion == 1){
				std::cout << "system : win98 " << osVersionInfo.dwMajorVersion << "." << osVersionInfo.dwMinorVersion << std::endl;
			}
			else{
				std::cout << "system : win95 " << osVersionInfo.dwMajorVersion << "." << osVersionInfo.dwMinorVersion << std::endl;
			}
		}
		else{
			std::cout << "system : win3.1  " << osVersionInfo.dwMajorVersion << "." << osVersionInfo.dwMinorVersion << std::endl;
		}
	}
	else if (osVersionInfo.dwPlatformId == VER_PLATFORM_WIN32s){
		std::cout << "system : win3.1  " << osVersionInfo.dwMajorVersion << "." << osVersionInfo.dwMinorVersion << std::endl;
	}
	else {
		std::cout << "system : unknown  " << osVersionInfo.dwMajorVersion << "." << osVersionInfo.dwMinorVersion << std::endl;
	}
	
	//处理器情况
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	printf("中央处理器架构(返回的是处理器掩码)：%u\n", sysInfo.wProcessorArchitecture); //主流的cpu架构有intel amd等
	printf("处理器的数量%u\n", sysInfo.dwNumberOfProcessors);
	printf("处理器级别：%u\n", sysInfo.wProcessorLevel);
	printf("处理器类型: %u\n", sysInfo.dwProcessorType);
	printf("处理器版本：%u\n", sysInfo.wProcessorRevision);
	printf("最大寻址单元：%u\n", sysInfo.lpMaximumApplicationAddress);
	printf("最小寻址单元：%u\n", sysInfo.lpMinimumApplicationAddress);
	
}

int _tmain(int argc, _TCHAR* argv[])
{
	//这里测试我自己的电脑的结构是compute name :bww  username: 空   
	//系统的plateformid是VER_PLATEFORM_WIN32_NT, 主版本号6 次版本号1
	getSystemInfo();
	system("pause");
	return 0;
}

