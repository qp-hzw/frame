#include "Stdafx.h"
#include "GameServer.h"
#include "ServiceUnits.h"
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

	CServiceUnits m_ServiceUnits;
	m_ServiceUnits.StartService();

	//启动服务
	while(true)
	{
		Sleep(100);
	}

	return FALSE;
}


//控制台
CConsoleAdapter g_ConsoleAdpter;

//////////////////////////////////////////////////////////////////////////////////
