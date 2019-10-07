#include "Stdafx.h"
#include "Resource.h"
#include "DlgServerItem.h"

//////////////////////////////////////////////////////////////////////////////////

//列表属性
#define LIST_STYTE LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_FLATSB

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgServerItem, CDialog)

	//按钮消息
	ON_BN_CLICKED(IDC_LOAD_SERVER, OnBnClickedLoadServer)
	ON_BN_CLICKED(IDC_RELOAD_LIST, OnBnClickedReLoadList)
	ON_BN_CLICKED(IDC_DELETE_SERVER, OnBnClickedDeleteServer)

	//控件消息
	ON_NOTIFY(NM_DBLCLK, IDC_SERVER_LIST, OnNMDblclkServerList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_SERVER_LIST, OnLvnItemChangedServerList)

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDlgServerItem::CDlgServerItem() : CDialog(IDD_SERVER_ITEM)
{
	//模块参数
	ZeroMemory(&m_ModuleInitParameter,sizeof(m_ModuleInitParameter));

	return;
}

//析构函数
CDlgServerItem::~CDlgServerItem()
{
}

//控件绑定
VOID CDlgServerItem::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SERVER_LIST, m_ServerListControl);
}

//消息解释
BOOL CDlgServerItem::PreTranslateMessage(MSG * pMsg)
{
	//按键过滤
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//初始化函数
BOOL CDlgServerItem::OnInitDialog()
{
	__super::OnInitDialog();

	//设置资源
	AfxSetResourceHandle(GetModuleHandle(NULL));

	//加载房间
	LoadDBServerItem();

	return TRUE;
}

//确定消息
VOID CDlgServerItem::OnOK()
{
	return;
}

//打开房间
bool CDlgServerItem::OpenGameServer()
{
	//设置资源
	AfxSetResourceHandle(GetModuleHandle(MODULE_MANAGER_DLL_NAME));

	//配置房间
	if (DoModal()==IDC_LOAD_SERVER)
	{
		return true;
	}

	return false;
}

//打开房间
bool CDlgServerItem::OpenGameServer(DWORD dwServerID)
{
	//变量定义
	tagGameRoomItem tagGameRoomItem;

	//机器标识
	TCHAR szMachineID[LEN_MACHINE_ID] = { 0 };
	CWHService::GetMachineID(szMachineID);

	//加载房间
	CServerInfoManager ServerInfoManager;
	if (ServerInfoManager.LoadGameServerInfo(szMachineID,dwServerID,tagGameRoomItem)==false)
	{
		return false;
	}

	//获取参数
	if (GetModuleInitParameter(&tagGameRoomItem,true)==false)
	{
		return false;
	}

	return true;
}

//加载列表
bool CDlgServerItem::LoadDBServerItem()
{
	//重置列表
	m_ServerListControl.DeleteAllItems();

	//设置按钮
	GetDlgItem(IDC_LOAD_SERVER)->EnableWindow(FALSE);
	GetDlgItem(IDC_DELETE_SERVER)->EnableWindow(FALSE);

	//机器标识
	TCHAR szMachineID[LEN_MACHINE_ID] = { 0 };
	//CWHService::GetMachineID(szMachineID);

	//加载信息
	CServerInfoManager ServerInfoManager;
	if (ServerInfoManager.LoadGameServerInfo(szMachineID,m_ServerItemMap)==true)
	{
		//变量定义
		POSITION Position=m_ServerItemMap.m_GameServerInfoMap.GetStartPosition();

		//枚举模块
		while (Position!=NULL)
		{
			//获取对象
			DWORD dwServerID=0L;
			tagGameRoomItem * pGameServerInfo=NULL;
			m_ServerItemMap.m_GameServerInfoMap.GetNextAssoc(Position,dwServerID,pGameServerInfo);

			//插入列表
			ASSERT(pGameServerInfo!=NULL);
			if (pGameServerInfo!=NULL) m_ServerListControl.InsertServerInfo(pGameServerInfo);
		}

		return true;
	}

	return false;
}

//更新按钮
bool CDlgServerItem::UpdateControlStatus()
{
	//获取选择
	POSITION Position=m_ServerListControl.GetFirstSelectedItemPosition();

	//设置列表
	if (Position!=NULL)
	{
		//获取房间
		INT nListItem=m_ServerListControl.GetNextSelectedItem(Position);
		DWORD *pServerID = (DWORD*) m_ServerListControl.GetItemData(nListItem);
		DWORD dwServerID = *pServerID;
		tagGameRoomItem * pGameServerInfo =  m_ServerItemMap.SearchServerInfo(dwServerID);

		//设置按钮
		if (pGameServerInfo->dwNativeVersion!=0L)
		{
			GetDlgItem(IDC_LOAD_SERVER)->EnableWindow(TRUE);
			GetDlgItem(IDC_DELETE_SERVER)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_LOAD_SERVER)->EnableWindow(FALSE);
			GetDlgItem(IDC_DELETE_SERVER)->EnableWindow(TRUE);
		}
	}
	else
	{
		//设置按钮
		GetDlgItem(IDC_LOAD_SERVER)->EnableWindow(FALSE);
		GetDlgItem(IDC_DELETE_SERVER)->EnableWindow(FALSE);
	}

	return true;
}

//获取参数
bool CDlgServerItem::GetModuleInitParameter(tagGameRoomItem * pGameServerInfo, bool bAutoMode)
{
	//效验参数
	if (pGameServerInfo==NULL) return false;

	//游戏模块
	CGameServiceManagerHelper GameServiceManager;
	GameServiceManager.SetModuleCreateInfo(pGameServerInfo->szDLLName,GAME_SERVICE_CREATE_NAME);

	//创建判断
	if (pGameServerInfo->dwNativeVersion==0)
	{
		return false;
	}

	//加载模块
	if (GameServiceManager.CreateInstance()==false)
	{
		return false;
	}

	//模块属性
	GameServiceManager->GetServiceAttrib(m_ModuleInitParameter.GameServiceAttrib);

	//索引标志
	m_ModuleInitParameter.GameServiceOption.dwServerID=pGameServerInfo->dwServerID;
	lstrcpyn(m_ModuleInitParameter.GameServiceOption.szServerName,pGameServerInfo->szServerName,LEN_SERVER);
	//游戏服端口
	m_ModuleInitParameter.GameServiceOption.wGameServerPort=pGameServerInfo->wGameServerPort;

	//游戏服地址
	lstrcpyn(m_ModuleInitParameter.GameServiceOption.szGameServerAddr,pGameServerInfo->szGameServerAddr,LEN_DB_ADDR);

	return true;
}

//加载游戏房间
VOID CDlgServerItem::OnBnClickedLoadServer()
{
	//获取选择
	POSITION Position=m_ServerListControl.GetFirstSelectedItemPosition();
	
	//获取房间
	INT nListItem=m_ServerListControl.GetNextSelectedItem(Position);
	DWORD *pServerID = (DWORD*) m_ServerListControl.GetItemData(nListItem);
	DWORD dwServerID = *pServerID;
	tagGameRoomItem * pGameServerInfo =  m_ServerItemMap.SearchServerInfo(dwServerID);

	//设置变量
	if ((pGameServerInfo!=NULL)&&(GetModuleInitParameter(pGameServerInfo,false)==true))
	{
		EndDialog(IDC_LOAD_SERVER);
	}

	return;
}

//刷新列表
VOID CDlgServerItem::OnBnClickedReLoadList()
{
	//加载列表
	LoadDBServerItem();

	return;
}

//删除房间
VOID CDlgServerItem::OnBnClickedDeleteServer()
{
	//获取选择
	ASSERT(m_ServerListControl.GetFirstSelectedItemPosition()!=NULL);
	POSITION Position=m_ServerListControl.GetFirstSelectedItemPosition();
	
	//获取房间
	INT nListItem=m_ServerListControl.GetNextSelectedItem(Position);
	DWORD *pServerID = (DWORD*) m_ServerListControl.GetItemData(nListItem);
	DWORD dwServerID = *pServerID;
	tagGameRoomItem * pGameServerInfo =  m_ServerItemMap.SearchServerInfo(dwServerID);

	//设置变量
	if (pGameServerInfo!=NULL)
	{
		//构造提示
		TCHAR szString[128]=TEXT("");
		_sntprintf_s(szString,CountArray(szString),TEXT("确实要删除 [ %s ] 游戏房间吗？"),pGameServerInfo->szServerName);

		//提示消息
		if (AfxMessageBox(szString,MB_ICONQUESTION|MB_YESNO)!=IDYES) return;

		//删除房间
		CServerInfoManager ServerInfoManager;
		if (ServerInfoManager.DeleteGameServer(pGameServerInfo->dwServerID)==false) return;

		//删除列表
		m_ServerListControl.DeleteItem(nListItem);
		m_ServerItemMap.DeleteServerInfo(pGameServerInfo->dwServerID);

		//设置列表
		if (m_ServerListControl.GetItemCount()>0)
		{
			//设置变量
			INT nItemCount=m_ServerListControl.GetItemCount();
			INT nNextItem=(nListItem>=nItemCount)?(nItemCount-1):nListItem;

			//设置选择
			m_ServerListControl.SetItemState(nNextItem,LVIS_SELECTED,LVIS_SELECTED);
		}

		//更新控制
		UpdateControlStatus();
	}

	return;
}

//双击列表
VOID CDlgServerItem::OnNMDblclkServerList(NMHDR * pNMHDR, LRESULT * pResult)
{
	//加载配置
	if (((NMITEMACTIVATE *)pNMHDR)->iItem!=LB_ERR)
	{
		OnBnClickedLoadServer();
	}

	return;
}

//选择改变
VOID CDlgServerItem::OnLvnItemChangedServerList(NMHDR * pNMHDR, LRESULT * pResult)
{
	//更新控制
	UpdateControlStatus();

	return;
}

//////////////////////////////////////////////////////////////////////////////////
