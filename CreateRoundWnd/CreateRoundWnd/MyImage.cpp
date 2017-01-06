#include "MyImage.h"

map<UINT, MyImage> MyImage::mapGdiImage; //用来存放所有用到的MyImage对象集合

//通过资源载入位图
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

	//直接在hGlobal上面创建流好像是有问题的
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

//绘制图片
void MyImage::DrawGdiImage(){
	DrawGdiImage(hdc, rtfDes);
}

void MyImage::DrawGdiImage(HDC hdc){
	DrawGdiImage(hdc, rtfDes);
}

//在指定设备的指定位置绘制图片
void MyImage::DrawGdiImage(HDC hdc, Gdiplus::RectF rtfDesPos){
	Gdiplus::Graphics graphics(hdc);
	if (pGdiImage == NULL){
		return;
	}

	//如果当前的nSelected或者nTotalCount 设置有问题，就把他们设置成为默认的0跟1，这样就直接载入整幅图片
	if (nSelected < 0 || nTotalCount <= 0){
		nSelected = 0;
		nTotalCount = 1;
	}
	int width = pGdiImage->GetWidth() / nTotalCount;
	int height = pGdiImage->GetHeight();
	graphics.DrawImage(pGdiImage, rtfDesPos, width*nSelected, 0, width, height,Gdiplus::UnitPixel);
}

//绘制集合中的所有图像
void MyImage::DrawImage(){
	for (map<UINT, MyImage>::iterator it = mapGdiImage.begin(); it != mapGdiImage.end(); it++){
		MyImage &image = it->second;
		image.DrawGdiImage(image.hdc, image.rtfDes);
	}
}

//释放集合中所有对象的图片资源
void MyImage::RleaseResource(){
	for (map<UINT, MyImage>::iterator it = mapGdiImage.begin(); it != mapGdiImage.end(); it++){
		MyImage &image = it->second;
		image.FreeImageResource();
	}
}

MyImage::~MyImage()
{
	//delete pGdiImage;  //这里会出现异常，查到的原因是GdiplusShutdown会释放自己绘图过程中使用过的资源
}
