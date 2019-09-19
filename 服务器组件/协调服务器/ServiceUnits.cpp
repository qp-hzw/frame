#include "StdAfx.h"
#include "ServiceUnits.h"

//启动服务
bool CServiceUnits::StartService()
{
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
		CTraceService::TraceString(TraceLevel_Exception, pszString2);

		return false;
	}

	//启动内核
	if (StartKernelService()==false)
	{
		ConcludeService();
		return false;
	}

	return true;
}

//停止服务
bool CServiceUnits::ConcludeService()
{
	//停止服务
	if (m_AttemperEngine.GetInterface()!=NULL) m_AttemperEngine->ConcludeService();
	if (m_TCPNetworkEngine.GetInterface()!=NULL) m_TCPNetworkEngine->ConcludeService();

	return true;
}

//配置组件
bool CServiceUnits::InitializeService()
{
	//创建组件
	if ((m_AttemperEngine.GetInterface()==NULL)&&(m_AttemperEngine.CreateInstance()==false)) return false;
	if ((m_TCPNetworkEngine.GetInterface()==NULL)&&(m_TCPNetworkEngine.CreateInstance()==false)) return false;

	//组件接口
	IUnknownEx * pIAttemperEngine=m_AttemperEngine.GetInterface();
	IUnknownEx * pIAttemperEngineSink=QUERY_OBJECT_INTERFACE(m_AttemperEngineSink,IUnknownEx);

	//设置各服务回调为AttemperEngine
	if (m_TCPNetworkEngine->SetTCPNetworkEngineEvent(pIAttemperEngine)==false) return false;

	//AttemperEngine设置回调
	if (m_AttemperEngine->SetAttemperEngineSink(pIAttemperEngineSink)==false) return false;

	//调度回调
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
		return false;
	}

	//网络引擎
	if (m_TCPNetworkEngine->StartService()==false)
	{
		return false;
	}

	return true;
}