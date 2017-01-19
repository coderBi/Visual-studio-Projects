// GetSystemOperationInfo.cpp : �������̨Ӧ�ó������ڵ㡣
//
/*
	�������ȡ����ӡһЩ�����Ĳ���ϵͳ��ص�����
	�������ȡһ��������������Լ���ǰʹ�õ��û�����
*/

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <stdio.h>
//����GetVersionEx����İ�ȫ���
#pragma warning(disable: 4996)

void getSystemInfo(){
	PSTR pstrComputerName = (PSTR)calloc(MAXBYTE, sizeof(char)), pstrUserName = (PSTR)calloc(MAXBYTE, sizeof(char)); //calloc���Զ���ʼλȫ0
	DWORD nSize = MAXBYTE;

	//�õ����������, �����ɹ����ط�0 ʧ�ܷ���0 ���һ�����getlasterror
	if (GetComputerName(pstrComputerName, &nSize) == 0 && GetLastError() == ERROR_BUFFER_OVERFLOW){
		
		realloc(pstrComputerName, nSize * sizeof(char) + 1);
		GetComputerName(pstrComputerName, &nSize);
	}
	std::cout << "computer name : " << pstrComputerName << std::endl;
	free(pstrComputerName); 
	pstrComputerName = nullptr;

	//�õ���ǰ�û���
	nSize = MAXBYTE; //������Ҫ��nSize���ã���Ϊ����inout���͵ģ�����GetComputerName���ܸ�����ֵΪ��С��������Ų��¡�
	if (GetUserName(pstrUserName, &nSize) == 0 && GetLastError() == ERROR_BUFFER_OVERFLOW){
		realloc(pstrUserName, nSize * sizeof(char) + 1);
		GetUserName(pstrUserName, &nSize);
	}
	std::cout << "user name : " << pstrUserName << std::endl;
	free(pstrUserName);
	pstrUserName = nullptr;

	//���߲���ϵͳ���
	OSVERSIONINFO osVersionInfo;
	osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO); //���ýṹ��Ĵ�С
	GetVersionEx(&osVersionInfo);
	//dwPlatformId����ȡ������ֵ�� VER_PLATFORM_WIN32_NT��ʾwindowsNT VER_PLATFORM_WIN32_WINDOWS��ʾwin98��win95�������
	//win95,dwMinorVersionΪ0�� �����win98dwMinorVersion����0�� VER_PLATFORM_WIN32s ��ʾwindows3.1�� 
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
	
	//���������
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	printf("���봦�����ܹ�(���ص��Ǵ���������)��%u\n", sysInfo.wProcessorArchitecture); //������cpu�ܹ���intel amd��
	printf("������������%u\n", sysInfo.dwNumberOfProcessors);
	printf("����������%u\n", sysInfo.wProcessorLevel);
	printf("����������: %u\n", sysInfo.dwProcessorType);
	printf("�������汾��%u\n", sysInfo.wProcessorRevision);
	printf("���Ѱַ��Ԫ��%u\n", sysInfo.lpMaximumApplicationAddress);
	printf("��СѰַ��Ԫ��%u\n", sysInfo.lpMinimumApplicationAddress);
	
}

int _tmain(int argc, _TCHAR* argv[])
{
	//����������Լ��ĵ��ԵĽṹ��compute name :bww  username: ��   
	//ϵͳ��plateformid��VER_PLATEFORM_WIN32_NT, ���汾��6 �ΰ汾��1
	getSystemInfo();
	system("pause");
	return 0;
}

