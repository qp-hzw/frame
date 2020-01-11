#include "HandleFromGate.h"
#include "GameCtrl.h"
#include "TableManager.h"
#include <iostream>

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
	case DBO_CG_USER_CREATE_GROUP_ROOM:	//��������Ȧ���䷵��
		{
			return On_CMD_GC_User_CreateGroupRoom(dwSocketID, pData, wDataSize);
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

	case DBO_GR_LOTTERY_RESULT:
		{
			return OnDBLotteryResult(dwSocketID,pData,wDataSize);
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
	case SUB_CG_CLUB_CREATE_TABLE:	//��������	����Ȧ������
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
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�������
	DWORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	if ((wTableID==INVALID_TABLE)||(wChairID==INVALID_CHAIR)) return true;

	//��Ϣ���� 
	CTableFrame * pTableFrame=CTableManager::FindTableByTableID(wTableID);
	if (pTableFrame == NULL)
	{
		CLog::Log(log_error, "pTableFrame == NULL");
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

	//���û��б��ȡ�û�
	CPlayer *pIServerUserItem = CPlayerManager::FindPlayerByID(pLogonUserID->dwUserID);

	//��������¼���û��������߳�
	if (pIServerUserItem!=NULL)
	{
		//��������¼���������
		//1. ���û�����ǻ����û�, ���Ǵ���ص�¼
		//2. �û����ǻ����û�, �����ڸý����ϵ�¼
	}

	
	//�л��ж� TODONOW �ص�鿴
	if( NULL != pIServerUserItem )
	{
		//return SwitchUserItemConnect(pIServerUserItem, pLogonUserID->szMachineID,
			//pLogonUserID->dLongitude, pLogonUserID->dLatitude);

	}
	

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

	//�ж�����

	//�û���¼ʧ�ܣ�ֱ�ӷ���
	if ( DB_SUCCESS != pDBOLogon->lResultCode )
	{
		//���췵������
		STR_CMD_GC_LOGON_USERID logonFail;
		ZeroMemory(&logonFail, sizeof(STR_CMD_GC_LOGON_USERID));
		logonFail.lResultCode = 0;
		lstrcpyn(logonFail.szDescribeString, pDBOLogon->szDescribeString, CountArray(logonFail.szDescribeString));

		//��������
		g_GameCtrl->SendData(dwSocketID, MDM_GR_LOGON, CMD_GC_LOGON_USERID, &logonFail, sizeof(STR_CMD_GC_LOGON_USERID));
		return true;
	}

	//���û��б��л�ȡ �û�
	CPlayer *pIServerUserItem = CPlayerManager::FindPlayerByID(pDBOLogon->dwUserID);

	
	////�ظ���¼�ж� TODONOW
	//if (pIServerUserItem!=NULL)
	//{
	//	//�л��û�
	//	SwitchUserItemConnect(pIServerUserItem, pDBOLogon->szMachineID, pDBOLogon->dLongitude, pDBOLogon->dLatitude, 
	//							pDBOLogon->cbDeviceType, pDBOLogon->wBehaviorFlags, pDBOLogon->wPageTableCount);

	//	//��ʾ��Ϣ
	//	TCHAR szDescribe[128]=TEXT("");
	//	_sntprintf_s(szDescribe,CountArray(szDescribe),
	//		TEXT("��ID��¼����%ld�ظ���¼2��"),
	//		pIServerUserItem->GetUserID());

	//	return true;
	//}
	

	//�����û�
	ActiveUserItem(&pIServerUserItem, dwSocketID, pDBOLogon);

	return true;
}

//ID��¼�ɹ��������û�
void CHandleFromGate::ActiveUserItem(CPlayer **pIServerUserItem, DWORD dwSocketID, 
		STR_DBO_CG_LOGON_USERID *pDBOLogon)
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

	UserInfo.dwSocketId = dwSocketID;

	//������Ϣ
	//UserInfo.wBindIndex=wBindIndex;
	//UserInfo.dwClientAddr=pBindParameter->dwClientAddr;
	lstrcpyn(UserInfo.szMachineID,pDBOLogon->szMachineID,CountArray(UserInfo.szMachineID));

	//�����û� -- �����û���Ϣ
	CPlayerManager::InsertPlayer(pIServerUserItem, UserInfo);

	//�����ж� -- �����û�pIServerUserItem��Ϣʧ��
	if (pIServerUserItem == NULL)
	{
		//�Ͽ��û�
		if (bAndroidUser==true)
		{
			//m_AndroidUserManager.DeleteAndroidUserItem(dwSocketID);
		}
		else
		{
			g_TCPNetworkEngine->CloseSocket(dwSocketID);
		}
		return;
	}

	//��¼�¼�
	OnEventUserLogon(*pIServerUserItem, false);
}

//�û���¼ 
VOID CHandleFromGate::OnEventUserLogon(CPlayer * pIServerUserItem, bool bAlreadyOnLine)
{
	//��ȡ����
	WORD wBindIndex = pIServerUserItem->GetBindIndex();
	bool bAndroidUser = pIServerUserItem->IsAndroidUser();

	/* 1. ���췵������ */
	STR_CMD_GC_LOGON_USERID logon;
	ZeroMemory(&logon, sizeof(STR_CMD_GC_LOGON_USERID));
	logon.lResultCode = 0;
	logon.dwKindID = g_GameCtrl->GetKindID();
	lstrcpyn(logon.szDescribeString, TEXT("�û���¼�ɹ�"), CountArray(logon.szDescribeString));
	
	//TODONOW
	logon.dwOffLineGameID = pIServerUserItem -> GetOfflineGameID();

	//��������
	g_GameCtrl->SendData(pIServerUserItem->GetSocketID(), MDM_GR_LOGON, CMD_GC_LOGON_USERID, &logon, sizeof(STR_CMD_GC_LOGON_USERID));
	

	/* 11. ֪ͨ���ݿ�ı� ���״̬ */
	//TODONOWW
	DBR_GP_UserQuitInfo quitInfo;
	quitInfo.dwUserID = pIServerUserItem->GetUserID();
	quitInfo.byOnlineMask = 2;
	g_GameCtrl->PostDataBaseRequest(DBR_GP_GAME_USER_STATE,0, &quitInfo,sizeof(quitInfo));
}

//�л�����
bool CHandleFromGate::SwitchUserItemConnect(CPlayer * pIServerUserItem, TCHAR szMachineID[LEN_MACHINE_ID],
												const double &dLongitude, const double &dLatitude,
												BYTE cbDeviceType, WORD wBehaviorFlags, WORD wPageTableCount)
{
	//Ч�����
	if (NULL == pIServerUserItem) 
		return false;

	//�û���Ч���Ͽ��Ѱ󶨵��û�
	if (INVALID_WORD != pIServerUserItem->GetBindIndex())	
	{
		//�Ͽ��û�
		if (pIServerUserItem->IsAndroidUser()==true)
		{
			//m_AndroidUserManager.DeleteAndroidUserItem(pSourceParameter->dwSocketID);
		}
		else
		{
			g_TCPNetworkEngine->CloseSocket(pIServerUserItem->GetSocketID());
		}
	}

	//�����ж�
	LPCTSTR pszMachineID = pIServerUserItem->GetMachineID();
	bool bSameMachineID = (lstrcmp(pszMachineID, szMachineID)==0);

	//���¼����û�
	pIServerUserItem->SetUserParameter(0, szMachineID, false, false, dLongitude, dLatitude);

	//��¼�¼�
	OnEventUserLogon(pIServerUserItem,true);

	return true;
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
		pTableFrame->PlayerSitTable(wRequestChairID,pIServerUserItem,pUserSitDown->szPassword, NULL);

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

//�ܾ��������
bool CHandleFromGate::On_SUB_CG_User_RefuseSit(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_GR_UserRepulseSit));
	if (wDataSize!=sizeof(CMD_GR_UserRepulseSit)) return false;

	//��Ϣ����
	CMD_GR_UserRepulseSit * pUserRepulseSit=(CMD_GR_UserRepulseSit *)pData;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	CPlayer * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//��ȡ����
	CTableFrame * pTableFrame=CTableManager::FindTableByTableID(pUserRepulseSit->wTableID);

	//��ȡ�û�
	CPlayer * pRepulseIServerUserItem = pTableFrame->GetTableUserItem(pUserRepulseSit->wChairID);
	if (pRepulseIServerUserItem==NULL) return true;
	if(pRepulseIServerUserItem->GetUserID() != pUserRepulseSit->dwRepulseUserID)return true;

	//������Ϣ
	TCHAR szDescribe[256]=TEXT("");
	lstrcpyn(szDescribe,TEXT("��������������˲�����ͬ����Ϸ��"),CountArray(szDescribe));
	g_GameCtrl->SendRoomMessage(pRepulseIServerUserItem,szDescribe,0);

	//�������
	pTableFrame->PlayerUpTable(pRepulseIServerUserItem);

	return true;
}

//�߳��û�
bool CHandleFromGate::On_SUB_CG_User_KickUser(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_KickUser));
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
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->GetMemberOrder()>pITargetUserItem->GetMemberOrder()));
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

//���󷿼��б�
bool CHandleFromGate::OnTCPNetworkSubGetTableList(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_GR_GetTableList));
	if (wDataSize<sizeof(CMD_GR_GetTableList)) return false;

	//������Ϣ
	CMD_GR_GetTableList * pGetTableList = (CMD_GR_GetTableList *)pData;

	CMD_GR_TableListResult pTableList;
	ZeroMemory(&pTableList,sizeof(CMD_GR_TableListResult));


	int iCount = 20;
	for(int i=0;i < CTableManager::TableCount(),i < iCount;i++)
	{
		CTableFrame* pTableFrame = CTableManager::FindTableByIndex(i);
		if(NULL == pTableFrame) continue;

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
		g_GameCtrl->SendData(dwSocketID,MDM_USER,SUB_GR_GET_TABLELIST_RESULT,&pTableList,sizeof(CMD_GR_TableListResult));	
	}

	return true;
}

//�齱����
bool CHandleFromGate::OnTcpNetworkQueryLottery(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//����У��
	ASSERT(wDataSize==sizeof(CMD_GR_QueryLottery));
	if(wDataSize!=sizeof(CMD_GR_QueryLottery))
		return false;

	CMD_GR_QueryLottery* pGetRankReward = (CMD_GR_QueryLottery*)pData;

	DBR_GR_QueryLottery QueryLottery;
	memcpy(&QueryLottery,pGetRankReward,sizeof(DBR_GR_QueryLottery));

	return g_GameCtrl->PostDataBaseRequest(DBR_GR_QUERY_LOTTERY,dwSocketID,&QueryLottery,sizeof(DBR_GR_QueryLottery));

}

/***************************************������Ϣ 3��*******************************************************/
//��������
bool CHandleFromGate::On_SUB_CG_USER_CREATE_ROOM(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��ȡ�û�
	CPlayer *pIServerUserItem = CPlayerManager::FindPlayerBySocketID(dwSocketID);

	//�û�У��
	if (pIServerUserItem==NULL) 
	{
		SendRequestFailure(pIServerUserItem,TEXT("�����������ݴ�С��ƥ�䣡"),REQUEST_FAILURE_NORMAL);
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

	return bRet;
}

//��������
bool CHandleFromGate::On_SUB_CG_USER_CREATE_TABLE(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//��ȡ�û�
	WORD wBindIndex = LOWORD(dwSocketID);
	CPlayer *pIServerUserItem = GetBindUserItem(wBindIndex);

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
	return g_GameCtrl->PostDataBaseRequest(DBR_CG_CLUB_CREATE_TABLE, dwSocketID, pData, wDataSize);
}

//������ͨ����
bool CHandleFromGate::CreateTableNormal(tagTableRule * pCfg, CPlayer *pIServerUserItem, STR_SUB_CG_USER_CREATE_ROOM* pCreateRoom)
{
	//��������Ʊ
	//if(!CheckCreateTableTicket(pCfg, pIServerUserItem))
	{
		CLog::Log(log_debug, "��Ʊ����: %d", pCfg->GameMode);
		//return true; //TODONOW ���Ϊfalse �ͻ��˾Ͷ��������ˣ� ֮���޸ĵ�
	} 

	//�û�Ч��
	if (INVALID_CHAIR != pIServerUserItem->GetChairID())   //������
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

	//���÷���״̬
	pCurrTableFrame->SetGameStatus(GAME_STATUS_FREE);

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

	//���ʹ�������ɹ���Ϣ
	g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_CREATE_ROOM_SUCCESS, NULL, 0);

	return true;
}

//��������Ȧ����
bool CHandleFromGate::CreateRoomClub(tagTableRule * pCfg, CPlayer *pIServerUserItem, STR_SUB_CG_USER_CREATE_ROOM* pCreateRoom)
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
	pCurrTableFrame->SetTableID(dwPassword);

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

	//���ʹ�������ɹ���Ϣ
	g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_CREATE_ROOM_SUCCESS, NULL, 0);	

	//TODONOWW ��Ҫ���͸�Э����, Ȼ��Э���� ���͸���¼��.  ��¼��֪ͨ�ͻ���ʵʱˢ�¾��ֲ�����
	STR_CMD_LC_CLUB_ROOM_RE RECMD;
	RECMD.dwClubID = pCfg->dwClubID;
	g_GameCtrl->SendData(pIServerUserItem, MDM_CLUB, CMD_LC_CLUB_ROOM_RE, &RECMD, sizeof(STR_CMD_LC_CLUB_ROOM_RE));	
	
	//added by WangChengQing 2018/7/2   ����Ȧ�����������֮��, Ӧ�öϿ���client������
	//added by WangChengQing 2018/8/31  �˴��ɿͻ��������Ͽ�socket����
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
	pCurrTableFrame->SetTableID(dwPassword);

	//���÷����Զ���ɢ��Ĭ��һ���� -- ������ָ����ʼ��Ϸ �Զ�һ���Ӻ��ɢ
	pCurrTableFrame->SetTableAutoDismiss();

	/* ����������Ϣд�����ݿ� && ���͸��ͻ���(����Ϣ�ŵĺ��������з���) */
	DWORD dwClubRoomID = pDbo->dwClubRoomID;
	DWORD dwUserID = pIServerUserItem->GetUserID();
	DWORD dwTableID = pCurrTableFrame->GetTableID();
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
	//�û�����		TODO �����ÿͻ�����������
	if(pCurrTableFrame->PlayerSitTable(INVALID_CHAIR, pIServerUserItem, 0, true) != 0 )
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
	tagTableRule *pCfg = (tagTableRule*)pDbo->strCreateRoom.CommonRule;


	//��������Ʊ
	if(!CheckCreateTableTicket(pCfg, pIServerUserItem))
	{
		//return true; //TODONOW ���Ϊfalse �ͻ��˾Ͷ��������ˣ� ֮���޸ĵ�
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
		if(pCurrTableFrame->PlayerSitTable(wChairID, pIServerUserItem, 0, true) != 0)
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
	
	//���ͼ����ҷ���ɹ�
	g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_JOIN_ROOM_SUCCESS, NULL, 0);

	return true;
}


//�������� ���ֲ�����
bool CHandleFromGate::CreateTableAutoClub(STR_DBO_CG_USER_JOIN_TABLE_NO_PASS * pDbo, CPlayer *pIServerUserItem)
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
		if(pCurrTableFrame->PlayerSitTable(wChairID, pIServerUserItem, 0, true) != 0)
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
	return g_GameCtrl->PostDataBaseRequest(DBR_CG_JOIN_TABLE, dwSocketID, &DBR,sizeof(DBR));
}

//�������� ���� -- ��Ҫ����
bool CHandleFromGate::On_CMD_GC_JOIN_TABLE( DWORD dwSocketID, VOID * pData, WORD wDataSize)
{
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
	WORD wChairID = pCurrTableFrame->GetNullChairID();//Ѱ�ҿ�����
	if (wChairID != INVALID_CHAIR)
	{
		if(pCurrTableFrame->PlayerSitTable(wChairID, pIServerUserItem, 0, true) != 0)
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
	WORD wChairID = pCurrTableFrame->GetNullChairID();//Ѱ�ҿ�����
	if (wChairID != INVALID_CHAIR)
	{
		if(pCurrTableFrame->PlayerSitTable(wChairID, pIServerUserItem, 0, true) != 0)
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

	//Ͷ������
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
	WORD wChairID = pCurrTableFrame->GetNullChairID();//Ѱ�ҿ�����
	if (wChairID != INVALID_CHAIR)
	{
		if(pCurrTableFrame->PlayerSitTable(wChairID, pIServerUserItem, 0, true) != 0)
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

	//���ͼ��뷿��ɹ�
	g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_JOIN_ROOM_SUCCESS, NULL, 0);
	return true;
}

//��������
bool CHandleFromGate::On_SUB_User_ReconnectRoom(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{	
	//��У�黧
	WORD wBindIndex = LOWORD(dwSocketID);
	CPlayer *pIServerUserItem = GetBindUserItem(wBindIndex);
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
		if(wOldTableID > CTableManager::TableCount()) //��Ч����
		{
			wOldTableID = INVALID_TABLE;
		}
		else //�����Ѿ�����, ��wOldTableID����Ϊ��Ч
		{
			CTableFrame* pOldTable = CTableManager::FindTableByTableID(wOldTableID);
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
	if( CTableManager::FindTableByTableID(wOldTableID)->PlayerSitTable(wChairID, pIServerUserItem) != 0 )
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
		tagTableRule* pRule = (tagTableRule* )CTableManager::FindTableByTableID(wOldTableID)->GetCustomRule();
		ReJoinResult.retCode = 0;
		memcpy(&ReJoinResult.strTableRule, pRule, sizeof(ReJoinResult.strTableRule));
		ReJoinResult.wChairID = wChairID;
		ReJoinResult.dwPassWord = CTableManager::FindTableByTableID(wOldTableID)->GetTableID();
		ReJoinResult.cbGameCount = pRule->GameCount;
		ReJoinResult.cbPlayerCount = pRule->PlayerCount;
		ReJoinResult.cbPayType = pRule->cbPayType;
		ReJoinResult.lSinglePayCost = pRule->lSinglePayCost;
		ReJoinResult.gameMod = pRule->GameMode;
	}
	else
	{
		return true;
	}

	//��������
	g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_RECONNECT_ROOM, &ReJoinResult, sizeof(STR_CMD_GC_USER_RECONNECT_ROOM));

	//���������ɹ���ɾ��list
	if (ReJoinResult.retCode == 0)
	{
		//��Ϸ������  TODONOW
		pIServerUserItem->SetOfflineGameID(0);

		//���͸�Э����, ��ת������¼��
		tagOfflineUser data;
		data.dwUserID = pIServerUserItem->GetUserID();
		data.byMask = 2; //��ʾɾ�������û�
		g_TCPSocketEngine->SendData(MDM_USER,SUB_CS_C_USER_OFFLINE,&data,sizeof(tagOfflineUser));
	}
	return true;
}

//��������Ȧ����
bool CHandleFromGate::On_Sub_CG_User_CreateGroupRoom(VOID *pData, WORD wDataSize, DWORD dwSocketID)
{	
	//�û�У��
	WORD wBindIndex = LOWORD(dwSocketID);
	CPlayer *pIServerUserItem = GetBindUserItem(wBindIndex);	
	if (pIServerUserItem==NULL) return true;

	//���ݰ�У��
	if(wDataSize != sizeof(STR_SUB_CG_USER_CREATE_GROUP_ROOM))
		return true;

	//SUB����
	STR_SUB_CG_USER_CREATE_GROUP_ROOM *pCreateRoom = (STR_SUB_CG_USER_CREATE_GROUP_ROOM*)pData;

	//У��Ȧ���ǲ����Ѿ�����������Ȧ���䣬�������ҷ��䶼δ�����ˣ��Ͳ����ٴδ�������
	for (WORD i=0; i<CTableManager::TableCount(); i++)
	{
		//��ȡ����
		CTableFrame *pGroupTable = CTableManager::FindTableByIndex(i);
		if(NULL == pGroupTable) continue;

		//������Ȧ���䣬������һ������δ��ʼ��Ϸ����˵�����п����Ӽ��룬����Ҫ�ٴ�������
		if ( pGroupTable->GetGroupID() == pCreateRoom->dwGroupID)
		{
			//���ش����ɹ�
			STR_CMD_GC_USER_CREATE_GROUP_ROOM CMDCreate;
			ZeroMemory(&CMDCreate, sizeof(STR_CMD_GC_USER_CREATE_GROUP_ROOM));
			CMDCreate.dwResultCode = DB_SUCCESS;
			lstrcpyn(CMDCreate.szDescribeString, TEXT("����Ȧ���пշ���"), sizeof(CMDCreate.szDescribeString));

			//��������
			g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_CREATE_GROUP_ROOM, &CMDCreate, sizeof(STR_CMD_GC_USER_CREATE_GROUP_ROOM));	

			return true;
		}
	}

	//����DBR����
	STR_DBR_CG_USER_CREATE_GROUP_ROOM CreateGroupRoom;
	ZeroMemory(&CreateGroupRoom, sizeof(STR_DBR_CG_USER_CREATE_GROUP_ROOM));

	//��ֵ
	memcpy_s(&CreateGroupRoom, sizeof(STR_DBR_CG_USER_CREATE_GROUP_ROOM), pCreateRoom, sizeof(STR_DBR_CG_USER_CREATE_GROUP_ROOM));

	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_CG_USER_CREATE_GROUP_ROOM, dwSocketID, &CreateGroupRoom, sizeof(STR_DBR_CG_USER_CREATE_GROUP_ROOM));
	
	return true;
}

//��������Ȧ���䷵��
bool CHandleFromGate::On_CMD_GC_User_CreateGroupRoom(DWORD dwSocketID, VOID * pData, WORD wDataSize)
{
	//�ж�����

	//����У��
	ASSERT(wDataSize == sizeof(STR_DBO_CG_USER_CREATE_GROUP_ROOM));
	if(wDataSize != sizeof(STR_DBO_CG_USER_CREATE_GROUP_ROOM))
		return false;

	//��ȡ�û�
	WORD wBindIndex = LOWORD(dwSocketID);
	CPlayer *pIServerUserItem = GetBindUserItem(wBindIndex);

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
		g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_CREATE_GROUP_ROOM, &CMDCreate, sizeof(STR_CMD_GC_USER_CREATE_GROUP_ROOM));	
		return true;
	}

	//�������
	tagTableRule *pCfg = (tagTableRule* )pDBOGroup->rule;

	//У���û�����
	if ( pIServerUserItem->GetUserRoomCard() < pCfg->lSinglePayCost)
	{
		CMDCreate.dwResultCode = 1;
		lstrcpyn(CMDCreate.szDescribeString, TEXT("�������㣬���ֵ"), CountArray(CMDCreate.szDescribeString));

		g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_CREATE_GROUP_ROOM, &CMDCreate, sizeof(STR_CMD_GC_USER_CREATE_GROUP_ROOM));	
		return true;
	}

	//�û�����ܴ���������Ȧ������
	WORD wCreateTableNum = static_cast<WORD> (pIServerUserItem->GetUserRoomCard() / pCfg->lSinglePayCost);
	
	//Ѱ�ҿ������ӣ�Ϊ����Ȧ����̶���������
	srand(static_cast<unsigned int >(time(NULL)));
	int nTableNum = 0;
	for (WORD i=0; i<CTableManager::TableCount(); i++)
	{
		//��ȡ����
		CTableFrame *pGroupTable = CTableManager::FindTableByIndex(i);
		if(NULL == pGroupTable) continue;

		//�����������£����Ҳ�������˿��ķ���
		if ( (pGroupTable->GetSitUserCount()==0) && 
			 (pGroupTable->GetCreateTableUser() == 0))
		{
			//����Ȧ����������
			if ( (nTableNum >= CTableManager::TableCount()) || 
				 (NUM_MAX_GOURP_CREATE_ROOM == nTableNum) ||
				 nTableNum >= wCreateTableNum)
			{
				break;
			}

			//������������	
			DWORD dwPassword = GenerateTablePassword();		
			pGroupTable->SetTableID(dwPassword);
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
	g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_CREATE_GROUP_ROOM, &CMDCreate, sizeof(STR_CMD_GC_USER_CREATE_GROUP_ROOM));	

	//�۳������û�����
	SCORE lDeductRoomCard = (-nTableNum)*((int)pCfg->lSinglePayCost);
	
	//
	//OnEventUserOpenRoomCard(pIServerUserItem, pIServerUserItem->GetUserID(), lDeductRoomCard);

	return true;
}

//��������Ȧ����
bool CHandleFromGate::On_Sub_CG_User_JoinGroupRoom(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	ASSERT(wDataSize == sizeof(STR_SUB_CG_USER_JOIN_GROUP_ROOM));
	if (wDataSize != sizeof(STR_SUB_CG_USER_JOIN_GROUP_ROOM)) return true;

	//������Ϣ
	STR_SUB_CG_USER_JOIN_GROUP_ROOM *pJoinGroupRoom = (STR_SUB_CG_USER_JOIN_GROUP_ROOM *)pData;

	//����Ϣ
	CPlayer *pIServerUserItem = CPlayerManager::FindPlayerBySocketID(dwSocketID);

	//У���û�
	if (NULL == pIServerUserItem)
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
	g_GameCtrl->PostDataBaseRequest(DBR_CG_USER_JOIN_GROUP_ROOM, dwSocketID, &JoinRoom, sizeof(STR_DBR_CG_USER_JOIN_GROUP_ROOM));

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
			//�ҵ����䣬�ж��Ƿ��п�λ��
			WORD wChairID = pTableFrame->GetNullChairID();
			if(wChairID != INVALID_CHAIR)
			{
				//�û�����
				if(pTableFrame->PlayerSitTable(wChairID, pIServerUserItem, 0, true) != 0)
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
	}

	//�û��޷����룬���ʹ�����ʾ
	if ( !bHaveRoom )
	{
		SendRequestFailure(pIServerUserItem, TEXT("����Ȧ���������򲻴����ѿ����䣬����ϵ����"), REQUEST_FAILURE_PASSWORD);
	}

	return true;
}

//�����ҷ���
bool CHandleFromGate::On_SUB_User_JoinGoldRoom(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//�û�Ч��
	WORD wBindIndex = LOWORD(dwSocketID);
	CPlayer *pIServerUserItem = GetBindUserItem(wBindIndex);		
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
		if(pTableFrame->PlayerSitTable(wChairID, pIServerUserItem, 0, true) == 0)
		{
			//��������
			g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_JOIN_ROOM_SUCCESS, NULL, 0);
			return true;
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
		if(!pCurrTableFrame->PlayerSitTable(wChairID, pIServerUserItem, 0, true) != 0)
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
	g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_JOIN_ROOM_SUCCESS, NULL ,0);

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

//�齱���
bool CHandleFromGate::OnDBLotteryResult( DWORD dwSocketID, VOID * pData, WORD wDataSize )
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

	g_GameCtrl->SendData(dwSocketID,MDM_G_FRAME,SUB_GR_LOTTERY_RESULT,&LotteryResult,sizeof(CMD_GR_LotteryResult));

	return true;
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
CTableFrame* CHandleFromGate::GetGlodRoomEmptyChair(WORD &wChairID)
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
void CHandleFromGate::HandleCreateTableForOthers(CTableFrame *pCurTableFrame, CPlayer *pIServerUserItem, tagTableRule *pCfg)
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
	pCurTableFrame->SetTableID(dwPassword);

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


	//���ʹ�������ɹ���Ϣ
	g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_CREATE_ROOM_SUCCESS, NULL, 0);	

	//�����˿��������ȿ۳����������û��ķ��� �����˿���ֻ֧�ַ���ģʽ
	OnEventModifyUserTreasure(pIServerUserItem, dwPassword ,  TABLE_MODE_FK, 0,  -pCfg->lSinglePayCost, 0);
}

//����������������
bool CHandleFromGate::HandleCreateTable(CTableFrame *pCurTableFrame, CPlayer *pIServerUserItem, tagTableRule *pCfg)
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

	CLog::Log(log_debug, "PassWord: %d", dwPassword);

	//������������
	pCurTableFrame->SetTableOwner(pIServerUserItem->GetUserID());	//��������
	pCurTableFrame->SetTableID(dwPassword);

	//���÷����Զ���ɢ��Ĭ��һ���� -- ������ָ����ʼ��Ϸ �Զ�һ���Ӻ��ɢ
	pCurTableFrame->SetTableAutoDismiss();
	
	//��ÿ�����
	WORD wChairID = pCurTableFrame->GetNullChairID(); 

	//���Ӻ���Ч
	if (INVALID_CHAIR != wChairID)
	{
		//�û�����		TODO �����ÿͻ�����������
		if(pCurTableFrame->PlayerSitTable(wChairID, pIServerUserItem, 0, true) != 0)
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
bool CHandleFromGate::CheckJoinTableTicket(tagTableRule *pCfg, CPlayer *pIServerUserItem)
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
bool CHandleFromGate::CheckCreateTableTicket(tagTableRule *pCfg, CPlayer *pIServerUserItem)
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
