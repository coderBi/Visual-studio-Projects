// GetProcessMemoryInfo.cpp : 定义控制台应用程序的入口点。
//

/*
	获取当前程序的内存使用情况
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
	std::cout << "当前程序使用内存：" << pmc.WorkingSetSize << std::endl; //当前程序使用的内存
	std::cout << "缺页中断次数：" << pmc.PageFaultCount << std::endl;
	std::cout << "使用内存高峰值：" << pmc.PeakWorkingSetSize << std::endl;
	std::cout << "使用分页文件高峰值：" << pmc.PeakPagefileUsage << std::endl;
	std::cout << "当前使用页面缓存池：" << pmc.QuotaPagedPoolUsage << std::endl;
	std::cout << "使用页面缓存池高峰：" << pmc.QuotaPeakPagedPoolUsage << std::endl;
	std::cout << "使用非分页缓存池：" << pmc.QuotaNonPagedPoolUsage << std::endl;
	std::cout << "使用非分页缓存池高峰：" << pmc.QuotaPeakNonPagedPoolUsage << std::endl;

	system("pause");
	return 0;
}

