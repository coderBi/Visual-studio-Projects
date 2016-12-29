// CopySelfExe2WindowsAndSystemDirectory.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>

PSTR g_AppName = "CopySelfExe2WindowsAndSystemDirectory";

//����Ŀ¼�ṹ ĩβ�� "\"
PSTR adjustDirectory(char pstr[]){
	unsigned int len = strlen(pstr);
	if (len <= 1) //Ŀ¼�ṹ�������磺 c: Ҳ�������ٴ���Ĳ���lenҪ >=2
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
	//�����������·��
	char szSelfPath[MAX_PATH] = { 0 };
	//����windowsĿ¼
	char szWindowsPath[MAX_PATH] = { 0 };
	//����systemĿ¼
	char szSystemPath[MAX_PATH] = { 0 };
	//����tempĿ¼
	char szTempPath[MAX_PATH] = { 0 };

	GetModuleFileName(NULL, szSelfPath, MAX_PATH);
	GetWindowsDirectory(szWindowsPath, MAX_PATH);
	GetSystemDirectory(szSystemPath, MAX_PATH);
	GetTempPath(MAX_PATH, szTempPath);

	//����Ŀ¼��β�� "\\"
	adjustDirectory(szWindowsPath);
	adjustDirectory(szSystemPath);
	adjustDirectory(szTempPath);

	//��ӡ��ȡ�����ĸ�����
	std::cout << "��ȡ����4������������Ϊ :" << std::endl;
	std::cout << "szSelfPath : " << szSelfPath << std::endl;
	std::cout << "szWindowsPath : " << szWindowsPath << std::endl;
	std::cout << "szSystemPath : " << szSystemPath << std::endl;
	std::cout << "szTempPath : " << szTempPath << std::endl;

	//����CopyFile���� �����ļ��ƶ��� ����ɹ����ط� 0�� ���ʧ�ܷ��� 0
	strcat_s(szWindowsPath, g_AppName);
	strcat_s(szWindowsPath, ".exe");
	//CopyFile �����ֱ��ǣ� �ɵ��ļ���  �µ��ļ���  �Ƿ��жϸ��ǣ�������ó�true��������Ŀ¼������Ӧ�ļ��ͱ���������ó�false����ʹ��Ŀ¼����Ӧ�ļ�Ҳ�᲻�����ĸ��ǣ�
	if (0 == CopyFile(szSelfPath, szWindowsPath, true)){
		std::cout << "������WindowsPathʧ��" << std::endl;
	}

	//������systemĿ¼
	strcat_s(szSystemPath, g_AppName);
	strcat_s(szSystemPath, ".exe");
	if (0 == CopyFile(szSelfPath, szSystemPath, true)){
		std::cout << "������systemĿ¼ʧ��" << std::endl;
	}

	//������systemĿ¼
	strcat_s(szTempPath, g_AppName);
	strcat_s(szTempPath, ".exe");
	if (0 == CopyFile(szSelfPath, szTempPath, true)){
		std::cout << "������tempĿ¼ʧ��" << std::endl;
	}

	//�鿴��ɽ�������п������ļ�ɾ��
	std::cout << "��ȷ���Ƿ�ɾ��������ȥ�������ļ�(y: yes n: no) : ";
	if ('y' == getchar()){
		remove(szWindowsPath);
		remove(szSystemPath);
		remove(szTempPath);
	}
	return;
}
int _tmain(int argc, _TCHAR* argv[])
{
	//ͨ��������Ŀ¼�Ĺ۲죬���Է��֣����Խ������򿽱���windows system �� temp����Ŀ¼�С�
	copySelf();
	system("pause");
	return 0;
}

