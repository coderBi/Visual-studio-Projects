// GetProcessMemoryInfo.cpp : �������̨Ӧ�ó������ڵ㡣
//

/*
	��ȡ��ǰ������ڴ�ʹ�����
*/
#include "stdafx.h"
#include <Windows.h>
#include <process.h>
#include <iostream>
#pragma comment(lib, "Psapi.lib")
#include <Psapi.h>

int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE handle = GetCurrentProcess();
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
	std::cout << "��ǰ����ʹ���ڴ棺" << pmc.WorkingSetSize << std::endl; //��ǰ����ʹ�õ��ڴ�
	std::cout << "ȱҳ�жϴ�����" << pmc.PageFaultCount << std::endl;
	std::cout << "ʹ���ڴ�߷�ֵ��" << pmc.PeakWorkingSetSize << std::endl;
	std::cout << "ʹ�÷�ҳ�ļ��߷�ֵ��" << pmc.PeakPagefileUsage << std::endl;
	std::cout << "��ǰʹ��ҳ�滺��أ�" << pmc.QuotaPagedPoolUsage << std::endl;
	std::cout << "ʹ��ҳ�滺��ظ߷壺" << pmc.QuotaPeakPagedPoolUsage << std::endl;
	std::cout << "ʹ�÷Ƿ�ҳ����أ�" << pmc.QuotaNonPagedPoolUsage << std::endl;
	std::cout << "ʹ�÷Ƿ�ҳ����ظ߷壺" << pmc.QuotaPeakNonPagedPoolUsage << std::endl;

	system("pause");
	return 0;
}

