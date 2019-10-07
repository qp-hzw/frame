#include "Stdafx.h"
#include "Resource.h"
#include "DlgGameServer.h"

#include "..\..\���������\ģ�����\DlgServerItem.h"
#include "..\..\���������\ģ�����\DlgServerWizard.h"


//////////////////////////////////////////////////////////////////////////////////

//��Ϣ����
#define WM_PROCESS_CMD_LINE			(WM_USER+100)						//��������

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameServerDlg, CDialog)

	//ϵͳ��Ϣ
	ON_WM_QUERYENDSESSION()
	ON_BN_CLICKED(IDC_OPEN_SERVER, OnBnClickedOpenServer)
	ON_BN_CLICKED(IDC_CREATE_SERVER, OnBnClickedCreateServer)

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CGameServerDlg::CGameServerDlg() : CDialog(IDD_DLG_GAME_SERVER)
{
	ZeroMemory(&m_ModuleInitParameter,sizeof(m_ModuleInitParameter));

	m_ServiceUnits.StartService();
}

//��������
CGameServerDlg::~CGameServerDlg()
{
	m_ServiceUnits.ConcludeService();
}

//����״̬
VOID CGameServerDlg::UpdateParameterStatus(tagModuleInitParameter & ModuleInitParameter)
{
	//���ñ���
	m_ModuleInitParameter=ModuleInitParameter;	

	//����ģ��
	m_ServiceUnits.CollocateService(m_ModuleInitParameter.GameServiceAttrib.szDllName,m_ModuleInitParameter.GameServiceOption);
	return;
}

//�򿪷���
VOID CGameServerDlg::OnBnClickedOpenServer()
{
	//���÷���
	CDlgServerItem DlgServerItem;
	if (DlgServerItem.OpenGameServer()==false) return;

	//����״̬
	UpdateParameterStatus(DlgServerItem.m_ModuleInitParameter);

	return;
}