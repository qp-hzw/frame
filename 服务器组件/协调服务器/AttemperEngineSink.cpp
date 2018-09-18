#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "AttemperEngineSink.h"

#include "..\..\������\�������\WHEncrypt.h"
#include "..\..\��Ϣ����\STR_CMD_LogonServer.h"
//////////////////////////////////////////////////////////////////////////////////

//���캯��
CAttemperEngineSink::CAttemperEngineSink()
{
	//״̬����
	m_wCollectItem=INVALID_WORD;

	//���ñ���
	m_pInitParameter=NULL;
	m_pBindParameter=NULL;

	//�������
	m_pITimerEngine=NULL;
	m_pITCPNetworkEngine=NULL;

	return;
}

//��������
CAttemperEngineSink::~CAttemperEngineSink()
{
}

//�ӿڲ�ѯ
VOID * CAttemperEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAttemperEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAttemperEngineSink,Guid,dwQueryVer);
	return NULL;
}

//�����¼�
bool CAttemperEngineSink::OnAttemperEngineStart(IUnknownEx * pIUnknownEx)
{
	//�󶨲���
	m_pBindParameter=new tagBindParameter[MAX_CONTENT];
	ZeroMemory(m_pBindParameter,sizeof(tagBindParameter)*MAX_CONTENT);

	return true;
}

//ֹͣ�¼�
bool CAttemperEngineSink::OnAttemperEngineConclude(IUnknownEx * pIUnknownEx)
{
	//״̬����
	m_wCollectItem=INVALID_WORD;
	m_WaitCollectItemArray.RemoveAll();

	//���ñ���
	m_pITimerEngine=NULL;
	m_pITCPNetworkEngine=NULL;

	//ɾ������
	SafeDeleteArray(m_pBindParameter);

	//�������
	m_GlobalInfoManager.ResetData();

	return true;
}

//Ӧ���¼�
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID)
{
	//��ȡ����
	if (LOWORD(dwSocketID)>=MAX_CONTENT) return false;

	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//���ñ���
	pBindParameter->dwSocketID=dwSocketID;
	pBindParameter->dwClientAddr=dwClientAddr;
	pBindParameter->dwActiveTime=(DWORD)time(NULL);

	return true;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (Command.wMainCmdID)
	{
	case MDM_REGISTER:		//����ע��
		{
			return OnTCPNetworkMainRegister(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_CPD_LIST:	//������Ϣ
		{
			return OnTCPNetworkMainServiceInfo(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_TRANSFER:	//��ת����
		{
			return OnTCPNetworkMainTransfer(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_CS_USER_COLLECT:	//�û�����
		{
			return OnTCPNetworkMainUserCollect(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_WEB: //�������
		{
			return OnTCPNetworkMainManagerService(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	}

	return false;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID)
{
	//��ȡ��Ϣ
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//��Ϸ����
	if (pBindParameter->ServiceKind==ServiceKind_Game)
	{
		//��������
		WORD wBindIndex=LOWORD(dwSocketID);

		/* 1. ���ܴ��� */
		if (wBindIndex==m_wCollectItem)
		{
			//���ñ���
			m_wCollectItem=INVALID_WORD;

			//�����л�
			if (m_WaitCollectItemArray.GetCount()>0)
			{
				//��ȡ����
				INT_PTR nWaitCount=m_WaitCollectItemArray.GetCount();
				m_wCollectItem=m_WaitCollectItemArray[nWaitCount-1];

				//ɾ������
				m_WaitCollectItemArray.RemoveAt(nWaitCount-1);

				//������Ϣ
				DWORD dwSocketID=(m_pBindParameter+m_wCollectItem)->dwSocketID;
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_USER_COLLECT,SUB_CS_S_COLLECT_REQUEST);
			}
		}
		else
		{
			//ɾ���ȴ�
			for (INT_PTR i=0;i<m_WaitCollectItemArray.GetCount();i++)
			{
				if (wBindIndex==m_WaitCollectItemArray[i])
				{
					m_WaitCollectItemArray.RemoveAt(i);
					break;
				}
			}
		}

		/* 2. ֪ͨ��¼�� */
		//��������
		STR_CPO_PL_LIST_REMOVE_GAME ServerRemove;
		ZeroMemory(&ServerRemove,sizeof(ServerRemove));

		//ɾ��֪ͨ
		ServerRemove.dwServerID=pBindParameter->dwServiceID;
		m_pITCPNetworkEngine->SendDataBatch(MDM_CPD_LIST,CPO_PL_LIST_REMOVE_GAME,&ServerRemove,sizeof(ServerRemove),0L);

		//ע������
		m_GlobalInfoManager.DeleteServerItem(pBindParameter->dwServiceID);
	}

	//�㳡����
	if (pBindParameter->ServiceKind==ServiceKind_Logon)
	{
		//��������
		WORD wBindIndex=LOWORD(dwSocketID);
		tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

		//ע������
		m_GlobalInfoManager.DeletePlazaItem( static_cast<WORD>(pBindParameter->dwServiceID));
	}

	//�����Ϣ
	ZeroMemory(pBindParameter,sizeof(tagBindParameter));

	return false;
}

#pragma region  ����Ϣ�ַ�����
//ע��ģ��
bool CAttemperEngineSink::OnTCPNetworkMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case CPR_LP_REGISTER_LOGON:	//ע���¼��
		{
			//Ч������
			if (wDataSize!=sizeof(STR_CPR_LP_REGISTER_LOGON)) return false;

			//��Ϣ����
			STR_CPR_LP_REGISTER_LOGON * pCPR=(STR_CPR_LP_REGISTER_LOGON *)pData;

			//��Ч�ж�		//lee����½����ַ�����֣�ֻ�������õ���
			if (pCPR->szServerAddr[0]==0)
			{
				//��������
				STR_CPO_PGPL_REGISTER_FAILURE RegisterFailure;
				ZeroMemory(&RegisterFailure,sizeof(RegisterFailure));

				//���ñ���
				RegisterFailure.lErrorCode=0L;
				lstrcpyn(RegisterFailure.szDescribeString,TEXT("������ע��ʧ�ܣ��������ַ��Ϊ�գ�"),CountArray(RegisterFailure.szDescribeString));

				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_REGISTER,CPO_PGPL_REGISTER_FAILURE,&RegisterFailure,sizeof(RegisterFailure));

				//�ж�����
				m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);

				return true;
			}

			//���ð�
			WORD wBindIndex=LOWORD(dwSocketID);
			(m_pBindParameter+wBindIndex)->dwServiceID=wBindIndex;
			(m_pBindParameter+wBindIndex)->ServiceKind=ServiceKind_Logon;

			//��������
			tagGamePlaza GamePlaza;
			ZeroMemory(&GamePlaza,sizeof(GamePlaza));

			//��������
			GamePlaza.wPlazaID=wBindIndex;
			lstrcpyn(GamePlaza.szServerAddr,pCPR->szServerAddr,CountArray(GamePlaza.szServerAddr));

			//ע�᷿��
			m_GlobalInfoManager.ActivePlazaItem(wBindIndex,GamePlaza);

			//�����б�
			SendServerListItem(dwSocketID);

			//Ⱥ������
			m_pITCPNetworkEngine->AllowBatchSend(dwSocketID,true,0L);

			return true;
		}
	case CPR_GP_REGISTER_GAME:	//ע����Ϸ��
		{
			//Ч������
			if (wDataSize!=sizeof(STR_CPR_GP_REGISTER_GAME)) return false;

			//��Ϣ����
			STR_CPR_GP_REGISTER_GAME * pRegisterServer=(STR_CPR_GP_REGISTER_GAME *)pData;

			//ʧ�ܴ���
			if (m_GlobalInfoManager.SearchServerItem(pRegisterServer->dwServerID)!=NULL)
			{
				//��������
				STR_CPO_PGPL_REGISTER_FAILURE RegisterFailure;
				ZeroMemory(&RegisterFailure,sizeof(RegisterFailure));

				//���ñ���
				RegisterFailure.lErrorCode=0L;
				lstrcpyn(RegisterFailure.szDescribeString,TEXT("�Ѿ�������ͬ��ʶ����Ϸ������񣬷������ע��ʧ��"),CountArray(RegisterFailure.szDescribeString));

				//������Ϣ
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_REGISTER,CPO_PGPL_REGISTER_FAILURE,&RegisterFailure,sizeof(RegisterFailure));

				//�ж�����
				m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);

				return true;
			}

			//���ð�
			WORD wBindIndex=LOWORD(dwSocketID);
			(m_pBindParameter+wBindIndex)->ServiceKind=ServiceKind_Game;
			(m_pBindParameter+wBindIndex)->dwServiceID=pRegisterServer->dwServerID;

			//��������
			tagGameServer GameServer;
			ZeroMemory(&GameServer,sizeof(GameServer));

			//��������
			GameServer.wServerID=pRegisterServer->dwServerID;
			GameServer.wGameServerPort=pRegisterServer->wGameServerPort;
			GameServer.dwSubGameVersion=pRegisterServer->dwSubGameVersion;
			lstrcpyn(GameServer.szServerName,pRegisterServer->szServerName,CountArray(GameServer.szServerName));
			lstrcpyn(GameServer.szGameServerAddr,pRegisterServer->szGameServerAddr,CountArray(GameServer.szGameServerAddr));

			//������� -- ֪ͨ��Ϸ�� �����ں�
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_REGISTER,CPO_PGPL_REGISTER_SUCESS);

			//ע�᷿��
			m_GlobalInfoManager.ActiveServerItem(wBindIndex,GameServer);

			//Ⱥ������
			m_pITCPNetworkEngine->SendDataBatch(MDM_CPD_LIST, CPO_PL_LIST_INSERT_GAME, &GameServer, sizeof(GameServer), 0L);

			//Ⱥ������ TODONOWû������ʲô��
			m_pITCPNetworkEngine->AllowBatchSend(dwSocketID,true,0L);

			//����֪ͨ TODONOW û������ʲô��
			if (m_wCollectItem==INVALID_WORD)
			{
				m_wCollectItem=wBindIndex;
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_USER_COLLECT,SUB_CS_S_COLLECT_REQUEST);
			}
			else m_WaitCollectItemArray.Add(wBindIndex);

			return true;
		}
	}

	return false;
}

//�б�����
bool CAttemperEngineSink::OnTCPNetworkMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case CPR_GP_LIST_GAME_ONLINE:	//�������� ֪ͨ���ݿ�
		{
			//Ч������
			if (wDataSize!=sizeof(STR_CPR_GP_LIST_GAME_ONLINE)) return false;

			//��Ϣ����
			STR_CPR_GP_LIST_GAME_ONLINE * pServerOnLine=(STR_CPR_GP_LIST_GAME_ONLINE *)pData;

			//��ȡ����
			WORD wBindIndex=LOWORD(dwSocketID);
			tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

			//����Ч��
			if (pBindParameter->ServiceKind!=ServiceKind_Game) return false;

			//���ҷ���
			DWORD wServerID=pBindParameter->dwServiceID;
			CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.SearchServerItem(wServerID);

			//��������
			if (pGlobalServerItem!=NULL)
			{
				//���ñ���
				pGlobalServerItem->m_GameServer.dwOnlineCount=pServerOnLine->dwOnLineCount;

				//��������
				STR_CPR_PL_LIST_GAME_ONLINE ServerOnLine;
				ZeroMemory(&ServerOnLine,sizeof(ServerOnLine));
				ServerOnLine.wServerID=wServerID;
				ServerOnLine.dwOnLineCount=pServerOnLine->dwOnLineCount;

				//����֪ͨ
				m_pITCPNetworkEngine->SendDataBatch(MDM_CPD_LIST,CPO_PL_LIST_GAME_ONLINE,&ServerOnLine,sizeof(ServerOnLine),0L);
			}

			return true;
		}
	}

	return false;
}

//��ת����
bool CAttemperEngineSink::OnTCPNetworkMainTransfer(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case CPR_GP_CLUB_TABLE_INFO: //club���ֲ�������Ϣ����
		{
			//Ч������
			if (wDataSize!=sizeof(STR_CMD_LC_CLUB_TABLE_LIST)) return true;
			
			//����֪ͨ -- ȫ����¼��
			m_pITCPNetworkEngine->SendDataBatch(MDM_TRANSFER,CPO_PL_CLUB_TABLE_INFO,pData,wDataSize,0L);
			return true;
		}
	case CPR_GP_CLUB_PLAYER_INFO: //club���ֲ������Ϣ����
		{
			//Ч������
			if (wDataSize!=sizeof(STR_CMD_LC_CLUB_TABLE_USER_LIST)) return false;
			
			//����֪ͨ -- ȫ����¼��
			m_pITCPNetworkEngine->SendDataBatch(MDM_TRANSFER,CPO_PL_CLUB_PLAYER_INFO,pData,wDataSize,0L);
			return true;
		}
	case CPR_LP_CLUB_TABLE_DISSOLVE :	//��¼��֪ͨЭ����, Э����֪ͨ��Ϸ�� ��ɢ����
		{
			//У������
			if (wDataSize!=sizeof(STR_CPR_LP_CLUB_TABLE_DISSOLVE)) return true;

			//֪ͨ�ض���Ϸ�� ��GameIDΪ�ж�����
			//TOODNOW added by WangChengQing ��Ϊ��֪��Э�������ͨ��gmaeID�ҵ���Ϸ����socketID
			//���Դ˴���֪ͨ������Ϸ����Ȼ����Ϸ������У��
			//֮��ĵ�

			STR_CPR_LP_CLUB_TABLE_DISSOLVE *pCPR = (STR_CPR_LP_CLUB_TABLE_DISSOLVE*) pData;

			STR_CPO_PG_CLUB_TABLE_DISSOLVE CPO;
			CPO.dwGameID = pCPR->dwGameID;
			CPO.dwTableID = pCPR->dwTableID;

			//����֪ͨ -- ȫ����Ϸ��
			m_pITCPNetworkEngine->SendDataBatch(MDM_TRANSFER,CPO_PG_CLUB_TABLE_DISSOLVE,&CPO,sizeof(CPO),0L);

			return true;
		}
		case CPR_LP_CREATE_TABLE:	//�������� ��ѯ���õ�GameID 
		{
			//У������
			if (wDataSize!=sizeof(STR_CPR_LP_CREATE_TABLE)) return true;
			STR_CPR_LP_CREATE_TABLE *pCPR = (STR_CPR_LP_CREATE_TABLE*) pData;


			DWORD dwMatchGameID = 0;
			DWORD dwMinOnlineCount = 60*4; //ÿ�������������Ϊ240 ����Ժ��޸�, ����Ҳ��Ҫ�޸� added by WangChengQing
			//��ȡ���õ�, ��ǰ������С��GameID -- ���ظ��ݵ�ǰ�������ж�
			for (int j = 0; j < MAX_CONTENT; j++)
			{
				//��ȡ����Ϸ��KindIDƥ�������
				tagBindParameter * pBindParameter=(m_pBindParameter+j);
				if ( pBindParameter &&
					(pBindParameter->ServiceKind == ServiceKind_Game) && 
					(((pBindParameter->dwServiceID &0x00FF0000) >>16) == pCPR->dwKindID ))
				{
					CGlobalServerItem * pGlobalServerItem = m_GlobalInfoManager.SearchServerItem(pBindParameter->dwServiceID);
					if(NULL == pGlobalServerItem) continue;

					if(pGlobalServerItem->m_GameServer.dwOnlineCount < dwMinOnlineCount)
					{
						dwMinOnlineCount = pGlobalServerItem->m_GameServer.dwOnlineCount;
						dwMatchGameID = pGlobalServerItem->m_GameServer.wServerID;
					}
				}
			}


			//CPO����
			STR_CPO_PL_CREATE_TABLE CPO;
			ZeroMemory(&CPO, sizeof(CPO));

			if(0 == dwMatchGameID)
			{
				CPO.dwResultCode = 1;
				memcpy(CPO.szDescribeString, TEXT("δ�ҵ����ʵ���Ϸ������"), sizeof(CPO.szDescribeString));
				CPO.dwSocketID = pCPR->dwSocketID;
			}
			else
			{
				CPO.dwResultCode = 0;
				CPO.dwGameID = dwMatchGameID & 0x00FFFF00;
				CPO.dwSocketID = pCPR->dwSocketID;
			}

			//������Ϣ
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_TRANSFER,CPO_PL_CREATE_TABLE, &CPO, sizeof(CPO));
			return true;
		}

	}

	return false;
}

//�û�����
bool CAttemperEngineSink::OnTCPNetworkMainUserCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_USER_ENTER:		//�û�����
		{
			//Ч������
			if (wDataSize!=sizeof(CMD_CS_C_UserEnter)) return false;

			//��Ϣ����
			CMD_CS_C_UserEnter * pUserEnter=(CMD_CS_C_UserEnter *)pData;
			pUserEnter->szNickName[CountArray(pUserEnter->szNickName)-1]=0;

			//��ȡ����
			WORD wBindIndex=LOWORD(dwSocketID);
			tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

			//����Ч��
			if (pBindParameter->ServiceKind!=ServiceKind_Game) return false;

			//��������
			tagGlobalUserInfo GlobalUserInfo;
			ZeroMemory(&GlobalUserInfo,sizeof(GlobalUserInfo));

			//���ñ���
			GlobalUserInfo.dwUserID=pUserEnter->dwUserID;
			GlobalUserInfo.dwGameID=pUserEnter->dwGameID;
			lstrcpyn(GlobalUserInfo.szNickName,pUserEnter->szNickName,CountArray(GlobalUserInfo.szNickName));

			//������Ϣ
			GlobalUserInfo.cbGender=pUserEnter->cbGender;
			GlobalUserInfo.cbMemberOrder=pUserEnter->cbMemberOrder;
			GlobalUserInfo.cbMasterOrder=pUserEnter->cbMasterOrder;

			//�����û�
			m_GlobalInfoManager.ActiveUserItem(GlobalUserInfo,pBindParameter->dwServiceID);

			return true;
		}
	case SUB_CS_C_USER_LEAVE:		//�û��뿪
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_CS_C_UserLeave));
			if (wDataSize!=sizeof(CMD_CS_C_UserLeave)) return false;

			//��Ϣ����
			CMD_CS_C_UserLeave * pUserLeave=(CMD_CS_C_UserLeave *)pData;

			//��ȡ����
			WORD wBindIndex=LOWORD(dwSocketID);
			tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

			//����Ч��
			ASSERT(pBindParameter->ServiceKind==ServiceKind_Game);
			if (pBindParameter->ServiceKind!=ServiceKind_Game) return false;

			//ɾ���û�
			m_GlobalInfoManager.DeleteUserItem(pUserLeave->dwUserID,pBindParameter->dwServiceID);

			return true;
		}
	case SUB_CS_C_USER_FINISH:		//�û����
		{
			//��ȡ����
			WORD wBindIndex=LOWORD(dwSocketID);
			tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

			//����Ч��
			ASSERT((m_wCollectItem==wBindIndex)&&(pBindParameter->ServiceKind==ServiceKind_Game));
			if ((m_wCollectItem!=wBindIndex)||(pBindParameter->ServiceKind!=ServiceKind_Game)) return false;

			//���ñ���
			m_wCollectItem=INVALID_WORD;

			//�����л�
			if (m_WaitCollectItemArray.GetCount()>0)
			{
				//�л�����
				INT_PTR nWaitCount=m_WaitCollectItemArray.GetCount();
				m_wCollectItem=m_WaitCollectItemArray[nWaitCount-1];

				//ɾ������
				m_WaitCollectItemArray.RemoveAt(nWaitCount-1);

				//������Ϣ
				DWORD dwSocketID=(m_pBindParameter+m_wCollectItem)->dwSocketID;
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_USER_COLLECT,SUB_CS_S_COLLECT_REQUEST);
			}

			return true;
		}
	case SUB_CS_C_USER_OFFLINE:	//�û�����
		{
			//Ч������
			if (wDataSize!=sizeof(STR_SUB_CS_C_USER_OFFLINE)) return false;

			//��Ϣ����
			STR_SUB_CS_C_USER_OFFLINE * pUserOffLine=(STR_SUB_CS_C_USER_OFFLINE *)pData;

			STR_SUB_CS_C_USER_OFFLINE data;
			data.dwUserID = pUserOffLine->dwUserID;
			data.dwGameID = pUserOffLine->dwGameID;
			data.byMask = pUserOffLine->byMask;
			
			//����֪ͨ -- ȫ����¼��
			m_pITCPNetworkEngine->SendDataBatch(MDM_CPD_LIST,SUB_CS_C_USER_OFFLINE_B,&data,sizeof(STR_SUB_CS_C_USER_OFFLINE),0L);
			
			return true;
		}
	}

	return false;
}

//�������
bool CAttemperEngineSink::OnTCPNetworkMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case CPR_WP_WEB_MARQUEE: //�������Ϣ
		{
			//Ч���С
			if (wDataSize!=sizeof(STR_CPR_WP_WEB_MARQUEE)) 
			{
				return false;
			}

			//У������
			STR_CPR_WP_WEB_MARQUEE *pSub = (STR_CPR_WP_WEB_MARQUEE*) pData;
			if(1 != pSub->byMask) return false;

			//����֪ͨ���ͻ���(ֻ���͸�����)
			SendDataToPlaza(INVALID_WORD, MDM_WEB, CPO_PL_WEB_MARQUEE, pData, wDataSize);
			////Ⱥ�����������ӵķ�����
			//m_pITCPNetworkEngine->SendDataBatch(MDM_WEB,CPO_PL_WEB_MARQUEE, &pSendLobbyMessage,sizeof(CMD_CS_MarqueeMessage),0L);
			return true;
		}
	case CPR_WP_WEB_SYSTEM_MESSAGE:	//����Ƶ�� -- ϵͳ��Ϣ
		{
			//Ч���С
			if (wDataSize!=sizeof(STR_CPR_WP_WEB_SYSTEM_MESSAGE))
			{
				return false;
			}

			//У������
			STR_CPR_WP_WEB_SYSTEM_MESSAGE *pSub = (STR_CPR_WP_WEB_SYSTEM_MESSAGE*) pData;
			
			LPCTSTR pszDescribe=pSub->szMessage;
			CTraceService::TraceString(pszDescribe,TraceLevel_Normal);

			//����֪ͨ���ͻ���(ֻ���͸�����)
			SendDataToPlaza(INVALID_WORD, MDM_WEB, CPO_PL_WEB_SYSTEM_MESSAGE, pData, wDataSize);
			return true;
		}
	}

	return false;
}
#pragma endregion 

//�����б�
bool CAttemperEngineSink::SendServerListItem(DWORD dwSocketID)
{
	//��������
	WORD wPacketSize=0L;
	POSITION Position=NULL;
	BYTE cbBuffer[SOCKET_TCP_PACKET];

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CPD_LIST,SUB_CS_S_SERVER_INFO);

	//�ռ�����
	do
	{
		//��������
		if ((wPacketSize+sizeof(tagGameServer))>sizeof(cbBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CPD_LIST,CPO_PL_LIST_INSERT_GAME_LIST,cbBuffer,wPacketSize);
			wPacketSize=0;
		}

		//��ȡ����
		tagGameServer * pGameServer=(tagGameServer *)(cbBuffer+wPacketSize);
		CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.EnumServerItem(Position);		//���ÿ����Ϸ����Ϣ

		//��������
		if (pGlobalServerItem!=NULL)
		{
			wPacketSize+=sizeof(tagGameServer);
			CopyMemory(pGameServer,&pGlobalServerItem->m_GameServer,sizeof(tagGameServer));
		}

	} while (Position!=NULL);

	//��������
	if (wPacketSize>0) 
		m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CPD_LIST,CPO_PL_LIST_INSERT_GAME_LIST,cbBuffer,wPacketSize);

	//�������
	m_pITCPNetworkEngine->SendData(dwSocketID,MDM_REGISTER,CPO_PGPL_REGISTER_SUCESS);

	return true;
}

//���䷢��
bool CAttemperEngineSink::SendDataToGame(DWORD wServerID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//���ҷ���
	CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.SearchServerItem(wServerID);
	if (pGlobalServerItem==NULL) return false;

	//��ȡ����
	WORD wBindIndex=pGlobalServerItem->GetIndex();
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//��������
	DWORD dwSocketID=pBindParameter->dwSocketID;
	m_pITCPNetworkEngine->SendData(dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);

	return true;
}

//�û�����
bool CAttemperEngineSink::SendDataToUser(DWORD wServerID, DWORD dwUserID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return true;
}

//��������
bool CAttemperEngineSink::SendDataToPlaza( WORD wSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize )
{
	if ( wSocketID != INVALID_WORD)
	{
		return m_pITCPNetworkEngine->SendData(wSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);
	}

	POSITION Position = NULL;

	do
	{
		CGlobalPlazaItem * pGlobalPlazaItem = m_GlobalInfoManager.EnumPlazaItem(Position);
		if (pGlobalPlazaItem != NULL)
		{
			WORD wBindIndex = pGlobalPlazaItem->m_wIndex;
			if(wBindIndex < MAX_CONTENT)
			{
				tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);
				DWORD dwSocketID=pBindParameter->dwSocketID;
				if(!m_pITCPNetworkEngine->SendData(dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize))
				{
					return false;
				}
			}
		}

	} while (Position != NULL);

	return true;
}

//��Ϸ����
bool CAttemperEngineSink::SendDataToGameAllRoom( WORD wKindID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize )
{
	//��������
	POSITION Position=NULL;
	//���ҷ���
	do
	{
		CGlobalServerItem * pGlobalServerItem=m_GlobalInfoManager.EnumServerItem(Position);
		if (pGlobalServerItem != NULL)
		{
			if (pGlobalServerItem->GetKindID() == wKindID)
			{
				SendDataToGame(pGlobalServerItem->GetServerID(),wMainCmdID,wSubCmdID,pData,wDataSize);
			}
		}
	} while (Position != NULL);

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
