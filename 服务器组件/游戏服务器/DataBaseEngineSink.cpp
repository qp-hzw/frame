#include "StdAfx.h"
#include "GameCtrl.h"
#include "DataBaseEngineSink.h"
#include "AttemperEngineSink.h"

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDataBaseEngineSink::CDataBaseEngineSink()
{
	return;
}

//��������
CDataBaseEngineSink::~CDataBaseEngineSink()
{
	return;
}

//�����¼�
bool CDataBaseEngineSink::OnDataBaseEngineStart(IUnknownEx * pIUnknownEx)
{
	m_PlatformDB = static_cast<IDataBase*>(CWHModule::Database());
	m_TreasureDB = static_cast<IDataBase*>(CWHModule::Database());
	
	if(m_PlatformDB == NULL) return false;
	if(m_TreasureDB == NULL) return false;

	//��������
	bool bRet1 = m_PlatformDB->Connect(1);
	bool bRet2 = m_TreasureDB->Connect(3);

	if(!bRet1)
	{
		return false;
	}

	if(!bRet2)
	{
		return false;
	}
	return true;
}

//ֹͣ�¼�
bool CDataBaseEngineSink::OnDataBaseEngineConclude(IUnknownEx * pIUnknownEx)
{
	return true;
}


//�����¼�
bool CDataBaseEngineSink::OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//��������
	bool bSucceed = false;
	DWORD dwUserID = 0L;

	//������
	switch (wRequestID)
	{
	case DBR_GP_GAME_USER_STATE: //socket���ӹر�
		{
			bSucceed = On_DBR_GP_QUIT(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_CG_LOGON_USERID:			//I D ��¼
		{
			bSucceed = On_DBR_Logon_UserID(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_CG_USER_CREATE_GROUP_ROOM:	//��������Ȧ����
		{
			bSucceed = On_DBR_User_CreateGroupRoom(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_CG_USER_JOIN_GROUP_ROOM:	//��������Ȧ����
		{
			bSucceed = On_DBR_User_JoinGroupRoom(dwContextID,pData,wDataSize,dwUserID);
		}
		break;		
	case DBR_GR_WRITE_GAME_SCORE:		//��Ϸд��
		{
			bSucceed = OnRequestWriteGameScore(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_LEAVE_GAME_SERVER:		//�뿪��Ϸ��
		{
			bSucceed = OnRequestLeaveGameServer(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_GAME_SCORE_RECORD:		//��Ϸ��¼
		{
			bSucceed = OnRequestGameScoreRecord(dwContextID,pData,wDataSize,dwUserID);
		}
		break;

	case DBR_GR_LOAD_ANDROID_USER:		//���ػ���
		{
			bSucceed = OnRequestLoadAndroidUser(dwContextID,pData,wDataSize,dwUserID);
			break;
		}
	case DBR_GR_LOAD_OFFLINE:			//���ض�������
		{
			bSucceed = On_DBR_GR_LOAD_OFFLINE(dwContextID,pData,wDataSize,dwUserID);
			break;
		}
	case DBR_GR_MODIFY_USER_TREASURE:		//�޸��û��Ƹ���Ϣ
		{
			bSucceed = On_DBR_ModifyUserTreasure(dwContextID,pData,wDataSize,dwUserID);
			break;
		}
	case DBR_GR_SAVE_RECORDINFO:			//����С��¼����Ϣ
		{
			bSucceed = On_DBR_SaveGameRecord(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_MANAGE_USER_RIGHT:		//�û�Ȩ��
		{
			bSucceed = OnRequestManageUserRight(dwContextID,pData,wDataSize,dwUserID);
		}
		break;

	case DBR_GR_MATCH_FEE:				//��������
		{
			bSucceed = OnRequestMatchFee(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_MATCH_START:			//������ʼ
		{
			bSucceed = OnRequestMatchStart(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_MATCH_OVER:				//��������
		{
			bSucceed = OnRequestMatchOver(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_MATCH_REWARD:			//��������
		{
			bSucceed = OnRequestMatchReward(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_MATCH_QUIT:				//�˳�����
		{
			bSucceed = OnRequestMatchQuit(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_GR_WRITE_CURRENT_STOCK:
		{
			bSucceed = OnWriteCurrentStock(dwContextID,pData,wDataSize,dwUserID);
		}
		break;
	case DBR_CLUB_ROOM_INFO:	//club����Ȧ������Ϣ
		{
			bSucceed = On_DBR_CLUB_ROOM_INFO(dwContextID,pData,wDataSize);
			break;
		}
	case DBR_CLUB_TABLE_INFO: //club����Ȧ������Ϣ
		{
			bSucceed = On_DBR_CLUB_TABLE_INFO(dwContextID,pData,wDataSize);
			break;
		}
	case DBR_CLUB_PLAYER_INFO:	//club����Ȧ�����Ϣ
		{
			bSucceed = On_DBR_CLUB_PLAYER_INFO(dwContextID,pData,wDataSize);
			break;
		}
	case DBR_HALL_GOLD_TABLE_INFO: //��Ҵ��� ������Ϣ
		{
			bSucceed = On_DBR_HALL_GOLD_TABLE_INFO(dwContextID,pData,wDataSize);
			break;
		}
	case DBR_HALL_GOLD_PLAYER_INFO: //��Ҵ��� �����Ϣ
		{
			bSucceed = On_DBR_HALL_GOLD_PLAYER_INFO(dwContextID,pData,wDataSize);
			break;
		}
	case DBR_GR_ADD_TABLE_INFO:		//��ӿ�����Ϣ
		{
			bSucceed = OnRequestAddTableInfo(dwContextID,pData,wDataSize);
		}
		break;
	case DBR_GR_START_TABLE:	//��ʼ����
		{
			bSucceed = OnRequestStartTable(dwContextID,pData,wDataSize);
		}
		break;
	case DBR_GR_END_TABLE:	//��������
		{
			bSucceed = OnRequestEndTable(dwContextID,pData,wDataSize);
		}
		break;
	case DBR_GR_ADD_TABLEUSER://�������û�
		{
			bSucceed = OnRequestAddUserToTable(dwContextID,pData,wDataSize);
		}
		break;
	case DBR_GR_DELETE_TABLEUSER://ɾ�����û�
		{
			bSucceed = OnRequestDeleteUserToTable(dwContextID,pData,wDataSize);
		}
		break;
	case DBR_GR_QUERY_LOTTERY:
		{
			bSucceed = OnQueryLottery(dwContextID,pData,wDataSize);
		}
		break;
	case DBR_SC_TABLE_UPDATE_TASK_STATUS:		//������Ϸ����״̬
		{
			bSucceed = On_Table_UpdateGameTaskStatus(dwContextID,pData,wDataSize);
		}
		break;
	case DBR_CG_CLUB_CREATE_TABLE://����Ȧ��������
		{
			bSucceed = On_DBR_CG_CLUB_CREATE_TABLE(dwContextID,pData,wDataSize);
			break;
		}
	case DBR_CG_USER_JOIN_TABLE_NO_PASS://�������� ����Ҫ����,���ټ���
		{
			bSucceed = On_DBR_CG_USER_JOIN_TABLE_NO_PASS(dwContextID,pData,wDataSize);
			break;
		}
	case DBR_CG_JOIN_TABLE: //��������
		{
			bSucceed = On_DBR_CG_JOIN_TABLE(dwContextID,pData,wDataSize);
			break;
		}
	case DBR_CG_USER_JOIN_TABLE_HALL_GOLD: //�������� -- ��Ҵ�������
		{
			bSucceed = On_DBR_CG_USER_JOIN_TABLE_HALL_GOLD(dwContextID,pData,wDataSize);
			break;
		}
	}

	return bSucceed;
}

//ID ��¼
bool CDataBaseEngineSink::On_DBR_Logon_UserID(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//����У��
	if (wDataSize!=sizeof(STR_DBR_CG_LOGON_USERID)) return false;

	//ִ�в�ѯ
	STR_DBR_CG_LOGON_USERID *pLogonUserID = (STR_DBR_CG_LOGON_USERID *)pData;

	//ת����ַ
	TCHAR szClientAddr[16]=TEXT("");
	BYTE *pClientAddr = (BYTE *)&pLogonUserID->dwClientAddr;
	if ( NULL != pClientAddr )
	{
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);
	}

	//�������
	CLog::Log(log_debug, "1");
	m_TreasureDB->ResetParameter();
	CLog::Log(log_debug, "2");
	m_TreasureDB->AddParameter(TEXT("@dwUserID"),pLogonUserID->dwUserID);
	CLog::Log(log_debug, "3");
	m_TreasureDB->AddParameter(TEXT("@strPassword"),pLogonUserID->szPassword);
	CLog::Log(log_debug, "4");
	m_TreasureDB->AddParameter(TEXT("@strClientIP"),szClientAddr);
	CLog::Log(log_debug, "5");
	m_TreasureDB->AddParameter(TEXT("@strMachineID"),pLogonUserID->szMachineID);
	CLog::Log(log_debug, "6");
	m_TreasureDB->AddParameter(TEXT("@dwGameID"),g_GameCtrl->GetServerID());

	CLog::Log(log_debug, "7");
	//�������
	TCHAR szDescribeString[128]=TEXT("");
	m_TreasureDB->AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString),adParamOutput);

	CLog::Log(log_debug, "8");
	//ִ�в�ѯ
	LONG lResultCode = m_TreasureDB->ExecuteProcess(TEXT("GSP_CG_Logon_UserID"),true);

	CLog::Log(log_debug, "lResultCode: %d", lResultCode);

	//�������
	CDBVarValue DBVarValue;
	m_TreasureDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	On_DBO_Logon_UserID(dwContextID, pLogonUserID->dLongitude, pLogonUserID->dLatitude, lResultCode, CW2CT(DBVarValue.bstrVal));

	return true;
}
//�û�Socket���ӹر�
bool CDataBaseEngineSink::On_DBR_GP_QUIT(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	if (wDataSize!=sizeof(DBR_GP_UserQuitInfo)) return false;

	DBR_GP_UserQuitInfo* groupInfo = (DBR_GP_UserQuitInfo*)pData;
	//�������
	m_TreasureDB->ResetParameter();

	m_TreasureDB->AddParameter(TEXT("@dwUserID"),groupInfo->dwUserID);
	m_TreasureDB->AddParameter(TEXT("@byOnlineMask"),groupInfo->byOnlineMask);

	//ִ�����
	m_TreasureDB->ExecuteProcess(TEXT("GSP_CL_Quit"),false);	  

	return true;

}


//ID��¼����
bool CDataBaseEngineSink::On_DBO_Logon_UserID(DWORD dwContextID, const double &dLongitude, const double &dLatitude, DWORD dwErrorCode, LPCTSTR pszErrorString)
{
	//��������
	STR_DBO_CG_LOGON_USERID LogonUserID;
	ZeroMemory(&LogonUserID, sizeof(STR_DBO_CG_LOGON_USERID));

	//��ֵ
	LogonUserID.lResultCode = dwErrorCode;
	lstrcpyn(LogonUserID.szDescribeString, pszErrorString, CountArray(LogonUserID.szDescribeString));

	//ִ�гɹ�����ȡ�û���Ϣ
	if ( DB_SUCCESS == dwErrorCode )
	{	
		//��������
		LogonUserID.wFaceID = m_TreasureDB->GetValue_WORD(TEXT("FaceID"));
		LogonUserID.dwUserID=m_TreasureDB->GetValue_DWORD(TEXT("UserID"));
		LogonUserID.dwGameID=m_TreasureDB->GetValue_DWORD(TEXT("GameID"));
		LogonUserID.dwGroupID=m_TreasureDB->GetValue_DWORD(TEXT("GroupID"));
		LogonUserID.dwCustomID=m_TreasureDB->GetValue_DWORD(TEXT("CustomID"));
		m_TreasureDB->GetValue_String(TEXT("NickName"),LogonUserID.szNickName,CountArray(LogonUserID.szNickName));
		m_TreasureDB->GetValue_String(TEXT("GroupName"),LogonUserID.szGroupName,CountArray(LogonUserID.szGroupName));
		m_TreasureDB->GetValue_String(TEXT("HeadUrl"),LogonUserID.szHeadUrl,CountArray(LogonUserID.szHeadUrl));

		//�û�����
		LogonUserID.cbGender=m_TreasureDB->GetValue_BYTE(TEXT("Gender"));
		LogonUserID.cbMemberOrder=m_TreasureDB->GetValue_BYTE(TEXT("MemberOrder"));
		LogonUserID.cbMasterOrder=m_TreasureDB->GetValue_BYTE(TEXT("MasterOrder"));
		m_TreasureDB->GetValue_String(TEXT("UnderWrite"),LogonUserID.szUnderWrite,CountArray(LogonUserID.szUnderWrite));

		//������Ϣ
		LogonUserID.lScore=m_TreasureDB->GetValue_LONGLONG(TEXT("Score"));
		LogonUserID.lGrade=m_TreasureDB->GetValue_LONGLONG(TEXT("Grade"));
		LogonUserID.lControlScore=m_TreasureDB->GetValue_LONGLONG(TEXT("ControlScore"));

		//������Ϣ add by guo
		LogonUserID.lOpenRoomCard = m_TreasureDB->GetValue_LONGLONG(TEXT("OpenRoomCard"));
		LogonUserID.lDiamond = m_TreasureDB->GetValue_LONGLONG(TEXT("Diamond"));
		LogonUserID.lGold = m_TreasureDB->GetValue_LONGLONG(TEXT("Gold"));

		//������Ϣ
		LogonUserID.dwWinCount=m_TreasureDB->GetValue_LONG(TEXT("WinCount"));
		LogonUserID.dwLostCount=m_TreasureDB->GetValue_LONG(TEXT("LostCount"));
		LogonUserID.dwDrawCount=m_TreasureDB->GetValue_LONG(TEXT("DrawCount"));
		LogonUserID.dwFleeCount=m_TreasureDB->GetValue_LONG(TEXT("FleeCount"));
		LogonUserID.dwUserMedal=m_TreasureDB->GetValue_LONG(TEXT("UserMedal"));
		LogonUserID.dwExperience=m_TreasureDB->GetValue_LONG(TEXT("Experience"));
		LogonUserID.lLoveLiness=m_TreasureDB->GetValue_LONG(TEXT("LoveLiness"));

		//������Ϣ
		LogonUserID.dwUserRight=m_TreasureDB->GetValue_DWORD(TEXT("UserRight"));
		LogonUserID.dwMasterRight=m_TreasureDB->GetValue_DWORD(TEXT("MasterRight"));
		LogonUserID.cbDeviceType = DEVICE_TYPE_PC;
		LogonUserID.wBehaviorFlags = 0;
		LogonUserID.wPageTableCount = 0;

		//λ����Ϣ
		LogonUserID.dLatitude = dLatitude;
		LogonUserID.dLongitude = dLongitude;

		//��������
		LogonUserID.dwInoutIndex = m_TreasureDB->GetValue_DWORD(TEXT("InoutIndex"));
	}

	//��������
	g_AttemperEngineSink->OnEventDataBaseResult(DBO_CG_LOGON_USERID, dwContextID, &LogonUserID, sizeof(STR_DBO_CG_LOGON_USERID));
	return true;
}

//��������Ȧ����
bool CDataBaseEngineSink::On_DBR_User_CreateGroupRoom(DWORD dwContextID, VOID *pData, WORD wDataSize, DWORD &dwUserID)
{
	//����
	STR_DBR_CG_USER_CREATE_GROUP_ROOM *pDBRCreateGoupRoom = (STR_DBR_CG_USER_CREATE_GROUP_ROOM *)pData;
	dwUserID = pDBRCreateGoupRoom->dwUserID;

	//Ч�����
	ASSERT(wDataSize == sizeof(STR_DBR_CG_USER_CREATE_GROUP_ROOM));
	if (wDataSize != sizeof(STR_DBR_CG_USER_CREATE_GROUP_ROOM)) return false;

	tagTableRule *pCfg = (tagTableRule* )pDBRCreateGoupRoom->rule;
	if (NULL == pCfg)
		return false;

	//�����������
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@dwUserID"), pDBRCreateGoupRoom->dwUserID);
	m_TreasureDB->AddParameter(TEXT("@dwGroupID"), pDBRCreateGoupRoom->dwGroupID);
	m_TreasureDB->AddParameter(TEXT("@RoomPayCost"), pCfg->lSinglePayCost);

	//�������
	TCHAR szDescribeString[128]=TEXT("");
	m_TreasureDB->AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString),adParamOutput);

	//ִ�в�ѯ
	LONG lResultCode = m_TreasureDB->ExecuteProcess(TEXT("GSP_CG_User_CreateGroupRoom"),true);

	//�������
	CDBVarValue DBVarValue;
	m_TreasureDB->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

	On_DBO_User_CreateGroupRoom(dwContextID, pDBRCreateGoupRoom->rule, lResultCode, CW2CT(DBVarValue.bstrVal));


	return true;
}

//��������Ȧ���䷵��
VOID CDataBaseEngineSink::On_DBO_User_CreateGroupRoom(DWORD dwContextID, BYTE RULE[LEN_PRIVATE_TABLE_RULE], DWORD dwErrorCode, LPCTSTR pszErrorString)
{
	//��������
	STR_DBO_CG_USER_CREATE_GROUP_ROOM CreateGroupRoom;
	ZeroMemory(&CreateGroupRoom, sizeof(STR_DBO_CG_USER_CREATE_GROUP_ROOM));

	//������Ϣ
	CreateGroupRoom.dwResultCode = dwErrorCode;
	lstrcpyn(CreateGroupRoom.szDescribeString, pszErrorString, sizeof(STR_DBO_CG_USER_CREATE_GROUP_ROOM) );

	//ִ�гɹ�
	if(dwErrorCode == DB_SUCCESS)
	{
		//��������
		CreateGroupRoom.dwUserID = m_TreasureDB->GetValue_DWORD(TEXT("UserID"));
		CreateGroupRoom.dwGroupID = m_TreasureDB->GetValue_DWORD(TEXT("GroupID"));

		//����
		memcpy_s(CreateGroupRoom.rule, sizeof(CreateGroupRoom.rule), RULE, sizeof(CreateGroupRoom.rule));
	}

	//��������
	g_AttemperEngineSink->OnEventDataBaseResult(DBO_CG_USER_CREATE_GROUP_ROOM, dwContextID, &CreateGroupRoom, sizeof(STR_DBO_CG_USER_CREATE_GROUP_ROOM));
}

//��������Ȧ����
bool CDataBaseEngineSink::On_DBR_User_JoinGroupRoom(DWORD dwContextID, VOID *pData, WORD wDataSize, DWORD &dwUserID)
{
	//����
	STR_DBR_CG_USER_JOIN_GROUP_ROOM *pDBRJoinGoupRoom = (STR_DBR_CG_USER_JOIN_GROUP_ROOM *)pData;
	dwUserID = pDBRJoinGoupRoom->dwUserID;

	//Ч�����
	ASSERT(wDataSize == sizeof(STR_DBR_CG_USER_JOIN_GROUP_ROOM));
	if (wDataSize != sizeof(STR_DBR_CG_USER_JOIN_GROUP_ROOM)) return false;


	//�����������
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@dwUserID"), pDBRJoinGoupRoom->dwUserID);
	m_TreasureDB->AddParameter(TEXT("@dwGroupID"), pDBRJoinGoupRoom->dwGroupID);

	//�������
	TCHAR szDescribeString[128] = TEXT("");
	m_TreasureDB->AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString),adParamOutput);

	//ִ�в�ѯ
	LONG lResultCode = m_TreasureDB->ExecuteProcess(TEXT("GSP_CG_User_JoinGroupRoom"),true);

	//�������
	CDBVarValue DBVarValue;
	m_TreasureDB->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

	On_DBO_User_JoinGroupRoom(dwContextID, pDBRJoinGoupRoom->dwGroupID, lResultCode, CW2CT(DBVarValue.bstrVal));

	return true;
}

//��������Ȧ���䷵��
VOID CDataBaseEngineSink::On_DBO_User_JoinGroupRoom(DWORD dwContextID,  DWORD dwGroupID, DWORD dwErrorCode, LPCTSTR pszErrorString)
{
	//��������
	STR_DBO_CG_USER_JOIN_GROUP_ROOM JoinGroupRoom;
	ZeroMemory(&JoinGroupRoom, sizeof(STR_DBO_CG_USER_JOIN_GROUP_ROOM));

	//������Ϣ
	JoinGroupRoom.dwResultCode = dwErrorCode;
	lstrcpyn(JoinGroupRoom.szDescribeString, pszErrorString, sizeof(STR_DBO_CG_USER_JOIN_GROUP_ROOM) );

	//ִ�гɹ�
	if(dwErrorCode == DB_SUCCESS)
	{
		JoinGroupRoom.dwGroupID = dwGroupID/*m_TreasureDB->GetValue_DWORD(TEXT("GroupID"))*/;
	}

	//��������
	g_AttemperEngineSink->OnEventDataBaseResult(DBO_CG_USER_JOIN_GROUP_ROOM, dwContextID, &JoinGroupRoom, sizeof(STR_DBO_CG_USER_JOIN_GROUP_ROOM));
}


//��Ϸд��
bool CDataBaseEngineSink::OnRequestWriteGameScore(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//������
	DBR_GR_WriteGameScore * pWriteGameScore=(DBR_GR_WriteGameScore *)pData;
	dwUserID=pWriteGameScore->dwUserID;

	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_WriteGameScore));
	if (wDataSize!=sizeof(DBR_GR_WriteGameScore)) return false;

	//ת����ַ
	TCHAR szClientAddr[16]=TEXT("");
	BYTE * pClientAddr=(BYTE *)&pWriteGameScore->dwClientAddr;
	_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

	//�������
	m_TreasureDB->ResetParameter();

	//�û���Ϣ
	m_TreasureDB->AddParameter(TEXT("@dwUserID"),pWriteGameScore->dwUserID);

	//����ɼ�
	m_TreasureDB->AddParameter(TEXT("@lVariationScore"),pWriteGameScore->VariationInfo.lScore);
	m_TreasureDB->AddParameter(TEXT("@lVariationGrade"),pWriteGameScore->VariationInfo.lGrade);
	m_TreasureDB->AddParameter(TEXT("@lVariationInsure"),pWriteGameScore->VariationInfo.lInsure);
	m_TreasureDB->AddParameter(TEXT("@lVariationRevenue"),pWriteGameScore->VariationInfo.lRevenue);
	m_TreasureDB->AddParameter(TEXT("@lVariationWinCount"),pWriteGameScore->VariationInfo.dwWinCount);
	m_TreasureDB->AddParameter(TEXT("@lVariationLostCount"),pWriteGameScore->VariationInfo.dwLostCount);
	m_TreasureDB->AddParameter(TEXT("@lVariationDrawCount"),pWriteGameScore->VariationInfo.dwDrawCount);
	m_TreasureDB->AddParameter(TEXT("@lVariationFleeCount"),pWriteGameScore->VariationInfo.dwFleeCount);
	m_TreasureDB->AddParameter(TEXT("@lVariationUserMedal"),pWriteGameScore->VariationInfo.dwUserMedal);
	m_TreasureDB->AddParameter(TEXT("@lVariationExperience"),pWriteGameScore->VariationInfo.dwExperience);
	m_TreasureDB->AddParameter(TEXT("@lVariationLoveLiness"),pWriteGameScore->VariationInfo.lLoveLiness);
	m_TreasureDB->AddParameter(TEXT("@dwVariationPlayTimeCount"),pWriteGameScore->VariationInfo.dwPlayTimeCount);

	//����ֵ
	m_TreasureDB->AddParameter(TEXT("@lControlScore"),pWriteGameScore->lControlScore);

	//��ˮ��Ϣ
	m_TreasureDB->AddParameter(TEXT("@lChoushui"),pWriteGameScore->VariationInfo.lChoushui);;
	m_TreasureDB->AddParameter(TEXT("@dwChoushuiType"),pWriteGameScore->VariationInfo.dwChoushuiType);;

	//������Ϣ
	//TODONOW 
	m_TreasureDB->AddParameter(TEXT("@wKindID"),g_GameCtrl->GetServerID());
	m_TreasureDB->AddParameter(TEXT("@wServerID"),g_GameCtrl->GetServerID());
	m_TreasureDB->AddParameter(TEXT("@strClientIP"),szClientAddr);

	//ִ�в�ѯ
	LONG lResultCode=m_TreasureDB->ExecuteProcess(TEXT("GSP_GR_WriteGameScore"),true);

	return true;
}

//�޸��û��Ƹ�
bool CDataBaseEngineSink::On_DBR_ModifyUserTreasure(DWORD dwContextID, void * pData, WORD wDataSize, DWORD &dwUserID)
{
	//���ݴ�СУ��
	if (sizeof(STR_DBR_GR_MODIFY_USER_TREASURE) != wDataSize)
		return false;

	//��������
	STR_DBR_GR_MODIFY_USER_TREASURE* pDBR = (STR_DBR_GR_MODIFY_USER_TREASURE*)pData;

	//�������
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@UserID"),pDBR->dwUserId);
	m_TreasureDB->AddParameter(TEXT("@dwKind"),pDBR->dwKind);
	m_TreasureDB->AddParameter(TEXT("@dwTableID"),pDBR->dwTableID);
	m_TreasureDB->AddParameter(TEXT("@byTableMode"),pDBR->byTableMode);
	m_TreasureDB->AddParameter(TEXT("@byRound"),pDBR->byRound);
	m_TreasureDB->AddParameter(TEXT("@lUserTreasure"),pDBR->lUserTreasure);

	BYTE byWinOrLose = 0; //0ƽ;  1Ӯ;  2��

	if( pDBR->byRound != 0xFF) //С��
	{
		if(pDBR->lUserTreasure > 0)
		{
			byWinOrLose = 1;
		}
		else if(pDBR->lUserTreasure < 0)
		{
			byWinOrLose = 2;
		}
	}
	else //���
	{
		byWinOrLose = pDBR->byWin;
	}
	m_TreasureDB->AddParameter(TEXT("@byWinOrLose"),byWinOrLose);

	//�������
	TCHAR szDescribeString[128] = TEXT("");
	m_TreasureDB->AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

	//ִ�в�ѯ
	LONG lResultCode = m_TreasureDB->ExecuteProcess(TEXT("GSP_MODIFY_USER_TREASURE_TABLE"), true);

	//�������
	CDBVarValue DBVarValue;
	m_TreasureDB->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

	//ִ�гɹ�
	if(DB_SUCCESS == lResultCode)
	{
		//��������
		STR_DBO_GR_MODIFY_USER_TREASURE ModifyTreasure;
		ZeroMemory(&ModifyTreasure, sizeof(STR_DBO_GR_MODIFY_USER_TREASURE));

		//������Ϣ
		ModifyTreasure.dwResultCode = lResultCode;
		lstrcpyn(ModifyTreasure.szDescribeString, CW2CT(DBVarValue.bstrVal), CountArray(ModifyTreasure.szDescribeString) );

		ModifyTreasure.dwUserID = m_TreasureDB->GetValue_DWORD(TEXT("UserID"));
		//�û���Ϸ��
		ModifyTreasure.lUserDiamond = m_TreasureDB->GetValue_LONGLONG(TEXT("UserDiamond"));
		//�û���Ϸ��
		ModifyTreasure.lUserGold = m_TreasureDB->GetValue_LONGLONG(TEXT("UserGold"));
		//�û�����
		ModifyTreasure.lOpenRoomCard = m_TreasureDB->GetValue_LONGLONG(TEXT("OpenRoomCard"));

		//��������
		g_AttemperEngineSink->OnEventDataBaseResult(DBO_SG_MODIFY_USER_TREASURE, dwContextID, &ModifyTreasure, sizeof(STR_DBO_GR_MODIFY_USER_TREASURE));
	}
	return true;
}

//����С��¼����Ϣ
bool CDataBaseEngineSink::On_DBR_SaveGameRecord(DWORD dwContextID, void * pData, WORD wDataSize, DWORD &dwUserID)
{
	if (sizeof(DBR_GR_GameRecordInfo) != wDataSize)
		return false;

	//���
	DBR_GR_GameRecordInfo *pDbReq = (DBR_GR_GameRecordInfo*)pData;

	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@dwTableID"),pDbReq->dwTableID);
	m_TreasureDB->AddParameter(TEXT("@curRound"),pDbReq->wCurrentCount);
	CString szData = toHexString(pDbReq->szData, LEN_MAX_RECORD_SIZE);
	m_TreasureDB->AddParameter(TEXT("@VideoData"),szData);
	m_TreasureDB->AddParameter(TEXT("@VideoSize"),LEN_MAX_RECORD_SIZE);

	m_TreasureDB->ExecuteProcess(TEXT("GSP_GR_SaveGameRecord"), true);


	return true;
}

//�뿪����
bool CDataBaseEngineSink::OnRequestLeaveGameServer(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//������
	DBR_GR_LeaveGameServer * pLeaveGameServer=(DBR_GR_LeaveGameServer *)pData;
	dwUserID=pLeaveGameServer->dwUserID;


	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_LeaveGameServer));
	if (wDataSize!=sizeof(DBR_GR_LeaveGameServer)) return false;

	//ת����ַ
	TCHAR szClientAddr[16]=TEXT("");
	BYTE * pClientAddr=(BYTE *)&pLeaveGameServer->dwClientAddr;
	_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

	//�������
	m_TreasureDB->ResetParameter();

	//�û���Ϣ
	m_TreasureDB->AddParameter(TEXT("@dwUserID"),pLeaveGameServer->dwUserID);
	m_TreasureDB->AddParameter(TEXT("@dwOnLineTimeCount"),pLeaveGameServer->dwOnLineTimeCount);

	//ϵͳ��Ϣ
	m_TreasureDB->AddParameter(TEXT("@dwInoutIndex"),pLeaveGameServer->dwInoutIndex);
	m_TreasureDB->AddParameter(TEXT("@dwLeaveReason"),pLeaveGameServer->dwLeaveReason);

	/*
	//��¼�ɼ�
	m_TreasureDB->AddParameter(TEXT("@lRecordScore"),pLeaveGameServer->RecordInfo.lScore);
	m_TreasureDB->AddParameter(TEXT("@lRecordGrade"),pLeaveGameServer->RecordInfo.lGrade);
	m_TreasureDB->AddParameter(TEXT("@lRecordInsure"),pLeaveGameServer->RecordInfo.lInsure);
	m_TreasureDB->AddParameter(TEXT("@lRecordRevenue"),pLeaveGameServer->RecordInfo.lRevenue);
	m_TreasureDB->AddParameter(TEXT("@lRecordWinCount"),pLeaveGameServer->RecordInfo.dwWinCount);
	m_TreasureDB->AddParameter(TEXT("@lRecordLostCount"),pLeaveGameServer->RecordInfo.dwLostCount);
	m_TreasureDB->AddParameter(TEXT("@lRecordDrawCount"),pLeaveGameServer->RecordInfo.dwDrawCount);
	m_TreasureDB->AddParameter(TEXT("@lRecordFleeCount"),pLeaveGameServer->RecordInfo.dwFleeCount);
	m_TreasureDB->AddParameter(TEXT("@lRecordUserMedal"),pLeaveGameServer->RecordInfo.dwUserMedal);
	m_TreasureDB->AddParameter(TEXT("@lRecordExperience"),pLeaveGameServer->RecordInfo.dwExperience);
	m_TreasureDB->AddParameter(TEXT("@lRecordLoveLiness"),pLeaveGameServer->RecordInfo.lLoveLiness);
	m_TreasureDB->AddParameter(TEXT("@dwRecordPlayTimeCount"),pLeaveGameServer->RecordInfo.dwPlayTimeCount);

	//����ɼ�
	m_TreasureDB->AddParameter(TEXT("@lVariationScore"),pLeaveGameServer->VariationInfo.lScore);
	m_TreasureDB->AddParameter(TEXT("@lVariationGrade"),pLeaveGameServer->VariationInfo.lGrade);
	m_TreasureDB->AddParameter(TEXT("@lVariationInsure"),pLeaveGameServer->VariationInfo.lInsure);
	m_TreasureDB->AddParameter(TEXT("@lVariationRevenue"),pLeaveGameServer->VariationInfo.lRevenue);
	m_TreasureDB->AddParameter(TEXT("@lVariationWinCount"),pLeaveGameServer->VariationInfo.dwWinCount);
	m_TreasureDB->AddParameter(TEXT("@lVariationLostCount"),pLeaveGameServer->VariationInfo.dwLostCount);
	m_TreasureDB->AddParameter(TEXT("@lVariationDrawCount"),pLeaveGameServer->VariationInfo.dwDrawCount);
	m_TreasureDB->AddParameter(TEXT("@lVariationFleeCount"),pLeaveGameServer->VariationInfo.dwFleeCount);
	m_TreasureDB->AddParameter(TEXT("@lVariationUserMedal"),pLeaveGameServer->VariationInfo.dwUserMedal);
	m_TreasureDB->AddParameter(TEXT("@lVariationExperience"),pLeaveGameServer->VariationInfo.dwExperience);
	m_TreasureDB->AddParameter(TEXT("@lVariationLoveLiness"),pLeaveGameServer->VariationInfo.lLoveLiness);
	m_TreasureDB->AddParameter(TEXT("@dwVariationPlayTimeCount"),pLeaveGameServer->VariationInfo.dwPlayTimeCount);
	*/

	//////������Ϣ(����Ҫд)
	//m_TreasureDB->AddParameter(TEXT("@lControlScore"),pLeaveGameServer->lControlScore);

	////��ˮ��Ϣ
	//m_TreasureDB->AddParameter(TEXT("@lChoushui"),pLeaveGameServer->VariationInfo.lChoushui);
	//m_TreasureDB->AddParameter(TEXT("@dwChoushuiType"),pLeaveGameServer->VariationInfo.dwChoushuiType);

	//��������
	//TODONOW 
	m_TreasureDB->AddParameter(TEXT("@wKindID"),g_GameCtrl->GetServerID());
	m_TreasureDB->AddParameter(TEXT("@wServerID"),g_GameCtrl->GetServerID());
	m_TreasureDB->AddParameter(TEXT("@strClientIP"),szClientAddr);
	m_TreasureDB->AddParameter(TEXT("@strMachineID"),pLeaveGameServer->szMachineID);

	//ִ�в�ѯ
	LONG lResultCode=m_TreasureDB->ExecuteProcess(TEXT("GSP_GR_LeaveGameServer"),true);

	return true;
}

//��Ϸ��¼
bool CDataBaseEngineSink::OnRequestGameScoreRecord(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//��������
	DBR_GR_GameScoreRecord * pGameScoreRecord=(DBR_GR_GameScoreRecord *)pData;
	dwUserID=INVALID_DWORD;

	//Ч�����
	ASSERT(wDataSize<=sizeof(DBR_GR_GameScoreRecord));
	ASSERT(wDataSize>=(sizeof(DBR_GR_GameScoreRecord)-sizeof(pGameScoreRecord->GameScoreRecord)));
	ASSERT(wDataSize==(sizeof(DBR_GR_GameScoreRecord)-sizeof(pGameScoreRecord->GameScoreRecord)+pGameScoreRecord->wRecordCount*sizeof(pGameScoreRecord->GameScoreRecord[0])));

	//Ч�����
	if (wDataSize>sizeof(DBR_GR_GameScoreRecord)) return false;
	if (wDataSize<(sizeof(DBR_GR_GameScoreRecord)-sizeof(pGameScoreRecord->GameScoreRecord))) return false;
	if (wDataSize!=(sizeof(DBR_GR_GameScoreRecord)-sizeof(pGameScoreRecord->GameScoreRecord)+pGameScoreRecord->wRecordCount*sizeof(pGameScoreRecord->GameScoreRecord[0]))) return false;

	//������Ϣ
	m_TreasureDB->ResetParameter();
	//TODONOW 
	m_TreasureDB->AddParameter(TEXT("@wKindID"),g_GameCtrl->GetServerID());
	m_TreasureDB->AddParameter(TEXT("@wServerID"),g_GameCtrl->GetServerID());

	//������Ϣ
	m_TreasureDB->AddParameter(TEXT("@wTableID"),pGameScoreRecord->wTableID);
	m_TreasureDB->AddParameter(TEXT("@wUserCount"),pGameScoreRecord->wUserCount);
	m_TreasureDB->AddParameter(TEXT("@wAndroidCount"),pGameScoreRecord->wAndroidCount);

	//˰�����
	m_TreasureDB->AddParameter(TEXT("@lWasteCount"),pGameScoreRecord->lWasteCount);
	m_TreasureDB->AddParameter(TEXT("@lRevenueCount"),pGameScoreRecord->lRevenueCount);

	//ͳ����Ϣ
	m_TreasureDB->AddParameter(TEXT("@dwUserMemal"),pGameScoreRecord->dwUserMemal);
	m_TreasureDB->AddParameter(TEXT("@dwPlayTimeCount"),pGameScoreRecord->dwPlayTimeCount);

	//ʱ����Ϣ
	m_TreasureDB->AddParameter(TEXT("@SystemTimeStart"),pGameScoreRecord->SystemTimeStart);
	m_TreasureDB->AddParameter(TEXT("@SystemTimeConclude"),pGameScoreRecord->SystemTimeConclude);

	//��ӿ�����yang
	m_TreasureDB->AddParameter(TEXT("@ChangeStockScore"),pGameScoreRecord->wChangeStockScore);

	//ִ�в�ѯ
	LONG lResultCode=m_TreasureDB->ExecuteProcess(TEXT("GSP_GR_RecordDrawInfo"),true);

	//д���¼
	if (lResultCode==DB_SUCCESS)
	{
		//��ȡ��ʶ
		DWORD dwDrawID=m_TreasureDB->GetValue_DWORD(TEXT("DrawID"));

		//д���¼
		for (WORD i=0;i<pGameScoreRecord->wRecordCount;i++)
		{
			//���ò���
			m_TreasureDB->ResetParameter();

			//������Ϣ
			m_TreasureDB->AddParameter(TEXT("@dwDrawID"),dwDrawID);
			m_TreasureDB->AddParameter(TEXT("@dwUserID"),pGameScoreRecord->GameScoreRecord[i].dwUserID);
			m_TreasureDB->AddParameter(TEXT("@wChairID"),pGameScoreRecord->GameScoreRecord[i].wChairID);

			//�ɼ���Ϣ
			m_TreasureDB->AddParameter(TEXT("@lScore"),pGameScoreRecord->GameScoreRecord[i].lScore);
			m_TreasureDB->AddParameter(TEXT("@lGrade"),pGameScoreRecord->GameScoreRecord[i].lGrade);
			m_TreasureDB->AddParameter(TEXT("@lRevenue"),pGameScoreRecord->GameScoreRecord[i].lRevenue);
			m_TreasureDB->AddParameter(TEXT("@lControlScore"),pGameScoreRecord->GameScoreRecord[i].lControlScore);
			m_TreasureDB->AddParameter(TEXT("@lChoushui"),pGameScoreRecord->GameScoreRecord[i].lChoushui);
			m_TreasureDB->AddParameter(TEXT("@dwChoushuiType"),pGameScoreRecord->GameScoreRecord[i].dwChoushuiType);
			m_TreasureDB->AddParameter(TEXT("@dwUserMedal"),pGameScoreRecord->GameScoreRecord[i].dwUserMemal);
			m_TreasureDB->AddParameter(TEXT("@dwPlayTimeCount"),pGameScoreRecord->GameScoreRecord[i].dwPlayTimeCount);

			//ִ�в�ѯ
			m_TreasureDB->ExecuteProcess(TEXT("GSP_GR_RecordDrawScore"),true);
		}
	}

	return true;
}


//���ػ���
bool CDataBaseEngineSink::OnRequestLoadAndroidUser(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//��������
	DBO_GR_GameAndroidInfo GameAndroidInfo;
	ZeroMemory(&GameAndroidInfo,sizeof(GameAndroidInfo));

	//�û��ʻ�
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@wKindID"),g_GameCtrl->GetKindID());
	m_TreasureDB->AddParameter(TEXT("@wServerID"),g_GameCtrl->GetServerID());

	//ִ�в�ѯ
	GameAndroidInfo.lResultCode=m_TreasureDB->ExecuteProcess(TEXT("GSP_GR_LoadAndroidUser"),true);


	//��ȡ��Ϣ
	for (WORD i=0;i<CountArray(GameAndroidInfo.AndroidParameter);i++)
	{
		//�����ж�
		if (m_TreasureDB->IsRecordsetEnd()==true) break;

		//���Ч��
		ASSERT(GameAndroidInfo.wAndroidCount<CountArray(GameAndroidInfo.AndroidParameter));
		if (GameAndroidInfo.wAndroidCount>=CountArray(GameAndroidInfo.AndroidParameter)) break;

		//��ȡ����
		GameAndroidInfo.wAndroidCount++;
		GameAndroidInfo.AndroidParameter[i].dwUserID=m_TreasureDB->GetValue_DWORD(TEXT("UserID"));
		GameAndroidInfo.AndroidParameter[i].dwServiceTime=m_TreasureDB->GetValue_DWORD(TEXT("ServiceTime"));
		GameAndroidInfo.AndroidParameter[i].dwMinPlayDraw=m_TreasureDB->GetValue_DWORD(TEXT("MinPlayDraw"));
		GameAndroidInfo.AndroidParameter[i].dwMaxPlayDraw=m_TreasureDB->GetValue_DWORD(TEXT("MaxPlayDraw"));
		GameAndroidInfo.AndroidParameter[i].dwMinReposeTime=m_TreasureDB->GetValue_DWORD(TEXT("MinReposeTime"));
		GameAndroidInfo.AndroidParameter[i].dwMaxReposeTime=m_TreasureDB->GetValue_DWORD(TEXT("MaxReposeTime"));
		GameAndroidInfo.AndroidParameter[i].dwServiceGender=m_TreasureDB->GetValue_DWORD(TEXT("ServiceGender"));
		GameAndroidInfo.AndroidParameter[i].lMinTakeScore=m_TreasureDB->GetValue_LONGLONG(TEXT("MinTakeScore"));
		GameAndroidInfo.AndroidParameter[i].lMaxTakeScore=m_TreasureDB->GetValue_LONGLONG(TEXT("MaxTakeScore"));

		//�ƶ���¼
		m_TreasureDB->MoveToNext();
	}

	//������Ϣ
	WORD wHeadSize=sizeof(GameAndroidInfo)-sizeof(GameAndroidInfo.AndroidParameter);
	WORD wDataSize_=GameAndroidInfo.wAndroidCount*sizeof(GameAndroidInfo.AndroidParameter[0]);
	//g_AttemperEngineSink->OnEventDataBaseResult(DBR_GR_GAME_ANDROID_INFO,dwContextID,&GameAndroidInfo,wHeadSize+wDataSize_);

	return true;

}

//���ض�������
bool CDataBaseEngineSink::On_DBR_GR_LOAD_OFFLINE(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//����У��
	if(wDataSize != sizeof(STR_DBR_GR_LOAD_OFFLINE)) return false;
	STR_DBR_GR_LOAD_OFFLINE *pDbr = (STR_DBR_GR_LOAD_OFFLINE*) pData;

	//�û��ʻ�
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@mystery"),pDbr->byMystery);
	m_TreasureDB->AddParameter(TEXT("@dwGameID"),pDbr->dwGameID);

	//ִ�в�ѯ
	long lResultCode=m_TreasureDB->ExecuteProcess(TEXT("GSP_GS_GAMEPROGRESS_INFO"),true);

	if(DB_SUCCESS == lResultCode)
	{
		BYTE GameProgress = m_TreasureDB->GetValue_BYTE(TEXT("GameProgress"));
		g_AttemperEngineSink->OnEventDataBaseResult(DBO_GR_LOAD_OFFLINE,dwContextID,&GameProgress,sizeof(GameProgress));
	}


	return true;
}

//��������
bool CDataBaseEngineSink::OnRequestMatchFee(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_MatchFee));
	if (wDataSize!=sizeof(DBR_GR_MatchFee)) return false;

	//��������
	DBR_GR_MatchFee * pMatchFee=(DBR_GR_MatchFee *)pData;
	dwUserID=INVALID_DWORD;

	//������
	//ת����ַ
	TCHAR szClientAddr[16]=TEXT("");
	BYTE * pClientAddr=(BYTE *)&pMatchFee->dwClientAddr;
	_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

	//�������
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@dwUserID"),pMatchFee->dwUserID);
	m_TreasureDB->AddParameter(TEXT("@dwMatchFee"),pMatchFee->dwMatchFee);
	//TODONOW 
	m_TreasureDB->AddParameter(TEXT("@wKindID"),g_GameCtrl->GetServerID());
	m_TreasureDB->AddParameter(TEXT("@wServerID"),g_GameCtrl->GetServerID());
	m_TreasureDB->AddParameter(TEXT("@strClientIP"),szClientAddr);
	m_TreasureDB->AddParameter(TEXT("@dwMatchID"),pMatchFee->dwMatchID);
	m_TreasureDB->AddParameter(TEXT("@dwMatchNO"),pMatchFee->dwMatchNO);
	m_TreasureDB->AddParameter(TEXT("@strMachineID"),pMatchFee->szMachineID);

	//�������
	TCHAR szDescribeString[128]=TEXT("");
	m_TreasureDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	//�������
	LONG lReturnValue=m_TreasureDB->ExecuteProcess(TEXT("GSP_GR_UserMatchFee"),true);

	//���ͽ��
	g_AttemperEngineSink->OnEventDataBaseResult(DBO_GR_MATCH_FEE_RESULT,dwContextID,&lReturnValue,sizeof(lReturnValue));

	return true;
}

//������ʼ
bool CDataBaseEngineSink::OnRequestMatchStart(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_MatchStart));
	if (wDataSize!=sizeof(DBR_GR_MatchStart)) return false;

	//��������
	DBR_GR_MatchStart * pMatchStart=(DBR_GR_MatchStart *)pData;
	dwUserID=INVALID_DWORD;

	//������
	//�������
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@wMatchID"),pMatchStart->dwMatchID);
	m_TreasureDB->AddParameter(TEXT("@wMatchNo"),pMatchStart->dwMatchNo);
	m_TreasureDB->AddParameter(TEXT("@wMatchCount"),pMatchStart->wMatchCount);	
	m_TreasureDB->AddParameter(TEXT("@lInitScore"),pMatchStart->lInitScore);	

	//�������
	LONG lReturnValue=m_TreasureDB->ExecuteProcess(TEXT("GSP_GR_MatchStart"),true);

	return true;
}

//��������
bool CDataBaseEngineSink::OnRequestMatchOver(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_MatchOver));
	if (wDataSize!=sizeof(DBR_GR_MatchOver)) return false;

	//��������
	DBR_GR_MatchOver * pMatchOver=(DBR_GR_MatchOver *)pData;
	dwUserID=INVALID_DWORD;

	//������

	//�������
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@wMatchID"),pMatchOver->dwMatchID);
	m_TreasureDB->AddParameter(TEXT("@wMatchNo"),pMatchOver->dwMatchNo);
	m_TreasureDB->AddParameter(TEXT("@wMatchCount"),pMatchOver->wMatchCount);		

	//�������
	LONG lReturnValue=m_TreasureDB->ExecuteProcess(TEXT("GSP_GR_MatchOver"),true);

	if(lReturnValue==DB_SUCCESS)
	{
		LONG lCount=0;
		if(m_TreasureDB !=NULL)
			lCount=m_TreasureDB->GetRecordCount();
		DBO_GR_MatchRank *pMatchRank=new DBO_GR_MatchRank[lCount];
		for(LONG i=0; i<lCount; i++)
		{
			pMatchRank[i].cbRank=(BYTE)i;
			m_TreasureDB->GetValue_String(TEXT("NickName"),pMatchRank[i].szNickName,CountArray(pMatchRank[i].szNickName));
			pMatchRank[i].lMatchScore=m_TreasureDB->GetValue_LONG(TEXT("HScore"));
			m_TreasureDB->MoveToNext();
			if(m_TreasureDB->IsRecordsetEnd()) break;
		}
		g_AttemperEngineSink->OnEventDataBaseResult(DBO_GR_MATCH_RANK,dwContextID,pMatchRank,(WORD)(sizeof(DBO_GR_MatchRank)*lCount));
		if(pMatchRank!=NULL)
			SafeDeleteArray(pMatchRank);
	}


	return true;
}

//��������
bool CDataBaseEngineSink::OnRequestMatchReward(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_MatchReward));
	if (wDataSize!=sizeof(DBR_GR_MatchReward)) return false;

	//��������
	DBR_GR_MatchReward * pMatchReward=(DBR_GR_MatchReward *)pData;
	dwUserID=INVALID_DWORD;

	//������
	//ת����ַ
	TCHAR szClientAddr[16]=TEXT("");
	BYTE * pClientAddr=(BYTE *)&pMatchReward->dwClientAddr;
	_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

	//�������
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@dwUserID"),pMatchReward->dwUserID);
	m_TreasureDB->AddParameter(TEXT("@dwMatchID"),pMatchReward->dwMatchID);
	m_TreasureDB->AddParameter(TEXT("@dwMatchNO"),pMatchReward->dwMatchNO);
	m_TreasureDB->AddParameter(TEXT("@wRank"),pMatchReward->wRank);
	m_TreasureDB->AddParameter(TEXT("@lMatchScore"),pMatchReward->lMatchScore);
	m_TreasureDB->AddParameter(TEXT("@dwExperience"),pMatchReward->dwExperience);
	m_TreasureDB->AddParameter(TEXT("@dwGold"),pMatchReward->dwGold);
	m_TreasureDB->AddParameter(TEXT("@dwMedal"),pMatchReward->dwMedal);
	//TODONWO 
	m_TreasureDB->AddParameter(TEXT("@wKindID"),g_GameCtrl->GetServerID());
	m_TreasureDB->AddParameter(TEXT("@wServerID"),g_GameCtrl->GetServerID());
	m_TreasureDB->AddParameter(TEXT("@strClientIP"),szClientAddr);

	//�������
	LONG lReturnValue=m_TreasureDB->ExecuteProcess(TEXT("GSP_GR_MatchReward"),true);

	return true;
}

//�˳�����
bool CDataBaseEngineSink::OnRequestMatchQuit(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_MatchFee));
	if (wDataSize!=sizeof(DBR_GR_MatchFee)) return false;

	//��������
	DBR_GR_MatchFee * pMatchFee=(DBR_GR_MatchFee *)pData;
	dwUserID=INVALID_DWORD;

	//������

	//ת����ַ
	TCHAR szClientAddr[16]=TEXT("");
	BYTE * pClientAddr=(BYTE *)&pMatchFee->dwClientAddr;
	_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

	//�������
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@dwUserID"),pMatchFee->dwUserID);
	m_TreasureDB->AddParameter(TEXT("@dwMatchFee"),pMatchFee->dwMatchFee);
	//TOODNOW 
	m_TreasureDB->AddParameter(TEXT("@wKindID"),g_GameCtrl->GetServerID());
	m_TreasureDB->AddParameter(TEXT("@wServerID"),g_GameCtrl->GetServerID());
	m_TreasureDB->AddParameter(TEXT("@strClientIP"),szClientAddr);
	m_TreasureDB->AddParameter(TEXT("@dwMatchID"),pMatchFee->dwMatchID);
	m_TreasureDB->AddParameter(TEXT("@dwMatchNO"),pMatchFee->dwMatchNO);
	m_TreasureDB->AddParameter(TEXT("@strMachineID"),pMatchFee->szMachineID);

	//�������
	TCHAR szDescribeString[128]=TEXT("");
	m_TreasureDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	//�������
	LONG lReturnValue=m_TreasureDB->ExecuteProcess(TEXT("GSP_GR_UserMatchQuit"),true);

	//���ͽ��
	g_AttemperEngineSink->OnEventDataBaseResult(DBO_GR_MATCH_QUIT_RESULT,dwContextID,&lReturnValue,sizeof(lReturnValue));

	return true;

}

//д��ǰ���
bool CDataBaseEngineSink::OnWriteCurrentStock( DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID )
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_WriteCurrentStock));
	if (wDataSize!=sizeof(DBR_GR_WriteCurrentStock)) return false;

	DBR_GR_WriteCurrentStock * pWriteStock = (DBR_GR_WriteCurrentStock *)pData;
	//������Ϣ
	m_TreasureDB->ResetParameter();

	m_TreasureDB->AddParameter(TEXT("@wServerID"),g_GameCtrl->GetServerID());
	m_TreasureDB->AddParameter(TEXT("@ConfigName"),1);

	TCHAR  extend[50] = TEXT("");
	wsprintf( extend,TEXT("%d"), pWriteStock->wCurrentStock);
	m_TreasureDB->AddParameter(TEXT("@CurrentStockScore"),extend);

	//ִ�в�ѯ
	LONG lResultCode=m_TreasureDB->ExecuteProcess(TEXT("GSP_GR_WriteStockScore"),true);

	return true;
}

//�û�Ȩ��
bool CDataBaseEngineSink::OnRequestManageUserRight(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD &dwUserID)
{

	//Ч�����
	if (wDataSize!=sizeof(DBR_GR_ManageUserRight)) return false;

	//������
	DBR_GR_ManageUserRight * pManageUserRight=(DBR_GR_ManageUserRight *)pData;
	dwUserID=pManageUserRight->dwUserID;

	//�������
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@dwUserID"),pManageUserRight->dwUserID);
	m_TreasureDB->AddParameter(TEXT("@dwAddRight"),pManageUserRight->dwAddRight);
	m_TreasureDB->AddParameter(TEXT("@dwRemoveRight"),pManageUserRight->dwRemoveRight);

	//ִ�в�ѯ
	LONG lReturnValue=m_TreasureDB->ExecuteProcess(TEXT("GSP_GR_ManageUserRight"),false);

	return true;
}

//������ݱ�����yang
VOID CDataBaseEngineSink::OnClearDB()
{
	//�������
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("ServerID"),g_GameCtrl->GetServerID());

	//ִ�в�ѯ
	LONG lResultCode=m_TreasureDB->ExecuteProcess(TEXT("GSP_GR_ClearRoomLocker"),true);
}

#pragma region Club����Ȧ

// byte����תΪ string  TODONOW ��ʱ�������ﴦ��
const CString toHexString(const byte * input, const int datasize)
{
	CString output;
	char ch[3];

	for(int i = 0; i < datasize; ++i)
	{
		if( (input+i) != NULL)
		{
			sprintf_s(ch, 3, "%02x", input[i]);
			output += ch;
		}
	}
	return output;
} 

//club����Ȧ������Ϣ
bool CDataBaseEngineSink::On_DBR_CLUB_ROOM_INFO(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize!=sizeof(STR_DBR_CLUB_ROOM_INFO)) return false;
	STR_DBR_CLUB_ROOM_INFO* pTableInfo = (STR_DBR_CLUB_ROOM_INFO*)pData;

	//�������
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@dwUserID"),pTableInfo->dwUserID);
	m_TreasureDB->AddParameter(TEXT("@dwClubID"),pTableInfo->dwClubID);
	m_TreasureDB->AddParameter(TEXT("@dwGameID"),pTableInfo->dwGameID);
	m_TreasureDB->AddParameter(TEXT("@byModeID"),pTableInfo->byModeID);
	//m_TreasureDB->AddParameter(TEXT("@szKindName"),pTableInfo->dwLockState);
	m_TreasureDB->AddParameter(TEXT("@dwServiceGold"),pTableInfo->dwServiceGold);
	m_TreasureDB->AddParameter(TEXT("@dwRevenue"),pTableInfo->dwRevenue);
	m_TreasureDB->AddParameter(TEXT("@byPlayerNum"),0); //��������ʱ��Ĭ��Ϊ0
	m_TreasureDB->AddParameter(TEXT("@DissolveMinute"),pTableInfo->DissolveMinute);
	m_TreasureDB->AddParameter(TEXT("@byChairNum"),pTableInfo->byChairNum);
	m_TreasureDB->AddParameter(TEXT("@byAllRound"),pTableInfo->byAllRound);

	//�������˵�� 40��TCHAR�ַ�
	TCHAR szRoomRuleNote[40] = TEXT("");
	memcpy(szRoomRuleNote, (pTableInfo->szRealRoomRule) + 128, sizeof(TCHAR)*40);

	//�������� 16��TCHAR�ַ�
	TCHAR szRoomName[16] = TEXT("");
	memcpy(szRoomName, (pTableInfo->szRealRoomRule) + 128 + sizeof(TCHAR)*40, sizeof(TCHAR)*16);

	//16���Ƶķ������ 256���ֽ�
	CString strRealRoomRule = toHexString(pTableInfo->szRealRoomRule, 255); //�������һ���ַ�

	m_TreasureDB->AddParameter(TEXT("@szRoomRule"),szRoomRuleNote);
	m_TreasureDB->AddParameter(TEXT("@szRealRoomRule"), strRealRoomRule);

	m_TreasureDB->AddParameter(TEXT("@dwDizhu"),pTableInfo->dwDizhu);
	m_TreasureDB->AddParameter(TEXT("@dwGold"), pTableInfo->dwGold);
	m_TreasureDB->AddParameter(TEXT("@byMask"),pTableInfo->byMask);
	m_TreasureDB->AddParameter(TEXT("@RoomName"),szRoomName);

	//ִ�й���
	LONG lResultCode=m_TreasureDB->ExecuteProcess(TEXT("GSP_WRITE_CLUB_ROOM_INFO"),false);

	if (lResultCode != DB_SUCCESS)
	{
	}


	return true;
}

//club����Ȧ������Ϣ
bool CDataBaseEngineSink::On_DBR_CLUB_TABLE_INFO(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize!=sizeof(STR_DBR_CLUB_TABLE_INFO)) return false;
	STR_DBR_CLUB_TABLE_INFO* pTableInfo = (STR_DBR_CLUB_TABLE_INFO*)pData;

	//�������
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@dwUserID"),pTableInfo->dwUserID);
	m_TreasureDB->AddParameter(TEXT("@dwClubRoomID"),pTableInfo->dwClubRoomID);
	m_TreasureDB->AddParameter(TEXT("@dwTableID"),pTableInfo->dwTableID);
	m_TreasureDB->AddParameter(TEXT("@dwTableState"),pTableInfo->dwTableState);
	m_TreasureDB->AddParameter(TEXT("@dwLockState"),pTableInfo->dwLockState);
	m_TreasureDB->AddParameter(TEXT("@byCurrentRound"),pTableInfo->byCurrentRound);
	m_TreasureDB->AddParameter(TEXT("@byMask"),pTableInfo->byMask);
	m_TreasureDB->AddParameter(TEXT("@byCompanyID"),pTableInfo->byCompanyID);
	m_TreasureDB->AddParameter(TEXT("@dwGameID"),pTableInfo->dwGameID);

	//ִ�й���
	LONG lResultCode=m_TreasureDB->ExecuteProcess(TEXT("GSP_WRITE_CLUB_TABLE_INFO"),true);

	if (lResultCode != DB_SUCCESS)
	{
		return true;
	}

	STR_CMD_LC_CLUB_TABLE_LIST CMD;
	CMD.dwClubRoomID  = m_TreasureDB->GetValue_DWORD(TEXT("RoomID"));
	CMD.dwTableID = m_TreasureDB->GetValue_DWORD(TEXT("TableID"));
	CMD.dwClubID = m_TreasureDB->GetValue_DWORD(TEXT("ClubID"));
	CMD.byMask = m_TreasureDB->GetValue_BYTE(TEXT("IsOwner"));
	CMD.TableState = m_TreasureDB->GetValue_DWORD(TEXT("TableState"));

	DWORD dwResuleLockState =  m_TreasureDB->GetValue_DWORD(TEXT("LockState"));
	CMD.CurrentRound =  m_TreasureDB->GetValue_DWORD(TEXT("CurrentRound"));
	CMD.dwOwnerID =  m_TreasureDB->GetValue_DWORD(TEXT("OwnerID"));
	CMD.byDel = (pTableInfo->byMask == 3) ? 1 : 0;

	//���ͽ��
	g_AttemperEngineSink->OnEventDataBaseResult(DBO_GC_CLUB_UPDATE_TABLE_INFO,dwContextID,&CMD,sizeof(CMD));

	return true;
}

//club����Ȧ�����Ϣ
bool CDataBaseEngineSink::On_DBR_CLUB_PLAYER_INFO(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize!=sizeof(STR_DBR_CLUB_PLAYER_INFO)) return false;
	STR_DBR_CLUB_PLAYER_INFO* pTableInfo = (STR_DBR_CLUB_PLAYER_INFO*)pData;

	//�������
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@dwUserID"),pTableInfo->dwUserID);
	m_TreasureDB->AddParameter(TEXT("@dwTableID"),pTableInfo->dwTableID);
	m_TreasureDB->AddParameter(TEXT("@byChairID"),pTableInfo->byChairID);
	m_TreasureDB->AddParameter(TEXT("@byMask"),pTableInfo->byMask);

	//ִ�й���
	LONG lResultCode=m_TreasureDB->ExecuteProcess(TEXT("GSP_WRITE_CLUB_PLAYER_INFO"),true);

	if (lResultCode != DB_SUCCESS)
	{
		return true;
	}

	STR_CMD_LC_CLUB_TABLE_USER_LIST CMD;
	CMD.dwClubRoomID=m_TreasureDB->GetValue_DWORD(TEXT("ClubRoomID"));
	CMD.dwUserID=m_TreasureDB->GetValue_DWORD(TEXT("UserID"));
	m_TreasureDB->GetValue_String(TEXT("UserName"),CMD.szUserName,CountArray(CMD.szUserName));
	CMD.bySex=m_TreasureDB->GetValue_BYTE(TEXT("Sex"));
	CMD.wLevel=m_TreasureDB->GetValue_WORD(TEXT("UserLevel"));
	m_TreasureDB->GetValue_String(TEXT("HeadUrl"),CMD.szHeadUrl,CountArray(CMD.szHeadUrl));
	CMD.byClubRole=m_TreasureDB->GetValue_BYTE(TEXT("ClubRole"));
	CMD.byClubReputation=m_TreasureDB->GetValue_BYTE(TEXT("ClubReputation"));
	CMD.dwTableID=m_TreasureDB->GetValue_DWORD(TEXT("TableID"));
	CMD.byChairID=m_TreasureDB->GetValue_BYTE(TEXT("ChairID"));
	CMD.byDel = (pTableInfo->byMask == 1) ? 0 : 1;

	//���ͽ��
	g_AttemperEngineSink->OnEventDataBaseResult(DBO_GC_CLUB_UPDATE_PLAYER_INFO,dwContextID,&CMD,sizeof(CMD));

	return true;
}

//��Ҵ��� ������Ϣ
bool CDataBaseEngineSink::On_DBR_HALL_GOLD_TABLE_INFO(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize!=sizeof(STR_DBR_HALL_GOLD_TABLE_INFO)) return false;
	STR_DBR_HALL_GOLD_TABLE_INFO* pTableInfo = (STR_DBR_HALL_GOLD_TABLE_INFO*)pData;

	//�������
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@FirmID"),_MYSTERY);
	m_TreasureDB->AddParameter(TEXT("@dwUserID"),pTableInfo->dwUserID);
	m_TreasureDB->AddParameter(TEXT("@dwKindID"),pTableInfo->dwKindID);
	m_TreasureDB->AddParameter(TEXT("@byGameType"),pTableInfo->byGameType);
	m_TreasureDB->AddParameter(TEXT("@dwTableID"),pTableInfo->dwTableID);
	m_TreasureDB->AddParameter(TEXT("@dwTableState"),pTableInfo->dwTableState);
	m_TreasureDB->AddParameter(TEXT("@byMask"),pTableInfo->byMask);
	m_TreasureDB->AddParameter(TEXT("@dwGameID"),pTableInfo->dwGameID);

	//ִ�й���
	LONG lResultCode=m_TreasureDB->ExecuteProcess(TEXT("GSP_WRITE_HALL_GOLD_TABLE_INFO"),true);

	if (lResultCode != DB_SUCCESS)
	{
		return true;
	}


	return true;
}

//��Ҵ��� �����Ϣ
bool CDataBaseEngineSink::On_DBR_HALL_GOLD_PLAYER_INFO(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize!=sizeof(STR_DBR_HALL_GOLD_PLAYER_INFO)) return false;
	STR_DBR_HALL_GOLD_PLAYER_INFO* pTableInfo = (STR_DBR_HALL_GOLD_PLAYER_INFO*)pData;

	//�������
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@dwUserID"),pTableInfo->dwUserID);
	m_TreasureDB->AddParameter(TEXT("@dwTableID"),pTableInfo->dwTableID);
	m_TreasureDB->AddParameter(TEXT("@byChairID"),pTableInfo->byChairID);
	m_TreasureDB->AddParameter(TEXT("@byMask"),pTableInfo->byMask);

	//ִ�й���
	LONG lResultCode=m_TreasureDB->ExecuteProcess(TEXT("GSP_WRITE_HALL_GOLD_PLAYER_INFO"),true);

	if (lResultCode != DB_SUCCESS)
	{
		return true;
	}

	return true;
}

#pragma endregion
////////////////////////////////////////////////////////////////////////////////// 
//��ӿ�����Ϣ
bool CDataBaseEngineSink::OnRequestAddTableInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GR_UpdateTableInfo));
	if (wDataSize!=sizeof(DBR_GR_UpdateTableInfo)) return false;

	DBR_GR_UpdateTableInfo* pTableInfo = (DBR_GR_UpdateTableInfo*)pData;
	//�������
	//�������
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@wTableID"),pTableInfo->dwTableID);
	m_TreasureDB->AddParameter(TEXT("@dwPassword"),pTableInfo->dwPassword);
	m_TreasureDB->AddParameter(TEXT("@dwCreaterID"),pTableInfo->dwCreaterID);
	m_TreasureDB->AddParameter(TEXT("@wJuShu"),pTableInfo->wJuShu);
	m_TreasureDB->AddParameter(TEXT("@byMode"),pTableInfo->byMode);
	m_TreasureDB->AddParameter(TEXT("@byZhuangType"),pTableInfo->byZhuangType);
	m_TreasureDB->AddParameter(TEXT("@byPlayerCount"),pTableInfo->byPlayerCount);
	m_TreasureDB->AddParameter(TEXT("@byMaxPlayerCount"),pTableInfo->byMaxPlayerCount);
	m_TreasureDB->AddParameter(TEXT("@byIsStart"),pTableInfo->byIsStart);
	m_TreasureDB->AddParameter(TEXT("@byCost"),pTableInfo->byCost);
	m_TreasureDB->AddParameter(TEXT("@szCreateTime"),pTableInfo->szTime);

	//�������
	TCHAR szDescribeString[128]=TEXT("");
	m_TreasureDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	//ִ�й���
	LONG lResultCode=m_TreasureDB->ExecuteProcess(TEXT("GSP_GR_UpdateTableInfo"),true);
	//�������
	CDBVarValue DBVarValue;
	m_TreasureDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	if (lResultCode==DB_SUCCESS)
	{
	}
	else
	{
	}

	return true;
}

//��ʼ����
bool CDataBaseEngineSink::OnRequestStartTable(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DWORD));
	if (wDataSize!=sizeof(DWORD)) return false;

	DWORD* pTableID = (DWORD*)pData;
	//�������
	//�������
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@dwTableID"),*pTableID);

	//�������
	TCHAR szDescribeString[128]=TEXT("");
	m_TreasureDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	//ִ�й���
	LONG lResultCode=m_TreasureDB->ExecuteProcess(TEXT("GSP_GR_StartTable"),true);
	//�������
	CDBVarValue DBVarValue;
	m_TreasureDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	if (lResultCode==DB_SUCCESS)
	{

	}
	else
	{
	}

	return true;
}

//��������
bool CDataBaseEngineSink::OnRequestEndTable(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GP_EndTable));
	if (wDataSize!=sizeof(DBR_GP_EndTable)) return false;

	DBR_GP_EndTable* pEndTable = (DBR_GP_EndTable*)pData;
	//�������
	//�������
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@dwTableID"),pEndTable->dwTableID);
	m_TreasureDB->AddParameter(TEXT("@dwUserID1"),pEndTable->dwUserID[0]);
	m_TreasureDB->AddParameter(TEXT("@dwUserID2"),pEndTable->dwUserID[1]);
	m_TreasureDB->AddParameter(TEXT("@dwUserID3"),pEndTable->dwUserID[2]);
	m_TreasureDB->AddParameter(TEXT("@dwUserID4"),pEndTable->dwUserID[3]);
	m_TreasureDB->AddParameter(TEXT("@dwUserID5"),pEndTable->dwUserID[4]);
	m_TreasureDB->AddParameter(TEXT("@dwScore1"),pEndTable->dwScore[0]);
	m_TreasureDB->AddParameter(TEXT("@dwScore2"),pEndTable->dwScore[1]);
	m_TreasureDB->AddParameter(TEXT("@dwScore3"),pEndTable->dwScore[2]);
	m_TreasureDB->AddParameter(TEXT("@dwScore4"),pEndTable->dwScore[3]);
	m_TreasureDB->AddParameter(TEXT("@dwScore5"),pEndTable->dwScore[4]);

	//�������
	TCHAR szDescribeString[128]=TEXT("");
	m_TreasureDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	//ִ�й���
	LONG lResultCode=m_TreasureDB->ExecuteProcess(TEXT("GSP_GR_EndTable"),true);
	//�������
	CDBVarValue DBVarValue;
	m_TreasureDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	if (lResultCode==DB_SUCCESS)
	{

	}
	else
	{
	}

	return true;
}



//����û�������
bool CDataBaseEngineSink::OnRequestAddUserToTable(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GP_AddTableUser));
	if (wDataSize!=sizeof(DBR_GP_AddTableUser)) return false;

	DBR_GP_AddTableUser* pEndTable = (DBR_GP_AddTableUser*)pData;
	//�������
	//�������
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@dwTableID"),pEndTable->dwTableID);
	m_TreasureDB->AddParameter(TEXT("@dwUserID"),pEndTable->dwUserID);
	//�������
	TCHAR szDescribeString[128]=TEXT("");
	m_TreasureDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	//ִ�й���
	LONG lResultCode=m_TreasureDB->ExecuteProcess(TEXT("GSP_GR_AddTableUser"),true);
	//�������
	CDBVarValue DBVarValue;
	m_TreasureDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	if (lResultCode==DB_SUCCESS)
	{

	}
	else
	{
	}

	return true;
}

//ɾ�������û�
bool CDataBaseEngineSink::OnRequestDeleteUserToTable(DWORD dwContextID, VOID * pData, WORD wDataSize)
{

	//Ч�����
	ASSERT(wDataSize==sizeof(DBR_GP_DeleteTableUser));
	if (wDataSize!=sizeof(DBR_GP_DeleteTableUser)) return false;

	DBR_GP_DeleteTableUser* pEndTable = (DBR_GP_DeleteTableUser*)pData;
	//�������
	//�������
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@dwTableID"),pEndTable->dwTableID);
	m_TreasureDB->AddParameter(TEXT("@dwUserID"),pEndTable->dwUserID);
	//�������
	TCHAR szDescribeString[128]=TEXT("");
	m_TreasureDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	//ִ�й���
	LONG lResultCode=m_TreasureDB->ExecuteProcess(TEXT("GSP_GR_DeleteTableUser"),true);
	//�������
	CDBVarValue DBVarValue;
	m_TreasureDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	if (lResultCode==DB_SUCCESS)
	{
		//
	}
	else
	{
	}


	return true;
}


//���¿�����Ϣ
bool CDataBaseEngineSink::OnUpdateTableInfo(DWORD dwTableID)
{
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("dwTableID"),dwTableID);

	//ִ�в�ѯ
	LONG lResultCode = m_TreasureDB->ExecuteProcess(TEXT("GSP_GR_GetTableInfo"),true);

	//�������
	if (lResultCode==DB_SUCCESS)
	{

		//��������
		DBO_GP_TableInfo TableInfo;
		ZeroMemory(&TableInfo,sizeof(TableInfo));

		TableInfo.dwTableID = m_TreasureDB->GetValue_DWORD(TEXT("TableID"));
		TableInfo.dwCreaterID = m_TreasureDB->GetValue_DWORD(TEXT("CreaterID"));
		TableInfo.dwPassword = m_TreasureDB->GetValue_DWORD(TEXT("Password"));
		TableInfo.wJuShu = m_TreasureDB->GetValue_WORD(TEXT("JuShu"));
		TableInfo.byMode = m_TreasureDB->GetValue_BYTE(TEXT("GameMode"));
		TableInfo.byZhuangType = m_TreasureDB->GetValue_BYTE(TEXT("ZhuangType"));
		TableInfo.byPlayerCount = m_TreasureDB->GetValue_BYTE(TEXT("PlayerCount"));
		TableInfo.byMaxPlayerCount = m_TreasureDB->GetValue_BYTE(TEXT("MaxPlayerCount"));
		TableInfo.byCost = m_TreasureDB->GetValue_BYTE(TEXT("PayCost"));
		TableInfo.byIsStart = m_TreasureDB->GetValue_BYTE(TEXT("IsStart"));

		TableInfo.dwUserID[0] =  m_TreasureDB->GetValue_DWORD(TEXT("UserID1"));
		TableInfo.dwUserID[1] =  m_TreasureDB->GetValue_DWORD(TEXT("UserID2"));
		TableInfo.dwUserID[2] =  m_TreasureDB->GetValue_DWORD(TEXT("UserID3"));
		TableInfo.dwUserID[3] =  m_TreasureDB->GetValue_DWORD(TEXT("UserID4"));
		TableInfo.dwUserID[4] =  m_TreasureDB->GetValue_DWORD(TEXT("UserID5"));

		lstrcpyn( TableInfo.szNickName[0],TEXT("NickName1"),CountArray(TableInfo.szNickName[0]));
		lstrcpyn( TableInfo.szNickName[1],TEXT("100012"),CountArray(TableInfo.szNickName[1]));
		lstrcpyn( TableInfo.szNickName[2],TEXT(""),CountArray(TableInfo.szNickName[1]));
		lstrcpyn( TableInfo.szNickName[3],TEXT(""),CountArray(TableInfo.szNickName[2]));
		lstrcpyn( TableInfo.szNickName[4],TEXT(""),CountArray(TableInfo.szNickName[3]));

		m_TreasureDB->GetValue_String(TEXT("CreateTime"),TableInfo.szTime,CountArray(TableInfo.szTime));	

		//g_AttemperEngineSink->OnEventDataBaseResult(DBO_GP_TABLE_INFO_RESULT,dwContextID,&TableInfo,sizeof(DBO_GP_TableInfo));
	}

	return true;
}

//�����ѿ�����Ϣ
bool CDataBaseEngineSink::OnUpdateEndTableInfo(DWORD dwTableID)
{
	return true;
}



//����齱�������
bool CDataBaseEngineSink::OnQueryLottery(DWORD dwContextID, void * pData, WORD wDataSize)
{
	if (sizeof(DBR_GR_QueryLottery) < wDataSize)
	{
		return false;
	}

	DBR_GR_QueryLottery* pDbReq = (DBR_GR_QueryLottery*)pData;
	//BYTE type = pDbReq->byType;

	//��ȡһ��1-100�������
	srand((unsigned int)time(NULL)) ;
	int index = rand() % 100 + 1;


	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@UserID"), pDbReq->dwUserID);
	m_TreasureDB->AddParameter(TEXT("@Type"), pDbReq->byType);
	m_TreasureDB->AddParameter(TEXT("@Index"), index);
	//�������
	WCHAR szDescribe[128] = L"";
	m_TreasureDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

	//ִ�в�ѯ,ִ�г齱
	LONG lResultCode = m_TreasureDB->ExecuteProcess(TEXT("GSP_GP_ImplementLottery"), true);
	//�������
	CDBVarValue DBVarValue;
	m_TreasureDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	if(lResultCode == DB_SUCCESS)
	{
		DBO_GR_LotteryResult LotteryResult;
		ZeroMemory(&LotteryResult,sizeof(DBO_GR_LotteryResult));

		LotteryResult.byIndex = -1;
		LotteryResult.byType = m_TreasureDB->GetValue_INT(TEXT("RewardType"));
		LotteryResult.dwRewardCount = m_TreasureDB->GetValue_DWORD(TEXT("RewardCount"));
		m_TreasureDB->GetValue_String(TEXT("PacketID"),LotteryResult.szPacketID,CountArray(LotteryResult.szPacketID));
		lstrcpyn(LotteryResult.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(LotteryResult.szDescribe));

		//֪ͨ�齱���
		g_AttemperEngineSink->OnEventDataBaseResult(DBO_GR_LOTTERY_RESULT,dwContextID,&LotteryResult,sizeof(DBO_GR_LotteryResult));

		//if(LotteryResult.byType != 0)
		//{
		//	//��ȡ�����û���ҷ�����ʯ
		//	DBO_GR_ScoreInfo UserScoreInfo;
		//	ZeroMemory(&UserScoreInfo,sizeof(DBO_GP_ScoreInfo));

		//	UserScoreInfo.dwUserID = pDbReq->dwUserID;
		//	UserScoreInfo.lGold = m_TreasureDB->GetValue_LONG(TEXT("Gold"));
		//	UserScoreInfo.lOpenRoomCard = m_TreasureDB->GetValue_LONG(TEXT("OpenRoomCard"));
		//	UserScoreInfo.lDiamond = m_TreasureDB->GetValue_LONG(TEXT("Diamond"));
		//	UserScoreInfo.lRewardCard = m_TreasureDB->GetValue_LONG(TEXT("RewardCard"));
		//	UserScoreInfo.lScore = m_TreasureDB->GetValue_LONG(TEXT("Score"));

		//	//���ͲƸ������Ϣ
		//	g_AttemperEngineSink->OnEventDataBaseResult(DBO_QUERY_SCOREINFO,dwContextID,&UserScoreInfo,sizeof(DBO_GP_ScoreInfo));
		//}
	}else
	{
		//û�н�Ʒ���鵽��
		DBO_GR_LotteryResult LotteryResult;
		ZeroMemory(&LotteryResult,sizeof(DBO_GR_LotteryResult));

		LotteryResult.byIndex = -1;
		LotteryResult.byType = 0;
		LotteryResult.dwRewardCount = 0;

		lstrcpyn(LotteryResult.szPacketID,TEXT(""),CountArray(LotteryResult.szPacketID));
		lstrcpyn(LotteryResult.szDescribe,TEXT("��Ʒ�Ͳ�һ��㣬���ٽ������ɣ�"),CountArray(LotteryResult.szDescribe));

		//֪ͨ�齱���
		g_AttemperEngineSink->OnEventDataBaseResult(DBO_GR_LOTTERY_RESULT,dwContextID,&LotteryResult,sizeof(DBO_GR_LotteryResult));
	}

	return true;
}

//������Ϸ����״̬
bool CDataBaseEngineSink::On_Table_UpdateGameTaskStatus(DWORD dwContextID, void *pData, WORD wDataSize)
{
	//���ݰ�У��
	if (sizeof(STR_DBR_CG_TABLE_UPDATE_TASK_STATUS) != wDataSize)
		return false;

	STR_DBR_CG_TABLE_UPDATE_TASK_STATUS* pDbReq = (STR_DBR_CG_TABLE_UPDATE_TASK_STATUS*)pData;

	//���ݿ⴫�����
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@dwUserID"), pDbReq->dwUserID);
	m_TreasureDB->AddParameter(TEXT("@cbCurGameCount"), pDbReq->cbCurGameCount);
	m_TreasureDB->AddParameter(TEXT("@cbTableMode"), pDbReq->cbTableMode);

	//�������
	WCHAR szDescribe[128] = L"";
	m_TreasureDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

	//ִ�в�ѯ,ִ�г齱
	LONG lResultCode = m_TreasureDB->ExecuteProcess(TEXT("GSP_SG_Table_UpdateGameTaskStatus"), true);

	//ֻ��Ҫ���£�����Ҫ���أ���������б�ʱ�����»������״̬

	return true;
}


// string תΪbyte����  TODONOW ��ʱ�������ﴦ��
int StrToBin(TCHAR* inWord, BYTE* OutBin, int source_len_begin, int source_len_end)
{
	TCHAR t2;
	int count = 0;

	for(int t = source_len_begin ;t < source_len_end; t ++)
	{   
		t2 = inWord[t];

		BYTE intTemp = 0;
		if(t2 == '0')
		{
			intTemp=0;
		}
		else if(t2 == '1')
		{
			intTemp=1;
		}
		else if(t2 == '2')
		{
			intTemp=2;
		}
		else if(t2 == '3')
		{
			intTemp=3;
		}
		else if(t2 == '4')
		{
			intTemp=4;
		}
		else if(t2 == '5')
		{
			intTemp=5;
		}
		else if(t2 == '6')
		{
			intTemp=6;
		}
		else if(t2 == '7')
		{
			intTemp=7;
		}
		else if(t2 == '8')
		{
			intTemp=8;
		}
		else if(t2 == '9')
		{
			intTemp=9;
		}
		else if(t2 == 'a' || t2 == 'A')
		{
			intTemp=10;
		}
		else if(t2 == 'b' || t2 == 'B')
		{
			intTemp=11;
		}
		else if(t2 == 'c' || t2 == 'C')
		{
			intTemp=12;
		}
		else if(t2 == 'd' || t2 == 'D')
		{
			intTemp=13;
		}
		else if(t2 == 'e' || t2 == 'E')
		{
			intTemp=14;
		}
		else if(t2 == 'f' || t2 =='F')
		{
			intTemp=15;
		}


		count = (t-source_len_begin)/2;

		if((t % 2) == 0) //��λ
		{
			OutBin[count] = intTemp<<4;
		}
		else if((t % 2) == 1) //��λ
		{
			OutBin[count] += intTemp;
		}
	}
	return count;
}

// add lihao 
int BinToStr(BYTE *inBin, TCHAR *OutWord, int begin, int end)
{
	int t = 0;

	for (int i = begin; i < end; i++)
	{
		int t2 = inBin[i] >> 4;
		int t3 = t2;

		for (int j = 0; j < 2; j++)
		{
			if (t2 == 0)
			{
				OutWord[t++] = '0';
			}
			else if (t2 == 1)
			{
				OutWord[t++] = '1';
			}
			else if (t2 == 2)
			{
				OutWord[t++] = '2';
			}
			else if (t2 == 3)
			{
				OutWord[t++] = '3';
			}
			else if (t2 == 4)
			{
				OutWord[t++] = '4';
			}
			else if (t2 == 5)
			{
				OutWord[t++] = '5';
			}
			else if (t2 == 6)
			{
				OutWord[t++] = '6';
			}
			else if (t2 == 7)
			{
				OutWord[t++] = '7';
			}
			else if (t2 == 8)
			{
				OutWord[t++] = '8';
			}
			else if (t2 == 9)
			{
				OutWord[t++] = '9';
			}
			else if (t2 == 10)
			{
				OutWord[t++] = 'a';
			}
			else if (t2 == 11)
			{
				OutWord[t++] = 'b';
			}
			else if (t2 == 12)
			{
				OutWord[t++] = 'c';
			}
			else if (t2 == 13)
			{
				OutWord[t++] = 'd';
			}
			else if (t2 == 14)
			{
				OutWord[t++] = 'e';
			}
			else if (t2 == 15)
			{
				OutWord[t++] = 'f';
			}
			t2 = inBin[i] - (t3 << 4);
		}
	}

	return 0;
}


//����Ȧ����������Ϣ
bool CDataBaseEngineSink::On_DBR_CG_CLUB_CREATE_TABLE(DWORD dwContextID, void *pData, WORD wDataSize)
{
	//���ݰ�У��
	if (sizeof(STR_SUB_CG_USER_CREATE_TABLE) != wDataSize)
		return false;

	STR_SUB_CG_USER_CREATE_TABLE* pDbReq = (STR_SUB_CG_USER_CREATE_TABLE*)pData;

	//���ݿ⴫�����
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@dwUserID"), pDbReq->dwUserID);
	m_TreasureDB->AddParameter(TEXT("@dwClubRoomID"), pDbReq->dwClubRoomID);

	//ִ�в�ѯ
	LONG lResultCode = m_TreasureDB->ExecuteProcess(TEXT("GSP_CL_CLUB_ROOM_INFO"), true);

	if(lResultCode != DB_SUCCESS)
	{
		return false ;
	}

	//��ȡ������
	STR_DBO_GC_CLUB_CREATE_TABLE Dbo;
	ZeroMemory(&Dbo,sizeof(STR_DBO_GC_CLUB_CREATE_TABLE));
	Dbo.byTableMode = pDbReq->byTableMode;
	Dbo.dwClubRoomID = pDbReq->dwClubRoomID;

	TCHAR szRealRoomRule[2048];
	m_TreasureDB->GetValue_String(TEXT("RealRoomRule"), szRealRoomRule, sizeof(szRealRoomRule));

	StrToBin(szRealRoomRule, Dbo.strCreateRoom.CommonRule, 0, 255);
	StrToBin(szRealRoomRule, Dbo.strCreateRoom.SubGameRule, 256, 512);

	STR_SUB_CG_USER_CREATE_ROOM CreateRoom = Dbo.strCreateRoom;
	tagTableRule *pCfg = (tagTableRule*)CreateRoom.CommonRule;

	BYTE byPlayNum = pCfg->PlayerCount;
	BYTE byGameCountType = pCfg->GameCountType;
	SCORE cost = byPlayNum * byGameCountType;

	//���ݿ⴫�����
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@cost"), cost);
	m_TreasureDB->AddParameter(TEXT("@dwClubRoomID"), pDbReq->dwClubRoomID);

	//ִ�в�ѯ
	lResultCode = m_TreasureDB->ExecuteProcess(TEXT("GSP_CL_CLUB_ROOM_INFO_COST"), false);
	Dbo.lResultCode = lResultCode;
	return g_AttemperEngineSink->OnEventDataBaseResult(DBO_GC_CLUB_CREATE_TABLE,dwContextID,&Dbo,sizeof(Dbo));

}

//�������� ����Ҫ����
bool CDataBaseEngineSink::On_DBR_CG_USER_JOIN_TABLE_NO_PASS(DWORD dwContextID, void *pData, WORD wDataSize)
{
	//���ݰ�У��
	if (sizeof(STR_SUB_CG_USER_JOIN_TABLE_NO_PASS) != wDataSize)
		return false;

	STR_SUB_CG_USER_JOIN_TABLE_NO_PASS* pDbReq = (STR_SUB_CG_USER_JOIN_TABLE_NO_PASS*)pData;

	//���ݿ⴫�����
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@dwUserID"), pDbReq->dwUserID);
	m_TreasureDB->AddParameter(TEXT("@dwClubRoomID"), pDbReq->dwClubRoomID);

	//ִ�в�ѯ
	LONG lResultCode = m_TreasureDB->ExecuteProcess(TEXT("GSP_CG_USER_JOIN_TABLE_NO_PASS"), true);

	STR_DBO_CG_USER_JOIN_TABLE_NO_PASS Dbo;
	ZeroMemory(&Dbo,sizeof(STR_DBO_CG_USER_JOIN_TABLE_NO_PASS));
	Dbo.lResultCode = lResultCode;
	if(lResultCode == DB_SUCCESS)
	{
		TCHAR szRealRoomRule[2048];
		m_TreasureDB->GetValue_String(TEXT("RealRoomRule"), szRealRoomRule, sizeof(szRealRoomRule));

		StrToBin(szRealRoomRule, Dbo.strCreateRoom.CommonRule, 0, 255);
		StrToBin(szRealRoomRule, Dbo.strCreateRoom.SubGameRule, 256, 512);

		Dbo.dLongitude = pDbReq->dLongitude;
		Dbo.dLatitude = pDbReq->dLatitude;
		Dbo.dwPassword = m_TreasureDB->GetValue_DWORD(TEXT("TableID"));
		Dbo.dwClubRoomID = pDbReq->dwClubRoomID;


		//����洢�������ж� �����Ƿ��㹻
		STR_SUB_CG_USER_CREATE_ROOM CreateRoom = Dbo.strCreateRoom;
		tagTableRule *pCfg = (tagTableRule*)CreateRoom.CommonRule;

		BYTE byPlayNum = pCfg->PlayerCount;
		BYTE byGameCountType = pCfg->GameCountType;
		SCORE cost = byPlayNum * byGameCountType;

		//���ݿ⴫�����
		m_TreasureDB->ResetParameter();
		m_TreasureDB->AddParameter(TEXT("@cost"), cost);
		m_TreasureDB->AddParameter(TEXT("@dwClubRoomID"), pDbReq->dwClubRoomID);

		//ִ�в�ѯ
		Dbo.lResultCode2 = m_TreasureDB->ExecuteProcess(TEXT("GSP_CL_CLUB_ROOM_INFO_COST"), false);
	}

	return g_AttemperEngineSink->OnEventDataBaseResult(DBO_GC_USER_JOIN_TABLE_NO_PASS,dwContextID,&Dbo,sizeof(Dbo));
}

//��������
bool CDataBaseEngineSink::On_DBR_CG_JOIN_TABLE(DWORD dwContextID, void *pData, WORD wDataSize)
{
	//���ݰ�У��
	if (sizeof(STR_DBR_JOIN_ROOM) != wDataSize)
		return false;

	STR_DBR_JOIN_ROOM* pDbReq = (STR_DBR_JOIN_ROOM*)pData;

	//���ݿ⴫�����
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@dwUserID"), pDbReq->dwUserID);
	m_TreasureDB->AddParameter(TEXT("@dwTableID"), pDbReq->dwTableID);

	//ִ�в�ѯ
	LONG lResultCode = m_TreasureDB->ExecuteProcess(TEXT("GSP_CG_USER_JOIN_TABLE"), false);
	pDbReq->lResultCode = lResultCode;

	return g_AttemperEngineSink->OnEventDataBaseResult(DBO_GC_JOIN_TABLE,dwContextID, pDbReq, wDataSize);

}

//�������� ��Ҵ�������
bool CDataBaseEngineSink::On_DBR_CG_USER_JOIN_TABLE_HALL_GOLD(DWORD dwContextID, void *pData, WORD wDataSize)
{
	//���ݰ�У��
	if (sizeof(STR_SUB_CG_USER_JOIN_GOLD_HALL_ROOM) != wDataSize)
		return false;

	//У���û�
	CPlayer *pIServerUserItem = CPlayerManager::FindPlayerBySocketID(dwContextID);
	if (NULL == pIServerUserItem) return true;

	STR_SUB_CG_USER_JOIN_GOLD_HALL_ROOM* pDbReq = (STR_SUB_CG_USER_JOIN_GOLD_HALL_ROOM*)pData;

	////��ʼ��һ�ν�ҳ��������
	//STR_SUB_CG_USER_CREATE_ROOM rule;
	//ZeroMemory(&rule, sizeof(STR_SUB_CG_USER_CREATE_ROOM));

	//tagTableRule *pTableRule = (tagTableRule *)rule.CommonRule;
	//pTableRule->GameMode = 2;
	//pTableRule->GameCount = 2;
	//pTableRule->PlayerCount = 3;
	//pTableRule->CellScore = 1;
	//pTableRule->FangZhu = 0;
	//pTableRule->dwLevelGold = 2000;   //1000 2000 4000 10000

	//TCHAR tmpRealRoomRule[520] = { 0 };
	//BinToStr(rule.CommonRule, tmpRealRoomRule, 0, 128);
	//BinToStr(rule.SubGameRule, &tmpRealRoomRule[256], 128, 256);

	//���ݿ⴫�����
	m_TreasureDB->ResetParameter();
	//m_TreasureDB->AddParameter(TEXT("@RoomRule"), tmpRealRoomRule);  //��һ�������
	m_TreasureDB->AddParameter(TEXT("@dwUserID"), pIServerUserItem->GetUserID());
	m_TreasureDB->AddParameter(TEXT("@byGameMod"), pDbReq->byGameMod);
	m_TreasureDB->AddParameter(TEXT("@byType"), pDbReq->byType);
	m_TreasureDB->AddParameter(TEXT("@dwKindID"), g_GameCtrl->GetKindID());
	m_TreasureDB->AddParameter(TEXT("@dwFirmID"), _MYSTERY);

	//ִ�в�ѯ
	LONG lResultCode = m_TreasureDB->ExecuteProcess(TEXT("GSP_CG_USER_JOIN_GOLD_HALL_ROOM"), true);

	STR_DBO_CG_USER_JOIN_TABLE_HALL_GOLD Dbo;
	ZeroMemory(&Dbo,sizeof(STR_DBO_CG_USER_JOIN_TABLE_HALL_GOLD));
	Dbo.lResultCode = lResultCode;
	Dbo.dwKindID = g_GameCtrl->GetKindID();
	Dbo.byGameType =  pDbReq->byType;

	if(lResultCode == DB_SUCCESS)
	{
		TCHAR szRealRoomRule[520] = {0};
		m_TreasureDB->GetValue_String(TEXT("RealRoomRule"), szRealRoomRule, sizeof(szRealRoomRule));

		StrToBin(szRealRoomRule, Dbo.strCreateRoom.CommonRule, 0, 255);
		StrToBin(szRealRoomRule, Dbo.strCreateRoom.SubGameRule, 256, 512);

		Dbo.dwMinGold = m_TreasureDB->GetValue_DWORD(TEXT("Gold"));

		//��ʹ�����ݿ������Ƽ��뷿������ //ֻ�����ݿ��ȡ����Ϸ����
		//Dbo.dwPassword = m_TreasureDB->GetValue_DWORD(TEXT("TableID"));
	}

	g_AttemperEngineSink->OnEventDataBaseResult(DBO_GC_USER_JOIN_TABLE_HALL_GOLD,dwContextID,&Dbo,sizeof(Dbo));

	return true;
}
