#include "stdafx.h"
#include "resource.h"
#include "MyImage.h"
#include "MyButton.h"

//���ֺ궨��
#define MAX_LOADSTRING 100

//Global variable
Gdiplus::GdiplusStartupInput g_GdiPlusStartupInput; //Gdiplus������
ULONG_PTR g_GdiToken;  //����Gdi״̬
TCHAR g_szMainWndClassName[MAX_LOADSTRING] = { 0 }; //��������������
TCHAR g_szMainWndTitleName[MAX_LOADSTRING] = { 0 }; //�����ڵı���
HINSTANCE g_hInst; //����Ӧ�ó�����
UINT g_uJpgRes[] = { IDR_JPG1, IDR_JPG2, IDR_JPG3, IDR_JPG4 }; //������������ѡ��ǰ��ͼƬ�ı�ʶ
UINT g_uBtnRes[] = { IDB_BUTTON1, IDB_BUTTON2, IDB_BUTTON3, IDB_BUTTON4 }; //��������ѡ��ť��ʶ
int g_nSelect = 0; //����ѡ����ͼƬ���

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


//WinMain �������
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPreInstance, PSTR pszCmdLine, int nCmdShow){
	WNDCLASS wndCls;
	HWND hWnd = NULL;
	MSG msg; //������Ϣ�Ľṹ��

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

	//ע�ᴰ����
	if (!RegisterClass(&wndCls)){
		return FALSE;
	}

	//����������
	hWnd = CreateWindow(g_szMainWndClassName, g_szMainWndTitleName, WS_POPUP |WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
	if (hWnd == NULL){
		return FALSE;
	}

	//��ʾ����
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	//SetTimer(hWnd, 1, 100, NULL);
	//��Ϣѭ��
	while (GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

//�����ڹ���
LRESULT __stdcall WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	int nCmdId; //wParam�ĵ�λ
	int nEvent; //wParam�ĸ�λ

	switch (message)
	{
	case WM_CREATE:
		//Gdiplus��ʼ��
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

//�����Զ����Բ�Ǵ���
void MoveWndToRoundRgn(HWND hWnd, int nWidth, int nHeight){
	HRGN hRgn = CreateRoundRectRgn(0, 0, nWidth, nHeight, 10, 10);
	if (hRgn == NULL){
		return;
	}
	SetWindowRgn(hWnd, hRgn, TRUE);

	//�������ƶ�����Ļ����λ��
	int nScreenWith = GetSystemMetrics(SM_CXSCREEN);
	int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
	SetWindowPos(hWnd, NULL, (nScreenWith - nWidth) / 2, (nScreenHeight - nHeight) / 2, nWidth, nHeight, SWP_DRAWFRAME);
}

//��������wm_create��Ϣ�������и�����Դ�ĳ�ʼ������
LRESULT OnCreate(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	MyImage myImage;
	MyButton myButton;
	Gdiplus::Image* pGdiImage;
	RECT rtClient;
	int nWitdth;

	HDC hdc = GetDC(hWnd);
	myImage.SetHdc(hdc);

	//��������ܱ���
	myImage.SetImageFromResource(g_hInst, IDB_BACKGROUND);
	myImage.SetSelected(0);
	myImage.SetTotalCount(1);
	pGdiImage = myImage.GetGdiImage();
	myImage.SetDesRectF(0, 0, pGdiImage->GetWidth(), pGdiImage->GetHeight());
	MyImage::mapGdiImage[IDB_BACKGROUND] = myImage;

	//������Ӧ��С��Բ�Ǵ��ڣ����ҽ������ƶ�����Ļ�м�λ��
	MoveWndToRoundRgn(hWnd, pGdiImage->GetWidth(), pGdiImage->GetHeight());
	SetClassLong(hWnd, GCL_HBRBACKGROUND, (LONG)GetStockObject(BLACK_BRUSH));

	//���ñ���֮�ϵ�һ��ȫ��
	myImage.SetImageFromResource(g_hInst, IDB_FRAME);
	MyImage::mapGdiImage[IDB_FRAME] = myImage;

	//����4��ǰ��ͼ
	myImage.SetImageFromResource(g_hInst,IDR_JPG1,TEXT("JPG"));
	MyImage::mapGdiImage[IDR_JPG1] = myImage;
	myImage.SetImageFromResource(g_hInst, IDR_JPG2, TEXT("JPG"));
	MyImage::mapGdiImage[IDR_JPG2] = myImage;
	myImage.SetImageFromResource(g_hInst, IDR_JPG3, TEXT("JPG"));
	MyImage::mapGdiImage[IDR_JPG3] = myImage;
	myImage.SetImageFromResource(g_hInst, IDR_JPG4, TEXT("JPG"));
	MyImage::mapGdiImage[IDR_JPG4] = myImage;

	//�����ƶ��ļ�ͷ
	myImage.SetImageFromResource(g_hInst, IDB_ARROW);
	pGdiImage = myImage.GetGdiImage();
	myImage.SetDesRectF(0, 299, pGdiImage->GetWidth(), pGdiImage->GetHeight());
	myImage.SetSelected(0);
	myImage.SetTotalCount(1);
	MyImage::mapGdiImage[IDB_ARROW] = myImage;

	//���ùرհ�ť
	myImage.SetImageFromResource(g_hInst, IDB_CLOSE);
	myImage.SetTotalCount(4);
	myImage.SetSelected(0);
	GetClientRect(hWnd, &rtClient);
	pGdiImage = myImage.GetGdiImage();
	nWitdth = pGdiImage->GetWidth() / 4;
	myImage.SetDesRectF(rtClient.right - rtClient.left - nWitdth, 0, nWitdth, pGdiImage->GetHeight());
	MyImage::mapGdiImage[IDB_CLOSE] = myImage;

	//���ð�ť����ͼƬ
	myImage.SetImageFromResource(g_hInst, IDB_HOVER);
	myImage.SetDesRectF(1, 317, 170, 362 - 317);
	myImage.SetTotalCount(3);
	myImage.SetSelected(0);
	MyImage::mapGdiImage[IDB_HOVER] = myImage;

	//����4���л���ť
	myButton.SetTotalCount(1);
	myButton.SetSelected(0);
	POINT ptStart;
	int nCount = 0;
	ptStart.y = 315;
	GetClientRect(hWnd, &rtClient);

	myButton.SetBtnState(2);
	ptStart.x = 1 + 170 * (nCount++);
	myButton.SetImageFromResource(g_hInst, IDB_BUTTON1);
	myButton.SetStringContent(TEXT("����1"));
	myButton.SetBtnPos(ptStart.x, ptStart.y, 170, rtClient.bottom - 5 - ptStart.y);
	MyButton::mapMyButton[IDB_BUTTON1] = myButton;

	myButton.SetBtnState(0);
	ptStart.x = 1 + 170 * (nCount++);
	myButton.SetImageFromResource(g_hInst, IDB_BUTTON2);
	myButton.SetStringContent(TEXT("����2"));
	myButton.SetBtnPos(ptStart.x, ptStart.y, 170, rtClient.bottom - 5 - ptStart.y);
	MyButton::mapMyButton[IDB_BUTTON2] = myButton;

	myButton.SetBtnState(0);
	ptStart.x = 1 + 170 * (nCount++);
	myButton.SetImageFromResource(g_hInst, IDB_BUTTON3);
	myButton.SetStringContent(TEXT("����3"));
	myButton.SetBtnPos(ptStart.x, ptStart.y, 170, rtClient.bottom - 5 - ptStart.y);
	MyButton::mapMyButton[IDB_BUTTON3] = myButton;

	myButton.SetBtnState(0);
	ptStart.x = 1 + 170 * (nCount++);
	myButton.SetImageFromResource(g_hInst, IDB_BUTTON4);
	myButton.SetStringContent(TEXT("����4"));
	myButton.SetBtnPos(ptStart.x, ptStart.y, 170, rtClient.bottom - 5 - ptStart.y);
	MyButton::mapMyButton[IDB_BUTTON4] = myButton;

	//����Ĭ�ϱ���
	g_nSelect = 0;

	ReleaseDC(hWnd, hdc);
	return 0;
}

//����������wm_paint��Ϣ
LRESULT OnPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	HDC hdc;
	HDC hMem; //���ڻ����ͼ
	HBITMAP hBitMap; //���ڴ�������HDC��λͼ
	PAINTSTRUCT ps;
	RECT rtClient;
	StringFormat strFormat; //�ַ�����ʽ

	hdc = BeginPaint(hWnd, &ps);
	hMem = CreateCompatibleDC(hdc);
	GetClientRect(hWnd, &rtClient);
	hBitMap = CreateCompatibleBitmap(hdc, rtClient.right - rtClient.left, rtClient.bottom - rtClient.top);
	SelectObject(hMem, hBitMap);
	//���Ʊ���
	MyImage::mapGdiImage[IDB_BACKGROUND].DrawGdiImage(hMem);

	//����ǰ��ͼ
	MyImage::mapGdiImage[g_uJpgRes[g_nSelect]].DrawGdiImage(hMem);

	//���Ƶ�һ��ȫ��
	//MyImage::mapGdiImage[IDB_FRAME].DrawGdiImage();  //��ͼ��ʱ��Ҫ���»�ȡhdc
	MyImage::mapGdiImage[IDB_FRAME].DrawGdiImage(hMem);

	//���ƹرհ�ť
	MyImage::mapGdiImage[IDB_CLOSE].DrawGdiImage(hMem);

	//�����ƶ���ͷ
	MyImage::mapGdiImage[IDB_ARROW].DrawGdiImage(hMem);

	//���Ƽ�ͷ����������
	{
		GetClientRect(hWnd, &rtClient);
		Gdiplus::RectF rtf = MyImage::mapGdiImage[IDB_ARROW].GetImagePos();
		rtf.X += rtf.Width;
		rtf.Width = rtClient.right - rtf.X;
		Gdiplus::Graphics gs(hMem);
		gs.DrawImage(MyImage::mapGdiImage[IDB_ARROW].GetGdiImage(), rtf, 0, 0, 1, rtf.Height, UnitPixel);
	}

	//����4����ť
	//�����ַ�����ʽ
	strFormat.SetFormatFlags(StringFormatFlagsNoWrap);
	strFormat.SetAlignment(StringAlignmentNear);
	strFormat.SetLineAlignment(StringAlignmentNear);

	//���ư�ť1
	MyImage::mapGdiImage[IDB_HOVER].SetSelected(MyButton::mapMyButton[IDB_BUTTON1].GetBtnState());
	MyImage::mapGdiImage[IDB_HOVER].SetDesRectF(MyButton::mapMyButton[IDB_BUTTON1].GetBtnPos());
	MyImage::mapGdiImage[IDB_HOVER].DrawGdiImage(hMem);
	MyButton::mapMyButton[IDB_BUTTON1].DrawButton(hMem, &strFormat);

	//���ư�ť2
	MyImage::mapGdiImage[IDB_HOVER].SetSelected(MyButton::mapMyButton[IDB_BUTTON2].GetBtnState());
	MyImage::mapGdiImage[IDB_HOVER].SetDesRectF(MyButton::mapMyButton[IDB_BUTTON2].GetBtnPos());
	MyImage::mapGdiImage[IDB_HOVER].DrawGdiImage(hMem);
	MyButton::mapMyButton[IDB_BUTTON2].DrawButton(hMem, &strFormat);

	//���ư�ť3
	MyImage::mapGdiImage[IDB_HOVER].SetSelected(MyButton::mapMyButton[IDB_BUTTON3].GetBtnState());
	MyImage::mapGdiImage[IDB_HOVER].SetDesRectF(MyButton::mapMyButton[IDB_BUTTON3].GetBtnPos());
	MyImage::mapGdiImage[IDB_HOVER].DrawGdiImage(hMem);
	MyButton::mapMyButton[IDB_BUTTON3].DrawButton(hMem, &strFormat);

	//���ư�ť4
	MyImage::mapGdiImage[IDB_HOVER].SetSelected(MyButton::mapMyButton[IDB_BUTTON4].GetBtnState());
	MyImage::mapGdiImage[IDB_HOVER].SetDesRectF(MyButton::mapMyButton[IDB_BUTTON4].GetBtnPos());
	MyImage::mapGdiImage[IDB_HOVER].DrawGdiImage(hMem);
	MyButton::mapMyButton[IDB_BUTTON4].DrawButton(hMem, &strFormat);

	GetClientRect(hWnd, &rtClient);
	if (BitBlt(hdc, rtClient.left, rtClient.top, rtClient.right - rtClient.left, rtClient.bottom - rtClient.top, hMem, 0, 0, SRCCOPY) == 0){
		MessageBox(NULL, TEXT("��ͼʧ�ܣ�"), TEXT("error"), MB_OK);
	}
	EndPaint(hWnd, &ps);
	return 0;
}

//���������������������Ϣ
LRESULT LButtonDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	POINT pt; //��¼����λ��
	pt.x = LOWORD(lParam);
	pt.y = HIWORD(lParam);

	//�����������˹رհ�ť�������ǵ�����˸��رհ�ťһ���߶ȵ���������
	if (pt.y <= MyImage::mapGdiImage[IDB_CLOSE].GetImagePos().Height){
		if (IsPtInRectF(pt, MyImage::mapGdiImage[IDB_CLOSE].GetImagePos())){
			SendMessage(hWnd, WM_CLOSE, 0, 0);
		}
		else
		{
			//�ڹرհ�ť��ͬ���߶ȵ��������֣����ʹ����ƶ���Ϣ
			SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
		}
		return 0;
	}

	//����������4��ѡ��ť
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

//����������������̧�����Ϣ
LRESULT LButtonUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	return 0;
}

//��������������ƶ�����Ϣ
LRESULT MouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
		
	return 0;
}

//�ж�һ�����Ƿ���ĳ������ΪGdiplus::RectF�ľ����ڲ�
bool IsPtInRectF(POINT pt, Gdiplus::RectF rtf){
	if (pt.x >= rtf.GetLeft() && pt.x <= rtf.GetRight() && pt.y >= rtf.GetTop() && pt.y <= rtf.GetBottom()){
		return true;
	}
	return false;
}