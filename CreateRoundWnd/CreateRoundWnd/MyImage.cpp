#include "MyImage.h"

map<UINT, MyImage> MyImage::mapGdiImage; //������������õ���MyImage���󼯺�

//ͨ����Դ����λͼ
void MyImage::SetImageFromResource(HINSTANCE hInst, UINT uResId, TCHAR szFileKind[]){
	HRSRC hRes = FindResource(hInst, MAKEINTRESOURCE(uResId), szFileKind);
	if (hRes == NULL){
		this->pGdiImage = NULL;
		return;
	}
	DWORD dwSize = SizeofResource(hInst, hRes);
	if (dwSize == 0){
		this->pGdiImage = NULL;
		return;
	}
	HGLOBAL hGlobal = LoadResource(hInst, hRes);
	if (hGlobal == NULL){
		this->pGdiImage = NULL;
		return;
	}
	LPVOID lpBuffer = LockResource(hGlobal);
	if (lpBuffer == NULL){
		this->pGdiImage = NULL;
		FreeResource(hGlobal);
		return;
	}

	IStream *pIstream = NULL;

	//ֱ����hGlobal���洴�����������������
	/*if (CreateStreamOnHGlobal(hGlobal, FALSE, &pIstream) == Gdiplus::Ok){
		pGdiImage = Gdiplus::Image::FromStream(pIstream,FALSE);
		pIstream->Release();
	}*/
	HGLOBAL hTemp = GlobalAlloc(GMEM_MOVEABLE, dwSize);
	if (hTemp == NULL){
		goto end;
	}
	LPVOID lpTempBuffer = GlobalLock(hTemp);
	if (lpTempBuffer == NULL){
		GlobalFree(hTemp);
		goto end;
	}
	memcpy(lpTempBuffer, lpBuffer, dwSize);
	GlobalUnlock(hTemp);
	if (CreateStreamOnHGlobal(hTemp, TRUE, &pIstream) == Gdiplus::Ok){
		pGdiImage = Gdiplus::Image::FromStream(pIstream, FALSE);
		pIstream->Release();
	}
end: 
	UnlockResource(hGlobal);
	FreeResource(hGlobal);
}

//����ͼƬ
void MyImage::DrawGdiImage(){
	DrawGdiImage(hdc, rtfDes);
}

void MyImage::DrawGdiImage(HDC hdc){
	DrawGdiImage(hdc, rtfDes);
}

//��ָ���豸��ָ��λ�û���ͼƬ
void MyImage::DrawGdiImage(HDC hdc, Gdiplus::RectF rtfDesPos){
	Gdiplus::Graphics graphics(hdc);
	if (pGdiImage == NULL){
		return;
	}

	//�����ǰ��nSelected����nTotalCount ���������⣬�Ͱ��������ó�ΪĬ�ϵ�0��1��������ֱ����������ͼƬ
	if (nSelected < 0 || nTotalCount <= 0){
		nSelected = 0;
		nTotalCount = 1;
	}
	int width = pGdiImage->GetWidth() / nTotalCount;
	int height = pGdiImage->GetHeight();
	graphics.DrawImage(pGdiImage, rtfDesPos, width*nSelected, 0, width, height,Gdiplus::UnitPixel);
}

//���Ƽ����е�����ͼ��
void MyImage::DrawImage(){
	for (map<UINT, MyImage>::iterator it = mapGdiImage.begin(); it != mapGdiImage.end(); it++){
		MyImage &image = it->second;
		image.DrawGdiImage(image.hdc, image.rtfDes);
	}
}

//�ͷż��������ж����ͼƬ��Դ
void MyImage::RleaseResource(){
	for (map<UINT, MyImage>::iterator it = mapGdiImage.begin(); it != mapGdiImage.end(); it++){
		MyImage &image = it->second;
		image.FreeImageResource();
	}
}

MyImage::~MyImage()
{
	//delete pGdiImage;  //���������쳣���鵽��ԭ����GdiplusShutdown���ͷ��Լ���ͼ������ʹ�ù�����Դ
}
