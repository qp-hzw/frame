#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"

//////////////////////////////////////////////////////////////////////////////////

//静态变量
CServiceUnits *			CServiceUnits::g_pServiceUnits=NULL;			//对象指针

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CServiceUnits, CWnd)
	ON_MESSAGE(WM_UICONTROL_REQUEST,OnUIControlRequest)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CServiceUnits::CServiceUnits()
{
	//服务参数
	m_ServiceStatus=ServiceStatus_Stop;

	//设置接口
	m_pIServiceUnitsSink=NULL;

	//设置对象
	ASSERT(g_pServiceUnits==NULL);
	if (g_pServiceUnits==NULL) g_pServiceUnits=this;

	//组件配置
	ZeroMemory(&m_GameParameter,sizeof(m_GameParameter));
	ZeroMemory(&m_GameServiceAttrib,sizeof(m_GameServiceAttrib));
	ZeroMemory(&m_GameServiceOption,sizeof(m_GameServiceOption));



	return;
}

//析构函数
CServiceUnits::~CServiceUnits()
{
}

//设置接口
bool CServiceUnits::SetServiceUnitsSink(IServiceUnitsSink * pIServiceUnitsSink)
{
	//设置变量
	m_pIServiceUnitsSink=pIServiceUnitsSink;

	return true;
}

//投递请求
bool CServiceUnits::PostControlRequest(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	//状态判断
	ASSERT(IsWindow(m_hWnd));
	if (IsWindow(m_hWnd)==FALSE) return false;

	//插入队列
	CWHDataLocker DataLocker(m_CriticalSection);
	if (m_DataQueue.InsertData(wIdentifier,pData,wDataSize)==false) return false;

	//发送消息
	PostMessage(WM_UICONTROL_REQUEST,wIdentifier,wDataSize);

	return true;
}

//启动服务
bool CServiceUnits::StartService()
{
	//效验状态
	ASSERT(m_ServiceStatus==ServiceStatus_Stop);
	if (m_ServiceStatus!=ServiceStatus_Stop) return false;

	TCHAR szText[50];
	bool bResult = true;

	do 
	{
		//设置状态
		SetServiceStatus(ServiceStatus_Config);

		//创建窗口
		if (m_hWnd==NULL)
		{
			CRect rcCreate(0,0,0,0);
			Create(NULL,NULL,WS_CHILD,rcCreate,AfxGetMainWnd(),100);
		}

		//创建模块
		if (CreateServiceDLL()==false)
		{
			lstrcpy(szText, TEXT("创建模块失败"));
			bResult = false;
			break;
		}
		//调整参数
		if (RectifyServiceParameter()==false)
		{
			lstrcpy(szText, TEXT("调整参数失败"));
			bResult = false;
			break;
		}

		//配置服务
		if (InitializeService()==false)
		{
			lstrcpy(szText, TEXT("配置服务失败"));
			bResult = false;
			break;
		}

		//内核版本判断
		CWHIniData InitData;
		DWORD realKernel = InitData.Get_Code_Kernel_Version();
		DWORD frameKernel = Get_Kernel_Version();
		if(Compare_Kernek_Framework(realKernel, frameKernel) != 0)
		{		
			TCHAR pszString2[512]=TEXT("");
			_sntprintf_s(pszString2,CountArray(pszString2),TEXT("服务启动失败, 内核版本不匹配, realKernel: %ld; frameKernel: %ld\n"),
					realKernel,
					frameKernel);
			CLog::Log(pszString2,log_error);
			bResult = false;
			break;
		}

		//启动内核
		if (StartKernelService()==false)
		{
			lstrcpy(szText, TEXT("启动内核失败"));
			bResult = false;
			break;
		}

	} while (false);

	if(!bResult)
	{
		CLog::Log(szText,log_error);
		ConcludeService();
		return false;
	}

	//加载配置
	SendControlPacket(CT_LOAD_SERVICE_CONFIG,NULL,0);

	//启动比赛
	if (m_GameMatchServiceManager.GetInterface()!=NULL && m_GameMatchServiceManager->StartService()==false) return false;

	return true;
}

//停止服务
bool CServiceUnits::ConcludeService()
{
	if(m_ServiceStatus == ServiceStatus_Service)
	{
		//设置变量
		SetServiceStatus(ServiceStatus_Stopping);

		SendControlPacket(CT_TRY_TO_STOP_SERVICE,NULL,0);
	}
	else
	{
		StopServiceImmediate();
	}

	return true;
}


bool CServiceUnits::StopServiceImmediate()
{
	SetServiceStatus(ServiceStatus_Stop);

	//内核组件
	if (m_TimerEngine.GetInterface()!=NULL) m_TimerEngine->ConcludeService();
	if (m_AttemperEngine.GetInterface()!=NULL) m_AttemperEngine->ConcludeService();
	if (m_TCPSocketService.GetInterface()!=NULL) m_TCPSocketService->ConcludeService();
	if (m_TCPNetworkEngine.GetInterface()!=NULL) m_TCPNetworkEngine->ConcludeService();

	//数据引擎
	if (m_RecordDataBaseEngine.GetInterface()!=NULL) m_RecordDataBaseEngine->ConcludeService();
	if (m_KernelDataBaseEngine.GetInterface()!=NULL) m_KernelDataBaseEngine->ConcludeService();
	m_DBCorrespondManager.ConcludeService();

	//注销组件
	if (m_GameServiceManager.GetInterface()!=NULL) m_GameServiceManager.CloseInstance();
	if (m_GameMatchServiceManager.GetInterface()!=NULL)m_GameMatchServiceManager.CloseInstance();

	return true;
}

//游戏配置
bool CServiceUnits::CollocateService(LPCTSTR pszGameModule, tagGameServiceOption & GameServiceOption)
{
	//效验状态
	ASSERT(m_ServiceStatus==ServiceStatus_Stop);
	if (m_ServiceStatus!=ServiceStatus_Stop) return false;

	//配置模块(给服务单元赋值)
	m_GameServiceOption = GameServiceOption;

	m_GameServiceManager.SetModuleCreateInfo(pszGameModule,GAME_SERVICE_CREATE_NAME);

	return true;
}

//创建模块
bool CServiceUnits::CreateServiceDLL()
{
	//时间引擎
	if ((m_TimerEngine.GetInterface()==NULL)&&(m_TimerEngine.CreateInstance()==false))
	{
		CLog::Log(m_TimerEngine.GetErrorDescribe(),log_error);
		return false;
	}

	//调度引擎
	if ((m_AttemperEngine.GetInterface()==NULL)&&(m_AttemperEngine.CreateInstance()==false))
	{
		CLog::Log(m_AttemperEngine.GetErrorDescribe(),log_error);
		return false;
	}

	//网络组件
	if ((m_TCPSocketService.GetInterface()==NULL)&&(m_TCPSocketService.CreateInstance()==false))
	{
		CLog::Log(m_TCPSocketService.GetErrorDescribe(),log_error);
		return false;
	}

	//网络引擎
	if ((m_TCPNetworkEngine.GetInterface()==NULL)&&(m_TCPNetworkEngine.CreateInstance()==false))
	{
		CLog::Log(m_TCPNetworkEngine.GetErrorDescribe(),log_error);
		return false;
	}

	//数据组件
	if ((m_KernelDataBaseEngine.GetInterface()==NULL)&&(m_KernelDataBaseEngine.CreateInstance()==false))
	{
		CLog::Log(m_KernelDataBaseEngine.GetErrorDescribe(),log_error);
		return false;
	}

	//数据组件
	if ((m_RecordDataBaseEngine.GetInterface()==NULL)&&(m_RecordDataBaseEngine.CreateInstance()==false))
	{
		CLog::Log(m_RecordDataBaseEngine.GetErrorDescribe(),log_error);
		return false;
	}

	//游戏模块
	if ((m_GameServiceManager.GetInterface()==NULL)&&(m_GameServiceManager.CreateInstance()==false))
	{
		CLog::Log(m_GameServiceManager.GetErrorDescribe(),log_error);
		return false;
	}

	//if ((m_GameServiceOption.wServerType&GAME_GENRE_MATCH)!=0 )
	if(false)
	{
		if ((m_GameMatchServiceManager.GetInterface()==NULL)&&(m_GameMatchServiceManager.CreateInstance()==false))
		{
			CLog::Log(m_GameMatchServiceManager.GetErrorDescribe(),log_error);
			return false;
		}
	}

	return true;
}

//配置组件
bool CServiceUnits::InitializeService()
{
	//配置参数
	m_GameParameter.wMedalRate=1L;
	m_GameParameter.wRevenueRate=1L;

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

	//内核组件
	if (m_TimerEngine->SetTimerEngineEvent(pIAttemperEngine)==false) return false;
	if (m_AttemperEngine->SetNetworkEngine(pITCPNetworkEngine)==false) return false;
	if (m_TCPNetworkEngine->SetTCPNetworkEngineEvent(pIAttemperEngine)==false) return false;

	//协调服务
	if (m_TCPSocketService->SetServiceID(NETWORK_CORRESPOND)==false) return false;
	if (m_TCPSocketService->SetTCPSocketEvent(pIAttemperEngine)==false) return false;
	if (m_TCPSocketService->SetEncrypt(TRUE)==false) return false;

	//数据协调
	m_DBCorrespondManager.InitDBCorrespondManager(m_KernelDataBaseEngine.GetInterface());

	//调度回调
	m_AttemperEngineSink.m_pGameParameter = &m_GameParameter;
	m_AttemperEngineSink.m_pGameServiceAttrib = &m_GameServiceAttrib;
	m_AttemperEngineSink.m_pGameServiceOption = &m_GameServiceOption;

	//调度回调
	m_AttemperEngineSink.m_pITimerEngine=m_TimerEngine.GetInterface();
	m_AttemperEngineSink.m_pIAttemperEngine=m_AttemperEngine.GetInterface();
	m_AttemperEngineSink.m_pITCPSocketService=m_TCPSocketService.GetInterface();
	m_AttemperEngineSink.m_pITCPNetworkEngine=m_TCPNetworkEngine.GetInterface();
	m_AttemperEngineSink.m_pIGameServiceManager=m_GameServiceManager.GetInterface();
	m_AttemperEngineSink.m_pIRecordDataBaseEngine=m_RecordDataBaseEngine.GetInterface();
	m_AttemperEngineSink.m_pIKernelDataBaseEngine=m_KernelDataBaseEngine.GetInterface();
	m_AttemperEngineSink.m_pIGameMatchServiceManager=m_GameMatchServiceManager.GetInterface();
	m_AttemperEngineSink.m_pIDBCorrespondManager=(IDBCorrespondManager*)m_DBCorrespondManager.QueryInterface(IID_IDBCorrespondManager,VER_IDBCorrespondManager);

	//数据回调
	for (INT i=0;i<CountArray(m_RecordDataBaseSink);i++)
	{
		m_RecordDataBaseSink[i].m_pGameParameter=&m_GameParameter;
		m_RecordDataBaseSink[i].m_pGameServiceAttrib=&m_GameServiceAttrib;
		m_RecordDataBaseSink[i].m_pGameServiceOption=&m_GameServiceOption;
		m_RecordDataBaseSink[i].m_pIGameServiceManager=m_GameServiceManager.GetInterface();
		m_RecordDataBaseSink[i].m_pIDataBaseEngineEvent=QUERY_OBJECT_PTR_INTERFACE(pIAttemperEngine,IDataBaseEngineEvent);
	}

	//数据回调
	for (INT i=0;i<CountArray(m_KernelDataBaseSink);i++)
	{
		m_KernelDataBaseSink[i].m_pGameParameter=&m_GameParameter;
		m_KernelDataBaseSink[i].m_pGameServiceAttrib=&m_GameServiceAttrib;
		m_KernelDataBaseSink[i].m_pGameServiceOption=&m_GameServiceOption;
		m_KernelDataBaseSink[i].m_pIGameServiceManager=m_GameServiceManager.GetInterface();
		m_KernelDataBaseSink[i].m_pIDataBaseEngineEvent=QUERY_OBJECT_PTR_INTERFACE(pIAttemperEngine,IDataBaseEngineEvent);
		m_KernelDataBaseSink[i].m_pIDBCorrespondManager=(IDBCorrespondManager*)m_DBCorrespondManager.QueryInterface(IID_IDBCorrespondManager,VER_IDBCorrespondManager);
	}

	//配置网络
	m_TCPNetworkEngine->SetServiceParameter(m_GameServiceOption.wGameServerPort, MAX_TABLE,TRUE );

	//考虑到游戏服到现在才能知道ServerID, 因此只能将log的配置信息写到这里
	TCHAR szIniFile[MAX_PATH]=TEXT("");
	_sntprintf_s(szIniFile,CountArray(szIniFile),TEXT("GameServer-%d.log"), m_GameServiceOption.dwServerID);

	//设置服务器日志输出等级
	CLog::EnableTrace(log_debug,log_debug,szIniFile);

	return true;
}

//启动内核
bool CServiceUnits::StartKernelService()
{
	//时间引擎
	if (m_TimerEngine->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//调度引擎
	if (m_AttemperEngine->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//协调引擎
	if (m_TCPSocketService->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//数据引擎
	if (m_RecordDataBaseEngine->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//数据引擎
	if (m_KernelDataBaseEngine->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//数据协调
	if (m_DBCorrespondManager.StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//启动网络
bool CServiceUnits::StartNetworkService()
{
	//网络引擎
	if (m_TCPNetworkEngine->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//调整参数
bool CServiceUnits::RectifyServiceParameter()
{
	//读取属性
	m_GameServiceManager->GetServiceAttrib(m_GameServiceAttrib);

	//调整参数
	if (m_GameServiceManager->RectifyParameter(m_GameServiceOption)==false)
	{
		CLog::Log(TEXT("游戏模块调整配置参数失败"),log_error);
		return false;
	}

	//比赛调整
	if (m_GameMatchServiceManager.GetInterface()!=NULL)
	{
		m_GameMatchServiceManager->RectifyServiceOption(&m_GameServiceOption,&m_GameServiceAttrib);
	}

	return true;
}

//设置状态
bool CServiceUnits::SetServiceStatus(enServiceStatus ServiceStatus)
{
	//状态判断
	if (m_ServiceStatus!=ServiceStatus)
	{
		//错误通知
		if ((m_ServiceStatus!=ServiceStatus_Service)&&(ServiceStatus==ServiceStatus_Stopping))
		{
			LPCTSTR pszString=TEXT("服务启动失败");
			CLog::Log(pszString,log_error);
		}

		//设置变量
		m_ServiceStatus=ServiceStatus;

		//状态通知
		ASSERT(m_pIServiceUnitsSink!=NULL);
		if (m_pIServiceUnitsSink!=NULL) m_pIServiceUnitsSink->OnServiceUnitsStatus(m_ServiceStatus);
	}

	return true;
}

//发送控制
bool CServiceUnits::SendControlPacket(WORD wControlID, VOID * pData, WORD wDataSize)
{
	//状态效验
	if (m_AttemperEngine.GetInterface()==NULL) return false;

	//发送控制
	m_AttemperEngine->OnEventControl(wControlID,pData,wDataSize);

	return true;
}

//控制消息
LRESULT CServiceUnits::OnUIControlRequest(WPARAM wParam, LPARAM lParam)
{
	//变量定义
	tagDataHead DataHead;
	BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];

	//提取数据
	CWHDataLocker DataLocker(m_CriticalSection);
	if (m_DataQueue.DistillData(DataHead,cbBuffer,sizeof(cbBuffer))==false)
	{
		ASSERT(FALSE);
		return NULL;
	}

	//数据处理
	switch (DataHead.wIdentifier)
	{
	case UI_CORRESPOND_RESULT:		//协调成功
		{
			//效验消息
			if (DataHead.wDataSize!=sizeof(CP_ControlResult)) return 0;

			//变量定义
			CP_ControlResult * pControlResult=(CP_ControlResult *)cbBuffer;

			//失败处理
			if ((m_ServiceStatus!=ServiceStatus_Service)&&(pControlResult->cbSuccess==ER_FAILURE))
			{
				ConcludeService();
				return 0;
			}

			//成功处理
			if ((m_ServiceStatus!=ServiceStatus_Service)&&(pControlResult->cbSuccess==ER_SUCCESS))
			{
				//设置状态
				SetServiceStatus(ServiceStatus_Service);
			}

			return 0;
		}
	case UI_SERVICE_CONFIG_RESULT:	//配置结果
		{
			//效验消息
			ASSERT(DataHead.wDataSize==sizeof(CP_ControlResult));
			if (DataHead.wDataSize!=sizeof(CP_ControlResult)) return 0;

			//变量定义
			CP_ControlResult * pControlResult=(CP_ControlResult *)cbBuffer;

			//失败处理
			if ((m_ServiceStatus!=ServiceStatus_Service)&&(pControlResult->cbSuccess==ER_FAILURE))
			{
				ConcludeService();
				return 0;
			}

			//成功处理
			if ((m_ServiceStatus!=ServiceStatus_Service)&&(pControlResult->cbSuccess==ER_SUCCESS))
			{
				//启动网络
				if (StartNetworkService()==false)
				{
					ConcludeService();
					return 0;
				}

				//连接协调
				SendControlPacket(CT_CONNECT_CORRESPOND,NULL,0);
			}

			return 0;
		}
	case UI_SERVICE_STOP_SERVICE_FINISH:
		{
			//效验消息
			ASSERT(DataHead.wDataSize==sizeof(CP_ControlResult));
			if (DataHead.wDataSize!=sizeof(CP_ControlResult)) return 0;

			//变量定义
			CP_ControlResult * pControlResult=(CP_ControlResult *)cbBuffer;

			//失败处理
			if ((m_ServiceStatus!=ServiceStatus_Stopping)&&(pControlResult->cbSuccess==ER_FAILURE))
			{
				CLog::Log(TEXT("停止失败"),log_error);
			}

			StopServiceImmediate();
		}
	}

	return 0;
}


//////////////////////////////////////////////////////////////////////////////////
