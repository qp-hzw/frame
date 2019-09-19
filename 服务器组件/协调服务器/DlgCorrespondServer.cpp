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

//构造函数
CCorrespondServerDlg::CCorrespondServerDlg() : CDialog(IDD_DLG_CORRESPOND_SERVER)
{
	 //自动启动
    OnBnClickedStartService();
}

//析构函数
CCorrespondServerDlg::~CCorrespondServerDlg()
{
	//停止服务
	OnBnClickedStopService();
}

//控件绑定
VOID CCorrespondServerDlg::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TRACE_MESSAGE, m_TraceServiceControl);
}


//启动服务
VOID CCorrespondServerDlg::OnBnClickedStartService()
{
	//启动服务
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

//停止服务
VOID CCorrespondServerDlg::OnBnClickedStopService()
{
	//停止服务
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
