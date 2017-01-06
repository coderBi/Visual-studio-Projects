#pragma once
#include "stdafx.h"
using namespace Gdiplus;

class GdiString
{
private:
	TCHAR *lpszBuffer; //�ַ���ָ��
	Gdiplus::RectF rtfStringDes;  //Ŀ���豸�ϵ�λ��
	LOGFONT logFont; //�߼����壬����ָ�������ַ���������
	//Gdiplus::SolidBrush gdiBrus(Gdiplus::Color(Gdiplus::Color::White));
	Gdiplus::Color color;
public:
	GdiString();
	//�����ı�����
	void SetStringContent(TCHAR *szStr){
		int length = lstrlen(szStr);
		lpszBuffer = new TCHAR[length + 1];
		lstrcpy(lpszBuffer, szStr);
	}
	//���û��Ƶ�λ��
	void SetStringPos(RectF rtf){
		this->rtfStringDes = rtf;
	}
	//�����߼�����
	void SetLogFont(LOGFONT logFont){
		this->logFont = logFont;
	}
	//������ɫ
	void SetColor(ARGB argb){
		color.SetValue(argb);
	}
	//�����ַ���
	void DrawString(HDC hdc, StringFormat *strFormat);
	//�õ��ַ���ָ��
	TCHAR* GetStringContent(){
		return lpszBuffer;
	}
	~GdiString();
public:
	static std::map<UINT, GdiString> mapGdiString; //��������ù����ַ�����Դ
};

