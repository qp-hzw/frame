#include <string>
#include <iostream>
#include "StdAfx.h"
#include "ServiceUnits.h"


//全局变量
CServiceUnits              *g_pServiceUnits = NULL; 
IAttemperEngine			   *g_AttemperEngine = NULL;					//调度引擎
ITCPNetworkEngine		   *g_TCPNetworkEngine = NULL;				    //socket::server
ITCPSocketEngine		   *g_TCPSocketEngine = NULL;					//socker::client -- 目标服务器为 协调服
ITimerEngine			   *g_TimerEngine = NULL;						//定时器


//构造函数
CServiceUnits::CServiceUnits(std::string dll_name)
{
	//设置对象
	if (g_pServiceUnits==NULL) g_pServiceUnits=this;

	m_AttemperEngine = NULL;
	m_TCPNetworkEngine = NULL;
	m_TCPSocketEngine = NULL;
	m_TimerEngine = NULL;

	m_subgame_dll_name = dll_name;

	//组件配置
	ZeroMemory(&m_GameServiceAttrib,sizeof(m_GameServiceAttrib));
	ZeroMemory(&m_GameServiceOption,sizeof(m_GameServiceOption));
}

//析构函数
CServiceUnits::~CServiceUnits()
{
	ConcludeService();
}

//创建模块
int CServiceUnits::InitializeService()
{
	/***************************************************  创建服务 *************************************************/
	m_AttemperEngine = static_cast<IAttemperEngine*>(CWHModule::AttemperEngine());
	m_TCPNetworkEngine = static_cast<ITCPNetworkEngine*>(CWHModule::TCPNetworkEngine());
	m_TCPSocketEngine = static_cast<ITCPSocketEngine*>(CWHModule::TCPSocketEngine());
	m_TimerEngine = static_cast<ITimerEngine*>(CWHModule::TimerEngine());

	ITableFrameSink *SubGameDll = static_cast<ITableFrameSink*>(CWHModule::SubGame(m_subgame_dll_name)); //此处只是校验

	if(m_AttemperEngine == NULL) return 1;
	if(m_TCPNetworkEngine == NULL) return 2;
	if(m_TCPSocketEngine == NULL) return 3;
	if(m_TimerEngine == NULL) return 4;
	if(SubGameDll == NULL) return 5;

	g_AttemperEngine = m_AttemperEngine;
	g_TCPNetworkEngine = m_TCPNetworkEngine;
	g_TCPSocketEngine = m_TCPSocketEngine;
	g_TimerEngine = m_TimerEngine;

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

	/***************************************************  socket::server 配置信息 *************************************************/
	//配置网络
	if(m_TCPNetworkEngine->SetServiceParameter(m_GameServiceOption.wGameServerPort)==false) return 12;

	/***************************************************  log 配置信息 *************************************************/
	//考虑到游戏服到现在才能知道ServerID, 因此只能将log的配置信息写到这里
	//std::string log_nam= "GameServer-" + std::to_string(m_GameServiceOption.dwServerID) + ".log";

	std::string log_nam= "GameServer.log";
	CLog::Init(log_nam.c_str());

	CLog::Log(log_debug, "service create success");
	return 0;
}

//启动服务
bool CServiceUnits::StartService()
{
	//配置服务
	int iRet = InitializeService();
	if (iRet != 0)
	{
		CLog::Log(log_error, "CServiceUnits::InitializeService %d", iRet);
		ConcludeService();
		return false;
	}

	//启动内核
	iRet = StartKernelService();
	if (iRet != 0)
	{
		CLog::Log(log_error, "CServiceUnits::InitializeService %d", iRet);
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
bool CServiceUnits::ConcludeService()
{
	//服务对象
	if (m_TimerEngine !=NULL) m_TimerEngine->ConcludeService();
	if (m_AttemperEngine!=NULL) m_AttemperEngine->ConcludeService();
	if (m_TCPSocketEngine!=NULL) m_TCPSocketEngine->ConcludeService();
	if (m_TCPNetworkEngine!=NULL) m_TCPNetworkEngine->ConcludeService();

	return true;
}

//启动内核
int CServiceUnits::StartKernelService()
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
bool CServiceUnits::StartNetworkService()
{
	//网络引擎
	if (m_TCPNetworkEngine->StartService()==false)
	{
		return false;
	}

	return true;
}
