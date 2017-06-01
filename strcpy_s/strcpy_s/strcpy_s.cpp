// strcpy_s.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <string.h>
#include <iostream>
using namespace std;

//看到有人是用strcpy_s出现程序崩溃现象，故进行测试


int _tmain(int argc, _TCHAR* argv[])
{
	char str[1000] = {1};
	
	//会崩溃 原因是第二个参数指示的是提供的缓冲区的大小  如果写成strlen("我擦")  那么函数的内部实现会发现"我擦"最后一个字符'\0'放不下所以会抛出错误
	//strcpy_s(str, strlen("我擦"), "我擦");

	//正常
	strcpy_s(str, strlen("我擦") + 1, "我擦");
	cout << str << endl;
	return 0;
}

