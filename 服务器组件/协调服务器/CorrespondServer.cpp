#include "Stdafx.h"
#include "CorrespondServer.h"
#include "DlgCorrespondServer.h"

//////////////////////////////////////////////////////////////////////////////////

//�������
CCorrespondServerApp theApp;

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CCorrespondServerApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CCorrespondServerApp::CCorrespondServerApp()
{
}

//��������
BOOL CCorrespondServerApp::InitInstance()
{
	__super::InitInstance();

	//�������
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();

	//��ʾ����
	CCorrespondServerDlg CorrespondServerDlg;
	m_pMainWnd=&CorrespondServerDlg;
	CorrespondServerDlg.DoModal();
 
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////
