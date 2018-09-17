#include "Stdafx.h"
#include "Resource.h"
#include "DlgGameServer.h"

#include "..\..\������\�������\WHService.h"
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
	ON_BN_CLICKED(IDC_STOP_SERVICE, OnBnClickedStopService)
	ON_BN_CLICKED(IDC_START_SERVICE, OnBnClickedStartService)
	ON_BN_CLICKED(IDC_CREATE_SERVER, OnBnClickedCreateServer)

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CGameServerDlg::CGameServerDlg() : CDialog(IDD_DLG_GAME_SERVER)
{
	//���ò���
	m_bAutoControl=false;
	m_bOptionSuccess=false;
	m_bQuit = false;
	ZeroMemory(&m_ModuleInitParameter,sizeof(m_ModuleInitParameter));
}

//��������
CGameServerDlg::~CGameServerDlg()
{
}

//�ؼ���
VOID CGameServerDlg::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TRACE_MESSAGE, m_TraceServiceControl);
}

//��ʼ������
BOOL CGameServerDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//���ñ���
	SetWindowText(TEXT("��Ϸ������ -- [ ֹͣ ]"));

    //����mystery
    SetDlgItemInt(IDC_STATIC_MYSTERY, _MYSTERY);

	//����ͼ��
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//�������
	m_ServiceUnits.SetServiceUnitsSink(this);

	//�����
	LPCTSTR pszCmdLine=AfxGetApp()->m_lpCmdLine;
	if (pszCmdLine[0]!=0) PostMessage(WM_PROCESS_CMD_LINE,0,(LPARAM)pszCmdLine);

	return TRUE;
}

//ȷ����Ϣ
VOID CGameServerDlg::OnOK()
{
	return;
}

//ȡ������
VOID CGameServerDlg::OnCancel()
{
	//�ر�ѯ��
 	if (m_ServiceUnits.GetServiceStatus() != ServiceStatus_Stop &&
		m_ServiceUnits.GetServiceStatus() != ServiceStatus_Stopping)
 	{
 		LPCTSTR pszQuestion=TEXT("��Ϸ���������������У���ȷʵҪ�رշ�������");
 		if (AfxMessageBox(pszQuestion,MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION)!=IDYES) return;
		m_ServiceUnits.ConcludeService();

		m_bQuit = true;
	}
	else
	{		
		__super::OnCancel();
	}
}

//��Ϣ����
BOOL CGameServerDlg::PreTranslateMessage(MSG * pMsg)
{
	//��������
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//����״̬
VOID CGameServerDlg::OnServiceUnitsStatus(enServiceStatus ServiceStatus)
{
	//״̬����
	switch (ServiceStatus)
	{
	case ServiceStatus_Stopping:
		{
			//���±���
			UpdateServerTitle(ServiceStatus);

			//����ť
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(FALSE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);

			//���ð�ť
			GetDlgItem(IDC_OPEN_SERVER)->EnableWindow(FALSE);
			GetDlgItem(IDC_CREATE_SERVER)->EnableWindow(FALSE);

			//��ʾ��Ϣ
			LPCTSTR pszDescribe=TEXT("ֹͣ��������");
			CTraceService::TraceString(pszDescribe,TraceLevel_Normal);

			break;
		}
	case ServiceStatus_Stop:	//ֹͣ״̬
		{
			//���±���
			UpdateServerTitle(ServiceStatus);

			//����ť
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(FALSE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(TRUE);

			//���ð�ť
			GetDlgItem(IDC_OPEN_SERVER)->EnableWindow(TRUE);
			GetDlgItem(IDC_CREATE_SERVER)->EnableWindow(TRUE);

			//��ʾ��Ϣ
			LPCTSTR pszDescribe=TEXT("����ֹͣ�ɹ�");
			CTraceService::TraceString(pszDescribe,TraceLevel_Normal);

			if(m_bQuit)
			{
				__super::OnCancel();
			}

			break;
		}
	case ServiceStatus_Config:	//����״̬
		{
			//���±���
			UpdateServerTitle(ServiceStatus);

			//���ð�ť
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);

			//���ð�ť
			GetDlgItem(IDC_OPEN_SERVER)->EnableWindow(FALSE);
			GetDlgItem(IDC_CREATE_SERVER)->EnableWindow(FALSE);

			//��ʾ��Ϣ
			LPCTSTR pszDescribe=TEXT("���ڳ�ʼ�����...");
			CTraceService::TraceString(pszDescribe,TraceLevel_Normal);

			break;
		}
	case ServiceStatus_Service:	//����״̬
		{
			//���±���
			UpdateServerTitle(ServiceStatus);

			//����ť
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);

			//���ð�ť
			GetDlgItem(IDC_OPEN_SERVER)->EnableWindow(FALSE);
			GetDlgItem(IDC_CREATE_SERVER)->EnableWindow(FALSE);

			//��ʾ��Ϣ
			LPCTSTR pszDescribe=TEXT("���������ɹ�");
			CTraceService::TraceString(pszDescribe,TraceLevel_Normal);

			break;
		}
	}

	return;
}

//���±���
VOID CGameServerDlg::UpdateServerTitle(enServiceStatus ServiceStatus)
{
	//��������
	LPCTSTR pszStatusName=NULL;
	LPCTSTR pszServerName=NULL;

	//״̬�ַ�
	switch (ServiceStatus)
	{
	case ServiceStatus_Stopping:	//ֹͣ״̬
		{
			pszStatusName=TEXT("����ֹͣ");
			break;
		}
	case ServiceStatus_Stop:	//ֹͣ״̬
		{
			pszStatusName=TEXT("ֹͣ");
			break;
		}
	case ServiceStatus_Config:	//����״̬
		{
			pszStatusName=TEXT("��ʼ��");
			break;
		}
	case ServiceStatus_Service:	//����״̬
		{
			pszStatusName=TEXT("����");
			break;
		}
	}

	//���ñ���
	TCHAR szTitle[128]=TEXT("");
	pszServerName=m_ModuleInitParameter.GameServiceOption.szServerName;
	_sntprintf_s(szTitle,CountArray(szTitle),TEXT("[ %s Ver:%d.%d.%d-%d ] -- [ %s ]"),
		pszServerName,
		GetCodeVer(m_ModuleInitParameter.GameServiceAttrib.dwSubGameVersion), 
		GetKernelVer(m_ModuleInitParameter.GameServiceAttrib.dwSubGameVersion), 
		GetPlatformVer(m_ModuleInitParameter.GameServiceAttrib.dwSubGameVersion), 
		m_ModuleInitParameter.GameServiceOption.dwServerID, 
		pszStatusName);
	
	SetWindowText(szTitle);

	return;
}

//����״̬
VOID CGameServerDlg::UpdateParameterStatus(tagModuleInitParameter & ModuleInitParameter)
{
	//���ñ���
	m_bOptionSuccess=true;
	m_ModuleInitParameter=ModuleInitParameter;

	//���±���
	UpdateServerTitle(ServiceStatus_Stop);
	
	//���ð�ť
	GetDlgItem(IDC_START_SERVICE)->EnableWindow(TRUE);

	//�����˿�
	if (m_ModuleInitParameter.GameServiceOption.wGameServerPort==0)
	{
		SetDlgItemText(IDC_SERVER_PORT,TEXT("�˿ڲ�����"));
	}
	else
	{
		SetDlgItemInt(IDC_SERVER_PORT,m_ModuleInitParameter.GameServiceOption.wGameServerPort);
	}

	//����ID
	if (m_ModuleInitParameter.GameServiceOption.dwServerID == 0)
	{
		SetDlgItemText(IDC_GAME_NAME,TEXT("����ID������"));
	}
	else
	{
		SetDlgItemInt(IDC_GAME_NAME, m_ModuleInitParameter.GameServiceOption.dwServerID);
	}

	//��������
	if (_T('0') == m_ModuleInitParameter.GameServiceOption.szServerName[0])
	{
		SetDlgItemText(IDC_SERVER_NAME,TEXT("�������ֲ�����"));
	}
	else
	{
		SetDlgItemText(IDC_SERVER_NAME, m_ModuleInitParameter.GameServiceOption.szServerName);
	}
	

	//����ģ��
	m_ServiceUnits.CollocateService(m_ModuleInitParameter.GameServiceAttrib.szDllName,m_ModuleInitParameter.GameServiceOption);

	//������ʾ
	TCHAR szString[256]=TEXT("");
	LPCTSTR pszServerName=m_ModuleInitParameter.GameServiceOption.szServerName;
	_sntprintf_s(szString,CountArray(szString),TEXT("[ %s ] ����������سɹ�"),pszServerName);

	//�����Ϣ
	CTraceService::TraceString(szString,TraceLevel_Normal);

	return;
}

//��������
VOID CGameServerDlg::OnBnClickedStartService()
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
VOID CGameServerDlg::OnBnClickedStopService()
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

//��������
VOID CGameServerDlg::OnBnClickedCreateServer()
{
	//��������
	CDlgServerWizard DlgServerWizard;
	if (DlgServerWizard.CreateGameServer()==false) return;

	//����״̬�����ӶԻ������Ϣ��ֵ����Ϸ���Ի���
	UpdateParameterStatus(DlgServerWizard.m_ModuleInitParameter);

	return;
}


//�ر�ѯ��
BOOL CGameServerDlg::OnQueryEndSession()
{
	//��ʾ��Ϣ
	if (m_ServiceUnits.GetServiceStatus()!=ServiceStatus_Stop)
	{
		CTraceService::TraceString(TEXT("�������������У�ϵͳҪ��ע���ػ�����ʧ��"),TraceLevel_Warning);
		return FALSE;
	}

	return TRUE;
}