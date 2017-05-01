// setConsoleFontAndColor.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <windows.h>
#include <iostream>
using namespace std;

/*
���ÿ���̨���壬ͨ�����Է��֣����������ȫ�ֵģ������ܵ�������ĳһ�����ֵĴ�С��
@param int y ����ĸ߶�
@return BOOL �ɹ����ط�0  ʧ�ܷ���0
*/
BOOL setFontSize(int y)
{
	CONSOLE_FONT_INFOEX info = { 0 };
	info.cbSize = sizeof(info);
	info.dwFontSize.Y = y;
	info.dwFontSize.X = 0;  //���� Xһ��������Ϊ�Զ��������������ͬʱ����x y���������ڲ����ɣ���������������ʧЧ
	return SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), NULL, &info);
}


BOOL setStdTextColor(WORD color){
	//SetConsoleTextAttribute �����������ÿ���̨���ֵ�ǰ��ɫ�뱳��ɫ
	return SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

int _tmain(int argc, _TCHAR* argv[])
{
	//�������׺�ɫ
	setStdTextColor(FOREGROUND_INTENSITY | FOREGROUND_RED | BACKGROUND_BLUE);
	printf("\nthis is red font");
	
	setFontSize(20);
	//���úڵװ�ɫ
	setStdTextColor(FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	printf("\nthis is white font");

	//���û��ɫ
	setStdTextColor(199);
	printf("\nthis is mixed font");
	getchar();
	return 0;
}

