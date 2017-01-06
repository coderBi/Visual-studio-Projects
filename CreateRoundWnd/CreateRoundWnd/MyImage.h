#pragma once
#include "stdafx.h"
using namespace std;

class MyImage
{
private:
	Gdiplus::Image *pGdiImage; //图片资源指针
	int nTotalCount; //一共包含图片数量
	int nSelected; //当前选择的图片编号
	HDC hdc; //绘制目标设备的hdc
public:
	Gdiplus::RectF rtfDes; //绘制目标上面的矩形位置
	static map<UINT, MyImage> mapGdiImage; //用来存放所有用到的MyImage对象集合

	//绘制集合中的所有图像
	static void DrawImage();
	//释放集合中所有对象的图片资源
	static void RleaseResource();
public:
	MyImage(int nTotalCount = 1, int nSelected = 0, Gdiplus::Image *pGdiImage = NULL){
		this->nTotalCount = nTotalCount;
		this->nSelected = nSelected;
		this->pGdiImage = pGdiImage;
	}

	//设置hdc
	void SetHdc(HDC hdc){
		this->hdc = hdc;
	}
	//设置nTotalCount
	void SetTotalCount(int nTotalCount){
		this->nTotalCount = nTotalCount;
	}
	//设置nSelected
	void SetSelected(int nSelected){
		this->nSelected = nSelected;
	}
	//设置目标设备上面绘制位置
	void SetDesRectF(int x, int y, int width, int height){
		rtfDes.X = x;
		rtfDes.Y = y;
		rtfDes.Width = width;
		rtfDes.Height = height;
	}
	void SetDesRectF(Gdiplus::RectF rtf){
		rtfDes = rtf;
	}
	//得到pGdiImage
	Gdiplus::Image* GetGdiImage(){
		return pGdiImage;
	}
	//得到图片位置
	Gdiplus::RectF GetImagePos(){
		return rtfDes;
	}
	//通过资源载入位图
	void SetImageFromResource(HINSTANCE hInst, UINT uResId, TCHAR szFileKind[] = TEXT("PNG"));
	//从文件中载入位图
	void SetImageFromResource();
	//释放图片资源
	void FreeImageResource(){
		delete pGdiImage;
		pGdiImage = NULL;
	}
	//在指定设备的指定位置绘制图片
	void DrawGdiImage(); //这里通过实验得知是需要重新获取hdc的，没有办法将其保存直接使用，所以这个函数其实废掉了
	void DrawGdiImage(HDC hdc);
	void DrawGdiImage(HDC hdc, Gdiplus::RectF rtfDesPos);
	~MyImage();
};

