#include "ServiceUnits.h"
#include "CorrespondServer.h"

//�������
CCorrespondServerApp theApp;

BEGIN_MESSAGE_MAP(CCorrespondServerApp, CWinApp)
END_MESSAGE_MAP()

//��������
BOOL CCorrespondServerApp::InitInstance()
{
	__super::InitInstance();

	//�������
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();


	//��������
	CServiceUnits m_ServiceUnits;
	m_ServiceUnits.StartService();

	return FALSE;
}

//����̨
CConsoleAdapter g_ConsoleAdpter;