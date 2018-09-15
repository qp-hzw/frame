#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CServiceUnits::CServiceUnits()
{
	//���ýӿ�
	m_pIServiceUnitsSink=NULL;

	//���ñ���
	m_ServiceStatus=ServiceStatus_Stop;

	return;
}

//��������
CServiceUnits::~CServiceUnits()
{
	ConcludeService();
}

//��������
bool CServiceUnits::StartService()
{
	//Ч��״̬
	ASSERT(m_ServiceStatus==ServiceStatus_Stop);
	if (m_ServiceStatus!=ServiceStatus_Stop) return false;

	//����״̬
	SetServiceStatus(ServiceStatus_Config);

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
		CTraceService::TraceString(pszString2,TraceLevel_Exception);

		return false;
	}

	//�����ں�
	if (StartKernelService()==false)
	{
		ConcludeService();
		return false;
	}

	//����״̬
	SetServiceStatus(ServiceStatus_Service);

	return true;
}

//ֹͣ����
bool CServiceUnits::ConcludeService()
{
	//���ñ���
	SetServiceStatus(ServiceStatus_Stop);

	//ֹͣ����
	if (m_TimerEngine.GetInterface()!=NULL) m_TimerEngine->ConcludeService();
	if (m_AttemperEngine.GetInterface()!=NULL) m_AttemperEngine->ConcludeService();
	if (m_TCPNetworkEngine.GetInterface()!=NULL) m_TCPNetworkEngine->ConcludeService();

	return true;
}

//���ýӿ�
bool CServiceUnits::SetServiceUnitsSink(IServiceUnitsSink * pIServiceUnitsSink)
{
	//���ñ���
	m_pIServiceUnitsSink=pIServiceUnitsSink;

	return true;
}

//�������
bool CServiceUnits::InitializeService()
{
	//�������
	if ((m_TimerEngine.GetInterface()==NULL)&&(m_TimerEngine.CreateInstance()==false)) return false;
	if ((m_AttemperEngine.GetInterface()==NULL)&&(m_AttemperEngine.CreateInstance()==false)) return false;
	if ((m_TCPNetworkEngine.GetInterface()==NULL)&&(m_TCPNetworkEngine.CreateInstance()==false)) return false;

	//����ӿ�
	IUnknownEx * pIAttemperEngine=m_AttemperEngine.GetInterface();
	IUnknownEx * pITCPNetworkEngine=m_TCPNetworkEngine.GetInterface();

	//�ص��ӿ�
	IUnknownEx * pIAttemperEngineSink=QUERY_OBJECT_INTERFACE(m_AttemperEngineSink,IUnknownEx);

	//�󶨽ӿ�
	if (m_AttemperEngine->SetAttemperEngineSink(pIAttemperEngineSink)==false) return false;

	//�ں����
	if (m_TimerEngine->SetTimerEngineEvent(pIAttemperEngine)==false) return false;
	if (m_AttemperEngine->SetNetworkEngine(pITCPNetworkEngine)==false) return false;
	if (m_TCPNetworkEngine->SetTCPNetworkEngineEvent(pIAttemperEngine)==false) return false;

	//���Ȼص�
	m_AttemperEngineSink.m_pITimerEngine=m_TimerEngine.GetInterface();
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
		ASSERT(FALSE);
		return false;
	}

	//��������
	if (m_TCPNetworkEngine->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//ʱ������
	if (m_TimerEngine->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//����״̬
bool CServiceUnits::SetServiceStatus(enServiceStatus ServiceStatus)
{
	//״̬�ж�
	if (m_ServiceStatus!=ServiceStatus)
	{
		//����֪ͨ
		if ((m_ServiceStatus!=ServiceStatus_Service)&&(ServiceStatus==ServiceStatus_Stop))
		{
			LPCTSTR pszString=TEXT("��������ʧ��");
			CTraceService::TraceString(pszString,TraceLevel_Exception);
		}

		//���ñ���
		m_ServiceStatus=ServiceStatus;

		//״̬֪ͨ
		ASSERT(m_pIServiceUnitsSink!=NULL);
		if (m_pIServiceUnitsSink!=NULL) m_pIServiceUnitsSink->OnServiceUnitsStatus(m_ServiceStatus);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
