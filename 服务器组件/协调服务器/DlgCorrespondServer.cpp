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
