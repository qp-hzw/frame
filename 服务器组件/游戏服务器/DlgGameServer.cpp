#include "Stdafx.h"
#include "Resource.h"
#include "DlgGameServer.h"

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
	ON_BN_CLICKED(IDC_CREATE_SERVER, OnBnClickedCreateServer)

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CGameServerDlg::CGameServerDlg() : CDialog(IDD_DLG_GAME_SERVER)
{
	ZeroMemory(&m_ModuleInitParameter,sizeof(m_ModuleInitParameter));

	m_ServiceUnits.StartService();
}

//析构函数
CGameServerDlg::~CGameServerDlg()
{
	m_ServiceUnits.ConcludeService();
}

//更新状态
VOID CGameServerDlg::UpdateParameterStatus(tagModuleInitParameter & ModuleInitParameter)
{
	//设置变量
	m_ModuleInitParameter=ModuleInitParameter;

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