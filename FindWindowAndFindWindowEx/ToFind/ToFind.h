
// ToFind.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CToFindApp: 
// �йش����ʵ�֣������ ToFind.cpp
//

class CToFindApp : public CWinApp
{
public:
	CToFindApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CToFindApp theApp;