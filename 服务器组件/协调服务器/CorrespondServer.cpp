#include "ServiceUnits.h"
#include "CorrespondServer.h"

//�������
CCorrespondServerApp theApp;

//��������
BOOL CCorrespondServerApp::InitInstance()
{
	__super::InitInstance();

	//�������
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();

	CServiceUnits m_ServiceUnits;
	m_ServiceUnits.StartService();

	//��������
	while(true)
	{
		Sleep(10000000000);
	}

	return FALSE;
}

//����̨
CConsoleAdapter g_ConsoleAdpter;