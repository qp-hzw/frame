#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"


//��̬����
CServiceUnits *			g_pServiceUnits=NULL;			//����ָ��


//���캯��
CServiceUnits::CServiceUnits()
{
	//���ö���
	if (g_pServiceUnits==NULL) g_pServiceUnits=this;

	//�������
	ZeroMemory(&m_GameServiceAttrib,sizeof(m_GameServiceAttrib));
	ZeroMemory(&m_GameServiceOption,sizeof(m_GameServiceOption));

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
	//����ģ��
	int iRet = CreateServiceDLL();
	if (iRet != 0)
	{
		CLog::Log(log_error, "CServiceUnits::CreateServiceDLL %d", iRet);
		ConcludeService();
		return false;
	}

	//���÷���
	iRet = InitializeService();
	if (iRet != 0)
	{
		CLog::Log(log_error, "CServiceUnits::InitializeService %d", iRet);
		ConcludeService();
		return false;
	}

	//�����ں�
	iRet = StartKernelService();
	if (iRet != 0)
	{
		CLog::Log(log_error, "CServiceUnits::InitializeService %d", iRet);
		ConcludeService();
		return false;
	}

	//����Э��
	if (m_TCPSocketEngine.GetInterface()!=NULL)
	{
		m_TCPSocketEngine.GetInterface()->Connect(_CPD_SERVER_ADDR, PORT_CENTER);
	}

	return true;
}

//ֹͣ����
bool CServiceUnits::ConcludeService()
{

	//�ں����
	if (m_TimerEngine.GetInterface()!=NULL) m_TimerEngine->ConcludeService();
	if (m_AttemperEngine.GetInterface()!=NULL) m_AttemperEngine->ConcludeService();
	if (m_TCPSocketEngine.GetInterface()!=NULL) m_TCPSocketEngine->ConcludeService();
	if (m_TCPNetworkEngine.GetInterface()!=NULL) m_TCPNetworkEngine->ConcludeService();

	//��������
	if (m_RecordDataBaseEngine.GetInterface()!=NULL) m_RecordDataBaseEngine->ConcludeService();
	if (m_KernelDataBaseEngine.GetInterface()!=NULL) m_KernelDataBaseEngine->ConcludeService();
	m_DBCorrespondManager.ConcludeService();

	//ע�����
	if (m_GameServiceManager.GetInterface()!=NULL) m_GameServiceManager.CloseInstance();

	return true;
}


//����ģ��
int CServiceUnits::CreateServiceDLL()
{
	//��ʱ������
	if ((m_TimerEngine.GetInterface()==NULL)&&(m_TimerEngine.CreateInstance()==false))
	{
		return -1;
	}

	//��������
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

	//�������
	if ((m_RecordDataBaseEngine.GetInterface()==NULL)&&(m_RecordDataBaseEngine.CreateInstance()==false))
	{
		return -6;
	}

	//��Ϸģ��
	if ((m_GameServiceManager.GetInterface()==NULL)&&(m_GameServiceManager.CreateInstance()==false))
	{
		return -7;
	}

	return 0;
}

//�������
int CServiceUnits::InitializeService()
{
	//����ӿ�
	IUnknownEx * pIAttemperEngine=m_AttemperEngine.GetInterface();
	IUnknownEx * pITCPNetworkEngine=m_TCPNetworkEngine.GetInterface();
	IUnknownEx * pIAttemperEngineSink=QUERY_OBJECT_INTERFACE(m_AttemperEngineSink,IUnknownEx);

	//��������
	IUnknownEx * pIDataBaseEngineRecordSink[CountArray(m_RecordDataBaseSink)];
	IUnknownEx * pIDataBaseEngineKernelSink[CountArray(m_KernelDataBaseSink)];
	for (WORD i=0;i<CountArray(pIDataBaseEngineRecordSink);i++) pIDataBaseEngineRecordSink[i]=QUERY_OBJECT_INTERFACE(m_RecordDataBaseSink[i],IUnknownEx);
	for (WORD i=0;i<CountArray(pIDataBaseEngineKernelSink);i++) pIDataBaseEngineKernelSink[i]=QUERY_OBJECT_INTERFACE(m_KernelDataBaseSink[i],IUnknownEx);

	//�󶨽ӿ�
	if (m_AttemperEngine->SetAttemperEngineSink(pIAttemperEngineSink)==false) return false;
	if (m_RecordDataBaseEngine->SetDataBaseEngineSink(pIDataBaseEngineRecordSink,CountArray(pIDataBaseEngineRecordSink))==false) return false;
	if (m_KernelDataBaseEngine->SetDataBaseEngineSink(pIDataBaseEngineKernelSink,CountArray(pIDataBaseEngineKernelSink))==false) return false;

	//Э������
	if (m_TCPSocketEngine->SetServiceID(NETWORK_CORRESPOND)==false) return false;

	//����Э��
	m_DBCorrespondManager.InitDBCorrespondManager(m_KernelDataBaseEngine.GetInterface());

	//���Ȼص�
	m_AttemperEngineSink.m_pGameServiceAttrib = &m_GameServiceAttrib;
	m_AttemperEngineSink.m_pGameServiceOption = &m_GameServiceOption;

	//���Ȼص�
	m_AttemperEngineSink.m_pITimerEngine=m_TimerEngine.GetInterface();
	m_AttemperEngineSink.m_pIAttemperEngine=m_AttemperEngine.GetInterface();
	m_AttemperEngineSink.m_pITCPSocketEngine=m_TCPSocketEngine.GetInterface();
	m_AttemperEngineSink.m_pITCPNetworkEngine=m_TCPNetworkEngine.GetInterface();
	m_AttemperEngineSink.m_pIGameServiceManager=m_GameServiceManager.GetInterface();
	m_AttemperEngineSink.m_pIRecordDataBaseEngine=m_RecordDataBaseEngine.GetInterface();
	m_AttemperEngineSink.m_pIKernelDataBaseEngine=m_KernelDataBaseEngine.GetInterface();
	m_AttemperEngineSink.m_pIDBCorrespondManager=(IDBCorrespondManager*)m_DBCorrespondManager.QueryInterface(IID_IDBCorrespondManager,VER_IDBCorrespondManager);

	//���ݻص�
	for (INT i=0;i<CountArray(m_RecordDataBaseSink);i++)
	{
		m_RecordDataBaseSink[i].m_pGameServiceAttrib=&m_GameServiceAttrib;
		m_RecordDataBaseSink[i].m_pGameServiceOption=&m_GameServiceOption;
		m_RecordDataBaseSink[i].m_pIGameServiceManager=m_GameServiceManager.GetInterface();
	}

	//���ݻص�
	for (INT i=0;i<CountArray(m_KernelDataBaseSink);i++)
	{
		m_KernelDataBaseSink[i].m_pGameServiceAttrib=&m_GameServiceAttrib;
		m_KernelDataBaseSink[i].m_pGameServiceOption=&m_GameServiceOption;
		m_KernelDataBaseSink[i].m_pIGameServiceManager=m_GameServiceManager.GetInterface();
		m_KernelDataBaseSink[i].m_pIDBCorrespondManager=(IDBCorrespondManager*)m_DBCorrespondManager.QueryInterface(IID_IDBCorrespondManager,VER_IDBCorrespondManager);
	}

	//��������
	m_TCPNetworkEngine->SetServiceParameter(m_GameServiceOption.wGameServerPort);

	//���ǵ���Ϸ�������ڲ���֪��ServerID, ���ֻ�ܽ�log��������Ϣд������
	TCHAR szIniFile[MAX_PATH]=TEXT("");
	_sntprintf_s(szIniFile,CountArray(szIniFile),TEXT("GameServer-%d.log"), m_GameServiceOption.dwServerID);

	return true;
}

//�����ں�
int CServiceUnits::StartKernelService()
{
	//ʱ������
	if (m_TimerEngine->StartService()==false)
	{
		return -1;
	}

	//��������
	if (m_AttemperEngine->StartService()==false)
	{
		return -2;
	}

	//Э������
	if (m_TCPSocketEngine->StartService()==false)
	{
		return -3;
	}

	//��������
	if (m_RecordDataBaseEngine->StartService()==false)
	{
		return -4;
	}

	//��������
	if (m_KernelDataBaseEngine->StartService()==false)
	{
		return -5;
	}

	//����Э��
	if (m_DBCorrespondManager.StartService()==false)
	{
		return -6;
	}

	return 0;
}

//��������
bool CServiceUnits::StartNetworkService()
{
	//��������
	if (m_TCPNetworkEngine->StartService()==false)
	{
		return false;
	}

	return true;
}
