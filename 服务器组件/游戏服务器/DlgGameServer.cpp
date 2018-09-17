#include "Stdafx.h"
#include "Resource.h"
#include "DlgGameServer.h"

#include "..\..\依赖项\服务核心\WHService.h"
#include "..\..\服务器组件\模块管理\DlgServerItem.h"
#include "..\..\服务器组件\模块管理\DlgServerWizard.h"


//////////////////////////////////////////////////////////////////////////////////

//消息定义
#define WM_PROCESS_CMD_LINE			(WM_USER+100)						//处理命令

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameServerDlg, CDialog)

	//系统消息
	ON_WM_QUERYENDSESSION()
	ON_BN_CLICKED(IDC_OPEN_SERVER, OnBnClickedOpenServer)
	ON_BN_CLICKED(IDC_STOP_SERVICE, OnBnClickedStopService)
	ON_BN_CLICKED(IDC_START_SERVICE, OnBnClickedStartService)
	ON_BN_CLICKED(IDC_CREATE_SERVER, OnBnClickedCreateServer)

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CGameServerDlg::CGameServerDlg() : CDialog(IDD_DLG_GAME_SERVER)
{
	//配置参数
	m_bAutoControl=false;
	m_bOptionSuccess=false;
	m_bQuit = false;
	ZeroMemory(&m_ModuleInitParameter,sizeof(m_ModuleInitParameter));
}

//析构函数
CGameServerDlg::~CGameServerDlg()
{
}

//控件绑定
VOID CGameServerDlg::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TRACE_MESSAGE, m_TraceServiceControl);
}

//初始化函数
BOOL CGameServerDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//设置标题
	SetWindowText(TEXT("游戏服务器 -- [ 停止 ]"));

    //设置mystery
    SetDlgItemInt(IDC_STATIC_MYSTERY, _MYSTERY);

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//设置组件
	m_ServiceUnits.SetServiceUnitsSink(this);

	//命令处理
	LPCTSTR pszCmdLine=AfxGetApp()->m_lpCmdLine;
	if (pszCmdLine[0]!=0) PostMessage(WM_PROCESS_CMD_LINE,0,(LPARAM)pszCmdLine);

	return TRUE;
}

//确定消息
VOID CGameServerDlg::OnOK()
{
	return;
}

//取消函数
VOID CGameServerDlg::OnCancel()
{
	//关闭询问
 	if (m_ServiceUnits.GetServiceStatus() != ServiceStatus_Stop &&
		m_ServiceUnits.GetServiceStatus() != ServiceStatus_Stopping)
 	{
 		LPCTSTR pszQuestion=TEXT("游戏服务器正在运行中，您确实要关闭服务器吗？");
 		if (AfxMessageBox(pszQuestion,MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION)!=IDYES) return;
		m_ServiceUnits.ConcludeService();

		m_bQuit = true;
	}
	else
	{		
		__super::OnCancel();
	}
}

//消息解释
BOOL CGameServerDlg::PreTranslateMessage(MSG * pMsg)
{
	//按键过滤
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//服务状态
VOID CGameServerDlg::OnServiceUnitsStatus(enServiceStatus ServiceStatus)
{
	//状态设置
	switch (ServiceStatus)
	{
	case ServiceStatus_Stopping:
		{
			//更新标题
			UpdateServerTitle(ServiceStatus);

			//服务按钮
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(FALSE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);

			//配置按钮
			GetDlgItem(IDC_OPEN_SERVER)->EnableWindow(FALSE);
			GetDlgItem(IDC_CREATE_SERVER)->EnableWindow(FALSE);

			//提示信息
			LPCTSTR pszDescribe=TEXT("停止服务器中");
			CTraceService::TraceString(pszDescribe,TraceLevel_Normal);

			break;
		}
	case ServiceStatus_Stop:	//停止状态
		{
			//更新标题
			UpdateServerTitle(ServiceStatus);

			//服务按钮
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(FALSE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(TRUE);

			//配置按钮
			GetDlgItem(IDC_OPEN_SERVER)->EnableWindow(TRUE);
			GetDlgItem(IDC_CREATE_SERVER)->EnableWindow(TRUE);

			//提示信息
			LPCTSTR pszDescribe=TEXT("服务停止成功");
			CTraceService::TraceString(pszDescribe,TraceLevel_Normal);

			if(m_bQuit)
			{
				__super::OnCancel();
			}

			break;
		}
	case ServiceStatus_Config:	//配置状态
		{
			//更新标题
			UpdateServerTitle(ServiceStatus);

			//设置按钮
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);

			//配置按钮
			GetDlgItem(IDC_OPEN_SERVER)->EnableWindow(FALSE);
			GetDlgItem(IDC_CREATE_SERVER)->EnableWindow(FALSE);

			//提示信息
			LPCTSTR pszDescribe=TEXT("正在初始化组件...");
			CTraceService::TraceString(pszDescribe,TraceLevel_Normal);

			break;
		}
	case ServiceStatus_Service:	//服务状态
		{
			//更新标题
			UpdateServerTitle(ServiceStatus);

			//服务按钮
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);

			//配置按钮
			GetDlgItem(IDC_OPEN_SERVER)->EnableWindow(FALSE);
			GetDlgItem(IDC_CREATE_SERVER)->EnableWindow(FALSE);

			//提示信息
			LPCTSTR pszDescribe=TEXT("服务启动成功");
			CTraceService::TraceString(pszDescribe,TraceLevel_Normal);

			break;
		}
	}

	return;
}

//更新标题
VOID CGameServerDlg::UpdateServerTitle(enServiceStatus ServiceStatus)
{
	//变量定义
	LPCTSTR pszStatusName=NULL;
	LPCTSTR pszServerName=NULL;

	//状态字符
	switch (ServiceStatus)
	{
	case ServiceStatus_Stopping:	//停止状态
		{
			pszStatusName=TEXT("正在停止");
			break;
		}
	case ServiceStatus_Stop:	//停止状态
		{
			pszStatusName=TEXT("停止");
			break;
		}
	case ServiceStatus_Config:	//配置状态
		{
			pszStatusName=TEXT("初始化");
			break;
		}
	case ServiceStatus_Service:	//运行状态
		{
			pszStatusName=TEXT("运行");
			break;
		}
	}

	//设置标题
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

//更新状态
VOID CGameServerDlg::UpdateParameterStatus(tagModuleInitParameter & ModuleInitParameter)
{
	//设置变量
	m_bOptionSuccess=true;
	m_ModuleInitParameter=ModuleInitParameter;

	//更新标题
	UpdateServerTitle(ServiceStatus_Stop);
	
	//设置按钮
	GetDlgItem(IDC_START_SERVICE)->EnableWindow(TRUE);

	//监听端口
	if (m_ModuleInitParameter.GameServiceOption.wGameServerPort==0)
	{
		SetDlgItemText(IDC_SERVER_PORT,TEXT("端口不存在"));
	}
	else
	{
		SetDlgItemInt(IDC_SERVER_PORT,m_ModuleInitParameter.GameServiceOption.wGameServerPort);
	}

	//房间ID
	if (m_ModuleInitParameter.GameServiceOption.dwServerID == 0)
	{
		SetDlgItemText(IDC_GAME_NAME,TEXT("房间ID不存在"));
	}
	else
	{
		SetDlgItemInt(IDC_GAME_NAME, m_ModuleInitParameter.GameServiceOption.dwServerID);
	}

	//房间名称
	if (_T('0') == m_ModuleInitParameter.GameServiceOption.szServerName[0])
	{
		SetDlgItemText(IDC_SERVER_NAME,TEXT("房间名字不存在"));
	}
	else
	{
		SetDlgItemText(IDC_SERVER_NAME, m_ModuleInitParameter.GameServiceOption.szServerName);
	}
	

	//设置模块
	m_ServiceUnits.CollocateService(m_ModuleInitParameter.GameServiceAttrib.szDllName,m_ModuleInitParameter.GameServiceOption);

	//构造提示
	TCHAR szString[256]=TEXT("");
	LPCTSTR pszServerName=m_ModuleInitParameter.GameServiceOption.szServerName;
	_sntprintf_s(szString,CountArray(szString),TEXT("[ %s ] 房间参数加载成功"),pszServerName);

	//输出信息
	CTraceService::TraceString(szString,TraceLevel_Normal);

	return;
}

//启动服务
VOID CGameServerDlg::OnBnClickedStartService()
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
VOID CGameServerDlg::OnBnClickedStopService()
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

//打开房间
VOID CGameServerDlg::OnBnClickedOpenServer()
{
	//配置房间
	CDlgServerItem DlgServerItem;
	if (DlgServerItem.OpenGameServer()==false) return;

	//更新状态
	UpdateParameterStatus(DlgServerItem.m_ModuleInitParameter);

	return;
}

//创建房间
VOID CGameServerDlg::OnBnClickedCreateServer()
{
	//创建房间
	CDlgServerWizard DlgServerWizard;
	if (DlgServerWizard.CreateGameServer()==false) return;

	//更新状态（将子对话框的信息赋值给游戏服对话框）
	UpdateParameterStatus(DlgServerWizard.m_ModuleInitParameter);

	return;
}


//关闭询问
BOOL CGameServerDlg::OnQueryEndSession()
{
	//提示消息
	if (m_ServiceUnits.GetServiceStatus()!=ServiceStatus_Stop)
	{
		CTraceService::TraceString(TEXT("服务正在运行中，系统要求注销回话请求失败"),TraceLevel_Warning);
		return FALSE;
	}

	return TRUE;
}