#include "Stdafx.h"
#include "LogonServer.h"
#include "ServiceUnits.h"

//�������
CLogonServerApp theApp;

BEGIN_MESSAGE_MAP(CLogonServerApp, CWinApp)
END_MESSAGE_MAP()

//��������
BOOL CLogonServerApp::InitInstance()
{
	__super::InitInstance();

	//�������
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();

	//��ʾ����
	CServiceUnits	m_ServiceUnits;	
	m_ServiceUnits.StartService();

	while(true)
	{
		Sleep(100);
	}


	return FALSE;
}


//����̨
CConsoleAdapter g_ConsoleAdpter;