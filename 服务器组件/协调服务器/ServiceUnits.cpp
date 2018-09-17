#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CServiceUnits::CServiceUnits()
{
	//设置接口
	m_pIServiceUnitsSink=NULL;

	//设置变量
	m_ServiceStatus=ServiceStatus_Stop;

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
	//效验状态
	ASSERT(m_ServiceStatus==ServiceStatus_Stop);
	if (m_ServiceStatus!=ServiceStatus_Stop) return false;

	//设置状态
	SetServiceStatus(ServiceStatus_Config);

	//配置服务
	if (InitializeService()==false)
	{
		ConcludeService();
		return false;
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
		CTraceService::TraceString(pszString2,TraceLevel_Exception);

		return false;
	}

	//启动内核
	if (StartKernelService()==false)
	{
		ConcludeService();
		return false;
	}

	//设置状态
	SetServiceStatus(ServiceStatus_Service);

	return true;
}

//停止服务
bool CServiceUnits::ConcludeService()
{
	//设置变量
	SetServiceStatus(ServiceStatus_Stop);

	//停止服务
	if (m_TimerEngine.GetInterface()!=NULL) m_TimerEngine->ConcludeService();
	if (m_AttemperEngine.GetInterface()!=NULL) m_AttemperEngine->ConcludeService();
	if (m_TCPNetworkEngine.GetInterface()!=NULL) m_TCPNetworkEngine->ConcludeService();

	return true;
}

//设置接口
bool CServiceUnits::SetServiceUnitsSink(IServiceUnitsSink * pIServiceUnitsSink)
{
	//设置变量
	m_pIServiceUnitsSink=pIServiceUnitsSink;

	return true;
}

//配置组件
bool CServiceUnits::InitializeService()
{
	//创建组件
	if ((m_TimerEngine.GetInterface()==NULL)&&(m_TimerEngine.CreateInstance()==false)) return false;
	if ((m_AttemperEngine.GetInterface()==NULL)&&(m_AttemperEngine.CreateInstance()==false)) return false;
	if ((m_TCPNetworkEngine.GetInterface()==NULL)&&(m_TCPNetworkEngine.CreateInstance()==false)) return false;

	//组件接口
	IUnknownEx * pIAttemperEngine=m_AttemperEngine.GetInterface();
	IUnknownEx * pITCPNetworkEngine=m_TCPNetworkEngine.GetInterface();

	//回调接口
	IUnknownEx * pIAttemperEngineSink=QUERY_OBJECT_INTERFACE(m_AttemperEngineSink,IUnknownEx);

	//绑定接口
	if (m_AttemperEngine->SetAttemperEngineSink(pIAttemperEngineSink)==false) return false;

	//内核组件
	if (m_TimerEngine->SetTimerEngineEvent(pIAttemperEngine)==false) return false;
	if (m_AttemperEngine->SetNetworkEngine(pITCPNetworkEngine)==false) return false;
	if (m_TCPNetworkEngine->SetTCPNetworkEngineEvent(pIAttemperEngine)==false) return false;

	//调度回调
	m_AttemperEngineSink.m_pITimerEngine=m_TimerEngine.GetInterface();
	m_AttemperEngineSink.m_pITCPNetworkEngine=m_TCPNetworkEngine.GetInterface();

	//配置网络
	WORD wMaxConnect=MAX_CONTENT;
	WORD wServicePort=PORT_CENTER;
	if (m_TCPNetworkEngine->SetServiceParameter(wServicePort,wMaxConnect, TRUE)==false) return false;

	//log的配置信息
	TCHAR szIniFile[MAX_PATH]=TEXT("");
	_sntprintf_s(szIniFile,CountArray(szIniFile),TEXT("correspond.log"));

	//设置服务器日志输出等级
	CTraceService::EnableTrace(TraceLevel_Info,TraceLevel_Debug,szIniFile);

	return true;
}

//启动内核
bool CServiceUnits::StartKernelService()
{
	//调度引擎
	if (m_AttemperEngine->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//网络引擎
	if (m_TCPNetworkEngine->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//时间引擎
	if (m_TimerEngine->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
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
		if ((m_ServiceStatus!=ServiceStatus_Service)&&(ServiceStatus==ServiceStatus_Stop))
		{
			LPCTSTR pszString=TEXT("服务启动失败");
			CTraceService::TraceString(pszString,TraceLevel_Exception);
		}

		//设置变量
		m_ServiceStatus=ServiceStatus;

		//状态通知
		ASSERT(m_pIServiceUnitsSink!=NULL);
		if (m_pIServiceUnitsSink!=NULL) m_pIServiceUnitsSink->OnServiceUnitsStatus(m_ServiceStatus);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
