// Regex.cpp : 定义控制台应用程序的入口点。
//


/*****************************C++正则使用练习*********************************/

#include "stdafx.h"
#include <regex>
#include <iostream>
using namespace std;


int _tmain(int argc, _TCHAR* argv[])
{
	/***************全匹配之regex_match(string,regex)第二个参数是regex对象  match到返回true 否则返回false*****************************/
	regex r1("get(.*)");
	string s1 = "get\ngetsss";
	//输出false  通过测试发现regex_match是首尾全匹配
	cout << boolalpha << regex_match(s1,r1) << endl;

	/*********************regex_match 获取匹配结果  中的子表达式*************************/
	///存储的结果中 index=0是全匹配结果  1是子表达式1结果
	//方式1 
	cmatch cm; //用来保存匹配的结果，每一个结果项存的是char* cmath中的c是char*
	regex_match(s1.c_str(), cm, r1);
	for (unsigned i = 0; i < cm.size(); ++i)
		cout << cm[i] << endl;
	cout << endl;
	//方式2
	smatch sm; //用来保存匹配的结果，每一个结果项存的是string smath中的s是string
	regex_match(s1, sm, r1);
	for (unsigned i = 0; i < sm.size(); ++i)
		cout << sm[i] << endl;
	cout << endl;

	/***************依次匹配之regex_search 找到返回true 没找到返回false*****************************/
	string toSearch = s1;
	while (regex_search(toSearch, sm, r1)){
		//一次匹配的结果放到一个sm中  sm[0]后面的是子表达式结果
		for (auto it = sm.begin(); it != sm.end(); ++it){
			//输出一个匹配结果
			cout << (*it) << " ";
		}
		cout << endl;
		//重置搜索字符串 将剩下的字符串赋给toSearch接着搜索
		toSearch = sm.suffix().str();
	}

	//////////实现一个请求头过滤
	const char * pattern = "(GET|POST)\\s+(\\S+)";
	string str = "GET www.baidu.com \nPOST content-type:skkk\n";
	while (regex_search(str, sm, regex(pattern))){
		//一次匹配的结果放到一个sm中  sm[0]后面的是子表达式结果
		for (auto it = sm.begin(); it != sm.end(); ++it){
			//输出一个匹配结果
			cout << (*it) << " ";
		}
		cout << endl;
		//重置搜索字符串 将剩下的字符串赋给toSearch接着搜索
		str = sm.suffix().str();
	}
	cout << endl;

	/***************regex_replace 返回替换之后的字符串*****************************/
	const char * mode = "(GET|POST)\\s+(\\S+)";
	str = "GET www.baidu.com \nPOST content-type:skkk\n";
	string rp = regex_replace(str, regex(mode), "replaced");
	cout << "before: " << str << endl << "after: " << rp << endl;
	cout << endl;

	//测试取标签里面内容
	mode = "<(.*?)>(.*?)</\\1>";
	str = "<邮编>51800</邮编><a>hello</a>";
	while (regex_search(str, sm, regex(mode))){
		//一次匹配的结果放到一个sm中  sm[0]后面的是子表达式结果
		for (auto it = sm.begin(); it != sm.end(); ++it){
			//输出一个匹配结果
			cout << (*it) << " ";
		}
		cout << endl;
		//重置搜索字符串 将剩下的字符串赋给toSearch接着搜索
		str = sm.suffix().str();
	}

	getchar();
	return 0;
}

