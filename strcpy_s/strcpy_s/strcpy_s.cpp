// strcpy_s.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <string.h>
#include <iostream>
using namespace std;

//������������strcpy_s���ֳ���������󣬹ʽ��в���


int _tmain(int argc, _TCHAR* argv[])
{
	char str[1000] = {1};
	
	//����� ԭ���ǵڶ�������ָʾ�����ṩ�Ļ������Ĵ�С  ���д��strlen("�Ҳ�")  ��ô�������ڲ�ʵ�ֻᷢ��"�Ҳ�"���һ���ַ�'\0'�Ų������Ի��׳�����
	//strcpy_s(str, strlen("�Ҳ�"), "�Ҳ�");

	//����
	strcpy_s(str, strlen("�Ҳ�") + 1, "�Ҳ�");
	cout << str << endl;
	return 0;
}

