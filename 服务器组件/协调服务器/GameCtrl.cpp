#include "StdAfx.h"
#include "GameCtrl.h"
#include "ServerItemManager.h"

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


/*为GameServer自动生成端口*/
int CGameCtrl::GeneratePort2Game(int& port)
{
	/*
	** 1. 读取所有m_GameMap中所有的ServerID的端口 -- 以后需要查看GameServer list运行情况的时候, 可以改为存储在数据库
	** 2. 过滤不符合规则的端口
	** 3. 找到 最小的port 和 最大的port 
	** 4. 最小的 +1 或者 最大的port -1
	** 5. 配置最新的port
	**
	** _MAX_SERVER_PORT - _MIN_SERVER_PORT < 1500  -- 因为Gate.port = Game.port + 1500
	*/

	//寻找最大 和  最小的ServerPort
	int iMinServerPort = _MAX_SERVER_PORT;
	int iMaxServerPort = _MIN_SERVER_PORT;

	//遍历m_GameMap
	std::vector<tagServerItem*> ALLGameServer= CServerItemManager::FindAllGameServer();
	for (auto item : ALLGameServer)
	{
		if(!item) continue;

		//数据校验 port不正确则 continue
		if (item->wServerPort < _MIN_SERVER_PORT
			|| item->wServerPort > _MAX_SERVER_PORT)
		{
			continue;
		}

		//获取最小的port
		iMinServerPort = (item->wServerPort < iMinServerPort) ? item->wServerPort : iMinServerPort;
		//获取最大的port
		iMaxServerPort = (item->wServerPort > iMaxServerPort) ? item->wServerPort : iMaxServerPort;

	}

	//数据校验 -- 数据错误
	if ((iMinServerPort < _MIN_SERVER_PORT) || (iMaxServerPort > _MAX_SERVER_PORT))
	{
		return 1;
	}

	//端口已满
	if ((iMinServerPort == _MIN_SERVER_PORT) && (iMaxServerPort == _MAX_SERVER_PORT))
	{
		return 2;
	}

	//优先选用 低端口
	if (iMinServerPort > _MIN_SERVER_PORT)
	{
		port = iMinServerPort - 1;
	}
	else
	{
		port = iMaxServerPort - 1;
	}

	return 0;
}