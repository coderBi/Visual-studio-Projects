// Regex.cpp : �������̨Ӧ�ó������ڵ㡣
//


/*****************************C++����ʹ����ϰ*********************************/

#include "stdafx.h"
#include <regex>
#include <iostream>
using namespace std;


int _tmain(int argc, _TCHAR* argv[])
{
	/***************ȫƥ��֮regex_match(string,regex)�ڶ���������regex����  match������true ���򷵻�false*****************************/
	regex r1("get(.*)");
	string s1 = "get\ngetsss";
	//���false  ͨ�����Է���regex_match����βȫƥ��
	cout << boolalpha << regex_match(s1,r1) << endl;

	/*********************regex_match ��ȡƥ����  �е��ӱ��ʽ*************************/
	///�洢�Ľ���� index=0��ȫƥ����  1���ӱ��ʽ1���
	//��ʽ1 
	cmatch cm; //��������ƥ��Ľ����ÿһ�����������char* cmath�е�c��char*
	regex_match(s1.c_str(), cm, r1);
	for (unsigned i = 0; i < cm.size(); ++i)
		cout << cm[i] << endl;
	cout << endl;
	//��ʽ2
	smatch sm; //��������ƥ��Ľ����ÿһ�����������string smath�е�s��string
	regex_match(s1, sm, r1);
	for (unsigned i = 0; i < sm.size(); ++i)
		cout << sm[i] << endl;
	cout << endl;

	/***************����ƥ��֮regex_search �ҵ�����true û�ҵ�����false*****************************/
	string toSearch = s1;
	while (regex_search(toSearch, sm, r1)){
		//һ��ƥ��Ľ���ŵ�һ��sm��  sm[0]��������ӱ��ʽ���
		for (auto it = sm.begin(); it != sm.end(); ++it){
			//���һ��ƥ����
			cout << (*it) << " ";
		}
		cout << endl;
		//���������ַ��� ��ʣ�µ��ַ�������toSearch��������
		toSearch = sm.suffix().str();
	}

	//////////ʵ��һ������ͷ����
	const char * pattern = "(GET|POST)\\s+(\\S+)";
	string str = "GET www.baidu.com \nPOST content-type:skkk\n";
	while (regex_search(str, sm, regex(pattern))){
		//һ��ƥ��Ľ���ŵ�һ��sm��  sm[0]��������ӱ��ʽ���
		for (auto it = sm.begin(); it != sm.end(); ++it){
			//���һ��ƥ����
			cout << (*it) << " ";
		}
		cout << endl;
		//���������ַ��� ��ʣ�µ��ַ�������toSearch��������
		str = sm.suffix().str();
	}
	cout << endl;

	/***************regex_replace �����滻֮����ַ���*****************************/
	const char * mode = "(GET|POST)\\s+(\\S+)";
	str = "GET www.baidu.com \nPOST content-type:skkk\n";
	string rp = regex_replace(str, regex(mode), "replaced");
	cout << "before: " << str << endl << "after: " << rp << endl;
	cout << endl;

	//����ȡ��ǩ��������
	mode = "<(.*?)>(.*?)</\\1>";
	str = "<�ʱ�>51800</�ʱ�><a>hello</a>";
	while (regex_search(str, sm, regex(mode))){
		//һ��ƥ��Ľ���ŵ�һ��sm��  sm[0]��������ӱ��ʽ���
		for (auto it = sm.begin(); it != sm.end(); ++it){
			//���һ��ƥ����
			cout << (*it) << " ";
		}
		cout << endl;
		//���������ַ��� ��ʣ�µ��ַ�������toSearch��������
		str = sm.suffix().str();
	}

	getchar();
	return 0;
}

