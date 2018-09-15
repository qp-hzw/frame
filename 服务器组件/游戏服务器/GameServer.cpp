#include "Stdafx.h"
#include "GameServer.h"
#include "DlgGameServer.h"
//////////////////////////////////////////////////////////////////////////////////

//程序对象
CGameServerApp theApp;

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameServerApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CGameServerApp::CGameServerApp()
{
}


//启动函数
BOOL CGameServerApp::InitInstance()
{
//#ifndef _DEBUG
//	if(InstallCrashRpt() != TRUE)
//	{
//		return FALSE;
//	}
//#endif // !DEBUG

	__super::InitInstance();
	
	//设置组件
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();

	//设置注册表
	//SetRegistryKey(szProduct);

	//显示窗口
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
