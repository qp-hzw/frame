#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"

//////////////////////////////////////////////////////////////////////////////////

//��̬����
CServiceUnits *			CServiceUnits::g_pServiceUnits=NULL;			//����ָ��

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CServiceUnits, CWnd)
	ON_MESSAGE(WM_UICONTROL_REQUEST,OnUIControlRequest)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CServiceUnits::CServiceUnits()
{
	//���ö���
	if (g_pServiceUnits==NULL) g_pServiceUnits=this;

	return;
}

//��������
CServiceUnits::~CServiceUnits()
{
	ConcludeService();
}

//�������
int CServiceUnits::InitializeService()
{
	//���÷�������־����ȼ�
	bool bRet = CLog::Init("logon.log");

	/***************************************************  ������������� *************************************************/
	//�������
	if ((m_TimerEngine.GetInterface()==NULL)&&(m_TimerEngine.CreateInstance()==false)) return 1;
	if ((m_AttemperEngine.GetInterface()==NULL)&&(m_AttemperEngine.CreateInstance()==false)) return 2;
	if ((m_DataBaseEngine.GetInterface()==NULL)&&(m_DataBaseEngine.CreateInstance()==false)) return 3;
	if ((m_TCPNetworkEngine.GetInterface()==NULL)&&(m_TCPNetworkEngine.CreateInstance()==false)) return 4;
	if ((m_TCPSocketService.GetInterface()==NULL)&&(m_TCPSocketService.CreateInstance()==false)) return 5;

	//����ӿ�
	IUnknownEx * pIAttemperEngine=m_AttemperEngine.GetInterface();
	IUnknownEx * pITCPNetworkEngine=m_TCPNetworkEngine.GetInterface();
	IUnknownEx * pIAttemperEngineSink=QUERY_OBJECT_INTERFACE(m_AttemperEngineSink,IUnknownEx);

	//����AttemperEngine�ص�
	if (m_AttemperEngine->SetAttemperEngineSink(pIAttemperEngineSink)==false) return 6;

	//���������ûص�ΪAttemperEngine
	if (m_TimerEngine->SetTimerEngineEvent(pIAttemperEngine)==false) return 7;
	if (m_TCPNetworkEngine->SetTCPNetworkEngineEvent(pIAttemperEngine)==false) return 8;
	if (m_TCPSocketService->SetTCPSocketEvent(pIAttemperEngine)==false) return 9;


	//��������
	IUnknownEx * pIDataBaseEngineSink[CountArray(m_DataBaseEngineSink)];
	for (WORD i=0;i<CountArray(pIDataBaseEngineSink);i++) pIDataBaseEngineSink[i]=QUERY_OBJECT_INTERFACE(m_DataBaseEngineSink[i],IUnknownEx);

	//TODONOW ����
	if (m_DataBaseEngine->SetDataBaseEngineSink(pIDataBaseEngineSink,CountArray(pIDataBaseEngineSink))==false) return 10;

	//���ݿ�ص�
	for (INT i=0;i<CountArray(m_DataBaseEngineSink);i++)
    {
		m_DataBaseEngineSink[i].m_pIDataBaseEngineEvent=QUERY_OBJECT_PTR_INTERFACE(pIAttemperEngine,IDataBaseEngineEvent);
	}

	//AttemperEngine�ص�
	m_AttemperEngineSink.m_pITimerEngine=m_TimerEngine.GetInterface();
	m_AttemperEngineSink.m_pIDataBaseEngine=m_DataBaseEngine.GetInterface();
	m_AttemperEngineSink.m_pITCPNetworkEngine=m_TCPNetworkEngine.GetInterface();
	m_AttemperEngineSink.m_pITCPSocketService=m_TCPSocketService.GetInterface();

	/***************************************************  ����������Ϣ *************************************************/
	//Э������Э����������
	if (m_TCPSocketService->SetServiceID(NETWORK_CORRESPOND)==false) return 11;
	if (m_TCPSocketService->SetEncrypt(TRUE)==false) return 12;

	//��������
	WORD wMaxConnect=MAX_CONTENT;
	WORD wServicePort=PORT_LOGON;
	if (m_TCPNetworkEngine->SetServiceParameter(wServicePort,wMaxConnect, TRUE)==false) return 13;

	return 0;
}

//��������
bool CServiceUnits::StartService()
{
	//�ں˰汾�ж�
	/*
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
		CLog::Log(log_error, "CServiceUnits::StartKernelService  %d", iRet);
		ConcludeService();
		return false;
	}

	//����Э��
	SendControlPacket(CT_CONNECT_CORRESPOND,NULL,0);

	//��ȡ�б�
	//SendControlPacket(CT_LOAD_DB_GAME_LIST,NULL,0);

	return true;
}

//ֹͣ����
bool CServiceUnits::ConcludeService()
{
	//ֹͣ����
	if (m_TimerEngine.GetInterface()!=NULL) m_TimerEngine->ConcludeService();
	if (m_AttemperEngine.GetInterface()!=NULL) m_AttemperEngine->ConcludeService();
	if (m_DataBaseEngine.GetInterface()!=NULL) m_DataBaseEngine->ConcludeService();
	if (m_TCPNetworkEngine.GetInterface()!=NULL) m_TCPNetworkEngine->ConcludeService();
	if (m_TCPSocketService.GetInterface()!=NULL) m_TCPSocketService->ConcludeService();
	
	return true;
}

//�����ں�
int CServiceUnits::StartKernelService()
{
	//ʱ������
	if (m_TimerEngine->StartService()==false)
	{
		return 1;
	}
	
	//��������
	if (m_AttemperEngine->StartService()==false)
	{
		return 2;
	}

	//Э������
	if (m_TCPSocketService->StartService()==false)
	{
		return 3;
	}
	
	/*
	//��������
	if (m_DataBaseEngine->StartService()==false)
	{
		return 4;
	}
	*/

	return 0;
}

//��������
int CServiceUnits::StartNetworkService()
{
	//��������
	if (m_TCPNetworkEngine->StartService()==false)
	{
		return 1;
	}

	return 0;
}

//Ͷ������
bool CServiceUnits::PostControlRequest(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	//״̬�ж�
	if (IsWindow(m_hWnd)==FALSE) return false;

	//�������
	CWHDataLocker DataLocker(m_CriticalSection);
	if (m_DataQueue.InsertData(wIdentifier,pData,wDataSize)==false) return false;

	//������Ϣ
	PostMessage(WM_UICONTROL_REQUEST,wIdentifier,wDataSize);

	return true;
}

//���Ϳ���
bool CServiceUnits::SendControlPacket(WORD wControlID, VOID * pData, WORD wDataSize)
{
	//״̬Ч��
	if (m_AttemperEngine.GetInterface()==NULL) return false;

	//���Ϳ���
	m_AttemperEngine->OnEventControl(wControlID,pData,wDataSize);

	return true;
}

//������Ϣ
LRESULT CServiceUnits::OnUIControlRequest(WPARAM wParam, LPARAM lParam)
{
	//��������
	tagDataHead DataHead;
	BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];

	//��ȡ����
	CWHDataLocker DataLocker(m_CriticalSection);
	if (m_DataQueue.DistillData(DataHead,cbBuffer,sizeof(cbBuffer))==false)
	{
		return NULL;
	}

	//���ݴ���
	switch (DataHead.wIdentifier)
	{
	case UI_LOAD_DB_LIST_RESULT:	//�б���
		{
			//Ч����Ϣ
			if (DataHead.wDataSize!=sizeof(CP_ControlResult)) return 0;

			//��������
			CP_ControlResult * pControlResult=(CP_ControlResult *)cbBuffer;

			//ʧ�ܴ���
			if (pControlResult->cbSuccess==ER_FAILURE)
			{
				ConcludeService();
				return 0;
			}

			return 0;
		}
	case UI_CORRESPOND_RESULT:		//Э�����
		{
			//Ч����Ϣ
			if (DataHead.wDataSize!=sizeof(CP_ControlResult)) return 0;

			//��������
			CP_ControlResult * pControlResult=(CP_ControlResult *)cbBuffer;
			//ʧ�ܴ���
			if (pControlResult->cbSuccess==ER_FAILURE)
			{
				ConcludeService();
				return 0;
			}

			//�ɹ�����
			if (pControlResult->cbSuccess==ER_SUCCESS)
			{
				//��������
				if (StartNetworkService()==false)
				{
					ConcludeService();
					return 0;
				}
			}

			return 0;
		}
	}

	return 0;
}