#include "Stdafx.h"
#include "LogonServer.h"
#include "ServiceUnits.h"

//程序对象
CLogonServerApp theApp;

BEGIN_MESSAGE_MAP(CLogonServerApp, CWinApp)
END_MESSAGE_MAP()

//启动函数
BOOL CLogonServerApp::InitInstance()
{
	__super::InitInstance();

	//设置组件
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();

	//显示窗口
	CServiceUnits	m_ServiceUnits;	
	m_ServiceUnits.StartService();

	while(true)
	{
		Sleep(10000000000);
	}


	return FALSE;
}


//控制台
CConsoleAdapter g_ConsoleAdpter;