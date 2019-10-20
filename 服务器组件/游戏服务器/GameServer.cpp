#include "Stdafx.h"
#include "GameServer.h"
#include "ServiceUnits.h"
//////////////////////////////////////////////////////////////////////////////////

//�������
CGameServerApp theApp;

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameServerApp, CWinApp)
END_MESSAGE_MAP()

//��������
BOOL CGameServerApp::InitInstance()
{
	__super::InitInstance();
	
	//�������
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();

	CServiceUnits m_ServiceUnits("dll_name.so"); //jTODONOW
	m_ServiceUnits.StartService();

	//��������
	while(true)
	{
		Sleep(100);
	}

	return FALSE;
}


//����̨
CConsoleAdapter g_ConsoleAdpter;

//////////////////////////////////////////////////////////////////////////////////
