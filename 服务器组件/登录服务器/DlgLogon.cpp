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



BEGIN_MESSAGE_MAP(DlgLogon, CDialog)
END_MESSAGE_MAP()


// DlgLogon ��Ϣ�������
