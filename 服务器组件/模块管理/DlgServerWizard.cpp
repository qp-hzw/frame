#include "StdAfx.h"
#include "Resource.h"
#include "DlgServerWizard.h"
#include ".\dlgserverwizard.h"

#include "//www.45quyou.com/share\依赖项\服务核心\WHService.h"
#include "//www.45quyou.com/share\依赖项\内核引擎\TraceService.h"
#include "//www.45quyou.com/share\依赖项\服务核心\WHIniData.h"
#include "..\..\全局定义\Struct.h"
#include "//www.45quyou.com/share\依赖项\服务核心\WHEncrypt.h"

#pragma region DECLARE_MESSAGE_MAP
BEGIN_MESSAGE_MAP(CDlgServerWizardItem, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgServerWizardItem1, CDlgServerWizardItem)
	ON_NOTIFY(NM_DBLCLK, IDC_MODULE_LIST, OnNMDblclkModuleList)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgServerWizardItem2, CDlgServerWizardItem)
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgServerWizard, CDialog)
	ON_BN_CLICKED(IDC_LAST, OnBnClickedLast)
	ON_BN_CLICKED(IDC_NEXT, OnBnClickedNext)
	ON_BN_CLICKED(IDC_FINISH, OnBnClickedFinish)
END_MESSAGE_MAP()

#pragma endregion

#pragma region CDlgServerWizardItem
//构造函数
CDlgServerWizardItem::CDlgServerWizardItem(UINT nIDTemplate) : CDialog(nIDTemplate)
{
	//设置变量
	m_pDlgServerWizard=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	return;
}

//析构函数
CDlgServerWizardItem::~CDlgServerWizardItem()
{
}

//保存数据
bool CDlgServerWizardItem::SaveItemData()
{
	//保存数据
	if ((m_hWnd!=NULL)&&(SaveInputInfo()==false))
	{
		return false;
	}

	return true;
}

//创建向导
bool CDlgServerWizardItem::ShowWizardItem(const CRect & rcRect, CWnd * pParentWnd)
{
	//创建窗口
	if (m_hWnd==NULL) 
	{
		//设置资源
		AfxSetResourceHandle(GetModuleHandle(MODULE_MANAGER_DLL_NAME));

		//创建窗口
		Create(m_lpszTemplateName,pParentWnd);
		SetWindowPos(NULL,rcRect.left,rcRect.top,rcRect.Width(),rcRect.Height(),SWP_NOZORDER|SWP_NOACTIVATE);

		//设置资源
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}

	//显示窗口
	ShowWindow(SW_SHOW);

	return true;
}

//确定函数
VOID CDlgServerWizardItem::OnOK()
{ 
	//投递消息
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDOK,0),0);

	return;
}

//取消消息
VOID CDlgServerWizardItem::OnCancel()
{ 
	//投递消息
	GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDCANCEL,0),0);

	return;
}

//位置消息
VOID CDlgServerWizardItem::OnSize(UINT nType, INT cx, INT cy)
{
	__super::OnSize(nType, cx, cy);

	//调整控件
	RectifyControl(cx,cy);

	return;
}
#pragma endregion

#pragma region CDlgServerWizardItem1
/* *******************************************************************************
** CDlgServerWizardItem 1      
** ******************************************************************************/
//构造函数
CDlgServerWizardItem1::CDlgServerWizardItem1() : CDlgServerWizardItem(IDD_SERVER_WIZARD_1)
{
}

//控件绑定
VOID CDlgServerWizardItem1::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROMPT, m_StaticPrompt);
	DDX_Control(pDX, IDC_MODULE_LIST, m_ModuleListControl);
}

//初始化函数
BOOL CDlgServerWizardItem1::OnInitDialog()
{
	__super::OnInitDialog();

	//加载游戏模块列表
	LoadDBModuleItem();

	return TRUE;
}

//保存输入
bool CDlgServerWizardItem1::SaveInputInfo()
{	
	POSITION Position=m_ModuleListControl.GetFirstSelectedItemPosition();
	tagGameGame * pGameGameItem = NULL;

	//获取选择
	if (Position!=NULL)
	{
		INT nListItem=m_ModuleListControl.GetNextSelectedItem(Position);

		DWORD *pGameID = (DWORD*) m_ModuleListControl.GetItemData(nListItem);
		DWORD dwGameID = *pGameID;
		pGameGameItem =  m_GameItemMap.SearchModuleInfo(dwGameID);
	}

	//选择判断
	if (pGameGameItem == NULL)
	{
		AfxMessageBox(TEXT("请您先从游戏列表中选择游戏组件"),MB_ICONERROR);
		return false;
	}

	//游戏模块
	m_GameServiceManager.CloseInstance();
	m_GameServiceManager.SetModuleCreateInfo(pGameGameItem->szDLLName,GAME_SERVICE_CREATE_NAME);

	//加载模块，CreateInstance加载子游戏dll
	if (m_GameServiceManager.CreateInstance()==false)
	{
		//构造提示
		TCHAR szString[512]=TEXT("");
		_sntprintf_s(szString,CountArray(szString),TEXT("[ %s ] 服务组件加载失败，创建游戏房间失败"),pGameGameItem->szGameName);

		//提示消息
		AfxMessageBox(szString,MB_ICONERROR);

		return false;
	}

	//配置模块 -- 此处才开始实例化m_pGameServiceOption 与 m_pGameServiceAttrib
	m_pDlgServerWizard->SetWizardParameter(m_GameServiceManager.GetInterface(),NULL, pGameGameItem->szGameName);

	//配置模块（加载dll）之后, 进行版本校验  子游戏中的框架版本 与 真实的框架版本
	DWORD realFrameVersion = Get_Framework_Version(PLATFORM_VERSION);
	DWORD subGameFrameVersion = m_pGameServiceAttrib->dwSubGameVersion;
	
	//TODONOW 暂时取消这个判断 
	/*
	if ( 0 != Compare_Dll_Framework(realFrameVersion, subGameFrameVersion))
	{
		TCHAR pszString2[512]=TEXT("");
		_sntprintf_s(pszString2,CountArray(pszString2),TEXT("dll加载失败, 框架版本不匹配, realFrame: %ld; subGameFrame: %ld\n"),
					realFrameVersion,
					subGameFrameVersion);
		CTraceService::TraceString(pszString2,TraceLevel_Exception);

		return false; 
	}
	*/

	//m_pGameServiceOption第一次赋值, 数据来源于数据库
	m_pGameServiceOption->dwServerID = GetServerID();
	memcpy(m_pGameServiceOption->szServerName, pGameGameItem->szGameName, sizeof(m_pGameServiceOption->szServerName));//此处的serverName实为KindName
	m_pGameServiceOption->wGameServerPort = GetServerPort();
	memcpy(m_pGameServiceOption->szGameServerAddr, pGameGameItem->szGameServerAddr, sizeof(m_pGameServiceOption->szGameServerAddr));
	
	return true;
}
//自动获取ServerID
DWORD CDlgServerWizardItem1::GetServerID()
{
	//added by WangChengQing 设置ServerID
	//1. 读取所有Sql中所有的ServerID
	//2. 判断(当前ServerID的GameID) 与  读取到的ServerID 对比
	//3. 找到 最大的RoomID
	//4. 最大的RoomId +1
	//5. 配置最新的ServerID
	CServerInfoManager ServerInfoManager;
	CServerItemManager ServerInfoBuffer;

	if (ServerInfoManager.LoadGameServerInfo(NULL,ServerInfoBuffer)==false)
	{
		//提示消息
		AfxMessageBox(TEXT("创建ServerID失败"),MB_ICONERROR);
		return false;
	}

	//寻找同GameID 最大的RoomID
	BYTE byMaxRoomID = 0;
	POSITION Position=ServerInfoBuffer.m_GameServerInfoMap.GetStartPosition();
	//枚举模块
	while (Position!=NULL)
	{
		//获取对象
		DWORD wServerID=0L;
		tagGameRoomItem * pGameServerInfo=NULL;
		ServerInfoBuffer.m_GameServerInfoMap.GetNextAssoc(Position,wServerID,pGameServerInfo);

		//循环判断
		ASSERT(pGameServerInfo!=NULL);
		if (pGameServerInfo!=NULL)
		{
			DWORD dwGameID = (pGameServerInfo->dwServerID & 0xFFFFFF00);
			BYTE  byRoomID = pGameServerInfo->dwServerID & 0xFF;
			if((dwGameID == m_pGameServiceAttrib->dwGameID) && (byRoomID > byMaxRoomID))
			{
				byMaxRoomID = byRoomID;
			}
		}
	}
	byMaxRoomID += 1;
	DWORD dwServerID = (m_pGameServiceAttrib->dwGameID) | byMaxRoomID;

	return dwServerID;
}
//自动获取ServerPort
WORD CDlgServerWizardItem1::GetServerPort()
{
	//added by WangChengQing 设置ServerID
	//1. 读取所有Sql中所有的ServerID的端口
	//2. 过滤不符合规则的端口
	//3. 找到 最小的port 和 最大的port
	//4. 最小的 +1 或者 最大的port -1
	//5. 配置最新的port
	CServerInfoManager ServerInfoManager;
	CServerItemManager ServerInfoBuffer;

	if (ServerInfoManager.LoadGameServerInfo(NULL,ServerInfoBuffer)==false)
	{
		//提示消息
		AfxMessageBox(TEXT("创建ServerPort失败"),MB_ICONERROR);
		return false;
	}

	//寻找最大 和  最小的ServerPort
	WORD wMinServerPort = _MAX_SERVER_PORT;
	WORD wMaxServerPort = _MIN_SERVER_PORT;

	POSITION Position=ServerInfoBuffer.m_GameServerInfoMap.GetStartPosition();
	//枚举模块
	while (Position!=NULL)
	{
		//获取对象
		DWORD wServerID=0L;
		tagGameRoomItem * pGameServerInfo=NULL;
		ServerInfoBuffer.m_GameServerInfoMap.GetNextAssoc(Position,wServerID,pGameServerInfo);

		//循环判断
		if (pGameServerInfo!=NULL)
		{
			//数据校验 port不正确则 continue
			if(pGameServerInfo->wGameServerPort < _MIN_SERVER_PORT
			|| pGameServerInfo->wGameServerPort > _MAX_SERVER_PORT)
			{
				continue;
			}
			//获取最小的port
			wMinServerPort = (pGameServerInfo->wGameServerPort < wMinServerPort) ? pGameServerInfo->wGameServerPort : wMinServerPort;
			
			//获取最大的port
			wMaxServerPort = (pGameServerInfo->wGameServerPort > wMaxServerPort) ? pGameServerInfo->wGameServerPort : wMaxServerPort;
		}
	}

	//数据校验 -- 保证有可用数据
	if((wMinServerPort < _MIN_SERVER_PORT) || (wMaxServerPort > _MAX_SERVER_PORT) )
	{
		//提示消息
		AfxMessageBox(TEXT("配置ServerPort失败, 数据异常"),MB_ICONERROR);
		return false;
	}
	if( (wMinServerPort ==_MIN_SERVER_PORT) && (wMaxServerPort == _MAX_SERVER_PORT ))
	{
		//提示消息
		AfxMessageBox(TEXT("配置ServerPort失败, 端口已满"),MB_ICONERROR);
		return false;
	}

	WORD wServerPort = 0;
	//优先选用 低端口
	if(wMinServerPort > _MIN_SERVER_PORT)
	{
		wServerPort = wMinServerPort-1;
	}
	else
	{
		wServerPort = wMaxServerPort-1;
	}	

	return wServerPort;
}
//调整控件
VOID CDlgServerWizardItem1::RectifyControl(INT nWidth, INT nHeight)
{
	//调整提示
	if (m_StaticPrompt.m_hWnd!=NULL)
	{
		m_StaticPrompt.SetWindowPos(NULL,5,8,nWidth-10,12,SWP_NOZORDER|SWP_NOCOPYBITS);
	}

	//调整列表
	if (m_ModuleListControl.m_hWnd!=NULL)
	{
		m_ModuleListControl.SetWindowPos(NULL,5,28,nWidth-10,nHeight-28,SWP_NOZORDER|SWP_NOCOPYBITS);
	}

	return;
}

//加载列表
bool CDlgServerWizardItem1::LoadDBModuleItem()
{
	//加载信息
	if (m_ModuleInfoManager.LoadGameModuleInfo(m_GameItemMap)==true)
	{
		//重置列表
		m_ModuleListControl.DeleteAllItems();

		//变量定义
		POSITION Position=m_GameItemMap.m_GameModuleInfoMap.GetStartPosition();

		//枚举模块
		while (Position!=NULL)
		{
			//获取对象
			DWORD wModuleID=0L;
			tagGameGame * pGameGameItem=NULL;
			m_GameItemMap.m_GameModuleInfoMap.GetNextAssoc(Position,wModuleID,pGameGameItem);

			//插入列表
			ASSERT(pGameGameItem!=NULL);
			if (pGameGameItem!=NULL) m_ModuleListControl.InsertModuleInfo(pGameGameItem);
		}
	}

	return false;
}

//双击列表
VOID CDlgServerWizardItem1::OnNMDblclkModuleList(NMHDR * pNMHDR, LRESULT * pResult)
{
	//加载配置
	if (((NMITEMACTIVATE *)pNMHDR)->iItem!=LB_ERR)
	{
		//变量定义
		INT nListItem=((NMITEMACTIVATE *)pNMHDR)->iItem;
		DWORD *pGameID = (DWORD*) m_ModuleListControl.GetItemData(nListItem);
		DWORD dwGameID = *pGameID;
		tagGameGame * pGameGameItem =  m_GameItemMap.SearchModuleInfo(dwGameID);

		//投递消息
		if (pGameGameItem->dwNativeVersion!=0L)
		{
			GetParent()->PostMessage(WM_COMMAND,MAKELONG(IDC_NEXT,0),0);
		}

		return;
	}

	return;
}
#pragma endregion

#pragma region CDlgServerWizardItem2
/* *******************************************************************************
** CDlgServerWizardItem   2  
** ******************************************************************************/
//构造函数
CDlgServerWizardItem2::CDlgServerWizardItem2() : CDlgServerWizardItem(IDD_SERVER_WIZARD_2)
{
	//设置变量
	m_pDlgServerWizard=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;
}

//控件绑定
VOID CDlgServerWizardItem2::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
}

//调整控件
VOID CDlgServerWizardItem2::RectifyControl(INT nWidth, INT nHeight)
{
	return;
}

//焦点消息
VOID CDlgServerWizardItem2::OnSetFocus(CWnd * pNewWnd)
{
	__super::OnSetFocus(pNewWnd);

	return;
}
//初始化函数
BOOL CDlgServerWizardItem2::OnInitDialog()
{
	__super::OnInitDialog();

	//构造控件
	InitCtrlWindow();

	/* Top: 常规配置 */
	//房间名字
	//GameName
	TCHAR chGameName[LEN_GAME];
	memcpy(chGameName, m_pGameServiceOption->szServerName, LEN_GAME);//此处的serverName为GameName
	//RoomId
	BYTE byRoomID = m_pGameServiceOption->dwServerID & 0xFF;
	//RoomName
	TCHAR chRoomName[LEN_SERVER] = TEXT("Error未找到房间名字");	
	_sntprintf_s(chRoomName, LEN_SERVER, TEXT("%s·%d房间"),chGameName, byRoomID);

	SetDlgItemText(IDC_SERVER_NAME,chRoomName);

	//服务端口 -- 此处端口号无效，将提示错误
	if (m_pGameServiceOption->wGameServerPort!=0)
	{
		SetDlgItemInt(IDC_SERVER_PORT,m_pGameServiceOption->wGameServerPort);
	}
	else
	{
		TCHAR note[32] = TEXT("Error端口号错误");
		SetDlgItemText(IDC_SERVER_PORT,note);
	}

	return TRUE;
}

//保存输入
bool CDlgServerWizardItem2::SaveInputInfo()
{
	/* 第一步: 数据获取 */
	//变量定义
	CComboBox * pServerType=(CComboBox *)GetDlgItem(IDC_SERVER_TYPE);
	CIPAddressCtrl * pIPAddressCtrl=(CIPAddressCtrl *)GetDlgItem(IDC_DATABASE_ADDR);

	/* 常规配置 */
	// 房间名字
	GetDlgItemText(IDC_SERVER_NAME,m_pGameServiceOption->szServerName,CountArray(m_pGameServiceOption->szServerName));
	// 房间端口
	m_pGameServiceOption->wGameServerPort=GetDlgItemInt(IDC_SERVER_PORT);

	/* 第二步: 数据校验 */
	//数据库地址
	//端口名字
	if(m_pGameServiceOption->wGameServerPort == 0)
	{
		AfxMessageBox(TEXT("Server端口号不能为空，请检查数据库"),MB_ICONERROR);
		return false;
	}
	//房间名字
	if (m_pGameServiceOption->szServerName[0]==0)
	{
		AfxMessageBox(TEXT("游戏房间名字不能为空，请输入游戏房间名字"),MB_ICONERROR);
		return false;
	}
	return true;
}

//构造控件
VOID CDlgServerWizardItem2::InitCtrlWindow()
{
	((CEdit *)GetDlgItem(IDC_SERVER_PORT))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_SERVER_NAME))->LimitText(31);
	return;
}

//确定函数
VOID CDlgServerWizardItem2::OnOK() 
{ 
	//投递消息
	m_pDlgServerWizard->PostMessage(WM_COMMAND,MAKELONG(IDOK,0),0);

	return;
}

//取消消息
VOID CDlgServerWizardItem2::OnCancel() 
{ 
	//投递消息
	m_pDlgServerWizard->PostMessage(WM_COMMAND,MAKELONG(IDCANCEL,0),0);

	return;
}

//保存数据
bool CDlgServerWizardItem2::SaveItemData()
{
	//保存数据
	if ((m_hWnd!=NULL)&&(SaveInputInfo()==false))
	{
		return false;
	}

	return true;
}

//显示配置
bool CDlgServerWizardItem2::ShowOptionItem(const CRect & rcRect, CWnd * pParentWnd)
{
	//创建窗口
	if (m_hWnd==NULL) 
	{
		//设置资源
		AfxSetResourceHandle(GetModuleHandle(MODULE_MANAGER_DLL_NAME));

		//创建窗口
		Create(m_lpszTemplateName,pParentWnd);
		SetWindowPos(NULL,rcRect.left,rcRect.top,rcRect.Width(),rcRect.Height(),SWP_NOZORDER|SWP_NOACTIVATE);

		//设置资源
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}

	//显示窗口
	ShowWindow(SW_SHOW);

	return true;
}

#pragma endregion

#pragma region CDlgServerWizard
/* *******************************************************************************
** CDlgServerWizard 
** ******************************************************************************/
//构造函数
CDlgServerWizard::CDlgServerWizard() : CDialog(IDD_SERVER_WIZARD_MAIN)
{
	//设置变量
	m_wActiveIndex=INVALID_WORD;
	m_pWizardItem[0]=&m_ServerWizardItem1;
	m_pWizardItem[1]=&m_ServerWizardItem2;

	//接口变量
	m_pIGameServiceManager=NULL;
	m_pIGameServiceCustomRule=NULL;

	//配置信息
	ZeroMemory(&m_ModuleInitParameter,sizeof(m_ModuleInitParameter));

	return;
}

//初始化函数
BOOL CDlgServerWizard::OnInitDialog()
{
	__super::OnInitDialog();

	//设置资源
	AfxSetResourceHandle(GetModuleHandle(NULL));

	//激活向导 -- 配置房间时候 直接跳到CDlgServerWizardItem2
	ActiveWizardItem((m_pIGameServiceManager==NULL)?0:1);

	return FALSE;
}

//确定函数
VOID CDlgServerWizard::OnOK()
{
	if ((m_wActiveIndex+1)<CountArray(m_pWizardItem))
	{
		OnBnClickedNext();
	}
	else
	{
		OnBnClickedFinish();
	}

	return;
}

//创建房间
bool CDlgServerWizard::CreateGameServer()
{
	//设置资源
	AfxSetResourceHandle(GetModuleHandle(MODULE_MANAGER_DLL_NAME));

	//配置房间
	if (DoModal()==IDOK)
	{
		return true;
	}

	return false;
}

//设置接口
VOID CDlgServerWizard::SetWizardParameter(IGameServiceManager * pIGameServiceManager, tagGameServiceOption * pGameServiceOption, TCHAR* GameName)
{
	//销毁子项
	if (pGameServiceOption==NULL)
	{
		for (WORD i=1;i<CountArray(m_pWizardItem);i++)
		{
			if ((m_pWizardItem[i]!=NULL)&&(m_pWizardItem[i]->m_hWnd!=NULL))
			{
				m_pWizardItem[i]->DestroyWindow();
			}
		}
	}

	//设置变量
	ZeroMemory(&m_ModuleInitParameter,sizeof(m_ModuleInitParameter));

	//获取子游戏中的Attribute
	pIGameServiceManager->GetServiceAttrib(m_ModuleInitParameter.GameServiceAttrib);

	//设置接口
	m_pIGameServiceManager=pIGameServiceManager;

	//组件属性
	if (m_hWnd!=NULL)
	{
		//游戏名字
		if(GameName != NULL)
		{
			SetDlgItemText(IDC_GAME_NAME, GameName);//此处的serverName还为KindName
		}

		//GameID
		SetDlgItemInt(IDC_GAME_ID,m_ModuleInitParameter.GameServiceAttrib.dwGameID);
		
		//子游戏   的游戏版本 
		SetDlgItemInt(IDC_SUBGAME_VERSION,m_ModuleInitParameter.GameServiceAttrib.dwSubGameVersion);
	}

	//设置规则
	if (pGameServiceOption==NULL)
	{
		//调整参数
		m_pIGameServiceManager->RectifyParameter(m_ModuleInitParameter.GameServiceOption);
		
		//配置GameID
		m_ModuleInitParameter.GameServiceOption.dwGameID = m_ModuleInitParameter.GameServiceAttrib.dwGameID;
	}
	else
	{
		//拷贝规则
		CopyMemory(&m_ModuleInitParameter.GameServiceOption,pGameServiceOption,sizeof(tagGameServiceOption));
	}

	return;
}

//激活向导
bool CDlgServerWizard::ActiveWizardItem(WORD wIndex)
{
	//判断状态
	if (m_wActiveIndex==wIndex)
	{
		m_pWizardItem[m_wActiveIndex]->SetFocus();
		return true;
	}

	//保存旧项
	CDlgServerWizardItem * pItemWizard=NULL;
	if (m_wActiveIndex<CountArray(m_pWizardItem))
	{
		//设置变量
		pItemWizard=m_pWizardItem[m_wActiveIndex];

		//保存数据 -- 进行下一步时候,当前保存失败了。 则报错			
		if ((wIndex>m_wActiveIndex)&&(m_pWizardItem[m_wActiveIndex]->SaveItemData()==false)) return false;
	}

	//获取位置
	CRect rcItemRect;
	GetDlgItem(IDC_ITEM_FRAME)->GetWindowRect(rcItemRect);
	ScreenToClient(&rcItemRect);

	//设置新项
	m_pWizardItem[wIndex]->m_pDlgServerWizard=this;
	m_pWizardItem[wIndex]->m_pGameServiceAttrib=&m_ModuleInitParameter.GameServiceAttrib;
	m_pWizardItem[wIndex]->m_pGameServiceOption=&m_ModuleInitParameter.GameServiceOption;

	//创建新项
	m_wActiveIndex=wIndex;
	m_pWizardItem[m_wActiveIndex]->ShowWizardItem(rcItemRect,this);

	//激活新项
	m_pWizardItem[m_wActiveIndex]->SetFocus();
	if (pItemWizard!=NULL) pItemWizard->ShowWindow(SW_HIDE);

	//界面变量
	CButton * pButtonLast=(CButton * )GetDlgItem(IDC_LAST);
	CButton * pButtonNext=(CButton * )GetDlgItem(IDC_NEXT);
	CButton * pButtonFinish=(CButton * )GetDlgItem(IDC_FINISH);

	//进度界面
	pButtonNext->EnableWindow(((m_wActiveIndex+1)<CountArray(m_pWizardItem))?TRUE:FALSE);
	pButtonFinish->EnableWindow(((m_wActiveIndex+1)==CountArray(m_pWizardItem))?TRUE:FALSE);
	pButtonLast->EnableWindow(((m_wActiveIndex>1)||((m_ModuleInitParameter.GameServiceOption.dwServerID==0)&&(m_wActiveIndex>0)))?TRUE:FALSE);

	//构造标题
	TCHAR szTitle[128]=TEXT("");
	_sntprintf_s(szTitle,CountArray(szTitle),TEXT("房间配置向导 --- [ 步骤 %d ]"),m_wActiveIndex+1);
	
	//设置标题
	SetWindowText(szTitle);

	return true;
}

//上一步
VOID CDlgServerWizard::OnBnClickedLast()
{
	//效验参数
	ASSERT(m_wActiveIndex>0);
	ASSERT(m_wActiveIndex<CountArray(m_pWizardItem));

	//切换页面
	ActiveWizardItem(m_wActiveIndex-1);

	return;
}

//下一步
VOID CDlgServerWizard::OnBnClickedNext()
{
	//效验参数
	ASSERT((m_wActiveIndex+1)<CountArray(m_pWizardItem));

	//激活页面
	ActiveWizardItem(m_wActiveIndex+1);

	return;
}

//完成按钮
VOID CDlgServerWizard::OnBnClickedFinish()
{
	//保存设置
	ASSERT(m_wActiveIndex>=1);
	if (m_pWizardItem[m_wActiveIndex]->SaveItemData()==false) return;

	//变量定义
	CServerInfoManager ServerInfoManager;
	
	//插入房间（将从对话框界面上获取的数据写入数据库）
	if (ServerInfoManager.InsertGameServer(&m_ModuleInitParameter.GameServiceOption)==false) 
	{
		CTraceService::TraceString(TEXT("创建房间失败"), TraceLevel_Normal);
		return;
	}
	else
		CTraceService::TraceString(TEXT("创建房间成功"), TraceLevel_Normal);

	//关闭窗口
	EndDialog(IDOK);

	return;
}

#pragma endregion