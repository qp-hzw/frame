#include "StdAfx.h"
#include "GameCtrl.h"

//////////////////////////////////////////////////////////////////////////////////
//全局变量
CGameCtrl                  *g_GameCtrl = NULL; 
ITCPNetworkEngine		   *g_TCPNetworkEngine = NULL;				    //socket::server
ITCPSocketEngine		   *g_TCPSocketEngine = NULL;					//socker::client -- 目标服务器为 协调服

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CGameCtrl::CGameCtrl()
{
	m_AttemperEngine = NULL;
	m_TCPNetworkEngine = NULL;
	m_TCPSocketEngine = NULL;
	m_TimerEngine = NULL;

	//设置对象
	if (g_GameCtrl==NULL) g_GameCtrl=this;

	return;
}

//析构函数
CGameCtrl::~CGameCtrl()
{
	ConcludeService();
}

//配置组件
int CGameCtrl::InitializeService()
{
	/***************************************************  创建服务 *************************************************/
	//创建组件
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
	IUnknownEx * pIAttemperEngineSink=static_cast<IUnknownEx*>(&m_AttemperEngineSink);
	IUnknownEx * pIDataBaseEngineSink=static_cast<IUnknownEx*>(&m_DataBaseEngineSink);

	/***************************************************  AttemperEngine 配置信息 *************************************************/
	//AttemperEngine设置 Attemper钩子
	if (m_AttemperEngine->SetAttemperEngineSink(pIAttemperEngineSink)==false) return 6;

	//AttemperEngine设置 DB钩子
	if (m_AttemperEngine->SetDataBaseEngineSink(pIDataBaseEngineSink)==false) return 7;

	/***************************************************  socket::client 配置信息 *************************************************/
	//协调服务（协调服务器）
	if (m_TCPSocketEngine->SetServiceID(NETWORK_CORRESPOND)==false) return 11;

	/***************************************************  socket::server 配置信息 *************************************************/
	//配置网络
	WORD wServicePort=PORT_LOGON;
	if (m_TCPNetworkEngine->SetServiceParameter(wServicePort)==false) return 12;

	/***************************************************  log 配置信息 *************************************************/
	CLog::Init("logon.log");

	CLog::Log(log_debug, "service create success");
	return 0;
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
		CLog::Log(log_error, "CGameCtrl::StartKernelService  %d", iRet);
		ConcludeService();
		return false;
	}

	//连接协调
	if (m_TCPSocketEngine!=NULL)
	{
		CLog::Log(log_error, "[socket::client] connect to center");
		m_TCPSocketEngine->Connect(_CPD_SERVER_ADDR, PORT_CENTER);
	}
	
	return true;
}

//停止服务
bool CGameCtrl::ConcludeService()
{
	//停止服务
	if (m_TimerEngine!=NULL) m_TimerEngine->ConcludeService();
	if (m_AttemperEngine!=NULL) m_AttemperEngine->ConcludeService();
	if (m_TCPNetworkEngine!=NULL) m_TCPNetworkEngine->ConcludeService();
	if (m_TCPSocketEngine!=NULL) m_TCPSocketEngine->ConcludeService();
	
	return true;
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

	//socket::client
	if (m_TCPSocketEngine->StartService()==false)
	{
		return 3;
	}
	
	return 0;
}

//启动网络
int CGameCtrl::StartNetworkService()
{
	//网络引擎
	if (m_TCPNetworkEngine->StartService()==false)
	{
		return 1;
	}

	return 0;
}


/***************************************************  消息发送  S->C  *************************************************/
//发送函数
bool CGameCtrl::SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID)
{
	return m_TCPNetworkEngine->SendData(dwSocketID, wMainCmdID, wSubCmdID);
}
//发送函数
bool CGameCtrl::SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return m_TCPNetworkEngine->SendData(dwSocketID, wMainCmdID, wSubCmdID, pData, wDataSize);
}
//批量发送
bool CGameCtrl::SendDataBatch(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return m_TCPNetworkEngine->SendDataBatch(wMainCmdID, wSubCmdID, pData, wDataSize);
}

/***************************************************  消息发送  ->DB  *************************************************/
bool CGameCtrl::PostDataBaseRequest(WORD wRequestID, DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	return m_AttemperEngine->PostDataBaseRequest(wRequestID, dwScoketID, pData, wDataSize);
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