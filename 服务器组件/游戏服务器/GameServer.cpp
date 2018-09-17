#include "Stdafx.h"
#include "GameServer.h"
#include "DlgGameServer.h"
//////////////////////////////////////////////////////////////////////////////////

//�������
CGameServerApp theApp;

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameServerApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CGameServerApp::CGameServerApp()
{
}


//��������
BOOL CGameServerApp::InitInstance()
{
//#ifndef _DEBUG
//	if(InstallCrashRpt() != TRUE)
//	{
//		return FALSE;
//	}
//#endif // !DEBUG

	__super::InitInstance();
	
	//�������
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();

	//����ע���
	//SetRegistryKey(szProduct);

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

//////////////////////////////////////////////////////////////////////////////////
