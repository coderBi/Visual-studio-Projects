
// FindWindowEx.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CFindWindowExApp: 
// �йش����ʵ�֣������ FindWindowEx.cpp
//

class CFindWindowExApp : public CWinApp
{
public:
	CFindWindowExApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CFindWindowExApp theApp;