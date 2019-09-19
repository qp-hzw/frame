#include "StdAfx.h"
#include "ServiceUnits.h"

//��������
bool CServiceUnits::StartService()
{
	//���÷���
	if (InitializeService()==false)
	{
		ConcludeService();
		return false;
	}

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
		CTraceService::TraceString(TraceLevel_Exception, pszString2);

		return false;
	}

	//�����ں�
	if (StartKernelService()==false)
	{
		ConcludeService();
		return false;
	}

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
bool CServiceUnits::InitializeService()
{
	//�������
	if ((m_AttemperEngine.GetInterface()==NULL)&&(m_AttemperEngine.CreateInstance()==false)) return false;
	if ((m_TCPNetworkEngine.GetInterface()==NULL)&&(m_TCPNetworkEngine.CreateInstance()==false)) return false;

	//����ӿ�
	IUnknownEx * pIAttemperEngine=m_AttemperEngine.GetInterface();
	IUnknownEx * pIAttemperEngineSink=QUERY_OBJECT_INTERFACE(m_AttemperEngineSink,IUnknownEx);

	//���ø�����ص�ΪAttemperEngine
	if (m_TCPNetworkEngine->SetTCPNetworkEngineEvent(pIAttemperEngine)==false) return false;

	//AttemperEngine���ûص�
	if (m_AttemperEngine->SetAttemperEngineSink(pIAttemperEngineSink)==false) return false;

	//���Ȼص�
	m_AttemperEngineSink.m_pITCPNetworkEngine=m_TCPNetworkEngine.GetInterface();

	//��������
	WORD wMaxConnect=MAX_CONTENT;
	WORD wServicePort=PORT_CENTER;
	if (m_TCPNetworkEngine->SetServiceParameter(wServicePort,wMaxConnect, TRUE)==false) return false;

	//log��������Ϣ
	TCHAR szIniFile[MAX_PATH]=TEXT("");
	_sntprintf_s(szIniFile,CountArray(szIniFile),TEXT("correspond.log"));

	//���÷�������־����ȼ�
	CTraceService::EnableTrace(TraceLevel_Info,TraceLevel_Debug,szIniFile);

	return true;
}

//�����ں�
bool CServiceUnits::StartKernelService()
{
	//��������
	if (m_AttemperEngine->StartService()==false)
	{
		return false;
	}

	//��������
	if (m_TCPNetworkEngine->StartService()==false)
	{
		return false;
	}

	return true;
}