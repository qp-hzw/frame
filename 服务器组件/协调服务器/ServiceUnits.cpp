#include "StdAfx.h"
#include "ServiceUnits.h"

//��������
bool CServiceUnits::StartService()
{
	/*
	//�ں˰汾�ж�
	CWHIniData InitData;
	DWORD realKernel = InitData.Get_Code_Kernel_Version();
	DWORD frameKernel = Get_Kernel_Version();
	if(Compare_Kernek_Framework(realKernel, frameKernel) != 0)
	{
		TCHAR pszString2[512]=TEXT("");
		_sntprintf_s(pszString2,CountArray(pszString2),TEXT("��������ʧ��, �ں˰汾��ƥ��, realKernel: %ld; frameKernel: %ld\n"),
					realKernel,
					frameKernel);
		CLog::Log(log_error, pszString2);

		return false;
	}
	*/

	//���÷���
	int iRet = InitializeService();
	if (iRet != 0 )
	{
		CLog::Log(log_error, TEXT("%s : %d"), TEXT("CServiceUnits::InitializeService"), iRet);
		ConcludeService();
		return false;
	}

	//�����ں�
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

//ֹͣ����
bool CServiceUnits::ConcludeService()
{
	//ֹͣ����
	if (m_AttemperEngine.GetInterface()!=NULL) m_AttemperEngine->ConcludeService();
	if (m_TCPNetworkEngine.GetInterface()!=NULL) m_TCPNetworkEngine->ConcludeService();

	return true;
}

//�������
int CServiceUnits::InitializeService()
{
	//���÷�������־����ȼ�
	CLog::EnableTrace(log_debug,log_debug,TEXT("correspond.log"));

	//�������
	if ((m_AttemperEngine.GetInterface()==NULL)&&(m_AttemperEngine.CreateInstance()==false)) return 1;
	if ((m_TCPNetworkEngine.GetInterface()==NULL)&&(m_TCPNetworkEngine.CreateInstance()==false)) return 2;

	//����ӿ�
	IUnknownEx * pIAttemperEngine=m_AttemperEngine.GetInterface();
	IUnknownEx * pIAttemperEngineSink=QUERY_OBJECT_INTERFACE(m_AttemperEngineSink,IUnknownEx);

	//���ø�����ص�ΪAttemperEngine
	if (m_TCPNetworkEngine->SetTCPNetworkEngineEvent(pIAttemperEngine)==false) return 3;

	//AttemperEngine���ûص�
	if (m_AttemperEngine->SetAttemperEngineSink(pIAttemperEngineSink)==false) return 4;

	//���Ȼص�
	m_AttemperEngineSink.m_pITCPNetworkEngine=m_TCPNetworkEngine.GetInterface();

	//��������
	WORD wMaxConnect=MAX_CONTENT;
	WORD wServicePort=PORT_CENTER;
	if (m_TCPNetworkEngine->SetServiceParameter(wServicePort,wMaxConnect, TRUE)==false) return 5;

	return 0;
}

//�����ں�
int CServiceUnits::StartKernelService()
{
	//��������
	if (m_AttemperEngine->StartService()==false)
	{
		return 1;
	}

	//��������
	if (m_TCPNetworkEngine->StartService()==false)
	{
		return 2;
	}

	return 0;
}