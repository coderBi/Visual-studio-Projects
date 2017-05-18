// Manacher.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>

/*	Manacher�㷨�������ǽ��������ַ����Ӵ���Ѱ������
	1,����˼·���Ժ�����д���� n*n �ĸ��Ӷȵ��㷨��Ҳ�����ȼ���ÿ�����ַ��м���һ�����ַ���Ȼ�����ÿһ���ַ�����������ַ�
	Ϊ���ĵ����ߵĶԳ���չ��
	2,manacher�㷨�ĳ������kmp�㷨���ƣ�Ҫ�������֮ǰ�Ѿ��жϹ���ǰ���i-1���ݵĴ�������
		a�����ַ��� abc ��ÿһ���ַ����������һ���ַ����в����ܴ��ڵ��ַ� �������Ϊ #����Ӻ���ַ����� #a#b#c#
		b����indexΪ i ���ַ� ������ i���ұߵ���Զ���Թ���i�γɻ����ַ������ַ�λ��  ��i�ľ������ R[i] ����ͨ���۲�����
		���#֮������鳤���� 2*n+1��ô���R[i]�������iΪ�Գ��������ĵĳ���
		c������ǰ��i��λ�ã���������Դﵽ����Զ�����ұߵ�λ��index�� maxRight �ﵽ���ұߵ�maxRight��Ӧ�ĶԳ����index�� id
		d��ǰ i��Ԫ�ص�����ĳ���  R[0] �� R[i-1]�Ѿ����  �������ǿ��Դﵽ�����ұߵ�λ���� maxRight ��ӦindexΪid�� ����������
		�����i+1Ҳ���ǽű�Ϊi ��Ԫ�أ�
			---���� i < maxRight ��ô�����ҵ� i����id�ԳƵ�λ�� j ���j�Ļ������ǰ���Ѿ�̽���ˣ����j���ɵĻ����������id���ɵ�
			���ĵ����棬��ôi���ĵĳ��ȴ��ڵ���j�ģ��������id���߶Գ��Ծ����ģ���ʱֱ�ӴӶԳƵ�λ�ý���Ѱ�ҡ����j���ɵĻ��ĵ�
			����߳�����maxRight�ĶԳ�λ�ã���ô����ȷ������i��maxRight��һ���ֵĶԳ��� ������ֱ�Ӵ�maxRight��ʼ������չ��
			---���i > maxRight ��ôR[i] ��0��ʼ��չ
			---��������������Խ��кϲ��� ��ȡ R[i] = max(0,maxRight - i) Ȼ��ȡ R[i] = min(R[j],R[i]) ����������Ͻ��� R[i]����
			����չ��һֱ���ȶԳƵĵط� �ж��Ƿ���Ҫ���� maxRight �� id
*/

int min(int x, int y){
	return ((x) > (y) ? (y) : (x));
}

int max(int x, int y) {
	return ((x) > (y) ? (x) : (y));
}

/*
	��ȡ������  ע�����������ַ����� #  ����ԭ�����ַ����в�����#
@param cosnt std::string& str �����ҵ��ַ���
@param std::string& substr ���ܷ��ص����ַ���
@param int* fIndex ���ܵ����ַ�����λ�� ���û���ҵ��κ�����ֵΪ -1
@return bool ��������ڲ�ִ�г����� false ���򷵻�true
*/
bool getMaxPalindrome(__in const std::string & str, __out std::string & substr, __out int* fIndex){
	size_t sLen = str.length();
	//�����ַ���Ϊ�� ������������������ַ�#
	if (!sLen || str.find('#') != std::string::npos){
		substr = "";
		(*fIndex) = -1;
		return true;
	}

	//�Ƚ������
	std::string in = "";
	for (unsigned i = 0; i < sLen; ++i){
		in += "#";
		in += str[i];
	}
	in += "#";
	//ɨ��
	size_t iLen = 2 * sLen + 1, maxR = 0;
	std::vector<size_t> R(iLen, 0);  //��뾶 ����뾶�����ұߵ�index��Գ����index�Ĳ�ֵ
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
	//��ȡ�����Ӵ�
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

