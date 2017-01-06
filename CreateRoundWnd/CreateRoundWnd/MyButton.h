#pragma once
#include "MyImage.h"
#include "GdiString.h"

class MyButton :
	public MyImage, public GdiString
{
private:
	int nState; //按钮的状态，0~2 对应不同的按钮背景
	Gdiplus::RectF rtfBtnPos; //按钮的目标位置
	
public:
	//绘制button
	void DrawButton(HDC hdc, StringFormat *strFormat);
	//设置按钮状态
	void SetBtnState(int state){
		nState = state;
	}
	//得到按钮状态
	int GetBtnState(){
		return nState;
	}
	//设置位置
	void SetBtnPos(int x, int y, int width, int height){
		rtfBtnPos = Gdiplus::RectF(x, y, width, height);
	}
	void SetBtnPos(Gdiplus::RectF rtf){
		rtfBtnPos = rtf;
	}
	//返回按钮位置
	Gdiplus::RectF GetBtnPos(){
		return rtfBtnPos;
	}
	//释放button使用过的图片跟字符串资源
	static void ReleaseBtnResource(){
		for (map<UINT, MyButton>::iterator it = mapMyButton.begin(); it != mapMyButton.end(); it++){
			delete it->second.GetStringContent();
			delete it->second.GetGdiImage();
		}
	}
	MyButton();
	MyButton(Gdiplus::RectF);
	~MyButton();
	static map<UINT, MyButton> mapMyButton; //存放所有使用过的MyButton资源
};

