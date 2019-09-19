#include "Stdafx.h"
#include "Resource.h"
#include "DlgCorrespondServer.h"

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CCorrespondServerDlg, CDialog)
	ON_WM_QUERYENDSESSION()
	ON_BN_CLICKED(IDC_STOP_SERVICE, OnBnClickedStopService)
	ON_BN_CLICKED(IDC_START_SERVICE, OnBnClickedStartService)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CCorrespondServerDlg::CCorrespondServerDlg() : CDialog(IDD_DLG_CORRESPOND_SERVER)
{
	 //�Զ�����
    OnBnClickedStartService();
}

//��������
CCorrespondServerDlg::~CCorrespondServerDlg()
{
	//ֹͣ����
	OnBnClickedStopService();
}

//�ؼ���
VOID CCorrespondServerDlg::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TRACE_MESSAGE, m_TraceServiceControl);
}


//��������
VOID CCorrespondServerDlg::OnBnClickedStartService()
{
	//��������
	try
	{
		m_ServiceUnits.StartService();
	}
	catch (...)
	{
		ASSERT(FALSE);
	}

	return;
}

//ֹͣ����
VOID CCorrespondServerDlg::OnBnClickedStopService()
{
	//ֹͣ����
	try
	{
		m_ServiceUnits.ConcludeService();
	}
	catch (...)
	{
		ASSERT(FALSE);
	}

	return;
}
