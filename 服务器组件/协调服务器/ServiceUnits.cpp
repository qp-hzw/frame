#include "StdAfx.h"
#include "ServiceUnits.h"

//启动服务
bool CServiceUnits::StartService()
{
	/*
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
		CLog::Log(log_error, pszString2);

		return false;
	}
	*/

	//配置服务
	int iRet = InitializeService();
	if (iRet != 0 )
	{
		CLog::Log(log_error, TEXT("%s : %d"), TEXT("CServiceUnits::InitializeService"), iRet);
		ConcludeService();
		return false;
	}

	//启动内核
	iRet = StartKernelService();
	if (iRet != 0)
	{
		CLog::Log(log_error, TEXT("%s : %d"), TEXT("CServiceUnits::StartKernelService"), iRet);
		ConcludeService();
		return false;
	}

	CLog::Log(log_error, TEXT("%s : %d"), TEXT("CServiceUnits::StartService"), iRet);
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
int CServiceUnits::InitializeService()
{
	//设置服务器日志输出等级
	CLog::EnableTrace(log_debug,log_debug,TEXT("correspond.log"));

	//创建组件
	if ((m_AttemperEngine.GetInterface()==NULL)&&(m_AttemperEngine.CreateInstance()==false)) return 1;
	if ((m_TCPNetworkEngine.GetInterface()==NULL)&&(m_TCPNetworkEngine.CreateInstance()==false)) return 2;

	//组件接口
	IUnknownEx * pIAttemperEngine=m_AttemperEngine.GetInterface();
	IUnknownEx * pIAttemperEngineSink=QUERY_OBJECT_INTERFACE(m_AttemperEngineSink,IUnknownEx);

	//设置各服务回调为AttemperEngine
	if (m_TCPNetworkEngine->SetTCPNetworkEngineEvent(pIAttemperEngine)==false) return 3;

	//AttemperEngine设置回调
	if (m_AttemperEngine->SetAttemperEngineSink(pIAttemperEngineSink)==false) return 4;

	//调度回调
	m_AttemperEngineSink.m_pITCPNetworkEngine=m_TCPNetworkEngine.GetInterface();

	//配置网络
	WORD wMaxConnect=MAX_CONTENT;
	WORD wServicePort=PORT_CENTER;
	if (m_TCPNetworkEngine->SetServiceParameter(wServicePort,wMaxConnect, TRUE)==false) return 5;

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