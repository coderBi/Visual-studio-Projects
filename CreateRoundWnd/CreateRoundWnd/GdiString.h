#pragma once
#include "stdafx.h"
using namespace Gdiplus;

class GdiString
{
private:
	TCHAR *lpszBuffer; //字符串指针
	Gdiplus::RectF rtfStringDes;  //目标设备上的位置
	LOGFONT logFont; //逻辑字体，用来指定绘制字符串的字体
	//Gdiplus::SolidBrush gdiBrus(Gdiplus::Color(Gdiplus::Color::White));
	Gdiplus::Color color;
public:
	GdiString();
	//设置文本内容
	void SetStringContent(TCHAR *szStr){
		int length = lstrlen(szStr);
		lpszBuffer = new TCHAR[length + 1];
		lstrcpy(lpszBuffer, szStr);
	}
	//设置绘制的位置
	void SetStringPos(RectF rtf){
		this->rtfStringDes = rtf;
	}
	//设置逻辑字体
	void SetLogFont(LOGFONT logFont){
		this->logFont = logFont;
	}
	//设置颜色
	void SetColor(ARGB argb){
		color.SetValue(argb);
	}
	//绘制字符串
	void DrawString(HDC hdc, StringFormat *strFormat);
	//得到字符串指针
	TCHAR* GetStringContent(){
		return lpszBuffer;
	}
	~GdiString();
public:
	static std::map<UINT, GdiString> mapGdiString; //存放所有用过的字符串资源
};

