#pragma once

#include "Resource.h"		// ������

//////////////////////////////////////////////////////////////////////////////////

//�������
class CGameServerApp : public CWinApp
{
	//��������
public:
	//���캯�� 
	CGameServerApp(){}

	//���غ���
public:
	//��������
	virtual BOOL InitInstance();
	virtual int Run();
	virtual LRESULT ProcessWndProcException(CException* e, const MSG* pMsg);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

//�������
extern CGameServerApp theApp;

//////////////////////////////////////////////////////////////////////////////////
