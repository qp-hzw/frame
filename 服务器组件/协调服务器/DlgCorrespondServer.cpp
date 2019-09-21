#include "Stdafx.h"
#include "Resource.h"
#include "DlgCorrespondServer.h"

//构造函数
CCorrespondServerDlg::CCorrespondServerDlg() : CDialog(IDD_DLG_CORRESPOND_SERVER)
{
	m_ServiceUnits.StartService();
}

//析构函数
CCorrespondServerDlg::~CCorrespondServerDlg()
{
	m_ServiceUnits.ConcludeService();
}

//控件绑定
VOID CCorrespondServerDlg::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TRACE_MESSAGE, m_TraceServiceControl);
}
