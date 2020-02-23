#include "Stdafx.h"
#include "GameCtrl.h"
#include "RoomRuleManager.h"
#include "MatchManager.h"

#define MDM_CM_SYSTEM				1000								//系统命令
#define SUB_CM_SYSTEM_MESSAGE		1									//系统消息


//系统消息
struct CMD_CM_SystemMessage
{
	WORD							wType;								//消息类型
	WORD							wLength;							//消息长度
	TCHAR							szString[1024];						//消息内容
};

//全局变量
CGameCtrl                  *g_GameCtrl = NULL; 
ITCPNetworkEngine		   *g_TCPNetworkEngine = NULL;				    //socket::server
ITCPSocketEngine		   *g_TCPSocketEngine = NULL;					//socker::client -- 目标服务器为 协调服


//构造函数
CGameCtrl::CGameCtrl(std::string dll_name)
{
	//设置对象
	if (g_GameCtrl==NULL) g_GameCtrl=this;

	m_AttemperEngine = NULL;
	m_TCPNetworkEngine = NULL;
	m_TCPSocketEngine = NULL;
	m_TimerEngine = NULL;

	m_subgame_dll_name = dll_name;
	m_ServerID = 0;
}

//析构函数
CGameCtrl::~CGameCtrl()
{
	ConcludeService();
}

//启动服务
bool CGameCtrl::StartService()
{
	//配置服务
	int iRet = InitializeService();
	if (iRet != 0)
	{
		CLog::Log(log_error, "CGameCtrl::InitializeService %d", iRet);
		ConcludeService();
		return false;
	}

	//启动内核
	iRet = StartKernelService();
	if (iRet != 0)
	{
		CLog::Log(log_error, "CGameCtrl::InitializeService %d", iRet);
		ConcludeService();
		return false;
	}

	//连接协调
	if (m_TCPSocketEngine !=NULL)
	{
		m_TCPSocketEngine->Connect(_CPD_SERVER_ADDR, PORT_CENTER);
	}

	//加载机器人
	PostDataBaseRequest(DBR_GR_LOAD_ANDROID_USER,0L,NULL,0L);

	return true;
}
//停止服务
bool CGameCtrl::ConcludeService()
{
	//服务对象
	if (m_TimerEngine !=NULL) m_TimerEngine->ConcludeService();
	if (m_AttemperEngine!=NULL) m_AttemperEngine->ConcludeService();
	if (m_TCPSocketEngine!=NULL) m_TCPSocketEngine->ConcludeService();
	if (m_TCPNetworkEngine!=NULL) m_TCPNetworkEngine->ConcludeService();

	return true;
}

//创建模块
int CGameCtrl::InitializeService()
{
	/***************************************************  创建服务 *************************************************/
	m_AttemperEngine = static_cast<IAttemperEngine*>(CWHModule::AttemperEngine());
	m_TCPNetworkEngine = static_cast<ITCPNetworkEngine*>(CWHModule::TCPNetworkEngine());
	m_TCPSocketEngine = static_cast<ITCPSocketEngine*>(CWHModule::TCPSocketEngine());
	m_TimerEngine = static_cast<ITimerEngine*>(CWHModule::TimerEngine());

	if(m_AttemperEngine == NULL) return 1;
	if(m_TCPNetworkEngine == NULL) return 2;
	if(m_TCPSocketEngine == NULL) return 3;
	if(m_TimerEngine == NULL) return 4;

	g_TCPNetworkEngine = m_TCPNetworkEngine;
	g_TCPSocketEngine = m_TCPSocketEngine;

	//回调对象
	IUnknownEx * pIAttemperEngineSink=static_cast<IAttemperEngineSink*>(&m_AttemperEngineSink);
	IUnknownEx * pIDataBaseEngineSink=static_cast<IUnknownEx*>(&m_DataBaseEngineSink);

	/***************************************************  AttemperEngine 配置信息 *************************************************/
	//AttemperEngine设置 Attemper钩子
	if (m_AttemperEngine->SetAttemperEngineSink(pIAttemperEngineSink)==false) return 6;

	//AttemperEngine设置 DB钩子
	if (m_AttemperEngine->SetDataBaseEngineSink(pIDataBaseEngineSink)==false) return 7;

	/***************************************************  socket::client 配置信息 *************************************************/
	//协调服务
	if (m_TCPSocketEngine->SetServiceID(NETWORK_CORRESPOND)==false) return 11;

	/***************************************************  log 配置信息 *************************************************/
	CLog::Init("GameServer");

	/***************************************************  SubGame 配置信息 *************************************************/
	//读取子游戏配置文件
	DWORD dwKindId = static_cast<DWORD >(CWHModule::SubGameCfg(m_subgame_dll_name)); //此处只是校验
	if (0 == dwKindId)	return 8;
	SetServerID(dwKindId << 16);

	//读取房间规则配置文件
	RoomRuleManager::Init(dwKindId, m_subgame_dll_name);

	//加载比赛场配置
	CMatchManager::ReadMatchConfig(dwKindId);

	return 0;
}
//启动内核
int CGameCtrl::StartKernelService()
{
	//调度引擎
	if (m_AttemperEngine->StartService()==false)
	{
		return 1;
	}

	//时间引擎
	if (m_TimerEngine->StartService()==false)
	{
		return 2;
	}

	//协调引擎
	if (m_TCPSocketEngine->StartService()==false)
	{
		return 3;
	}

	return 0;
}
//启动网络
bool CGameCtrl::StartNetworkService()
{
	//网络引擎
	if (m_TCPNetworkEngine->StartService()==false)
	{
		return false;
	}

	return true;
}
//配置端口
int CGameCtrl::SetNetworkPort(WORD Port)
{
	if (m_TCPNetworkEngine->SetServiceParameter(Port) == false)
		return -1;

	return 0;
}

//获取ITableFrameSink
ITableFrameSink* CGameCtrl::GetITableFrameSink()
{
	ITableFrameSink *SubGameDll = static_cast<ITableFrameSink*>(CWHModule::SubGame(m_subgame_dll_name)); //此处只是校验
	return SubGameDll;
}

//释放ITableFrameSink
void CGameCtrl::FreeITableFrameSink(ITableFrameSink *p)
{
	CWHModule::FreeSubGame(m_subgame_dll_name, p);
}

/************************************************消息发送*********************************************************/
//发送数据
bool CGameCtrl::SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	m_TCPNetworkEngine->SendData(dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);

	return true;
}
//发送数据
bool CGameCtrl::SendData(CPlayer * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//效验参数
	if (pIServerUserItem==NULL) return false;

	//发送数据
	m_TCPNetworkEngine->SendData(pIServerUserItem->GetSocketID(),wMainCmdID,wSubCmdID,pData,wDataSize);
	
	return true;
}
//char* 2 TCHAR
TCHAR *chr2wch(const char *buffer)
{
        size_t len = strlen(buffer);
        size_t wlen = MultiByteToWideChar(CP_ACP, 0, (const char*)buffer, int(len), NULL, 0);
        TCHAR *wBuf = new TCHAR[wlen + 1];
        MultiByteToWideChar(CP_ACP, 0, (const char*)buffer, int(len), wBuf, int(wlen));
        return wBuf;
}
//发送通用错误提示
bool CGameCtrl::SendDataMsg(DWORD dwSocketID, string msg)
{
	STR_SUB_CL_COMMON_ERROR cmd;
	memcpy(cmd.szMsg, chr2wch(msg.c_str()), sizeof(TCHAR)*20);

	SendData(dwSocketID, MDM_GR_LOGON, CMD_GC_COMMON_ERROR, &cmd, sizeof(cmd));
	return true;
}

//房间消息
bool CGameCtrl::SendRoomMessage(LPCTSTR lpszMessage, WORD wType)
{
	//变量定义
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//构造数据
	SystemMessage.wType=wType;
	SystemMessage.wLength=lstrlen(lpszMessage)+1;
	lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

	//数据属性
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//发送数据
	m_TCPNetworkEngine->SendDataBatch(MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);

	return true;
}

//游戏消息
bool CGameCtrl::SendGameMessage(LPCTSTR lpszMessage, WORD wType)
{
	//变量定义
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//构造数据
	SystemMessage.wType=wType;
	SystemMessage.wLength=lstrlen(lpszMessage)+1;
	lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

	//数据属性
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//发送数据
	m_TCPNetworkEngine->SendDataBatch(MDM_G_FRAME,CMD_GF_SYSTEM_MESSAGE,&SystemMessage,wSendSize);

	return true;
}

//房间消息
bool CGameCtrl::SendRoomMessage(CPlayer * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//效验参数
	if (pIServerUserItem==NULL) return false;

	//发送数据
	//if (pIServerUserItem->GetBindIndex()!=INVALID_WORD)
	{
		//变量定义
		CMD_CM_SystemMessage SystemMessage;
		ZeroMemory(&SystemMessage,sizeof(SystemMessage));

		//构造数据
		SystemMessage.wType=wType;
		SystemMessage.wLength=lstrlen(lpszMessage)+1;
		lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

		//数据属性
		WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
		WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

		//发送数据
		if (pIServerUserItem->IsAndroidUser()==true)
		{

		}
		else
		{
			//常规用户
			m_TCPNetworkEngine->SendData(pIServerUserItem->GetSocketID(),MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
		}

		return true;
	}

	return false;
}

//游戏消息
bool CGameCtrl::SendGameMessage(CPlayer * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//发送数据
	//if ((pIServerUserItem->GetBindIndex()!=INVALID_WORD))
	{
		//变量定义
		CMD_CM_SystemMessage SystemMessage;
		ZeroMemory(&SystemMessage,sizeof(SystemMessage));

		//构造数据
		SystemMessage.wType=wType;
		SystemMessage.wLength=lstrlen(lpszMessage)+1;
		lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

		//数据属性
		WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
		WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

		//发送数据
		if (pIServerUserItem->IsAndroidUser()==true)
		{

		}
		else
		{
			//常规用户
			m_TCPNetworkEngine->SendData(pIServerUserItem->GetSocketID(),MDM_G_FRAME,CMD_GF_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
		}

		return true;
	}

	return false;
}

//房间消息
bool CGameCtrl::SendRoomMessage(DWORD dwSocketID, LPCTSTR lpszMessage, WORD wType, bool bAndroid)
{
	//变量定义
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//构造数据
	SystemMessage.wType=wType;
	SystemMessage.wLength=lstrlen(lpszMessage)+1;
	lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

	//数据属性
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//发送数据
	if (bAndroid)
	{
		//机器用户
		//m_AndroidUserManager.SendDataToClient(dwSocketID,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
	}
	else
	{
		//常规用户
		m_TCPNetworkEngine->SendData(dwSocketID,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
	}

	return true;
}

//发送跑马灯消息
bool CGameCtrl::SendMessageLobbyAndAllGame( LPCTSTR lpszMessage, WORD wType ,WORD MsgID)
{
	/*
	//构造消息
	CMD_CS_C_SendMarquee pSendLobbyMessage;
	ZeroMemory(&pSendLobbyMessage,sizeof(pSendLobbyMessage));
	lstrcpyn(pSendLobbyMessage.szMessage,lpszMessage,CountArray(pSendLobbyMessage.szMessage));
	pSendLobbyMessage.MsgType=wType;
	pSendLobbyMessage.MsgID = MsgID;
	//发送给协调服务器
	m_TCPSocketEngine->SendData(MDM_WEB,CPR_WP_WEB_MARQUEE,&pSendLobbyMessage,sizeof(CMD_CS_C_SendMarquee));
	*/
	return true;
}


/***************************************************  消息发送  ->DB  *************************************************/
bool CGameCtrl::PostDataBaseRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	return m_AttemperEngine->PostDataBaseRequest(wRequestID, dwContextID, pData, wDataSize);
}


/***************************************************     Timer       *************************************************/
//设置定时器
bool CGameCtrl::SetTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter)
{
	return m_TimerEngine->SetTimer(dwTimerID, dwElapse, dwRepeat, dwBindParameter);
}
//删除定时器
bool CGameCtrl::KillTimer(DWORD dwTimerID)
{
	return m_TimerEngine->KillTimer(dwTimerID);
}
//删除定时器
bool CGameCtrl::KillAllTimer()
{
	return m_TimerEngine->KillAllTimer();
}
//获取定时器剩余时间（毫秒）
DWORD CGameCtrl::GetTimerLeftTickCount(DWORD dwTimerID)
{
	return m_TimerEngine->GetTimerLeftTickCount(dwTimerID);
}
