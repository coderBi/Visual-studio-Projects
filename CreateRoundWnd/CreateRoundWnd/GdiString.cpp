#include "GdiString.h"

std::map<UINT, GdiString> GdiString::mapGdiString;
GdiString::GdiString()
{
	//默认置空字符串
	lpszBuffer = NULL;
	//设置字体为白色，默认是黑色的
	color.SetValue((ARGB)Color::White);

	//设置默认字体
	memset(&logFont, 0, sizeof(logFont));
	logFont.lfHeight = -15;
	logFont.lfWeight |= FW_BOLD;
	lstrcpy(logFont.lfFaceName, TEXT("宋体"));
}

void GdiString::DrawString(HDC hdc, StringFormat *strFormat){
	if (!lpszBuffer){
		return;
	}
	SolidBrush sBrush(color);
	Font font(hdc, &logFont);
	Graphics graphics(hdc);
	graphics.DrawString(lpszBuffer, lstrlen(lpszBuffer), &font, rtfStringDes, strFormat, &sBrush);
}

GdiString::~GdiString()
{
	//delete lpszBuffer;
}
