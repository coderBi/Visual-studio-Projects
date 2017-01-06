#pragma once
#include "MyImage.h"
#include "GdiString.h"

class MyButton :
	public MyImage, public GdiString
{
private:
	int nState; //��ť��״̬��0~2 ��Ӧ��ͬ�İ�ť����
	Gdiplus::RectF rtfBtnPos; //��ť��Ŀ��λ��
	
public:
	//����button
	void DrawButton(HDC hdc, StringFormat *strFormat);
	//���ð�ť״̬
	void SetBtnState(int state){
		nState = state;
	}
	//�õ���ť״̬
	int GetBtnState(){
		return nState;
	}
	//����λ��
	void SetBtnPos(int x, int y, int width, int height){
		rtfBtnPos = Gdiplus::RectF(x, y, width, height);
	}
	void SetBtnPos(Gdiplus::RectF rtf){
		rtfBtnPos = rtf;
	}
	//���ذ�ťλ��
	Gdiplus::RectF GetBtnPos(){
		return rtfBtnPos;
	}
	//�ͷ�buttonʹ�ù���ͼƬ���ַ�����Դ
	static void ReleaseBtnResource(){
		for (map<UINT, MyButton>::iterator it = mapMyButton.begin(); it != mapMyButton.end(); it++){
			delete it->second.GetStringContent();
			delete it->second.GetGdiImage();
		}
	}
	MyButton();
	MyButton(Gdiplus::RectF);
	~MyButton();
	static map<UINT, MyButton> mapMyButton; //�������ʹ�ù���MyButton��Դ
};

