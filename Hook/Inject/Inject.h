// Inject.h : Inject DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CInjectApp
// �йش���ʵ�ֵ���Ϣ������� Inject.cpp
//

class CInjectApp : public CWinApp
{
public:
	CInjectApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
