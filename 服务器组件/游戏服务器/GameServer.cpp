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

int CGameServerApp::Run()
{
	BOOL bRun = TRUE;
	BOOL bExit=FALSE;
	while(!bExit)
	{
		bRun= CWinApp::Run();
		bExit=TRUE;
	}
	return bRun;
}

LRESULT CGameServerApp::ProcessWndProcException( CException* e, const MSG* pMsg )
{
	THROW_LAST();
	return 0;
}

//����̨
CConsoleAdapter g_ConsoleAdpter;

//////////////////////////////////////////////////////////////////////////////////
