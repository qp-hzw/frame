#include "Stdafx.h"
#include "Resource.h"
#include "DlgCorrespondServer.h"

//���캯��
CCorrespondServerDlg::CCorrespondServerDlg() : CDialog(IDD_DLG_CORRESPOND_SERVER)
{
	m_ServiceUnits.StartService();
}

//��������
CCorrespondServerDlg::~CCorrespondServerDlg()
{
	m_ServiceUnits.ConcludeService();
}

//�ؼ���
VOID CCorrespondServerDlg::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TRACE_MESSAGE, m_TraceServiceControl);
}
