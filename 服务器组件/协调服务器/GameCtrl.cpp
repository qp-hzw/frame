#include "StdAfx.h"
#include "GameCtrl.h"

//全局变量
IAttemperEngine			   *g_AttemperEngine = NULL;					//调度引擎
ITCPNetworkEngine		   *g_TCPNetworkEngine = NULL;				    //socket::server
CGameCtrl                   *g_GameCtrl = NULL;          

CGameCtrl::CGameCtrl()
{
	g_GameCtrl = this;
	m_AttemperEngine = NULL;
	m_TCPNetworkEngine = NULL;
}

//启动服务
bool CGameCtrl::StartService()
{
	//配置服务
	int iRet = InitializeService();
	if (iRet != 0 )
	{
		CLog::Log(log_error, "CGameCtrl::InitializeService  %d", iRet);
		ConcludeService();
		return false;
	}

	//启动内核
	iRet = StartKernelService();
	if (iRet != 0)
	{
		CLog::Log(log_error,"CGameCtrl::StartKernelService  %d", iRet);
		ConcludeService();
		return false;
	}

	return true;
}

//停止服务
bool CGameCtrl::ConcludeService()
{
	//停止服务
	if (m_AttemperEngine!=NULL) m_AttemperEngine->ConcludeService();
	if (m_TCPNetworkEngine!=NULL) m_TCPNetworkEngine->ConcludeService();

	return true;
}

//配置组件
int CGameCtrl::InitializeService()
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
	IUnknownEx * pIAttemperEngineSink=static_cast<IUnknownEx*>(&m_AttemperEngineSink);

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

	CLog::Log(log_debug, "service create success");
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

	//网络引擎
	if (m_TCPNetworkEngine->StartService()==false)
	{
		return 2;
	}

	return 0;
}


/***************************************************  消息发送 *************************************************/

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
