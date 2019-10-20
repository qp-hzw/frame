#include "StdAfx.h"
#include "ServiceUnits.h"

//全局变量
IAttemperEngine			   *g_AttemperEngine = NULL;					//调度引擎
ITCPNetworkEngine		   *g_TCPNetworkEngine = NULL;				    //socket::server

//启动服务
bool CServiceUnits::StartService()
{
	//配置服务
	int iRet = InitializeService();
	if (iRet != 0 )
	{
		CLog::Log(log_error, "CServiceUnits::InitializeService  %d", iRet);
		ConcludeService();
		return false;
	}

	//启动内核
	iRet = StartKernelService();
	if (iRet != 0)
	{
		CLog::Log(log_error,"CServiceUnits::StartKernelService  %d", iRet);
		ConcludeService();
		return false;
	}

	return true;
}

//停止服务
bool CServiceUnits::ConcludeService()
{
	//停止服务
	if (m_AttemperEngine!=NULL) m_AttemperEngine->ConcludeService();
	if (m_TCPNetworkEngine!=NULL) m_TCPNetworkEngine->ConcludeService();

	return true;
}

//配置组件
int CServiceUnits::InitializeService()
{
	/***************************************************  创建服务 *************************************************/
	//创建服务
	m_AttemperEngine = static_cast<IAttemperEngine*>(CWHModule::AttemperEngine());
	m_TCPNetworkEngine = static_cast<ITCPNetworkEngine*>(CWHModule::TCPNetworkEngine());

	if(m_AttemperEngine == NULL) return 1;
	if(m_TCPNetworkEngine == NULL) return 2;

	g_AttemperEngine = m_AttemperEngine;
	g_TCPNetworkEngine = m_TCPNetworkEngine;

	//回调对象
	IUnknownEx * pIAttemperEngineSink=QUERY_OBJECT_INTERFACE(m_AttemperEngineSink,IUnknownEx);

	/***************************************************  AttemperEngine 配置信息 *************************************************/
	//AttemperEngine设置回调
	if (m_AttemperEngine->SetAttemperEngineSink(pIAttemperEngineSink)==false) return 4;


	/***************************************************  socket::server 配置信息 *************************************************/
	//配置网络
	WORD wServicePort=PORT_CENTER;
	if (m_TCPNetworkEngine->SetServiceParameter(wServicePort)==false) return 5;


	/***************************************************  log 配置信息 *************************************************/
	//设置服务器日志输出等级
	CLog::Init("correspond.log");

	return 0;
}

//启动内核
int CServiceUnits::StartKernelService()
{
	//调度引擎
	if (m_AttemperEngine->StartService()==false)
	{
		return 1;
	}

	//网络引擎
	if (m_TCPNetworkEngine->StartService()==false)
	{
		return 2;
	}

	return 0;
}
