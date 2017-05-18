// Manacher.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>

/*	Manacher算法，本身是解决最长回文字符串子串的寻找问题
	1,正常思路可以很容易写出从 n*n 的复杂度的算法，也就是先假设每两个字符中间有一个空字符，然后对于每一个字符进行以这个字符
	为中心到两边的对称扩展。
	2,manacher算法的出发点和kmp算法类似，要充分利用之前已经判断过的前面的i-1数据的处理结果。
		a）在字符串 abc 中每一个字符的两边添加一个字符串中不可能存在的字符 这里假设为 #。添加后的字符串是 #a#b#c#
		b）对index为 i 的字符 假设在 i的右边的最远可以关于i形成回文字符串的字符位置  与i的距离差是 R[i] 可以通过观察由于
		添加#之后的数组长度是 2*n+1那么这个R[i]本身就是i为对称轴的最长回文的长度
		c）对于前面i个位置，假设其可以达到的最远的最右边的位置index是 maxRight 达到最右边的maxRight对应的对称轴的index是 id
		d）前 i个元素的最长回文长度  R[0] 到 R[i-1]已经求出  并且他们可以达到的最右边的位置是 maxRight 对应index为id。 接下来就是
		处理第i+1也就是脚标为i 的元素：
			---假设 i < maxRight 那么可以找到 i关于id对称的位置 j 这个j的回文情况前面已经探明了，如果j构成的回文最左边在id构成的
			回文的里面，那么i回文的长度大于等于j的，这个由于id两边对称性决定的，这时直接从对称的位置进行寻找。如果j构成的回文的
			最左边超出了maxRight的对称位置，那么可以确定的是i到maxRight这一部分的对称性 接下来直接从maxRight开始向右扩展。
			---如果i > maxRight 那么R[i] 从0开始扩展
			---上面两种情况可以进行合并： 先取 R[i] = max(0,maxRight - i) 然后取 R[i] = min(R[j],R[i]) 在这个基础上进行 R[i]的向
			外扩展，一直到比对称的地方 判断是否需要重置 maxRight 和 id
*/

int min(int x, int y){
	return ((x) > (y) ? (y) : (x));
}

int max(int x, int y) {
	return ((x) > (y) ? (x) : (y));
}

/*
	获取最大回文  注意这里的填充字符串是 #  所以原来的字符串中不能有#
@param cosnt std::string& str 待查找的字符串
@param std::string& substr 接受返回的子字符串
@param int* fIndex 接受的子字符串的位置 如果没有找到任何内容值为 -1
@return bool 如果函数内部执行出错返回 false 否则返回true
*/
bool getMaxPalindrome(__in const std::string & str, __out std::string & substr, __out int* fIndex){
	size_t sLen = str.length();
	//传入字符串为空 或者是里面存在特殊字符#
	if (!sLen || str.find('#') != std::string::npos){
		substr = "";
		(*fIndex) = -1;
		return true;
	}

	//先进行填充
	std::string in = "";
	for (unsigned i = 0; i < sLen; ++i){
		in += "#";
		in += str[i];
	}
	in += "#";
	//扫描
	size_t iLen = 2 * sLen + 1, maxR = 0;
	std::vector<size_t> R(iLen, 0);  //存半径 这个半径是最右边的index与对称轴的index的差值
	int maxRight = 0, id = 0;
	for (unsigned i = 1; i < iLen; ++i){
		R[i] = max(0, maxRight - i);
		R[i] = 0 == R[i] ? 0 : min(R[2 * id - i], R[i]);
		for (unsigned j = i + (R[i] > 0 ? R[i] : 1); j < iLen && 2 * i >= j; j++){
			if (in[j] != in[2 * i - j]){
				if (j > maxRight){
					maxRight = j;
					id = i;
				}
				if (R[i] > maxR)
					maxR = R[i];
				break;
			}
			R[i]++;
		}
	}
	//获取最大的子串
	for (size_t i = 0; i < iLen; ++i){
		if (maxR == R[i]){
			int j = (*fIndex) = (i - R[i]) / 2;
			substr = str.substr(j,R[i]);
			break;
		}
	}
	return true;
}

int _tmain(int argc, _TCHAR* argv[])
{
	std::string str = "helloollxxx";
	std::string result;
	int index = 0;
	getMaxPalindrome(str, result, &index);
	std::cout << "index:" << index << std::endl << "substr:" << result << std::endl;
	system("pause");
	return 0;
}

