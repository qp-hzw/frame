#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"


//静态变量
CServiceUnits *			g_pServiceUnits=NULL;			//对象指针


//构造函数
CServiceUnits::CServiceUnits()
{
	//设置对象
	if (g_pServiceUnits==NULL) g_pServiceUnits=this;

	//组件配置
	ZeroMemory(&m_GameServiceAttrib,sizeof(m_GameServiceAttrib));
	ZeroMemory(&m_GameServiceOption,sizeof(m_GameServiceOption));

	return;
}

//析构函数
CServiceUnits::~CServiceUnits()
{
	ConcludeService();
}


//启动服务
bool CServiceUnits::StartService()
{
	//创建模块
	int iRet = CreateServiceDLL();
	if (iRet != 0)
	{
		CLog::Log(log_error, "CServiceUnits::CreateServiceDLL %d", iRet);
		ConcludeService();
		return false;
	}

	//配置服务
	iRet = InitializeService();
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
	if (m_TCPSocketEngine.GetInterface()!=NULL)
	{
		m_TCPSocketEngine.GetInterface()->Connect(_CPD_SERVER_ADDR, PORT_CENTER);
	}

	return true;
}

//停止服务
bool CServiceUnits::ConcludeService()
{

	//内核组件
	if (m_TimerEngine.GetInterface()!=NULL) m_TimerEngine->ConcludeService();
	if (m_AttemperEngine.GetInterface()!=NULL) m_AttemperEngine->ConcludeService();
	if (m_TCPSocketEngine.GetInterface()!=NULL) m_TCPSocketEngine->ConcludeService();
	if (m_TCPNetworkEngine.GetInterface()!=NULL) m_TCPNetworkEngine->ConcludeService();

	//数据引擎
	if (m_RecordDataBaseEngine.GetInterface()!=NULL) m_RecordDataBaseEngine->ConcludeService();
	if (m_KernelDataBaseEngine.GetInterface()!=NULL) m_KernelDataBaseEngine->ConcludeService();
	m_DBCorrespondManager.ConcludeService();

	//注销组件
	if (m_GameServiceManager.GetInterface()!=NULL) m_GameServiceManager.CloseInstance();

	return true;
}


//创建模块
int CServiceUnits::CreateServiceDLL()
{
	//定时器引擎
	if ((m_TimerEngine.GetInterface()==NULL)&&(m_TimerEngine.CreateInstance()==false))
	{
		return -1;
	}

	//调度引擎
	if ((m_AttemperEngine.GetInterface()==NULL)&&(m_AttemperEngine.CreateInstance()==false))
	{
		return -2;
	}

	//socket::client
	if ((m_TCPSocketEngine.GetInterface()==NULL)&&(m_TCPSocketEngine.CreateInstance()==false))
	{
		return -3;
	}

	//socket::server
	if ((m_TCPNetworkEngine.GetInterface()==NULL)&&(m_TCPNetworkEngine.CreateInstance()==false))
	{
		return -4;
	}

	//DB
	if ((m_KernelDataBaseEngine.GetInterface()==NULL)&&(m_KernelDataBaseEngine.CreateInstance()==false))
	{
		return -5;
	}

	//数据组件
	if ((m_RecordDataBaseEngine.GetInterface()==NULL)&&(m_RecordDataBaseEngine.CreateInstance()==false))
	{
		return -6;
	}

	//游戏模块
	if ((m_GameServiceManager.GetInterface()==NULL)&&(m_GameServiceManager.CreateInstance()==false))
	{
		return -7;
	}

	return 0;
}

//配置组件
int CServiceUnits::InitializeService()
{
	//组件接口
	IUnknownEx * pIAttemperEngine=m_AttemperEngine.GetInterface();
	IUnknownEx * pITCPNetworkEngine=m_TCPNetworkEngine.GetInterface();
	IUnknownEx * pIAttemperEngineSink=QUERY_OBJECT_INTERFACE(m_AttemperEngineSink,IUnknownEx);

	//数据引擎
	IUnknownEx * pIDataBaseEngineRecordSink[CountArray(m_RecordDataBaseSink)];
	IUnknownEx * pIDataBaseEngineKernelSink[CountArray(m_KernelDataBaseSink)];
	for (WORD i=0;i<CountArray(pIDataBaseEngineRecordSink);i++) pIDataBaseEngineRecordSink[i]=QUERY_OBJECT_INTERFACE(m_RecordDataBaseSink[i],IUnknownEx);
	for (WORD i=0;i<CountArray(pIDataBaseEngineKernelSink);i++) pIDataBaseEngineKernelSink[i]=QUERY_OBJECT_INTERFACE(m_KernelDataBaseSink[i],IUnknownEx);

	//绑定接口
	if (m_AttemperEngine->SetAttemperEngineSink(pIAttemperEngineSink)==false) return false;
	if (m_RecordDataBaseEngine->SetDataBaseEngineSink(pIDataBaseEngineRecordSink,CountArray(pIDataBaseEngineRecordSink))==false) return false;
	if (m_KernelDataBaseEngine->SetDataBaseEngineSink(pIDataBaseEngineKernelSink,CountArray(pIDataBaseEngineKernelSink))==false) return false;

	//协调服务
	if (m_TCPSocketEngine->SetServiceID(NETWORK_CORRESPOND)==false) return false;

	//数据协调
	m_DBCorrespondManager.InitDBCorrespondManager(m_KernelDataBaseEngine.GetInterface());

	//调度回调
	m_AttemperEngineSink.m_pGameServiceAttrib = &m_GameServiceAttrib;
	m_AttemperEngineSink.m_pGameServiceOption = &m_GameServiceOption;

	//调度回调
	m_AttemperEngineSink.m_pITimerEngine=m_TimerEngine.GetInterface();
	m_AttemperEngineSink.m_pIAttemperEngine=m_AttemperEngine.GetInterface();
	m_AttemperEngineSink.m_pITCPSocketEngine=m_TCPSocketEngine.GetInterface();
	m_AttemperEngineSink.m_pITCPNetworkEngine=m_TCPNetworkEngine.GetInterface();
	m_AttemperEngineSink.m_pIGameServiceManager=m_GameServiceManager.GetInterface();
	m_AttemperEngineSink.m_pIRecordDataBaseEngine=m_RecordDataBaseEngine.GetInterface();
	m_AttemperEngineSink.m_pIKernelDataBaseEngine=m_KernelDataBaseEngine.GetInterface();
	m_AttemperEngineSink.m_pIDBCorrespondManager=(IDBCorrespondManager*)m_DBCorrespondManager.QueryInterface(IID_IDBCorrespondManager,VER_IDBCorrespondManager);

	//数据回调
	for (INT i=0;i<CountArray(m_RecordDataBaseSink);i++)
	{
		m_RecordDataBaseSink[i].m_pGameServiceAttrib=&m_GameServiceAttrib;
		m_RecordDataBaseSink[i].m_pGameServiceOption=&m_GameServiceOption;
		m_RecordDataBaseSink[i].m_pIGameServiceManager=m_GameServiceManager.GetInterface();
	}

	//数据回调
	for (INT i=0;i<CountArray(m_KernelDataBaseSink);i++)
	{
		m_KernelDataBaseSink[i].m_pGameServiceAttrib=&m_GameServiceAttrib;
		m_KernelDataBaseSink[i].m_pGameServiceOption=&m_GameServiceOption;
		m_KernelDataBaseSink[i].m_pIGameServiceManager=m_GameServiceManager.GetInterface();
		m_KernelDataBaseSink[i].m_pIDBCorrespondManager=(IDBCorrespondManager*)m_DBCorrespondManager.QueryInterface(IID_IDBCorrespondManager,VER_IDBCorrespondManager);
	}

	//配置网络
	m_TCPNetworkEngine->SetServiceParameter(m_GameServiceOption.wGameServerPort);

	//考虑到游戏服到现在才能知道ServerID, 因此只能将log的配置信息写到这里
	TCHAR szIniFile[MAX_PATH]=TEXT("");
	_sntprintf_s(szIniFile,CountArray(szIniFile),TEXT("GameServer-%d.log"), m_GameServiceOption.dwServerID);

	return true;
}

//启动内核
int CServiceUnits::StartKernelService()
{
	//时间引擎
	if (m_TimerEngine->StartService()==false)
	{
		return -1;
	}

	//调度引擎
	if (m_AttemperEngine->StartService()==false)
	{
		return -2;
	}

	//协调引擎
	if (m_TCPSocketEngine->StartService()==false)
	{
		return -3;
	}

	//数据引擎
	if (m_RecordDataBaseEngine->StartService()==false)
	{
		return -4;
	}

	//数据引擎
	if (m_KernelDataBaseEngine->StartService()==false)
	{
		return -5;
	}

	//数据协调
	if (m_DBCorrespondManager.StartService()==false)
	{
		return -6;
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
