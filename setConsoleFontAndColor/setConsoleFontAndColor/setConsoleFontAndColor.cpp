// setConsoleFontAndColor.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>
#include <iostream>
using namespace std;

/*
设置控制台字体，通过测试发现，这个设置是全局的，并不能单独设置某一段文字的大小。
@param int y 字体的高度
@return BOOL 成功返回非0  失败返回0
*/
BOOL setFontSize(int y)
{
	CONSOLE_FONT_INFOEX info = { 0 };
	info.cbSize = sizeof(info);
	info.dwFontSize.Y = y;
	info.dwFontSize.X = 0;  //其中 X一般是设置为自动，否则可能由于同时设置x y而不满足内部规律（比例）导致设置失效
	return SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), NULL, &info);
}


BOOL setStdTextColor(WORD color){
	//SetConsoleTextAttribute 函数可以设置控制台文字的前景色与背景色
	return SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

int _tmain(int argc, _TCHAR* argv[])
{
	//设置蓝底红色
	setStdTextColor(FOREGROUND_INTENSITY | FOREGROUND_RED | BACKGROUND_BLUE);
	printf("\nthis is red font");
	
	setFontSize(20);
	//设置黑底白色
	setStdTextColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	printf("\nthis is white font");

	//设置混合色
	setStdTextColor(199);
	printf("\nthis is mixed font");
	getchar();
	return 0;
}

