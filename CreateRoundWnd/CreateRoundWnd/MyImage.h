#pragma once
#include "stdafx.h"
using namespace std;

class MyImage
{
private:
	Gdiplus::Image *pGdiImage; //ͼƬ��Դָ��
	int nTotalCount; //һ������ͼƬ����
	int nSelected; //��ǰѡ���ͼƬ���
	HDC hdc; //����Ŀ���豸��hdc
public:
	Gdiplus::RectF rtfDes; //����Ŀ������ľ���λ��
	static map<UINT, MyImage> mapGdiImage; //������������õ���MyImage���󼯺�

	//���Ƽ����е�����ͼ��
	static void DrawImage();
	//�ͷż��������ж����ͼƬ��Դ
	static void RleaseResource();
public:
	MyImage(int nTotalCount = 1, int nSelected = 0, Gdiplus::Image *pGdiImage = NULL){
		this->nTotalCount = nTotalCount;
		this->nSelected = nSelected;
		this->pGdiImage = pGdiImage;
	}

	//����hdc
	void SetHdc(HDC hdc){
		this->hdc = hdc;
	}
	//����nTotalCount
	void SetTotalCount(int nTotalCount){
		this->nTotalCount = nTotalCount;
	}
	//����nSelected
	void SetSelected(int nSelected){
		this->nSelected = nSelected;
	}
	//����Ŀ���豸�������λ��
	void SetDesRectF(int x, int y, int width, int height){
		rtfDes.X = x;
		rtfDes.Y = y;
		rtfDes.Width = width;
		rtfDes.Height = height;
	}
	void SetDesRectF(Gdiplus::RectF rtf){
		rtfDes = rtf;
	}
	//�õ�pGdiImage
	Gdiplus::Image* GetGdiImage(){
		return pGdiImage;
	}
	//�õ�ͼƬλ��
	Gdiplus::RectF GetImagePos(){
		return rtfDes;
	}
	//ͨ����Դ����λͼ
	void SetImageFromResource(HINSTANCE hInst, UINT uResId, TCHAR szFileKind[] = TEXT("PNG"));
	//���ļ�������λͼ
	void SetImageFromResource();
	//�ͷ�ͼƬ��Դ
	void FreeImageResource(){
		delete pGdiImage;
		pGdiImage = NULL;
	}
	//��ָ���豸��ָ��λ�û���ͼƬ
	void DrawGdiImage(); //����ͨ��ʵ���֪����Ҫ���»�ȡhdc�ģ�û�а취���䱣��ֱ��ʹ�ã��������������ʵ�ϵ���
	void DrawGdiImage(HDC hdc);
	void DrawGdiImage(HDC hdc, Gdiplus::RectF rtfDesPos);
	~MyImage();
};

