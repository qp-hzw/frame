#include "Stdafx.h"
#include "GameServer.h"
#include "DlgGameServer.h"
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


	//��ʾ����
	CGameServerDlg GameServerDlg;
	m_pMainWnd=&GameServerDlg;
	GameServerDlg.DoModal();

	return FALSE;
}


//����̨
CConsoleAdapter g_ConsoleAdpter;

//////////////////////////////////////////////////////////////////////////////////
