#include "HandleFromGate.h"
#include "GameCtrl.h"
#include "TableManager.h"
#include <iostream>
#include "RoomRuleManager.h"

bool CHandleFromGate::HandlePacket(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
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
	}
}

bool CHandleFromGate::HandlePacketDB(WORD wRequestID, DWORD dwSocketID, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
	case DBO_CG_LOGON_USERID:			//ID��¼����
		{
			return On_CMD_GC_Logon_UserID(dwSocketID, pData, wDataSize);
		}
	case DBO_CG_USER_JOIN_GROUP_ROOM:	//��������Ȧ���䷵��
		{
			return On_CMD_GC_User_JoinGroupRoom(dwSocketID, pData, wDataSize);
		}
	case DBO_SG_MODIFY_USER_TREASURE:	//�޸��û��Ƹ���Ϣ����
		{
			return On_CMD_GC_User_ModifyUserTreasure(dwSocketID, pData, wDataSize);
		}

	case DBO_GR_LOAD_OFFLINE:		//���ض�����������
		{
			return On_DBO_GR_LOAD_OFFLINE(dwSocketID,pData,wDataSize);
		}
	case DBO_GC_CLUB_CREATE_TABLE:	//club����Ȧ��������
		{
			return On_CMD_CG_CLUB_CREATE_TABLE(dwSocketID,pData,wDataSize);
		}
	case DBO_GC_USER_JOIN_TABLE_NO_PASS://�������� ����Ҫ����
		{
			return On_CMD_GC_USER_JOIN_TABLE_NO_PASS(dwSocketID,pData,wDataSize);
		}
	case DBO_GC_CLUB_UPDATE_TABLE_INFO:// ����������Ϣ
		{
			if(sizeof(STR_CMD_LC_CLUB_TABLE_LIST) != wDataSize) return false;
			g_TCPSocketEngine->SendData(MDM_TRANSFER, CPR_GP_CLUB_TABLE_INFO, pData, wDataSize);
			return true;;
		}
	case DBO_GC_CLUB_UPDATE_PLAYER_INFO://���������Ϣ
		{
			if(sizeof(STR_CMD_LC_CLUB_TABLE_USER_LIST) != wDataSize) return false;
			g_TCPSocketEngine->SendData(MDM_TRANSFER, CPR_GP_CLUB_PLAYER_INFO, pData, wDataSize);
			return true;
		}
	case DBO_GC_JOIN_TABLE://�������� ���ݿ�У��
		{
			return On_CMD_GC_JOIN_TABLE(dwSocketID,pData,wDataSize);
		}
	case DBO_GC_USER_JOIN_TABLE_HALL_GOLD: //�������� -- ��Ҵ�������
		{
			return On_CMD_GC_USER_JOIN_TABLE_HALL_GOLD(dwSocketID,pData,wDataSize);
		}
	}
}

//��¼����
bool CHandleFromGate::OnTCPNetworkMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
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
bool CHandleFromGate::OnTCPNetworkMainUser(WORD wSubCmdID, VOID *pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CG_USER_SITDOWN:		//�û�����
		{
			return On_SUB_CG_User_SitDown(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_STANDUP:		//�û�����
		{
			return On_SUB_CG_User_StandUp(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_READY:			//�û�׼��
		{
			return On_SUB_CG_User_Ready(pData, wDataSize, dwSocketID);
		}
	case SUB_CG_USER_INVITE_USER:	//�����û�
		{
			return On_SUB_CG_User_InviteUser(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_KICK_USER:		//�߳��û�
		{
			return On_SUB_CG_User_KickUser(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_CHAIR_REQ:    //�������λ��
		{
			return OnTCPNetworkSubUserChairReq(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_CREATE_ROOM:	//���봴������
		{
			return On_SUB_CG_USER_CREATE_ROOM(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_SET_ROOM_RULE:	//������� ����
		{
			return On_SUB_CG_USER_SET_ROOM_RULE(pData,wDataSize,dwSocketID);
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
	case SUB_RG_USER_ASK_DISMISS:		//���������ɢ����
		{
			return On_SUB_RG_USER_ASK_DISMISS(pData, wDataSize, dwSocketID);
		}
	case SUB_RG_USER_VOTE_DISMISS:		//�����ɢ����
		{
			return On_SUB_RG_USER_VOTE_DISMISS(pData, wDataSize, dwSocketID);
		}
	}

	return false;
}
//��Ϸ����
bool CHandleFromGate::OnTCPNetworkMainGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��ȡ��Ϣ
	CPlayer * pIServerUserItem= CPlayerManager::FindPlayerBySocketID(dwSocketID);

	//�û�Ч��
	if (pIServerUserItem==NULL) return false;

	//�������
	DWORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	if ((wTableID==INVALID_TABLE)||(wChairID==INVALID_CHAIR)) return true;

	//��Ϣ���� 
	CTableFrame * pTableFrame=CTableManager::FindTableByTableID(wTableID);
	if (pTableFrame == NULL)
	{
		CLog::Log(log_error, "pTableFrame == NULL δ�ҵ�����");
		return false;
	}

	return pTableFrame->OnEventSocketGame(wSubCmdID,pData,wDataSize,pIServerUserItem);
}

//��ܴ���
bool CHandleFromGate::OnTCPNetworkMainFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��ȡ��Ϣ
	CPlayer * pIServerUserItem=CPlayerManager::FindPlayerBySocketID(dwSocketID);

	//�û�Ч��
	if (pIServerUserItem==NULL)
	{
		CLog::Log(log_debug, "100, 1 ��Ҳ�����");
		return false;
	}

	//���Ӵ���
	DWORD wTableID=pIServerUserItem->GetTableID();
	CTableFrame * pTableFrame=CTableManager::FindTableByTableID(wTableID);
	if (pTableFrame == NULL)
	{
		CLog::Log(log_debug, "100, 1 ���Ӳ�����");
		return false;
	}

	return pTableFrame->OnEventSocketFrame(wSubCmdID,pData,wDataSize,pIServerUserItem);
}


#pragma region ��¼ģ�� MDM_LOGON
//I D ��¼
bool CHandleFromGate::On_SUB_CG_Logon_UserID(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	if (wDataSize != sizeof(STR_SUB_CG_LOGON_USERID)) return false;
	STR_SUB_CG_LOGON_USERID *pLogonUserID = (STR_SUB_CG_LOGON_USERID *)pData;
	
	//��������
	STR_DBR_CG_LOGON_USERID LogonUserID;
	ZeroMemory(&LogonUserID,sizeof(LogonUserID));

	//��������
	LogonUserID.dwUserID = pLogonUserID->dwUserID;
	//LogonUserID.dwClientAddr = pBindParameter->dwClientAddr;
	LogonUserID.dLatitude = pLogonUserID->dLatitude;
	LogonUserID.dLongitude = pLogonUserID->dLongitude;
	lstrcpyn(LogonUserID.szPassword,pLogonUserID->szPassword,CountArray(LogonUserID.szPassword));
	lstrcpyn(LogonUserID.szMachineID,pLogonUserID->szMachineID,CountArray(LogonUserID.szMachineID));

	//Ͷ������
	return g_GameCtrl->PostDataBaseRequest(DBR_CG_LOGON_USERID, dwSocketID, &LogonUserID,sizeof(LogonUserID));
}

//ID��¼����
bool CHandleFromGate::On_CMD_GC_Logon_UserID(DWORD dwSocketID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize != sizeof(STR_DBO_CG_LOGON_USERID)) return false;
	STR_DBO_CG_LOGON_USERID *pDBOLogon = (STR_DBO_CG_LOGON_USERID *)pData;

	//���͵�¼���
	STR_CMD_GC_LOGON_USERID logon;
	ZeroMemory(&logon, sizeof(STR_CMD_GC_LOGON_USERID));
	logon.lResultCode = pDBOLogon->lResultCode;
	logon.dwKindID = g_GameCtrl->GetKindID();
	lstrcpyn(logon.szDescribeString, pDBOLogon->szDescribeString, CountArray(logon.szDescribeString));

	//��������
	g_GameCtrl->SendData(dwSocketID, MDM_GR_LOGON, CMD_GC_LOGON_USERID, &logon, sizeof(STR_CMD_GC_LOGON_USERID));

	//��¼ʧ��, �˳�
	if(pDBOLogon->lResultCode != 0)
		return true;


	//�û������ж�
	CPlayer *player = CPlayerManager::FindPlayerByID(pDBOLogon->dwUserID);
	if(player == NULL)
	{
		ActiveUserItem(dwSocketID, pDBOLogon); //����
	}
	else //�������� && �ظ���¼
	{
		player->SetSocketID(dwSocketID);

		//�ظ���¼, ��Ҫ���ͻ��˷����ظ���Ϣ

		//��������
		tagOfflineUser data;
		data.dwUserID = pDBOLogon->dwUserID;
		data.byMask = 2; //��ʾɾ�������û�
		CLog::Log(log_debug, "2, 4 delete player");
		g_TCPSocketEngine->SendData(MDM_USER, SUB_CS_C_USER_OFFLINE,&data,sizeof(tagOfflineUser));
	}

	return true;
}

//ID��¼�ɹ��������û�
void CHandleFromGate::ActiveUserItem( DWORD dwSocketID, STR_DBO_CG_LOGON_USERID *pDBOLogon)
{
	if ( NULL == pDBOLogon )
	{
		return;
	}

	//�û�����
	tagUserInfo UserInfo;
	ZeroMemory(&UserInfo,sizeof(UserInfo));

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
	UserInfo.dwLogonTime=(DWORD)time(NULL);
	UserInfo.dwInoutIndex=pDBOLogon->dwInoutIndex;

	//�û�Ȩ��
	UserInfo.dwUserRight=pDBOLogon->dwUserRight;
	UserInfo.dwMasterRight=pDBOLogon->dwMasterRight;

	//��������
	bool bAndroidUser = false;						//�û����ͣ�����/������
	UserInfo.bAndroidUser = bAndroidUser;
	UserInfo.lRestrictScore=0L;//����ÿ�ַⶥ
	lstrcpyn(UserInfo.szPassword,pDBOLogon->szPassword,CountArray(UserInfo.szPassword));

	//������Ϣ
	lstrcpyn(UserInfo.szMachineID,pDBOLogon->szMachineID,CountArray(UserInfo.szMachineID));

	//�����û� -- �����û���Ϣ
	CPlayerManager::InsertPlayer(dwSocketID, UserInfo);
}

#pragma endregion


//���ض�����������
bool CHandleFromGate::On_DBO_GR_LOAD_OFFLINE(DWORD dwSocketID, VOID * pData, WORD wDataSize)
{
	//����У��
	if(sizeof(BYTE ) != wDataSize) return false;

	//m_GameProgress =  *((BYTE*)pData);

	return true;
}

/*********************************************������Ϣ 3 �û����*********************************************************/
//�û�����
bool CHandleFromGate::On_SUB_CG_User_SitDown(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	if (wDataSize!=sizeof(CMD_GR_UserSitDown)) return false;

	//Ч������
	CMD_GR_UserSitDown * pUserSitDown=(CMD_GR_UserSitDown *)pData;

	//��ȡ�û�
	CPlayer * pIServerUserItem=CPlayerManager::FindPlayerBySocketID(dwSocketID);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��Ϣ����
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

	//�ظ��ж�
	//if (pUserSitDown->wTableID < MAX_TABLE)
	{
		CTableFrame * pTableFrame=CTableManager::FindTableByTableID(pUserSitDown->wTableID);
		//if (pTableFrame->GetTableUserItem(pUserSitDown->wChairID)==pIServerUserItem) return true;
	}

	//�û��ж�
	if (cbUserStatus==US_PLAYING)
	{
		SendRequestFailure(pIServerUserItem,TEXT("��������Ϸ�У���ʱ�����뿪�����Ƚ�����ǰ��Ϸ��"),0);
		return true;
	}


	//�뿪����
	if (wTableID!=INVALID_TABLE)
	{
		CTableFrame * pTableFrame=CTableManager::FindTableByTableID(wTableID);
		if (pTableFrame->PlayerUpTable(pIServerUserItem)==false) return true;
	}

	//�������
	WORD wRequestTableID=pUserSitDown->wTableID;
	WORD wRequestChairID=pUserSitDown->wChairID;

	//���ӵ���
	if (wRequestTableID>=CTableManager::TableCount())
	{
		//��ʼ����
		WORD wStartTableID=0;

		//��̬����
		bool bDynamicJoin=true;

		//Ѱ��λ��
		for (WORD i=wStartTableID;i<CTableManager::TableCount();i++)
		{
			CTableFrame* pTable = CTableManager::FindTableByIndex(i);

			//��Ϸ״̬
			if(NULL == pTable )
				continue;

			//��ȡ��λ
			WORD wNullChairID=pTable->GetNullChairID();

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
	{
		//Ч�����
		if (wRequestTableID>=CTableManager::TableCount()) return false;

		CTableFrame* pTable = CTableManager::FindTableByTableID(wRequestTableID);
		if(NULL == pTable) return false;
		//���ҿ�λ
		wRequestChairID=pTable->GetNullChairID();

		//����ж�
		if (wRequestChairID==INVALID_CHAIR)
		{
			SendRequestFailure(pIServerUserItem,TEXT("���ڴ���Ϸ����ʱû�п������������λ���ˣ���ѡ���������Ϸ����"),0);
			return true;
		}
	}

	//���´���
	CTableFrame * pTableFrame=CTableManager::FindTableByTableID(wRequestTableID);
	if(pTableFrame != NULL) 
		pTableFrame->PlayerSitTable(pIServerUserItem, wRequestChairID);

	return true;
}

//�û�����
bool CHandleFromGate::On_SUB_CG_User_StandUp(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//���ݰ�У��
	ASSERT(wDataSize == sizeof(STR_SUB_CG_USER_STANDUP));
	if (wDataSize != sizeof(STR_SUB_CG_USER_STANDUP)) 
		return false;

	//��ȡ�û�
	CPlayer *pIServerUserItem = CPlayerManager::FindPlayerBySocketID(dwSocketID);

	//�û�Ч��
	if (pIServerUserItem == NULL) return false;

	//��������
	STR_SUB_CG_USER_STANDUP *pUserStandUp = (STR_SUB_CG_USER_STANDUP *)pData;

	//Ч�����Ӻ�����
	if (pUserStandUp->wTableID >= (WORD)CTableManager::TableCount()) 
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
		CTableFrame *pTableFrame = CTableManager::FindTableByTableID(wTableID);
		if ( NULL == pTableFrame )
			return false;

		//������򣺽�һ����ģʽ��̬����
		tagTableRule *pTableCfg = (tagTableRule*)pTableFrame->GetCustomRule();
		if(pTableCfg != NULL &&  pTableCfg->GameMode == TABLE_MODE_GOLD)
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
		CTableFrame *pTableFrame = CTableManager::FindTableByTableID(wTableID);
		if (NULL == pTableFrame) 
			return true;
		//�û�վ��
		if ( !pTableFrame->PlayerUpTable(pIServerUserItem) ) 
			return true;
	}

	return true;
}

//�û�׼��
bool CHandleFromGate::On_SUB_CG_User_Ready(VOID * pData, WORD wDataSize, DWORD dwSocketID) 
{
	//��ȡ�û�
	CPlayer * pIServerUserItem = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	//У��
	if (pIServerUserItem == NULL)
		return false;

	//��ȡ����
	CTableFrame *pTableFrame = CTableManager::FindTableByTableID(pIServerUserItem->GetTableID());
	if (pTableFrame == NULL)
		return false;

	//׼������
	pTableFrame->PlayerReady(pIServerUserItem->GetChairID(), pIServerUserItem);

	return true;
}

//�����û�
bool CHandleFromGate::On_SUB_CG_User_InviteUser(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_GR_UserInviteReq));
	if (wDataSize!=sizeof(CMD_GR_UserInviteReq)) return false;

	//��Ϣ����
	CMD_GR_UserInviteReq * pUserInviteReq=(CMD_GR_UserInviteReq *)pData;

	//��ȡ�û�
	WORD wBindIndex = LOWORD(dwSocketID);
	CPlayer * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//Ч��״̬
	if (pUserInviteReq->wTableID==INVALID_TABLE) return true;
	if (pIServerUserItem->GetTableID()!=pUserInviteReq->wTableID) return true;
	if (pIServerUserItem->GetUserStatus()==US_PLAYING) return true;
	if (pIServerUserItem->GetUserStatus()==US_OFFLINE) return true;

	//Ŀ���û�
	CPlayer * pITargetUserItem=CPlayerManager::FindPlayerByID(pUserInviteReq->dwUserID);
	if (pITargetUserItem==NULL) return true;
	if (pITargetUserItem->GetUserStatus()==US_PLAYING) return true;

	//������Ϣ
	CMD_GR_UserInvite UserInvite;
	memset(&UserInvite,0,sizeof(UserInvite));
	UserInvite.wTableID=pUserInviteReq->wTableID;
	UserInvite.dwUserID=pIServerUserItem->GetUserID();
	g_GameCtrl->SendData(pITargetUserItem, MDM_USER, CMD_GC_USER_INVITE_USER, &UserInvite, sizeof(UserInvite));

	return true;
}

//�߳��û�
bool CHandleFromGate::On_SUB_CG_User_KickUser(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	if (wDataSize!=sizeof(CMD_GR_KickUser)) return false;

	//��������
	CMD_GR_KickUser * pKickUser=(CMD_GR_KickUser *)pData;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	CPlayer * pIServerUserItem=GetBindUserItem(wBindIndex);

	//Ŀ���û�
	CPlayer * pITargetUserItem = CPlayerManager::FindPlayerByID(pKickUser->dwTargetUserID);
	if(pITargetUserItem==NULL) return true;

	//�û�Ч��
	if ((pIServerUserItem==NULL)||(pIServerUserItem->GetMemberOrder()<=pITargetUserItem->GetMemberOrder())) return false;

	//�û�״̬
	if(pITargetUserItem->GetUserStatus()==US_PLAYING)
	{
		//��������
		TCHAR szMessage[256]=TEXT("");
		_sntprintf_s(szMessage,CountArray(szMessage),TEXT("������� [ %s ] ������Ϸ��,�����ܽ����߳���Ϸ��"),pITargetUserItem->GetNickName());

		//������Ϣ
		g_GameCtrl->SendRoomMessage(pIServerUserItem,szMessage,0);
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
			g_GameCtrl->SendRoomMessage(pIServerUserItem,szMessage,0);

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
		g_GameCtrl->SendGameMessage(pITargetUserItem,szMessage,0);

		CTableFrame * pTableFrame=CTableManager::FindTableByTableID(wTargerTableID);
		if (pTableFrame->PlayerUpTable(pITargetUserItem)==false) return true;
	}

	return true;
}

//�������λ��  TODONOW �л������߼�  WangChengQing
bool CHandleFromGate::OnTCPNetworkSubUserChairReq(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	return true;
}


//���������ɢ����
bool CHandleFromGate::On_SUB_RG_USER_ASK_DISMISS(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//2���û�У��
	CPlayer *pIServerUserItem = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if (pIServerUserItem == NULL)
		return false;

	//3������У��
	CTableFrame *pTableFrame = CTableManager::FindTableByTableID(pIServerUserItem->GetTableID());
	if (pTableFrame == NULL)
		return false;

	STR_SUB_RG_FRAME_ASK_DISMISS *pApply = (STR_SUB_RG_FRAME_ASK_DISMISS*)pData;

	CLog::Log(log_debug, "DISMISS UserID: %d", pIServerUserItem->GetUserID());

	//2. У�鷿���Ƿ��Ѿ����ڽ�ɢ״̬
	if (pTableFrame->m_bUnderDissState)
	{
		return true;
	}

	//���µ����ɢ��ť����Ҫɾ����ʱ������ֹһ���˲�ͣ�����ɢ����ʱ�����Զ��˳�����
	pTableFrame->KillVoteDismissRoom();

	pTableFrame->OnEventApplyDismissRoom(pIServerUserItem->GetChairID(), true); //0Ϊfalse  ����Ϊtrue

	return true;
}

//�����ɢ����
bool CHandleFromGate::On_SUB_RG_USER_VOTE_DISMISS(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//1�����ݰ�У��
	if (sizeof(STR_SUB_RG_FRAME_VOTE_DISMISS) != wDataSize)
		return false;

	//2���û�У��
	CPlayer *pIServerUserItem = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if (pIServerUserItem == NULL)
		return false;

	//3������У��
	CTableFrame *pTableFrame = CTableManager::FindTableByTableID(pIServerUserItem->GetTableID());
	if (pTableFrame == NULL)
		return false;

	//���ݰ�У��
	if (wDataSize != sizeof(STR_SUB_RG_FRAME_VOTE_DISMISS))
		return false;

	//��������
	STR_SUB_RG_FRAME_VOTE_DISMISS *pVote = (STR_SUB_RG_FRAME_VOTE_DISMISS*)pData;

	//��ɢ�ɹ�
	pTableFrame->OnEventVoteDismissRoom(pIServerUserItem->GetChairID(), (pVote->cbAgree != 0));
	return true;
}

/***************************************������Ϣ 2��*******************************************************/
//���봴������
bool CHandleFromGate::On_SUB_CG_USER_CREATE_ROOM(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��ȡ�û�
	CPlayer *player = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if (player==NULL)  return false;

	//У�����ݰ�
	if(wDataSize != sizeof(STR_SUB_CG_USER_CREATE_ROOM)) return false;

	//�������� У��
	STR_SUB_CG_USER_CREATE_ROOM *pCreateRoom = (STR_SUB_CG_USER_CREATE_ROOM *)pData;
	if( !CheckCreateRoom(player, pCreateRoom->byGameMode))
	{
		SendRequestFailure(player, TEXT("�������� ʧ��"), REQUEST_FAILURE_NORMAL);
		return true;
	}

	//���ͷ������ѡ�� ��client
	g_GameCtrl->SendData(player, MDM_USER, CMD_GC_USER_GET_ROOM_RULE, &RoomRuleManager::GetRoomRuleSetting(), sizeof(rule_arry));

	return true;
}

//���÷������
bool CHandleFromGate::On_SUB_CG_USER_SET_ROOM_RULE(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��ȡ�û�
	CPlayer *player = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if (player==NULL)  return false;

	//У�����ݰ�
	if(wDataSize != sizeof(STR_SUB_CG_USER_SET_ROOM_RULE)) return false;
	STR_SUB_CG_USER_SET_ROOM_RULE *pCmd = (STR_SUB_CG_USER_SET_ROOM_RULE *)pData;

	//��ȡ����������� TODONOW
	tagTableRule cfg = RoomRuleManager::GetRoomRule(pCmd->byChoose);
	cfg.GameMode = pCmd->byGameMode;
	
	//��ƱУ��
	if(!CheckCreateTableTicket(&cfg, player))
	{
		CLog::Log(log_warn, "%ld create room failed;  check ticker failed ", player->GetUserID());
		return true;
	}

	//��������
	CTableFrame *pTableFrame = CTableManager::CreateTable();
	if(NULL == pTableFrame)
	{
		SendRequestFailure(player,TEXT("��������������,���Ժ����ԣ�"),REQUEST_FAILURE_NORMAL);
		return true ; 
	}

	//������������
	pTableFrame->SetCommonRule(&cfg);
	pTableFrame->SetTableOwner(player->GetUserID());
	
	//�û�����
	if(pTableFrame->PlayerSitTable(player) != 0)
	{
		SendRequestFailure(player, TEXT("�û�����ʧ��!"), REQUEST_FAILURE_NORMAL);
		return false;
	}

	//���ʹ�������ɹ���Ϣ
	g_GameCtrl->SendData(player, MDM_USER, CMD_GC_USER_ENTER_SUBGAME_ROOM, NULL, 0);

	return true;
}

//��������Ȧ����
bool CHandleFromGate::CreateRoomClub(tagTableRule * pCfg, CPlayer *pIServerUserItem, STR_SUB_CG_USER_CREATE_ROOM* pCreateRoom)
{
	/* ������  Ѱ�ҿ������� */
	CTableFrame *pCurrTableFrame = GetNextEmptyTable();
	if(NULL == pCurrTableFrame)
	{
		SendRequestFailure(pIServerUserItem,TEXT("��������������,���Ժ�����"),REQUEST_FAILURE_NORMAL);
		return false;
	}
	
	/* ���岽 ���÷������ */
	//����ͨ�÷������  
	pCurrTableFrame->SetCommonRule(pCfg);

	//��������Ϸ�������
	tagTableSubGameRule *pSubGameCfg = new tagTableSubGameRule(); //(tagTableSubGameRule*)pCreateRoom->SubGameRule;
	if(pSubGameCfg != NULL)
	{
		pCurrTableFrame->SetSubGameRule(pSubGameCfg);
	}

	// new  ��ȡ������ʼ������Ϸ
	if (!pCurrTableFrame->InitTableFrameSink())
	{
		SendRequestFailure(pIServerUserItem, TEXT("��Ϸ��ʼ��ʧ�ܣ�"), REQUEST_FAILURE_NORMAL);
		return true; //TODONOW ���Ϊfalse �ͻ��˾Ͷ��������ˣ� ֮���޸ĵ�
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

	//���ʹ�������ɹ���Ϣ
	g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_ENTER_SUBGAME_ROOM, NULL, 0);	

	//TODONOWW ��Ҫ���͸�Э����, Ȼ��Э���� ���͸���¼��.  ��¼��֪ͨ�ͻ���ʵʱˢ�¾��ֲ�����
	STR_CMD_LC_CLUB_ROOM_RE RECMD;
	RECMD.dwClubID = pCfg->dwClubID;
	g_GameCtrl->SendData(pIServerUserItem, MDM_CLUB, CMD_LC_CLUB_ROOM_RE, &RECMD, sizeof(STR_CMD_LC_CLUB_ROOM_RE));	
	
	return true; 
}

//��������Ȧ����
bool CHandleFromGate::CreateTableClub(STR_DBO_GC_CLUB_CREATE_TABLE * pDbo, CPlayer *pIServerUserItem)
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
	tagTableRule *pCfg = new tagTableRule(); //(tagTableRule*)CreateRoom.CommonRule;

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
	pCurrTableFrame->SetTableID(dwPassword);

	//���÷����Զ���ɢ��Ĭ��һ���� -- ������ָ����ʼ��Ϸ �Զ�һ���Ӻ��ɢ
	pCurrTableFrame->SetTableAutoDismiss();

	/* ����������Ϣд�����ݿ� && ���͸��ͻ���(����Ϣ�ŵĺ��������з���) */
	DWORD dwClubRoomID = pDbo->dwClubRoomID;
	DWORD dwUserID = pIServerUserItem->GetUserID();
	DWORD dwTableID = pCurrTableFrame->GetTableID();
    DWORD dwLockState = (pDbo->byTableMode == 2) ? 0 : 1;
	ClubTableCreate(dwClubRoomID, dwUserID, dwTableID, dwLockState);

	//����ͨ�÷������  
	pCurrTableFrame->SetCommonRule(pCfg);

	//��������Ϸ�������
	tagTableSubGameRule *pSubGameCfg = new tagTableSubGameRule(); //(tagTableSubGameRule*)(CreateRoom.SubGameRule);
	if(pSubGameCfg != NULL)
	{
		pCurrTableFrame->SetSubGameRule(pSubGameCfg);
	}

	//�û�����		TODO �����ÿͻ�����������
	if(pCurrTableFrame->PlayerSitTable(pIServerUserItem) != 0 )
	{
		SendRequestFailure(pIServerUserItem, TEXT("�û�����ʧ��!"), REQUEST_FAILURE_NORMAL);
		return false;
	}
	
	//���ʹ�������ɹ���Ϣ
	g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_CLUB_CREATE_TABKE, NULL, 0);	

	return true;
}

//�������� ��Ҵ�������
bool CHandleFromGate::CreateTableHallGold(STR_DBO_CG_USER_JOIN_TABLE_HALL_GOLD * pDbo, CPlayer *pIServerUserItem)
{
	//�ڲ�ʹ��, ��У��ָ��
	tagTableRule *pCfg =  new tagTableRule(); //(tagTableRule*)pDbo->strCreateRoom.CommonRule;

	//���ó�����ͽ��
	pCfg->dwLevelGold = pDbo->dwMinGold;

	//��������Ʊ
	if(!CheckJoinTableTicket(pCfg, pIServerUserItem))
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
	CLog::Log(log_debug, "dwPassword: %d", dwPassword);

	//������������
	//pCurTableFrame->SetTableOwner(pIServerUserItem->GetUserID());
	pCurrTableFrame->SetTableID(dwPassword);

	//���ý�ҳ�����
	pCurrTableFrame->SetGoldType(pDbo->byGameType);

	//���÷����Զ���ɢ��Ĭ��һ���� -- ������ָ����ʼ��Ϸ �Զ�һ���Ӻ��ɢ
	pCurrTableFrame->SetTableAutoDismiss();

	/* ����������Ϣд�����ݿ� && ���͸��ͻ���(����Ϣ�ŵĺ��������з���) */
	DWORD dwUserID = pIServerUserItem->GetUserID();
	DWORD dwKindID = pDbo->dwKindID;
	BYTE  byGameType = pDbo->byGameType;
	DWORD dwTableID = pCurrTableFrame->GetTableID();
	HallTableCreate(dwUserID, dwKindID, byGameType, dwTableID);

	/* ���÷������ */
	//����ͨ�÷������  
	pCurrTableFrame->SetCommonRule(pCfg);

	//��������Ϸ�������
	tagTableSubGameRule *pSubGameCfg = new tagTableSubGameRule(); //(tagTableSubGameRule*)(pDbo->strCreateRoom.SubGameRule);
	if(pSubGameCfg != NULL)
	{
		pCurrTableFrame->SetSubGameRule(pSubGameCfg);
	}

	//  new  ��ȡ������ʼ������Ϸ
	if (!pCurrTableFrame->InitTableFrameSink())
	{
		SendRequestFailure(pIServerUserItem, TEXT("��Ϸ��ʼ��ʧ�ܣ�"), REQUEST_FAILURE_NORMAL);
		return true; //TODONOW ���Ϊfalse �ͻ��˾Ͷ��������ˣ� ֮���޸ĵ�
	}

	//�û�����		TODO �����ÿͻ�����������
	if(pCurrTableFrame->PlayerSitTable(pIServerUserItem) != 0)
	{
		SendRequestFailure(pIServerUserItem, TEXT("�û�����ʧ��!"), REQUEST_FAILURE_NORMAL);
		return true; //TODONOW ���Ϊfalse �ͻ��˾Ͷ��������ˣ� ֮���޸ĵ�
	}		

	
	//���ͼ����ҷ���ɹ�
	g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_JOIN_ROOM_SUCCESS, NULL, 0);

	return true;
}


//�������� ���ֲ�����
bool CHandleFromGate::CreateTableAutoClub(STR_DBO_CG_USER_JOIN_TABLE_NO_PASS * pDbo, CPlayer *pIServerUserItem)
{
	//�ڲ�ʹ��, ��У��ָ��
	tagTableRule *pCfg = new tagTableRule(); //(tagTableRule*)pDbo->strCreateRoom.CommonRule;

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
	pCurrTableFrame->SetTableID(dwPassword);

	//���÷����Զ���ɢ��Ĭ��һ���� -- ������ָ����ʼ��Ϸ �Զ�һ���Ӻ��ɢ
	pCurrTableFrame->SetTableAutoDismiss();

	/* ����������Ϣд�����ݿ� && ���͸��ͻ���(����Ϣ�ŵĺ��������з���) */
	DWORD dwClubRoomID = pDbo->dwClubRoomID;
	DWORD dwUserID = pIServerUserItem->GetUserID();
	DWORD dwTableID = pCurrTableFrame->GetTableID();
    DWORD dwLockState =  1; //Ĭ��Ϊ��ͨ����(������˽������)
	ClubTableCreate(dwClubRoomID, dwUserID, dwTableID, dwLockState);


	/* ���÷������ */
	//����ͨ�÷������  
	pCurrTableFrame->SetCommonRule(pCfg);

	//��������Ϸ�������
	tagTableSubGameRule *pSubGameCfg = new tagTableSubGameRule(); //(tagTableSubGameRule*)(pDbo->strCreateRoom.SubGameRule);
	if(pSubGameCfg != NULL)
	{
		pCurrTableFrame->SetSubGameRule(pSubGameCfg);
	}

	//  new  ��ȡ������ʼ������Ϸ
	if (!pCurrTableFrame->InitTableFrameSink())
	{
		SendRequestFailure(pIServerUserItem, TEXT("��Ϸ��ʼ��ʧ�ܣ�"), REQUEST_FAILURE_NORMAL);
		return true; //TODONOW ���Ϊfalse �ͻ��˾Ͷ��������ˣ� ֮���޸ĵ�
	}

	/* �û����� */
		if(pCurrTableFrame->PlayerSitTable(pIServerUserItem) != 0)
		{
			SendRequestFailure(pIServerUserItem, TEXT("�û�����ʧ��!"), REQUEST_FAILURE_NORMAL);
			return true; //TODONOW ���Ϊfalse �ͻ��˾Ͷ��������ˣ� ֮���޸ĵ�
		}		
	
	//���ͼ��뷿������ɹ�
	g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_JOIN_ROOM_SUCCESS, NULL, 0);

	return true;
}


//Club����Ȧ������Ϣ д�����ݿ�
bool CHandleFromGate::WriteClubRoomToDB(STR_DBR_CLUB_ROOM_INFO* pTableInfo)
{
	//��Ч����
	if (pTableInfo==NULL) return false;

	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_CLUB_ROOM_INFO, 0, pTableInfo, sizeof(STR_DBR_CLUB_ROOM_INFO));

	return true;
}

//�������� -- ��Ҫ����
bool CHandleFromGate::On_SUB_User_JoinFkRoom(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У���û�
	CPlayer *pIServerUserItem = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	CLog::Log(log_debug, "On_SUB_User_JoinFkRoom  begin 000 ");
	if (NULL == pIServerUserItem) return false;

	//У�����ݰ�
	if(wDataSize != sizeof(STR_SUB_CG_USER_JOIN_FK_ROOM))
	{
		CLog::Log(log_debug, "On_SUB_User_JoinFkRoom  begin 1111 ");
		SendRequestFailure(pIServerUserItem,TEXT("��������Ȧ�������ݴ���"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	CLog::Log(log_debug, "On_SUB_User_JoinFkRoom  begin 22222 ");

	//���ݶ���
	STR_SUB_CG_USER_JOIN_FK_ROOM *pJoin = (STR_SUB_CG_USER_JOIN_FK_ROOM *)pData;

	//�����ݿ��ѯ
	STR_DBR_JOIN_ROOM DBR;
	DBR.dwTableID = pJoin->dwPassword;
	DBR.dwUserID = pIServerUserItem->GetUserID();

	//Ͷ������
	return g_GameCtrl->PostDataBaseRequest(DBR_CG_JOIN_TABLE, dwSocketID, &DBR,sizeof(DBR));
}

//�������� ���� -- ��Ҫ����
bool CHandleFromGate::On_CMD_GC_JOIN_TABLE( DWORD dwSocketID, VOID * pData, WORD wDataSize)
{
	CLog::Log(log_debug, "On_CMD_GC_JOIN_TABLE  begin");
	/* 1. У���û� */
	CPlayer *pIServerUserItem = CPlayerManager::FindPlayerBySocketID(dwSocketID);
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
		if(wOldTableID > CTableManager::TableCount())
		{
			wOldTableID = INVALID_TABLE;
		}
		else
		{
			CTableFrame* pOldTable = CTableManager::FindTableByTableID(wOldTableID);
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
			if(pCurrTableFrame->PlayerSitTable(pIServerUserItem) != 0)
		{
			SendRequestFailure(pIServerUserItem,TEXT("���뷿��ʧ��, ����ʧ��"),REQUEST_FAILURE_PASSWORD);
			return false;
		}	
	
	//���ͼ��뷿������ɹ�
	g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_JOIN_ROOM_SUCCESS, NULL, 0);

	return true;
}

//�������� -- ����Ҫ����, �����ٿ�ʼ
bool CHandleFromGate::On_SUB_CG_USER_JOIN_TABLE_NO_PASS(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У���û�
	WORD wBindIndex = LOWORD(dwSocketID);
	CPlayer *pIServerUserItem = GetBindUserItem(wBindIndex);
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
	return g_GameCtrl->PostDataBaseRequest(DBR_CG_USER_JOIN_TABLE_NO_PASS, dwSocketID, pData,wDataSize);
}

//�������� -- ����Ҫ����, �����ٿ�ʼ
bool CHandleFromGate::On_CMD_GC_USER_JOIN_TABLE_NO_PASS( DWORD dwSocketID, VOID * pData, WORD wDataSize)
{
	/* 1. У���û� */
	WORD wBindIndex = LOWORD(dwSocketID);
	CPlayer *pIServerUserItem = GetBindUserItem(wBindIndex);
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
		if(wOldTableID > CTableManager::TableCount())
		{
			wOldTableID = INVALID_TABLE;
		}
		else
		{
			CTableFrame* pOldTable = CTableManager::FindTableByTableID(wOldTableID);
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
			if(pCurrTableFrame->PlayerSitTable(pIServerUserItem) != 0)
		{
			SendRequestFailure(pIServerUserItem,TEXT("����ʧ��, ����ʧ��!"),REQUEST_FAILURE_PASSWORD);
			return false;
		}


	//���ͼ��뷿������ɹ�
	g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_JOIN_ROOM_SUCCESS, NULL, 0);

	return true;
}

//�������� -- ��Ҵ�������
bool CHandleFromGate::On_SUB_CG_USER_JOIN_GOLD_HALL_ROOM(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У���û�
	CPlayer *pIServerUserItem = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if (NULL == pIServerUserItem) return true;

	//У�����ݰ�
	if(wDataSize != sizeof(STR_SUB_CG_USER_JOIN_GOLD_HALL_ROOM))
	{
		SendRequestFailure(pIServerUserItem,TEXT("���뷿�� �ṹ�����ݴ���"),REQUEST_FAILURE_NORMAL);
		return true;
	}

	//���ݶ���
	STR_SUB_CG_USER_JOIN_GOLD_HALL_ROOM *pJoin = (STR_SUB_CG_USER_JOIN_GOLD_HALL_ROOM *)pData;

	//Ͷ������ ���ݿ�ֻ�����ѯ���˳��εķ������
	g_GameCtrl->PostDataBaseRequest(DBR_CG_USER_JOIN_TABLE_HALL_GOLD, dwSocketID, pData,wDataSize);

	return true;
}

//�������� -- ��Ҵ������� ����
bool CHandleFromGate::On_CMD_GC_USER_JOIN_TABLE_HALL_GOLD( DWORD dwSocketID, VOID * pData, WORD wDataSize)
{
	/* 1. У���û� */
	CPlayer *pIServerUserItem = CPlayerManager::FindPlayerBySocketID(dwSocketID);
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

	/* 5. У���Ƿ���֮ǰ����Ϸ�� */
	WORD wOldTableID = pIServerUserItem->GetTableID(); //�����Ӻ�	
	if (wOldTableID != INVALID_TABLE)
	{
		if (wOldTableID > CTableManager::TableCount())
		{
			wOldTableID = INVALID_TABLE;
		}
		else
		{
			CTableFrame* pOldTable = CTableManager::FindTableByTableID(wOldTableID);
			if (pOldTable == NULL || pOldTable->GetGameStatus() != GAME_STATUS_PLAY)
			{
				wOldTableID = INVALID_TABLE;
			}
		}
	}
	if (INVALID_TABLE != wOldTableID)
	{
		SendRequestFailure(pIServerUserItem, TEXT("���Ѿ�����Ϸ��,���ܽ�����������"), REQUEST_FAILURE_NORMAL);
		return false;
	}
	
	/* 6. ���ҽ�ҳ��Ƿ��п����� */
	WORD wChairID;
	CTableFrame *pCurrTableFrame = GetGlodRoomEmptyChair(wChairID, pJoin->byGameType);
	if (pCurrTableFrame == NULL)
	{
		//���û���ҵ���Ҵ���������, ��ֱ�Ӵ���һ����Ҵ���������
		return CreateTableHallGold(pJoin, pIServerUserItem);
	}

	/* 4. ����У�� */
	if(wChairID == INVALID_CHAIR)
	{
		SendRequestFailure(pIServerUserItem, TEXT("���ӺŴ���,�����³���"), REQUEST_FAILURE_PASSWORD);
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
			if(pCurrTableFrame->PlayerSitTable(pIServerUserItem) != 0)
		{
			SendRequestFailure(pIServerUserItem,TEXT("����ʧ��, ����ʧ��!"),REQUEST_FAILURE_PASSWORD);
			return false;
		}

	//���ͼ��뷿��ɹ�
	g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_JOIN_ROOM_SUCCESS, NULL, 0);
	return true;
}

//��������Ȧ���䷵��
bool CHandleFromGate::On_CMD_GC_User_JoinGroupRoom(DWORD dwSocketID, VOID * pData, WORD wDataSize)
{
	//�ж�����

	//����У��
	ASSERT(wDataSize == sizeof(STR_DBO_CG_USER_JOIN_GROUP_ROOM));
	if(wDataSize != sizeof(STR_DBO_CG_USER_JOIN_GROUP_ROOM))
		return false;

	//��ȡ�û�
	WORD wBindIndex = LOWORD(dwSocketID);
	CPlayer *pIServerUserItem = GetBindUserItem(wBindIndex);
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
	for(int i= 0;i < CTableManager::TableCount();i++)
	{
		CTableFrame *pTableFrame = CTableManager::FindTableByIndex(i);
		//У������
		if( (NULL != pTableFrame) && 
			(pTableFrame->GetGroupID() == pDBOGroup->dwGroupID) && 
			(GAME_STATUS_FREE == pTableFrame->GetGameStatus()) && 
			(0 != pTableFrame->GetTableID()) )
		{
							//�û�����
				if(pTableFrame->PlayerSitTable( pIServerUserItem) != 0)
				{
					SendRequestFailure(pIServerUserItem, TEXT("����Ų���!"), REQUEST_FAILURE_PASSWORD);
					return true;
				}	

								//�û����Լ�������Ȧ����
				bHaveRoom = true;

				//���ͼ���ɹ���Ϣ
				g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_JOIN_ROOM_SUCCESS, NULL, 0);
				break;
		}
	}

	//�û��޷����룬���ʹ�����ʾ
	if ( !bHaveRoom )
	{
		SendRequestFailure(pIServerUserItem, TEXT("����Ȧ���������򲻴����ѿ����䣬����ϵ����"), REQUEST_FAILURE_PASSWORD);
	}

	return true;
}

//�޸��û��Ƹ���Ϣ
bool CHandleFromGate::On_CMD_GC_User_ModifyUserTreasure(DWORD dwSocketID, VOID * pData, WORD wDataSize)
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
	CPlayer *pIServerUserItem = CPlayerManager::FindPlayerByID(pDBOModify->dwUserID);

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
	g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_MODIFY_TREASURE, &CMDModify, sizeof(STR_CMD_GC_USER_MODIFY_TREASURE));

	//�޸ĳɹ����޸��ڴ��е��û�����
	pIServerUserItem->SetUserRoomCard(CMDModify.lOpenRoomCard);
	pIServerUserItem->SetUserGold(CMDModify.lUserGold);
	pIServerUserItem->SetUserDiamond(CMDModify.lUserDiamond);

	return true;
}

//�û��뿪
VOID CHandleFromGate::OnEventUserLogout(CPlayer * pIServerUserItem, DWORD dwLeaveReason)
{
	//��������
	DBR_GR_LeaveGameServer LeaveGameServer;
	ZeroMemory(&LeaveGameServer,sizeof(LeaveGameServer));

	//LeaveGameServer.lControlScore = pIServerUserItem->GetUserControlScore();

	//�û���Ϣ
	LeaveGameServer.dwLeaveReason=dwLeaveReason;
	LeaveGameServer.dwUserID=pIServerUserItem->GetUserID();
	LeaveGameServer.dwOnLineTimeCount=(DWORD)(time(NULL))-pIServerUserItem->GetLogonTime();

	//������Ϣ
	LeaveGameServer.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(LeaveGameServer.szMachineID,pIServerUserItem->GetMachineID(),CountArray(LeaveGameServer.szMachineID));

	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_GR_LEAVE_GAME_SERVER,0L,&LeaveGameServer,sizeof(LeaveGameServer));

	return;
}

//����ʧ��
bool CHandleFromGate::SendRequestFailure(CPlayer * pIServerUserItem, LPCTSTR pszDescribe, LONG lErrorCode)
{
	//��������
	CMD_GR_RequestFailure RequestFailure;
	ZeroMemory(&RequestFailure,sizeof(RequestFailure));

	//���ñ���
	RequestFailure.lErrorCode=lErrorCode;
	lstrcpyn(RequestFailure.szDescribeString,pszDescribe,CountArray(RequestFailure.szDescribeString));

	g_GameCtrl->SendData(pIServerUserItem, MDM_USER, SUB_GR_REQUEST_FAILURE, &RequestFailure, sizeof(CMD_GR_RequestFailure));

	std::wcout << "HFG: " << pszDescribe << std::endl;

	return true;
}

//���û�
CPlayer * CHandleFromGate::GetBindUserItem(WORD wBindIndex)
{
	CPlayer *pPlayer = NULL;

	//��ȡ�û�
	pPlayer = CPlayerManager::FindPlayerByEnum(wBindIndex);
	if (pPlayer == NULL)
	{
		return NULL;
	}

	return pPlayer;
}

//Ⱥ������
bool CHandleFromGate::SendDataBatchToMobileUser(WORD wCmdTable, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//ö���û�
	WORD wEnumIndex=0;
	while(wEnumIndex<CPlayerManager::GetPlayerCount())
	{
		//�����û�
		CPlayer *pIServerUserItem=CPlayerManager::FindPlayerByEnum(wEnumIndex++);
		if(pIServerUserItem==NULL) continue;


		//�������
		WORD wTagerTableID = pIServerUserItem->GetTableID();

		//״̬����
		if(pIServerUserItem->GetUserStatus() >= US_SIT)
		{
			if(wTagerTableID != wCmdTable)continue;
		}


		//������Ϣ
		g_GameCtrl->SendData(pIServerUserItem,wMainCmdID,wSubCmdID,pData,wDataSize);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
//��������˿���
bool CHandleFromGate::AddOtherRoomInfo(DBR_GR_UpdateTableInfo* pTableInfo)
{
	//��Ч����
	if (pTableInfo==NULL) return false;

	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_GR_ADD_TABLE_INFO, 0, pTableInfo, sizeof(DBR_GR_UpdateTableInfo));

	return true;
}

//��ʼ�����˿���
void CHandleFromGate::StartTable(DWORD dwTableID)
{
	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_GR_START_TABLE,0,&dwTableID,sizeof(dwTableID));
}

//���������˿���
void CHandleFromGate::ConcludeTable(DWORD dwTableID)
{
	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_GR_END_TABLE,0,&dwTableID,sizeof(dwTableID));
}

#pragma region Club����Ȧ2 �¼�֪ͨ(1.������Ϣ 2.�����Ϣ)
//club ��������
void CHandleFromGate::ClubTableCreate(DWORD dwClubRoomID, DWORD dwUserID, DWORD dwTableID, DWORD dwLockState)
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
	Dbr.dwGameID = (g_GameCtrl->GetServerID()) & 0xFFFFFF00;

	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_CLUB_TABLE_INFO, 0, &Dbr, sizeof(Dbr));
}

//club ���ӿ�ʼ��Ϸ
void CHandleFromGate::ClubTableStart(DWORD dwTableID, BYTE byClubOrHallGold)
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

		g_GameCtrl->PostDataBaseRequest(DBR_CLUB_TABLE_INFO,0,&Dbr,sizeof(Dbr));
	}

	//��Ҵ���������Ϣ
	if(2 == byClubOrHallGold)
	{
		STR_DBR_HALL_GOLD_TABLE_INFO HallDbr;
		ZeroMemory(&HallDbr, sizeof(HallDbr));
		HallDbr.dwTableID = dwTableID;
		HallDbr.dwTableState = 1;
		HallDbr.byMask = 2;

		g_GameCtrl->PostDataBaseRequest(DBR_HALL_GOLD_TABLE_INFO,0,&HallDbr,sizeof(HallDbr));
	}
}

//club �����Ѿ���Ա
void CHandleFromGate::ClubTableMax(DWORD dwTableID, BYTE byClubOrHallGold)
{
	//��ע: ���ӿ�ʼ��Ϸ��, ���µ�ǰ����Ϊ (0 + 1)
	if(1 == byClubOrHallGold)
	{
		STR_DBR_CLUB_TABLE_INFO Dbr;
		ZeroMemory(&Dbr, sizeof(Dbr));
		Dbr.dwTableID = dwTableID;
		Dbr.dwTableState = 2;
		Dbr.byMask = 2;

		g_GameCtrl->PostDataBaseRequest(DBR_CLUB_TABLE_INFO,0,&Dbr,sizeof(Dbr));
	}

	//��Ҵ���������Ϣ
	if(2 == byClubOrHallGold)
	{
		STR_DBR_HALL_GOLD_TABLE_INFO HallDbr;
		ZeroMemory(&HallDbr, sizeof(HallDbr));
		HallDbr.dwTableID = dwTableID;
		HallDbr.dwTableState = 2;
		HallDbr.byMask = 2;

		g_GameCtrl->PostDataBaseRequest(DBR_HALL_GOLD_TABLE_INFO,0,&HallDbr,sizeof(HallDbr));
	}
}

//club ����û����Ա
void CHandleFromGate::ClubTableNotMax(DWORD dwTableID, BYTE byClubOrHallGold)
{
	//��ע: ���ӿ�ʼ��Ϸ��, ���µ�ǰ����Ϊ (0 + 1)
	if(1 ==  byClubOrHallGold)
	{
		STR_DBR_CLUB_TABLE_INFO Dbr;
		ZeroMemory(&Dbr, sizeof(Dbr));
		Dbr.dwTableID = dwTableID;
		Dbr.dwTableState = 0;
		Dbr.byMask = 2;

		g_GameCtrl->PostDataBaseRequest(DBR_CLUB_TABLE_INFO,0,&Dbr,sizeof(Dbr));
	}

	//��Ҵ���������Ϣ
	if( 2 ==  byClubOrHallGold)
	{
		STR_DBR_HALL_GOLD_TABLE_INFO HallDbr;
		ZeroMemory(&HallDbr, sizeof(HallDbr));
		HallDbr.dwTableID = dwTableID;
		HallDbr.dwTableState = 0;
		HallDbr.byMask = 2;

		g_GameCtrl->PostDataBaseRequest(DBR_HALL_GOLD_TABLE_INFO,0,&HallDbr,sizeof(HallDbr));
	}
}

//club ����С�ֽ���
void CHandleFromGate::ClubTableXJ(DWORD dwTableID)
{
	//��ע: ����С�ֽ�����, ���µ�ǰ����Ϊ (���ݿ� + 1)
	STR_DBR_CLUB_TABLE_INFO Dbr;
	ZeroMemory(&Dbr, sizeof(Dbr));
	Dbr.dwTableID = dwTableID;
	Dbr.dwTableState = 1;
	Dbr.byCurrentRound = 1;//����һ������ TODONOW �����޸�
	Dbr.byMask = 2;
	
	g_GameCtrl->PostDataBaseRequest(DBR_CLUB_TABLE_INFO,0,&Dbr,sizeof(Dbr));
}

//club ���Ӵ�ֽ���
void CHandleFromGate::ClubTableDJ(DWORD dwTableID)
{
    //��ע: ���Ӵ�ֽ���, ��ɢ����
    STR_DBR_CLUB_TABLE_INFO Dbr;
	ZeroMemory(&Dbr, sizeof(Dbr));
	Dbr.dwTableID = dwTableID;
	Dbr.byMask = 3;
	
	g_GameCtrl->PostDataBaseRequest(DBR_CLUB_TABLE_INFO,0,&Dbr,sizeof(Dbr));
}

#pragma endregion

//�û����������˿���
void CHandleFromGate::JoinTable(DWORD dwTableID, DWORD dwUserID)
{
	
	DBR_GP_AddTableUser AddTableUser;
	AddTableUser.dwTableID = dwTableID;
	AddTableUser.dwUserID = dwUserID;
	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_GR_ADD_TABLEUSER,0,&AddTableUser,sizeof(DBR_GP_AddTableUser));
}

//�û��뿪�����˿���
void CHandleFromGate::LeaveTable(DWORD dwTableID, DWORD dwUserID)
{
	DBR_GP_DeleteTableUser DeleteTableUser;
	DeleteTableUser.dwTableID = dwTableID;
	DeleteTableUser.dwUserID = dwUserID;

	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_GR_DELETE_TABLEUSER,0,&DeleteTableUser,sizeof(DBR_GP_DeleteTableUser));
}

//Club����Ȧ��������
bool CHandleFromGate::On_CMD_CG_CLUB_CREATE_TABLE( DWORD dwSocketID, VOID * pData, WORD wDataSize )
{
	//��ȡ�û�
	WORD wBindIndex = LOWORD(dwSocketID);
	CPlayer *pIServerUserItem = GetBindUserItem(wBindIndex);
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
void CHandleFromGate::ClubPlayerSitDown(DWORD dwTableID, DWORD dwUserID, BYTE byChairID, BYTE byClubOrHallGold)
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

		g_GameCtrl->PostDataBaseRequest(DBR_CLUB_PLAYER_INFO,0,&Dbr,sizeof(Dbr));
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

		g_GameCtrl->PostDataBaseRequest(DBR_HALL_GOLD_PLAYER_INFO,0,&HallDbr,sizeof(HallDbr));
	}
}

//�û�����
void CHandleFromGate::ClubPlayerSitUp(DWORD dwTableID, DWORD dwUserID, BYTE byChairID, BYTE byClubOrHallGold)
{
	//���ֲ�����
	if( 1 == byClubOrHallGold)
	{
		STR_DBR_CLUB_PLAYER_INFO Dbr;
		ZeroMemory(&Dbr, sizeof(Dbr));
		Dbr.dwTableID = dwTableID;
		Dbr.dwUserID = dwUserID;
		Dbr.byMask = 2; 

		g_GameCtrl->PostDataBaseRequest(DBR_CLUB_PLAYER_INFO,0,&Dbr,sizeof(Dbr));
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

		g_GameCtrl->PostDataBaseRequest(DBR_HALL_GOLD_PLAYER_INFO,0,&HallDbr,sizeof(HallDbr));
	}
}

//���һ���û�����
void CHandleFromGate::ClubLastPlayerSitUp(DWORD dwTableID, DWORD dwUserID, BYTE byChairID, BYTE byClubOrHallGold)
{
	//���ֲ�����
	if( 1 == byClubOrHallGold)
	{
		STR_DBR_CLUB_TABLE_INFO Dbr;
		ZeroMemory(&Dbr, sizeof(Dbr));
		Dbr.dwTableID = dwTableID;
		Dbr.byMask = 3;

		g_GameCtrl->PostDataBaseRequest(DBR_CLUB_TABLE_INFO,0,&Dbr,sizeof(Dbr));
	}

	//��Ҵ�������
	if( 2 == byClubOrHallGold)
	{
		STR_DBR_HALL_GOLD_TABLE_INFO HallDbr;
		ZeroMemory(&HallDbr, sizeof(HallDbr));
		HallDbr.dwTableID = dwTableID;
		HallDbr.byMask = 3;

		g_GameCtrl->PostDataBaseRequest(DBR_HALL_GOLD_TABLE_INFO,0,&HallDbr,sizeof(HallDbr));
	}
}

#pragma region ��������
//********************************************��������*******************************************//

//��������ID�������뷿������
DWORD CHandleFromGate::GenerateTablePassword()
{	
	//��������
	srand(static_cast<unsigned int >(time(NULL)));
	BYTE byHigh1 = 1+  rand() % 219 + 24;
	BYTE byHigh2 = ((g_GameCtrl->GetServerID()) >> 16) &  0xFF;
	BYTE byHign3 = ((g_GameCtrl->GetServerID()) >> 8) & 0xF;
	DWORD dwPassword = (byHigh1 << 12) + (byHigh2 <<4) + byHign3;

	//�ж����������Ƿ����
	while(true)
	{
		bool bFind = false;
		for (WORD i=0; i<CTableManager::TableCount(); i++)
		{
			//��ȡ����
			CTableFrame *pTableFrame = CTableManager::FindTableByIndex(i);

			//����״̬�ж�
			if ( (NULL != pTableFrame) && 
				 (0 < pTableFrame->GetSitUserCount() ) &&
				 (dwPassword == pTableFrame->GetTableID()) )
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

			/*dwPassword = ((1 + rand() % 5) << 17) + ((rand() % 512) << 8) + (BYTE)(g_GameCtrl->GetServerID() & 0XFF);*/
		}
		else
			break;
	}

	return dwPassword;
}

//Ѱ����һ��������
CTableFrame* CHandleFromGate::GetNextEmptyTable()
{
	//��������
	CTableFrame *pTableFrame = NULL;

	//Ѱ����һ��������
	for (WORD i=0;i<CTableManager::TableCount();i++)
	{
		//��ȡ����
		pTableFrame = CTableManager::FindTableByIndex(i);

		//�����ж�
		if ( (NULL != pTableFrame) && 
			 (0 == pTableFrame->GetSitUserCount()) && 
			 (0 == pTableFrame->GetCreateTableUser()) ) 
		{
			CLog::Log(log_debug, "Ѱ�ҵ�������");
			return pTableFrame;
		}
	}

	//��������
	pTableFrame = CTableManager::CreateTable();		//�������������CreateTable
	if (pTableFrame == NULL)
	{
		//����ʧ����
		return NULL;
	}

	CLog::Log(log_debug, "����������");
	return pTableFrame;
}

//��������Ѱ��ָ������
CTableFrame* CHandleFromGate::GetDesignatedTable(const DWORD &dwPassword)
{
	//Ѱ��ָ������
	for (WORD i=0; i<CTableManager::TableCount(); i++)
	{
		//��ȡ����
		CTableFrame *pTableFrame = CTableManager::FindTableByIndex(i);

		//�����ж�
		if ( (NULL != pTableFrame) && 
			 (dwPassword == pTableFrame->GetTableID()) )
		{
			return pTableFrame;
		}
	}

	return NULL;
}

//���ҽ�ҷ�������
CTableFrame* CHandleFromGate::GetGlodRoomEmptyChair(WORD &wChairID, BYTE byType)
{
	//��������
	CTableFrame *pTableFrame = NULL;
	wChairID = INVALID_CHAIR;

	//Ѱ�ҽ�ҷ�������
	for (WORD i=0; i<CTableManager::TableCount(); i++)
	{
		//��ȡ����
		CTableFrame *pTableFrame = CTableManager::FindTableByIndex(i);

		//����У��
		if ( (NULL == pTableFrame) || 
			 (pTableFrame->GetGameStatus() != GAME_STATUS_FREE) || 
			 (pTableFrame->GetTableMode() != TABLE_MODE_GOLD) ||
			(pTableFrame->GetGoldType() != byType))
			continue;

		//��ȡ������
		wChairID = pTableFrame->GetNullChairID();
		if(wChairID == INVALID_CHAIR)
			continue;

		return pTableFrame;
	}

	return NULL;
}

//У���Ƿ���Դ�������
bool CHandleFromGate::CheckCreateRoom(CPlayer * player, BYTE gameMode)
{
	//�û�Ч��
	if (INVALID_CHAIR != player->GetChairID())   //������
	{
		SendRequestFailure(player, TEXT("������Ϸ��,�޷��������䣡"), REQUEST_FAILURE_NORMAL);
		return true;
	}
	if(INVALID_TABLE != player->GetTableID())
	{
		SendRequestFailure(player, TEXT("���ڷ�����,�޷��������䣡"), REQUEST_FAILURE_NORMAL);
		return true;
	}

	switch(gameMode)
	{
	case TABLE_MODE_FK:
		{
			return true;
		}
	case TABLE_MODE_MATCH:
		{
			return false;
		}
	case TABLE_MODE_GOLD:
		{
			return false;
		}
	case TABLE_MODE_FK_GOLD:
		{
			//У����Ʊ
			return true;
		}
	case TABLE_MODE_CLUB:
		{
			//У��
			return true;
		}
	}

	return false;
}

//������������Ʊ
bool CHandleFromGate::CheckJoinTableTicket(tagTableRule *pCfg, CPlayer *pIServerUserItem)
{
	//�ڲ�����, ��У��ָ��

	switch( pCfg->GameMode )
	{
	case TABLE_MODE_FK://����ģʽ
		{
			//�����ķ�������AA֧�������û���������
			/*
			if( (1 == pCfg->cbPayType) )
			{
				SendRequestFailure(pIServerUserItem, TEXT("�����ڽ���AA֧���ķ�����, �򷿿�����,�޷�����"), REQUEST_FAILURE_NORMAL);
				return false;
			}
			*/
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
			/*
			if( (1 == pCfg->cbPayType) && 
				(pIServerUserItem->GetUserRoomCard() < pCfg->GameCountType) )
			{
				SendRequestFailure(pIServerUserItem, TEXT("�����ڽ����ΪAA֧���ķ�����ҷ�, �򷿿�����,�޷�����"), REQUEST_FAILURE_NORMAL);
				return false;
			}
			*/

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
bool CHandleFromGate::CheckCreateTableTicket(tagTableRule *pCfg, CPlayer *pIServerUserItem)
{
	switch( pCfg->GameMode )
	{
	case TABLE_MODE_FK://����ģʽ
		{
			//У���û�����: ����֧�������û���������
			/*
			if(0 == pCfg->cbPayType && 
				(pIServerUserItem->GetUserRoomCard() < ((pCfg->GameCountType) * (pCfg->PlayerCount)) ))	  
			{
				SendRequestFailure(pIServerUserItem, TEXT("�����ڴ�������֧���ķ�����, �򷿿�����,�޷�����"), REQUEST_FAILURE_NORMAL);
				return false;
			}
			*/
			/*
			//У���û�������AA֧�������û���������
			if( (1 == pCfg->cbPayType) && 
				(pIServerUserItem->GetUserRoomCard() < pCfg->GameCountType) )
			{
				SendRequestFailure(pIServerUserItem, TEXT("�����ڴ���AA֧���ķ�����, �򷿿�����,�޷�����"), REQUEST_FAILURE_NORMAL);
				return false;
			}
			*/

			return true;
		}
	case TABLE_MODE_FK_GOLD://������ҳ�
		{
			//У���û�����: ����֧�������û���������
			/*
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
			*/
			//�����ķ�����ҳ�
			if( pIServerUserItem->GetUserGold()  < pCfg->dwLevelGold)
			{
				SendRequestFailure(pIServerUserItem, TEXT("�����ڴ���������ҷ�, ���Ҳ���,�޷�����"), REQUEST_FAILURE_NORMAL);
				return false;
			}
			return true;
		}
	default:
		{
			SendRequestFailure(pIServerUserItem, TEXT("�������� ģʽ����ȷ, �޷�����"), REQUEST_FAILURE_NORMAL);
			return false;
		}
	}
}

//�������� -- ��Ҵ���
void CHandleFromGate::HallTableCreate(DWORD dwUserID, DWORD dwKindID, BYTE byGameType, DWORD dwTableID)
{
    STR_DBR_HALL_GOLD_TABLE_INFO Dbr;
	ZeroMemory(&Dbr, sizeof(Dbr));
	Dbr.dwUserID = dwUserID;
	Dbr.dwKindID = dwKindID;	
	Dbr.byGameType = byGameType;
	Dbr.dwTableID =dwTableID;
	Dbr.dwTableState = 0;
	Dbr.byMask =1;

	Dbr.dwGameID = (g_GameCtrl->GetServerID()) & 0xFFFFFF00;

	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_HALL_GOLD_TABLE_INFO, 0, &Dbr, sizeof(Dbr));
}

#pragma endregion


//�����û��Ƹ���Ϣ
bool CHandleFromGate::OnEventModifyUserTreasure(CPlayer *pIServerUserItem, DWORD dwTableID, BYTE byTableMode, BYTE byRound, SCORE lUserTreasuse, BYTE byWin)
{
	//Ч�����
	if (pIServerUserItem==NULL) return false;

	//��������
	STR_DBR_GR_MODIFY_USER_TREASURE DBR;
	ZeroMemory(&DBR, sizeof(DBR));
	DBR.dwUserId  =  pIServerUserItem->GetUserID(); //pUserInfo->dwUserID;
	DBR.dwKind = ((g_GameCtrl->GetServerID()) >> 16) & 0xFF;
	DBR.dwTableID = dwTableID;
	DBR.byTableMode = byTableMode;
	DBR.byRound = byRound;
	DBR.lUserTreasure = lUserTreasuse;
	DBR.byWin = byWin;

	//��������
	g_GameCtrl->PostDataBaseRequest(DBR_GR_MODIFY_USER_TREASURE, pIServerUserItem->GetSocketID(), &DBR, sizeof(DBR));

	return true;
}
