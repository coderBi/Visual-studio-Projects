#include "GdiString.h"

std::map<UINT, GdiString> GdiString::mapGdiString;
GdiString::GdiString()
{
	//Ĭ���ÿ��ַ���
	lpszBuffer = NULL;
	//��������Ϊ��ɫ��Ĭ���Ǻ�ɫ��
	color.SetValue((ARGB)Color::White);

	//����Ĭ������
	memset(&logFont, 0, sizeof(logFont));
	logFont.lfHeight = -15;
	logFont.lfWeight |= FW_BOLD;
	lstrcpy(logFont.lfFaceName, TEXT("����"));
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
