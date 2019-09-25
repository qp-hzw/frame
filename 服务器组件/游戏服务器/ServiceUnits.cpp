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
	ASSERT(g_pServiceUnits==NULL);
	if (g_pServiceUnits==NULL) g_pServiceUnits=this;

	//�������
	ZeroMemory(&m_GameParameter,sizeof(m_GameParameter));
	ZeroMemory(&m_GameServiceAttrib,sizeof(m_GameServiceAttrib));
	ZeroMemory(&m_GameServiceOption,sizeof(m_GameServiceOption));



	return;
}

//��������
CServiceUnits::~CServiceUnits()
{
}


//Ͷ������
bool CServiceUnits::PostControlRequest(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	//״̬�ж�
	ASSERT(IsWindow(m_hWnd));
	if (IsWindow(m_hWnd)==FALSE) return false;

	//�������
	CWHDataLocker DataLocker(m_CriticalSection);
	if (m_DataQueue.InsertData(wIdentifier,pData,wDataSize)==false) return false;

	//������Ϣ
	PostMessage(WM_UICONTROL_REQUEST,wIdentifier,wDataSize);

	return true;
}

//��������
bool CServiceUnits::StartService()
{
	bool bResult = true;


		//��������
		if (m_hWnd==NULL)
		{
			CRect rcCreate(0,0,0,0);
			Create(NULL,NULL,WS_CHILD,rcCreate,AfxGetMainWnd(),100);
		}

		//����ģ��
		if (CreateServiceDLL()==false)
		{
			bResult = false;
		}
		//��������
		if (RectifyServiceParameter()==false)
		{
			bResult = false;
		}

		//���÷���
		if (InitializeService()==false)
		{
			bResult = false;
		}

		/*
		//�ں˰汾�ж�
		CWHIniData InitData;
		DWORD realKernel = InitData.Get_Code_Kernel_Version();
		DWORD frameKernel = Get_Kernel_Version();
		if(Compare_Kernek_Framework(realKernel, frameKernel) != 0)
		{		
			bResult = false;
		}
		*/

		//�����ں�
		if (StartKernelService()==false)
		{
			bResult = false;
		}

	if(!bResult)
	{
		ConcludeService();
		return false;
	}

	//��������
	m_AttemperEngine->OnEventControl(CT_LOAD_SERVICE_CONFIG,NULL,0);

	//��������
	if (m_GameMatchServiceManager.GetInterface()!=NULL && m_GameMatchServiceManager->StartService()==false) return false;

	return true;
}

//ֹͣ����
bool CServiceUnits::ConcludeService()
{

		//�ں����
	if (m_TimerEngine.GetInterface()!=NULL) m_TimerEngine->ConcludeService();
	if (m_AttemperEngine.GetInterface()!=NULL) m_AttemperEngine->ConcludeService();
	if (m_TCPSocketService.GetInterface()!=NULL) m_TCPSocketService->ConcludeService();
	if (m_TCPNetworkEngine.GetInterface()!=NULL) m_TCPNetworkEngine->ConcludeService();

	//��������
	if (m_RecordDataBaseEngine.GetInterface()!=NULL) m_RecordDataBaseEngine->ConcludeService();
	if (m_KernelDataBaseEngine.GetInterface()!=NULL) m_KernelDataBaseEngine->ConcludeService();
	m_DBCorrespondManager.ConcludeService();

	//ע�����
	if (m_GameServiceManager.GetInterface()!=NULL) m_GameServiceManager.CloseInstance();
	if (m_GameMatchServiceManager.GetInterface()!=NULL)m_GameMatchServiceManager.CloseInstance();

	return true;
}


//��Ϸ����
bool CServiceUnits::CollocateService(LPCTSTR pszGameModule, tagGameServiceOption & GameServiceOption)
{
	//����ģ��(������Ԫ��ֵ)
	m_GameServiceOption = GameServiceOption;

	m_GameServiceManager.SetModuleCreateInfo(pszGameModule,GAME_SERVICE_CREATE_NAME);

	return true;
}

//����ģ��
bool CServiceUnits::CreateServiceDLL()
{
	//ʱ������
	if ((m_TimerEngine.GetInterface()==NULL)&&(m_TimerEngine.CreateInstance()==false))
	{
		return false;
	}

	//��������
	if ((m_AttemperEngine.GetInterface()==NULL)&&(m_AttemperEngine.CreateInstance()==false))
	{
		return false;
	}

	//�������
	if ((m_TCPSocketService.GetInterface()==NULL)&&(m_TCPSocketService.CreateInstance()==false))
	{
		return false;
	}

	//��������
	if ((m_TCPNetworkEngine.GetInterface()==NULL)&&(m_TCPNetworkEngine.CreateInstance()==false))
	{
		return false;
	}

	//�������
	if ((m_KernelDataBaseEngine.GetInterface()==NULL)&&(m_KernelDataBaseEngine.CreateInstance()==false))
	{
		return false;
	}

	//�������
	if ((m_RecordDataBaseEngine.GetInterface()==NULL)&&(m_RecordDataBaseEngine.CreateInstance()==false))
	{
		return false;
	}

	//��Ϸģ��
	if ((m_GameServiceManager.GetInterface()==NULL)&&(m_GameServiceManager.CreateInstance()==false))
	{
		return false;
	}

	//if ((m_GameServiceOption.wServerType&GAME_GENRE_MATCH)!=0 )
	if(false)
	{
		if ((m_GameMatchServiceManager.GetInterface()==NULL)&&(m_GameMatchServiceManager.CreateInstance()==false))
		{
			return false;
		}
	}

	return true;
}

//�������
bool CServiceUnits::InitializeService()
{
	//���ò���
	m_GameParameter.wMedalRate=1L;
	m_GameParameter.wRevenueRate=1L;

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

	//�ں����
	if (m_TimerEngine->SetTimerEngineEvent(pIAttemperEngine)==false) return false;
	if (m_TCPNetworkEngine->SetTCPNetworkEngineEvent(pIAttemperEngine)==false) return false;

	//Э������
	if (m_TCPSocketService->SetServiceID(NETWORK_CORRESPOND)==false) return false;
	if (m_TCPSocketService->SetTCPSocketEvent(pIAttemperEngine)==false) return false;

	//����Э��
	m_DBCorrespondManager.InitDBCorrespondManager(m_KernelDataBaseEngine.GetInterface());

	//���Ȼص�
	m_AttemperEngineSink.m_pGameParameter = &m_GameParameter;
	m_AttemperEngineSink.m_pGameServiceAttrib = &m_GameServiceAttrib;
	m_AttemperEngineSink.m_pGameServiceOption = &m_GameServiceOption;

	//���Ȼص�
	m_AttemperEngineSink.m_pITimerEngine=m_TimerEngine.GetInterface();
	m_AttemperEngineSink.m_pIAttemperEngine=m_AttemperEngine.GetInterface();
	m_AttemperEngineSink.m_pITCPSocketService=m_TCPSocketService.GetInterface();
	m_AttemperEngineSink.m_pITCPNetworkEngine=m_TCPNetworkEngine.GetInterface();
	m_AttemperEngineSink.m_pIGameServiceManager=m_GameServiceManager.GetInterface();
	m_AttemperEngineSink.m_pIRecordDataBaseEngine=m_RecordDataBaseEngine.GetInterface();
	m_AttemperEngineSink.m_pIKernelDataBaseEngine=m_KernelDataBaseEngine.GetInterface();
	m_AttemperEngineSink.m_pIGameMatchServiceManager=m_GameMatchServiceManager.GetInterface();
	m_AttemperEngineSink.m_pIDBCorrespondManager=(IDBCorrespondManager*)m_DBCorrespondManager.QueryInterface(IID_IDBCorrespondManager,VER_IDBCorrespondManager);

	//���ݻص�
	for (INT i=0;i<CountArray(m_RecordDataBaseSink);i++)
	{
		m_RecordDataBaseSink[i].m_pGameParameter=&m_GameParameter;
		m_RecordDataBaseSink[i].m_pGameServiceAttrib=&m_GameServiceAttrib;
		m_RecordDataBaseSink[i].m_pGameServiceOption=&m_GameServiceOption;
		m_RecordDataBaseSink[i].m_pIGameServiceManager=m_GameServiceManager.GetInterface();
		m_RecordDataBaseSink[i].m_pIDataBaseEngineEvent=QUERY_OBJECT_PTR_INTERFACE(pIAttemperEngine,IDataBaseEngineEvent);
	}

	//���ݻص�
	for (INT i=0;i<CountArray(m_KernelDataBaseSink);i++)
	{
		m_KernelDataBaseSink[i].m_pGameParameter=&m_GameParameter;
		m_KernelDataBaseSink[i].m_pGameServiceAttrib=&m_GameServiceAttrib;
		m_KernelDataBaseSink[i].m_pGameServiceOption=&m_GameServiceOption;
		m_KernelDataBaseSink[i].m_pIGameServiceManager=m_GameServiceManager.GetInterface();
		m_KernelDataBaseSink[i].m_pIDataBaseEngineEvent=QUERY_OBJECT_PTR_INTERFACE(pIAttemperEngine,IDataBaseEngineEvent);
		m_KernelDataBaseSink[i].m_pIDBCorrespondManager=(IDBCorrespondManager*)m_DBCorrespondManager.QueryInterface(IID_IDBCorrespondManager,VER_IDBCorrespondManager);
	}

	//��������
	m_TCPNetworkEngine->SetServiceParameter(m_GameServiceOption.wGameServerPort, MAX_TABLE);

	//���ǵ���Ϸ�������ڲ���֪��ServerID, ���ֻ�ܽ�log��������Ϣд������
	TCHAR szIniFile[MAX_PATH]=TEXT("");
	_sntprintf_s(szIniFile,CountArray(szIniFile),TEXT("GameServer-%d.log"), m_GameServiceOption.dwServerID);

	return true;
}

//�����ں�
bool CServiceUnits::StartKernelService()
{
	//ʱ������
	if (m_TimerEngine->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	if (m_AttemperEngine->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//Э������
	if (m_TCPSocketService->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	if (m_RecordDataBaseEngine->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	if (m_KernelDataBaseEngine->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//����Э��
	if (m_DBCorrespondManager.StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//��������
bool CServiceUnits::StartNetworkService()
{
	//��������
	if (m_TCPNetworkEngine->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//��������
bool CServiceUnits::RectifyServiceParameter()
{
	//��ȡ����
	m_GameServiceManager->GetServiceAttrib(m_GameServiceAttrib);

	//��������
	if (m_GameServiceManager->RectifyParameter(m_GameServiceOption)==false)
	{
		return false;
	}

	//��������
	if (m_GameMatchServiceManager.GetInterface()!=NULL)
	{
		m_GameMatchServiceManager->RectifyServiceOption(&m_GameServiceOption,&m_GameServiceAttrib);
	}

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
		ASSERT(FALSE);
		return NULL;
	}

	//���ݴ���
	switch (DataHead.wIdentifier)
	{
	case UI_CORRESPOND_RESULT:		//Э���ɹ�
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
			}

			return 0;
		}
	case UI_SERVICE_CONFIG_RESULT:	//���ý��
		{
			//Ч����Ϣ
			ASSERT(DataHead.wDataSize==sizeof(CP_ControlResult));
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

				//����Э��
				m_AttemperEngine->OnEventControl(CT_CONNECT_CORRESPOND,NULL,0);
			}

			return 0;
		}
	case UI_SERVICE_STOP_SERVICE_FINISH:
		{
			//Ч����Ϣ
			ASSERT(DataHead.wDataSize==sizeof(CP_ControlResult));
			if (DataHead.wDataSize!=sizeof(CP_ControlResult)) return 0;

			//��������
			CP_ControlResult * pControlResult=(CP_ControlResult *)cbBuffer;

			//ʧ�ܴ���
			if (pControlResult->cbSuccess==ER_FAILURE)
			{
			}

			ConcludeService();
		}
	}

	return 0;
}


//////////////////////////////////////////////////////////////////////////////////
