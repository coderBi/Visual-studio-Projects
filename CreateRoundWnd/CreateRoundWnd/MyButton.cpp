#include "MyButton.h"

map<UINT, MyButton> MyButton::mapMyButton;
MyButton::MyButton()
{
	nState = 0;
}

MyButton::MyButton(Gdiplus::RectF rtf){
	rtfBtnPos = rtf;
}

void MyButton::DrawButton(HDC hdc, StringFormat *strFormat){
	int x, y, x1;
	
	//»æÖÆÍ¼Æ¬
	Image *pGdiImage = GetGdiImage();
	int width = pGdiImage->GetWidth();
	int height = pGdiImage->GetHeight();
	x = rtfBtnPos.X + 10;
	y = rtfBtnPos.Y + (rtfBtnPos.GetBottom() - rtfBtnPos.Y - height) / 2;
	x1 = width + x;
	SetDesRectF(x, y, width, height);
	DrawGdiImage(hdc);

	//»æÖÆ×Ö·û
	x = x1 + 10;
	x1 = rtfBtnPos.GetRight() - 10;
	y = rtfBtnPos.Y + (rtfBtnPos.GetBottom() - rtfBtnPos.Y - 15) / 2;
	SetStringPos(RectF(x, y, x1 - x, 15));
	DrawString(hdc, strFormat);
}

MyButton::~MyButton()
{
}
