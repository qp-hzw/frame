#include "ServiceUnits.h"
#include "CorrespondServer.h"
#include "DlgCorrespondServer.h"

//程序对象
CCorrespondServerApp theApp;

BEGIN_MESSAGE_MAP(CCorrespondServerApp, CWinApp)
END_MESSAGE_MAP()

//启动函数
BOOL CCorrespondServerApp::InitInstance()
{
	__super::InitInstance();

	//设置组件
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();

	//显示窗口
	CCorrespondServerDlg CorrespondServerDlg;
	m_pMainWnd=&CorrespondServerDlg;
	CorrespondServerDlg.DoModal();

	return FALSE;
}

//控制台
CConsoleAdapter g_ConsoleAdpter;