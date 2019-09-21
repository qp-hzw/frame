// DlgLogon.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LogonServer.h"
#include "DlgLogon.h"
#include "afxdialogex.h"


// DlgLogon �Ի���

IMPLEMENT_DYNAMIC(DlgLogon, CDialog)

DlgLogon::DlgLogon(CWnd* pParent /*=NULL*/)
	: CDialog(DlgLogon::IDD, pParent)
{
	m_ServiceUnits.StartService();
}

DlgLogon::~DlgLogon()
{
	m_ServiceUnits.ConcludeService();
}

void DlgLogon::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_TRACE_MESSAGE, m_TraceServiceControl);

	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgLogon, CDialog)
END_MESSAGE_MAP()


// DlgLogon ��Ϣ�������
