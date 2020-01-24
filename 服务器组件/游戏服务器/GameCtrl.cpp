#include "Stdafx.h"
#include "GameCtrl.h"
#include <iostream>

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
	std::string log_nam= "GameServer.log";
	CLog::Init(log_nam.c_str());

	/***************************************************  SubGame 配置信息 *************************************************/
	//读取子游戏配置文件
	DWORD dwKindId = static_cast<DWORD >(CWHModule::SubGameCfg(m_subgame_dll_name)); //此处只是校验
	if (0 == dwKindId)	return 8;
	SetServerID(dwKindId << 16);


	//读取房间规则配置文件
	//ReadFrameRoomRule();
	//ReadSubGameRoomRule(dwKindId);
	memcpy(m_rule_arry.ItemArry[0].szHeadName, "局数", 15);
	memcpy(m_rule_arry.ItemArry[0].szItemValue[0], "1", 10);
	memcpy(m_rule_arry.ItemArry[0].szItemValue[1], "2", 10);
	memcpy(m_rule_arry.ItemArry[0].szItemValue[2], "8", 10);
	
	memcpy(m_rule_arry.ItemArry[1].szHeadName, "玩家数", 15);
	memcpy(m_rule_arry.ItemArry[1].szItemValue[0], "2", 10);
	memcpy(m_rule_arry.ItemArry[1].szItemValue[1], "3", 10);
	memcpy(m_rule_arry.ItemArry[1].szItemValue[2], "4", 10);

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

//读取通用房间配置文件
void CGameCtrl::ReadFrameRoomRule()
{
	//初始化数据
	ZeroMemory(&m_rule_arry, sizeof(m_rule_arry));
	m_frame_rule_count = 0;
	m_subgame_rule_count = 0;

	//打开文件
	string file_path = "frame.rule";
	int iRet = CWHCfg::Instance()->OpenFile(file_path);
	if(iRet != 0 )
	{
		return ;
	}

	BYTE card_group_num = 0;
	for(int i=0; i<20; i++)
	{
		char psz[20];
		sprintf(psz, "RULE_%d", i);
		string strTemp;
		iRet = CWHCfg::Instance()->GetItemValue(psz, "head", strTemp);
		if(iRet != 0) continue;
		std::cout << strTemp << std::endl;
		std::cout << strTemp.c_str() << std::endl;

		m_frame_rule_count ++;
		//swprintf(m_rule_arry.ItemArry[i].szHeadName, 15, L"%S", strTemp.c_str());

		for(int j=0; j<4; j++)
		{
			char value[20];
			sprintf(value, "value_%d", j);
			iRet = CWHCfg::Instance()->GetItemValue(psz, value, strTemp);
			if(iRet != 0) continue;
			//swprintf(m_rule_arry.ItemArry[i].szItemValue[j], 10, L"%S", strTemp.c_str());
		}
	}

	//关闭文件
	CWHCfg::Instance()->CloseFile();
}
//读取子游戏房间配置文件
void CGameCtrl::ReadSubGameRoomRule(int kindid)
{
	//打开文件
	string file_path = std::to_string(kindid)+ ".rule";
	int iRet = CWHCfg::Instance()->OpenFile(file_path);
	if(iRet != 0 )
	{
		return ;
	}

	BYTE card_group_num = 0;
	for(int i=0; i<20; i++)
	{
		char psz[20];
		sprintf(psz, "RULE_%d", i);
		string strTemp;
		iRet = CWHCfg::Instance()->GetItemValue(psz, "head", strTemp);
		if(iRet != 0) continue;
		m_subgame_rule_count ++;
		//swprintf(m_rule_arry.ItemArry[i].szHeadName, 15, L"%S", strTemp.c_str());

		for(int j=0; j<4; j++)
		{
			char value[20];
			sprintf(value, "value_%d", j);
			iRet = CWHCfg::Instance()->GetItemValue(psz, value, strTemp);
			if(iRet != 0) continue;
			//swprintf(m_rule_arry.ItemArry[i].szItemValue[j], 10, L"%S", strTemp.c_str());
		}
	}

	//关闭文件
	CWHCfg::Instance()->CloseFile();
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
	if (pIServerUserItem->GetBindIndex()!=INVALID_WORD)
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
	if ((pIServerUserItem->GetBindIndex()!=INVALID_WORD))
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