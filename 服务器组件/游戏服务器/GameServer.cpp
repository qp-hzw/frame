#include "Stdafx.h"
#include "GameServer.h"
#include "DlgGameServer.h"
//////////////////////////////////////////////////////////////////////////////////

//程序对象
CGameServerApp theApp;

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameServerApp, CWinApp)
END_MESSAGE_MAP()

//启动函数
BOOL CGameServerApp::InitInstance()
{
	__super::InitInstance();
	
	//设置组件
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();


	//显示窗口
	CGameServerDlg GameServerDlg;
	m_pMainWnd=&GameServerDlg;
	GameServerDlg.DoModal();

	return FALSE;
}


//控制台
CConsoleAdapter g_ConsoleAdpter;

//////////////////////////////////////////////////////////////////////////////////
