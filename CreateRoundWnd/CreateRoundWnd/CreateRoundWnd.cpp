#include "stdafx.h"
#include "resource.h"
#include "MyImage.h"
#include "MyButton.h"

//各种宏定义
#define MAX_LOADSTRING 100

//Global variable
Gdiplus::GdiplusStartupInput g_GdiPlusStartupInput; //Gdiplus输入流
ULONG_PTR g_GdiToken;  //保存Gdi状态
TCHAR g_szMainWndClassName[MAX_LOADSTRING] = { 0 }; //主窗口类类名称
TCHAR g_szMainWndTitleName[MAX_LOADSTRING] = { 0 }; //主窗口的标题
HINSTANCE g_hInst; //保留应用程序句柄
UINT g_uJpgRes[] = { IDR_JPG1, IDR_JPG2, IDR_JPG3, IDR_JPG4 }; //保存所有用于选定前景图片的标识
UINT g_uBtnRes[] = { IDB_BUTTON1, IDB_BUTTON2, IDB_BUTTON3, IDB_BUTTON4 }; //保存所有选择按钮标识
int g_nSelect = 0; //保存选定的图片编号

//Methods  statement
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPreInstance, PSTR pszCmdLine, int nCmdShow);
LRESULT __stdcall WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT OnCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT MouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT LButtonDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT LButtonUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void MoveWndToRoundRgn(HWND hWnd, int nWidth, int nHeight);
bool IsPtInRectF(POINT pt, Gdiplus::RectF rtf);


//WinMain 程序入口
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPreInstance, PSTR pszCmdLine, int nCmdShow){
	WNDCLASS wndCls;
	HWND hWnd = NULL;
	MSG msg; //接受消息的结构体

	g_hInst = hInstance;
	LoadString(g_hInst, IDS_MAINWNDCLASSNAME, g_szMainWndClassName, MAX_LOADSTRING);
	LoadString(g_hInst, IDS_MAINWNDTITLENAME, g_szMainWndTitleName, MAX_LOADSTRING);
	wndCls.cbClsExtra = 0;
	wndCls.cbWndExtra = 0;
	wndCls.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndCls.hCursor = LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW));
	wndCls.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAINWNDICON));
	wndCls.hInstance = hInstance;
	wndCls.lpfnWndProc = WndProc;
	wndCls.lpszClassName = g_szMainWndClassName;
	wndCls.lpszMenuName = NULL;
	wndCls.style = CS_HREDRAW | CS_VREDRAW;

	//注册窗口类
	if (!RegisterClass(&wndCls)){
		return FALSE;
	}

	//创建主窗口
	hWnd = CreateWindow(g_szMainWndClassName, g_szMainWndTitleName, WS_POPUP |WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
	if (hWnd == NULL){
		return FALSE;
	}

	//显示窗口
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	//SetTimer(hWnd, 1, 100, NULL);
	//消息循环
	while (GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

//主窗口过程
LRESULT __stdcall WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	int nCmdId; //wParam的低位
	int nEvent; //wParam的高位

	switch (message)
	{
	case WM_CREATE:
		//Gdiplus初始化
		Gdiplus::GdiplusStartup(&g_GdiToken, &g_GdiPlusStartupInput, NULL);
		return OnCreate(hWnd, message, wParam, lParam);
		break;
	case WM_PAINT:
		return OnPaint(hWnd, message, wParam, lParam);
		break;
	case WM_LBUTTONDOWN:
		return LButtonDown(hWnd, message, wParam, lParam);
		break;
	case WM_TIMER:
	{
					 g_nSelect++;
					 g_nSelect %= 4;
					 InvalidateRect(hWnd, NULL, TRUE);
	}
		break;
	case WM_CLOSE:
		SendMessage(hWnd, WM_DESTROY, 0, 0);
		break;
	case WM_DESTROY:
		MyImage::RleaseResource();
		MyButton::ReleaseBtnResource();
		
		Gdiplus::GdiplusShutdown(g_GdiToken);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

//创建自定义的圆角窗口
void MoveWndToRoundRgn(HWND hWnd, int nWidth, int nHeight){
	HRGN hRgn = CreateRoundRectRgn(0, 0, nWidth, nHeight, 10, 10);
	if (hRgn == NULL){
		return;
	}
	SetWindowRgn(hWnd, hRgn, TRUE);

	//将窗口移动到屏幕中央位置
	int nScreenWith = GetSystemMetrics(SM_CXSCREEN);
	int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
	SetWindowPos(hWnd, NULL, (nScreenWith - nWidth) / 2, (nScreenHeight - nHeight) / 2, nWidth, nHeight, SWP_DRAWFRAME);
}

//创建窗口wm_create消息处理，进行各种资源的初始化工作
LRESULT OnCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	MyImage myImage;
	MyButton myButton;
	Gdiplus::Image* pGdiImage;
	RECT rtClient;
	int nWitdth;

	HDC hdc = GetDC(hWnd);
	myImage.SetHdc(hdc);

	//设置主框架背景
	myImage.SetImageFromResource(g_hInst, IDB_BACKGROUND);
	myImage.SetSelected(0);
	myImage.SetTotalCount(1);
	pGdiImage = myImage.GetGdiImage();
	myImage.SetDesRectF(0, 0, pGdiImage->GetWidth(), pGdiImage->GetHeight());
	MyImage::mapGdiImage[IDB_BACKGROUND] = myImage;

	//创建相应大小的圆角窗口，并且将窗口移动到屏幕中间位置
	MoveWndToRoundRgn(hWnd, pGdiImage->GetWidth(), pGdiImage->GetHeight());
	SetClassLong(hWnd, GCL_HBRBACKGROUND, (LONG)GetStockObject(BLACK_BRUSH));

	//设置背景之上第一幅全景
	myImage.SetImageFromResource(g_hInst, IDB_FRAME);
	MyImage::mapGdiImage[IDB_FRAME] = myImage;

	//设置4副前景图
	myImage.SetImageFromResource(g_hInst,IDR_JPG1,TEXT("JPG"));
	MyImage::mapGdiImage[IDR_JPG1] = myImage;
	myImage.SetImageFromResource(g_hInst, IDR_JPG2, TEXT("JPG"));
	MyImage::mapGdiImage[IDR_JPG2] = myImage;
	myImage.SetImageFromResource(g_hInst, IDR_JPG3, TEXT("JPG"));
	MyImage::mapGdiImage[IDR_JPG3] = myImage;
	myImage.SetImageFromResource(g_hInst, IDR_JPG4, TEXT("JPG"));
	MyImage::mapGdiImage[IDR_JPG4] = myImage;

	//设置移动的箭头
	myImage.SetImageFromResource(g_hInst, IDB_ARROW);
	pGdiImage = myImage.GetGdiImage();
	myImage.SetDesRectF(0, 299, pGdiImage->GetWidth(), pGdiImage->GetHeight());
	myImage.SetSelected(0);
	myImage.SetTotalCount(1);
	MyImage::mapGdiImage[IDB_ARROW] = myImage;

	//设置关闭按钮
	myImage.SetImageFromResource(g_hInst, IDB_CLOSE);
	myImage.SetTotalCount(4);
	myImage.SetSelected(0);
	GetClientRect(hWnd, &rtClient);
	pGdiImage = myImage.GetGdiImage();
	nWitdth = pGdiImage->GetWidth() / 4;
	myImage.SetDesRectF(rtClient.right - rtClient.left - nWitdth, 0, nWitdth, pGdiImage->GetHeight());
	MyImage::mapGdiImage[IDB_CLOSE] = myImage;

	//设置按钮背景图片
	myImage.SetImageFromResource(g_hInst, IDB_HOVER);
	myImage.SetDesRectF(1, 317, 170, 362 - 317);
	myImage.SetTotalCount(3);
	myImage.SetSelected(0);
	MyImage::mapGdiImage[IDB_HOVER] = myImage;

	//设置4个切换按钮
	myButton.SetTotalCount(1);
	myButton.SetSelected(0);
	POINT ptStart;
	int nCount = 0;
	ptStart.y = 315;
	GetClientRect(hWnd, &rtClient);

	myButton.SetBtnState(2);
	ptStart.x = 1 + 170 * (nCount++);
	myButton.SetImageFromResource(g_hInst, IDB_BUTTON1);
	myButton.SetStringContent(TEXT("功能1"));
	myButton.SetBtnPos(ptStart.x, ptStart.y, 170, rtClient.bottom - 5 - ptStart.y);
	MyButton::mapMyButton[IDB_BUTTON1] = myButton;

	myButton.SetBtnState(0);
	ptStart.x = 1 + 170 * (nCount++);
	myButton.SetImageFromResource(g_hInst, IDB_BUTTON2);
	myButton.SetStringContent(TEXT("功能2"));
	myButton.SetBtnPos(ptStart.x, ptStart.y, 170, rtClient.bottom - 5 - ptStart.y);
	MyButton::mapMyButton[IDB_BUTTON2] = myButton;

	myButton.SetBtnState(0);
	ptStart.x = 1 + 170 * (nCount++);
	myButton.SetImageFromResource(g_hInst, IDB_BUTTON3);
	myButton.SetStringContent(TEXT("功能3"));
	myButton.SetBtnPos(ptStart.x, ptStart.y, 170, rtClient.bottom - 5 - ptStart.y);
	MyButton::mapMyButton[IDB_BUTTON3] = myButton;

	myButton.SetBtnState(0);
	ptStart.x = 1 + 170 * (nCount++);
	myButton.SetImageFromResource(g_hInst, IDB_BUTTON4);
	myButton.SetStringContent(TEXT("功能4"));
	myButton.SetBtnPos(ptStart.x, ptStart.y, 170, rtClient.bottom - 5 - ptStart.y);
	MyButton::mapMyButton[IDB_BUTTON4] = myButton;

	//设置默认背景
	g_nSelect = 0;

	ReleaseDC(hWnd, hdc);
	return 0;
}

//处理主窗口wm_paint消息
LRESULT OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	HDC hdc;
	HDC hMem; //用于缓冲绘图
	HBITMAP hBitMap; //用于创建缓冲HDC的位图
	PAINTSTRUCT ps;
	RECT rtClient;
	StringFormat strFormat; //字符串格式

	hdc = BeginPaint(hWnd, &ps);
	hMem = CreateCompatibleDC(hdc);
	GetClientRect(hWnd, &rtClient);
	hBitMap = CreateCompatibleBitmap(hdc, rtClient.right - rtClient.left, rtClient.bottom - rtClient.top);
	SelectObject(hMem, hBitMap);
	//绘制背景
	MyImage::mapGdiImage[IDB_BACKGROUND].DrawGdiImage(hMem);

	//绘制前景图
	MyImage::mapGdiImage[g_uJpgRes[g_nSelect]].DrawGdiImage(hMem);

	//绘制第一幅全景
	//MyImage::mapGdiImage[IDB_FRAME].DrawGdiImage();  //绘图的时候要重新获取hdc
	MyImage::mapGdiImage[IDB_FRAME].DrawGdiImage(hMem);

	//绘制关闭按钮
	MyImage::mapGdiImage[IDB_CLOSE].DrawGdiImage(hMem);

	//绘制移动箭头
	MyImage::mapGdiImage[IDB_ARROW].DrawGdiImage(hMem);

	//绘制箭头的其他部分
	{
		GetClientRect(hWnd, &rtClient);
		Gdiplus::RectF rtf = MyImage::mapGdiImage[IDB_ARROW].GetImagePos();
		rtf.X += rtf.Width;
		rtf.Width = rtClient.right - rtf.X;
		Gdiplus::Graphics gs(hMem);
		gs.DrawImage(MyImage::mapGdiImage[IDB_ARROW].GetGdiImage(), rtf, 0, 0, 1, rtf.Height, UnitPixel);
	}

	//绘制4个按钮
	//设置字符串格式
	strFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	strFormat.SetAlignment(StringAlignmentNear);
	strFormat.SetLineAlignment(StringAlignmentNear);

	//绘制按钮1
	MyImage::mapGdiImage[IDB_HOVER].SetSelected(MyButton::mapMyButton[IDB_BUTTON1].GetBtnState());
	MyImage::mapGdiImage[IDB_HOVER].SetDesRectF(MyButton::mapMyButton[IDB_BUTTON1].GetBtnPos());
	MyImage::mapGdiImage[IDB_HOVER].DrawGdiImage(hMem);
	MyButton::mapMyButton[IDB_BUTTON1].DrawButton(hMem, &strFormat);

	//绘制按钮2
	MyImage::mapGdiImage[IDB_HOVER].SetSelected(MyButton::mapMyButton[IDB_BUTTON2].GetBtnState());
	MyImage::mapGdiImage[IDB_HOVER].SetDesRectF(MyButton::mapMyButton[IDB_BUTTON2].GetBtnPos());
	MyImage::mapGdiImage[IDB_HOVER].DrawGdiImage(hMem);
	MyButton::mapMyButton[IDB_BUTTON2].DrawButton(hMem, &strFormat);

	//绘制按钮3
	MyImage::mapGdiImage[IDB_HOVER].SetSelected(MyButton::mapMyButton[IDB_BUTTON3].GetBtnState());
	MyImage::mapGdiImage[IDB_HOVER].SetDesRectF(MyButton::mapMyButton[IDB_BUTTON3].GetBtnPos());
	MyImage::mapGdiImage[IDB_HOVER].DrawGdiImage(hMem);
	MyButton::mapMyButton[IDB_BUTTON3].DrawButton(hMem, &strFormat);

	//绘制按钮4
	MyImage::mapGdiImage[IDB_HOVER].SetSelected(MyButton::mapMyButton[IDB_BUTTON4].GetBtnState());
	MyImage::mapGdiImage[IDB_HOVER].SetDesRectF(MyButton::mapMyButton[IDB_BUTTON4].GetBtnPos());
	MyImage::mapGdiImage[IDB_HOVER].DrawGdiImage(hMem);
	MyButton::mapMyButton[IDB_BUTTON4].DrawButton(hMem, &strFormat);

	GetClientRect(hWnd, &rtClient);
	if (BitBlt(hdc, rtClient.left, rtClient.top, rtClient.right - rtClient.left, rtClient.bottom - rtClient.top, hMem, 0, 0, SRCCOPY) == 0){
		MessageBox(NULL, TEXT("绘图失败！"), TEXT("error"), MB_OK);
	}
	EndPaint(hWnd, &ps);
	return 0;
}

//处理主窗口鼠标左键点击消息
LRESULT LButtonDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	POINT pt; //记录鼠标的位置
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);

	//如果鼠标点击到了关闭按钮，或者是点击到了跟关闭按钮一样高度的其他部分
	if (pt.y <= MyImage::mapGdiImage[IDB_CLOSE].GetImagePos().Height){
		if (IsPtInRectF(pt, MyImage::mapGdiImage[IDB_CLOSE].GetImagePos())){
			SendMessage(hWnd, WM_CLOSE, 0, 0);
		}
		else
		{
			//在关闭按钮的同样高度的其他部分，发送窗口移动消息
			SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
		}
		return 0;
	}

	//如果鼠标点击到4个选择按钮
	for (unsigned int i = 0; i < 4; i++){
		if (IsPtInRectF(pt, MyButton::mapMyButton[g_uBtnRes[i]].GetBtnPos())){
			MyButton::mapMyButton[g_uBtnRes[i]].SetBtnState(2);
			g_nSelect = i;
			for (unsigned int j = 0; j < 4 ; j++){
				if (j == i){
					continue;
				}
				MyButton::mapMyButton[g_uBtnRes[j]].SetBtnState(0);
			}
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		}
	}
	return 0;
}

//处理主窗口鼠标左键抬起的消息
LRESULT LButtonUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	return 0;
}

//处理主窗口鼠标移动的消息
LRESULT MouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
		
	return 0;
}

//判断一个点是否在某个类型为Gdiplus::RectF的矩形内部
bool IsPtInRectF(POINT pt, Gdiplus::RectF rtf){
	if (pt.x >= rtf.GetLeft() && pt.x <= rtf.GetRight() && pt.y >= rtf.GetTop() && pt.y <= rtf.GetBottom()){
		return true;
	}
	return false;
}