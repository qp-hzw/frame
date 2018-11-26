#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "AttemperEngineSink.h"
#include <vector>
#include <algorithm>
#include <iostream>

#include "../../ȫ�ֶ���/Define.h"

//////////////////////////////////////////////////////////////////////////////////
//ʱ���ʶ

#define IDI_LOAD_ANDROID_USER			(IDI_MAIN_MODULE_START+1)			//������Ϣ
#define IDI_REPORT_SERVER_INFO			(IDI_MAIN_MODULE_START+2)			//��������
#define IDI_CONNECT_CORRESPOND			(IDI_MAIN_MODULE_START+3)			//����ʱ��
#define IDI_GAME_SERVICE_PULSE			(IDI_MAIN_MODULE_START+4)			//��������
#define IDI_DISTRIBUTE_ANDROID			(IDI_MAIN_MODULE_START+5)			//�������
#define IDI_DBCORRESPOND_NOTIFY			(IDI_MAIN_MODULE_START+6)			//����֪ͨ

#define IDI_LOAD_SENSITIVE_WORD			(IDI_MAIN_MODULE_START+9)			//�������д�

#define IDI_WRITE_STOCK_SCORE			(IDI_MAIN_MODULE_START+14)			//д����¼
#define IDI_WAIT_SERVICE_STOP			(IDI_MAIN_MODULE_START+15)			//�ӳ�ֹͣ

#define TIME_CONNECT				30									//����ʱ��
//////////////////////////////////////////////////////////////////////////////////
//ʱ�䶨�� ��

#define TIME_LOAD_ANDROID_USER				600L								//���ػ���
#define TIME_DISTRIBUTE_ANDROID				15L									//�����û�
#define TIME_REPORT_SERVER_INFO				30L									//�ϱ�ʱ��
#define TIME_DBCORRESPOND_NOTIFY			3L									//����֪ͨʱ��
#define TIME_LOAD_SYSTEM_MESSAGE			3600L								//ϵͳ��Ϣʱ��
#define TIME_SEND_SYSTEM_MESSAGE			10L									//ϵͳ��Ϣʱ��
#define TIME_CONTROL_ROOM_VALUE_SAVE		120000L								//�������ֵʱ��yang
#define TIME_READ_ROOM_CONFIG				2000L								//��ȡ��������yang
#define TIME_WRITE_STOCK_SCORE				120000L								//д����¼
#define TIME_WAIT_SERVICE_STOP				5000L								//�ӳ�ֹͣ
#define TIME_CONNECT_CORRESPOND				30*1000L									//����ʱ��

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CAttemperEngineSink::CAttemperEngineSink()
{
	srand((unsigned)time(NULL));

	//״̬����
	m_bCollectUser=false;
	m_bNeekCorrespond=true;
	m_bStopping = false;

	//������
	m_pNormalParameter=NULL;
	m_pAndroidParameter=NULL;

	//״̬����
	m_pGameParameter=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//�������
	m_pITimerEngine=NULL;
	m_pIAttemperEngine=NULL;
	m_pITCPSocketService=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pIGameServiceManager=NULL;
	m_WaitDistributeList.RemoveAll();

	//��������
	m_pIRecordDataBaseEngine=NULL;
	m_pIKernelDataBaseEngine=NULL;
	m_pIDBCorrespondManager=NULL;

	//��������
	m_pIGameMatchServiceManager=NULL;

	return;
}

//��������
CAttemperEngineSink::~CAttemperEngineSink()
{
	//ɾ������
	SafeDeleteArray(m_pNormalParameter);
	SafeDeleteArray(m_pAndroidParameter);

	//ɾ������
	for (INT_PTR i=0;i<m_TableFrameArray.GetCount();i++)
	{
		SafeRelease(m_TableFrameArray[i]);
	}

	//��������
	m_WaitDistributeList.RemoveAll();

	return;
}

//�ӿڲ�ѯ
VOID * CAttemperEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IMainServiceFrame,Guid,dwQueryVer);
	QUERYINTERFACE(IAttemperEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE(IServerUserItemSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAttemperEngineSink,Guid,dwQueryVer);
	return NULL;
}

//�����¼�
bool CAttemperEngineSink::OnAttemperEngineStart(IUnknownEx * pIUnknownEx)
{
	//����Ϣ
	m_pAndroidParameter=new tagBindParameter[MAX_ANDROID + MAX_VIRTUAL];
	ZeroMemory(m_pAndroidParameter,sizeof(tagBindParameter)*(MAX_ANDROID +MAX_VIRTUAL));

	//����Ϣ
	m_pNormalParameter=new tagBindParameter[MAX_TABLE];
	ZeroMemory(m_pNormalParameter,sizeof(tagBindParameter)*MAX_TABLE);

	//m_GameControlManager.Reset();

	//���û���
	if (InitAndroidUser()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	if (InitTableFrameArray()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//���ýӿ�
	if (m_ServerUserManager.SetServerUserItemSink(QUERY_ME_INTERFACE(IServerUserItemSink))==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	if (m_AndroidUserManager.StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//����ʱ��
	m_pITimerEngine->SetTimer(IDI_GAME_SERVICE_PULSE,1000L,TIMES_INFINITY,NULL);

#ifdef _DEBUG
	m_pITimerEngine->SetTimer(IDI_LOAD_ANDROID_USER,10000L,TIMES_INFINITY,NULL);
#else
	m_pITimerEngine->SetTimer(IDI_LOAD_ANDROID_USER,TIME_LOAD_ANDROID_USER*1000L,TIMES_INFINITY,NULL);
#endif

	m_pITimerEngine->SetTimer(IDI_DISTRIBUTE_ANDROID,TIME_DISTRIBUTE_ANDROID*1000L,TIMES_INFINITY,NULL);

	m_pITimerEngine->SetTimer(IDI_DBCORRESPOND_NOTIFY,TIME_DBCORRESPOND_NOTIFY*1000L,TIMES_INFINITY,NULL);
	return true;
}

//ֹͣ�¼�
bool CAttemperEngineSink::OnAttemperEngineConclude(IUnknownEx * pIUnknownEx)
{
	//״̬����
	m_bCollectUser=false;
	m_bNeekCorrespond=true;
	m_bStopping=false;

	//������Ϣ
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//�������
	m_pITimerEngine=NULL;
	m_pITCPSocketService=NULL;
	m_pITCPNetworkEngine=NULL;

	//��������
	m_pIRecordDataBaseEngine=NULL;
	m_pIKernelDataBaseEngine=NULL;

	//������
	SafeDeleteArray(m_pNormalParameter);
	SafeDeleteArray(m_pAndroidParameter);

	//ɾ������
	for (INT_PTR i=0;i<m_TableFrameArray.GetCount();i++)
	{
		SafeRelease(m_TableFrameArray[i]);
	}

	//ɾ���û�
	m_TableFrameArray.RemoveAll();
	m_ServerUserManager.DeleteUserItem();
	m_WaitDistributeList.RemoveAll();

	//ֹͣ����
	m_AndroidUserManager.ConcludeService();

	if(m_pIGameMatchServiceManager!=NULL)
		m_pIGameMatchServiceManager->StopService();

	return true;
}

//�����¼�
bool CAttemperEngineSink::OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	switch (wIdentifier)
	{
	case CT_CONNECT_CORRESPOND:		//����Э��
		{
			//��������
			m_pITCPSocketService->Connect(_CPD_SERVER_ADDR, PORT_CENTER);

			//������ʾ
			TCHAR szString[512]=TEXT("");
			_sntprintf_s(szString,CountArray(szString),TEXT("��������Э��������"));

			//��ʾ��Ϣ
			CTraceService::TraceString(szString,TraceLevel_Normal);

			return true;
		}
	case CT_LOAD_SERVICE_CONFIG:	//��������
		{
			//���ػ���
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_ANDROID_USER,0L,NULL,0L);

			//��m_GameProgress ��ʼ��
			m_GameProgress = 0;

			//���ض����������� && ���ݿ��ÿն�Ӧ�����ӵ���������
			STR_DBR_GR_LOAD_OFFLINE  dbr;
			dbr.byMystery = _MYSTERY;
			dbr.dwGameID = (m_pGameServiceOption->dwServerID) & 0xFFFFFF00;
			m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_OFFLINE,0L,&dbr,sizeof(dbr));

			//�¼�֪ͨ -- ��UI_SERVICE_CONFIG_RESULT��case�������Ѿ���ServiceStatus���˴���, �������û������
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_SUCCESS;
			SendUIControlPacket(UI_SERVICE_CONFIG_RESULT,&ControlResult,sizeof(ControlResult));
			return true;
		}
	case CT_TRY_TO_STOP_SERVICE:	//ֹͣ����
		{
			StopService();
			break;
		}
	}

	return false;
}


//ʱ���¼�
bool CAttemperEngineSink::OnEventTimer(DWORD dwTimerID, WPARAM wBindParam)
{
	//����ʱ��
	if ((dwTimerID>=IDI_MAIN_MODULE_START)&&(dwTimerID<=IDI_MAIN_MODULE_FINISH))
	{
		//ʱ�䴦��
		switch (dwTimerID)
		{
		case IDI_LOAD_ANDROID_USER:		//���ػ���
			{
				//���ػ���
				m_pIDBCorrespondManager->PostDataBaseRequest(0L,DBR_GR_LOAD_ANDROID_USER,0L,NULL,0L);

				return true;
			}
		case IDI_REPORT_SERVER_INFO:	//��������
			{
				//��������
				STR_CPR_GP_LIST_GAME_ONLINE ServerOnLine;
				ZeroMemory(&ServerOnLine,sizeof(ServerOnLine));

				//���ñ���
				ServerOnLine.dwOnLineCount=m_ServerUserManager.GetUserItemCount();

				//��������
				m_pITCPSocketService->SendData(MDM_CPD_LIST,CPR_GP_LIST_GAME_ONLINE,&ServerOnLine,sizeof(ServerOnLine));
				return true;
			}
		case IDI_CONNECT_CORRESPOND:	//����Э��
			{
				//�������� TODONOWW
                m_pITCPSocketService->Connect(_CPD_SERVER_ADDR, PORT_CENTER);

				//������ʾ
				TCHAR szString[512]=TEXT("");
				_sntprintf_s(szString,CountArray(szString),TEXT("��������Э��������"));

				//��ʾ��Ϣ
				CTraceService::TraceString(szString,TraceLevel_Normal);

				return true;
			}
		case IDI_GAME_SERVICE_PULSE:	//����ά��
			{
				return true;
			}
		case IDI_DISTRIBUTE_ANDROID:	//�������
			{
				DistributeAndroid();
				return true;
			}
		case IDI_DBCORRESPOND_NOTIFY: //���涨ʱ����
			{
				if(m_pIDBCorrespondManager) m_pIDBCorrespondManager->OnTimerNotify();
				return true;
			}

		case IDI_LOAD_SENSITIVE_WORD:	//�������д�
			{
				//Ͷ������
				//m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_SENSITIVE_WORDS,0,NULL,0);				
				return true;
			}
		case IDI_WAIT_SERVICE_STOP:
			{
				SendServiceStopped();
				return true;
			}

		}
	}

	//����ʱ��
	if ((dwTimerID>=IDI_REBOT_MODULE_START)&&(dwTimerID<=IDI_REBOT_MODULE_FINISH))
	{
		//ʱ�䴦��
		m_AndroidUserManager.OnEventTimerPulse(dwTimerID,wBindParam);

		return true;
	}

	//������ʱ��
	if((dwTimerID>=IDI_MATCH_MODULE_START)&&(dwTimerID<IDI_MATCH_MODULE_FINISH))
	{
		if(m_pIGameMatchServiceManager!=NULL) m_pIGameMatchServiceManager->OnEventTimer(dwTimerID,wBindParam);
		return true;
	}

	//����ʱ��
	if ((dwTimerID>=IDI_TABLE_MODULE_START)&&(dwTimerID<=IDI_TABLE_MODULE_FINISH))
	{
		//���Ӻ���
		DWORD dwTableTimerID=dwTimerID-IDI_TABLE_MODULE_START;
		WORD wTableID=(WORD)(dwTableTimerID/TIME_TABLE_MODULE_RANGE);

		//ʱ��Ч��
		if (wTableID>=(WORD)m_TableFrameArray.GetCount()) 
		{
			ASSERT(FALSE);
			return false;
		}

		//ʱ��֪ͨ
		CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
		return pTableFrame->OnEventTimer(dwTableTimerID%TIME_TABLE_MODULE_RANGE,wBindParam);
	}

	return false;
}

//���ݿ��¼�
bool CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
	case DBO_CG_LOGON_USERID:			//ID��¼����
		{
			return On_CMD_GC_Logon_UserID(dwContextID, pData, wDataSize);
		}
	case DBO_CG_USER_CREATE_GROUP_ROOM:	//��������Ȧ���䷵��
		{
			return On_CMD_GC_User_CreateGroupRoom(dwContextID, pData, wDataSize);
		}
	case DBO_CG_USER_JOIN_GROUP_ROOM:	//��������Ȧ���䷵��
		{
			return On_CMD_GC_User_JoinGroupRoom(dwContextID, pData, wDataSize);
		}
	case DBO_SG_MODIFY_USER_TREASURE:	//�޸��û��Ƹ���Ϣ����
		{
			return On_CMD_GC_User_ModifyUserTreasure(dwContextID, pData, wDataSize);
		}
	case DBR_GR_GAME_ANDROID_INFO:		//������Ϣ
		{
			return OnDBGameAndroidInfo(dwContextID,pData,wDataSize);
		}

	case DBO_GR_LOAD_OFFLINE:		//���ض�����������
		{
			return On_DBO_GR_LOAD_OFFLINE(dwContextID,pData,wDataSize);
		}

	case DBO_GR_ROOMLEVELMODIFY:	//�����Ѷȵ���
		{
			return OnDBRoomLevelModify(dwContextID,pData,wDataSize);
		}
	case DBO_GR_ROOMCONTROLVALMODIFY:
		{
			return OnDBRoomControlValModify(dwContextID,pData,wDataSize);
		}
	case DBO_GR_LOTTERY_RESULT:
		{
			return OnDBLotteryResult(dwContextID,pData,wDataSize);
		}
	case DBO_GC_CLUB_CREATE_TABLE:	//club����Ȧ��������
		{
			return On_CMD_CG_CLUB_CREATE_TABLE(dwContextID,pData,wDataSize);
		}
	case DBO_GC_USER_JOIN_TABLE_NO_PASS://�������� ����Ҫ����
		{
			return On_CMD_GC_USER_JOIN_TABLE_NO_PASS(dwContextID,pData,wDataSize);
		}
	case DBO_GC_CLUB_UPDATE_TABLE_INFO:// ����������Ϣ
		{
			if(sizeof(STR_CMD_LC_CLUB_TABLE_LIST) != wDataSize) return false;
			m_pITCPSocketService->SendData(MDM_TRANSFER, CPR_GP_CLUB_TABLE_INFO, pData, wDataSize);
			return true;;
		}
	case DBO_GC_CLUB_UPDATE_PLAYER_INFO://���������Ϣ
		{
			if(sizeof(STR_CMD_LC_CLUB_TABLE_USER_LIST) != wDataSize) return false;
			m_pITCPSocketService->SendData(MDM_TRANSFER, CPR_GP_CLUB_PLAYER_INFO, pData, wDataSize);
			return true;
		}
	case DBO_GC_JOIN_TABLE://�������� ���ݿ�У��
		{
			return On_CMD_GC_JOIN_TABLE(dwContextID,pData,wDataSize);
		}
	case DBO_GC_USER_JOIN_TABLE_HALL_GOLD: //�������� -- ��Ҵ�������
		{
			return On_CMD_GC_USER_JOIN_TABLE_HALL_GOLD(dwContextID,pData,wDataSize);
		}
	}


	if(wRequestID>=DBO_GR_MATCH_EVENT_START && wRequestID<=DBO_GR_MATCH_EVENT_END)
	{
		//����Ч��
		if(m_pIGameMatchServiceManager==NULL) return false;

		tagBindParameter * pBindParameter=GetBindParameter(LOWORD(dwContextID));
		IServerUserItem *pIServerUserItem=pBindParameter!=NULL?pBindParameter->pIServerUserItem:NULL;

		//�����ж�
		if ((pBindParameter->pIServerUserItem==NULL)||(pBindParameter->dwSocketID!=dwContextID))
		{
			//�������
			ASSERT(FALSE);
			return true;
		}

		return m_pIGameMatchServiceManager->OnEventDataBase(wRequestID,pIServerUserItem,pData,wDataSize);
	}

	return false;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason)
{
	//Э������
	if (wServiceID==NETWORK_CORRESPOND)
	{
		//���ñ���
		m_bCollectUser=false;

		//ɾ��ʱ��
		m_pITimerEngine->KillTimer(IDI_REPORT_SERVER_INFO);

		//�����ж�
		if (m_bNeekCorrespond==true)
		{
			//������ʾ
			TCHAR szDescribe[128]=TEXT("");
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("��Э�������������ӹر��ˣ�%ld �����������"),TIME_CONNECT);

			//��ʾ��Ϣ
			CTraceService::TraceString(szDescribe,TraceLevel_Warning);

			//����ʱ��
			ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_CORRESPOND, TIME_CONNECT_CORRESPOND, 1, 0);
		}

		return true;
	}

	return false;
}

//�����¼�
bool CAttemperEngineSink::OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode)
{
	//Э������
	if (wServiceID==NETWORK_CORRESPOND)
	{
		//�����ж�
		if (nErrorCode!=0)
		{
			//������ʾ
			TCHAR szDescribe[128]=TEXT("");
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("Э������������ʧ�� [ %ld ]��%ld �����������"),
				nErrorCode,TIME_CONNECT_CORRESPOND);

			//��ʾ��Ϣ
			CTraceService::TraceString(szDescribe,TraceLevel_Warning);

			//����ʱ��
			m_pITimerEngine->SetTimer(IDI_CONNECT_CORRESPOND, TIME_CONNECT_CORRESPOND, 1, 0);

			return false;
		}

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("���ڷ�����Ϸ����ע����Ϣ..."),TraceLevel_Normal);

		//��������
		STR_CPR_GP_REGISTER_GAME RegisterServer;
		ZeroMemory(&RegisterServer,sizeof(RegisterServer));

		//��������
		RegisterServer.dwServerID=m_pGameServiceOption->dwServerID;
		lstrcpyn(RegisterServer.szServerName, m_pGameServiceOption->szServerName, CountArray(RegisterServer.szServerName));
		lstrcpyn(RegisterServer.szGameServerAddr, m_pGameServiceOption->szGameServerAddr,CountArray(RegisterServer.szGameServerAddr));
		RegisterServer.wGameServerPort=m_pGameServiceOption->wGameServerPort;
		RegisterServer.dwSubGameVersion = m_pGameServiceAttrib->dwSubGameVersion;

		//��������
		m_pITCPSocketService->SendData(MDM_REGISTER,CPR_GP_REGISTER_GAME,&RegisterServer,sizeof(RegisterServer));

		//����ʱ��
		m_pITimerEngine->SetTimer(IDI_REPORT_SERVER_INFO,TIME_REPORT_SERVER_INFO*1000L,TIMES_INFINITY,0);

		return true;
	}

	return true;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize)
{
	//Э������
	if (wServiceID==NETWORK_CORRESPOND)
	{
		switch (Command.wMainCmdID)
		{
		case MDM_REGISTER:		//ע�����
			{
				return OnTCPSocketMainRegister(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_TRANSFER:		//��ת����
			{
				return OnTCPSocketMainTransfer(Command.wSubCmdID,pData,wDataSize);	
			}
		case MDM_CS_USER_COLLECT:	//�û�����
			{
				return OnTCPSocketMainUserCollect(Command.wSubCmdID,pData,wDataSize);
			}
		}
	}

	//�������
	ASSERT(FALSE);

	return true;
}

//Ӧ���¼�
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID)
{
	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//���ñ���
	if (pBindParameter!=NULL)
	{
		pBindParameter->dwSocketID=dwSocketID;
		pBindParameter->dwClientAddr=dwClientAddr;
		pBindParameter->dwActiveTime=(DWORD)time(NULL);
		return true;
	}

	//�������
	return false;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID)
{
	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//��ȡ�û�
	IServerUserItem * pIServerUserItem=pBindParameter->pIServerUserItem;

	//Socket���ӹر� ���ݿ⴦��
	if((pIServerUserItem != NULL) && (pIServerUserItem->GetUserID() != 0))
	{
		//Club���ֲ� ��ҵ��ߵĴ���
		if(pIServerUserItem->GetTableID() != 0) //����û���������
		{
			ClubPlayerOffline(pIServerUserItem->GetTableID(), \
				pIServerUserItem->GetUserID(), static_cast<BYTE>(pIServerUserItem->GetChairID()), 1);
		}
	}

	//�û�����
	if (pIServerUserItem!=NULL)
	{
		bool bIsExsit = false; //�Ƿ����ڶ���

		//��������
		WORD wTableID=pIServerUserItem->GetTableID();

		//���ߴ���
		if (wTableID!=INVALID_TABLE)
		{
			//�����
			pIServerUserItem->DetachBindStatus();

			//�Ƿ���Ҫ�����������
			bool bRet = m_TableFrameArray[wTableID]->OnEventUserOffLine(pIServerUserItem);

			if(bRet &&  (m_GameProgress == 3)) //ֻ��3�ڹ��̲Ż��ж�������
			{
				DWORD dwServerID = (m_pGameServiceOption->dwServerID) &0xFFFFFF00;

				//��Ϸ��ֱ���޸� TODONOW ��ʱ����
				pIServerUserItem->SetOfflineGameID(dwServerID);

				//���͸�Э����, ��Э����֪ͨ��¼��
				tagOfflineUser data;
				data.dwUserID = pIServerUserItem->GetUserID();
				data.dwServerID = dwServerID;
				data.byMask = 1; //��ʾ���Ӷ����û�
				m_pITCPSocketService->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_USER_OFFLINE,&data,sizeof(tagOfflineUser));

				bIsExsit = true;
			}
		}
		else
		{
			pIServerUserItem->SetUserStatus(US_NULL,INVALID_TABLE,INVALID_CHAIR);
		}

		//���״̬�仯
		/* ������ ��Ϸ�� socket�ϵ���ʱ��
		**	1. �ж���Ϸ�б���  �Ƿ����
		**	�������, ���¼���϶������ˣ�Ӧ������Ϊ��������
		**	�����, ������Ϊ��Ϸ����
		*/
		DBR_GP_UserQuitInfo quitInfo;
		quitInfo.dwUserID = pIServerUserItem->GetUserID();
		if(bIsExsit)
		{
			quitInfo.byOnlineMask = 3;
		}
		else
		{
			quitInfo.byOnlineMask = 1;
		}
		m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GP_GAME_USER_STATE,dwSocketID, &quitInfo,sizeof(quitInfo));
	}


	//�����Ϣ
	ZeroMemory(pBindParameter,sizeof(tagBindParameter));

	return false;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
    switch (Command.wMainCmdID)
    {
    case MDM_GR_LOGON:		//��¼����
        {
            return OnTCPNetworkMainLogon(Command.wSubCmdID,pData,wDataSize,dwSocketID);
        }
    case MDM_USER:			//�û�����
        {
            return OnTCPNetworkMainUser(Command.wSubCmdID,pData,wDataSize,dwSocketID);
        }
    case MDM_G_FRAME:		//�������
        {
            return OnTCPNetworkMainFrame(Command.wSubCmdID,pData,wDataSize,dwSocketID);
        }
    case MDM_GF_GAME:		//��Ϸ����
        {
            return OnTCPNetworkMainGame(Command.wSubCmdID,pData,wDataSize,dwSocketID);
        }
    case MDM_GR_MATCH:		//��������
        {
            return OnTCPNetworkMainMatch(Command.wSubCmdID,pData,wDataSize,dwSocketID);
        }
    }

	//��Ϸ���ǳ�����, ��������Ч
	return true;
}

//������Ϣ
bool CAttemperEngineSink::SendRoomMessage(LPCTSTR lpszMessage, WORD wType)
{
	//��������
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//��������
	SystemMessage.wType=wType;
	SystemMessage.wLength=lstrlen(lpszMessage)+1;
	lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

	//��������
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//��������
	m_AndroidUserManager.SendDataToClient(MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
	m_pITCPNetworkEngine->SendDataBatch(MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize,BG_COMPUTER);

	return true;
}

//��Ϸ��Ϣ
bool CAttemperEngineSink::SendGameMessage(LPCTSTR lpszMessage, WORD wType)
{
	//��������
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//��������
	SystemMessage.wType=wType;
	SystemMessage.wLength=lstrlen(lpszMessage)+1;
	lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

	//��������
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//��������
	m_AndroidUserManager.SendDataToClient(MDM_G_FRAME,CMD_GF_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
	m_pITCPNetworkEngine->SendDataBatch(MDM_G_FRAME,CMD_GF_SYSTEM_MESSAGE,&SystemMessage,wSendSize,BG_COMPUTER);

	return true;
}

//������Ϣ
bool CAttemperEngineSink::SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//Ч�����
	if (pIServerUserItem==NULL) return false;

	//��������
	if (pIServerUserItem->GetBindIndex()!=INVALID_WORD)
	{
		//��������
		CMD_CM_SystemMessage SystemMessage;
		ZeroMemory(&SystemMessage,sizeof(SystemMessage));

		//��������
		SystemMessage.wType=wType;
		SystemMessage.wLength=lstrlen(lpszMessage)+1;
		lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

		//��������
		WORD dwUserIndex=pIServerUserItem->GetBindIndex();
		tagBindParameter * pBindParameter=GetBindParameter(dwUserIndex);

		//��������
		WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
		WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

		//��������
		if (pIServerUserItem->IsAndroidUser()==true)
		{
			//�����û�
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_AndroidUserManager.SendDataToClient(pBindParameter->dwSocketID,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);

			//�رմ���
			if ((wType&(SMT_CLOSE_ROOM|SMT_CLOSE_LINK))!=0) m_AndroidUserManager.DeleteAndroidUserItem(pBindParameter->dwSocketID);
		}
		else
		{
			//�����û�
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
		}

		return true;
	}

	return false;
}

//��Ϸ��Ϣ
bool CAttemperEngineSink::SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	if(pIServerUserItem->IsVirtualUser())
		return true;

	//��������
	if ((pIServerUserItem->GetBindIndex()!=INVALID_WORD)&&(pIServerUserItem->IsClientReady()==true))
	{
		//��������
		CMD_CM_SystemMessage SystemMessage;
		ZeroMemory(&SystemMessage,sizeof(SystemMessage));

		//��������
		SystemMessage.wType=wType;
		SystemMessage.wLength=lstrlen(lpszMessage)+1;
		lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

		//��������
		WORD dwUserIndex=pIServerUserItem->GetBindIndex();
		tagBindParameter * pBindParameter=GetBindParameter(dwUserIndex);

		//��������
		WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
		WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

		//��������
		if (pIServerUserItem->IsAndroidUser()==true)
		{
			//�����û�
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_AndroidUserManager.SendDataToClient(pBindParameter->dwSocketID,MDM_G_FRAME,CMD_GF_SYSTEM_MESSAGE,&SystemMessage,wSendSize);

			//�رմ���
			if ((wType&(SMT_CLOSE_ROOM|SMT_CLOSE_LINK))!=0) m_AndroidUserManager.DeleteAndroidUserItem(pBindParameter->dwSocketID);
		}
		else
		{
			//�����û�
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID,MDM_G_FRAME,CMD_GF_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
		}

		return true;
	}

	return false;
}

//������Ϣ
bool CAttemperEngineSink::SendRoomMessage(DWORD dwSocketID, LPCTSTR lpszMessage, WORD wType, bool bAndroid)
{
	//��������
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//��������
	SystemMessage.wType=wType;
	SystemMessage.wLength=lstrlen(lpszMessage)+1;
	lstrcpyn(SystemMessage.szString,lpszMessage,CountArray(SystemMessage.szString));

	//��������
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//��������
	if (bAndroid)
	{
		//�����û�
		m_AndroidUserManager.SendDataToClient(dwSocketID,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
	}
	else
	{
		//�����û�
		m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
	}

	return true;
}

//�����������Ϣ
bool CAttemperEngineSink::SendMessageLobbyAndAllGame( LPCTSTR lpszMessage, WORD wType ,WORD MsgID)
{
	/*
	//������Ϣ
	CMD_CS_C_SendMarquee pSendLobbyMessage;
	ZeroMemory(&pSendLobbyMessage,sizeof(pSendLobbyMessage));
	lstrcpyn(pSendLobbyMessage.szMessage,lpszMessage,CountArray(pSendLobbyMessage.szMessage));
	pSendLobbyMessage.MsgType=wType;
	pSendLobbyMessage.MsgID = MsgID;
	//���͸�Э��������
	m_pITCPSocketService->SendData(MDM_WEB,CPR_WP_WEB_MARQUEE,&pSendLobbyMessage,sizeof(CMD_CS_C_SendMarquee));
	*/
	return true;
}

//��������
bool CAttemperEngineSink::SendData(BYTE cbSendMask, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//��������
	if ((cbSendMask&BG_COMPUTER)!=0)
	{
		m_AndroidUserManager.SendDataToClient(wMainCmdID,wSubCmdID,pData,wDataSize);
	}

	//�û�����
	m_pITCPNetworkEngine->SendDataBatch(wMainCmdID,wSubCmdID,pData,wDataSize,cbSendMask);

	return true;
}

//��������
bool CAttemperEngineSink::SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//��������
	if (LOWORD(dwSocketID)!=INVALID_WORD)
	{
		if (LOWORD(dwSocketID)>=INDEX_ANDROID)
		{
			//�����û�
			m_AndroidUserManager.SendDataToClient(dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);
		}
		else 
		{
			//�����û�
			m_pITCPNetworkEngine->SendData(dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);
		}
	}

	return true;
}

//��������
bool CAttemperEngineSink::SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;
	if(pIServerUserItem->IsVirtualUser())
	{
		return true;
	}

	//��������
	if (pIServerUserItem->GetBindIndex()!=INVALID_WORD)
	{
		if (pIServerUserItem->IsAndroidUser()==true)
		{
			//�����û�
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_AndroidUserManager.SendDataToClient(pBindParameter->dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);
		}
		else
		{
			//�����û�
			WORD wBindIndex=pIServerUserItem->GetBindIndex();
			tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);
		}
		return true;
	}

	return false;
}

//�����û��Ƹ���Ϣ
bool CAttemperEngineSink::OnEventModifyUserTreasure(IServerUserItem *pIServerUserItem, DWORD dwTableID, BYTE byTableMode, BYTE byRound, SCORE lUserTreasuse, BYTE byWin)
{
	//Ч�����
	if (pIServerUserItem==NULL) return false;

	//��������
	STR_DBR_GR_MODIFY_USER_TREASURE DBR;
	ZeroMemory(&DBR, sizeof(DBR));
	DBR.dwUserId  =  pIServerUserItem->GetUserID(); //pUserInfo->dwUserID;
	DBR.dwKind = ((m_pGameServiceOption->dwServerID) >> 16) & 0xFF;
	DBR.dwTableID = dwTableID;
	DBR.byTableMode = byTableMode;
	DBR.byRound = byRound;
	DBR.lUserTreasure = lUserTreasuse;
	DBR.byWin = byWin;

	//��������
	DWORD dwBindIndex = pIServerUserItem->GetBindIndex();
	tagBindParameter *pBindParameter = GetBindParameter(static_cast<WORD>(dwBindIndex));
	 
	//������ʾ
		TCHAR szString[512]=TEXT("");
		_sntprintf_s(szString,CountArray(szString),TEXT("Step2 Round = %d, UserID = %ld, pBindParameter = %d"),
			byRound,
			pIServerUserItem->GetUserID(),
			(pBindParameter == NULL)
			);
		CTraceService::TraceString(szString,TraceLevel_Normal);

	if (NULL != pBindParameter)
	{
		m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_MODIFY_USER_TREASURE, pBindParameter->dwSocketID, &DBR, sizeof(DBR));
	}
	else
		m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GR_MODIFY_USER_TREASURE, 0, &DBR, sizeof(DBR));
	
	return true;
}

//�û�״̬
bool CAttemperEngineSink::OnEventUserItemStatus(IServerUserItem * pIServerUserItem, WORD wOldTableID, WORD wOldChairID)
{
	//Ч�����
	if (pIServerUserItem==NULL) return false;

	//��������
	CMD_GR_UserStatus UserStatus;
	ZeroMemory(&UserStatus,sizeof(UserStatus));

	//��������
	UserStatus.dwUserID=pIServerUserItem->GetUserID();
	UserStatus.UserStatus.wTableID=pIServerUserItem->GetTableID();
	UserStatus.UserStatus.wChairID=pIServerUserItem->GetChairID();
	UserStatus.UserStatus.cbUserStatus=pIServerUserItem->GetUserStatus();

	//��������
	SendData(BG_COMPUTER,MDM_USER,SUB_GR_USER_STATUS,&UserStatus,sizeof(UserStatus));

	if(pIServerUserItem->GetUserStatus()==US_SIT)
		SendDataBatchToMobileUser(pIServerUserItem->GetTableID(),MDM_USER,SUB_GR_USER_STATUS,&UserStatus,sizeof(UserStatus));
	else
		SendDataBatchToMobileUser(wOldTableID,MDM_USER,SUB_GR_USER_STATUS,&UserStatus,sizeof(UserStatus));


	//�뿪�ж�
	if (pIServerUserItem->GetUserStatus()==US_NULL)
	{
		//��ȡ��
		WORD wBindIndex=pIServerUserItem->GetBindIndex();
		tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

		//�������
		if (pBindParameter!=NULL)
		{
			//�󶨴���
			if (pBindParameter->pIServerUserItem==pIServerUserItem)
			{
				pBindParameter->pIServerUserItem=NULL;
			}

			//�ж�����
			if (pBindParameter->dwSocketID!=0L)
			{
				if (LOWORD(pBindParameter->dwSocketID)>=INDEX_ANDROID)
				{
					m_AndroidUserManager.DeleteAndroidUserItem(pBindParameter->dwSocketID);
				}
				else
				{
					m_pITCPNetworkEngine->ShutDownSocket(pBindParameter->dwSocketID);
				}
			}
		}

		//�뿪����
		OnEventUserLogout(pIServerUserItem,LER_NORMAL);
	}

	//��ʼ�ж�
	if (pIServerUserItem->GetUserStatus()==US_READY)
	{
		//״̬У��
		CTableFrame *pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];

		//��ʼ�ж�
		if (pTableFrame->EfficacyStartGame(pIServerUserItem->GetChairID())==true)
		{
			pTableFrame->StartGame(); 
		}
	}

	return true;
}

//�û�Ȩ��
bool CAttemperEngineSink::OnEventUserItemRight(IServerUserItem *pIServerUserItem, DWORD dwAddRight, DWORD dwRemoveRight,bool bGameRight)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();

	DBR_GR_ManageUserRight ManageUserRight= {0};
	ManageUserRight.dwUserID = pUserInfo->dwUserID;
	ManageUserRight.dwAddRight = dwAddRight;
	ManageUserRight.dwRemoveRight = dwRemoveRight;
	ManageUserRight.bGameRight=bGameRight;

	//��������
	m_pIDBCorrespondManager->PostDataBaseRequest(ManageUserRight.dwUserID,DBR_GR_MANAGE_USER_RIGHT,0,&ManageUserRight,sizeof(ManageUserRight));

	return true;
}

//������Ϣ
bool CAttemperEngineSink::OnDBGameAndroidInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��������
	DBO_GR_GameAndroidInfo * pGameAndroidInfo=(DBO_GR_GameAndroidInfo *)pData;
	WORD wHeadSize=sizeof(DBO_GR_GameAndroidInfo)-sizeof(pGameAndroidInfo->AndroidParameter);

	//Ч�����
	ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pGameAndroidInfo->wAndroidCount*sizeof(pGameAndroidInfo->AndroidParameter[0]))));
	if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pGameAndroidInfo->wAndroidCount*sizeof(pGameAndroidInfo->AndroidParameter[0])))) return false;

	//���û���
	if (pGameAndroidInfo->lResultCode==DB_SUCCESS)
	{
		m_AndroidUserManager.SetAndroidStock(pGameAndroidInfo->AndroidParameter,pGameAndroidInfo->wAndroidCount);
	}

	return true;
}

//���ض�����������
bool CAttemperEngineSink::On_DBO_GR_LOAD_OFFLINE(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	if(sizeof(BYTE ) != wDataSize) return false;

	m_GameProgress =  *((BYTE*)pData);

	return true;
}

//�޸����׶�yang
bool CAttemperEngineSink::OnDBRoomLevelModify(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT(wDataSize==sizeof(DBR_GR_RoomLevelModify));
	if(wDataSize!=sizeof(DBR_GR_RoomLevelModify)) return false;

	//���ݿ��޸ĳɹ� ֪ͨ��Ϸ������
	DBR_GR_RoomLevelModify * pRoomLevelModify = (DBR_GR_RoomLevelModify *)pData;
	/*if(pRoomLevelModify==NULL) return false;
	m_GameControlManager.SetControlLevel(pRoomLevelModify->wGameLevel);
	for ( int i = 0; i < m_TableFrameArray.GetCount(); i++ )
	{
	m_TableFrameArray[i]->WebControlGameLevel(pRoomLevelModify->wSeverID,pRoomLevelModify->wGameLevel);
	}*/

	return true;
}

//�޸ķ������ֵyang
bool CAttemperEngineSink::OnDBRoomControlValModify(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT(wDataSize==sizeof(DBR_GR_RoomControlValModify));
	if(wDataSize!=sizeof(DBR_GR_RoomControlValModify)) return false;

	//���ݿ��޸ĳɹ� ֪ͨ��Ϸ������
	DBR_GR_RoomControlValModify * pRoomControlValModify = (DBR_GR_RoomControlValModify *)pData;
	if(pRoomControlValModify==NULL) return false;

	InitRoomControlVal( pRoomControlValModify->wControlVal );

	return true;
}

#pragma region ����Ϣ�ŷַ�����
//ע���¼�
bool CAttemperEngineSink::OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case CPO_PGPL_REGISTER_SUCESS:		//ע�����
		{
			//�¼�����
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_SUCCESS;
			SendUIControlPacket(UI_CORRESPOND_RESULT,&ControlResult,sizeof(ControlResult));

			return true;
		}
	case CPO_PGPL_REGISTER_FAILURE:		//ע��ʧ��
		{
			//Ч�����
			ASSERT(wDataSize ! = sizeof(STR_CPO_PGPL_REGISTER_FAILURE));
			if (wDataSize != sizeof(STR_CPO_PGPL_REGISTER_FAILURE)) return false;

			//��������
			STR_CPO_PGPL_REGISTER_FAILURE * pRegisterFailure=(STR_CPO_PGPL_REGISTER_FAILURE *)pData;

			//�رմ���
			m_bNeekCorrespond=false;
			m_pITCPSocketService->CloseSocket();

			//��ʾ��Ϣ
			if (lstrlen(pRegisterFailure->szDescribeString)>0)
			{
				CTraceService::TraceString(pRegisterFailure->szDescribeString,TraceLevel_Exception);
			}

			//�¼�֪ͨ
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_FAILURE;
			SendUIControlPacket(UI_CORRESPOND_RESULT,&ControlResult,sizeof(ControlResult));

			return true;
		}
	}

	return true;
}

//��ת����
bool CAttemperEngineSink::OnTCPSocketMainTransfer(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case CPO_PG_CLUB_TABLE_DISSOLVE: //��¼��֪ͨЭ����, Э����֪ͨ��Ϸ�� ��ɢ����
		{
			//У������
			if (wDataSize != sizeof(STR_CPO_PG_CLUB_TABLE_DISSOLVE)) return false;
			
			STR_CPO_PG_CLUB_TABLE_DISSOLVE* pCPO = (STR_CPO_PG_CLUB_TABLE_DISSOLVE *)pData;

			//������Ǳ�����������Ϣ, ����˵�
			if(pCPO->dwGameID != (m_pGameServiceOption->dwServerID & 0xFFFFFF00))
			{
				return true;
			}

			//����Ǳ�����������Ϣ, ��֪ͨ����Ϸ��ɢ��Ϸ
			for(int i = 0; i < m_TableFrameArray.GetCount(); i++)
			{
				//У��
				CTableFrame *pTableFrame = m_TableFrameArray.GetAt(i);
				if (NULL == pTableFrame)
					continue;

				//�ҵ���ͬ���Ӻŵģ� Ȼ���ɢ
				if(pTableFrame->GetPassword() == pCPO->dwTableID)
				{
					//���÷��䴦�ڽ�ɢ״̬
					pTableFrame->SetDismissState(true);

					//֪ͨ�ͻ��˵�����ֽ������
					STR_CMD_GR_FRAME_DISMISS_RESULT DismissResult;
					ZeroMemory(&DismissResult, sizeof(DismissResult));
					DismissResult.cbDismiss = 1;
					DismissResult.cbAgree = 1;					
					if (pTableFrame->GetGameStatus() != GAME_STATUS_FREE)
					{
						DismissResult.cbClubQuit = 1;
					}
					
					pTableFrame->SendTableData(INVALID_CHAIR, CMD_GR_FRAME_DISMISS_RESULT, &DismissResult, sizeof(STR_CMD_GR_FRAME_DISMISS_RESULT), MDM_G_FRAME);
					
					//֪ͨ����Ϸ��ɢ����
					//����״̬ ֱ�ӽ�ɢ
					if(GAME_STATUS_FREE == pTableFrame->GetGameStatus())
					{
						pTableFrame->ConcludeGame(GAME_CONCLUDE_NORMAL);
					}
					//��Ϸ״̬�� ֪ͨ����Ϸ
					else
					{
						pTableFrame->OnEventClubDismissRoom();
					}
					break;
				}
			}

			return true;
		}
	}

	return true;
}

//�����¼�
bool CAttemperEngineSink::OnTCPSocketMainUserCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_COLLECT_REQUEST:	//�û�����
		{
			//��������
			CMD_CS_C_UserEnter UserEnter;
			ZeroMemory(&UserEnter,sizeof(UserEnter));

			//�����û�
			WORD wIndex=0;
			do
			{
				//��ȡ�û�
				IServerUserItem * pIServerUserItem=m_ServerUserManager.EnumUserItem(wIndex++);
				if (pIServerUserItem==NULL) break;

				//���ñ���
				UserEnter.dwUserID=pIServerUserItem->GetUserID();
				lstrcpyn(UserEnter.szNickName,pIServerUserItem->GetNickName(),CountArray(UserEnter.szNickName));

				//������Ϣ
				UserEnter.cbGender=pIServerUserItem->GetGender();
				UserEnter.cbMemberOrder=pIServerUserItem->GetMemberOrder();
				UserEnter.cbMasterOrder=pIServerUserItem->GetMasterOrder();

				//��������
				m_pITCPSocketService->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_USER_ENTER,&UserEnter,sizeof(UserEnter));
			} while (true);

			//�㱨���
			m_bCollectUser=true;
			m_pITCPSocketService->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_USER_FINISH);

			return true;
		}
	}

	return true;
}

#pragma endregion

//��¼����
bool CAttemperEngineSink::OnTCPNetworkMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CG_LOGON_USERID:		//I D ��¼
		{
			return On_SUB_CG_Logon_UserID(pData,wDataSize,dwSocketID);
		}
	}

	return true;
}

//�û�����
bool CAttemperEngineSink::OnTCPNetworkMainUser(WORD wSubCmdID, VOID *pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CG_USER_RULE:			//�û�����
		{
			return On_SUB_CG_User_Rule(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_LOOKON:		//�û��Թ�
		{
			return On_SUB_CG_User_Lookon(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_SITDOWN:		//�û�����
		{
			return On_SUB_CG_User_SitDown(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_STANDUP:		//�û�����
		{
			return On_SUB_CG_User_StandUp(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_INVITE_USER:	//�����û�
		{
			return On_SUB_CG_User_InviteUser(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_REFUSE_SIT:	//�ܾ��������
		{
			return On_SUB_CG_User_RefuseSit(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_KICK_USER:		//�߳��û�
		{
			return On_SUB_CG_User_KickUser(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_CHAIR_REQ:    //�������λ��
		{
			return OnTCPNetworkSubUserChairReq(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_CREATE_ROOM:	//��������
		{
			return On_SUB_CG_USER_CREATE_ROOM(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_CLUB_CREATE_TABLE:	//��������
		{
			return On_SUB_CG_USER_CREATE_TABLE(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_JOIN_FK_ROOM:		//�������� ��Ҫ����
		{
			return On_SUB_User_JoinFkRoom(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_JOIN_TABLE_NO_PASS://�������� ����Ҫ����
		{
			return On_SUB_CG_USER_JOIN_TABLE_NO_PASS(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_JOIN_GOLD_HALL_ROOM:	//�����Ҵ�������
		{
			return On_SUB_CG_USER_JOIN_GOLD_HALL_ROOM(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_RECONNECT_ROOM:	//��������
		{
			return On_SUB_User_ReconnectRoom(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_CREATE_GROUP_ROOM:	//��������Ȧ����
		{
			return On_Sub_CG_User_CreateGroupRoom(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_JOIN_GROUP_ROOM:	//��������Ȧ����
		{
			return On_Sub_CG_User_JoinGroupRoom(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_JOIN_GOLD_ROOM:	//�����ҷ�
		{
			return On_SUB_User_JoinGoldRoom(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_GET_TABLELIST:			//��ÿ����б�
		{
			return OnTCPNetworkSubGetTableList(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_QUERY_LOTTERY:			//����齱
		{
			return OnTcpNetworkQueryLottery(pData, wDataSize, dwSocketID);
		}
	}

	return false;
}

//��������
bool CAttemperEngineSink::OnTCPNetworkMainMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��ȡ��Ϣ
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//Ч��ӿ�
	ASSERT(m_pIGameMatchServiceManager!=NULL);
	if (m_pIGameMatchServiceManager==NULL) return false;

	//��Ϣ����
	return m_pIGameMatchServiceManager->OnEventSocketMatch(wSubCmdID,pData,wDataSize,pIServerUserItem,dwSocketID);
}

//��Ϸ����
bool CAttemperEngineSink::OnTCPNetworkMainGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��ȡ��Ϣ
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�������
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	if ((wTableID==INVALID_TABLE)||(wChairID==INVALID_CHAIR)) return true;

	//��Ϣ���� 
	CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
	return pTableFrame->OnEventSocketGame(wSubCmdID,pData,wDataSize,pIServerUserItem);
}

//��ܴ���
bool CAttemperEngineSink::OnTCPNetworkMainFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��ȡ��Ϣ
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�������
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	if ((wTableID==INVALID_TABLE)||(wChairID==INVALID_CHAIR)) return true;

	//��Ϣ���� 
	CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
	return pTableFrame->OnEventSocketFrame(wSubCmdID,pData,wDataSize,pIServerUserItem);
}

//I D ��¼
bool CAttemperEngineSink::On_SUB_CG_Logon_UserID(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	if (wDataSize != sizeof(STR_SUB_CG_LOGON_USERID)) return false;
	STR_SUB_CG_LOGON_USERID *pLogonUserID = (STR_SUB_CG_LOGON_USERID *)pData;

	//�汾У��   У��client����Ϸ  ��  server����Ϸ�汾�Ƿ�ƥ��
	DWORD clientSubGameVersion = pLogonUserID->dwSubGameVersion;
	DWORD serverSubGameVersion = m_pGameServiceAttrib->dwSubGameVersion;
	if(0 != Compate_clientSubGame_ServerSubGame(clientSubGameVersion, serverSubGameVersion))
	{
		//printf("\n���ͻ��ˡ���USERID��¼���汾��ƥ��\n");
	}

	//��ʾ��Ϣ
	TCHAR szString[512]=TEXT("");

	//�ظ��ж�
	WORD wBindIndex = LOWORD(dwSocketID);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex); //�п���socket
	IServerUserItem *pIBindUserItem = GetBindUserItem(wBindIndex);   //�ÿ���socketû�а��û�
	if ((pBindParameter==NULL)||(pIBindUserItem!=NULL))
	{ 
		_sntprintf_s(szString,CountArray(szString),
			TEXT("��ID��¼����%ld�ظ���¼1��"), 
			pLogonUserID->dwUserID);
		CTraceService::TraceString(szString,TraceLevel_Info);

		return false;
	}

	//���û��б��ȡ�û�
	IServerUserItem *pIServerUserItem = m_ServerUserManager.SearchUserItem(pLogonUserID->dwUserID);

	//��������¼���û��������߳�
	if (pIServerUserItem!=NULL)
	{
		//��������¼���������
		//1. ���û�����ǻ����û�, ���Ǵ���ص�¼
		//2. �û����ǻ����û�, �����ڸý����ϵ�¼
		if((pIServerUserItem->IsAndroidUser() && (pBindParameter->dwClientAddr!=0L))
			|| (!pIServerUserItem->IsAndroidUser() && (pBindParameter->dwClientAddr==0L)))
		{
			_sntprintf_s(szString,CountArray(szString),
				TEXT("��ID��¼����%ld��¼ʧ�ܡ���������¼, �������߳�"), 
				pLogonUserID->dwUserID);
			CTraceService::TraceString(szString,TraceLevel_Info);

			return false;
		}
	}

	
	//�л��ж� TODONOW �ص�鿴
	if( NULL != pIServerUserItem )
	{
		_sntprintf_s(szString,CountArray(szString),
			TEXT("��ID��¼����%ld�ظ���¼���л�����1"), 
			pLogonUserID->dwUserID);
		CTraceService::TraceString(szString,TraceLevel_Info);

		return SwitchUserItemConnect(pIServerUserItem, pLogonUserID->szMachineID, 
			wBindIndex,
			pLogonUserID->dLongitude, pLogonUserID->dLatitude);
	}
	

	//��������
	STR_DBR_CG_LOGON_USERID LogonUserID;
	ZeroMemory(&LogonUserID,sizeof(LogonUserID));

	//��������
	LogonUserID.dwUserID = pLogonUserID->dwUserID;
	LogonUserID.dwClientAddr = pBindParameter->dwClientAddr;
	LogonUserID.dLatitude = pLogonUserID->dLatitude;
	LogonUserID.dLongitude = pLogonUserID->dLongitude;
	lstrcpyn(LogonUserID.szPassword,pLogonUserID->szPassword,CountArray(LogonUserID.szPassword));
	lstrcpyn(LogonUserID.szMachineID,pLogonUserID->szMachineID,CountArray(LogonUserID.szMachineID));

	//Ͷ������
	return m_pIDBCorrespondManager->PostDataBaseRequest(LogonUserID.dwUserID, DBR_CG_LOGON_USERID, dwSocketID, &LogonUserID,sizeof(LogonUserID));
}

//ID��¼����
bool CAttemperEngineSink::On_CMD_GC_Logon_UserID(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize != sizeof(STR_DBO_CG_LOGON_USERID)) return false;
	STR_DBO_CG_LOGON_USERID *pDBOLogon = (STR_DBO_CG_LOGON_USERID *)pData;

	//�ж�����
	if ((m_pNormalParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//�û���¼ʧ�ܣ�ֱ�ӷ���
	if ( DB_SUCCESS != pDBOLogon->lResultCode )
	{
		//���췵������
		STR_CMD_GC_LOGON_USERID logonFail;
		ZeroMemory(&logonFail, sizeof(STR_CMD_GC_LOGON_USERID));
		logonFail.lResultCode = 0;
		lstrcpyn(logonFail.szDescribeString, pDBOLogon->szDescribeString, CountArray(logonFail.szDescribeString));

		//��������
		m_pITCPNetworkEngine->SendData(dwContextID, MDM_GR_LOGON, CMD_GC_LOGON_USERID, &logonFail, sizeof(STR_CMD_GC_LOGON_USERID));
		return true;
	}

	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex); //Ѱ�ҿ���socket

	//�޿���socket�˳�
	if ( (NULL != pBindParameter->pIServerUserItem)||
		 (pBindParameter->dwSocketID != dwContextID) )
	{
		//��ʾ��Ϣ
		TCHAR szDescribe[128]=TEXT("");
		_sntprintf_s(szDescribe,CountArray(szDescribe),
			TEXT("��ID��¼����%ld��¼ʧ�ܡ��޿���socket"),
			pDBOLogon->dwUserID);
		CTraceService::TraceString(szDescribe,TraceLevel_Warning);

		return true;
	}

	//���û��б��л�ȡ �û�
	IServerUserItem *pIServerUserItem = m_ServerUserManager.SearchUserItem(pDBOLogon->dwUserID);

	/*
	//�ظ���¼�ж� TODONOW
	if (pIServerUserItem!=NULL)
	{
		//�л��û�
		SwitchUserItemConnect(pIServerUserItem, pDBOLogon->szMachineID, wBindIndex, pDBOLogon->dLongitude, pDBOLogon->dLatitude, 
								pDBOLogon->cbDeviceType, pDBOLogon->wBehaviorFlags, pDBOLogon->wPageTableCount);

		//��ʾ��Ϣ
		TCHAR szDescribe[128]=TEXT("");
		_sntprintf_s(szDescribe,CountArray(szDescribe),
			TEXT("��ID��¼����%ld�ظ���¼2��"),
			pIServerUserItem->GetUserID());
		CTraceService::TraceString(szDescribe,TraceLevel_Warning);

		return true;
	}
	*/

	//�����û�
	ActiveUserItem(&pIServerUserItem, dwContextID, pBindParameter, pDBOLogon, wBindIndex);

	return true;
}

//ID��¼�ɹ��������û�
void CAttemperEngineSink::ActiveUserItem(IServerUserItem **pIServerUserItem, DWORD dwContextID, 
		tagBindParameter *pBindParameter, STR_DBO_CG_LOGON_USERID *pDBOLogon, WORD wBindIndex)
{
	if ( NULL == pBindParameter || NULL == pDBOLogon )
	{
		return;
	}

	//�û�����
	tagUserInfo UserInfo;
	tagUserInfoPlus UserInfoPlus;
	ZeroMemory(&UserInfo,sizeof(UserInfo));
	ZeroMemory(&UserInfoPlus,sizeof(UserInfoPlus));

	//��������
	UserInfo.dwUserID = pDBOLogon->dwUserID;
	UserInfo.dwGroupID = pDBOLogon->dwGroupID;
	UserInfo.dwCustomID = pDBOLogon->dwCustomID;
	lstrcpyn(UserInfo.szNickName,pDBOLogon->szNickName,CountArray(UserInfo.szNickName));
	lstrcpyn(UserInfo.szHeadUrl,pDBOLogon->szHeadUrl,CountArray(UserInfo.szHeadUrl));


	//�û�����
	UserInfo.cbGender=pDBOLogon->cbGender;
	UserInfo.cbMemberOrder=pDBOLogon->cbMemberOrder;
	UserInfo.cbMasterOrder=pDBOLogon->cbMasterOrder;
	lstrcpyn(UserInfo.szGroupName,pDBOLogon->szGroupName,CountArray(UserInfo.szGroupName));
	lstrcpyn(UserInfo.szUnderWrite,pDBOLogon->szUnderWrite,CountArray(UserInfo.szUnderWrite));

	//״̬����
	UserInfo.cbUserStatus=US_FREE;
	UserInfo.wTableID=INVALID_TABLE;
	UserInfo.wChairID=INVALID_CHAIR;

	//������Ϣ
	UserInfo.lScore=pDBOLogon->lScore;
	UserInfo.lGrade=pDBOLogon->lGrade;
	UserInfo.lOpenRoomCard=pDBOLogon->lOpenRoomCard;
	UserInfo.lDiamond=pDBOLogon->lDiamond;
	UserInfo.lGold=pDBOLogon->lGold;
	UserInfo.lControlScore=pDBOLogon->lControlScore;
	UserInfo.dwWinCount=pDBOLogon->dwWinCount;
	UserInfo.dwLostCount=pDBOLogon->dwLostCount;
	UserInfo.dwDrawCount=pDBOLogon->dwDrawCount;
	UserInfo.dwFleeCount=pDBOLogon->dwFleeCount;
	UserInfo.dwUserMedal=pDBOLogon->dwUserMedal;
	UserInfo.dwExperience=pDBOLogon->dwExperience;
	UserInfo.lLoveLiness=pDBOLogon->lLoveLiness;

	//λ����Ϣ
	UserInfo.dLatitude = pDBOLogon->dLatitude;
	UserInfo.dLongitude = pDBOLogon->dLongitude;

	//��¼��Ϣ
	UserInfoPlus.dwLogonTime=(DWORD)time(NULL);
	UserInfoPlus.dwInoutIndex=pDBOLogon->dwInoutIndex;

	//�û�Ȩ��
	UserInfoPlus.dwUserRight=pDBOLogon->dwUserRight;
	UserInfoPlus.dwMasterRight=pDBOLogon->dwMasterRight;

	//��������
	bool bAndroidUser = (wBindIndex>=INDEX_ANDROID);						//�û����ͣ�����/������
	UserInfoPlus.bAndroidUser = bAndroidUser;
	UserInfoPlus.lRestrictScore=0L;//����ÿ�ַⶥ
	lstrcpyn(UserInfoPlus.szPassword,pDBOLogon->szPassword,CountArray(UserInfoPlus.szPassword));

	//������Ϣ
	UserInfoPlus.wBindIndex=wBindIndex;
	UserInfoPlus.dwClientAddr=pBindParameter->dwClientAddr;
	lstrcpyn(UserInfoPlus.szMachineID,pDBOLogon->szMachineID,CountArray(UserInfoPlus.szMachineID));

	//�����û� -- �����û���Ϣ
	m_ServerUserManager.InsertUserItem(pIServerUserItem, UserInfo, UserInfoPlus);

	//�����ж� -- �����û�pIServerUserItem��Ϣʧ��
	if (pIServerUserItem == NULL)
	{
		TCHAR szDescribe[128]=TEXT("");
		_sntprintf_s(szDescribe,CountArray(szDescribe),
			TEXT("��ID��¼�������%ld��¼ʧ�ܡ� pIServerUserItem������Ϣʧ��"),
			UserInfo.dwUserID);
		CTraceService::TraceString(szDescribe,TraceLevel_Warning);

		//�Ͽ��û�
		if (bAndroidUser==true)
		{
			m_AndroidUserManager.DeleteAndroidUserItem(dwContextID);
		}
		else
		{
			m_pITCPNetworkEngine->ShutDownSocket(dwContextID);
		}
		return;
	}

	//�����û�
	pBindParameter->pIServerUserItem = *pIServerUserItem;
	
	//��¼�¼�
	OnEventUserLogon(*pIServerUserItem, false);
}

//�û���¼
VOID CAttemperEngineSink::OnEventUserLogon(IServerUserItem * pIServerUserItem, bool bAlreadyOnLine)
{
	//��ȡ����
	WORD wBindIndex = pIServerUserItem->GetBindIndex();
	bool bAndroidUser = pIServerUserItem->IsAndroidUser();
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);

	/* 1. ���췵������ */
	STR_CMD_GC_LOGON_USERID logon;
	ZeroMemory(&logon, sizeof(STR_CMD_GC_LOGON_USERID));
	logon.lResultCode = 0;
	lstrcpyn(logon.szDescribeString, TEXT("�û���¼�ɹ�"), CountArray(logon.szDescribeString));
	
	//TODONOW
	logon.dwOffLineGameID = pIServerUserItem -> GetOfflineGameID();

	CString strTrace;
	strTrace.Format(TEXT("��ID��¼����%ld���������� GetOfflineGameID = %ld"),
		(pIServerUserItem->GetUserInfo())->dwUserID, 
		logon.dwOffLineGameID);
	CTraceService::TraceString(strTrace, TraceLevel_Debug);

	//��������
	m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MDM_GR_LOGON, CMD_GC_LOGON_USERID, &logon, sizeof(STR_CMD_GC_LOGON_USERID));
	
	/* 2. ��������Լ�����Ϣ���ͻ��� */
	SendUserInfoPacket(pIServerUserItem, pBindParameter->dwSocketID);

	/* 3. ���͵�¼��Ϸ���������û���Ϣ�����û� */
	WORD wUserIndex = 0;
	IServerUserItem *pIServerUserItemSend = NULL;
	while (true)
	{
		pIServerUserItemSend = m_ServerUserManager.EnumUserItem(wUserIndex++);
		if (pIServerUserItemSend == NULL) break;
		if (pIServerUserItemSend == pIServerUserItem) continue;
		SendUserInfoPacket(pIServerUserItemSend, pBindParameter->dwSocketID);
	}

	/* 4. ������Ϣ */
	CMD_GR_TableInfo TableInfo;
	ZeroMemory(&TableInfo, sizeof(CMD_GR_TableInfo));

	//��ֵ
	TableInfo.wTableCount = (WORD)m_TableFrameArray.GetCount();
	for (WORD i=0; i<TableInfo.wTableCount; i++)
	{
		CTableFrame *pTableFrame = m_TableFrameArray[i];
		TableInfo.TableStatusArray[i].cbTableLock = pTableFrame->IsTableLocked()?TRUE:FALSE;
		TableInfo.TableStatusArray[i].cbPlayStatus = pTableFrame->IsTableStarted()?TRUE:FALSE;
	}

	//����״̬
	WORD wHeadSize=sizeof(TableInfo)-sizeof(TableInfo.TableStatusArray);
	WORD wSendSize=wHeadSize+TableInfo.wTableCount*sizeof(TableInfo.TableStatusArray[0]);
	SendData(pBindParameter->dwSocketID, MDM_GR_STATUS, CMD_GR_TABLE_INFO, &TableInfo, wSendSize);

	/* 5. ���������Ϣ		TODO  */
	if (!bAlreadyOnLine)
	{
		SendUserInfoPacket(pIServerUserItem,INVALID_DWORD);
	}

	/* 6. �ǻ����� */
	if (!bAndroidUser)
	{
		//��������Ⱥ��
		m_pITCPNetworkEngine->AllowBatchSend(pBindParameter->dwSocketID, true, BG_COMPUTER);
	}

	/* 7. �㲥��������		TODONOW �ͻ���û�õ�����Ϣ�� */
	CMD_GF_OnlinePlayers OnlinePlayers;
	ZeroMemory(&OnlinePlayers, sizeof(CMD_GF_OnlinePlayers));

	//��ֵ
	for(WORD i=0;static_cast<DWORD>(i) < m_ServerUserManager.GetUserItemCount();i++)
	{
		if( m_ServerUserManager.EnumUserItem(i)->GetUserStatus() != US_OFFLINE 
			&& !m_ServerUserManager.EnumUserItem(i)->IsVirtualUser())
			OnlinePlayers.wOnlinePlayers++;
	}

	//��������
	for(WORD i=0;static_cast<DWORD>(i) < m_ServerUserManager.GetUserItemCount();i++)
	{
		IServerUserItem* pItem = m_ServerUserManager.EnumUserItem(i);
		if( pItem->GetUserStatus() != US_OFFLINE && !pItem->IsVirtualUser())
		{
			SendData(pItem,MDM_USER,SUB_GF_ONLINE_PLAYERS,&OnlinePlayers,sizeof(CMD_GF_OnlinePlayers));
		}
	}

	/* 10. ������Ϣ */
	if(m_pIGameMatchServiceManager!=NULL)
	{
		m_pIGameMatchServiceManager->SendMatchInfo(pIServerUserItem);
	}

	/* 11. ֪ͨ���ݿ�ı� ���״̬ */
	//TODONOWW
	DBR_GP_UserQuitInfo quitInfo;
	quitInfo.dwUserID = pIServerUserItem->GetUserID();
	quitInfo.byOnlineMask = 2;
	m_pIRecordDataBaseEngine->PostDataBaseRequest(DBR_GP_GAME_USER_STATE,0, &quitInfo,sizeof(quitInfo));
}

//�л�����
bool CAttemperEngineSink::SwitchUserItemConnect(IServerUserItem * pIServerUserItem, TCHAR szMachineID[LEN_MACHINE_ID], WORD wTargetIndex,
												const double &dLongitude, const double &dLatitude,
												BYTE cbDeviceType, WORD wBehaviorFlags, WORD wPageTableCount)
{
	//Ч�����
	if ((NULL == pIServerUserItem)||(INVALID_WORD == wTargetIndex)) 
		return false;

	//�û���Ч���Ͽ��Ѱ󶨵��û�
	if (INVALID_WORD != pIServerUserItem->GetBindIndex())	
	{
		//�󶨲���
		WORD wSourceIndex = pIServerUserItem->GetBindIndex();
		tagBindParameter *pSourceParameter = GetBindParameter(wSourceIndex);

		//�����
		if ( (NULL != pSourceParameter) && (pSourceParameter->pIServerUserItem==pIServerUserItem) ) 
			pSourceParameter->pIServerUserItem=NULL;

		//�Ͽ��û�
		if (pIServerUserItem->IsAndroidUser()==true)
		{
			m_AndroidUserManager.DeleteAndroidUserItem(pSourceParameter->dwSocketID);
		}
		else
		{
			m_pITCPNetworkEngine->ShutDownSocket(pSourceParameter->dwSocketID);
		}
	}

	//�����ж�
	LPCTSTR pszMachineID = pIServerUserItem->GetMachineID();
	bool bSameMachineID = (lstrcmp(pszMachineID, szMachineID)==0);

	//��������
	bool bAndroidUser = (wTargetIndex>=INDEX_ANDROID);
	tagBindParameter *pTargetParameter = GetBindParameter(wTargetIndex);

	//���¼����û�
	pTargetParameter->pIServerUserItem = pIServerUserItem;
	pIServerUserItem->SetUserParameter(pTargetParameter->dwClientAddr, wTargetIndex, szMachineID, bAndroidUser, false, dLongitude, dLatitude);

	//��¼�¼�
	OnEventUserLogon(pIServerUserItem,true);

	return true;
}

/*********************************************������Ϣ 3 �û����*********************************************************/
//�û�����
bool CAttemperEngineSink::On_SUB_CG_User_Rule(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_GR_UserRule));
	if (wDataSize<sizeof(CMD_GR_UserRule)) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�����ж�
	//ASSERT(CServerRule::IsForfendGameRule(m_pGameServiceOption->dwServerRule)==false);
	//if (CServerRule::IsForfendGameRule(m_pGameServiceOption->dwServerRule)==true) return true;

	//��Ϣ����
	tagUserRule * pUserRule=pIServerUserItem->GetUserRule();
	CMD_GR_UserRule * pCMDUserRule=(CMD_GR_UserRule *)pData;

	//�����־
	pUserRule->bLimitSameIP=((pCMDUserRule->cbRuleMask&UR_LIMIT_SAME_IP)>0);
	pUserRule->bLimitWinRate=((pCMDUserRule->cbRuleMask&UR_LIMIT_WIN_RATE)>0);
	pUserRule->bLimitFleeRate=((pCMDUserRule->cbRuleMask&UR_LIMIT_FLEE_RATE)>0);
	pUserRule->bLimitGameScore=((pCMDUserRule->cbRuleMask&UR_LIMIT_GAME_SCORE)>0);

	//��������
	pUserRule->szPassword[0]=0;
	pUserRule->wMinWinRate=pCMDUserRule->wMinWinRate;
	pUserRule->wMaxFleeRate=pCMDUserRule->wMaxFleeRate;
	pUserRule->lMaxGameScore=pCMDUserRule->lMaxGameScore;
	pUserRule->lMinGameScore=pCMDUserRule->lMinGameScore;

	//��������
	if (wDataSize>sizeof(CMD_GR_UserRule))
	{
		//��������
		VOID * pDataBuffer=NULL;
		tagDataDescribe DataDescribe;
		CRecvPacketHelper RecvPacket(pCMDUserRule+1,wDataSize-sizeof(CMD_GR_UserRule));

		//��ȡ����
		while (true)
		{
			//��ȡ����
			pDataBuffer=RecvPacket.GetData(DataDescribe);
			if (DataDescribe.wDataDescribe==DTP_NULL) break;

			//���ݷ���
			switch (DataDescribe.wDataDescribe)
			{
			case DTP_GR_TABLE_PASSWORD:		//��������
				{
					//Ч������
					ASSERT(pDataBuffer!=NULL);
					ASSERT(DataDescribe.wDataSize<=sizeof(pUserRule->szPassword));

					//�����ж�
					ASSERT(CServerRule::IsForfendLockTable(m_pGameServiceOption->dwServerRule)==false);
					//if (CServerRule::IsForfendLockTable(m_pGameServiceOption->dwServerRule)==true) break;

					//��������
					if (DataDescribe.wDataSize<=sizeof(pUserRule->szPassword))
					{
						CopyMemory(&pUserRule->szPassword,pDataBuffer,DataDescribe.wDataSize);
						pUserRule->szPassword[CountArray(pUserRule->szPassword)-1]=0;
					}

					break;
				}
			}
		}
	}

	return true;
}

//�û��Թ�
bool CAttemperEngineSink::On_SUB_CG_User_Lookon(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_UserLookon));
	if (wDataSize!=sizeof(CMD_GR_UserLookon)) return false;

	//Ч������
	CMD_GR_UserLookon * pUserLookon=(CMD_GR_UserLookon *)pData;
	if (pUserLookon->wChairID>=m_pGameServiceAttrib->wChairCount) return false;
	if (pUserLookon->wTableID>=(WORD)m_TableFrameArray.GetCount()) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��Ϣ����
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();
	if ((wTableID==pUserLookon->wTableID)&&(wChairID==pUserLookon->wChairID)&&(cbUserStatus==US_LOOKON)) return true;

	//�û��ж�
	if (cbUserStatus==US_PLAYING)
	{
		SendRequestFailure(pIServerUserItem,TEXT("��������Ϸ�У���ʱ�����뿪�����Ƚ�����ǰ��Ϸ��"),0);
		return true;
	}

	//�뿪����
	if (wTableID!=INVALID_TABLE)
	{
		CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
		if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
	}

	//���´���
	CTableFrame * pTableFrame=m_TableFrameArray[pUserLookon->wTableID];
	pTableFrame->PerformLookonAction(pUserLookon->wChairID,pIServerUserItem);

	return true;
}

//�û�����
bool CAttemperEngineSink::On_SUB_CG_User_SitDown(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_UserSitDown));
	if (wDataSize!=sizeof(CMD_GR_UserSitDown)) return false;

	//Ч������
	CMD_GR_UserSitDown * pUserSitDown=(CMD_GR_UserSitDown *)pData;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��Ϣ����
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

	//�ظ��ж�
	if ((pUserSitDown->wTableID < MAX_TABLE)&&(pUserSitDown->wChairID<m_pGameServiceAttrib->wChairCount))
	{
		CTableFrame * pTableFrame=m_TableFrameArray[pUserSitDown->wTableID];
		if (pTableFrame->GetTableUserItem(pUserSitDown->wChairID)==pIServerUserItem) return true;
	}

	//�û��ж�
	if (cbUserStatus==US_PLAYING)
	{
		SendRequestFailure(pIServerUserItem,TEXT("��������Ϸ�У���ʱ�����뿪�����Ƚ�����ǰ��Ϸ��"),0);
		return true;
	}

	//�����ж�
	if(m_bStopping)
	{
		//����ʧ��
		//SendLogonFailure(TEXT("�ܱ�Ǹ������Ϸ�����Ѿ��ر��ˣ�������������룡"),LOGON_FAIL_SERVER_INVALIDATION,dwSocketID);
		return true;
	}


	//�뿪����
	if (wTableID!=INVALID_TABLE)
	{
		CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
		if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
	}

	////������
	//if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)&&(m_pGameServiceAttrib->wChairCount < MAX_CHAIR))
	//{
	//}

	//�������
	WORD wRequestTableID=pUserSitDown->wTableID;
	WORD wRequestChairID=pUserSitDown->wChairID;

	//���ӵ���
	if (wRequestTableID>=m_TableFrameArray.GetCount())
	{
		//��ʼ����
		WORD wStartTableID=0;
//		DWORD dwServerRule=m_pGameServiceOption->dwServerRule;
	//	if ((CServerRule::IsAllowAvertCheatMode(dwServerRule)==true)&&(m_pGameServiceAttrib->wChairCount<MAX_CHAIR)) wStartTableID=1;

		//��̬����
		bool bDynamicJoin=true;
		//if (m_pGameServiceAttrib->cbDynamicJoin==FALSE) bDynamicJoin=false;
	//	if (CServerRule::IsAllowDynamicJoin(m_pGameServiceOption->dwServerRule)==false) bDynamicJoin=false;

		//Ѱ��λ��
		for (WORD i=wStartTableID;i<m_TableFrameArray.GetCount();i++)
		{
			//��Ϸ״̬
			if ((m_TableFrameArray[i]->IsGameStarted()==true)&&(bDynamicJoin==false))continue;

			//��ȡ��λ
			WORD wNullChairID=m_TableFrameArray[i]->GetNullChairID();

			//�������
			if (wNullChairID!=INVALID_CHAIR)
			{
				//���ñ���
				wRequestTableID=i;
				wRequestChairID=wNullChairID;

				break;
			}
		}

		//����ж�
		if ((wRequestTableID==INVALID_CHAIR)||(wRequestChairID==INVALID_CHAIR))
		{
			SendRequestFailure(pIServerUserItem,TEXT("��ǰ��Ϸ�����Ѿ�����Ϊ���ˣ���ʱû�п������������λ�ã����Ժ����ԣ�"),0);
			return true;
		}
	}

	//���ӵ���
	if (wRequestChairID>=m_pGameServiceAttrib->wChairCount)
	{
		//Ч�����
		ASSERT(wRequestTableID<m_TableFrameArray.GetCount());
		if (wRequestTableID>=m_TableFrameArray.GetCount()) return false;

		//���ҿ�λ
		wRequestChairID=m_TableFrameArray[wRequestTableID]->GetNullChairID();

		//����ж�
		if (wRequestChairID==INVALID_CHAIR)
		{
			SendRequestFailure(pIServerUserItem,TEXT("���ڴ���Ϸ����ʱû�п������������λ���ˣ���ѡ���������Ϸ����"),0);
			return true;
		}
	}

	//���´���
	CTableFrame * pTableFrame=m_TableFrameArray[wRequestTableID];
	pTableFrame->PerformSitDownAction(wRequestChairID,pIServerUserItem,pUserSitDown->szPassword);

	return true;
}

//�û�����
bool CAttemperEngineSink::On_SUB_CG_User_StandUp(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//���ݰ�У��
	ASSERT(wDataSize == sizeof(STR_SUB_CG_USER_STANDUP));
	if (wDataSize != sizeof(STR_SUB_CG_USER_STANDUP)) 
		return false;

	//��ȡ�û�
	WORD wBindIndex = LOWORD(dwSocketID);
	IServerUserItem *pIServerUserItem = GetBindUserItem(wBindIndex);

	//�û�Ч��
	if (pIServerUserItem == NULL) return false;

	//��������
	STR_SUB_CG_USER_STANDUP *pUserStandUp = (STR_SUB_CG_USER_STANDUP *)pData;

	//ȡ������		TODO �����ڸ��
	if( //CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule) && 
		(m_pGameServiceAttrib->wChairCount < MAX_CHAIR) )
	{
		POSITION pos = m_WaitDistributeList.GetHeadPosition();
		while(pos != NULL)
		{
			POSITION tempPos = pos;
			IServerUserItem *pUserItem = m_WaitDistributeList.GetNext(pos);
			if(pUserItem->GetUserID() == pIServerUserItem->GetUserID())
			{
				m_WaitDistributeList.RemoveAt(tempPos);
				break;
			}
		}

		if(pUserStandUp->wTableID == INVALID_TABLE) return true;
	}

	//Ч�����Ӻ�����
	if (pUserStandUp->wChairID >= m_pGameServiceAttrib->wChairCount) 
		return false;
	if (pUserStandUp->wTableID >= (WORD)m_TableFrameArray.GetCount()) 
		return false;

	//���Ӻź����Ӻ�У��
	WORD wTableID = pIServerUserItem->GetTableID();
	WORD wChairID = pIServerUserItem->GetChairID();
	if ((wTableID !=pUserStandUp->wTableID) || (wChairID != pUserStandUp->wChairID)) 
		return true;
		
	//��̬���봦��
	bool bDynamicJoin = false;
	if (wTableID != INVALID_TABLE)
	{
		//У������
		CTableFrame *pTableFrame = m_TableFrameArray[wTableID];
		if ( NULL == pTableFrame )
			return false;

		//������򣺽�һ����ģʽ��̬����
		tagTableRule *pTableCfg = (tagTableRule*)pTableFrame->GetCustomRule();
		if(pTableCfg != NULL && (pTableCfg->GameMode == TABLE_MODE_DW || pTableCfg->GameMode == TABLE_MODE_GOLD))
			bDynamicJoin = true;
	}

	//�û��ж�
	if (!bDynamicJoin && (pIServerUserItem->GetUserStatus()==US_PLAYING))
	{
		SendRequestFailure(pIServerUserItem, TEXT("��������Ϸ�У���ʱ�����뿪�����Ƚ�����ǰ��Ϸ��"), REQUEST_FAILURE_NORMAL);
		return true;
	}

	//�뿪����
	if (wTableID != INVALID_TABLE)
	{
		//�������
		CTableFrame *pTableFrame = m_TableFrameArray[wTableID];
		if (NULL == pTableFrame) 
			return true;
		//�û�վ��
		if ( !pTableFrame->PerformStandUpAction(pIServerUserItem) ) 
			return true;
	}

	return true;
}

//�����û�
bool CAttemperEngineSink::On_SUB_CG_User_InviteUser(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_GR_UserInviteReq));
	if (wDataSize!=sizeof(CMD_GR_UserInviteReq)) return false;

	//��Ϣ����
	CMD_GR_UserInviteReq * pUserInviteReq=(CMD_GR_UserInviteReq *)pData;

	//��ȡ�û�
	WORD wBindIndex = LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//Ч��״̬
	if (pUserInviteReq->wTableID==INVALID_TABLE) return true;
	if (pIServerUserItem->GetTableID()!=pUserInviteReq->wTableID) return true;
	if (pIServerUserItem->GetUserStatus()==US_PLAYING) return true;
	if (pIServerUserItem->GetUserStatus()==US_OFFLINE) return true;

	//Ŀ���û�
	IServerUserItem * pITargetUserItem=m_ServerUserManager.SearchUserItem(pUserInviteReq->dwUserID);
	if (pITargetUserItem==NULL) return true;
	if (pITargetUserItem->GetUserStatus()==US_PLAYING) return true;

	//������Ϣ
	CMD_GR_UserInvite UserInvite;
	memset(&UserInvite,0,sizeof(UserInvite));
	UserInvite.wTableID=pUserInviteReq->wTableID;
	UserInvite.dwUserID=pIServerUserItem->GetUserID();
	SendData(pITargetUserItem, MDM_USER, CMD_GC_USER_INVITE_USER, &UserInvite, sizeof(UserInvite));

	return true;
}

//�ܾ��������
bool CAttemperEngineSink::On_SUB_CG_User_RefuseSit(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_GR_UserRepulseSit));
	if (wDataSize!=sizeof(CMD_GR_UserRepulseSit)) return false;

	//��Ϣ����
	CMD_GR_UserRepulseSit * pUserRepulseSit=(CMD_GR_UserRepulseSit *)pData;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//��ȡ����
	CTableFrame * pTableFrame=m_TableFrameArray[pUserRepulseSit->wTableID];
	if (pTableFrame->IsGameStarted()==true) return true;

	//��ȡ�û�
	IServerUserItem * pRepulseIServerUserItem = pTableFrame->GetTableUserItem(pUserRepulseSit->wChairID);
	if (pRepulseIServerUserItem==NULL) return true;
	if(pRepulseIServerUserItem->GetUserID() != pUserRepulseSit->dwRepulseUserID)return true;

	//������Ϣ
	TCHAR szDescribe[256]=TEXT("");
	lstrcpyn(szDescribe,TEXT("��������������˲�����ͬ����Ϸ��"),CountArray(szDescribe));
	SendRoomMessage(pRepulseIServerUserItem,szDescribe,SMT_EJECT|SMT_CHAT|SMT_CLOSE_GAME);

	//�������
	pTableFrame->PerformStandUpAction(pRepulseIServerUserItem);

	return true;
}

//�߳��û�
bool CAttemperEngineSink::On_SUB_CG_User_KickUser(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_KickUser));
	if (wDataSize!=sizeof(CMD_GR_KickUser)) return false;

	//��������
	CMD_GR_KickUser * pKickUser=(CMD_GR_KickUser *)pData;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//Ŀ���û�
	IServerUserItem * pITargetUserItem = m_ServerUserManager.SearchUserItem(pKickUser->dwTargetUserID);
	if(pITargetUserItem==NULL) return true;

	//�û�Ч��
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->GetMemberOrder()>pITargetUserItem->GetMemberOrder()));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->GetMemberOrder()<=pITargetUserItem->GetMemberOrder())) return false;

	//Ȩ���ж�
	ASSERT(CUserRight::CanKillOutUser(pIServerUserItem->GetUserRight())==true);
	if (CUserRight::CanKillOutUser(pIServerUserItem->GetUserRight())==false) return false;

	//������Ϸ
	if(m_pGameServiceAttrib->wChairCount >= MAX_CHAIR)
	{
		//������Ϣ
		SendRoomMessage(pIServerUserItem,TEXT("�ܱ�Ǹ��������Ϸ�������ˣ�"),SMT_EJECT);
		return true;
	}

	//�û�״̬
	if(pITargetUserItem->GetUserStatus()==US_PLAYING)
	{
		//��������
		TCHAR szMessage[256]=TEXT("");
		_sntprintf_s(szMessage,CountArray(szMessage),TEXT("������� [ %s ] ������Ϸ��,�����ܽ����߳���Ϸ��"),pITargetUserItem->GetNickName());

		//������Ϣ
		SendRoomMessage(pIServerUserItem,szMessage,SMT_EJECT);
		return true;
	}

	//�����ж�
	if((pITargetUserItem->GetUserProperty()->wPropertyUseMark&PT_USE_MARK_GUARDKICK_CARD)!=0)
	{
		//��������
		DWORD dwCurrentTime=(DWORD)time(NULL);
		tagUserProperty * pUserProperty = pITargetUserItem->GetUserProperty();

		//ʱЧ�ж�
		DWORD dwValidTime=pUserProperty->PropertyInfo[2].wPropertyCount*pUserProperty->PropertyInfo[2].dwValidNum;
		if(pUserProperty->PropertyInfo[2].dwEffectTime+dwValidTime>dwCurrentTime)
		{
			//��������
			TCHAR szMessage[256]=TEXT("");
			_sntprintf_s(szMessage,CountArray(szMessage),TEXT("������� [ %s ] ����ʹ�÷��߿�,���޷������߳���Ϸ��"),pITargetUserItem->GetNickName());

			//������Ϣ
			SendRoomMessage(pIServerUserItem,szMessage,SMT_EJECT);

			return true; 
		}
		else
			pUserProperty->wPropertyUseMark &= ~PT_USE_MARK_GUARDKICK_CARD;
	}

	//��������
	WORD wTargerTableID = pITargetUserItem->GetTableID();
	if(wTargerTableID != INVALID_TABLE)
	{
		//�������
		TCHAR szMessage[64]=TEXT("");
		_sntprintf_s(szMessage,CountArray(szMessage),TEXT("���ѱ�%s�������ӣ�"),pIServerUserItem->GetNickName());

		//������Ϣ
		SendGameMessage(pITargetUserItem,szMessage,SMT_CHAT|SMT_CLOSE_GAME);

		CTableFrame * pTableFrame=m_TableFrameArray[wTargerTableID];
		if (pTableFrame->PerformStandUpAction(pITargetUserItem)==false) return true;
	}

	return true;
}

//�������λ��  TODONOW �л������߼�  WangChengQing
bool CAttemperEngineSink::OnTCPNetworkSubUserChairReq(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetBindUserItem(wBindIndex);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�û�״̬
	if(pIServerUserItem->GetUserStatus() == US_PLAYING)
	{
		//ʧ��
		m_TableFrameArray[0]->SendRequestFailure(pIServerUserItem,TEXT("��������Ϸ�У���ʱ�����뿪�����Ƚ�����ǰ��Ϸ��"),REQUEST_FAILURE_NORMAL);
		return true;
	}

	//��������
	INT nStartIndex=rand()%MAX_TABLE;
	for (int i=0;i< MAX_TABLE; i++)
	{
		//�������
		INT nTableIndex=(i+nStartIndex)%MAX_TABLE;

		//����ͬ��
		if(nTableIndex == pIServerUserItem->GetTableID())continue;

		//��ȡ����
		CTableFrame * pTableFrame=m_TableFrameArray[nTableIndex];
		if ((pTableFrame->IsGameStarted()==true)/*&&(bDynamicJoin==false)*/) continue;
		if(pTableFrame->IsTableLocked()) continue;
		if(pTableFrame->GetChairCount()==pTableFrame->GetNullChairCount()) continue;

		//��Ч����
		WORD wChairID=pTableFrame->GetRandNullChairID();
		if (wChairID==INVALID_CHAIR) continue;

		//�뿪����
		if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
		{
			CTableFrame * pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];
			if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
		}

		//�������
		WORD wTagerDeskPos = pIServerUserItem->GetMobileUserDeskPos();
		WORD wTagerDeskCount = pIServerUserItem->GetMobileUserDeskCount();

		//������Ϣ
		if((nTableIndex < wTagerDeskPos) ||(nTableIndex > (wTagerDeskPos+wTagerDeskCount-1)))
		{
			pIServerUserItem->SetMobileUserDeskPos(nTableIndex/wTagerDeskCount);
		}

		//�û�����
		pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem);
		return true;
	}

	//��������
	nStartIndex=rand()%MAX_TABLE;
	for (INT_PTR i=0;i<MAX_TABLE;i++)
	{
		//�������
		INT nTableIndex=(i+nStartIndex)%MAX_TABLE;

		//����ͬ��
		if(nTableIndex == pIServerUserItem->GetTableID())continue;

		//��ȡ����
		CTableFrame * pTableFrame=m_TableFrameArray[nTableIndex];
		if ((pTableFrame->IsGameStarted()==true)/*&&(bDynamicJoin==false)*/) continue;
		if(pTableFrame->IsTableLocked()) continue;

		//��Ч����
		WORD wChairID=pTableFrame->GetRandNullChairID();
		if (wChairID==INVALID_CHAIR) continue;

		//�뿪����
		if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
		{
			CTableFrame * pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];
			if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
		}

		//�������
		WORD wTagerDeskPos = pIServerUserItem->GetMobileUserDeskPos();
		WORD wTagerDeskCount = pIServerUserItem->GetMobileUserDeskCount();

		//������Ϣ
		if((nTableIndex < wTagerDeskPos) ||(nTableIndex > (wTagerDeskPos+wTagerDeskCount-1)))
		{
			pIServerUserItem->SetMobileUserDeskPos(nTableIndex/wTagerDeskCount);
		}

		//�û�����
		pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem);
		return true;
	}

	//����ͬ��
	if(pIServerUserItem->GetTableID() != INVALID_TABLE)
	{
		//��ȡ����
		CTableFrame * pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];
		if (pTableFrame->IsGameStarted()==false && pTableFrame->IsTableLocked()==false)
		{
			//��Ч����
			WORD wChairID=pTableFrame->GetRandNullChairID();
			if (wChairID!=INVALID_CHAIR)
			{
				//�뿪����
				if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
				{
					CTableFrame * pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];
					if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
				}

				//�û�����
				pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem);
				return true;
			}
		}
	}

	//ʧ��
	m_TableFrameArray[0]->SendRequestFailure(pIServerUserItem,TEXT("û�ҵ��ɽ������Ϸ����"),REQUEST_FAILURE_NORMAL);
	return true;
}

//���󷿼��б�
bool CAttemperEngineSink::OnTCPNetworkSubGetTableList(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_GR_GetTableList));
	if (wDataSize<sizeof(CMD_GR_GetTableList)) return false;

	//������Ϣ
	CMD_GR_GetTableList * pGetTableList = (CMD_GR_GetTableList *)pData;

	CMD_GR_TableListResult pTableList;
	ZeroMemory(&pTableList,sizeof(CMD_GR_TableListResult));


	int iCount = 20;
	for(int i=0;i < m_TableFrameArray.GetCount(),i < iCount;i++)
	{
		CTableFrame* pTableFrame = m_TableFrameArray.GetAt(i);

		if( pTableFrame->GetTableMode() == pGetTableList->GameMode)
		{
			tagTableRule* pCfg = (tagTableRule*)pTableFrame->GetCustomRule();

			//������İ���˼����򲻷���
			if(pCfg->bAllowStranger == 0)
				continue;

			if((pGetTableList->GoldRank == 0  && pCfg->CellScore <= 100)
				||(pGetTableList->GoldRank == 1  && pCfg->CellScore <= 500 && pCfg->CellScore > 100 )
				||(pGetTableList->GoldRank == 2  && pCfg->CellScore <= 5000 && pCfg->CellScore > 500 ))
			{
				pTableList.byCount++;
				pTableList.tableInfoList[i].byCurrPlayerCount =  static_cast<BYTE>(pTableFrame->GetSitUserCount());
				pTableList.tableInfoList[i].byMaxPlayerCount = pCfg->PlayerCount;
				pTableList.tableInfoList[i].dwJoinScore = pCfg->JoinScore;
				pTableList.tableInfoList[i].dwLeaveScore = pCfg->LeaveScore;

				//pTableList.tableInfoList[i].dwTablePassword = pCfg->KaiHuaGang;
				//pTableList.tableInfoList[i].QingYiSe = pCfg->QingYiSe;
				//pTableList.tableInfoList[i].QiXiaoDui = pCfg->QiXiaoDui;
				pTableList.tableInfoList[i].wCellScore = pCfg->CellScore;
				//pTableList.tableInfoList[i].XuanFengGang = pCfg->XuanFengGang;
				//pTableList.tableInfoList[i].ZhangMao = pCfg->ZhangMao;
			}
		}
	}

	if(pTableList.byCount > 0)
	{
		pTableList.GoldRank = pGetTableList->GoldRank;
		//���ͷ����б�
		SendData(dwSocketID,MDM_USER,SUB_GR_GET_TABLELIST_RESULT,&pTableList,sizeof(CMD_GR_TableListResult));	
	}

	return true;
}

//�齱����
bool CAttemperEngineSink::OnTcpNetworkQueryLottery(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//����У��
	ASSERT(wDataSize==sizeof(CMD_GR_QueryLottery));
	if(wDataSize!=sizeof(CMD_GR_QueryLottery))
		return false;

	CMD_GR_QueryLottery* pGetRankReward = (CMD_GR_QueryLottery*)pData;

	DBR_GR_QueryLottery QueryLottery;
	memcpy(&QueryLottery,pGetRankReward,sizeof(DBR_GR_QueryLottery));

	return m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR,DBR_GR_QUERY_LOTTERY,dwSocketID,&QueryLottery,sizeof(DBR_GR_QueryLottery));

}

/***************************************������Ϣ 3��*******************************************************/
//��������
bool CAttemperEngineSink::On_SUB_CG_USER_CREATE_ROOM(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��ȡ�û�
	WORD wBindIndex = LOWORD(dwSocketID);
	IServerUserItem *pIServerUserItem = GetBindUserItem(wBindIndex);

	//�û�У��
	if (pIServerUserItem==NULL) 
	{
		return false;
	}

	//У�����ݰ�
	if(wDataSize != sizeof(STR_SUB_CG_USER_CREATE_ROOM))
	{
		SendRequestFailure(pIServerUserItem,TEXT("�����������ݴ�С��ƥ�䣡"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//�������
	STR_SUB_CG_USER_CREATE_ROOM *pCreateRoom = (STR_SUB_CG_USER_CREATE_ROOM *)pData;

	//תΪ���ӹ���
	tagTableRule *pCfg = (tagTableRule*)pCreateRoom->CommonRule;
	if ( NULL == pCfg )
	{
		return false;
	}
	
	//��������
	bool bRet = true;
	if(0 == pCfg->byClubCreate)//������ͨ����
	{
		bRet = CreateTableNormal(pCfg, pIServerUserItem, pCreateRoom);
	}
	else if(1 == pCfg->byClubCreate || 2 == pCfg->byClubCreate)//��������Ȧ����
	{
		bRet = CreateRoomClub(pCfg, pIServerUserItem, pCreateRoom);
	}
	else
	{
		TCHAR szString[512]=TEXT("");
		_sntprintf_s(szString,CountArray(szString),
			TEXT("���������䡿�����ڸ����͵�����"), 
			pCfg->byClubCreate);
		CTraceService::TraceString(szString,TraceLevel_Info);
	}

	return bRet;
}

//��������
bool CAttemperEngineSink::On_SUB_CG_USER_CREATE_TABLE(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��ȡ�û�
	WORD wBindIndex = LOWORD(dwSocketID);
	IServerUserItem *pIServerUserItem = GetBindUserItem(wBindIndex);

	//�û�У��
	if (pIServerUserItem==NULL) return false;

	//У�����ݰ�
	if(wDataSize != sizeof(STR_SUB_CG_USER_CREATE_TABLE))
	{
		SendRequestFailure(pIServerUserItem,TEXT("�����������ݴ�С��ƥ�䣡"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//�ṹ�年��
	STR_SUB_CG_USER_CREATE_TABLE *pCreateRoom = (STR_SUB_CG_USER_CREATE_TABLE *)pData;

	//Ͷ������
	return m_pIDBCorrespondManager->PostDataBaseRequest(pCreateRoom->dwUserID, DBR_CG_CLUB_CREATE_TABLE, dwSocketID, pData, wDataSize);
}

//������ͨ����
bool CAttemperEngineSink::CreateTableNormal(tagTableRule * pCfg, IServerUserItem *pIServerUserItem, STR_SUB_CG_USER_CREATE_ROOM* pCreateRoom)
{
	//��������Ʊ
	if(!CheckCreateTableTicket(pCfg, pIServerUserItem))
	{
		return true; //TODONOW ���Ϊfalse �ͻ��˾Ͷ��������ˣ� ֮���޸ĵ�
	} 

	//�û�Ч��
	if (INVALID_CHAIR != pIServerUserItem->GetChairID()) 
	{
		SendRequestFailure(pIServerUserItem, TEXT("������Ϸ��,�޷��������䣡"), REQUEST_FAILURE_NORMAL);
		return true;//TODONOW ���Ϊfalse �ͻ��˾Ͷ��������ˣ� ֮���޸ĵ�
	}
	if(INVALID_TABLE != pIServerUserItem->GetTableID())
	{
		SendRequestFailure(pIServerUserItem, TEXT("���ڷ�����,�޷��������䣡"), REQUEST_FAILURE_NORMAL);
		return true;//TODONOW ���Ϊfalse �ͻ��˾Ͷ��������ˣ� ֮���޸ĵ�
	}

	//Ѱ�ҿ��з���
	CTableFrame *pCurrTableFrame = GetNextEmptyTable();

	//�����ж�
	if(NULL == pCurrTableFrame)
	{
		SendRequestFailure(pIServerUserItem,TEXT("��������������,���Ժ����ԣ�"),REQUEST_FAILURE_NORMAL);
		return true ; //TODONOW ���Ϊfalse �ͻ��˾Ͷ��������ˣ� ֮���޸ĵ�
	}

	//����ͨ�÷������  
	pCurrTableFrame->SetCommonRule(pCfg);

	//��������Ϸ�������
	tagTableSubGameRule *pSubGameCfg = (tagTableSubGameRule*)pCreateRoom->SubGameRule;
	if(pSubGameCfg != NULL)
	{
		pCurrTableFrame->SetSubGameRule(pSubGameCfg);
	}

	//�����˿���
	if(1 == pCfg->bCreateToOther)
	{	
		//TODO �����˿���д���ݿ����������ﴦ��
		HandleCreateTableForOthers(pCurrTableFrame, pIServerUserItem, pCfg);
	}
	else	//��������
	{
		//����ʧ��, ��return false
		if( !HandleCreateTable(pCurrTableFrame, pIServerUserItem, pCfg) )
		{
			return false;
		}
	}

	//���ͷ�����Ϣ
	STR_CMD_GC_USER_CREATE_ROOM_SUCCESS nCreate;
	nCreate.dwPassword = pCurrTableFrame->GetPassword();
	nCreate.wChairID = pIServerUserItem->GetChairID();
	nCreate.byAllGameCount = pCfg->GameCount;
	nCreate.byGameMode = pCfg->GameMode;
	nCreate.byZhuangType = pCfg->RobBankType;
	//nCreate.byMask = 0;

	//���ʹ�������ɹ���Ϣ
	SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_CREATE_ROOM_SUCCESS, &nCreate, sizeof(STR_CMD_GC_USER_CREATE_ROOM_SUCCESS));	

	return true;
}

//��������Ȧ����
bool CAttemperEngineSink::CreateRoomClub(tagTableRule * pCfg, IServerUserItem *pIServerUserItem, STR_SUB_CG_USER_CREATE_ROOM* pCreateRoom)
{
	/* �ڶ���  У���û� 1.δ������������  2.δ�������� */
	/* added by WangChengQing 2018/5/11  
	** ����Ȧ���������ʱ��, ��Ϊ�����߲���Ҫ����������, ���Բ���Ҫ������ж�
	//�û�Ч��
	if (INVALID_CHAIR != pIServerUserItem->GetChairID()) 
	{
		SendRequestFailure(pIServerUserItem, TEXT("������Ϸ��,�޷��������䣡"), REQUEST_FAILURE_NORMAL);
		return false;
	}
	if(INVALID_TABLE != pIServerUserItem->GetTableID())
	{
		SendRequestFailure(pIServerUserItem, TEXT("���ڷ�����,�޷��������䣡"), REQUEST_FAILURE_NORMAL);
		return false;
	}
	*/

	/* ������  Ѱ�ҿ������� */
	CTableFrame *pCurrTableFrame = GetNextEmptyTable();
	if(NULL == pCurrTableFrame)
	{
		SendRequestFailure(pIServerUserItem,TEXT("��������������,���Ժ�����"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* ���Ĳ� �������������� */
	//�����������룬����� = ǰ�� + TABLEID
	srand(static_cast<unsigned int >(time(NULL)));
	DWORD dwPassword = GenerateTablePassword();

	//������������
	//pCurTableFrame->SetTableOwner(pIServerUserItem->GetUserID());
	pCurrTableFrame->SetPassword(dwPassword);

	//���÷����Զ���ɢ��Ĭ��һ���� -- ������ָ����ʼ��Ϸ �Զ�һ���Ӻ��ɢ
	pCurrTableFrame->SetTableAutoDismiss();
	
	//��ÿ�����
	WORD wChairID = pCurrTableFrame->GetNullChairID(); 

	/* ���岽 ���÷������ */
	//����ͨ�÷������  
	pCurrTableFrame->SetCommonRule(pCfg);

	//��������Ϸ�������
	tagTableSubGameRule *pSubGameCfg = (tagTableSubGameRule*)pCreateRoom->SubGameRule;
	if(pSubGameCfg != NULL)
	{
		pCurrTableFrame->SetSubGameRule(pSubGameCfg);
	}

	/* ������ ������Ϣ���浽���ݿ� */
	STR_DBR_CLUB_ROOM_INFO Dbr;
	Dbr.dwUserID = pCfg->dwUserID;
	Dbr.dwClubID = pCfg->dwClubID;
	Dbr.dwGameID = (pCfg->dwKindID) << 16;
	Dbr.byModeID = 0;

	if(1 == pCfg->byGoldOrFK)//���ֲ��ķ����� ������ͨ������
	{
		Dbr.byModeID  = 0;
	}
	else if( 2 == pCfg->byGoldOrFK)//���ֲ��Ľ�ҳ� ��ʵ�� ������ҳ�
	{
		Dbr.byModeID  = 3;
	}

	//Dbr.szKindName ; ���ݿ��Լ���ѯ
	Dbr.dwServiceGold = pCfg->dwAmount;
	Dbr.dwRevenue = pCfg->dwOwnerPercentage;
	Dbr.byMask = pCfg->byMask;
	Dbr.dwDizhu = pCfg->dwDizhu;
	Dbr.dwGold = pCfg->dwLevelGold;
	Dbr.byAllRound = pCfg->GameCount;
	Dbr.byChairNum = pCfg->PlayerCount;
	Dbr.DissolveMinute = pCfg->bDissolve;
	memcpy(Dbr.szRealRoomRule, pCfg, 256);

	WriteClubRoomToDB(&Dbr);


	//���ͷ�����Ϣ
	STR_CMD_GC_USER_CREATE_ROOM_SUCCESS nCreate;

	//added by WangChengQing 2018/9/11
	//�ͻ��˻����tableID(��password) �Զ����뷿�� -- һ�����
	//��˴˴� ������tableID
	nCreate.dwPassword = 0;
	nCreate.wChairID = pIServerUserItem->GetChairID();
	nCreate.byAllGameCount = pCfg->GameCount;
	nCreate.byGameMode = pCfg->GameMode;
	nCreate.byZhuangType = pCfg->RobBankType;
	//nCreate.byMask = 0;

	//���ʹ�������ɹ���Ϣ
	SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_CREATE_ROOM_SUCCESS, &nCreate, sizeof(STR_CMD_GC_USER_CREATE_ROOM_SUCCESS));	

	//TODONOWW ��Ҫ���͸�Э����, Ȼ��Э���� ���͸���¼��.  ��¼��֪ͨ�ͻ���ʵʱˢ�¾��ֲ�����
	STR_CMD_LC_CLUB_ROOM_RE RECMD;
	RECMD.dwClubID = pCfg->dwClubID;
	SendData(pIServerUserItem, MDM_CLUB, CMD_LC_CLUB_ROOM_RE, &RECMD, sizeof(STR_CMD_LC_CLUB_ROOM_RE));	
	
	//added by WangChengQing 2018/7/2   ����Ȧ�����������֮��, Ӧ�öϿ���client������
	//added by WangChengQing 2018/8/31  �˴��ɿͻ��������Ͽ�socket����
	return true; 
}

//��������Ȧ����
bool CAttemperEngineSink::CreateTableClub(STR_DBO_GC_CLUB_CREATE_TABLE * pDbo, IServerUserItem *pIServerUserItem)
{
	//�û�Ч��
	if (INVALID_CHAIR != pIServerUserItem->GetChairID()) 
	{
		SendRequestFailure(pIServerUserItem, TEXT("���ӺŲ�Ϊ��,�޷���������Ȧ����"), REQUEST_FAILURE_NORMAL);
		return false;
	}
	if(INVALID_TABLE != pIServerUserItem->GetTableID())
	{
		SendRequestFailure(pIServerUserItem, TEXT("���ӺŲ�Ϊ��,�޷���������Ȧ����"), REQUEST_FAILURE_NORMAL);
		return false;
	}
	
	/* ��0��  �жϹ��᷿���Ƿ��㹻 */
	if(pDbo->lResultCode != 0)
	{
		SendRequestFailure(pIServerUserItem,TEXT("���᷿������, ����ϵ����Ա��"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	STR_SUB_CG_USER_CREATE_ROOM CreateRoom = pDbo->strCreateRoom;
	tagTableRule *pCfg = (tagTableRule*)CreateRoom.CommonRule;

	/* ��һ�� Ѱ�ҿ��з��� */
	CTableFrame *pCurrTableFrame = GetNextEmptyTable();

	//�����ж�
	if(NULL == pCurrTableFrame)
	{
		SendRequestFailure(pIServerUserItem,TEXT("����������,���Ժ����ԣ�"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* �ڶ��� �������� */
	srand(static_cast<unsigned int >(time(NULL)));
	DWORD dwPassword = GenerateTablePassword();

	//������������
	//pCurTableFrame->SetTableOwner(pIServerUserItem->GetUserID());
	pCurrTableFrame->SetPassword(dwPassword);

	//���÷����Զ���ɢ��Ĭ��һ���� -- ������ָ����ʼ��Ϸ �Զ�һ���Ӻ��ɢ
	pCurrTableFrame->SetTableAutoDismiss();

	/* ����������Ϣд�����ݿ� && ���͸��ͻ���(����Ϣ�ŵĺ��������з���) */
	DWORD dwClubRoomID = pDbo->dwClubRoomID;
	DWORD dwUserID = pIServerUserItem->GetUserID();
	DWORD dwTableID = pCurrTableFrame->GetPassword();
    DWORD dwLockState = (pDbo->byTableMode == 2) ? 0 : 1;
	ClubTableCreate(dwClubRoomID, dwUserID, dwTableID, dwLockState);

	/* ���÷������ */
	//����ͨ�÷������  
	pCurrTableFrame->SetCommonRule(pCfg);

	//��������Ϸ�������
	tagTableSubGameRule *pSubGameCfg = (tagTableSubGameRule*)(CreateRoom.SubGameRule);
	if(pSubGameCfg != NULL)
	{
		pCurrTableFrame->SetSubGameRule(pSubGameCfg);
	}

	/* �û����� */
	WORD wChairID = pCurrTableFrame->GetNullChairID(); 
	//���Ӻ���Ч
	if (INVALID_CHAIR != wChairID)
	{
		//�û�����		TODO �����ÿͻ�����������
		if(pCurrTableFrame->PerformSitDownAction(wChairID, pIServerUserItem, 0, true)==false)
		{
			SendRequestFailure(pIServerUserItem, TEXT("�û�����ʧ��!"), REQUEST_FAILURE_NORMAL);
			return false;
		}		
	}
	else
	{
		SendRequestFailure(pIServerUserItem, TEXT("��������,�޷����뷿��!"), REQUEST_FAILURE_NORMAL);
		return false;
	}

	
	/* ���͸�client��������ɹ���Ϣ */
	STR_CMD_GC_USER_CREATE_ROOM_SUCCESS nCreate;
	nCreate.dwPassword = pCurrTableFrame->GetPassword();
	nCreate.wChairID = pIServerUserItem->GetChairID();
	nCreate.byAllGameCount = pCfg->GameCount;
	nCreate.byGameMode = pCfg->GameMode;
	nCreate.byZhuangType = pCfg->RobBankType;

	//���ʹ�������ɹ���Ϣ
	SendData(pIServerUserItem, MDM_USER, CMD_GC_CLUB_CREATE_TABKE, &nCreate, sizeof(STR_CMD_GC_USER_CREATE_ROOM_SUCCESS));	

	return true;
}

//�������� ��Ҵ�������
bool CAttemperEngineSink::CreateTableHallGold(STR_DBO_CG_USER_JOIN_TABLE_HALL_GOLD * pDbo, IServerUserItem *pIServerUserItem)
{
	//�ڲ�ʹ��, ��У��ָ��
	tagTableRule *pCfg = (tagTableRule*)pDbo->strCreateRoom.CommonRule;


	//��������Ʊ
	if(!CheckCreateTableTicket(pCfg, pIServerUserItem))
	{
		return true; //TODONOW ���Ϊfalse �ͻ��˾Ͷ��������ˣ� ֮���޸ĵ�
	}

	/* ��һ�� Ѱ�ҿ��з��� */
	CTableFrame *pCurrTableFrame = GetNextEmptyTable();

	//�����ж�
	if(NULL == pCurrTableFrame)
	{
		SendRequestFailure(pIServerUserItem,TEXT("����������,���Ժ����ԣ�"),REQUEST_FAILURE_NORMAL);
		return true; //TODONOW ���Ϊfalse �ͻ��˾Ͷ��������ˣ� ֮���޸ĵ�
	}

	/* �ڶ��� �������� */
	srand(static_cast<unsigned int >(time(NULL)));
	DWORD dwPassword = GenerateTablePassword();

	//������������
	//pCurTableFrame->SetTableOwner(pIServerUserItem->GetUserID());
	pCurrTableFrame->SetPassword(dwPassword);

	//���÷����Զ���ɢ��Ĭ��һ���� -- ������ָ����ʼ��Ϸ �Զ�һ���Ӻ��ɢ
	pCurrTableFrame->SetTableAutoDismiss();

	/* ����������Ϣд�����ݿ� && ���͸��ͻ���(����Ϣ�ŵĺ��������з���) */
	DWORD dwUserID = pIServerUserItem->GetUserID();
	DWORD dwKindID = pDbo->dwKindID;
	BYTE  byGameType = pDbo->byGameType;
	DWORD dwTableID = pCurrTableFrame->GetPassword();
	HallTableCreate(dwUserID, dwKindID, byGameType, dwTableID);

	/* ���÷������ */
	//����ͨ�÷������  
	pCurrTableFrame->SetCommonRule(pCfg);

	//��������Ϸ�������
	tagTableSubGameRule *pSubGameCfg = (tagTableSubGameRule*)(pDbo->strCreateRoom.SubGameRule);
	if(pSubGameCfg != NULL)
	{
		pCurrTableFrame->SetSubGameRule(pSubGameCfg);
	}

	/* �û����� */
	WORD wChairID = pCurrTableFrame->GetNullChairID(); 
	//���Ӻ���Ч
	if (INVALID_CHAIR != wChairID)
	{
		//�û�����		TODO �����ÿͻ�����������
		if(pCurrTableFrame->PerformSitDownAction(wChairID, pIServerUserItem, 0, true)==false)
		{
			SendRequestFailure(pIServerUserItem, TEXT("�û�����ʧ��!"), REQUEST_FAILURE_NORMAL);
			return true; //TODONOW ���Ϊfalse �ͻ��˾Ͷ��������ˣ� ֮���޸ĵ�
		}		
	}
	else
	{
		SendRequestFailure(pIServerUserItem, TEXT("��������,�޷����뷿��!"), REQUEST_FAILURE_NORMAL);
		return true ;//TODONOW ���Ϊfalse �ͻ��˾Ͷ��������ˣ� ֮���޸ĵ�
	}
	
	/* ���͸�client��������ɹ���Ϣ 
	TODONOW ��Ϊ�ͻ�����Ҫ��ת����, �����ɹ���������ת, ���Դ˴����ͼ���ɹ�����Ϣ added by WangChengQing
	*/
	STR_CMD_GC_USER_JOIN_ROOM_SUCCESS CMD;
	memcpy(&CMD.strTableRule, pDbo->strCreateRoom.CommonRule, sizeof(CMD.strTableRule));

	CMD.dwRoomID = pCurrTableFrame->GetPassword();
	CMD.wChairID = pIServerUserItem->GetChairID();
	CMD.byAllGameCount = pCfg->GameCount;
	CMD.byGameMode = pCfg->GameMode;
	CMD.byZhuangType = pCfg->RobBankType;
	CMD.wPlayerCount = pCfg->PlayerCount;

	//���ͼ��뷿������ɹ�
	SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_JOIN_ROOM_SUCCESS, &CMD, sizeof(CMD));

	
	return true;
}


//�������� ���ֲ�����
bool CAttemperEngineSink::CreateTableAutoClub(STR_DBO_CG_USER_JOIN_TABLE_NO_PASS * pDbo, IServerUserItem *pIServerUserItem)
{
	//�ڲ�ʹ��, ��У��ָ��
	tagTableRule *pCfg = (tagTableRule*)pDbo->strCreateRoom.CommonRule;

	/* ��һ�� Ѱ�ҿ��з��� */
	CTableFrame *pCurrTableFrame = GetNextEmptyTable();

	//�����ж�
	if(NULL == pCurrTableFrame)
	{
		SendRequestFailure(pIServerUserItem,TEXT("����������,���Ժ����ԣ�"),REQUEST_FAILURE_NORMAL);
		return true; //TODONOW ���Ϊfalse �ͻ��˾Ͷ��������ˣ� ֮���޸ĵ�
	}

	/* �ڶ��� �������� */
	srand(static_cast<unsigned int >(time(NULL)));
	DWORD dwPassword = GenerateTablePassword();

	//������������
	//pCurTableFrame->SetTableOwner(pIServerUserItem->GetUserID());
	pCurrTableFrame->SetPassword(dwPassword);

	//���÷����Զ���ɢ��Ĭ��һ���� -- ������ָ����ʼ��Ϸ �Զ�һ���Ӻ��ɢ
	pCurrTableFrame->SetTableAutoDismiss();

	/* ����������Ϣд�����ݿ� && ���͸��ͻ���(����Ϣ�ŵĺ��������з���) */
	DWORD dwClubRoomID = pDbo->dwClubRoomID;
	DWORD dwUserID = pIServerUserItem->GetUserID();
	DWORD dwTableID = pCurrTableFrame->GetPassword();
    DWORD dwLockState =  1; //Ĭ��Ϊ��ͨ����(������˽������)
	ClubTableCreate(dwClubRoomID, dwUserID, dwTableID, dwLockState);


	/* ���÷������ */
	//����ͨ�÷������  
	pCurrTableFrame->SetCommonRule(pCfg);

	//��������Ϸ�������
	tagTableSubGameRule *pSubGameCfg = (tagTableSubGameRule*)(pDbo->strCreateRoom.SubGameRule);
	if(pSubGameCfg != NULL)
	{
		pCurrTableFrame->SetSubGameRule(pSubGameCfg);
	}

	/* �û����� */
	WORD wChairID = pCurrTableFrame->GetNullChairID(); 
	//���Ӻ���Ч
	if (INVALID_CHAIR != wChairID)
	{
		//�û�����		TODO �����ÿͻ�����������
		if(pCurrTableFrame->PerformSitDownAction(wChairID, pIServerUserItem, 0, true)==false)
		{
			SendRequestFailure(pIServerUserItem, TEXT("�û�����ʧ��!"), REQUEST_FAILURE_NORMAL);
			return true; //TODONOW ���Ϊfalse �ͻ��˾Ͷ��������ˣ� ֮���޸ĵ�
		}		
	}
	else
	{
		SendRequestFailure(pIServerUserItem, TEXT("��������,�޷����뷿��!"), REQUEST_FAILURE_NORMAL);
		return true; //TODONOW ���Ϊfalse �ͻ��˾Ͷ��������ˣ� ֮���޸ĵ�
	}
	
	/* ���͸�client��������ɹ���Ϣ 
	TODONOW ��Ϊ�ͻ�����Ҫ��ת����, �����ɹ���������ת, ���Դ˴����ͼ���ɹ�����Ϣ added by WangChengQing
	*/
	STR_CMD_GC_USER_JOIN_ROOM_SUCCESS CMD;
	memcpy(&CMD.strTableRule, pDbo->strCreateRoom.CommonRule, sizeof(CMD.strTableRule));

	CMD.dwRoomID = pCurrTableFrame->GetPassword();
	CMD.wChairID = pIServerUserItem->GetChairID();
	CMD.byAllGameCount = pCfg->GameCount;
	CMD.byGameMode = pCfg->GameMode;
	CMD.byZhuangType = pCfg->RobBankType;
	CMD.wPlayerCount = pCfg->PlayerCount;

	//���ͼ��뷿������ɹ�
	SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_JOIN_ROOM_SUCCESS, &CMD, sizeof(CMD));

	
	return true;
}


//Club����Ȧ������Ϣ д�����ݿ�
bool CAttemperEngineSink::WriteClubRoomToDB(STR_DBR_CLUB_ROOM_INFO* pTableInfo)
{
	//��Ч����
	if (pTableInfo==NULL) return false;

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR, DBR_CLUB_ROOM_INFO, 0, pTableInfo, sizeof(STR_DBR_CLUB_ROOM_INFO));

	return true;
}

//�������� -- ��Ҫ����
bool CAttemperEngineSink::On_SUB_User_JoinFkRoom(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У���û�
	WORD wBindIndex = LOWORD(dwSocketID);
	IServerUserItem *pIServerUserItem = GetBindUserItem(wBindIndex);
	if (NULL == pIServerUserItem) return false;

	//У�����ݰ�
	if(wDataSize != sizeof(STR_SUB_CG_USER_JOIN_FK_ROOM))
	{
		SendRequestFailure(pIServerUserItem,TEXT("��������Ȧ�������ݴ���"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//���ݶ���
	STR_SUB_CG_USER_JOIN_FK_ROOM *pJoin = (STR_SUB_CG_USER_JOIN_FK_ROOM *)pData;

	//�����ݿ��ѯ
	STR_DBR_JOIN_ROOM DBR;
	DBR.dwTableID = pJoin->dwPassword;
	DBR.dwUserID = pIServerUserItem->GetUserID();

	//Ͷ������
	return m_pIDBCorrespondManager->PostDataBaseRequest(DBR.dwUserID, DBR_CG_JOIN_TABLE, dwSocketID, &DBR,sizeof(DBR));
}

//�������� ���� -- ��Ҫ����
bool CAttemperEngineSink::On_CMD_GC_JOIN_TABLE( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	/* 1. У���û� */
	WORD wBindIndex = LOWORD(dwContextID);
	IServerUserItem *pIServerUserItem = GetBindUserItem(wBindIndex);
	if (NULL == pIServerUserItem) return false;

	/* 2. У�����ݰ� */
	if(wDataSize != sizeof(STR_DBR_JOIN_ROOM))
	{
		SendRequestFailure(pIServerUserItem,TEXT("���뷿�����ݴ���"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 3. ���ݿ�У�� */
	STR_DBR_JOIN_ROOM *pJoin = (STR_DBR_JOIN_ROOM *)pData;
	if(pJoin->lResultCode != 0)
	{
		SendRequestFailure(pIServerUserItem,TEXT("�ǹ����Ա, ���ܽ��빤������"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 4. ����У�� */
	DWORD dwPassword = pJoin->dwTableID;
	if(dwPassword == 0)
	{
		SendRequestFailure(pIServerUserItem, TEXT("����Ŵ���,�����³���"), REQUEST_FAILURE_PASSWORD);
		return false;
	}

	/* 5. У���Ƿ���֮ǰ������ */	
	WORD wOldTableID = pIServerUserItem->GetTableID();	
	if(wOldTableID != INVALID_TABLE)
	{
		if(wOldTableID > m_TableFrameArray.GetCount())
		{
			wOldTableID = INVALID_TABLE;
		}
		else
		{
			CTableFrame* pOldTable = m_TableFrameArray.GetAt(wOldTableID);
			if(pOldTable == NULL || pOldTable->GetGameStatus() != GAME_STATUS_PLAY)
			{
				wOldTableID = INVALID_TABLE;
			}
		}	
	}

	//����֮ǰ�ķ�����(����Ӧ�ò����߽���, ��Ϊ�ж��������Ĵ���)
	if(INVALID_TABLE != wOldTableID)
	{
		SendRequestFailure(pIServerUserItem,TEXT("���Ѿ�����Ϸ��,���ܽ�����������!"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 6. Ѱ��ָ������ */
	CTableFrame *pCurrTableFrame = GetDesignatedTable(dwPassword);
	//�����ж�
	if(NULL == pCurrTableFrame)
	{
		SendRequestFailure(pIServerUserItem, TEXT("���Ӳ�����"), REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 7. �������У�� */
	tagTableRule *pCfg = (tagTableRule*)pCurrTableFrame->GetCustomRule();
	if (NULL == pCfg)
	{
		SendRequestFailure(pIServerUserItem, TEXT("����ʧ��, ������򲻴���"), REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 8. ������Ϸ״̬ �ж� */
	if(GAME_STATUS_FREE != pCurrTableFrame->GetGameStatus())
	{
		SendRequestFailure(pIServerUserItem, TEXT("����������Ϸ��,�޷�����"), REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 9. ��ƱУ�� */
	if(!CheckJoinTableTicket(pCfg, pIServerUserItem))
	{
		return false;
	}

	/* 10. ��������ж� */
	WORD wChairID = pCurrTableFrame->GetNullChairID();//Ѱ�ҿ�����
	if (wChairID != INVALID_CHAIR)
	{
		if(pCurrTableFrame->PerformSitDownAction(wChairID, pIServerUserItem, 0, true)==false)
		{
			SendRequestFailure(pIServerUserItem,TEXT("���뷿��ʧ��, ����ʧ��"),REQUEST_FAILURE_PASSWORD);
			return false;
		}	
	}
	else
	{
		SendRequestFailure(pIServerUserItem,TEXT("��������,�������������"),REQUEST_FAILURE_NORMAL);
		return false;
	}
	
	/* 11. ���͸�client */
	STR_CMD_GC_USER_JOIN_ROOM_SUCCESS CMD;
	memcpy(&CMD.strTableRule, pCfg, sizeof(CMD.strTableRule));
	CMD.dwRoomID = dwPassword;
	CMD.wChairID = wChairID;
	CMD.byAllGameCount = pCfg->GameCount;
	CMD.byGameMode = pCfg->GameMode;
	CMD.byZhuangType = pCfg->RobBankType;
	CMD.wPlayerCount = pCfg->PlayerCount;

	//���ͼ��뷿������ɹ�
	SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_JOIN_ROOM_SUCCESS, &CMD, sizeof(CMD));

	return true;
}

//�������� -- ����Ҫ����, �����ٿ�ʼ
bool CAttemperEngineSink::On_SUB_CG_USER_JOIN_TABLE_NO_PASS(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У���û�
	WORD wBindIndex = LOWORD(dwSocketID);
	IServerUserItem *pIServerUserItem = GetBindUserItem(wBindIndex);
	if (NULL == pIServerUserItem) return false;

	//У�����ݰ�
	if(wDataSize != sizeof(STR_SUB_CG_USER_JOIN_TABLE_NO_PASS))
	{
		SendRequestFailure(pIServerUserItem,TEXT("���뷿�� �ṹ�����ݴ���"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//���ݶ���
	STR_SUB_CG_USER_JOIN_TABLE_NO_PASS *pJoin = (STR_SUB_CG_USER_JOIN_TABLE_NO_PASS *)pData;

	//Ͷ������
	return m_pIDBCorrespondManager->PostDataBaseRequest(pJoin->dwUserID, DBR_CG_USER_JOIN_TABLE_NO_PASS, dwSocketID, pData,wDataSize);
}

//�������� -- ����Ҫ����, �����ٿ�ʼ
bool CAttemperEngineSink::On_CMD_GC_USER_JOIN_TABLE_NO_PASS( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	/* 1. У���û� */
	WORD wBindIndex = LOWORD(dwContextID);
	IServerUserItem *pIServerUserItem = GetBindUserItem(wBindIndex);
	if (NULL == pIServerUserItem) return false;

	/* 2. У�����ݰ� */
	if(wDataSize != sizeof(STR_DBO_CG_USER_JOIN_TABLE_NO_PASS))
	{
		SendRequestFailure(pIServerUserItem,TEXT("���뷿�����ݴ���"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 3. ���ݿ�У�� */

	STR_DBO_CG_USER_JOIN_TABLE_NO_PASS *pJoin = (STR_DBO_CG_USER_JOIN_TABLE_NO_PASS *)pData;
	if( pJoin->lResultCode != DB_SUCCESS)//����ֵ��Ϊ0, ����Ϊ���Ͻ�Ҳ��� ���� ���Ǳ���˾����
	{
		SendRequestFailure(pIServerUserItem,TEXT("δ�ҵ����ʷ���, ���Ժ�����"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	if( pJoin->lResultCode2 != DB_SUCCESS)//����ֵ��Ϊ0, �򹤻᷿������
	{
		SendRequestFailure(pIServerUserItem,TEXT("���᷿������, ����ϵ����Ա��ֵ"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	if( pJoin->dwPassword == 0)//���Ӻ�Ϊ��, ����Ϊû���ҵ���Ӧ������
	{
		//���û���ҵ����ֲ�������, ��ֱ�Ӵ���һ������
		return CreateTableAutoClub(pJoin ,  pIServerUserItem);
	}


	/* 4. ����У�� */
	DWORD dwPassword = pJoin->dwPassword;
	if(dwPassword == 0)
	{
		SendRequestFailure(pIServerUserItem, TEXT("���ӺŴ���,�����³���"), REQUEST_FAILURE_PASSWORD);
		return false;
	}

	/* 5. У���Ƿ���֮ǰ����Ϸ�� */
	WORD wOldTableID = pIServerUserItem->GetTableID(); //�����Ӻ�	
	if(wOldTableID != INVALID_TABLE)
	{
		if(wOldTableID > m_TableFrameArray.GetCount())
		{
			wOldTableID = INVALID_TABLE;
		}
		else
		{
			CTableFrame* pOldTable = m_TableFrameArray.GetAt(wOldTableID);
			if(pOldTable == NULL || pOldTable->GetGameStatus() != GAME_STATUS_PLAY)
			{
				wOldTableID = INVALID_TABLE;
			}
		}	
	}

	if (INVALID_TABLE != wOldTableID)
	{
		SendRequestFailure(pIServerUserItem,TEXT("���Ѿ�����Ϸ��,���ܽ�����������"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 6. Ѱ��ָ������ */
	CTableFrame *pCurrTableFrame = GetDesignatedTable(dwPassword);
	if(NULL == pCurrTableFrame)
	{
		SendRequestFailure(pIServerUserItem, TEXT("����ʧ��, ���Ӳ�����"), REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 7. ������� */
	tagTableRule * pCfg = (tagTableRule*)pCurrTableFrame->GetCustomRule();
	if (NULL == pCfg)
	{
		SendRequestFailure(pIServerUserItem, TEXT("����ʧ��, ������򲻴���!"), REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 8. �ж������Ƿ�����Ϸ�� */
	if(GAME_STATUS_FREE != pCurrTableFrame->GetGameStatus())
	{
		SendRequestFailure(pIServerUserItem, TEXT("����������Ϸ��,�޷����룡"), REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 9. ��ƱУ�� */
	if(!CheckJoinTableTicket(pCfg, pIServerUserItem))
	{
		return false;
	}

	/* 10. �û����� */
	WORD wChairID = pCurrTableFrame->GetNullChairID();//Ѱ�ҿ�����
	if (wChairID != INVALID_CHAIR)
	{
		if(pCurrTableFrame->PerformSitDownAction(wChairID, pIServerUserItem, 0, true)==false)
		{
			SendRequestFailure(pIServerUserItem,TEXT("����ʧ��, ����ʧ��!"),REQUEST_FAILURE_PASSWORD);
			return false;
		}
	}
	else
	{
		SendRequestFailure(pIServerUserItem,TEXT("��������,�������������!"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 11. ���ͼ��뷿��ɹ���Ϣ���ͻ��� */
	STR_CMD_GC_USER_JOIN_ROOM_SUCCESS nJoin;
	ZeroMemory(&nJoin, sizeof(STR_CMD_GC_USER_JOIN_ROOM_SUCCESS));
	memcpy(&nJoin.strTableRule, pCfg, sizeof(nJoin.strTableRule));
	//��ֵ
	nJoin.dwRoomID = dwPassword;
	nJoin.wChairID = wChairID;
	nJoin.byAllGameCount = pCfg->GameCount;
	nJoin.byGameMode = pCfg->GameMode;
	nJoin.byZhuangType = pCfg->RobBankType;
	nJoin.wPlayerCount = pCfg->PlayerCount;

	//���ͼ��뷿������ɹ�
	SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_JOIN_ROOM_SUCCESS, &nJoin, sizeof(STR_CMD_GC_USER_JOIN_ROOM_SUCCESS));

	return true;
}

//�������� -- ��Ҵ�������
bool CAttemperEngineSink::On_SUB_CG_USER_JOIN_GOLD_HALL_ROOM(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У���û�
	WORD wBindIndex = LOWORD(dwSocketID);
	IServerUserItem *pIServerUserItem = GetBindUserItem(wBindIndex);
	if (NULL == pIServerUserItem) return true;

	//У�����ݰ�
	if(wDataSize != sizeof(STR_SUB_CG_USER_JOIN_GOLD_HALL_ROOM))
	{
		SendRequestFailure(pIServerUserItem,TEXT("���뷿�� �ṹ�����ݴ���"),REQUEST_FAILURE_NORMAL);
		return true;
	}

	//���ݶ���
	STR_SUB_CG_USER_JOIN_GOLD_HALL_ROOM *pJoin = (STR_SUB_CG_USER_JOIN_GOLD_HALL_ROOM *)pData;

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(pJoin->dwUserID, DBR_CG_USER_JOIN_TABLE_HALL_GOLD, dwSocketID, pData,wDataSize);

	return true;
}

//�������� -- ��Ҵ������� ����
bool CAttemperEngineSink::On_CMD_GC_USER_JOIN_TABLE_HALL_GOLD( DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	/* 1. У���û� */
	WORD wBindIndex = LOWORD(dwContextID);
	IServerUserItem *pIServerUserItem = GetBindUserItem(wBindIndex);
	if (NULL == pIServerUserItem) return false;

	/* 2. У�����ݰ� */
	if(wDataSize != sizeof(STR_DBO_CG_USER_JOIN_TABLE_HALL_GOLD))
	{
		SendRequestFailure(pIServerUserItem,TEXT("���뷿�����ݴ���"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 3. ���ݿ�У�� */
	STR_DBO_CG_USER_JOIN_TABLE_HALL_GOLD *pJoin = (STR_DBO_CG_USER_JOIN_TABLE_HALL_GOLD *)pData;

	if( pJoin->lResultCode != 0)//����ֵ��Ϊ0, ����Ϊ���Ͻ�Ҳ��� ���� ���Ǳ���˾����
	{
		SendRequestFailure(pIServerUserItem,TEXT("���Ͻ�Ҳ���, �޷�����"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	if( pJoin->dwPassword == 0)//���Ӻ�Ϊ��, ����Ϊû���ҵ���Ӧ������
	{
		//���û���ҵ���Ҵ���������, ��ֱ�Ӵ���һ����Ҵ���������
		return CreateTableHallGold(pJoin ,  pIServerUserItem);
	}

	/* 4. ����У�� */
	DWORD dwPassword = pJoin->dwPassword;
	if(dwPassword == 0)
	{
		SendRequestFailure(pIServerUserItem, TEXT("���ӺŴ���,�����³���"), REQUEST_FAILURE_PASSWORD);
		return false;
	}

	/* 5. У���Ƿ���֮ǰ����Ϸ�� */
	WORD wOldTableID = pIServerUserItem->GetTableID(); //�����Ӻ�	
	if(wOldTableID != INVALID_TABLE)
	{
		if(wOldTableID > m_TableFrameArray.GetCount())
		{
			wOldTableID = INVALID_TABLE;
		}
		else
		{
			CTableFrame* pOldTable = m_TableFrameArray.GetAt(wOldTableID);
			if(pOldTable == NULL || pOldTable->GetGameStatus() != GAME_STATUS_PLAY)
			{
				wOldTableID = INVALID_TABLE;
			}
		}	
	}

	if (INVALID_TABLE != wOldTableID)
	{
		SendRequestFailure(pIServerUserItem,TEXT("���Ѿ�����Ϸ��,���ܽ�����������"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 6. Ѱ��ָ������ */
	CTableFrame *pCurrTableFrame = GetDesignatedTable(dwPassword);
	if(NULL == pCurrTableFrame)
	{
		SendRequestFailure(pIServerUserItem, TEXT("����ʧ��, ���Ӳ�����"), REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 7. ������� */
	tagTableRule * pCfg = (tagTableRule*)pCurrTableFrame->GetCustomRule();
	if (NULL == pCfg)
	{
		SendRequestFailure(pIServerUserItem, TEXT("����ʧ��, ������򲻴���!"), REQUEST_FAILURE_NORMAL);
		return false;
	}
	//������ʾ TODONOW
	TCHAR szString[512]=TEXT("");
	_sntprintf_s(szString,CountArray(szString),TEXT("���� �����: %ld, ������ң�%ld, ����: %d"), 
		dwPassword,
		pCfg->dwLevelGold,
		pCfg->GameCount
		);
	CTraceService::TraceString(szString,TraceLevel_Normal);

	/* 8. �ж������Ƿ�����Ϸ�� */
	if(GAME_STATUS_FREE != pCurrTableFrame->GetGameStatus())
	{
		SendRequestFailure(pIServerUserItem, TEXT("����������Ϸ��,�޷����룡"), REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 9. ��ƱУ�� */
	if(!CheckJoinTableTicket(pCfg, pIServerUserItem))
	{
		return false;
	}

	/* 10. �û����� */
	WORD wChairID = pCurrTableFrame->GetNullChairID();//Ѱ�ҿ�����
	if (wChairID != INVALID_CHAIR)
	{
		if(pCurrTableFrame->PerformSitDownAction(wChairID, pIServerUserItem, 0, true)==false)
		{
			SendRequestFailure(pIServerUserItem,TEXT("����ʧ��, ����ʧ��!"),REQUEST_FAILURE_PASSWORD);
			return false;
		}
	}
	else
	{
		SendRequestFailure(pIServerUserItem,TEXT("��������,�������������!"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 11. ���ͼ��뷿��ɹ���Ϣ���ͻ��� */
	STR_CMD_GC_USER_JOIN_ROOM_SUCCESS nJoin;
	ZeroMemory(&nJoin, sizeof(STR_CMD_GC_USER_JOIN_ROOM_SUCCESS));
	memcpy(&nJoin.strTableRule, pCfg, sizeof(nJoin.strTableRule));
	//��ֵ
	nJoin.dwRoomID = dwPassword;
	nJoin.wChairID = wChairID;
	nJoin.byAllGameCount = pCfg->GameCount;
	nJoin.byGameMode = pCfg->GameMode;
	nJoin.byZhuangType = pCfg->RobBankType;
	nJoin.wPlayerCount = pCfg->PlayerCount;

	//���ͼ��뷿��ɹ�
	SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_JOIN_ROOM_SUCCESS, &nJoin, sizeof(STR_CMD_GC_USER_JOIN_ROOM_SUCCESS));
	return true;
}

//��������
bool CAttemperEngineSink::On_SUB_User_ReconnectRoom(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{	
	//��У�黧
	WORD wBindIndex = LOWORD(dwSocketID);
	IServerUserItem *pIServerUserItem = GetBindUserItem(wBindIndex);
	if (NULL == pIServerUserItem)
	{
		return true;
	}

	//У�����ݰ�
	if ( wDataSize != sizeof(STR_SUB_CG_USER_RECONNECT_ROOM))
	{
		return true;
	}

	//У���û����Ӻź����Ӻ�
	WORD wChairID = pIServerUserItem->GetChairID();
	WORD wOldTableID = pIServerUserItem->GetTableID();

	//�ж���ʷ�����Ƿ�����Ϸ
	if(wOldTableID != INVALID_TABLE)
	{
		if(wOldTableID > m_TableFrameArray.GetCount()) //��Ч����
		{
			wOldTableID = INVALID_TABLE;
		}
		else //�����Ѿ�����, ��wOldTableID����Ϊ��Ч
		{
			CTableFrame* pOldTable = m_TableFrameArray.GetAt(wOldTableID);
			if(pOldTable == NULL)
			{
				wOldTableID = INVALID_TABLE;
			}
		}	
	}

	//���ӺŻ����Ӻ���Ч, ֱ���˳�
	if (INVALID_TABLE == wOldTableID || INVALID_CHAIR == wChairID)
	{
		return true;
	}

	//�������������������
	STR_CMD_GC_USER_RECONNECT_ROOM ReJoinResult;
	ZeroMemory(&ReJoinResult, sizeof(STR_CMD_GC_USER_RECONNECT_ROOM));

	//�û�����
	if( m_TableFrameArray[wOldTableID]->PerformSitDownAction(wChairID, pIServerUserItem))
	{
		BYTE OldGameStatus = pIServerUserItem->GetOldGameStatus();

		if(OldGameStatus == US_READY)
		{
			pIServerUserItem->SetUserStatus(US_SIT, wOldTableID, wChairID);
		}
		else
		{
			pIServerUserItem->SetUserStatus(OldGameStatus, wOldTableID, wChairID);	
		}

		//���ͷ�����Ϣ
		tagTableRule* pRule = (tagTableRule* )m_TableFrameArray[wOldTableID]->GetCustomRule();
		ReJoinResult.retCode = 0;
		memcpy(&ReJoinResult.strTableRule, pRule, sizeof(ReJoinResult.strTableRule));
		ReJoinResult.wChairID = wChairID;
		ReJoinResult.dwPassWord = m_TableFrameArray[wOldTableID]->GetPassword();
		ReJoinResult.cbGameCount = pRule->GameCount;
		ReJoinResult.cbPlayerCount = pRule->PlayerCount;
		ReJoinResult.cbPayType = pRule->cbPayType;
		ReJoinResult.lSinglePayCost = pRule->lSinglePayCost;
		ReJoinResult.gameMod = pRule->GameMode;
	}
	else
	{
		TCHAR szDescribe[128]=TEXT("");
		_sntprintf_s(szDescribe,CountArray(szDescribe),
			TEXT("��������������%ld����ʧ�ܡ��������ʧ��"),
			pIServerUserItem->GetUserID());
		CTraceService::TraceString(szDescribe,TraceLevel_Warning);

		return true;
	}

	//��������
	SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_RECONNECT_ROOM, &ReJoinResult, sizeof(STR_CMD_GC_USER_RECONNECT_ROOM));

	//���������ɹ���ɾ��list
	if (ReJoinResult.retCode == 0)
	{
		//��Ϸ������  TODONOW
		pIServerUserItem->SetOfflineGameID(0);

		//���͸�Э����, ��ת������¼��
		tagOfflineUser data;
		data.dwUserID = pIServerUserItem->GetUserID();
		//data.dwGameID = m_pGameServiceAttrib->dwGameID; ɾ����ʱ����Ҫ���ֶ�
		data.byMask = 2; //��ʾɾ�������û�
		m_pITCPSocketService->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_USER_OFFLINE,&data,sizeof(tagOfflineUser));
	}
	return true;
}

//��������Ȧ����
bool CAttemperEngineSink::On_Sub_CG_User_CreateGroupRoom(VOID *pData, WORD wDataSize, DWORD dwSocketID)
{	
	//�û�У��
	WORD wBindIndex = LOWORD(dwSocketID);
	IServerUserItem *pIServerUserItem = GetBindUserItem(wBindIndex);	
	if (pIServerUserItem==NULL) return true;

	//���ݰ�У��
	if(wDataSize != sizeof(STR_SUB_CG_USER_CREATE_GROUP_ROOM))
		return true;

	//SUB����
	STR_SUB_CG_USER_CREATE_GROUP_ROOM *pCreateRoom = (STR_SUB_CG_USER_CREATE_GROUP_ROOM*)pData;

	//У��Ȧ���ǲ����Ѿ�����������Ȧ���䣬�������ҷ��䶼δ�����ˣ��Ͳ����ٴδ�������
	for (WORD i=0; i<m_TableFrameArray.GetCount(); i++)
	{
		//��ȡ����
		CTableFrame *pGroupTable = m_TableFrameArray[i];

		//������Ȧ���䣬������һ������δ��ʼ��Ϸ����˵�����п����Ӽ��룬����Ҫ�ٴ�������
		if ( (pGroupTable->GetGroupID() == pCreateRoom->dwGroupID) && 
			 (!pGroupTable->IsGameStarted()) )
		{
			//���ش����ɹ�
			STR_CMD_GC_USER_CREATE_GROUP_ROOM CMDCreate;
			ZeroMemory(&CMDCreate, sizeof(STR_CMD_GC_USER_CREATE_GROUP_ROOM));
			CMDCreate.dwResultCode = DB_SUCCESS;
			lstrcpyn(CMDCreate.szDescribeString, TEXT("����Ȧ���пշ���"), sizeof(CMDCreate.szDescribeString));

			//��������
			SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_CREATE_GROUP_ROOM, &CMDCreate, sizeof(STR_CMD_GC_USER_CREATE_GROUP_ROOM));	

			return true;
		}
	}

	//����DBR����
	STR_DBR_CG_USER_CREATE_GROUP_ROOM CreateGroupRoom;
	ZeroMemory(&CreateGroupRoom, sizeof(STR_DBR_CG_USER_CREATE_GROUP_ROOM));

	//��ֵ
	memcpy_s(&CreateGroupRoom, sizeof(STR_DBR_CG_USER_CREATE_GROUP_ROOM), pCreateRoom, sizeof(STR_DBR_CG_USER_CREATE_GROUP_ROOM));

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(CreateGroupRoom.dwUserID, DBR_CG_USER_CREATE_GROUP_ROOM, dwSocketID, &CreateGroupRoom, sizeof(STR_DBR_CG_USER_CREATE_GROUP_ROOM));
	
	return true;
}

//��������Ȧ���䷵��
bool CAttemperEngineSink::On_CMD_GC_User_CreateGroupRoom(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	if ((m_pNormalParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//����У��
	ASSERT(wDataSize == sizeof(STR_DBO_CG_USER_CREATE_GROUP_ROOM));
	if(wDataSize != sizeof(STR_DBO_CG_USER_CREATE_GROUP_ROOM))
		return false;

	//��ȡ�û�
	WORD wBindIndex = LOWORD(dwContextID);
	IServerUserItem *pIServerUserItem = GetBindUserItem(wBindIndex);

	//����
	STR_DBO_CG_USER_CREATE_GROUP_ROOM *pDBOGroup = (STR_DBO_CG_USER_CREATE_GROUP_ROOM *)pData;

	//���췵������
	STR_CMD_GC_USER_CREATE_GROUP_ROOM CMDCreate;
	ZeroMemory(&CMDCreate, sizeof(STR_CMD_GC_USER_CREATE_GROUP_ROOM));

	//��ֵ
	CMDCreate.dwResultCode = pDBOGroup->dwResultCode;
	lstrcpyn(CMDCreate.szDescribeString, pDBOGroup->szDescribeString, CountArray(CMDCreate.szDescribeString));
	
	//ִ��ʧ��
	if (DB_SUCCESS != pDBOGroup->dwResultCode )
	{
		SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_CREATE_GROUP_ROOM, &CMDCreate, sizeof(STR_CMD_GC_USER_CREATE_GROUP_ROOM));	
		return true;
	}

	//�������
	tagTableRule *pCfg = (tagTableRule* )pDBOGroup->rule;

	//У���û�����
	if ( pIServerUserItem->GetUserRoomCard() < pCfg->lSinglePayCost)
	{
		CMDCreate.dwResultCode = 1;
		lstrcpyn(CMDCreate.szDescribeString, TEXT("�������㣬���ֵ"), CountArray(CMDCreate.szDescribeString));

		SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_CREATE_GROUP_ROOM, &CMDCreate, sizeof(STR_CMD_GC_USER_CREATE_GROUP_ROOM));	
		return true;
	}

	//�û�����ܴ���������Ȧ������
	WORD wCreateTableNum = static_cast<WORD> (pIServerUserItem->GetUserRoomCard() / pCfg->lSinglePayCost);
	
	//Ѱ�ҿ������ӣ�Ϊ����Ȧ����̶���������
	srand(static_cast<unsigned int >(time(NULL)));
	int nTableNum = 0;
	for (WORD i=0; i<m_TableFrameArray.GetCount(); i++)
	{
		//��ȡ����
		CTableFrame *pGroupTable = m_TableFrameArray[i];

		//�����������£����Ҳ�������˿��ķ���
		if ( (pGroupTable->GetSitUserCount()==0) && 
			 (pGroupTable->GetCreateTableUser() == 0))
		{
			//����Ȧ����������
			if ( (nTableNum >= m_TableFrameArray.GetCount()) || 
				 (NUM_MAX_GOURP_CREATE_ROOM == nTableNum) ||
				 nTableNum >= wCreateTableNum)
			{
				break;
			}

			//������������	
			DWORD dwPassword = GenerateTablePassword();		
			pGroupTable->SetPassword(dwPassword);
			pGroupTable->SetGroupID(pDBOGroup->dwGroupID);			
			pGroupTable->SetCommonRule(pCfg);
			pGroupTable->SetCreateTableUser(pIServerUserItem->GetUserID());		//���ô������ID

			nTableNum++;

			//TODO ��Ҫ��ӿ�����Ϣ
		}
	}

	//���ʹ����ɹ���Ϣ
	_sntprintf_s(CMDCreate.szDescribeString, CountArray(CMDCreate.szDescribeString), TEXT("���Ѿ�����[%d]������Ȧ���䣬�۳�����[%ld]"), 
		      nTableNum, nTableNum*pCfg->lSinglePayCost);
	SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_CREATE_GROUP_ROOM, &CMDCreate, sizeof(STR_CMD_GC_USER_CREATE_GROUP_ROOM));	

	//�۳������û�����
	SCORE lDeductRoomCard = (-nTableNum)*((int)pCfg->lSinglePayCost);
	
	//
	//OnEventUserOpenRoomCard(pIServerUserItem, pIServerUserItem->GetUserID(), lDeductRoomCard);

	return true;
}

//��������Ȧ����
bool CAttemperEngineSink::On_Sub_CG_User_JoinGroupRoom(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize == sizeof(STR_SUB_CG_USER_JOIN_GROUP_ROOM));
	if (wDataSize != sizeof(STR_SUB_CG_USER_JOIN_GROUP_ROOM)) return true;

	//������Ϣ
	STR_SUB_CG_USER_JOIN_GROUP_ROOM *pJoinGroupRoom = (STR_SUB_CG_USER_JOIN_GROUP_ROOM *)pData;

	//����Ϣ
	WORD wBindIndex = LOWORD(dwSocketID);
	IServerUserItem *pIServerUserItem = GetBindUserItem(wBindIndex);
	tagBindParameter *pBindParameter = GetBindParameter(wBindIndex);

	//У���û�
	if ( (NULL == pBindParameter) || (NULL == pIServerUserItem) )
		return true;

	//У���û��ǲ�������Ϸ��
	WORD wOldTableID = pIServerUserItem->GetTableID();
	WORD wOldChairID = pIServerUserItem->GetChairID();
	BYTE cbUserStatus = pIServerUserItem->GetUserInfo()->cbUserStatus;

	//�û�����Ϸ��
	if(wOldTableID != INVALID_TABLE || 
		wOldChairID != INVALID_CHAIR ||
		cbUserStatus == US_PLAYING )
	{
		SendRequestFailure(pIServerUserItem, TEXT("���Ѿ�����Ϸ��,���ܽ�����������!"), REQUEST_FAILURE_NORMAL);
		return true;
	}

	//����DBR����
	STR_DBR_CG_USER_JOIN_GROUP_ROOM JoinRoom;
	ZeroMemory(&JoinRoom, sizeof(STR_DBR_CG_USER_JOIN_GROUP_ROOM));

	//��ֵ
	memcpy_s(&JoinRoom, sizeof(STR_DBR_CG_USER_JOIN_GROUP_ROOM), pJoinGroupRoom, sizeof(STR_DBR_CG_USER_JOIN_GROUP_ROOM));

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(JoinRoom.dwUserID, DBR_CG_USER_JOIN_GROUP_ROOM, dwSocketID, &JoinRoom, sizeof(STR_DBR_CG_USER_JOIN_GROUP_ROOM));

	return true;
}

//��������Ȧ���䷵��
bool CAttemperEngineSink::On_CMD_GC_User_JoinGroupRoom(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	if ((m_pNormalParameter+LOWORD(dwContextID))->dwSocketID!=dwContextID) return true;

	//����У��
	ASSERT(wDataSize == sizeof(STR_DBO_CG_USER_JOIN_GROUP_ROOM));
	if(wDataSize != sizeof(STR_DBO_CG_USER_JOIN_GROUP_ROOM))
		return false;

	//��ȡ�û�
	WORD wBindIndex = LOWORD(dwContextID);
	IServerUserItem *pIServerUserItem = GetBindUserItem(wBindIndex);
	if ( NULL == pIServerUserItem )
	{
		return false;
	}

	//����
	STR_DBO_CG_USER_JOIN_GROUP_ROOM *pDBOGroup = (STR_DBO_CG_USER_JOIN_GROUP_ROOM *)pData;

	//ʧ�ܷ���
	if ( DB_SUCCESS != pDBOGroup->dwResultCode )
	{
		SendRequestFailure(pIServerUserItem, pDBOGroup->szDescribeString, pDBOGroup->dwResultCode);
		return true;
	}

	//�жϸ�Ȧ�ӵķ����Ƿ��п����ӵ�����
	bool bHaveRoom = false;
	for(int i= 0;i < m_TableFrameArray.GetCount();i++)
	{
		CTableFrame *pTableFrame = m_TableFrameArray.GetAt(i);
		//У������
		if( (NULL != pTableFrame) && 
			(pTableFrame->GetGroupID() == pDBOGroup->dwGroupID) && 
			(GAME_STATUS_FREE == pTableFrame->GetGameStatus()) && 
			(0 != pTableFrame->GetPassword()) )
		{
			//�ҵ����䣬�ж��Ƿ��п�λ��
			WORD wChairID = pTableFrame->GetNullChairID();
			if(wChairID != INVALID_CHAIR)
			{
				//�û�����
				if(pTableFrame->PerformSitDownAction(wChairID, pIServerUserItem, 0, true)==false)
				{
					SendRequestFailure(pIServerUserItem, TEXT("����Ų���!"), REQUEST_FAILURE_PASSWORD);
					return true;
				}	

				//�û����Լ�������Ȧ����
				bHaveRoom = true;

				//��ȡ�������
				tagTableRule *pSelfOption = (tagTableRule* )pTableFrame->GetCustomRule();

				//�������ɹ���Ϣ
				STR_CMD_GC_USER_JOIN_ROOM_SUCCESS nJoin;
				ZeroMemory(&nJoin, sizeof(STR_CMD_GC_USER_JOIN_ROOM_SUCCESS));
				memcpy(&nJoin.strTableRule, pSelfOption, sizeof(nJoin.strTableRule));
				//��ֵ
				nJoin.dwRoomID = pTableFrame->GetPassword();
				nJoin.wChairID = wChairID;
				nJoin.byAllGameCount = pSelfOption->GameCount;
				nJoin.byGameMode = pSelfOption->GameMode;
				nJoin.byZhuangType = pSelfOption->RobBankType;
				nJoin.wPlayerCount = pSelfOption->PlayerCount;

				//���ͼ���ɹ���Ϣ
				SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_JOIN_ROOM_SUCCESS, &nJoin, sizeof(STR_CMD_GC_USER_JOIN_ROOM_SUCCESS));
				break;
			}
		}
	}

	//�û��޷����룬���ʹ�����ʾ
	if ( !bHaveRoom )
	{
		SendRequestFailure(pIServerUserItem, TEXT("����Ȧ���������򲻴����ѿ����䣬����ϵ����"), REQUEST_FAILURE_PASSWORD);
	}

	return true;
}

//�����ҷ���
bool CAttemperEngineSink::On_SUB_User_JoinGoldRoom(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//�û�Ч��
	WORD wBindIndex = LOWORD(dwSocketID);
	IServerUserItem *pIServerUserItem = GetBindUserItem(wBindIndex);		
	if (NULL == pIServerUserItem) return true;

	//���ݰ�У��
	if ( wDataSize != sizeof(CMD_GR_Create_Gold_Room) )
		return false;

	//���ݶ���
	CMD_GR_Create_Gold_Room *pGoldRoom = (CMD_GR_Create_Gold_Room*)pData;

	//У���û����Ӻ�����
	if (pIServerUserItem->GetChairID()!=INVALID_CHAIR) 
	{
		SendRequestFailure(pIServerUserItem,TEXT("������Ϸ��,�޷����룡"),REQUEST_FAILURE_NORMAL);
		return true;
	}
	if(pIServerUserItem->GetTableID()!=INVALID_TABLE)
	{
		SendRequestFailure(pIServerUserItem,TEXT("���ڷ�����,�޷����룡"),REQUEST_FAILURE_NORMAL);
		return true;
	}
	//У���û����	Modified by lizhihu ֻ����Ϸ�׷�У��
	if( pIServerUserItem->GetUserGold() < pGoldRoom->dwCellScore )
	{
		SendRequestFailure(pIServerUserItem,TEXT("��Ҳ���,�޷����룡"),REQUEST_FAILURE_NORMAL);
		return true;
	}

	//Ѱ�ҽ�ҷ�������
	WORD wChairID;
	CTableFrame *pTableFrame = GetGlodRoomEmptyChair(wChairID);

	//�ҵ���ҷ�������
	if ( (INVALID_CHAIR != wChairID) && (NULL != pTableFrame) )
	{
		//���п����ӣ��û�����
		if(pTableFrame->PerformSitDownAction(wChairID, pIServerUserItem, 0, true))
		{
			//��ý�ҷ������
			tagTableRule *pTableCfg = (tagTableRule*)pTableFrame->GetCustomRule();
			if (NULL != pTableCfg)
			{
				//���ؼ���ɹ�
				STR_CMD_GC_USER_JOIN_ROOM_SUCCESS JoinSuccess;
				ZeroMemory(&JoinSuccess, sizeof(JoinSuccess));
				memcpy(&JoinSuccess.strTableRule, pTableCfg, sizeof(JoinSuccess.strTableRule));

				//��ֵ
				JoinSuccess.byGameMode = TABLE_MODE_GOLD;
				JoinSuccess.wChairID = wChairID;
				JoinSuccess.dwRoomID = pTableFrame->GetPassword();
				JoinSuccess.byAllGameCount = pTableCfg->GameCount;
				JoinSuccess.wPlayerCount = pTableCfg->PlayerCount;
				JoinSuccess.byZhuangType = pTableCfg->RobBankType;

				//��������
				SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_JOIN_ROOM_SUCCESS, &JoinSuccess ,sizeof(STR_CMD_GC_USER_JOIN_ROOM_SUCCESS));
				return true;
			}
		}	
	}

	//û�н�ҳ���λ�ã����´���һ����ҷ���
	CTableFrame *pCurrTableFrame = GetNextEmptyTable();

	//����У��
	if(pCurrTableFrame == NULL)
	{
		SendRequestFailure(pIServerUserItem, TEXT("����������,���Ժ����ԣ�"), REQUEST_FAILURE_NORMAL);
		return true;
	}

	//���ý�ҷ�����	д������û��ͨ���ԣ���õ����ڿͻ������ã�������ÿ���ͻ��˵�����Ϸ������������
	tagTableRule cfg;
	ZeroMemory(&cfg, sizeof(tagTableRule));

	cfg.GameMode = TABLE_MODE_GOLD;
	cfg.GameCount = 0;
	cfg.PlayerCount = 4;
	cfg.RobBankType = 3;
	cfg.CellScore = pGoldRoom->dwCellScore;
	cfg.JoinScore = 500;
	cfg.LeaveScore = 500;
	cfg.MaxFan = 4;
	/* ע�͵�����Ϸ��������
	cfg.ZhangMao = 1;					//��ë
	cfg.XuanFengGang = 1;				//�����
	cfg.QiXiaoDui = 1;					//��С��
	cfg.QingYiSe = 1;					//��һɫ
	cfg.KaiHuaGang = 1;					//�ܿ�
	*/

	//pCurrTableFrame->SetTableOwner(pIServerUserItem->GetUserID());
	pCurrTableFrame->SetCommonRule(&cfg);

	//Ѱ�ҿ�����
	wChairID = pCurrTableFrame->GetNullChairID();
	if (wChairID != INVALID_CHAIR)
	{
		//�û�����
		if(!pCurrTableFrame->PerformSitDownAction(wChairID, pIServerUserItem, 0, true))
		{
			SendRequestFailure(pIServerUserItem,TEXT("����������,���Ժ�����!"),REQUEST_FAILURE_NORMAL);
			return true;
		}		
	}
	else
	{
		SendRequestFailure(pIServerUserItem,TEXT("���Ѿ�����Ϸ��,�޷�����!"),REQUEST_FAILURE_NORMAL);
		return true;
	}

	//���ؼ���ɹ�
	STR_CMD_GC_USER_JOIN_ROOM_SUCCESS JoinSuccess;
	ZeroMemory(&JoinSuccess, sizeof(JoinSuccess));
	memcpy(&JoinSuccess.strTableRule, &cfg, sizeof(JoinSuccess.strTableRule));

	//��ֵ
	JoinSuccess.byGameMode = 2;
	JoinSuccess.wChairID = wChairID;
	JoinSuccess.dwRoomID = pCurrTableFrame->GetPassword();
	JoinSuccess.byAllGameCount = 0;
	JoinSuccess.wPlayerCount = 4;
	JoinSuccess.byZhuangType = 3;

	//��������
	SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_JOIN_ROOM_SUCCESS, &JoinSuccess ,sizeof(STR_CMD_GC_USER_JOIN_ROOM_SUCCESS));

	return true;
}

//�޸��û��Ƹ���Ϣ
bool CAttemperEngineSink::On_CMD_GC_User_ModifyUserTreasure(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//���ﲻ��У��socketID,��Ϊ����ľ���0
	//����У��
	ASSERT(wDataSize == sizeof(STR_DBO_GR_MODIFY_USER_TREASURE));
	if(wDataSize != sizeof(STR_DBO_GR_MODIFY_USER_TREASURE))
		return false;

	//����
	STR_DBO_GR_MODIFY_USER_TREASURE *pDBOModify = (STR_DBO_GR_MODIFY_USER_TREASURE *)pData;

	//�޸�ʧ�ܣ�ֱ�ӷ���
	if ( DB_SUCCESS != pDBOModify->dwResultCode )
	{
		return true;
	}

	//��ȡ�û�
	IServerUserItem *pIServerUserItem = m_ServerUserManager.SearchUserItem(pDBOModify->dwUserID);

	//�û�����
	if ( NULL == pIServerUserItem )
	{
		return true;
	}
	
	//���췵������
	STR_CMD_GC_USER_MODIFY_TREASURE CMDModify;
	ZeroMemory(&CMDModify, sizeof(STR_CMD_GC_USER_MODIFY_TREASURE));

	//��ֵ
	CMDModify.lOpenRoomCard = pDBOModify->lOpenRoomCard;
	CMDModify.lUserDiamond = pDBOModify->lUserDiamond;
	CMDModify.lUserGold = pDBOModify->lUserGold;

	//�����޸ĳɹ���Ϣ
	SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_MODIFY_TREASURE, &CMDModify, sizeof(STR_CMD_GC_USER_MODIFY_TREASURE));

	//�޸ĳɹ����޸��ڴ��е��û�����
	pIServerUserItem->SetUserRoomCard(CMDModify.lOpenRoomCard);
	pIServerUserItem->SetUserGold(CMDModify.lUserGold);
	pIServerUserItem->SetUserDiamond(CMDModify.lUserDiamond);

	return true;
}

//�û��뿪
VOID CAttemperEngineSink::OnEventUserLogout(IServerUserItem * pIServerUserItem, DWORD dwLeaveReason)
{
	//��������
	DBR_GR_LeaveGameServer LeaveGameServer;
	ZeroMemory(&LeaveGameServer,sizeof(LeaveGameServer));

	//��ȡ�ɼ�
	pIServerUserItem->QueryRecordInfo(LeaveGameServer.RecordInfo);
	pIServerUserItem->DistillVariation(LeaveGameServer.VariationInfo);

	//LeaveGameServer.lControlScore = pIServerUserItem->GetUserControlScore();

	//�û���Ϣ
	LeaveGameServer.dwLeaveReason=dwLeaveReason;
	LeaveGameServer.dwUserID=pIServerUserItem->GetUserID();
	LeaveGameServer.dwInoutIndex=pIServerUserItem->GetInoutIndex();
	LeaveGameServer.dwOnLineTimeCount=(DWORD)(time(NULL))-pIServerUserItem->GetLogonTime();

	//������Ϣ
	LeaveGameServer.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(LeaveGameServer.szMachineID,pIServerUserItem->GetMachineID(),CountArray(LeaveGameServer.szMachineID));

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(pIServerUserItem->GetUserID(),DBR_GR_LEAVE_GAME_SERVER,0L,&LeaveGameServer,sizeof(LeaveGameServer), TRUE);

	//�����û�
	if (m_bCollectUser==true)
	{
		//��������
		CMD_CS_C_UserLeave UserLeave;
		ZeroMemory(&UserLeave,sizeof(UserLeave));

		//���ñ���
		UserLeave.dwUserID=pIServerUserItem->GetUserID();

		//������Ϣ
		m_pITCPSocketService->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_USER_LEAVE,&UserLeave,sizeof(UserLeave));
	}

	//֪�����������˳���Ϸ
	if(m_pIGameMatchServiceManager!=NULL)m_pIGameMatchServiceManager->OnUserQuitGame(pIServerUserItem, 0);

	//����ɾ��
	DeleteWaitDistribute(pIServerUserItem);

	//�㲥��������
	CMD_GF_OnlinePlayers OnlinePlayers;
	ZeroMemory(&OnlinePlayers, sizeof(CMD_GF_OnlinePlayers));

	for(WORD i=0;i < static_cast<WORD>(m_ServerUserManager.GetUserItemCount());i++)
	{
		if( m_ServerUserManager.EnumUserItem(i)->GetUserStatus() != US_OFFLINE 
			&& !m_ServerUserManager.EnumUserItem(i)->IsVirtualUser())
			OnlinePlayers.wOnlinePlayers++;
	}

	for(WORD i=0;i < static_cast<WORD>(m_ServerUserManager.GetUserItemCount());i++)
	{
		IServerUserItem* pItem = m_ServerUserManager.EnumUserItem(i);
		if( pItem->GetUserStatus() != US_OFFLINE && !pItem->IsVirtualUser())
		{
			SendData(pItem,MDM_USER,SUB_GF_ONLINE_PLAYERS,&OnlinePlayers,sizeof(CMD_GF_OnlinePlayers));
		}
	}


	return;
}

//����ʧ��
bool CAttemperEngineSink::SendRequestFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszDescribe, LONG lErrorCode)
{
	//��������
	CMD_GR_RequestFailure RequestFailure;
	ZeroMemory(&RequestFailure,sizeof(RequestFailure));

	//���ñ���
	RequestFailure.lErrorCode=lErrorCode;
	lstrcpyn(RequestFailure.szDescribeString,pszDescribe,CountArray(RequestFailure.szDescribeString));

	SendData(pIServerUserItem, MDM_USER, SUB_GR_REQUEST_FAILURE, &RequestFailure, sizeof(CMD_GR_RequestFailure));

	return true;
}

//�����û�����
bool CAttemperEngineSink::SendUserInfoPacket(IServerUserItem *pIServerUserItem, DWORD dwSocketID)
{
	//Ч�����
	if (pIServerUserItem==NULL) return false;

	//��������
	tagUserInfo *pUserInfo = pIServerUserItem->GetUserInfo();
	if(pUserInfo == NULL)
		return false;

	//��������
	if (dwSocketID == INVALID_DWORD)
	{
		//TODO ����Ⱥ����ʲô��˼������
		SendData(BG_COMPUTER, MDM_GR_LOGON, CMD_GC_LOGON_GET_USER_INFO, pUserInfo, sizeof(tagUserInfo));
	}
	else
	{
		SendData(dwSocketID, MDM_GR_LOGON, CMD_GC_LOGON_GET_USER_INFO, pUserInfo, sizeof(tagUserInfo));
	}

	return true;
}

//���û�
IServerUserItem * CAttemperEngineSink::GetBindUserItem(WORD wBindIndex)
{
	//��ȡ����
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//��ȡ�û�
	if (pBindParameter!=NULL)
	{
		return pBindParameter->pIServerUserItem;
	}

	return NULL;
}

//�󶨲���
tagBindParameter * CAttemperEngineSink::GetBindParameter(WORD wBindIndex)
{
	//��Ч����
	if (wBindIndex==INVALID_WORD) return NULL;

	//��������
	if (wBindIndex<256
		&&(m_pNormalParameter != NULL))
	{
		return m_pNormalParameter+wBindIndex;
	}

	//��������
	if ((wBindIndex>=INDEX_ANDROID) &&(wBindIndex<(INDEX_ANDROID+MAX_ANDROID))
		&& (m_pAndroidParameter != NULL))
	{
		return m_pAndroidParameter+(wBindIndex-INDEX_ANDROID);
	}

	return NULL;
}

//��ȡ���������û� 
IServerUserItem * CAttemperEngineSink::GetVirtualUserItem()
{
	for(int i= INDEX_ANDROID+MAX_ANDROID;i < INDEX_ANDROID+MAX_ANDROID + MAX_VIRTUAL;i++)
	{
		tagBindParameter* pBindParameter = m_pAndroidParameter+(i-INDEX_ANDROID);
		if(NULL != pBindParameter && pBindParameter->pIServerUserItem == NULL)
		{
			//��ʼ�������û���Ϣ
			//�û�����
			tagUserInfo UserInfo;
			tagUserInfoPlus UserInfoPlus;
			ZeroMemory(&UserInfo,sizeof(UserInfo));
			ZeroMemory(&UserInfoPlus,sizeof(UserInfoPlus));

			//��������
			UserInfo.dwUserID=i;
			UserInfo.dwGroupID=0;
			UserInfo.dwCustomID=0;
			lstrcpyn(UserInfo.szNickName,TEXT("����ʮ����"),CountArray(UserInfo.szNickName));
			lstrcpyn(UserInfo.szHeadUrl,TEXT(""),CountArray(UserInfo.szHeadUrl));

			//�û�����
			UserInfo.cbGender=rand()%2;
			UserInfo.cbMemberOrder=0;
			UserInfo.cbMasterOrder=0;
			lstrcpyn(UserInfo.szGroupName,TEXT(""),CountArray(UserInfo.szGroupName));
			lstrcpyn(UserInfo.szUnderWrite,TEXT("����˺�����û�и���ǩ��"),CountArray(UserInfo.szUnderWrite));

			//״̬����
			UserInfo.cbUserStatus=US_FREE;
			UserInfo.wTableID=INVALID_TABLE;
			UserInfo.wChairID=INVALID_CHAIR;

			srand((DWORD)time(NULL));

			//������Ϣ
			UserInfo.lScore= 123456 + rand()%793421169;
			UserInfo.lGrade= 114235 + rand()%524242198;
			UserInfo.lOpenRoomCard= 4235 + rand()%164242198;
			UserInfo.lDiamond= 53222 +rand()%1123143523;
			UserInfo.lGold= 879111 + rand() % 218901000;
			UserInfo.lControlScore=0;
			UserInfo.dwWinCount= 23 + rand() % 15611;
			UserInfo.dwLostCount= 12 + rand() % 13611;
			UserInfo.dwDrawCount= rand() % 1611;
			UserInfo.dwFleeCount= rand() % 11142;
			UserInfo.dwUserMedal= rand() % 112314;
			UserInfo.dwExperience= rand() % 1111;
			UserInfo.lLoveLiness= rand() % 175432;

			//��¼��Ϣ
			UserInfoPlus.dwLogonTime=(DWORD)time(NULL);
			UserInfoPlus.dwInoutIndex=0;

			//�û�Ȩ��
			UserInfoPlus.dwUserRight=0;
			UserInfoPlus.dwMasterRight=0;

			//��������
			UserInfoPlus.bAndroidUser=0;

			//UserInfoPlus.lRestrictScore=m_pGameServiceOption->lRestrictScore;
			UserInfoPlus.lRestrictScore=0L;//����ÿ�ַⶥ
			lstrcpyn(UserInfoPlus.szPassword,TEXT(""),CountArray(UserInfoPlus.szPassword));

			//������Ϣ
			UserInfoPlus.wBindIndex=i;
			UserInfoPlus.dwClientAddr=pBindParameter->dwClientAddr;
			lstrcpyn(UserInfoPlus.szMachineID,TEXT(""),CountArray(UserInfoPlus.szMachineID));

			//�����û�
			m_ServerUserManager.InsertUserItem(&(pBindParameter->pIServerUserItem),UserInfo,UserInfoPlus);
			//����Ϊ�����û�
			pBindParameter->pIServerUserItem->SetVirtualUser(true);

			return pBindParameter->pIServerUserItem;
		}
	}

	return NULL;
}

//��������
WORD CAttemperEngineSink::GetPropertyType(WORD wPropertyIndex)
{
	switch(wPropertyIndex)
	{
	case PROPERTY_ID_CAR:	case PROPERTY_ID_EGG: 	case PROPERTY_ID_CLAP: 	case PROPERTY_ID_KISS: 	case PROPERTY_ID_BEER:
	case PROPERTY_ID_CAKE: 	case PROPERTY_ID_RING:  case PROPERTY_ID_BEAT: 	case PROPERTY_ID_BOMB:  case PROPERTY_ID_SMOKE:
	case PROPERTY_ID_VILLA: case PROPERTY_ID_BRICK: case PROPERTY_ID_FLOWER: 
		{
			return PT_TYPE_PRESENT;
		};
	case PROPERTY_ID_TWO_CARD: 	case PROPERTY_ID_FOUR_CARD:  case PROPERTY_ID_SCORE_CLEAR:     case PROPERTY_ID_ESCAPE_CLEAR:
	case PROPERTY_ID_TRUMPET:	case PROPERTY_ID_TYPHON:     case PROPERTY_ID_GUARDKICK_CARD:  case PROPERTY_ID_POSSESS:
	case PROPERTY_ID_BLUERING_CARD: case PROPERTY_ID_YELLOWRING_CARD: case PROPERTY_ID_WHITERING_CARD: case PROPERTY_ID_REDRING_CARD:
	case PROPERTY_ID_VIPROOM_CARD: 
		{
			return PT_TYPE_PROPERTY;
		};
	}

	ASSERT(false);

	return PT_TYPE_ERROR;
}


//���û���
bool CAttemperEngineSink::InitAndroidUser()
{
	//��������
	tagAndroidUserParameter AndroidUserParameter;
	ZeroMemory(&AndroidUserParameter,sizeof(AndroidUserParameter));

	//���ò���
	AndroidUserParameter.pGameParameter=m_pGameParameter;
	AndroidUserParameter.pGameServiceAttrib=m_pGameServiceAttrib;
	AndroidUserParameter.pGameServiceOption=m_pGameServiceOption;

	//�������
	AndroidUserParameter.pITimerEngine=m_pITimerEngine;
	AndroidUserParameter.pIServerUserManager=&m_ServerUserManager;
	AndroidUserParameter.pIGameServiceManager=m_pIGameServiceManager;
	AndroidUserParameter.pITCPNetworkEngineEvent=QUERY_OBJECT_PTR_INTERFACE(m_pIAttemperEngine,ITCPNetworkEngineEvent);

	//���ö���
	if (m_AndroidUserManager.InitAndroidUser(AndroidUserParameter)==false)
	{
		return false;
	}

	return true;
}

//��������
bool CAttemperEngineSink::InitTableFrameArray()
{
	//���Ӳ���
	tagTableFrameParameter TableFrameParameter;
	ZeroMemory(&TableFrameParameter,sizeof(TableFrameParameter));

	//�ں����
	TableFrameParameter.pITimerEngine=m_pITimerEngine;
	TableFrameParameter.pIKernelDataBaseEngine=m_pIKernelDataBaseEngine;
	TableFrameParameter.pIRecordDataBaseEngine=m_pIRecordDataBaseEngine;

	//�������
	TableFrameParameter.pIMainServiceFrame=this;
	TableFrameParameter.pIAndroidUserManager=&m_AndroidUserManager;
	TableFrameParameter.pIGameServiceManager=m_pIGameServiceManager;

	//����ӿ�
	TableFrameParameter.PITCPSocketService = m_pITCPSocketService;

	//���ò���
	TableFrameParameter.pGameParameter=m_pGameParameter;
	TableFrameParameter.pGameServiceAttrib=m_pGameServiceAttrib;
	TableFrameParameter.pGameServiceOption=m_pGameServiceOption;

	if(m_pIGameMatchServiceManager!=NULL)
		TableFrameParameter.pIGameMatchServiceManager=QUERY_OBJECT_PTR_INTERFACE(m_pIGameMatchServiceManager,IUnknownEx);

	//��������
	m_TableFrameArray.SetSize(MAX_TABLE);
	ZeroMemory(m_TableFrameArray.GetData(),MAX_TABLE*sizeof(CTableFrame *));

	//��������
	for (WORD i=0;i< MAX_TABLE; i++)
	{
		//��������
		m_TableFrameArray[i]=new CTableFrame;

		//��������
		if (m_TableFrameArray[i]->InitializationFrame(i,TableFrameParameter)==false)
		{
			return false;
		}

		if(m_pIGameMatchServiceManager!=NULL)
			m_pIGameMatchServiceManager->InitTableFrame(QUERY_OBJECT_PTR_INTERFACE((m_TableFrameArray[i]),ITableFrame),i);
	}

	if(m_pIGameMatchServiceManager!=NULL)
	{
		if (m_pIGameMatchServiceManager->InitMatchInterface(QUERY_OBJECT_PTR_INTERFACE(m_pIAttemperEngine,ITCPNetworkEngineEvent),m_pIKernelDataBaseEngine,
			(IServerUserManager*)QUERY_OBJECT_INTERFACE(m_ServerUserManager,IServerUserManager),this,m_pITimerEngine,&m_AndroidUserManager)==false)
		{
			ASSERT(FALSE);
			return false;
		}
		// 		if (m_pIGameMatchServiceManager->InitServerUserManager()==false)
		// 		{
		// 			ASSERT(FALSE);
		// 			return false;
		// 		}
		// 
		// 		if(m_pIGameMatchServiceManager->InitMainServiceFrame(QUERY_ME_INTERFACE(IMainServiceFrame))==false)
		// 		{
		// 			ASSERT(FALSE);
		// 			return false;
		// 		}

	}

	return true;
}

//��������
bool CAttemperEngineSink::SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	//��������
	CServiceUnits * pServiceUnits=CServiceUnits::g_pServiceUnits;
	pServiceUnits->PostControlRequest(wRequestID,pData,wDataSize);

	return true;
}

//�������
bool CAttemperEngineSink::InsertDistribute(IServerUserItem * pIServerUserItem)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//״̬�ж�
	ASSERT(pIServerUserItem->GetUserStatus()<US_PLAYING);
	if (pIServerUserItem->GetUserStatus()>=US_PLAYING) return false;

	if(m_pIGameMatchServiceManager!=NULL) m_pIGameMatchServiceManager->OnUserJoinGame(pIServerUserItem,0);
	return false;
}

//�����û�
bool CAttemperEngineSink::InsertWaitDistribute(IServerUserItem * pIServerUserItem)
{
	//����
	bool bAdd=true;
	POSITION pos=m_WaitDistributeList.GetHeadPosition();
	while(pos != NULL)
	{
		IServerUserItem *pUserItem = m_WaitDistributeList.GetNext(pos);
		if(pUserItem->GetUserID()==pIServerUserItem->GetUserID())
		{
			bAdd=false;
			break;
		}
	}

	//����
	if(bAdd)
	{
		int listCount = m_WaitDistributeList.GetCount();
		if (listCount > 0 )
		{
			POSITION InsertPos = m_WaitDistributeList.FindIndex( rand() % listCount);
			m_WaitDistributeList.InsertAfter(InsertPos,pIServerUserItem);
		}
		else
		{
			m_WaitDistributeList.AddTail(pIServerUserItem);
		}
	}

	//֪ͨ
	SendData(pIServerUserItem,MDM_USER,SUB_GR_USER_WAIT_DISTRIBUTE,NULL,0);

	return bAdd;
}

//ɾ���û�
bool CAttemperEngineSink::DeleteWaitDistribute(IServerUserItem * pIServerUserItem)
{
	//����
	POSITION pos=m_WaitDistributeList.GetHeadPosition();
	while(pos != NULL)
	{
		POSITION tempPos = pos;
		IServerUserItem *pUserItem = m_WaitDistributeList.GetNext(pos);
		if(pUserItem->GetUserID()==pIServerUserItem->GetUserID())
		{
			m_WaitDistributeList.RemoveAt(tempPos);
			break;
		}
	}

	//ɾ���û�
	m_ServerUserManager.DeleteUserItem(pIServerUserItem);

	return true;
}



bool  CAttemperEngineSink::Distribute(CTableFrame* pTableFrame, bool bChoiceAndroid, WORD wWantUserCount, WORD& wHandleCount )
{
	//���´���
	bool bAndroidFound = false;
	POSITION pos=m_WaitDistributeList.GetHeadPosition();
	while(pos!=NULL)
	{
		//��������
		WORD wChairID=pTableFrame->GetRandNullChairID();

		//��Ч����
		if (wChairID==INVALID_CHAIR) continue;

		//���´���
		POSITION tempPos=pos;
		IServerUserItem *pUserItem = m_WaitDistributeList.GetNext(pos);

		if (bChoiceAndroid && pUserItem->IsAndroidUser())
		{
			continue;
		}

		bAndroidFound = true;

		if(pUserItem && pUserItem->GetUserStatus()==US_FREE && (pTableFrame->PerformSitDownAction(wChairID,pUserItem)==true))
		{
			//ɾ���Ŷ�
			m_WaitDistributeList.RemoveAt(tempPos);

			//���ñ���
			wHandleCount++;

			//����ж�
			if (wHandleCount>=wWantUserCount) 
			{
				break;
			}
		}
	}

	return bAndroidFound;
}

//���ò���
void CAttemperEngineSink::SetMobileUserParameter(IServerUserItem * pIServerUserItem,BYTE cbDeviceType,WORD wBehaviorFlags,WORD wPageTableCount)
{
	if(wPageTableCount > MAX_TABLE)wPageTableCount=MAX_TABLE;
	pIServerUserItem->SetMobileUserDeskCount(wPageTableCount);
	pIServerUserItem->SetMobileUserRule(wBehaviorFlags);
}

//Ⱥ������
bool CAttemperEngineSink::SendDataBatchToMobileUser(WORD wCmdTable, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//ö���û�
	WORD wEnumIndex=0;
	while(wEnumIndex<m_ServerUserManager.GetUserItemCount())
	{
		//�����û�
		IServerUserItem *pIServerUserItem=m_ServerUserManager.EnumUserItem(wEnumIndex++);
		if(pIServerUserItem==NULL) continue;


		//�������
		WORD wMobileUserRule = pIServerUserItem->GetMobileUserRule();
		WORD wTagerTableID = pIServerUserItem->GetTableID();
		bool bViewModeAll = ((wMobileUserRule&VIEW_MODE_ALL)!=0);

		//״̬����
		if(pIServerUserItem->GetUserStatus() >= US_SIT)
		{
			if(wTagerTableID != wCmdTable)continue;
		}

		//���ֿ���
		if(!bViewModeAll)
		{
			//��Ϣ����
			if(wSubCmdID==CMD_GC_LOGON_GET_USER_INFO && wCmdTable==INVALID_TABLE) continue;
			if(wSubCmdID==SUB_GR_USER_SCORE && pIServerUserItem->GetUserStatus() < US_SIT) continue;

			//�������
			WORD wTagerDeskPos = pIServerUserItem->GetMobileUserDeskPos();
			WORD wTagerDeskCount = pIServerUserItem->GetMobileUserDeskCount();

			//״̬��Ϣ����
			if(wCmdTable < wTagerDeskPos) continue;
			if(wCmdTable > (wTagerDeskPos+wTagerDeskCount-1)) continue;
		}

		//������Ϣ
		SendData(pIServerUserItem,wMainCmdID,wSubCmdID,pData,wDataSize);
	}

	return true;
}

//�������Ƿ����yang
bool CAttemperEngineSink::IsControl( SCORE expectStock )
{
	bool isControl = false;
	//isControl = m_GameControlManager.IsControl(expectStock);
	return isControl;
}

//�޸ĵ�ǰ���ֵyang
bool CAttemperEngineSink::SetStockScore( SCORE currentStock )
{
	//m_GameControlManager.SetStockScore(currentStock);
	return true;
}

//���ÿ��ֵyang
SCORE CAttemperEngineSink::GetChangeStockScore()
{
	return 0/*m_GameControlManager.GetChangeStockScore()*/;
}

void CAttemperEngineSink::StopService()
{
	m_bStopping = true;

	tagBindParameter *pBindParameter = m_pNormalParameter;
	for( INT i = 0; i < MAX_TABLE; i++ )
	{
		//Ŀ¼�û�
		IServerUserItem * pITargerUserItem= pBindParameter->pIServerUserItem;
		if (pITargerUserItem==NULL) 
		{
			pBindParameter++;
			continue;
		}

		//������Ϣ
		SendGameMessage(pITargerUserItem,TEXT("�����������ر�ά�������Ժ����"),SMT_CHAT|SMT_EJECT|SMT_CLOSE_LINK|SMT_CLOSE_GAME);
		pBindParameter++;
	}

	//��ɢ������Ϸ
	for (INT_PTR i=0;i<m_TableFrameArray.GetCount();i++)
	{
		//��ȡ����
		CTableFrame * pTableFrame=m_TableFrameArray[i];
		if ( !pTableFrame->IsGameStarted() ) continue;

		pTableFrame->DismissGame();
	}

	//ֹͣ���ػ����ˡ������û������˵ȶ�ʱ��
	m_pITimerEngine->KillTimer(IDI_LOAD_ANDROID_USER);
	m_pITimerEngine->KillTimer(IDI_DISTRIBUTE_ANDROID);

	//�ӳٹرգ�ȷ��д�ֵ����ݿ�洢����ִ��
	m_pITimerEngine->SetTimer(IDI_WAIT_SERVICE_STOP, TIME_WAIT_SERVICE_STOP, 1, NULL);
}

void CAttemperEngineSink::SendServiceStopped()
{
	m_pITimerEngine->KillTimer(IDI_WAIT_SERVICE_STOP);

	CP_ControlResult ControlResult;
	ControlResult.cbSuccess = ER_SUCCESS;
	SendUIControlPacket(UI_SERVICE_STOP_SERVICE_FINISH, &ControlResult, sizeof(ControlResult));
}


bool CAttemperEngineSink::DistributeAndroid()
{
//	//��������
//	if (m_AndroidUserManager.GetAndroidCount()>0 )
//	{
//		//��������
////		bool bAllowDynamicJoin=CServerRule::IsAllowDynamicJoin(m_pGameServiceOption->dwServerRule);
//	//	bool bAllowAndroidAttend=CServerRule::IsAllowAndroidAttend(m_pGameServiceOption->dwServerRule);
//		//bool bAllowAndroidSimulate=CServerRule::IsAllowAndroidSimulate(m_pGameServiceOption->dwServerRule);
//		//bool bAllowAvertCheatMode=(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule)&&(m_pGameServiceAttrib->wChairCount < MAX_CHAIR));
//
//		//ģ�⴦��
//		if (bAllowAndroidSimulate==true && bAllowAvertCheatMode==false)
//		{
//			//����״̬
//			tagAndroidUserInfo AndroidSimulate; 
//			m_AndroidUserManager.GetAndroidUserInfo(AndroidSimulate,ANDROID_SIMULATE);
//
//			//��������
//			if (AndroidSimulate.wFreeUserCount>0)
//			{
//				for (WORD i=0;i<8;i++)
//				{
//					//�������
//					//WORD wTableID=rand()%(__max(m_pGameServiceOption->wTableCount/3,1));
//					WORD wTableID=rand()%(__max(MAX_TABLE,1));
//
//					//��ȡ����
//					CTableFrame * pTableFrame=m_TableFrameArray[wTableID];
//					if ((pTableFrame->IsGameStarted()==true)&&(bAllowDynamicJoin==false)) continue;
//
//					//����״��
//					tagTableUserInfo TableUserInfo;
//					WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);
//
//					//�����ж�
//					if (TableUserInfo.wTableUserCount>0) continue;
//					if ((wUserSitCount>=TableUserInfo.wMinUserCount)&&(m_pGameServiceAttrib->wChairCount<MAX_CHAIR)) continue;
//
//					//�����ж�
//					if (AndroidSimulate.wFreeUserCount>=TableUserInfo.wMinUserCount)
//					{
//						//��������
//						WORD wHandleCount=0;
//						WORD wWantAndroidCount=TableUserInfo.wMinUserCount;
//
//						//���ݵ���
//						if (m_pGameServiceAttrib->wChairCount>TableUserInfo.wMinUserCount)
//						{
//							WORD wChairCount=m_pGameServiceAttrib->wChairCount;
//							WORD wFreeUserCount=AndroidSimulate.wFreeUserCount;
//							WORD wOffUserCount=__min(wChairCount,wFreeUserCount)-TableUserInfo.wMinUserCount;
//							wWantAndroidCount+=((wOffUserCount > 0) ? (rand()%(wOffUserCount+1)) : 0);
//						}
//
//						//���´���
//						for (WORD j=0;j<AndroidSimulate.wFreeUserCount;j++)
//						{
//							//��������
//							WORD wChairID=pTableFrame->GetRandNullChairID();
//
//							//��Ч����
//							//ASSERT(wChairID!=INVALID_CHAIR);
//							if (wChairID==INVALID_CHAIR) continue;
//
//							//�û�����
//							IAndroidUserItem * pIAndroidUserItem=AndroidSimulate.pIAndroidUserFree[j];
//							if (pTableFrame->PerformSitDownAction(wChairID,pIAndroidUserItem->GetMeUserItem())==true)
//							{
//								//���ñ���
//								wHandleCount++;
//
//								//����ж�
//								if (wHandleCount>=wWantAndroidCount) 
//								{
//									return true;
//								}
//							}
//						}
//
//						if(wHandleCount > 0) return true;
//					}
//				}
//			}
//		}
//
//		//�����
//		if (bAllowAndroidAttend==true)
//		{
//			//����״̬
//			tagAndroidUserInfo AndroidPassivity;
//			m_AndroidUserManager.GetAndroidUserInfo(AndroidPassivity,ANDROID_PASSIVITY);
//
//			if(bAllowAvertCheatMode)
//			{
//				//���´���
//				for (WORD j=0;j<AndroidPassivity.wFreeUserCount;j++)
//				{
//					IAndroidUserItem * pIAndroidUserItem=AndroidPassivity.pIAndroidUserFree[j];
//					if (InsertWaitDistribute(pIAndroidUserItem->GetMeUserItem())==true) return true;
//				}
//			}
//			else
//			{
//				//��������
//				if (AndroidPassivity.wFreeUserCount>0)
//				{
//					//������Ϸ
//					if(m_pGameServiceAttrib->wChairCount >= MAX_CHAIR)
//					{
//						for (INT_PTR i=0;i<MAX_TABLE;i++)
//						{
//							//��ȡ����
//							CTableFrame * pTableFrame=m_TableFrameArray[i];
//							if ((pTableFrame->IsGameStarted()==true)&&(bAllowDynamicJoin==false)) continue;
//
//							//����״��
//							tagTableUserInfo TableUserInfo;
//							WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);
//
//							//�����ж�
//							if (wUserSitCount>m_pGameServiceAttrib->wChairCount*2/3) continue;
//
//							//��������
//							IServerUserItem * pIServerUserItem=NULL;
//							WORD wChairID=pTableFrame->GetRandNullChairID();
//
//							//��Ч����
//							ASSERT(wChairID!=INVALID_CHAIR);
//							if (wChairID==INVALID_CHAIR) continue;
//
//							//���´���
//							for (WORD j=0;j<AndroidPassivity.wFreeUserCount;j++)
//							{
//								IAndroidUserItem * pIAndroidUserItem=AndroidPassivity.pIAndroidUserFree[j];
//								if (pTableFrame->PerformSitDownAction(wChairID,pIAndroidUserItem->GetMeUserItem())==true) return true;
//							}
//						}
//					}
//					else
//					{
//						for (INT_PTR i=0;i<MAX_TABLE;i++)
//						{
//							//��ȡ����
//							CTableFrame * pTableFrame=m_TableFrameArray[i];
//							if ((pTableFrame->IsGameStarted()==true)&&(bAllowDynamicJoin==false)) continue;
//
//							//����״��
//							tagTableUserInfo TableUserInfo;
//							WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);
//
//							//�����ж�
//							if (wUserSitCount==0) continue;
//							if (TableUserInfo.wTableUserCount==0) continue;
//							if ((wUserSitCount>=TableUserInfo.wMinUserCount)&&(rand()%10>5)) continue;
//							if (pTableFrame->GetNullChairCount()==0) continue;
//
//							//��������
//							IServerUserItem * pIServerUserItem=NULL;
//							WORD wChairID=pTableFrame->GetRandNullChairID();
//
//							//��Ч����										
//							ASSERT(wChairID!=INVALID_CHAIR);
//							if (wChairID==INVALID_CHAIR) continue;
//
//							//���´���
//							for (WORD j=0;j<AndroidPassivity.wFreeUserCount;j++)
//							{
//								IAndroidUserItem * pIAndroidUserItem=AndroidPassivity.pIAndroidUserFree[j];
//								if (pTableFrame->PerformSitDownAction(wChairID,pIAndroidUserItem->GetMeUserItem())==true) return true;
//							}
//						}
//					}
//				}
//			}
//		}
//
//		//�����
//		if (bAllowAndroidAttend==true)
//		{
//			//����״̬
//			tagAndroidUserInfo AndroidInitiative;
//			m_AndroidUserManager.GetAndroidUserInfo(AndroidInitiative,ANDROID_INITIATIVE);
//			WORD wAllAndroidCount = AndroidInitiative.wFreeUserCount+AndroidInitiative.wPlayUserCount+AndroidInitiative.wSitdownUserCount;
//
//			if(bAllowAvertCheatMode)
//			{
//				//���´���
//				for (WORD j=0;j<AndroidInitiative.wFreeUserCount;j++)
//				{
//					IAndroidUserItem * pIAndroidUserItem=AndroidInitiative.pIAndroidUserFree[j];
//					if (InsertWaitDistribute(pIAndroidUserItem->GetMeUserItem())==true) return true;
//				}
//			}
//			else
//			{
//				//��������
//				if (AndroidInitiative.wFreeUserCount>0)
//				{
//					for (INT_PTR i=0;i<MAX_TABLE;i++)
//					{
//						//��ȡ����
//						CTableFrame * pTableFrame=m_TableFrameArray[i];
//						if ((pTableFrame->IsGameStarted()==true)&&(bAllowDynamicJoin==false)) continue;
//
//						//����״��
//						tagTableUserInfo TableUserInfo;
//						WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);
//
//						//�����ж�
//						if((m_pGameServiceAttrib->wChairCount<MAX_CHAIR) && wUserSitCount>(TableUserInfo.wMinUserCount-1)) continue;
//
//						//��������
//						IServerUserItem * pIServerUserItem=NULL;
//						WORD wChairID=pTableFrame->GetRandNullChairID();
//
//						//��Ч����
//						ASSERT(wChairID!=INVALID_CHAIR);
//						if (wChairID==INVALID_CHAIR) continue;
//
//						//���´���
//						for (WORD j=0;j<AndroidInitiative.wFreeUserCount;j++)
//						{
//							IAndroidUserItem * pIAndroidUserItem=AndroidInitiative.pIAndroidUserFree[j];
//							if (pTableFrame->PerformSitDownAction(wChairID,pIAndroidUserItem->GetMeUserItem())==true) return true;
//						}
//					}
//				}
//			}
//		}
//
//		//��������
//		WORD wStandUpCount=0;
//		WORD wRandCount=((rand()%3)+1);
//		INT_PTR nIndex = rand()%(__max(MAX_TABLE,1));
//		for (INT_PTR i=nIndex;i< MAX_TABLE+nIndex;++i)
//		{
//			//��ȡ����
//			INT_PTR nTableIndex=i% MAX_TABLE;
//			CTableFrame * pTableFrame=m_TableFrameArray[nTableIndex];
//			if (pTableFrame->IsGameStarted()==true) continue;
//
//			//����״��
//			tagTableUserInfo TableUserInfo;
//			WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);
//
//			//�û�����
//			bool bRand = ((rand()%100)>50);
//			if (TableUserInfo.wTableAndroidCount==0) continue;
//			if ((TableUserInfo.wTableUserCount>0)&&(bAllowAndroidAttend==true) && bRand) continue;
//			if (TableUserInfo.wTableAndroidCount>=TableUserInfo.wMinUserCount && bRand) continue;
//
//			//��������
//			for (WORD j=0;j<pTableFrame->GetChairCount();j++)
//			{
//				//��ȡ�û�
//				IServerUserItem * pIServerUserItem=pTableFrame->GetTableUserItem(j);
//				if (pIServerUserItem==NULL) continue;
//
//				//�û�����
//				if ((pIServerUserItem->IsAndroidUser()==true)&&(pTableFrame->PerformStandUpAction(pIServerUserItem)==true))
//				{
//					wStandUpCount++;
//					if(wStandUpCount>=wRandCount)
//						return true;
//					else
//						break;
//				}
//			}
//		}
//
//		//��������
//		nIndex = rand()%(__max(MAX_TABLE,1));
//		for (INT_PTR i=nIndex;i< MAX_TABLE+nIndex; ++i)
//		{
//			//��ȡ����
//			INT_PTR nTableIndex=i%MAX_TABLE;
//			CTableFrame * pTableFrame=m_TableFrameArray[nTableIndex];
//			if (pTableFrame->IsGameStarted()==true) continue;
//
//			//����״��
//			tagTableUserInfo TableUserInfo;
//			WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);
//
//			//�û�����
//			bool bRand = ((rand()%100)>50);
//			if (TableUserInfo.wTableAndroidCount==0) continue;
//			if ((TableUserInfo.wTableUserCount>0)&&(bAllowAndroidAttend==true) && bRand) continue;
//
//			//��������
//			for (WORD j=0;j<pTableFrame->GetChairCount();j++)
//			{
//				//��ȡ�û�
//				IServerUserItem * pIServerUserItem=pTableFrame->GetTableUserItem(j);
//				if (pIServerUserItem==NULL) continue;
//
//				//�û�����
//				if ((pIServerUserItem->IsAndroidUser()==true)&&(pTableFrame->PerformStandUpAction(pIServerUserItem)==true))
//				{
//					wStandUpCount++;
//					if(wStandUpCount>=wRandCount)
//						return true;
//					else
//						break;
//				}
//			}
//		}
//	}

	return true;
}

void CAttemperEngineSink::InitRoomControlVal( SCORE val )
{
	m_ControlValueForRoomOrgi = m_ControlValueForRoom = val;
}

SCORE CAttemperEngineSink::CheckAndSaveRoomControlValue()
{
	if ( m_ControlValueForRoom != m_ControlValueForRoomOrgi )
	{
		m_ControlValueForRoomOrgi = m_ControlValueForRoom;

		return m_ControlValueForRoom;
	}

	return 0;
}


//////////////////////////////////////////////////////////////////////////////////
//��������˿���
bool CAttemperEngineSink::AddOtherRoomInfo(DBR_GR_UpdateTableInfo* pTableInfo)
{
	//��Ч����
	if (pTableInfo==NULL) return false;

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR, DBR_GR_ADD_TABLE_INFO, 0, pTableInfo, sizeof(DBR_GR_UpdateTableInfo));

	return true;
}

//��ʼ�����˿���
void CAttemperEngineSink::StartTable(DWORD dwTableID)
{
	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR,DBR_GR_START_TABLE,0,&dwTableID,sizeof(dwTableID));
}

//���������˿���
void CAttemperEngineSink::ConcludeTable(DWORD dwTableID)
{
	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR,DBR_GR_END_TABLE,0,&dwTableID,sizeof(dwTableID));
}

#pragma region Club����Ȧ2 �¼�֪ͨ(1.������Ϣ 2.�����Ϣ)
//club ��������
void CAttemperEngineSink::ClubTableCreate(DWORD dwClubRoomID, DWORD dwUserID, DWORD dwTableID, DWORD dwLockState)
{
    STR_DBR_CLUB_TABLE_INFO Dbr;
	ZeroMemory(&Dbr, sizeof(Dbr));
	Dbr.dwClubRoomID = dwClubRoomID;
	Dbr.dwUserID = dwUserID;
	Dbr.dwTableID =dwTableID;
    Dbr.dwLockState =dwLockState;
	Dbr.dwTableState = 0;
	Dbr.byCurrentRound = 0;
	Dbr.byMask =1;

	Dbr.byCompanyID = _MYSTERY;
	Dbr.dwGameID = (m_pGameServiceOption->dwServerID) & 0xFFFFFF00;

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR, DBR_CLUB_TABLE_INFO, 0, &Dbr, sizeof(Dbr));
}

//club ���ӿ�ʼ��Ϸ
void CAttemperEngineSink::ClubTableStart(DWORD dwTableID, BYTE byClubOrHallGold)
{
	//���ֲ�������Ϣ  ��ע: ���ӿ�ʼ��Ϸ��, ���µ�ǰ����Ϊ (0 + 1)
	if(1 == byClubOrHallGold)
	{
		STR_DBR_CLUB_TABLE_INFO Dbr;
		ZeroMemory(&Dbr, sizeof(Dbr));
		Dbr.dwTableID = dwTableID;
		Dbr.dwTableState = 1;
		Dbr.byCurrentRound = 1;//����һ������ TODONOW �����޸�
		Dbr.byMask = 2;

		m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR,DBR_CLUB_TABLE_INFO,0,&Dbr,sizeof(Dbr));
	}

	//��Ҵ���������Ϣ
	if(2 == byClubOrHallGold)
	{
		STR_DBR_HALL_GOLD_TABLE_INFO HallDbr;
		ZeroMemory(&HallDbr, sizeof(HallDbr));
		HallDbr.dwTableID = dwTableID;
		HallDbr.dwTableState = 1;
		HallDbr.byMask = 2;

		m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR,DBR_HALL_GOLD_TABLE_INFO,0,&HallDbr,sizeof(HallDbr));
	}
}

//club �����Ѿ���Ա
void CAttemperEngineSink::ClubTableMax(DWORD dwTableID, BYTE byClubOrHallGold)
{
	//��ע: ���ӿ�ʼ��Ϸ��, ���µ�ǰ����Ϊ (0 + 1)
	if(1 == byClubOrHallGold)
	{
		STR_DBR_CLUB_TABLE_INFO Dbr;
		ZeroMemory(&Dbr, sizeof(Dbr));
		Dbr.dwTableID = dwTableID;
		Dbr.dwTableState = 2;
		Dbr.byMask = 2;

		m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR,DBR_CLUB_TABLE_INFO,0,&Dbr,sizeof(Dbr));
	}

	//��Ҵ���������Ϣ
	if(2 == byClubOrHallGold)
	{
		STR_DBR_HALL_GOLD_TABLE_INFO HallDbr;
		ZeroMemory(&HallDbr, sizeof(HallDbr));
		HallDbr.dwTableID = dwTableID;
		HallDbr.dwTableState = 2;
		HallDbr.byMask = 2;

		m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR,DBR_HALL_GOLD_TABLE_INFO,0,&HallDbr,sizeof(HallDbr));
	}
}

//club ����û����Ա
void CAttemperEngineSink::ClubTableNotMax(DWORD dwTableID, BYTE byClubOrHallGold)
{
	//��ע: ���ӿ�ʼ��Ϸ��, ���µ�ǰ����Ϊ (0 + 1)
	if(1 ==  byClubOrHallGold)
	{
		STR_DBR_CLUB_TABLE_INFO Dbr;
		ZeroMemory(&Dbr, sizeof(Dbr));
		Dbr.dwTableID = dwTableID;
		Dbr.dwTableState = 0;
		Dbr.byMask = 2;

		m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR,DBR_CLUB_TABLE_INFO,0,&Dbr,sizeof(Dbr));
	}

	//��Ҵ���������Ϣ
	if( 2 ==  byClubOrHallGold)
	{
		STR_DBR_HALL_GOLD_TABLE_INFO HallDbr;
		ZeroMemory(&HallDbr, sizeof(HallDbr));
		HallDbr.dwTableID = dwTableID;
		HallDbr.dwTableState = 0;
		HallDbr.byMask = 2;

		m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR,DBR_HALL_GOLD_TABLE_INFO,0,&HallDbr,sizeof(HallDbr));
	}
}

//club ����С�ֽ���
void CAttemperEngineSink::ClubTableXJ(DWORD dwTableID)
{
	//��ע: ����С�ֽ�����, ���µ�ǰ����Ϊ (���ݿ� + 1)
	STR_DBR_CLUB_TABLE_INFO Dbr;
	ZeroMemory(&Dbr, sizeof(Dbr));
	Dbr.dwTableID = dwTableID;
	Dbr.dwTableState = 1;
	Dbr.byCurrentRound = 1;//����һ������ TODONOW �����޸�
	Dbr.byMask = 2;
	
	m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR,DBR_CLUB_TABLE_INFO,0,&Dbr,sizeof(Dbr));
}

//club ���Ӵ�ֽ���
void CAttemperEngineSink::ClubTableDJ(DWORD dwTableID)
{
    //��ע: ���Ӵ�ֽ���, ��ɢ����
    STR_DBR_CLUB_TABLE_INFO Dbr;
	ZeroMemory(&Dbr, sizeof(Dbr));
	Dbr.dwTableID = dwTableID;
	Dbr.byMask = 3;
	
	m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR,DBR_CLUB_TABLE_INFO,0,&Dbr,sizeof(Dbr));
}

#pragma endregion

//�û����������˿���
void CAttemperEngineSink::JoinTable(DWORD dwTableID, DWORD dwUserID)
{
	
	DBR_GP_AddTableUser AddTableUser;
	AddTableUser.dwTableID = dwTableID;
	AddTableUser.dwUserID = dwUserID;
	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR,DBR_GR_ADD_TABLEUSER,0,&AddTableUser,sizeof(DBR_GP_AddTableUser));
}

//�û��뿪�����˿���
void CAttemperEngineSink::LeaveTable(DWORD dwTableID, DWORD dwUserID)
{
	DBR_GP_DeleteTableUser DeleteTableUser;
	DeleteTableUser.dwTableID = dwTableID;
	DeleteTableUser.dwUserID = dwUserID;

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR,DBR_GR_DELETE_TABLEUSER,0,&DeleteTableUser,sizeof(DBR_GP_DeleteTableUser));
}

//�齱���
bool CAttemperEngineSink::OnDBLotteryResult( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//����У��
	ASSERT(wDataSize==sizeof(DBO_GR_LotteryResult));
	if(wDataSize!=sizeof(DBO_GR_LotteryResult))
		return false;

	DBO_GR_LotteryResult *pLotteryResult = (DBO_GR_LotteryResult*)pData;

	CMD_GR_LotteryResult LotteryResult;
	ZeroMemory(&LotteryResult,sizeof(CMD_GR_LotteryResult));

	LotteryResult.byIndex = pLotteryResult->byIndex;
	LotteryResult.byType = pLotteryResult->byType;
	LotteryResult.dwRewardCount = pLotteryResult->dwRewardCount;
	lstrcpyn(LotteryResult.szDescribe,pLotteryResult->szDescribe,CountArray(LotteryResult.szDescribe));
	lstrcpyn(LotteryResult.szPacketID,pLotteryResult->szPacketID,CountArray(LotteryResult.szPacketID));
	//memcpy(&LotteryResult,pLotteryResult,sizeof(CMD_GP_LotteryResult));

	wprintf(TEXT("pLottery:%d,%d,%d,%s,%s\n"),pLotteryResult->byType,pLotteryResult->byIndex,pLotteryResult->dwRewardCount,pLotteryResult->szPacketID,pLotteryResult->szDescribe);
	wprintf(TEXT("Lottery:%d,%d,%d,%s,%s\n"),LotteryResult.byType,LotteryResult.byIndex,LotteryResult.dwRewardCount,LotteryResult.szPacketID,LotteryResult.szDescribe);

	m_pITCPNetworkEngine->SendData(dwContextID,MDM_G_FRAME,SUB_GR_LOTTERY_RESULT,&LotteryResult,sizeof(CMD_GR_LotteryResult));

	return true;
}

//Club����Ȧ��������
bool CAttemperEngineSink::On_CMD_CG_CLUB_CREATE_TABLE( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//��ȡ�û�
	WORD wBindIndex = LOWORD(dwContextID);
	IServerUserItem *pIServerUserItem = GetBindUserItem(wBindIndex);
	//�û�У��
	if (pIServerUserItem==NULL) return false;

	//У�����
	if(wDataSize!=sizeof(STR_DBO_GC_CLUB_CREATE_TABLE))
		return false;

	STR_DBO_GC_CLUB_CREATE_TABLE *pDbo = (STR_DBO_GC_CLUB_CREATE_TABLE*)pData;

	//���̴���
	return CreateTableClub(pDbo, pIServerUserItem);
}

//�û�����
void CAttemperEngineSink::ClubPlayerSitDown(DWORD dwTableID, DWORD dwUserID, BYTE byChairID, BYTE byClubOrHallGold)
{
	//���ֲ�����
	if(1 == byClubOrHallGold)
	{
		STR_DBR_CLUB_PLAYER_INFO Dbr;
		ZeroMemory(&Dbr, sizeof(Dbr));
		Dbr.dwTableID = dwTableID;
		Dbr.dwUserID = dwUserID;
		Dbr.byChairID = byChairID;
		Dbr.byMask = 1; 

		m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR,DBR_CLUB_PLAYER_INFO,0,&Dbr,sizeof(Dbr));
	}

	//��Ҵ�������
	if(2 ==  byClubOrHallGold)
	{
		STR_DBR_HALL_GOLD_PLAYER_INFO HallDbr;
		ZeroMemory(&HallDbr, sizeof(HallDbr));
		HallDbr.dwTableID = dwTableID;
		HallDbr.dwUserID = dwUserID;
		HallDbr.byChairID = byChairID;
		HallDbr.byMask = 1; 

		m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR,DBR_HALL_GOLD_PLAYER_INFO,0,&HallDbr,sizeof(HallDbr));
	}
}

//�û�����
void CAttemperEngineSink::ClubPlayerSitUp(DWORD dwTableID, DWORD dwUserID, BYTE byChairID, BYTE byClubOrHallGold)
{
	//���ֲ�����
	if( 1 == byClubOrHallGold)
	{
		STR_DBR_CLUB_PLAYER_INFO Dbr;
		ZeroMemory(&Dbr, sizeof(Dbr));
		Dbr.dwTableID = dwTableID;
		Dbr.dwUserID = dwUserID;
		Dbr.byMask = 2; 

		m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR,DBR_CLUB_PLAYER_INFO,0,&Dbr,sizeof(Dbr));
	}

	//��Ҵ���
	if( 2 == byClubOrHallGold)
	{
		STR_DBR_HALL_GOLD_PLAYER_INFO HallDbr;
		ZeroMemory(&HallDbr, sizeof(HallDbr));
		HallDbr.dwTableID = dwTableID;
		HallDbr.dwUserID = dwUserID;
		HallDbr.byChairID = byChairID;
		HallDbr.byMask = 2; 

		m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR,DBR_HALL_GOLD_PLAYER_INFO,0,&HallDbr,sizeof(HallDbr));
	}
}

//���һ���û�����
void CAttemperEngineSink::ClubLastPlayerSitUp(DWORD dwTableID, DWORD dwUserID, BYTE byChairID, BYTE byClubOrHallGold)
{
	//���ֲ�����
	if( 1 == byClubOrHallGold)
	{
		STR_DBR_CLUB_TABLE_INFO Dbr;
		ZeroMemory(&Dbr, sizeof(Dbr));
		Dbr.dwTableID = dwTableID;
		Dbr.byMask = 3;

		m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR,DBR_CLUB_TABLE_INFO,0,&Dbr,sizeof(Dbr));
	}

	//��Ҵ�������
	if( 2 == byClubOrHallGold)
	{
		STR_DBR_HALL_GOLD_TABLE_INFO HallDbr;
		ZeroMemory(&HallDbr, sizeof(HallDbr));
		HallDbr.dwTableID = dwTableID;
		HallDbr.byMask = 3;

		m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR,DBR_HALL_GOLD_TABLE_INFO,0,&HallDbr,sizeof(HallDbr));
	}
}

#pragma region ��������
//********************************************��������*******************************************//

//��������ID�������뷿������
DWORD CAttemperEngineSink::GenerateTablePassword()
{	
	//��������
	srand(static_cast<unsigned int >(time(NULL)));
	BYTE byHigh1 = 1+  rand() % 219 + 24;
	BYTE byHigh2 = ((m_pGameServiceAttrib->dwGameID) >> 16) &  0xFF;
	BYTE byHign3 = ((m_pGameServiceAttrib->dwGameID) >> 8) & 0xF;
	DWORD dwPassword = (byHigh1 << 12) + (byHigh2 <<4) + byHign3;

	//�ж����������Ƿ����
	while(true)
	{
		bool bFind = false;
		for (WORD i=0; i<m_TableFrameArray.GetCount(); i++)
		{
			//��ȡ����
			ASSERT(m_TableFrameArray[i] != NULL);
			CTableFrame *pTableFrame = m_TableFrameArray[i];

			//����״̬�ж�
			if ( (NULL != pTableFrame) && 
				 (0 < pTableFrame->GetSitUserCount() ) && 
				 (dwPassword == pTableFrame->GetPassword()) )
			{
				bFind = true;
				break;
			}
		}
		//���������Ѿ��õ������룬��������	TODONOW ��������ԭ�������ɷ�ʽ
		if(bFind) 
		{
			//added by lizhihu
			byHigh1 = 1+  rand() % 219 + 24;
			dwPassword = (byHigh1 << 12) + (byHigh2 <<4) + byHign3;

			/*dwPassword = ((1 + rand() % 5) << 17) + ((rand() % 512) << 8) + (BYTE)(m_pGameServiceOption->dwServerID & 0XFF);*/
		}
		else
			break;
	}

	return dwPassword;
}

//Ѱ����һ��������
CTableFrame* CAttemperEngineSink::GetNextEmptyTable()
{
	//Ѱ����һ��������
	for (WORD i=0;i<m_TableFrameArray.GetCount();i++)
	{
		//��ȡ����
		ASSERT(m_TableFrameArray[i] != NULL);
		CTableFrame *pTableFrame = m_TableFrameArray[i];

		//�����ж�
		if ( (NULL != pTableFrame) && 
			 (0 == pTableFrame->GetSitUserCount()) && 
			 (0 == pTableFrame->GetCreateTableUser()) ) 
		{
			return pTableFrame;
		}
	}

	return NULL;
}

//��������Ѱ��ָ������
CTableFrame* CAttemperEngineSink::GetDesignatedTable(const DWORD &dwPassword)
{
	//Ѱ��ָ������
	for (WORD i=0; i<m_TableFrameArray.GetCount(); i++)
	{
		//��ȡ����
		ASSERT(m_TableFrameArray[i]!=NULL);
		CTableFrame *pTableFrame = m_TableFrameArray[i];

		//�����ж�
		if ( (NULL != pTableFrame) && 
			 (dwPassword == pTableFrame->GetPassword()) )
		{
			return pTableFrame;
		}
	}

	return NULL;
}

//���ҽ�ҷ�������
CTableFrame* CAttemperEngineSink::GetGlodRoomEmptyChair(WORD &wChairID)
{
	//��������
	CTableFrame *pTableFrame = NULL;
	wChairID = INVALID_CHAIR;

	//Ѱ�ҽ�ҷ�������
	for (WORD i=0; i<m_TableFrameArray.GetCount(); i++)
	{
		//��ȡ����
		ASSERT(m_TableFrameArray[i] != NULL);
		CTableFrame *pTableFrame = m_TableFrameArray[i];

		//����У��
		if ( (NULL == pTableFrame) || 
			 (pTableFrame->GetGameStatus() != GAME_STATUS_FREE) || 
			 (pTableFrame->GetTableMode() != TABLE_MODE_GOLD))
			continue;

		//��ȡ������
		wChairID = pTableFrame->GetNullChairID();
		if(wChairID == INVALID_CHAIR)
			continue;

		return pTableFrame;
	}

	return NULL;
}

//���������˿�������
void CAttemperEngineSink::HandleCreateTableForOthers(CTableFrame *pCurTableFrame, IServerUserItem *pIServerUserItem, tagTableRule *pCfg)
{
	//У��
	if ( NULL == pCurTableFrame || 
		 NULL == pIServerUserItem ||
		 NULL == pCfg)
	{
		return;
	}

	//������������
	srand(static_cast<unsigned int >(time(NULL)));
	DWORD dwPassword = GenerateTablePassword();

	//���÷�������͹���
	pCurTableFrame->SetPassword(dwPassword);

	//���÷����Զ���ɢ,�����˿�����������ʱ����ʮ����
	pCurTableFrame->SetTableAutoDismiss(10);
	pCurTableFrame->SetCreateTableUser(pIServerUserItem->GetUserID()); //���ô������ID

	//��ӿ�����Ϣ
	DBR_GR_UpdateTableInfo TableInfo;
	TableInfo.byCost =  static_cast<BYTE>(pCfg->lSinglePayCost);
	TableInfo.byIsStart = 0;
	TableInfo.byMaxPlayerCount = 5;
	TableInfo.byMode = pCfg->GameMode;
	TableInfo.byPlayerCount = 0;
	TableInfo.byZhuangType = pCfg->RobBankType;
	TableInfo.dwCreaterID = pIServerUserItem->GetUserID();
	TableInfo.dwPassword = dwPassword;
	TableInfo.dwTableID = pCurTableFrame->GetTableID();
	TableInfo.wJuShu = pCfg->GameCount;

	//��������ʱ��
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);
	_sntprintf_s(TableInfo.szTime, sizeof(TableInfo.szTime), TEXT("%04d-%02d-%02d %02d:%02d:%02d"), SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay, SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond);

	//��ӷ�����Ϣ�����ݿ�  TODO ����û��DBO�����������Ƿ���Ҫ�޸�
	AddOtherRoomInfo(&TableInfo);

	//���ͷ�����Ϣ
	STR_CMD_GC_USER_CREATE_ROOM_SUCCESS nCreate;
	nCreate.dwPassword = dwPassword;
	nCreate.wChairID = INVALID_CHAIR;
	nCreate.byAllGameCount = pCfg->GameCount;
	nCreate.byGameMode = pCfg->GameMode;
	nCreate.byZhuangType = pCfg->RobBankType;

	//���ʹ�������ɹ���Ϣ
	SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_CREATE_ROOM_SUCCESS, &nCreate, sizeof(STR_CMD_GC_USER_CREATE_ROOM_SUCCESS));	

	//�����˿��������ȿ۳����������û��ķ��� �����˿���ֻ֧�ַ���ģʽ
	OnEventModifyUserTreasure(pIServerUserItem, dwPassword ,  TABLE_MODE_FK, 0,  -pCfg->lSinglePayCost, 0);
}

//����������������
bool CAttemperEngineSink::HandleCreateTable(CTableFrame *pCurTableFrame, IServerUserItem *pIServerUserItem, tagTableRule *pCfg)
{
	//У��
	if ( NULL == pCurTableFrame || 
		 NULL == pIServerUserItem ||
		 NULL == pCfg)
	{
		return false;
	}

	//�����������룬����� = ǰ�� + TABLEID
	srand(static_cast<unsigned int >(time(NULL)));
	DWORD dwPassword = GenerateTablePassword();

	//������������
	//pCurTableFrame->SetTableOwner(pIServerUserItem->GetUserID());
	pCurTableFrame->SetPassword(dwPassword);

	//���÷����Զ���ɢ��Ĭ��һ���� -- ������ָ����ʼ��Ϸ �Զ�һ���Ӻ��ɢ
	pCurTableFrame->SetTableAutoDismiss();
	
	//��ÿ�����
	WORD wChairID = pCurTableFrame->GetNullChairID(); 

	//���Ӻ���Ч
	if (INVALID_CHAIR != wChairID)
	{
		//�û�����		TODO �����ÿͻ�����������
		if(pCurTableFrame->PerformSitDownAction(wChairID, pIServerUserItem, 0, true)==false)
		{
			SendRequestFailure(pIServerUserItem, TEXT("�û�����ʧ��!"), REQUEST_FAILURE_NORMAL);
			return false;
		}		
	}
	else
	{
		SendRequestFailure(pIServerUserItem, TEXT("��������,�޷����뷿��!"), REQUEST_FAILURE_NORMAL);
		return false;
	}

	return true;
}

//������������Ʊ
bool CAttemperEngineSink::CheckJoinTableTicket(tagTableRule *pCfg, IServerUserItem *pIServerUserItem)
{
	//�ڲ�����, ��У��ָ��

	switch( pCfg->GameMode )
	{
	case TABLE_MODE_FK://����ģʽ
		{
			//�����ķ�������AA֧�������û���������
			if( (1 == pCfg->cbPayType) &&
				(0 == pCfg->byClubCreate) &&
				(pIServerUserItem->GetUserRoomCard() < pCfg->GameCountType) )
			{
				SendRequestFailure(pIServerUserItem, TEXT("�����ڽ���AA֧���ķ�����, �򷿿�����,�޷�����"), REQUEST_FAILURE_NORMAL);
				return false;
			}
			break;
		}
	case TABLE_MODE_JJC://������ģʽ
		{
			break;
		}
	case TABLE_MODE_GOLD://���ģʽ
		{
			//�����Ľ�ҳ�
			if(  pIServerUserItem->GetUserGold() < pCfg->dwLevelGold)
			{
				SendRequestFailure(pIServerUserItem, TEXT("�����ڽ����ҷ�,���Ҳ���,�޷�����"), REQUEST_FAILURE_NORMAL);
				return false;
			}
			break;
		}
	case TABLE_MODE_FK_GOLD://������ҳ�
		{
			//�����ķ�����ҳ�  У��һ:  AA֧�������û���������
			if( (1 == pCfg->cbPayType) && 
				(0 == pCfg->byClubCreate) &&
				(pIServerUserItem->GetUserRoomCard() < pCfg->GameCountType) )
			{
				SendRequestFailure(pIServerUserItem, TEXT("�����ڽ����ΪAA֧���ķ�����ҷ�, �򷿿�����,�޷�����"), REQUEST_FAILURE_NORMAL);
				return false;
			}

			//���ֲ��ķ�����ҳ� ���� �����ķ�����ҳ�
			if( pIServerUserItem->GetUserGold()  < pCfg->dwLevelGold)
			{
				SendRequestFailure(pIServerUserItem, TEXT("�����ڽ��뷿����ҷ�, ���Ҳ���,�޷�����"), REQUEST_FAILURE_NORMAL);
				return false;
			}
			break;
		}
	}

	return true;
}

//��鴴��������Ʊ
bool CAttemperEngineSink::CheckCreateTableTicket(tagTableRule *pCfg, IServerUserItem *pIServerUserItem)
{
	//�ڲ�����, ��У��ָ��
	//�� ���� ʹ��, ���ֲ���ʹ�øú���

	switch( pCfg->GameMode )
	{
	case TABLE_MODE_FK://����ģʽ
		{
			//У���û�����: ����֧�������û���������
			if(0 == pCfg->cbPayType && 
				(pIServerUserItem->GetUserRoomCard() < ((pCfg->GameCountType) * (pCfg->PlayerCount)) ))	  
			{
				SendRequestFailure(pIServerUserItem, TEXT("�����ڴ�������֧���ķ�����, �򷿿�����,�޷�����"), REQUEST_FAILURE_NORMAL);
				return false;
			}

			//У���û�������AA֧�������û���������
			if( (1 == pCfg->cbPayType) && 
				(pIServerUserItem->GetUserRoomCard() < pCfg->GameCountType) )
			{
				SendRequestFailure(pIServerUserItem, TEXT("�����ڴ���AA֧���ķ�����, �򷿿�����,�޷�����"), REQUEST_FAILURE_NORMAL);
				return false;
			}
			break;
		}
	case TABLE_MODE_JJC://������ģʽ
		{
			break;
		}
	case TABLE_MODE_GOLD://���ģʽ
		{
			break;
		}
	case TABLE_MODE_FK_GOLD://������ҳ�
		{
			//У���û�����: ����֧�������û���������
			if(0 == pCfg->cbPayType && 
				(pIServerUserItem->GetUserRoomCard() < ((pCfg->GameCountType) * (pCfg->PlayerCount)) ))	  
			{
				SendRequestFailure(pIServerUserItem, TEXT("�����ڴ�������֧���ķ�����ҷ�, �򷿿�����,�޷�����"), REQUEST_FAILURE_NORMAL);
				return false;
			}

			//У���û�������AA֧�������û���������
			if( (1 == pCfg->cbPayType) && 
				(pIServerUserItem->GetUserRoomCard() < pCfg->GameCountType) )
			{
				SendRequestFailure(pIServerUserItem, TEXT("�����ڴ���AA֧���ķ�����ҷ�, �򷿿�����,�޷�����"), REQUEST_FAILURE_NORMAL);
				return false;
			}

			//�����ķ�����ҳ�
			if( pIServerUserItem->GetUserGold()  < pCfg->dwLevelGold)
			{
				SendRequestFailure(pIServerUserItem, TEXT("�����ڴ���������ҷ�, ���Ҳ���,�޷�����"), REQUEST_FAILURE_NORMAL);
				return false;
			}
			break;
		}
	}

	return true;

}

//�������� -- ��Ҵ���
void CAttemperEngineSink::HallTableCreate(DWORD dwUserID, DWORD dwKindID, BYTE byGameType, DWORD dwTableID)
{
    STR_DBR_HALL_GOLD_TABLE_INFO Dbr;
	ZeroMemory(&Dbr, sizeof(Dbr));
	Dbr.dwUserID = dwUserID;
	Dbr.dwKindID = dwKindID;	
	Dbr.byGameType = byGameType;
	Dbr.dwTableID =dwTableID;
	Dbr.dwTableState = 0;
	Dbr.byMask =1;

	Dbr.dwGameID = (m_pGameServiceOption->dwServerID) & 0xFFFFFF00;

	//Ͷ������
	m_pIDBCorrespondManager->PostDataBaseRequest(INVALID_CHAIR, DBR_HALL_GOLD_TABLE_INFO, 0, &Dbr, sizeof(Dbr));
}


#pragma endregion