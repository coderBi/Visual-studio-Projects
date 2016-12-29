// CopySelfExe2WindowsAndSystemDirectory.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>

PSTR g_AppName = "CopySelfExe2WindowsAndSystemDirectory";

//调整目录结构 末尾的 "\"
PSTR adjustDirectory(char pstr[]){
	unsigned int len = strlen(pstr);
	if (len <= 1) //目录结构至少形如： c: 也就是至少传入的参数len要 >=2
		return nullptr;

	if (pstr[len - 1] != '\\'){
		strcat_s(pstr, MAX_PATH, "\\\\");
	}
	else if (pstr[len - 2] != '\\'){
		strcat_s(pstr, MAX_PATH, "\\");
	}
	return pstr;
}

void copySelf(){
	//保存自身程序路径
	char szSelfPath[MAX_PATH] = { 0 };
	//保存windows目录
	char szWindowsPath[MAX_PATH] = { 0 };
	//保存system目录
	char szSystemPath[MAX_PATH] = { 0 };
	//保存temp目录
	char szTempPath[MAX_PATH] = { 0 };

	GetModuleFileName(NULL, szSelfPath, MAX_PATH);
	GetWindowsDirectory(szWindowsPath, MAX_PATH);
	GetSystemDirectory(szSystemPath, MAX_PATH);
	GetTempPath(MAX_PATH, szTempPath);

	//调整目录结尾的 "\\"
	adjustDirectory(szWindowsPath);
	adjustDirectory(szSystemPath);
	adjustDirectory(szTempPath);

	//打印获取到的四个变量
	std::cout << "获取到的4个变量的内容为 :" << std::endl;
	std::cout << "szSelfPath : " << szSelfPath << std::endl;
	std::cout << "szWindowsPath : " << szWindowsPath << std::endl;
	std::cout << "szSystemPath : " << szSystemPath << std::endl;
	std::cout << "szTempPath : " << szTempPath << std::endl;

	//利用CopyFile函数 进行文件移动， 如果成功返回非 0， 如果失败返回 0
	strcat_s(szWindowsPath, g_AppName);
	strcat_s(szWindowsPath, ".exe");
	//CopyFile 参数分别是： 旧的文件名  新的文件名  是否判断覆盖（如果设置成true并且新年目录存在相应文件就报错，如果设置成false，即使新目录有相应文件也会不加甄别的覆盖）
	if (0 == CopyFile(szSelfPath, szWindowsPath, true)){
		std::cout << "拷贝到WindowsPath失败" << std::endl;
	}

	//拷贝到system目录
	strcat_s(szSystemPath, g_AppName);
	strcat_s(szSystemPath, ".exe");
	if (0 == CopyFile(szSelfPath, szSystemPath, true)){
		std::cout << "拷贝到system目录失败" << std::endl;
	}

	//拷贝到system目录
	strcat_s(szTempPath, g_AppName);
	strcat_s(szTempPath, ".exe");
	if (0 == CopyFile(szSelfPath, szTempPath, true)){
		std::cout << "拷贝到temp目录失败" << std::endl;
	}

	//查看完成结果，进行拷贝的文件删除
	std::cout << "请确认是否删除拷贝过去的三个文件(y: yes n: no) : ";
	if ('y' == getchar()){
		remove(szWindowsPath);
		remove(szSystemPath);
		remove(szTempPath);
	}
	return;
}
int _tmain(int argc, _TCHAR* argv[])
{
	//通过对三个目录的观察，可以发现，可以将本程序拷贝到windows system 跟 temp三个目录中。
	copySelf();
	system("pause");
	return 0;
}

