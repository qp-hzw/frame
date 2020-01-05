#include "Stdafx.h"
#include <vector>
#include <string>
#include <algorithm> 
#include "GameCtrl.h"
#include "DataBaseEngineSink.h"
#include "RankManager.h"

//�����¼�
bool CDataBaseEngineSink::OnDataBaseEngineStart(IUnknownEx * pIUnknownEx)
{
	m_AccountsDB = static_cast<IDataBase*>(CWHModule::Database());
	m_TreasureDB = static_cast<IDataBase*>(CWHModule::Database());
	m_PlatformDB = static_cast<IDataBase*>(CWHModule::Database());

	if(m_AccountsDB == NULL) return false;
	if(m_TreasureDB == NULL) return false;
	if(m_PlatformDB == NULL) return false;

	//��������
	m_PlatformDB->Connect(1);
	m_AccountsDB->Connect(2);
	m_TreasureDB->Connect(3);

	return true;
}

//ֹͣ�¼�
bool CDataBaseEngineSink::OnDataBaseEngineConclude(IUnknownEx * pIUnknownEx)
{
	return true;
}

//���ݿ������¼�
bool CDataBaseEngineSink::OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
	case DBR_CL_LOGON_ACCOUNTS:			//�ʺŵ�¼
		{
			return On_DBR_Logon_Accounts(dwContextID,pData,wDataSize);
		}
	case DBR_CL_LOGON_REGISTER:			//ע���ʺ�
		{
			return On_DBR_Logon_Register(dwContextID,pData,wDataSize);
		}
	case DBR_CL_LOGON_PLATFORM:			//ƽ̨��¼
		{
			return On_DBR_Logon_Platform(dwContextID,pData,wDataSize);
		}
	case DBR_CL_USER_QUERY_SCORE_INFO:		//��ѯ�����ʯ��Ϣ
		{
			return On_DBR_QueryScoreInfo(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SERVICE_USER_FEEDBACK:			//��ҷ���
		{
			return On_DBR_Service_UserFeedback(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SERVICE_REFRESH_USER_INFO:		//ˢ���û���Ϣ
		{
			return On_DBR_Service_RefreshUserInfo(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SERVICE_QUERY_ROOM_LIST:		//��ѯ������Ϣ�б�
		{
			return On_DBR_Service_QueryRoomList(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SERVICE_GET_RICH_LIST:			//��ȡ������
		{
			return On_DBR_Service_GetRichList(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SERVICE_GET_USER_RECORD_LIST:	//��ȡ�û�Ϸ¼���б�
		{
			return On_DBR_Service_GetUserRecordList(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SERVICE_GET_SPECIFIC_RECORD:	//��ȡָ��ID¼�����ݣ�С�֣�
		{
			return On_DBR_Service_GetSpecifiRecord(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SERVICE_ONLINE_REWARD:			//��ȡ���߽���
		{
			return On_DBR_Service_OnlineReward(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SERVICE_GET_TASK_LIST:			//��ȡ�����б�
		{
			return On_DBR_Service_GetTaskList(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SERVICE_GET_TASK_REWARD:		//��ȡ������
		{
			return On_DBR_Service_GetTaskReward(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SERVICE_REQUEST_LOTTERY:		//�齱����
		{
			return On_DBR_Service_RequestLottery(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SERVICE_PURE_STANDING_LIST:		//pure�������а�
		{
			return On_DBR_CL_SERVICE_PURE_STANDING_LIST(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SERVICE_PURE_RECORD_LIST:		//pure���ս��
		{
			return On_DBR_CL_SERVICE_PURE_RECORD_LIST(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SERVICE_PURE_XJ_RECORD_LIST:	//pureС��ս��
		{
			return On_DBR_CL_SERVICE_PURE_XJ_RECORD_LIST(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SERVICE_XJ_RECORD_PLAYBACK:		//С��¼��ط�
		{
			return On_DBR_CL_Service_XJRecordPlayback(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SERVICE_CUSTOMER_MESSEGE:		//�ͷ���Ϣ
		{
			return On_DBR_CL_SERVICE_CUSTOMER_MESSEGE(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SERVICE_GOLD_INFO:				//�����Ҵ�����Ϣ
		{
			return On_DBR_CL_SERVICE_GOLD_INFO(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SERVICE_MODIFY_PERSONAL_INFO:	//�޸ĸ�������
		{
			return On_DBR_Service_ModifyPersonalInfo(dwContextID,pData,wDataSize);
		}
	case DBR_CL_USER_RECHARGE_INFO:				//��ֵ��Ϣ
		{
			return On_DBR_Other_RechargeInfo(dwContextID,pData,wDataSize);
		}
	case DBR_CL_USER_EXCHANGE_INFO:				//�һ���Ϣ
		{
			return On_DBR_Other_ExchangeInfo(dwContextID,pData,wDataSize);
		}
#pragma region MDM_GIFT �������
	case DBR_CL_GIFT_GIVE_PROPS:			//���͵���
		{
			return On_DBR_CL_GIFT_GIVE_PROPS(dwContextID,pData,wDataSize);
		}
#pragma endregion
	case DBR_GP_UPDATE_RANK_VALUE:			//�������а��û�����
		{
			return OnDBUpdateRankUserItem(dwContextID,pData,wDataSize);
		}
	case DBR_GP_READ_RANK_LIST:				//��ȡ���а��б�
		{
			return OnDBReadRankList(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SERVICE_GET_RANK_REWARD:	//��ȡ���а���
		{
			return OnReceiveRankReward(dwContextID,pData,wDataSize);
		}
	case DBR_GP_MODIFY_USER_INSURE:			//�޸��û��Ƹ���Ϣ
		{
			return OnModifyUserInsure(dwContextID,pData,wDataSize);
		}
#pragma region MDM_CLUB ����Ȧ(�汾2)
	case DBR_CL_CLUB_ALL_CLUB_INFO_LIST:  //��ѯ����Ȧ�б�
		{
			return On_DBR_CL_CLUB_ALL_CLUB_INFO_LIST(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_ROOM_LIST: //��ѯָ������Ȧ�����б�
		{
			return On_DBR_CL_CLUB_ROOM_LIST(dwContextID,pData,wDataSize); 
		}
	case DBR_CL_CLUB_RANDOM_CLUB_LIST:  //��ѯδ��Ա, �������Ȧ(���9��)
		{
			return On_DBR_CL_CLUB_RANDOM_CLUB_LIST(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_JOIN_CLUB: //�����������Ȧ
		{
			return On_DBR_CL_CLUB_JOIN_CLUB(dwContextID,pData,wDataSize);	 
		}
	case DBR_CL_CLUB_DISS_CLUB: //��ɢ����Ȧ
		{
			return On_DBR_CL_CLUB_DISS_CLUB(dwContextID,pData,wDataSize);	
		}
	case DBR_CL_CLUB_ROOM_SETTING: //��������
		{
			return On_DBR_CL_CLUB_ROOM_SETTING(dwContextID,pData,wDataSize);	
		}
	case DBR_CL_CLUB_ROOM_QUERY_SETTING: //���󷿼�����
		{
			return On_DBR_CL_CLUB_ROOM_QUERY_SETTING(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_ROOM_DISSOLVE: //��ɢ��������
		{
			return On_DBR_CL_CLUB_ROOM_DISSOLVE(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_TABLE_DISSOLVE: //��ɢ��������
		{
			return On_DBR_CL_CLUB_TABLE_DISSOLVE(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_ROOM_USER_LEAVE://����뿪���ֲ�����
		{
			return On_DBR_CL_CLUB_ROOM_USER_LEAVE(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_NOTICE: //����Ȧ����
		{
			return On_DBR_CL_CLUB_NOTICE(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_MESSAGE: //����Ȧ���
		{
			return On_DBR_CL_CLUB_MESSAGE(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_CONTRIBUTE_FK://���׷���
		{
			return On_DBR_CL_CLUB_CONTRIBUTE_FK(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_AUTO_AGREE://����Ȧ����
		{
			return On_DBR_CL_CLUB_AUTO_AGREE(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_INVITE: //�������˼�������Ȧ
		{
			return On_DBR_CL_CLUB_INVITE(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_INVITE_RESULT: //������ �ظ�
		{
			return On_DBR_CL_CLUB_INVITE_RESULT(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_INQUERY_LIST://�������˲鿴�Լ��������б�
		{
			return On_DBR_CL_CLUB_INQUERY_LIST(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_APPLICANT_LIST://�������б�
		{
			return On_DBR_CL_CLUB_APPLICANT_LIST(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_APPOINTMENT://ְ������
		{
			return On_DBR_CL_CLUB_APPOINTMENT(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_CHAT://����Ȧ����
		{
			return On_DBR_CL_CLUB_CHAT(dwContextID,pData,wDataSize);
		}
	case DBR_CL_WORD_CHAT: //��������
		{
			return On_DBR_CL_WORD_CHAT(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SYSTEM_CHAT: //ϵͳ����
		{
			return On_DBR_CL_SYSTEM_CHAT(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SECRET_CHAT: //˽������
		{
			return On_DBR_CL_SECRET_CHAT(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_STICKY_POST:	//����Ȧ�ö�
		{
			return On_DBR_CL_STICKY_POST(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_RECORD_LIST: //����ս��ͳ��
		{
			return On_DBR_CL_CLUB_RECORD_LIST(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_CREATE_CLUB:  //��������Ȧ
		{
			return On_DBR_CL_CLUB_CREATE_CLUB(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_JOIN_ROOM:  //������뷿��
		{
			return On_DBR_CL_CLUB_JOIN_ROOM(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_APPLICANT_RESULT:  //Ⱥ��|�����������Ϣ�Ĵ�(ͬ��|�ܾ�)
		{
			return On_DBR_CL_CLUB_APPLICANT_RESULT(dwContextID,pData,wDataSize);
		}
	case DBR_CL_CLUB_MEMBER_MANAGER: //�����Ա����
		{
			return On_DBR_CL_CLUB_MEMBER_MANAGER(dwContextID,pData,wDataSize); 
		}
	case DBR_CL_CLUB_QUIT:  //�߳��˳�����
		{
			return On_DBR_CL_CLUB_QUIT(dwContextID,pData,wDataSize);
		}
#pragma endregion
#pragma region MDM_SHOP �̳ǵ���
	case DBR_CL_SHOP_QUERY:  //��ѯ�̳�
		{
			return On_DBR_CL_SHOP_QUERY(dwContextID,pData,wDataSize);
		}
	case DBR_CL_SHOP_MONEY: //��Ǯ�������
		{
			//return On_DBR_CL_SHOP_MONEY(dwContextID,pData,wDataSize); 
		}
	case DBR_CL_SHOP_DIAMOND:  //��ʯ�������
		{
			return On_DBR_CL_SHOP_DIAMOND(dwContextID,pData,wDataSize);
		}
	case DBR_CL_BAG_QUERY: //��Ʒ������ѯ
		{
			return On_DBR_CL_BAG_QUERY(dwContextID,pData,wDataSize);	 
		}
#pragma endregion
	}

	return false;
}

/* **************************��MAIN:3    MDM_SERVICE    ����*************************************/
//��ҷ���
bool CDataBaseEngineSink::On_DBR_Service_UserFeedback( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//Ч�����
	ASSERT(wDataSize==sizeof(STR_DBR_CL_SERVICE_FEEDBACK));
	if (wDataSize!=sizeof(STR_DBR_CL_SERVICE_FEEDBACK)) return false;

	//������
	STR_DBR_CL_SERVICE_FEEDBACK * pUserSuggestion = (STR_DBR_CL_SERVICE_FEEDBACK *)pData;

	//�������
	m_AccountsDB->ResetParameter();
	m_AccountsDB->AddParameter(TEXT("@iUserID"),pUserSuggestion->dwUserID);
	m_AccountsDB->AddParameter(TEXT("@chFB_Title"),pUserSuggestion->szFB_Title);
	m_AccountsDB->AddParameter(TEXT("@chFB_Content"),pUserSuggestion->szFB_Content);
	m_AccountsDB->AddParameter(TEXT("@chContact"),pUserSuggestion->szContact);

	//�������
	LONG lResultCode=m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_Service_Feedback"),true);		//��QPAccountsDB���ݿ��AccountsInfo���в����û�����

	//���ؽ������
	On_DBO_Service_UserFeedback(dwContextID, lResultCode, TEXT("��ҷ����������!"));

	return true;
}

//��ҷ�������
bool CDataBaseEngineSink::On_DBO_Service_UserFeedback(DWORD dwContextID, DWORD dwResultCode, LPCTSTR pszErrorString)
{
	//��������
	STR_DBO_CL_SERVICE_FEEDBACK OperateResult;
	ZeroMemory(&OperateResult,sizeof(OperateResult));

	//��������
	OperateResult.lResultCode = dwResultCode;
	lstrcpyn(OperateResult.szDescribeString,pszErrorString,CountArray(OperateResult.szDescribeString));

	//���ͽ��
	WORD wDataSize=CountStringBuffer(OperateResult.szDescribeString);
	WORD wHeadSize=sizeof(OperateResult)-sizeof(OperateResult.szDescribeString);
	g_AttemperEngineSink->OnEventDataBaseResult(DBO_CL_SERVICE_USER_FEEDBACK,dwContextID,&OperateResult,wHeadSize+wDataSize);

	return true;
}

//ˢ���û���Ϣ
bool CDataBaseEngineSink::On_DBR_Service_RefreshUserInfo( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//Ч�����
	ASSERT(wDataSize == sizeof(STR_DBR_CL_SERCIVR_REFRESH_INFO));
	if (wDataSize != sizeof(STR_DBR_CL_SERCIVR_REFRESH_INFO)) return false;

	//������
	STR_DBR_CL_SERCIVR_REFRESH_INFO * pUserRequest=(STR_DBR_CL_SERCIVR_REFRESH_INFO *)pData;

	//�������
	m_AccountsDB->ResetParameter();
	m_AccountsDB->AddParameter(TEXT("@dwUserID"),pUserRequest->dwUserID);

	WCHAR szDescribe[128]=L"";
	m_AccountsDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

	//�������
	LONG lResultCode = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_Service_RefreshUserInfo"),true);

	CDBVarValue DBVarValue;
	m_AccountsDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);


	//�������
	On_DBO_Service_RefreshUserInfo(dwContextID, lResultCode, CW2CT(DBVarValue.bstrVal));

	return true;
}

//ˢ���û���Ϣ����
bool CDataBaseEngineSink::On_DBO_Service_RefreshUserInfo(DWORD dwContextID, DWORD dwResultCode, LPCTSTR pszErrorString)
{
	//��������
	STR_DBO_CL_SERCIVR_REFRESH_INFO UserInfo;
	ZeroMemory( &UserInfo, sizeof(UserInfo));
	UserInfo.dwResultCode = dwResultCode;
	lstrcpyn(UserInfo.szDescribeString, pszErrorString, CountArray(UserInfo.szDescribeString));

	if (dwResultCode == DB_SUCCESS)		//�ɹ�
	{
		/* *****************************    �û���Ϣ     ****************************/
		//�û���־
		UserInfo.dwUserID=m_AccountsDB->GetValue_DWORD(TEXT("UserID"));
		//�û��ǳ�
		m_AccountsDB->GetValue_String(TEXT("NickName"),UserInfo.szNickName,CountArray(UserInfo.szNickName));
		//�û��Ա�
		UserInfo.cbGender=m_AccountsDB->GetValue_BYTE(TEXT("Gender"));
		//ͷ������
		UserInfo.wFaceID=m_AccountsDB->GetValue_WORD(TEXT("FaceID"));
		//����ǩ��
		m_AccountsDB->GetValue_String(TEXT("MySignature"),UserInfo.szMySignature,CountArray(UserInfo.szMySignature));

		//�û�Ԫ��
		UserInfo.dwUserDiamond=m_AccountsDB->GetValue_LONGLONG(TEXT("UserDiamond"));
		//�û�����
		UserInfo.dwUserMedal=m_AccountsDB->GetValue_DWORD(TEXT("UserMedal"));
		//������ֵ
		UserInfo.dwExperience=m_AccountsDB->GetValue_DWORD(TEXT("Experience"));
		//�û�����
		UserInfo.dwLoveLiness=m_AccountsDB->GetValue_DWORD(TEXT("LoveLiness"));

		//����Ա�ȼ�
		UserInfo.cbMasterOrder=m_AccountsDB->GetValue_BYTE(TEXT("MasterOrder"));
		//��Ա�ȼ�
		UserInfo.cbMemberOrder=m_AccountsDB->GetValue_BYTE(TEXT("MemberOrder"));
		//��Ա����ʱ
		m_AccountsDB->GetValue_SystemTime(TEXT("MemberOverDate"),UserInfo.MemberOverDate);

		//��ʵ����
		m_AccountsDB->GetValue_String(TEXT("IdentityName"),UserInfo.szIdentityName,CountArray(UserInfo.szIdentityName));
		//���֤��
		m_AccountsDB->GetValue_String(TEXT("IdentityNum"),UserInfo.szIdentityNum,CountArray(UserInfo.szIdentityNum));
		//�ֻ�����
		m_AccountsDB->GetValue_String(TEXT("MobilePhone"),UserInfo.szMobilePhone,CountArray(UserInfo.szMobilePhone));
		/* *****************************    �˺���Ϣ     ****************************/
		//����¼��ַ
		m_AccountsDB->GetValue_String(TEXT("LastLogonIP"),UserInfo.szLasLogonIp,CountArray(UserInfo.szLasLogonIp));
		//�������ʱ�� 
		m_AccountsDB->GetValue_SystemTime(TEXT("LastLogonDate"),UserInfo.LasLogonDate);
		/* *****************************    ��������     ****************************/
		//���ű�־ -- ����Ȧ
		UserInfo.dwGroupID=m_AccountsDB->GetValue_DWORD(TEXT("GroupID"));
		//�������� -- ����Ȧ
		m_AccountsDB->GetValue_String(TEXT("GroupName"),UserInfo.szGroupName,CountArray(UserInfo.szGroupName));

		//�û�����
		UserInfo.lUserScore = m_AccountsDB->GetValue_LONGLONG(TEXT("UserScore"));
		//�û���Ϸ��
		UserInfo.lUserGold = m_AccountsDB->GetValue_LONGLONG(TEXT("UserGold"));
		//�û�����
		UserInfo.lUserRoomCard = m_AccountsDB->GetValue_LONGLONG(TEXT("UserRoomCard"));

		//��������
		g_AttemperEngineSink->OnEventDataBaseResult(DBO_CL_SERVICE_REFRESH_USER_INFO, dwContextID, &UserInfo, sizeof(UserInfo));
	}
	else								//ʧ�ܴ���
	{
		//��������
		g_AttemperEngineSink->OnEventDataBaseResult(DBO_CL_SERVICE_REFRESH_USER_INFO, dwContextID, &UserInfo, sizeof(UserInfo));
	}

	return true;
}

//��ѯ������Ϣ�б�
bool CDataBaseEngineSink::On_DBR_Service_QueryRoomList(DWORD dwContextID, void * pData, WORD wDataSize)
{
	STR_DBR_CL_SERCIVR_QUERY_ROOMLIST* pGetTableInfo = (STR_DBR_CL_SERCIVR_QUERY_ROOMLIST* )pData;
	if(pData == NULL || wDataSize != sizeof(STR_DBR_CL_SERCIVR_QUERY_ROOMLIST))
		return false;

	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("dwOwnerID"),pGetTableInfo->dwUserID);


	//ִ�в�ѯ
	LONG lResultCode = m_TreasureDB->ExecuteProcess(TEXT("GSP_CL_Service_GetRoomList"),true);	//��ѯQPTreasure���ݿ��TableInfo��

	//�������
	On_DBO_Service_QueryRoomList(dwContextID, lResultCode);

	return true;

}

//��ѯ������Ϣ�б���
bool CDataBaseEngineSink::On_DBO_Service_QueryRoomList(DWORD dwContextID, DWORD dwResultCode)
{
	//�������
	if (dwResultCode == DB_SUCCESS)		//�ɹ�
	{
		for(int i=0;i < m_TreasureDB->GetRecordCount() && i < 10;i++,m_TreasureDB->MoveToNext())
		{
			//��������
			STR_DBO_CL_SERCIVR_QUERY_ROOMLIST TableInfo;
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

			//�û�ID
			TableInfo.dwUserID[0] = m_TreasureDB->GetValue_DWORD(TEXT("UserID1"));
			TableInfo.dwUserID[1] = m_TreasureDB->GetValue_DWORD(TEXT("UserID2"));
			TableInfo.dwUserID[2] = m_TreasureDB->GetValue_DWORD(TEXT("UserID3"));
			TableInfo.dwUserID[3] = m_TreasureDB->GetValue_DWORD(TEXT("UserID4"));
			TableInfo.dwUserID[4] = m_TreasureDB->GetValue_DWORD(TEXT("UserID5"));

			//�û��ǳ�
			m_TreasureDB->GetValue_String(TEXT("NickName1"),TableInfo.szNickName[0],CountArray(TableInfo.szNickName[0]));
			m_TreasureDB->GetValue_String(TEXT("NickName2"),TableInfo.szNickName[1],CountArray(TableInfo.szNickName[1]));
			m_TreasureDB->GetValue_String(TEXT("NickName3"),TableInfo.szNickName[2],CountArray(TableInfo.szNickName[2]));
			m_TreasureDB->GetValue_String(TEXT("NickName4"),TableInfo.szNickName[3],CountArray(TableInfo.szNickName[3]));
			m_TreasureDB->GetValue_String(TEXT("NickName5"),TableInfo.szNickName[4],CountArray(TableInfo.szNickName[4]));

			//�û�����
			TableInfo.dwScore[0] = m_TreasureDB->GetValue_DWORD(TEXT("Score1"));
			TableInfo.dwScore[1] = m_TreasureDB->GetValue_DWORD(TEXT("Score2"));
			TableInfo.dwScore[2] = m_TreasureDB->GetValue_DWORD(TEXT("Score3"));
			TableInfo.dwScore[3] = m_TreasureDB->GetValue_DWORD(TEXT("Score4"));
			TableInfo.dwScore[4] = m_TreasureDB->GetValue_DWORD(TEXT("Score5"));

			m_TreasureDB->GetValue_String(TEXT("CreateTime"),TableInfo.szTime,CountArray(TableInfo.szTime));	

			g_AttemperEngineSink->OnEventDataBaseResult(DBO_CL_SERVICE_QUERY_ROOM_LIST, dwContextID, &TableInfo, sizeof(STR_DBO_CL_SERCIVR_QUERY_ROOMLIST));
		}
	}
	else			//TODO ʧ���Ƿ���Ҫ����ͨ�ô���
	{

	}

	return true;
}

//��ȡ������
bool CDataBaseEngineSink::On_DBR_Service_GetRichList(DWORD dwContextID, void * pData, WORD wDataSize)
{

	STR_DBR_CL_SERCIVR_GET_RICHLIST* pRichList = (STR_DBR_CL_SERCIVR_GET_RICHLIST* )pData;
	if(pData == NULL || wDataSize != sizeof(STR_DBR_CL_SERCIVR_GET_RICHLIST))
		return false;

	m_AccountsDB->ResetParameter();

	//ִ�в�ѯ
	LONG lResultCode = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_Service_GetRichList"), true);		//��ѯ���ݿ�[QPAccountsDB]�е�RegalRank��

	//�������
	On_DBO_Service_GetRichList(dwContextID, lResultCode);


	return true;

}

//��ȡ�����񷵻�
bool CDataBaseEngineSink::On_DBO_Service_GetRichList(DWORD dwContextID, DWORD dwResultCode)
{
	if(dwResultCode == DB_SUCCESS)
	{
		STR_DBO_CL_SERCIVR_GET_RICHLIST result;
		ZeroMemory(&result,sizeof(STR_DBO_CL_SERCIVR_GET_RICHLIST));

		for(int i=0;i < m_AccountsDB->GetRecordCount() && i < 15;i++)
		{
			result.byCount ++;
			m_AccountsDB->GetValue_String(TEXT("Name"),result.RegalInfoList[i].szName,CountArray(result.RegalInfoList[i].szName));
			result.RegalInfoList[i].dwMoney = m_AccountsDB->GetValue_DWORD(TEXT("Money"));
			m_AccountsDB->GetValue_String(TEXT("Wechat"),result.RegalInfoList[i].szWechatAccount,CountArray(result.RegalInfoList[i].szWechatAccount));
			result.RegalInfoList[i].dwRegistID = m_AccountsDB->GetValue_DWORD(TEXT("RegistID"));	
			m_AccountsDB->GetValue_String(TEXT("QQ"),result.RegalInfoList[i].szQQ,CountArray(result.RegalInfoList[i].szQQ));
			m_AccountsDB->GetValue_String(TEXT("MobilePhone"),result.RegalInfoList[i].szMobilePhone,CountArray(result.RegalInfoList[i].szMobilePhone));

			m_AccountsDB->MoveToNext();
		}

		if(result.byCount > 0)
			g_AttemperEngineSink->OnEventDataBaseResult(DBO_CL_SERVICE_GET_RICH_LIST,dwContextID,&result,sizeof(STR_DBO_CL_SERCIVR_GET_RICHLIST));
	}

	return true;
}

//��ȡ¼���б�
bool CDataBaseEngineSink::On_DBR_Service_GetUserRecordList(DWORD dwContextID, void * pData, WORD wDataSize)
{
	if (sizeof(STR_DBR_CL_SERCIVR_GET_RECORDLIST) < wDataSize)
	{
		return false;
	}

	STR_DBR_CL_SERCIVR_GET_RECORDLIST* pDbReq = (STR_DBR_CL_SERCIVR_GET_RECORDLIST*)pData;

	m_TreasureDB->ResetParameter();

	m_TreasureDB->AddParameter(TEXT("@UserID"),pDbReq->dwUserID);

	//ִ�в�ѯ
	LONG lResultCode = m_TreasureDB->ExecuteProcess(TEXT("GSP_CL_Service_GetRecordList"),true);		//��ѯ���ݿ�[QPTreasureDB]�е�GameRecord��

	//�������
	On_DBO_Service_GetUserRecordList(dwContextID,lResultCode,NULL);

	return true;
}

//��ȡ¼���б���
bool CDataBaseEngineSink::On_DBO_Service_GetUserRecordList(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString)
{
	if (dwErrorCode == DB_SUCCESS)
	{
		//�����������ID
		vector <DWORD> dwTableID;
		for (int i = 0; i < m_TreasureDB->GetRecordCount(); i++)
		{
			//ɾ���ظ�������
			DWORD dwTmpTableID = m_TreasureDB->GetValue_DWORD(TEXT("TableID"));
			std::vector <DWORD>::iterator result = find(dwTableID.begin(), dwTableID.end(), dwTmpTableID);
			if ( result == dwTableID.end() )
			{
				dwTableID.push_back(dwTmpTableID);
			}
			m_TreasureDB->MoveToNext();
		}

		//�Ƶ���ͷ
		m_TreasureDB->MoveToFirst();
		int nRecordListNum = m_TreasureDB->GetRecordCount();

		//��ֵ÷�
		LONGLONG lTotalscore[10][5];
		for (int i = 0; i < 10; i++)
		{
			for (int j = 0; j < 5; j++)
			{
				lTotalscore[i][j] = 0;
			}			
		}

		//DBO����
		STR_DBO_CL_SERCIVR_GET_RECORDLIST *RecordList = new STR_DBO_CL_SERCIVR_GET_RECORDLIST[nRecordListNum];
		//TableID��ֵ
		for (DWORD i = 0; i < dwTableID.size(); i++)
		{
			RecordList[i].dwTableID = dwTableID[i];
		}

		for(int i=0;i < nRecordListNum;i++)
		{
			m_TreasureDB->ResetParameter();

			//�������ID
			DWORD dwTempTableID = m_TreasureDB->GetValue_DWORD(TEXT("TableID"));

			//��ø����ӵ�����
			for (DWORD j = 0; j < dwTableID.size(); j++)
			{
				if ( dwTempTableID == dwTableID[j] )
				{
					RecordList[i].dwUserID[0] = m_TreasureDB->GetValue_DWORD(TEXT("UserID0"));
					RecordList[i].dwUserID[1] = m_TreasureDB->GetValue_DWORD(TEXT("UserID1"));
					RecordList[i].dwUserID[2] = m_TreasureDB->GetValue_DWORD(TEXT("UserID2"));
					RecordList[i].dwUserID[3] = m_TreasureDB->GetValue_DWORD(TEXT("UserID3"));
					RecordList[i].dwUserID[4] = m_TreasureDB->GetValue_DWORD(TEXT("UserID4"));
					m_TreasureDB->GetValue_String(TEXT("Nick0"),RecordList[i].szUserName[0],LEN_NICKNAME);
					m_TreasureDB->GetValue_String(TEXT("Nick1"),RecordList[i].szUserName[1],LEN_NICKNAME);
					m_TreasureDB->GetValue_String(TEXT("Nick2"),RecordList[i].szUserName[2],LEN_NICKNAME);
					m_TreasureDB->GetValue_String(TEXT("Nick3"),RecordList[i].szUserName[3],LEN_NICKNAME);
					m_TreasureDB->GetValue_String(TEXT("Nick4"),RecordList[i].szUserName[4],LEN_NICKNAME);
					RecordList[i].byGameMode = m_TreasureDB->GetValue_BYTE(TEXT("GameMode"));
					RecordList[i].byZhuangType = m_TreasureDB->GetValue_BYTE(TEXT("ZhuangType"));
					RecordList[i].wCurrentJuShu = m_TreasureDB->GetValue_WORD(TEXT("CurrentJuShu"));
					RecordList[i].wTotalJuShu = m_TreasureDB->GetValue_WORD(TEXT("TotalJuShu"));
					m_TreasureDB->GetValue_String(TEXT("GameTime"),RecordList[i].szGameTime,30);
					m_TreasureDB->GetValue_String(TEXT("Score"),RecordList[i].szScore,50);

					//��ȡ�ܷ���
					CString strScore = RecordList[i].szScore;
					int nPos = 0;
					int score_index = 0;
					CString strTemp = strScore.Tokenize(CString(TEXT(',')), nPos);
					while (strTemp.Trim() != _T("") && score_index < 4)
					{
						lTotalscore[j][score_index++] += StrToInt(strTemp.GetBuffer());
						strTemp = strScore.Tokenize(CString(TEXT(',')), nPos);
					}
				}
			}

			m_TreasureDB->MoveToNext();
		}

		//���ʹ��¼������
		for (DWORD i = 0; i < dwTableID.size(); i++)
		{
			//���Ӵ�ַ���
			_sntprintf_s(RecordList[i].szTotalScore, CountArray(RecordList[i].szTotalScore), TEXT("%lld,%lld,%lld,%lld,%lld"),
				lTotalscore[i][0], lTotalscore[i][1], lTotalscore[i][2], lTotalscore[i][3], lTotalscore[i][4]);

			g_AttemperEngineSink->OnEventDataBaseResult(DBO_CL_SERVICE_GET_USER_RECORD_LIST, dwContextID, &RecordList[i], 
				sizeof(STR_DBO_CL_SERCIVR_GET_RECORDLIST));
		}

		delete RecordList;
	}
	else if(dwErrorCode == DB_NEEDMB)
	{
	}

	return true;
}

//��ȡָ��ID¼��
bool CDataBaseEngineSink::On_DBR_Service_GetSpecifiRecord(DWORD dwContextID, void * pData, WORD wDataSize)
{

	if (sizeof(STR_DBR_CL_SERCIVR_GET_SPECIFIC_RECORD) != wDataSize)
	{
		return false;
	}

	STR_DBR_CL_SERCIVR_GET_SPECIFIC_RECORD* pDbReq = (STR_DBR_CL_SERCIVR_GET_SPECIFIC_RECORD*)pData;

	m_TreasureDB->ResetParameter();

	m_TreasureDB->AddParameter(TEXT("@dwTableID"),pDbReq->dwTableID);

	//�������
	TCHAR szDescribeString[128]=TEXT("");
	m_TreasureDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	//ִ�в�ѯ
	LONG lResultCode = m_TreasureDB->ExecuteProcess(TEXT("GSP_CL_Service_GetSpecificRecord"),true);		//��ѯ���ݿ�[QPTreasureDB]�е�GameRecord��

	//�������
	CDBVarValue DBVarValue;
	m_TreasureDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	On_DBO_Service_GetSpecificRecord(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal));

	return true;

}

//��ȡָ��ID¼�񷵻�
bool CDataBaseEngineSink::On_DBO_Service_GetSpecificRecord(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString)
{
	if (dwErrorCode==DB_SUCCESS)
	{
		for(int i=0; i < m_TreasureDB->GetRecordCount(); i++)
		{
			//��������
			STR_DBO_CL_SERCIVR_GET_SPECIFIC_RECORD RecordInfo;
			ZeroMemory(&RecordInfo,sizeof(RecordInfo));

			//�������
			RecordInfo.wCurrentJuShu = m_TreasureDB->GetValue_WORD(TEXT("CurrentJuShu"));
			m_TreasureDB->GetValue_String(TEXT("Nick0"), RecordInfo.szNickName[0], LEN_NICKNAME);
			m_TreasureDB->GetValue_String(TEXT("Nick1"), RecordInfo.szNickName[1], LEN_NICKNAME);
			m_TreasureDB->GetValue_String(TEXT("Nick2"), RecordInfo.szNickName[2], LEN_NICKNAME);
			m_TreasureDB->GetValue_String(TEXT("Nick3"), RecordInfo.szNickName[3], LEN_NICKNAME);
			m_TreasureDB->GetValue_String(TEXT("Nick4"), RecordInfo.szNickName[4], LEN_NICKNAME);
			m_TreasureDB->GetValue_String(TEXT("Score"), RecordInfo.szScore, 50);

			g_AttemperEngineSink->OnEventDataBaseResult(DBO_CL_SERVICE_GET_SPECIFIC_RECORD, dwContextID, &RecordInfo, sizeof(STR_DBO_CL_SERCIVR_GET_SPECIFIC_RECORD));

			m_TreasureDB->MoveToNext();
		}
	}

	return true;
}

//��ȡ���߽���
bool CDataBaseEngineSink::On_DBR_Service_OnlineReward(DWORD dwContextID, void * pData, WORD wDataSize)
{

	if (sizeof(STR_DBR_CL_SERCIVR_ONLINE_REWARD) < wDataSize)
	{
		return false;
	}

	STR_DBR_CL_SERCIVR_ONLINE_REWARD* pDbReq = (STR_DBR_CL_SERCIVR_ONLINE_REWARD*)pData;

	//��¼����200���

	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@UserID"), pDbReq->dwUserID);
	m_TreasureDB->AddParameter(TEXT("@lGold"), 200);
	m_TreasureDB->AddParameter(TEXT("@lOpenRoomCard"), 0);
	m_TreasureDB->AddParameter(TEXT("@lDiamond"), 0);

	//ִ�в�ѯ
	LONG lResultCode = m_TreasureDB->ExecuteProcess(TEXT("GSP_CL_Service_ModifyUserMoney"), true);	//�޸�[QPTreasureDB]���ݿ��е�GameScoreInfo��

	//�������
	On_DBO_Service_OnlineReward(dwContextID, lResultCode, TEXT("��ȡ��¼����ʧ�ܣ�"));

	return true;

}

//��ȡ���߽�������
bool CDataBaseEngineSink::On_DBO_Service_OnlineReward(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString)
{
	if(dwErrorCode == DB_SUCCESS)
	{
		STR_DBO_CL_SERCIVR_ONLINE_REWARD result;
		result.byType = 1;
		result.dwCount = m_TreasureDB->GetValue_DWORD(TEXT("Gold"));
		lstrcpyn(result.szDescribe,TEXT("�ɹ���ȡ��¼������"),CountArray(result.szDescribe));

		g_AttemperEngineSink->OnEventDataBaseResult(DBO_CL_SERVICE_ONLINE_REWARD, dwContextID, &result, sizeof(STR_DBO_CL_SERCIVR_ONLINE_REWARD));

		//�Ƹ��������
		STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO UserScoreInfo;
		UserScoreInfo.dwUserID = m_TreasureDB->GetValue_DWORD(TEXT("UserID"));
		UserScoreInfo.lGold = m_TreasureDB->GetValue_LONGLONG(TEXT("Gold"));
		UserScoreInfo.lOpenRoomCard = m_TreasureDB->GetValue_LONGLONG(TEXT("OpenRoomCard"));
		UserScoreInfo.lDiamond = m_TreasureDB->GetValue_LONGLONG(TEXT("Diamond"));

		//���ͽ��
		g_AttemperEngineSink->OnEventDataBaseResult(DBO_CL_USER_QUERY_SCORE_INFO,dwContextID,&UserScoreInfo,sizeof(STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO));

	}
	else
	{
		STR_DBO_CL_SERCIVR_ONLINE_REWARD result;
		result.byType = 0;
		result.dwCount = 0;
		lstrcpyn(result.szDescribe, pszErrorString, CountArray(result.szDescribe));

		g_AttemperEngineSink->OnEventDataBaseResult(DBO_CL_SERVICE_ONLINE_REWARD,dwContextID,&result,sizeof(STR_DBO_CL_SERCIVR_ONLINE_REWARD));
	}

	return true;
}

//��ȡ�����б�
bool CDataBaseEngineSink::On_DBR_Service_GetTaskList(DWORD dwContextID, void * pData, WORD wDataSize)
{

	if (sizeof(STR_DBR_CL_SERCIVR_GET_TASKLIST) != wDataSize)
	{
		return false;
	}

	STR_DBR_CL_SERCIVR_GET_TASKLIST* pDbReq = (STR_DBR_CL_SERCIVR_GET_TASKLIST*)pData;

	//��ȡ�����б�
	m_AccountsDB->ResetParameter();
	m_AccountsDB->AddParameter(TEXT("@dwUserID"), pDbReq->dwUserID);

	//ִ�в�ѯ
	LONG lResultCode = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_Service_GetTaskList"), true);		//��ѯ[QPAccountsDB]���ݿ��TaskList��

	//�������
	On_DBO_Service_GetTaskList(pDbReq->dwUserID, dwContextID, lResultCode, TEXT("������ʧ�ܣ����Ժ�����!"));

	return true;

}

//��ȡ�����б���
bool CDataBaseEngineSink::On_DBO_Service_GetTaskList(DWORD dwUserID, DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString)
{
	//��������
	STR_DBO_CL_SERCIVR_GET_TASKLIST taskItem;
	ZeroMemory(&taskItem,sizeof(STR_DBO_CL_SERCIVR_GET_TASKLIST));

	//��ʱ����
	int nTaskNum = m_AccountsDB->GetRecordCount();
	//TODO ��������Ŀ��ƺ�����web���ƣ�������ʱд��
	if ( MAX_SEND_CLIENT_TASK_NUM < nTaskNum )
	{
		nTaskNum = MAX_SEND_CLIENT_TASK_NUM;
	}

	//��������
	STR_DBO_CL_SERCIVR_GET_TASKLIST *arTaskList = new STR_DBO_CL_SERCIVR_GET_TASKLIST [nTaskNum]; 

	//��ѯ�ɹ�����������	
	if(dwErrorCode == DB_SUCCESS)
	{
		//��������
		WORD wSendSize=0;

		//ѭ�����ÿ����������
		for(int i=0; i<nTaskNum; i++)
		{
			//��������б�
			taskItem.wTaskID = m_AccountsDB->GetValue_WORD(TEXT("TaskID"));
			taskItem.cbTaskType = m_AccountsDB->GetValue_BYTE(TEXT("TaskType"));
			taskItem.cbActionType = m_AccountsDB->GetValue_BYTE(TEXT("ActionType"));
			taskItem.dwNeedCount = m_AccountsDB->GetValue_DWORD(TEXT("NeedCount"));
			m_AccountsDB->GetValue_String(TEXT("Describe"), taskItem.szDescribe, CountArray(taskItem.szDescribe));		
			taskItem.byRewardType = m_AccountsDB->GetValue_BYTE(TEXT("RewardType"));
			taskItem.dwRewardCount = m_AccountsDB->GetValue_DWORD(TEXT("RewardCount"));

			//�������״̬
			taskItem.cbTaskStatus = m_AccountsDB->GetValue_BYTE(TEXT("TaskStatus"));
			taskItem.dwFinishedCount = m_AccountsDB->GetValue_DWORD(TEXT("LeftCount"));

			//��ֵ
			memcpy_s(&arTaskList[i], sizeof(STR_DBO_CL_SERCIVR_GET_TASKLIST), &taskItem, sizeof(STR_DBO_CL_SERCIVR_GET_TASKLIST));

			m_AccountsDB->MoveToNext();
		}		
	}

	g_AttemperEngineSink->OnEventDataBaseResult(DBO_CL_SERVICE_GET_TASK_LIST, dwContextID, arTaskList, nTaskNum*sizeof(STR_DBO_CL_SERCIVR_GET_TASKLIST));

	//TODO �ں���ʹ�õ�ʱ�����
	/*delete [] arTaskList;*/

	return true;
}

//��ȡ������
bool CDataBaseEngineSink::On_DBR_Service_GetTaskReward(DWORD dwContextID, void * pData, WORD wDataSize)
{
	if (sizeof(STR_DBR_CL_SERCIVR_GET_TASK_REWARD) < wDataSize)
	{
		return false;
	}

	STR_DBR_CL_SERCIVR_GET_TASK_REWARD* pDbReq = (STR_DBR_CL_SERCIVR_GET_TASK_REWARD*)pData;

	m_AccountsDB->ResetParameter();

	m_AccountsDB->AddParameter(TEXT("@dwUserID"), pDbReq->dwUserID);
	m_AccountsDB->AddParameter(TEXT("@wTaskID"), pDbReq->wTaskID);

	//�������
	WCHAR szDescribe[128]=L"";
	m_AccountsDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

	//ִ�в�ѯ
	LONG lResultCode = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_Service_GetTaskReward"), true);		//��ѯ���ݿ�[QPAccountsDB]�е�AccountsTaskStatus���������ݿ��е�GameScoreInfo��
	//�������
	CDBVarValue DBVarValue;
	m_AccountsDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	//��ȡ����������
	On_DBO_Service_GetTaskReward(pDbReq->wTaskID, dwContextID, lResultCode, CW2CT(DBVarValue.bstrVal));


	return true;

}

//��ȡ����������
bool CDataBaseEngineSink::On_DBO_Service_GetTaskReward(DWORD dwUserID, DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString)
{
	if(dwErrorCode == DB_SUCCESS)
	{
		//��ȡ�����û���ҷ�����ʯ
		STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO UserScoreInfo;
		ZeroMemory(&UserScoreInfo,sizeof(STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO));

		UserScoreInfo.dwUserID = dwUserID;
		UserScoreInfo.lGold = m_AccountsDB->GetValue_LONG(TEXT("Gold"));
		UserScoreInfo.lOpenRoomCard = m_AccountsDB->GetValue_LONG(TEXT("OpenRoomCard"));
		UserScoreInfo.lDiamond = m_AccountsDB->GetValue_LONG(TEXT("Diamond"));

		//�����ʲ�������Ϣ
		g_AttemperEngineSink->OnEventDataBaseResult(DBO_CL_USER_QUERY_SCORE_INFO, dwContextID, &UserScoreInfo, sizeof(STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO));
	}

	//�յ�������
	STR_DBO_CL_SERCIVR_GET_TASK_REWARD result;
	ZeroMemory(&result,sizeof(result));
	result.lResultCode = dwErrorCode;
	lstrcpyn( result.szDescribe, pszErrorString, CountArray( result.szDescribe));

	g_AttemperEngineSink->OnEventDataBaseResult(DBO_CL_SERVICE_GET_TASK_REWARD,dwContextID,&result,sizeof(result));

	return true;
}

//����齱
bool CDataBaseEngineSink::On_DBR_Service_RequestLottery(DWORD dwContextID, void * pData, WORD wDataSize)
{

	if (sizeof(STR_DBR_CL_SERCIVR_REQUEST_LOTTERY) != wDataSize)
	{
		return false;
	}

	STR_DBR_CL_SERCIVR_REQUEST_LOTTERY* pDbReq = (STR_DBR_CL_SERCIVR_REQUEST_LOTTERY*)pData;

	//��ȡһ��1-100�������
	srand((unsigned int)time(NULL)) ;
	int index = rand() % 100 + 1;

	m_AccountsDB->ResetParameter();
	m_AccountsDB->AddParameter(TEXT("@UserID"), pDbReq->dwUserID);
	m_AccountsDB->AddParameter(TEXT("@Rand"), index);
	//�������
	WCHAR szDescribe[128]=L"";
	m_AccountsDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

	//ִ�в�ѯ,ִ�г齱
	LONG lResultCode = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_Service_RequestLottery"), true);		//��ѯ���ݿ�[QPAccountsDB]��LotteryList��ִ�г齱��������Ϸ��Ҳ�и�����齱��
	//�������
	CDBVarValue DBVarValue;
	m_AccountsDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	On_DBO_Service_RequestLottery(pDbReq->dwUserID, dwContextID, lResultCode, CW2CT(DBVarValue.bstrVal));

	return true;
}

//����齱����
bool CDataBaseEngineSink::On_DBO_Service_RequestLottery(DWORD dwUserID, DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString)
{
	//��������
	STR_DBO_CL_SERCIVR_REQUEST_LOTTERY LotteryResult;
	ZeroMemory(&LotteryResult, sizeof(STR_DBO_CL_SERCIVR_REQUEST_LOTTERY));

	//��Ϣ����
	LotteryResult.lResultCode = dwErrorCode;
	lstrcpyn(LotteryResult.szDescribeString, pszErrorString, CountArray(LotteryResult.szDescribeString));

	//ִ�гɹ�
	if(dwErrorCode == DB_SUCCESS)
	{
		//���ݿ�1-6��Ӧ�ͻ���0-5�����ݿ�7-11��Ӧ�ͻ���7-11���ͻ���6����δ�н������ݿ�δ�洢
		LotteryResult.byIndex = m_AccountsDB->GetValue_INT(TEXT("RandIndex"));
		if ( LotteryResult.byIndex >=1 && LotteryResult.byIndex <= 11 )
		{
			if ( LotteryResult.byIndex <= 6 )
			{
				LotteryResult.byIndex = LotteryResult.byIndex - 1;
			}

			//���ͳ齱���
			g_AttemperEngineSink->OnEventDataBaseResult(DBO_CL_SERVICE_REQUEST_LOTTERY, dwContextID, &LotteryResult, sizeof(STR_DBO_CL_SERCIVR_REQUEST_LOTTERY));

		}
		else		//δ�н�����6
		{
			//��ֵ
			LotteryResult.byIndex = 6;

			//���ͳ齱���
			g_AttemperEngineSink->OnEventDataBaseResult(DBO_CL_SERVICE_REQUEST_LOTTERY, dwContextID, &LotteryResult, sizeof(STR_DBO_CL_SERCIVR_REQUEST_LOTTERY));
		}	
	}
	else	//ʧ�ܣ�ֱ�ӷ��ͳ齱���
	{
		g_AttemperEngineSink->OnEventDataBaseResult(DBO_CL_SERVICE_REQUEST_LOTTERY, dwContextID, &LotteryResult, sizeof(STR_DBO_CL_SERCIVR_REQUEST_LOTTERY));
	}

	//����Ƹ��������
	STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO UserScoreInfo;
	ZeroMemory(&UserScoreInfo,sizeof(STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO));

	UserScoreInfo.dwUserID = dwUserID;
	UserScoreInfo.lGold = m_AccountsDB->GetValue_LONG(TEXT("Gold"));
	UserScoreInfo.lOpenRoomCard = m_AccountsDB->GetValue_LONG(TEXT("OpenRoomCard"));
	UserScoreInfo.lDiamond = m_AccountsDB->GetValue_LONG(TEXT("Diamond"));
	UserScoreInfo.lRewardCard = m_AccountsDB->GetValue_LONG(TEXT("RewardCard"));
	UserScoreInfo.lScore = m_AccountsDB->GetValue_LONG(TEXT("Score"));

	//���ͲƸ������Ϣ
	g_AttemperEngineSink->OnEventDataBaseResult(DBO_CL_USER_QUERY_SCORE_INFO, dwContextID, &UserScoreInfo, sizeof(STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO));

	return true;
}

//pure���� ���а� 
bool CDataBaseEngineSink::On_DBR_CL_SERVICE_PURE_STANDING_LIST(DWORD dwContextID, void * pData, WORD wDataSize)
{
	if (sizeof(STR_SUB_CL_SERVICE_PURE_STANDING_LIST) != wDataSize)
	{
		return false;
	}

	STR_SUB_CL_SERVICE_PURE_STANDING_LIST* pDbReq = (STR_SUB_CL_SERVICE_PURE_STANDING_LIST*)pData;

	m_AccountsDB->ResetParameter();
	m_AccountsDB->AddParameter(TEXT("mystery"), _MYSTERY);
	m_AccountsDB->AddParameter(TEXT("dwUserID"), pDbReq->dwUserID);

	//ִ�в�ѯ,ִ�г齱
	LONG lResultCode = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_SERVICE_PURE_STANDING_LIST"), true);		//��ѯ���ݿ�[QPAccountsDB]��LotteryList��ִ�г齱��������Ϸ��Ҳ�и�����齱��

	//�б���
	WORD wPacketSize=0;
	BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
	wPacketSize=0;
	STR_CMD_LC_SERVICE_PURE_STANDING_LIST * pDBO=NULL;
	STR_CMD_LC_SERVICE_PURE_STANDING_FINISH DBOFinish;
	ZeroMemory(&DBOFinish, sizeof(DBOFinish));

	while (m_AccountsDB->IsRecordsetEnd()==false)
	{
		//������Ϣ
		if ((wPacketSize+sizeof(STR_CMD_LC_SERVICE_PURE_STANDING_LIST))>sizeof(cbBuffer))
		{
			g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_SERVICE_PURE_STANDING_LIST,dwContextID,cbBuffer,wPacketSize);
			wPacketSize=0;
		}
		//��ȡ��Ϣ
		pDBO=(STR_CMD_LC_SERVICE_PURE_STANDING_LIST *)(cbBuffer+wPacketSize);
		pDBO->byRanking = m_AccountsDB->GetValue_BYTE(TEXT("Ranking"));
		pDBO->dwUserID=m_AccountsDB->GetValue_DWORD(TEXT("UserID"));
		pDBO->dwLoveLiness=m_AccountsDB->GetValue_DWORD(TEXT("LoveLiness"));
		m_AccountsDB->GetValue_String(TEXT("NickName"),pDBO->szNickName,CountArray(pDBO->szNickName));
		m_AccountsDB->GetValue_String(TEXT("HeadUrl"),pDBO->szHeadUrl,CountArray(pDBO->szHeadUrl));

		if(pDBO->dwUserID == pDbReq->dwUserID)
		{
			DBOFinish.dwRanking = pDBO->byRanking;
			DBOFinish.dwLoveLiness = pDBO->dwLoveLiness;
		}
		//����λ��
		wPacketSize+=sizeof(STR_CMD_LC_SERVICE_PURE_STANDING_LIST);

		//�ƶ���¼
		m_AccountsDB->MoveToNext();
	}
	if (wPacketSize>0) g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_SERVICE_PURE_STANDING_LIST,dwContextID,cbBuffer,wPacketSize);

	g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_SERVICE_PURE_STANDING_FINISH,dwContextID,&DBOFinish,sizeof(DBOFinish));

	return true;
}

//pure���� ���ս�� 
bool CDataBaseEngineSink::On_DBR_CL_SERVICE_PURE_RECORD_LIST(DWORD dwContextID, void * pData, WORD wDataSize)
{
	if (sizeof(STR_SUB_CL_SERVICE_PURE_RECORD_LIST) != wDataSize)
	{
		return false;
	}

	STR_SUB_CL_SERVICE_PURE_RECORD_LIST* pDbReq = (STR_SUB_CL_SERVICE_PURE_RECORD_LIST*)pData;

#pragma region �����Ϣ
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("dwUserID"), pDbReq->dwUserID);
	m_TreasureDB->AddParameter(TEXT("dwClubID"), pDbReq->dwClubID);
	m_TreasureDB->AddParameter(TEXT("tmQueryStartData"), pDbReq->tmQueryStartData);
	m_TreasureDB->AddParameter(TEXT("byMask"), pDbReq->byMask);
	//m_TreasureDB->AddParameter(TEXT("tmQueryEndData"), pDbReq->tmQueryEndData);

	//ִ�в�ѯ,ִ�г齱
	LONG lResultCode = m_TreasureDB->ExecuteProcess(TEXT("GSP_CL_SERVICE_PURE_RECORD_LIST"), true);

	//�б���
	WORD wPacketSize=0;
	BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
	wPacketSize=0;
	STR_CMD_LC_SERVICE_PURE_RECORD_LIST * pDBO=NULL;

	while ((lResultCode == DB_SUCCESS) && (m_TreasureDB->IsRecordsetEnd()==false))
	{
		//������Ϣ
		if ((wPacketSize+sizeof(STR_CMD_LC_SERVICE_PURE_RECORD_LIST))>sizeof(cbBuffer))
		{
			g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_SERVICE_PURE_RECORD_LIST,dwContextID,cbBuffer,wPacketSize);
			wPacketSize=0;
		}
		//��ȡ��Ϣ
		pDBO=(STR_CMD_LC_SERVICE_PURE_RECORD_LIST *)(cbBuffer+wPacketSize);
		BYTE gameMode = m_TreasureDB->GetValue_BYTE(TEXT("ModeID"));
		//0���� 1����  2���  3�������
		if(0 == gameMode)
		{
			pDBO->byMask = 1;
		}
		else if((3 == gameMode) || (2 ==gameMode) )
		{
			pDBO->byMask = 2;
		}

		pDBO->dwDrawID = m_TreasureDB->GetValue_DWORD(TEXT("DrawID"));
		m_TreasureDB->GetValue_String(TEXT("KindName"),pDBO->szKindName,CountArray(pDBO->szKindName));
		pDBO->dwTableID=m_TreasureDB->GetValue_DWORD(TEXT("TableID"));
		SYSTEMTIME timeStartTime,tiemEndTime;
		m_TreasureDB->GetValue_SystemTime(TEXT("StartTime"), timeStartTime);
		m_TreasureDB->GetValue_SystemTime(TEXT("EndTime"), tiemEndTime);

		CString temp;
		temp.Format(TEXT("%d/%d/%d %d:%d:%d - %d:%d:%d"),
			timeStartTime.wYear, timeStartTime.wMonth, timeStartTime.wDay,
			timeStartTime.wHour,timeStartTime.wMinute,timeStartTime.wSecond,
			tiemEndTime.wHour,tiemEndTime.wMinute,tiemEndTime.wSecond
			);

		memcpy(pDBO->szTime, temp, sizeof(TCHAR)*128);

		//����λ��
		wPacketSize+=sizeof(STR_CMD_LC_SERVICE_PURE_RECORD_LIST);

		//�ƶ���¼
		m_TreasureDB->MoveToNext();
	}
	if (wPacketSize>0) g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_SERVICE_PURE_RECORD_LIST,dwContextID,cbBuffer,wPacketSize);

#pragma endregion

#pragma region ����ϵ������Ϣ
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("dwUserID"), pDbReq->dwUserID);
	m_TreasureDB->AddParameter(TEXT("dwClubID"), pDbReq->dwClubID);
	m_TreasureDB->AddParameter(TEXT("tmQueryStartData"), pDbReq->tmQueryStartData);
	m_TreasureDB->AddParameter(TEXT("byMask"), pDbReq->byMask);
	//m_TreasureDB->AddParameter(TEXT("tmQueryEndData"), pDbReq->tmQueryEndData);

	//ִ�в�ѯ,ִ�г齱
	LONG lResultCode2 = m_TreasureDB->ExecuteProcess(TEXT("GSP_CL_SERVICE_PURE_RECORD_LIST_PLAYINFO"), true);

	//�б���
	WORD wPacketSize2=0;
	BYTE cbBuffer2[MAX_ASYNCHRONISM_DATA/10];
	wPacketSize2=0;
	STR_CMD_LC_SERVICE_PURE_RECORD_LIST_PLAYERINFO * pDBO2=NULL;

	while ((lResultCode2 == DB_SUCCESS ) && (m_TreasureDB->IsRecordsetEnd()==false))
	{
		//������Ϣ
		if ((wPacketSize2+sizeof(STR_CMD_LC_SERVICE_PURE_RECORD_LIST_PLAYERINFO))>sizeof(cbBuffer2))
		{
			g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_SERVICE_PURE_RECORD_LIST_PINFO,dwContextID,cbBuffer2,wPacketSize2);
			wPacketSize2=0;
		}
		//��ȡ��Ϣ
		pDBO2=(STR_CMD_LC_SERVICE_PURE_RECORD_LIST_PLAYERINFO*)(cbBuffer2+wPacketSize2);
		pDBO2->dwDrawID = m_TreasureDB->GetValue_DWORD(TEXT("DrawID"));
		pDBO2->dwUserID = m_TreasureDB->GetValue_DWORD(TEXT("UserID"));
		m_TreasureDB->GetValue_String(TEXT("NickName"),pDBO2->szNickName,CountArray(pDBO2->szNickName));
		m_TreasureDB->GetValue_String(TEXT("HeadUrl"),pDBO2->szHeadUrl,CountArray(pDBO2->szHeadUrl));
		pDBO2->lScore=m_TreasureDB->GetValue_LONGLONG(TEXT("Score"));

		//����λ��
		wPacketSize2+=sizeof(STR_CMD_LC_SERVICE_PURE_RECORD_LIST_PLAYERINFO);

		//�ƶ���¼
		m_TreasureDB->MoveToNext();
	}
	if (wPacketSize2>0) g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_SERVICE_PURE_RECORD_LIST_PINFO,dwContextID,cbBuffer2,wPacketSize2);

#pragma endregion

#pragma region �����Ϣ����
	STR_CMD_LC_SERVICE_PURE_RECORD_LIST_FINIST DBOFinish;
	DBOFinish.byMask = 1;

	g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_SERVICE_PURE_RECORD_FINISH,dwContextID,&DBOFinish,sizeof(DBOFinish));

	return true;
#pragma endregion

}

//pure���� С��ս�� 
bool CDataBaseEngineSink::On_DBR_CL_SERVICE_PURE_XJ_RECORD_LIST(DWORD dwContextID, void * pData, WORD wDataSize)
{
	if (sizeof(STR_SUB_CL_SERVICE_PURE_XJ_RECORD_LIST) != wDataSize)
	{
		return false;
	}

	STR_SUB_CL_SERVICE_PURE_XJ_RECORD_LIST* pDbReq = (STR_SUB_CL_SERVICE_PURE_XJ_RECORD_LIST*)pData;

#pragma region С����Ϣ
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("dwDrawID"), pDbReq->dwDrawID);

	//ִ�в�ѯ
	LONG lResultCode = m_TreasureDB->ExecuteProcess(TEXT("GSP_CL_SERVICE_PURE_XJ_RECORD_LIST"), true);

	//�б���
	WORD wPacketSize=0;
	BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
	wPacketSize=0;
	STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST * pDBO=NULL;

	if(lResultCode == DB_SUCCESS)
	{
		while (m_TreasureDB->IsRecordsetEnd()==false)
		{
			//������Ϣ
			if ((wPacketSize+sizeof(STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST))>sizeof(cbBuffer))
			{
				g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_SERVICE_PURE_XJ_RECORD_LIST,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}
			//��ȡ��Ϣ
			pDBO=(STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST *)(cbBuffer+wPacketSize);
			pDBO->bRoundCount = m_TreasureDB->GetValue_BYTE(TEXT("RoundCount"));
			pDBO->dwRecordID = m_TreasureDB->GetValue_DWORD(TEXT("RoundID"));

			//����λ��
			wPacketSize+=sizeof(STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST);

			//�ƶ���¼
			m_TreasureDB->MoveToNext();
		}
		if (wPacketSize>0) g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_SERVICE_PURE_XJ_RECORD_LIST,dwContextID,cbBuffer,wPacketSize);
	}
#pragma endregion

#pragma region С���ϵ������Ϣ
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("dwDrawID"), pDbReq->dwDrawID);

	//ִ�в�ѯ,ִ�г齱
	LONG lResultCode2 = m_TreasureDB->ExecuteProcess(TEXT("GSP_CL_SERVICE_PURE_XJ_RECORD_LIST_PLAYINFO"), true);

	//�б���
	WORD wPacketSize2=0;
	BYTE cbBuffer2[MAX_ASYNCHRONISM_DATA/10];
	STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_PLAYERINFO * pDBO2=NULL;

	if(lResultCode2 == DB_SUCCESS)
	{
		while (m_TreasureDB->IsRecordsetEnd()==false)
		{
			//������Ϣ
			if ((wPacketSize2+sizeof(STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_PLAYERINFO))>sizeof(cbBuffer2))
			{
				g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_SERVICE_PURE_XJ_RECORD_LIST_PINFO,dwContextID,cbBuffer2,static_cast<WORD>(wPacketSize2));
				wPacketSize2=0;
			}
			//��ȡ��Ϣ
			pDBO2=(STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_PLAYERINFO*)(cbBuffer2+wPacketSize2);
			pDBO2->bRoundCount = m_TreasureDB->GetValue_BYTE(TEXT("RoundCount"));
			pDBO2->dwUserID = m_TreasureDB->GetValue_DWORD(TEXT("UserID"));
			m_TreasureDB->GetValue_String(TEXT("NickName"),pDBO2->szNickName,CountArray(pDBO2->szNickName));
			pDBO2->lScore=m_TreasureDB->GetValue_LONGLONG(TEXT("Score"));

			//����λ��
			wPacketSize2+=sizeof(STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_PLAYERINFO);

			//�ƶ���¼
			m_TreasureDB->MoveToNext();
		}
		if (wPacketSize2>0) g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_SERVICE_PURE_XJ_RECORD_LIST_PINFO,dwContextID,cbBuffer2,wPacketSize2);
	}
#pragma endregion

#pragma region С����Ϣ����
	STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_FINISH DBOFinish;
	DBOFinish.byMask = 1;

	g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_SERVICE_PURE_XJ_RECORD_FINISH,dwContextID,&DBOFinish,sizeof(DBOFinish));
#pragma endregion


	return true;
}

//С��¼��ط�
bool CDataBaseEngineSink::On_DBR_CL_Service_XJRecordPlayback(DWORD dwContextID, void * pData, WORD wDataSize)
{
	if (sizeof(STR_DBR_CL_SERVICE_XJ_RECORD_PLAYBACK) != wDataSize)
	{
		return false;
	}

	STR_DBR_CL_SERVICE_XJ_RECORD_PLAYBACK* pDbReq = (STR_DBR_CL_SERVICE_XJ_RECORD_PLAYBACK*)pData;

	//�������
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@dwRoundID"), pDbReq->dwRecordID);

	//�������
	WCHAR szDescribe[128] = L"";
	m_TreasureDB->AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

	//ִ�в�ѯ
	LONG lResultCode = m_TreasureDB->ExecuteProcess(TEXT("GSP_CL_GETVIDEORECORD"), true);
	if (lResultCode == DB_SUCCESS)
	{
		//�������
		CDBVarValue DBVarValue;
		m_TreasureDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//����DBO����
		STR_DBO_LC_SERVICE_XJ_RECORD_PLAYBACK DBO;
		ZeroMemory(&DBO, sizeof(STR_DBO_LC_SERVICE_XJ_RECORD_PLAYBACK));

		//��ȡ¼������
		TCHAR szData[2*LEN_MAX_RECORD_SIZE];
		m_TreasureDB->GetValue_String(TEXT("VideoData"), szData, CountArray(szData));
		StrToBin(szData, DBO.cbRecordData, 0, LEN_MAX_RECORD_SIZE*2-1);

		DBO.dwDataSize = m_TreasureDB->GetValue_DWORD(TEXT("VideoSize"));
		DBO.wCurrentCount =  m_TreasureDB->GetValue_WORD(TEXT("CurrentRound"));

		g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_SERVICE_XJ_RECORD_PLAYBACK, dwContextID, &DBO, sizeof(STR_DBO_LC_SERVICE_XJ_RECORD_PLAYBACK));

	}


	return true;
}

//�ͷ���Ϣ
bool CDataBaseEngineSink::On_DBR_CL_SERVICE_CUSTOMER_MESSEGE(DWORD dwContextID, void * pData, WORD wDataSize)
{

	if (sizeof(STR_DBR_CL_SERVICE_CUSTOMER_MESSEGE) != wDataSize)
	{
		return false;
	}

	STR_DBR_CL_SERVICE_CUSTOMER_MESSEGE* pDbReq = (STR_DBR_CL_SERVICE_CUSTOMER_MESSEGE*)pData;

	//�������
	m_PlatformDB->ResetParameter();
	m_PlatformDB->AddParameter(TEXT("@FirmID"), _MYSTERY);
	m_PlatformDB->AddParameter(TEXT("@MessageType"), pDbReq->cbMessegeFlag);

	//ִ�в�ѯ
	LONG lResultCode = m_PlatformDB->ExecuteProcess(TEXT("GSP_GP_GET_SERVICE_MESSAGE"), true);
	if (lResultCode == DB_SUCCESS)
	{
		//����DBO����
		STR_DBO_LC_SERVICE_CUSTOMER_MESSEGE DBO;
		ZeroMemory(&DBO, sizeof(STR_DBO_LC_SERVICE_CUSTOMER_MESSEGE));

		//��ȡ¼������
		m_PlatformDB->GetValue_String(TEXT("MessageText"), DBO.szMessege, CountArray(DBO.szMessege));
		DBO.cbMessegeFlag = m_PlatformDB->GetValue_BYTE(TEXT("MessageType"));

		g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_SERVICE_CUSTOMER_MESSEGE, dwContextID, &DBO, sizeof(STR_DBO_LC_SERVICE_CUSTOMER_MESSEGE));
	}


	return true;
}

//�����Ҵ�����Ϣ 
bool CDataBaseEngineSink::On_DBR_CL_SERVICE_GOLD_INFO(DWORD dwContextID, void * pData, WORD wDataSize)
{
	if (sizeof(STR_SUB_CL_SERVICE_GOLD_INFO) != wDataSize)
	{
		return false;
	}

	STR_SUB_CL_SERVICE_GOLD_INFO* pDbReq = (STR_SUB_CL_SERVICE_GOLD_INFO*)pData;

#pragma region �����Ҵ�����Ϣ
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("dwUserID"), pDbReq->dwUserID);
	m_TreasureDB->AddParameter(TEXT("dwModID"), pDbReq->dwModID);
	m_TreasureDB->AddParameter(TEXT("dwKindID"), pDbReq->dwKindID);
	m_TreasureDB->AddParameter(TEXT("mystery"), _MYSTERY);

	//ִ�в�ѯ
	LONG lResultCode = m_TreasureDB->ExecuteProcess(TEXT("GSP_CL_SERVICE_GOLD_INFO"), true);

	//�б���
	WORD wPacketSize=0;
	BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
	wPacketSize=0;
	STR_CMD_LC_SERVICE_GOLD_INFO * pDBO=NULL;

	while ((lResultCode == DB_SUCCESS) && m_TreasureDB->IsRecordsetEnd()==false)
	{
		//������Ϣ
		if ((wPacketSize+sizeof(STR_CMD_LC_SERVICE_GOLD_INFO))>sizeof(cbBuffer))
		{
			g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_SERVICE_GOLD_INFO,dwContextID,cbBuffer,wPacketSize);
			wPacketSize=0;
		}
		//��ȡ��Ϣ
		pDBO=(STR_CMD_LC_SERVICE_GOLD_INFO *)(cbBuffer+wPacketSize);
		pDBO->byGoldMod = m_TreasureDB->GetValue_BYTE(TEXT("GoldMod"));
		pDBO->dwScore = m_TreasureDB->GetValue_DWORD(TEXT("GoldScore"));
		pDBO->dwGold = m_TreasureDB->GetValue_DWORD(TEXT("Gold"));
		pDBO->dwServiceGold = m_TreasureDB->GetValue_DWORD(TEXT("ServiceGold"));

		//����λ��
		wPacketSize+=sizeof(STR_CMD_LC_SERVICE_GOLD_INFO);

		//�ƶ���¼
		m_TreasureDB->MoveToNext();
	}
	if (wPacketSize>0) g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_SERVICE_GOLD_INFO,dwContextID,cbBuffer,wPacketSize);

#pragma endregion

#pragma region �����Ҵ�����Ϣ ����
	STR_CMD_LC_SERVICE_GOLD_INFO_FINISH DBOFinish;
	DBOFinish.byMask = 1;

	g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_SERVICE_GOLD_INFO_FINISH,dwContextID,&DBOFinish,sizeof(DBOFinish));
#pragma endregion


	return true;
}


//�޸ĸ�������
bool CDataBaseEngineSink::On_DBR_Service_ModifyPersonalInfo(DWORD dwContextID, void * pData, WORD wDataSize)
{

	if (sizeof(STR_SUB_CL_SERVICE_MODIFY_PERSONAL_INFO) != wDataSize)
	{
		return false;
	}

	STR_SUB_CL_SERVICE_MODIFY_PERSONAL_INFO* pDbReq = (STR_SUB_CL_SERVICE_MODIFY_PERSONAL_INFO*)pData;

	//�������
	m_AccountsDB->ResetParameter();
	m_AccountsDB->AddParameter(TEXT("@UserID"), pDbReq->dwUserID);
	m_AccountsDB->AddParameter(TEXT("@OldLogonPassword"), pDbReq->szOldLogonPassword);
	m_AccountsDB->AddParameter(TEXT("@NewLogonPassword"), pDbReq->szNewLogonPassword);		
	m_AccountsDB->AddParameter(TEXT("@NickName"), pDbReq->szNickName);
	m_AccountsDB->AddParameter(TEXT("@Gender"), pDbReq->cbGender);
	m_AccountsDB->AddParameter(TEXT("@HeadImageUrl"), pDbReq->szHeadImageUrl);
	m_AccountsDB->AddParameter(TEXT("@Signature"), pDbReq->szSignature);
	m_AccountsDB->AddParameter(TEXT("@RealName"), pDbReq->szRealName);
	m_AccountsDB->AddParameter(TEXT("@IDCardNum"), pDbReq->szIDCardNum);
	m_AccountsDB->AddParameter(TEXT("@PhoneNum"), pDbReq->szPhoneNum);
	m_AccountsDB->AddParameter(TEXT("@ProxyUserID"), pDbReq->dwProxyUserID);		//TODO ����ID�����ݿ�����ʱδ���ӣ���������

	//�������
	WCHAR szDescribe[128] = L"";
	m_AccountsDB->AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

	//ִ�в�ѯ
	LONG lResultCode = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_Service_ModifyPersonalInfo"), true);

	//�������
	CDBVarValue DBVarValue;
	m_AccountsDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	//����
	On_DBO_Service_ModifyPersonalInfo(dwContextID, lResultCode, CW2CT(DBVarValue.bstrVal));


	return true;
}

//�޸ĸ������Ϸ���
bool CDataBaseEngineSink::On_DBO_Service_ModifyPersonalInfo(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString)
{
	//������Ϣ
	STR_DBO_CL_MODIFY_PERSONL_INFO PersonalInfo;
	ZeroMemory(&PersonalInfo, sizeof(PersonalInfo));

	//���ݸ�ֵ
	PersonalInfo.lResultCode = dwErrorCode;
	lstrcpyn(PersonalInfo.szDescribeString, pszErrorString, CountArray(PersonalInfo.szDescribeString));

	//�����ʲ�������Ϣ
	g_AttemperEngineSink->OnEventDataBaseResult(DBO_CL_SERVICE_MODIFY_PERSONAL_INFO, dwContextID, &PersonalInfo, sizeof(STR_DBO_CL_MODIFY_PERSONL_INFO));

	return true;
}

//��ֵ��Ϣ
bool CDataBaseEngineSink::On_DBR_Other_RechargeInfo( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//Ч�����
	ASSERT(wDataSize==sizeof(STR_DBR_CL_OTHER_RECHARGE_INFO));
	if (wDataSize!=sizeof(STR_DBR_CL_OTHER_RECHARGE_INFO)) return false;

	//������
	STR_DBR_CL_OTHER_RECHARGE_INFO * pUserRequest=(STR_DBR_CL_OTHER_RECHARGE_INFO *)pData;

	//�������
	m_AccountsDB->ResetParameter();

	//�������
	LONG lResultCode=m_AccountsDB->ExecuteProcess(TEXT("GSP_GP_UserRechangeInfo"),true);

	On_DBO_Other_RechargeInfo(dwContextID, lResultCode, TEXT("���ݿ⴦�����!"));


	return true;
}

//��ֵ��Ϣ����
bool CDataBaseEngineSink::On_DBO_Other_RechargeInfo(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString)
{
	//�������
	if (dwErrorCode == DB_SUCCESS)
	{
		//��������
		STR_DBO_CL_OTHER_RECHARGE_INFO RechangeInfo;
		ZeroMemory( &RechangeInfo, sizeof(RechangeInfo));

		RechangeInfo.dwMinMoney = m_AccountsDB->GetValue_LONGLONG(TEXT("MinMoney"));
		RechangeInfo.dwChangeScale = m_AccountsDB->GetValue_UINT(TEXT("ChangeScale"));

		//��������
		g_AttemperEngineSink->OnEventDataBaseResult(DBO_CL_USER_RECHARGE_INFO, dwContextID, &RechangeInfo, sizeof(RechangeInfo));
	}
	else
	{
		DBR_CommandSource wRequestID ;
		ZeroMemory(&wRequestID,sizeof(wRequestID));
		wRequestID.MainCommand = MDM_GIFT;
		wRequestID.SubCommand  = SUB_CL_OTHERS_RECHARGE_INFO;

		//������Ϣ
		On_DBO_CommonOperateResult(dwContextID, dwErrorCode, pszErrorString,wRequestID);
	}

	return true;
}

//�һ�������Ϣ
bool CDataBaseEngineSink::On_DBR_Other_ExchangeInfo( DWORD dwContextID, VOID * pData, WORD wDataSize )
{
	//Ч�����
	ASSERT(wDataSize==sizeof(STR_DBR_CL_OTHER_EXCHANGE_INFO));
	if (wDataSize!=sizeof(STR_DBR_CL_OTHER_EXCHANGE_INFO)) return false;

	//������
	STR_DBR_CL_OTHER_EXCHANGE_INFO * pUserRequest=(STR_DBR_CL_OTHER_EXCHANGE_INFO *)pData;

	//�������
	m_AccountsDB->ResetParameter();
	m_AccountsDB->AddParameter(TEXT("@UserID"),pUserRequest->dwUserID);

	//�������
	LONG lResultCode=m_AccountsDB->ExecuteProcess(TEXT("GSP_GP_UserExchangeInfo"),true);

	On_DBO_Other_ExchangeInfo(dwContextID, lResultCode, TEXT("���ݿ����!"));

	return true;
}

//�һ�������Ϣ����
bool CDataBaseEngineSink::On_DBO_Other_ExchangeInfo(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString)
{
	//�������
	if (dwErrorCode == DB_SUCCESS)
	{
		STR_DBO_CL_OTHER_EXCHANGE_INFO ExchangeInfo;
		ZeroMemory( &ExchangeInfo, sizeof(ExchangeInfo));

		ExchangeInfo.dwMinMoney = m_AccountsDB->GetValue_LONGLONG(TEXT("MinMoney"));
		ExchangeInfo.dwChangeScale = m_AccountsDB->GetValue_UINT(TEXT("ChangeScale"));
		ExchangeInfo.dwBankMoney = m_AccountsDB->GetValue_LONGLONG(TEXT("BankMoney"));
		ExchangeInfo.dwWithdrawals = m_AccountsDB->GetValue_LONGLONG(TEXT("Withdrawals"));

		//��������
		g_AttemperEngineSink->OnEventDataBaseResult(DBO_CL_USER_EXCHANGE_INFO,dwContextID,&ExchangeInfo,sizeof(ExchangeInfo));
	}
	else
	{
		//������
		DBR_CommandSource wRequestID ;
		ZeroMemory(&wRequestID,sizeof(wRequestID));
		wRequestID.MainCommand = MDM_GIFT;
		wRequestID.SubCommand  = SUB_CL_OTHERS_EXCHANGE_INFO;

		On_DBO_CommonOperateResult(dwContextID, dwErrorCode, TEXT("���ݿ����!"), wRequestID);
	}

	return true;
}

//��ѯ�û���ҷ���
bool CDataBaseEngineSink::On_DBR_QueryScoreInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(STR_DBR_CL_SERCIVR_QUERY_SCORE_INFO));
	if (wDataSize!=sizeof(STR_DBR_CL_SERCIVR_QUERY_SCORE_INFO)) return false;

	//������
	STR_DBR_CL_SERCIVR_QUERY_SCORE_INFO * pQueryScoreInfo =(STR_DBR_CL_SERCIVR_QUERY_SCORE_INFO *)pData;

	//�������
	m_TreasureDB->ResetParameter();
	m_TreasureDB->AddParameter(TEXT("@dwUserID"),pQueryScoreInfo->dwUserID);

	//ִ�в�ѯ
	LONG lResultCode=m_TreasureDB->ExecuteProcess(TEXT("GSP_CL_Service_GetUserScoreInfo"),true);

	//�������
	On_DBO_QueryScoreInfo(dwContextID,lResultCode,NULL,false);

	return true;

}

//��ѯ��ҷ�������
VOID CDataBaseEngineSink::On_DBO_QueryScoreInfo(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString, bool bMobileClient)
{
	//��������
	STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO UserScoreInfo;
	ZeroMemory(&UserScoreInfo,sizeof(UserScoreInfo));

	//������Ϣ
	UserScoreInfo.lResultCode = dwErrorCode;
	lstrcpyn(UserScoreInfo.szDescribeString, pszErrorString, CountArray(UserScoreInfo.szDescribeString));

	if (dwErrorCode==DB_SUCCESS)
	{
		//�������
		UserScoreInfo.dwUserID = m_TreasureDB->GetValue_DWORD(TEXT("UserID"));
		UserScoreInfo.lGold = m_TreasureDB->GetValue_LONGLONG(TEXT("Gold"));
		UserScoreInfo.lOpenRoomCard = m_TreasureDB->GetValue_LONGLONG(TEXT("OpenRoomCard"));
		UserScoreInfo.lDiamond = m_TreasureDB->GetValue_LONGLONG(TEXT("Diamond"));

		//���ͽ��
		g_AttemperEngineSink->OnEventDataBaseResult(DBO_CL_USER_QUERY_SCORE_INFO,dwContextID,&UserScoreInfo,sizeof(STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO));
	}
	else
	{
		//���ͽ��
		g_AttemperEngineSink->OnEventDataBaseResult(DBO_CL_USER_QUERY_SCORE_INFO,dwContextID,&UserScoreInfo,sizeof(STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO));
	}

	return;
}

//�������а��û���Ϣ
bool CDataBaseEngineSink::OnDBUpdateRankUserItem(DWORD dwContextID, void * pData, WORD wDataSize)
{
	if (sizeof(DBR_GP_UserRankItem) < wDataSize)
	{
		return false;
	}

	DBR_GP_UserRankItem* pDbReq = (DBR_GP_UserRankItem*)pData;

	m_AccountsDB->ResetParameter();

	m_AccountsDB->AddParameter(TEXT("@dwUserID"), pDbReq->dwUserID);
	m_AccountsDB->AddParameter(TEXT("@wRankNum"), pDbReq->byRankNum);
	m_AccountsDB->AddParameter(TEXT("@wReceived"), pDbReq->byReceived);
	m_AccountsDB->AddParameter(TEXT("@dwCount"), pDbReq->dwCount);
	m_AccountsDB->AddParameter(TEXT("@wIndex"), pDbReq->byIndex);

	//�������
	WCHAR szDescribe[128]=L"";
	m_AccountsDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

	//ִ�в�ѯ
	LONG lResultCode = m_AccountsDB->ExecuteProcess(TEXT("GSP_GP_UpdateRankUserItem"), true);
	//�������
	CDBVarValue DBVarValue;
	m_AccountsDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	if(lResultCode == DB_SUCCESS)
	{

	}

	return true;
}


//��ȡ���а���Ϣ
bool CDataBaseEngineSink::OnDBReadRankList(DWORD dwContextID, void * pData, WORD wDataSize)
{
	if (sizeof(RankManager) != wDataSize && NULL != pData)
	{
		return false;
	}

	RankManager* pRankManager = (RankManager*)pData;
	m_AccountsDB->ResetParameter();
	return true;
}

//��ȡ������
bool CDataBaseEngineSink::OnReceiveRankReward(DWORD dwContextID, void * pData, WORD wDataSize)
{
	if (sizeof(STR_DBR_CL_SERCIVR_GET_RANK_REWARD) < wDataSize)
	{
		return false;
	}

	STR_DBR_CL_SERCIVR_GET_RANK_REWARD* pDbReq = (STR_DBR_CL_SERCIVR_GET_RANK_REWARD*)pData;

	m_AccountsDB->ResetParameter();

	m_AccountsDB->AddParameter(TEXT("@dwUserID"), pDbReq->dwUserID);
	m_AccountsDB->AddParameter(TEXT("@byIndex"), pDbReq->byIndex);

	//�������
	WCHAR szDescribe[128]=L"";
	m_AccountsDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

	//ִ�в�ѯ
	LONG lResultCode = m_AccountsDB->ExecuteProcess(TEXT("GSP_GP_GetRankReward"), true);
	//�������
	CDBVarValue DBVarValue;
	m_AccountsDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	if(lResultCode == DB_SUCCESS)
	{
		//��ȡ�����û���ҷ�����ʯ
		STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO UserScoreInfo;
		ZeroMemory(&UserScoreInfo,sizeof(STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO));

		UserScoreInfo.dwUserID = pDbReq->dwUserID;
		UserScoreInfo.lGold = m_AccountsDB->GetValue_LONG(TEXT("@Gold"));
		UserScoreInfo.lOpenRoomCard = m_AccountsDB->GetValue_LONG(TEXT("@OpenRoomCard"));
		UserScoreInfo.lDiamond = m_AccountsDB->GetValue_LONG(TEXT("@Diamond"));

		//�����ʲ�������Ϣ
		g_AttemperEngineSink->OnEventDataBaseResult(DBO_CL_USER_QUERY_SCORE_INFO,dwContextID,&UserScoreInfo,sizeof(STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO));
	}

	//�յ�������
	STR_DBO_CL_SERCIVR_GET_TASK_REWARD result;
	ZeroMemory(&result,sizeof(result));
	result.lResultCode = lResultCode;
	lstrcpyn( result.szDescribe, CW2CT(DBVarValue.bstrVal), CountArray( result.szDescribe));

	g_AttemperEngineSink->OnEventDataBaseResult(DBO_CL_SERVICE_GET_RANK_REWARD,dwContextID,&result,sizeof(result));
	return true;
}

//�޸��û��Ƹ���Ϣ
bool CDataBaseEngineSink::OnModifyUserInsure(DWORD dwContextID, void * pData, WORD wDataSize)
{
	if (sizeof(DBR_GP_ModifyUserInsure) < wDataSize)
	{
		return false;
	}

	DBR_GP_ModifyUserInsure* pDbReq = (DBR_GP_ModifyUserInsure*)pData;

	m_TreasureDB->ResetParameter();

	m_TreasureDB->AddParameter(TEXT("@dwUserID"), pDbReq->dwUserID);
	m_TreasureDB->AddParameter(TEXT("@lGold"), pDbReq->lGold);
	m_TreasureDB->AddParameter(TEXT("@lOpenRoomCard"), pDbReq->lOpenRoomCard);
	m_TreasureDB->AddParameter(TEXT("@lDiamond"), pDbReq->lDiamond);
	m_TreasureDB->AddParameter(TEXT("@lRewardCard"), pDbReq->lRewardCard);
	m_TreasureDB->AddParameter(TEXT("@lScore"), pDbReq->lScore);

	//�������
	WCHAR szDescribe[128]=L"";
	m_TreasureDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

	//ִ�в�ѯ
	LONG lResultCode = m_TreasureDB->ExecuteProcess(TEXT("GSP_GP_GetRankReward"), true);
	//�������
	CDBVarValue DBVarValue;
	m_AccountsDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	if(lResultCode == DB_SUCCESS)
	{
		//��ȡ�����û���ҷ�����ʯ
		STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO UserScoreInfo;
		ZeroMemory(&UserScoreInfo,sizeof(STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO));

		UserScoreInfo.dwUserID = pDbReq->dwUserID;
		UserScoreInfo.lGold = m_AccountsDB->GetValue_LONG(TEXT("@Gold"));
		UserScoreInfo.lOpenRoomCard = m_AccountsDB->GetValue_LONG(TEXT("@OpenRoomCard"));
		UserScoreInfo.lDiamond = m_AccountsDB->GetValue_LONG(TEXT("@Diamond"));
		UserScoreInfo.lRewardCard = m_AccountsDB->GetValue_LONG(TEXT("@RewardCard"));
		UserScoreInfo.lScore = m_AccountsDB->GetValue_LONG(TEXT("@Score"));

		//���ͲƸ������Ϣ
		g_AttemperEngineSink->OnEventDataBaseResult(DBO_CL_USER_QUERY_SCORE_INFO,dwContextID,&UserScoreInfo,sizeof(STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO));
	}
	return true;
}

//�������û����͵���
bool CDataBaseEngineSink::On_DBR_CL_GIFT_GIVE_PROPS(DWORD dwContextID, void * pData, WORD wDataSize)
{
	if (sizeof(STR_SUB_CL_GIFT_GIVE_PROPS) != wDataSize)
	{
		return false;
	}

	STR_SUB_CL_GIFT_GIVE_PROPS* pDbReq = (STR_SUB_CL_GIFT_GIVE_PROPS*)pData;

	m_AccountsDB->ResetParameter();
	m_AccountsDB->AddParameter(TEXT("@dwSrcUserID"), pDbReq->dwSrcUserID);
	m_AccountsDB->AddParameter(TEXT("@dwDstUserID"), pDbReq->dwDstUserID);
	m_AccountsDB->AddParameter(TEXT("@dwPropsID"), pDbReq->dwPropsID);
	m_AccountsDB->AddParameter(TEXT("@dwCount"), pDbReq->dwCount);

	//�������
	TCHAR szDescribe[128]=TEXT("");
	m_AccountsDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

	//ִ�в�ѯ
	long lResultCode = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_GIFT_GIVE_PROPS"), true);

	//�������
	CDBVarValue DBVarValue;
	m_AccountsDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	STR_CMD_LC_GIFT_GIVE_PROPS CMD;
	CMD.dwResultCode = lResultCode;
	lstrcpyn( CMD.szDescribeString, CW2CT(DBVarValue.bstrVal), CountArray(CMD.szDescribeString));

	if(lResultCode == DB_SUCCESS)
	{
		CMD.dwPropsID = pDbReq->dwPropsID;
		CMD.dwCount = m_AccountsDB->GetValue_DWORD(TEXT("GoodsCount"));
		CMD.dwLoveness = m_AccountsDB->GetValue_DWORD(TEXT("Loveness"));

		STR_CMD_LC_GIFT_GIVE_PROPS_SHOW DBO;
		DBO.dwTargetID = pDbReq->dwDstUserID;
		lstrcpyn( DBO.szDescribeString, CW2CT(DBVarValue.bstrVal), CountArray(DBO.szDescribeString));

		//����DBO
		g_AttemperEngineSink->OnEventDataBaseResult(DBO_CL_GIFT_GIVE_PROPS_SHOW,dwContextID,&DBO,sizeof(DBO));
	}

	//����DBO
	g_AttemperEngineSink->OnEventDataBaseResult(DBO_CL_GIFT_GIVE_PROPS,dwContextID,&CMD,sizeof(CMD));
	return true;
}

#pragma region MDM_LOGON ��¼ģ��
/***************************** ��¼����*******************************************/
//�ʺŵ�¼
bool CDataBaseEngineSink::On_DBR_Logon_Accounts(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize!=sizeof(STR_DBR_CL_LOGON_ACCOUNTS)) return false;

	//������
	STR_DBR_CL_LOGON_ACCOUNTS * pDBRLogonAccounts=(STR_DBR_CL_LOGON_ACCOUNTS *)pData;

	//�������
	m_AccountsDB->ResetParameter();
	m_AccountsDB->AddParameter(TEXT("@mystery"),_MYSTERY);
	m_AccountsDB->AddParameter(TEXT("@strAccounts"),pDBRLogonAccounts->szAccounts);
	m_AccountsDB->AddParameter(TEXT("@strPassword"),pDBRLogonAccounts->szPassword);
	m_AccountsDB->AddParameter(TEXT("@strClientIP"),TEXT("")); //TODONOW ɾ�������ַ
	m_AccountsDB->AddParameter(TEXT("@strMachineID"),pDBRLogonAccounts->szMachineID);
	m_AccountsDB->AddParameter(TEXT("@ProxyID"),pDBRLogonAccounts->dwProxyID);

	//�������
	TCHAR szDescribeString[128]=TEXT("");
	m_AccountsDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	//ִ�в�ѯ
	LONG lResultCode = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_Logon_Accounts"),true);

	//�������
	CDBVarValue DBVarValue;
	m_AccountsDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	On_DBO_Logon_Accounts(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal));
	return true;
}

//�˺ŵ�¼����
bool CDataBaseEngineSink::On_DBO_Logon_Accounts(DWORD dwContextID, DWORD dwResultCode, LPCTSTR pszErrorString)
{
	//��������
	STR_CMD_LC_LOGON_PLATFORM DBOLogonAccount;
	ZeroMemory(&DBOLogonAccount,sizeof(DBOLogonAccount));

	//��¼�ɹ���ȡ��Ϣ
	if(DB_SUCCESS == dwResultCode)
	{
		/* *****************************    �û���Ϣ     ****************************/
		//�û���־
		DBOLogonAccount.dwUserID=m_AccountsDB->GetValue_DWORD(TEXT("UserID"));
		//�û��ǳ�
		m_AccountsDB->GetValue_String(TEXT("NickName"),DBOLogonAccount.szNickName,CountArray(DBOLogonAccount.szNickName));
		//�û��Ա�
		DBOLogonAccount.cbGender=m_AccountsDB->GetValue_BYTE(TEXT("Gender"));
		//ͷ������
		m_AccountsDB->GetValue_String(TEXT("HeadUrl"),DBOLogonAccount.szHeadUrl,CountArray(DBOLogonAccount.szHeadUrl));
		//����ǩ��
		m_AccountsDB->GetValue_String(TEXT("MySignature"),DBOLogonAccount.szMySignature,CountArray(DBOLogonAccount.szMySignature));

		//�û�Ԫ��
		DBOLogonAccount.dwUserDiamond=m_AccountsDB->GetValue_LONGLONG(TEXT("UserDiamond"));
		//�û�����
		DBOLogonAccount.dwUserMedal=m_AccountsDB->GetValue_DWORD(TEXT("UserMedal"));
		//������ֵ
		DBOLogonAccount.byLevel=m_AccountsDB->GetValue_BYTE(TEXT("UserLevel"));
		//�û�����
		DBOLogonAccount.dwLoveLiness=m_AccountsDB->GetValue_DWORD(TEXT("LoveLiness"));

		//����Ա�ȼ�
		DBOLogonAccount.cbMasterOrder=m_AccountsDB->GetValue_BYTE(TEXT("MasterOrder"));
		//��Ա�ȼ�
		DBOLogonAccount.cbMemberOrder=m_AccountsDB->GetValue_BYTE(TEXT("MemberOrder"));
		//��Ա����ʱ��
		m_AccountsDB->GetValue_SystemTime(TEXT("MemberOverDate"),DBOLogonAccount.MemberOverDate);

		//��ʵ����
		m_AccountsDB->GetValue_String(TEXT("IdentityName"),DBOLogonAccount.szIdentityName,CountArray(DBOLogonAccount.szIdentityName));
		//���֤��
		m_AccountsDB->GetValue_String(TEXT("IdentityNum"),DBOLogonAccount.szIdentityNum,CountArray(DBOLogonAccount.szIdentityNum));
		//�ֻ�����
		m_AccountsDB->GetValue_String(TEXT("MobilePhone"),DBOLogonAccount.szMobilePhone,CountArray(DBOLogonAccount.szMobilePhone));
		/* *****************************    �˺���Ϣ     ****************************/
		//����¼��ַ
		m_AccountsDB->GetValue_String(TEXT("LastLogonIP"),DBOLogonAccount.szLasLogonIp,CountArray(DBOLogonAccount.szLasLogonIp));
		//�������ʱ�� 
		m_AccountsDB->GetValue_SystemTime(TEXT("LastLogonDate"),DBOLogonAccount.LasLogonDate);

		/* *****************************    ��������     ****************************/
		//�û�����
		DBOLogonAccount.lUserScore = m_AccountsDB->GetValue_LONGLONG(TEXT("UserScore"));
		//�û���Ϸ��
		DBOLogonAccount.lUserGold = m_AccountsDB->GetValue_LONGLONG(TEXT("UserGold"));
		//�û�����
		DBOLogonAccount.lOpenRoomCard = m_AccountsDB->GetValue_LONGLONG(TEXT("UserRoomCard"));
	}

	//��������
	DBOLogonAccount.dwResultCode=dwResultCode;
	lstrcpyn(DBOLogonAccount.szDescribeString,pszErrorString,CountArray(DBOLogonAccount.szDescribeString));

	//���ͽ��
	g_AttemperEngineSink->OnEventDataBaseResult(DBO_CL_LOGON_ACCOUNTS,dwContextID,&DBOLogonAccount, sizeof(DBOLogonAccount));

	return true;
}

//ע�ᴦ��
bool CDataBaseEngineSink::On_DBR_Logon_Register(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize!=sizeof(STR_DBR_CL_LOGON_REGISTER)) return false;

	//������
	STR_DBR_CL_LOGON_REGISTER * pDBRLogonRegister=(STR_DBR_CL_LOGON_REGISTER *)pData;

	//�������
	m_AccountsDB->ResetParameter();
	m_AccountsDB->AddParameter(TEXT("@byMYSTERY"), _MYSTERY);
	m_AccountsDB->AddParameter(TEXT("@strAccounts"),pDBRLogonRegister->szAccounts);
	m_AccountsDB->AddParameter(TEXT("@strPassword"),pDBRLogonRegister->szPassword);
	m_AccountsDB->AddParameter(TEXT("@strNickName"),pDBRLogonRegister->szNickName);
	m_AccountsDB->AddParameter(TEXT("@cbGender"),pDBRLogonRegister->cbGender);
	m_AccountsDB->AddParameter(TEXT("@strClientIP"),TEXT(""));
	m_AccountsDB->AddParameter(TEXT("@strMobilePhone"),pDBRLogonRegister->strMobilePhone);
	m_AccountsDB->AddParameter(TEXT("@strMachineID"),pDBRLogonRegister->szMachineID);
	m_AccountsDB->AddParameter(TEXT("@dwProxyID"),pDBRLogonRegister->dwProxyID);

	//�������
	TCHAR szDescribeString[128]=TEXT("");
	m_AccountsDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	//ִ�в�ѯ
	LONG lResultCode=m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_Logon_Register"),true);

	//�������
	CDBVarValue DBVarValue;
	m_AccountsDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
	On_DBO_Logon_Accounts(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal));

	return true;
}

//ƽ̨��¼
bool CDataBaseEngineSink::On_DBR_Logon_Platform(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(STR_DBR_CL_LOGON_PLATFORM));
	if (wDataSize!=sizeof(STR_DBR_CL_LOGON_PLATFORM)) return false;

	//������
	STR_DBR_CL_LOGON_PLATFORM * pDBRLogonPlatform=(STR_DBR_CL_LOGON_PLATFORM *)pData;

	//�������
	m_AccountsDB->ResetParameter();
	m_AccountsDB->AddParameter(TEXT("@byMystery"), _MYSTERY);
	m_AccountsDB->AddParameter(TEXT("@strOpenID"), pDBRLogonPlatform->szOpenID);
	m_AccountsDB->AddParameter(TEXT("@szUnionID"), pDBRLogonPlatform->szUnionID);
	m_AccountsDB->AddParameter(TEXT("@strNickName"), pDBRLogonPlatform->szNickName);
	m_AccountsDB->AddParameter(TEXT("@cbGender"), pDBRLogonPlatform->cbGender);
	m_AccountsDB->AddParameter(TEXT("@strHeadUrl"), pDBRLogonPlatform->strHeadUrl);
	m_AccountsDB->AddParameter(TEXT("@strMachineID"), pDBRLogonPlatform->szMachineID);
	m_AccountsDB->AddParameter(TEXT("@strClientIP"), TEXT(""));
	m_AccountsDB->AddParameter(TEXT("@dwProxyID"), pDBRLogonPlatform->dwProxyID);

	//�������
	TCHAR szDescribeString[128]=TEXT("");
	m_AccountsDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	//ִ�в�ѯ			
	LONG lResultCode = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_Logon_Platform"),true);

	//�������
	CDBVarValue DBVarValue;
	m_AccountsDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	On_DBO_Logon_Platform(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal));

	return true;
}

//ƽ̨��¼����
bool CDataBaseEngineSink::On_DBO_Logon_Platform(DWORD dwContextID, DWORD dwResultCode, LPCTSTR pszErrorString)
{
	//��������
	STR_CMD_LC_LOGON_PLATFORM DBOLogonPlatform;
	ZeroMemory(&DBOLogonPlatform,sizeof(DBOLogonPlatform));

	//��¼�ɹ���ȡ��Ϣ
	if(DB_SUCCESS == dwResultCode)
	{
		/* *****************************    �û���Ϣ     ****************************/
		//�û���־
		DBOLogonPlatform.dwUserID=m_AccountsDB->GetValue_DWORD(TEXT("UserID"));
		//�û��ǳ�
		m_AccountsDB->GetValue_String(TEXT("NickName"),DBOLogonPlatform.szNickName,CountArray(DBOLogonPlatform.szNickName));
		//�û��Ա�
		DBOLogonPlatform.cbGender=m_AccountsDB->GetValue_BYTE(TEXT("Gender"));
		//ͷ������
		m_AccountsDB->GetValue_String(TEXT("HeadUrl"),DBOLogonPlatform.szHeadUrl,CountArray(DBOLogonPlatform.szHeadUrl));
		//����ǩ��
		m_AccountsDB->GetValue_String(TEXT("MySignature"),DBOLogonPlatform.szMySignature,CountArray(DBOLogonPlatform.szMySignature));

		//�û�Ԫ��
		DBOLogonPlatform.dwUserDiamond=m_AccountsDB->GetValue_LONGLONG(TEXT("UserDiamond"));
		//�û�����
		DBOLogonPlatform.dwUserMedal=m_AccountsDB->GetValue_DWORD(TEXT("UserMedal"));
		//������ֵ
		DBOLogonPlatform.byLevel=m_AccountsDB->GetValue_BYTE(TEXT("UserLevel"));
		//�û�����
		DBOLogonPlatform.dwLoveLiness=m_AccountsDB->GetValue_DWORD(TEXT("LoveLiness"));

		//����Ա�ȼ�
		DBOLogonPlatform.cbMasterOrder=m_AccountsDB->GetValue_BYTE(TEXT("MasterOrder"));
		//��Ա�ȼ�
		DBOLogonPlatform.cbMemberOrder=m_AccountsDB->GetValue_BYTE(TEXT("MemberOrder"));
		//��Ա����ʱ��
		m_AccountsDB->GetValue_SystemTime(TEXT("MemberOverDate"),DBOLogonPlatform.MemberOverDate);

		//��ʵ����
		m_AccountsDB->GetValue_String(TEXT("IdentityName"),DBOLogonPlatform.szIdentityName,CountArray(DBOLogonPlatform.szIdentityName));
		//���֤��
		m_AccountsDB->GetValue_String(TEXT("IdentityNum"),DBOLogonPlatform.szIdentityNum,CountArray(DBOLogonPlatform.szIdentityNum));
		//�ֻ�����
		m_AccountsDB->GetValue_String(TEXT("MobilePhone"),DBOLogonPlatform.szMobilePhone,CountArray(DBOLogonPlatform.szMobilePhone));
		/* *****************************    �˺���Ϣ     ****************************/
		//����¼��ַ
		m_AccountsDB->GetValue_String(TEXT("LastLogonIP"),DBOLogonPlatform.szLasLogonIp,CountArray(DBOLogonPlatform.szLasLogonIp));
		//�������ʱ��
		m_AccountsDB->GetValue_SystemTime(TEXT("LastLogonDate"),DBOLogonPlatform.LasLogonDate);

		/* *****************************    ��������     ****************************/
		//�û����� TODONOW ���ݿ�ȱ�ٸ��ֶ�
		//DBOLogonPlatform.lUserScore = m_AccountsDB->GetValue_LONGLONG(TEXT("UserScore"));
		//�û���Ϸ��
		DBOLogonPlatform.lUserGold = m_AccountsDB->GetValue_LONGLONG(TEXT("UserGold"));
		//�û�����
		DBOLogonPlatform.lOpenRoomCard = m_AccountsDB->GetValue_LONGLONG(TEXT("OpenRoomCard"));
	}

	//��������
	DBOLogonPlatform.dwResultCode=dwResultCode;
	lstrcpyn(DBOLogonPlatform.szDescribeString,pszErrorString,CountArray(DBOLogonPlatform.szDescribeString));

	g_AttemperEngineSink->OnEventDataBaseResult(DBO_CL_LOGON_PLATFORM,dwContextID,&DBOLogonPlatform,sizeof(DBOLogonPlatform));
	return true;
}

#pragma endregion
//����Ϣ��6-������Ϣ�����ò������
VOID CDataBaseEngineSink::On_DBO_CommonOperateResult( DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString, DBR_CommandSource wRequestID )
{
	//��������
	DBO_GP_OperateCommand OperateResult;
	ZeroMemory(&OperateResult,sizeof(OperateResult));

	//��������
	OperateResult.lResultCode=dwErrorCode;
	lstrcpyn(OperateResult.szDescribeString,pszErrorString,CountArray(OperateResult.szDescribeString));
	OperateResult.mCommand.MainCommand = wRequestID.MainCommand;
	OperateResult.mCommand.SubCommand = wRequestID.SubCommand;

	//���ͽ��
	WORD wDataSize=CountStringBuffer(OperateResult.szDescribeString);
	WORD wHeadSize=sizeof(OperateResult)-sizeof(OperateResult.szDescribeString);
	g_AttemperEngineSink->OnEventDataBaseResult(DBO_CL_USER_COMMAND_RESULT,dwContextID,&OperateResult,wHeadSize+wDataSize);

	return;
}
#pragma region MDM_CLUB ����Ȧ(�汾2)
//��ѯ����Ȧ�б�
bool CDataBaseEngineSink::On_DBR_CL_CLUB_ALL_CLUB_INFO_LIST(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize!=sizeof(STR_SUB_CL_CLUB_ALL_CLUB_INFO_LIST)) return false;
	STR_SUB_CL_CLUB_ALL_CLUB_INFO_LIST *pDbr = (STR_SUB_CL_CLUB_ALL_CLUB_INFO_LIST *)pData;

	//��������
	m_AccountsDB->ResetParameter();

	m_AccountsDB->AddParameter(TEXT("@UserID"),pDbr->dwUserID);
	m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_CLUB_ALL_CLUB_INFO_LIST"),true);

	//�б���
	WORD wPacketSize=0;
	BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
	wPacketSize=0;
	STR_CMD_LC_CLUB_ALL_CLUB_INFO_LIST * pCMD=NULL;
	while (m_AccountsDB->IsRecordsetEnd()==false)
	{
		//������Ϣ
		if ((wPacketSize+sizeof(STR_CMD_LC_CLUB_ALL_CLUB_INFO_LIST))>sizeof(cbBuffer))
		{
			g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_ALL_CLUB_INFO_LIST,dwContextID,cbBuffer,wPacketSize);
			wPacketSize=0;
		}

		//��ȡ��Ϣ
		pCMD=(STR_CMD_LC_CLUB_ALL_CLUB_INFO_LIST *)(cbBuffer+wPacketSize);
		pCMD->dwClubID=m_AccountsDB->GetValue_DWORD(TEXT("ClubID"));	
		m_AccountsDB->GetValue_String(TEXT("ClubName"),pCMD->szClubName,CountArray(pCMD->szClubName));
		pCMD->byClubRole=m_AccountsDB->GetValue_BYTE(TEXT("ClubRole"));

		//����λ��
		wPacketSize+=sizeof(STR_CMD_LC_CLUB_ALL_CLUB_INFO_LIST);

		//�ƶ���¼
		m_AccountsDB->MoveToNext();
	}
	if (wPacketSize>0) g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_ALL_CLUB_INFO_LIST,dwContextID,cbBuffer,wPacketSize);

	g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_ALL_INFO_FINISH,dwContextID,NULL,0);

	return true;
}

//��ѯָ������Ȧ�����б�
bool CDataBaseEngineSink::On_DBR_CL_CLUB_ROOM_LIST(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize!=sizeof(STR_DBR_CL_CLUB_ROOM_LIST)) return false;

	STR_DBR_CL_CLUB_ROOM_LIST *pDBR = (STR_DBR_CL_CLUB_ROOM_LIST*) pData;

	STR_DBR_CL_CLUB_ROOM_LIST DBR;
	DBR.dwClubID = pDBR->dwClubID;

	//��������
	m_AccountsDB->ResetParameter();

	//�������
	m_AccountsDB->AddParameter(TEXT("@ClubID"),DBR.dwClubID);

	LONG lResultCode = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_CLUB_ROOM_LIST"),true);

	//�б���
	//��������
	WORD wPacketSize=0;
	BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
	wPacketSize=0;
	STR_CMD_LC_CLUB_ROOM_LIST * pDBO=NULL;
	while ((lResultCode == DB_SUCCESS) && (m_AccountsDB->IsRecordsetEnd()==false))
	{
		//������Ϣ
		if ((wPacketSize+sizeof(STR_CMD_LC_CLUB_ROOM_LIST))>sizeof(cbBuffer))
		{
			g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_ROOM_LIST,dwContextID,cbBuffer,wPacketSize);
			wPacketSize=0;
		}

		//��ȡ��Ϣ
		pDBO=(STR_CMD_LC_CLUB_ROOM_LIST *)(cbBuffer+wPacketSize);
		pDBO->dwRoomID=m_AccountsDB->GetValue_DWORD(TEXT("RoomID"));
		DWORD dwGameID =m_AccountsDB->GetValue_DWORD(TEXT("GameID"));
		pDBO->dwKindID = (dwGameID >> 16);

		BYTE byMode = m_AccountsDB->GetValue_BYTE(TEXT("ModeID"));
		pDBO->byGoldOrFK = 1;
		if(0 == byMode) //������  �Ǿ��ֲ��еķ�����
		{
			pDBO->byGoldOrFK = 1;
		}
		else if(3 == byMode) //������ҳ�  �Ǿ��ֲ��н�ҳ�
		{
			pDBO->byGoldOrFK = 2;
		}

		m_AccountsDB->GetValue_String(TEXT("KindName"),pDBO->szKindName,CountArray(pDBO->szKindName));
		m_AccountsDB->GetValue_String(TEXT("RoomName"),pDBO->szRoomName,CountArray(pDBO->szRoomName));

		pDBO->wPlayerNum=m_AccountsDB->GetValue_WORD(TEXT("PlayerNum"));
		m_AccountsDB->GetValue_String(TEXT("RoomRule"),pDBO->szRoomRule,CountArray(pDBO->szRoomRule));
		pDBO->byAllRound=m_AccountsDB->GetValue_BYTE(TEXT("AllRound"));
		pDBO->byChairNum=m_AccountsDB->GetValue_BYTE(TEXT("ChairNum"));

		pDBO->bDissolve=m_AccountsDB->GetValue_BYTE(TEXT("DissolveMinute"));
		pDBO->dwDissolveTime=m_AccountsDB->GetValue_DWORD(TEXT("DissolveMinute"));

		pDBO->dwAmount=m_AccountsDB->GetValue_DWORD(TEXT("ServiceGold"));
		pDBO->dwOwnerPercentage=m_AccountsDB->GetValue_DWORD(TEXT("Revenue"));

		pDBO->byMask=m_AccountsDB->GetValue_BYTE(TEXT("Mask"));
		pDBO->dwDizhu=m_AccountsDB->GetValue_DWORD(TEXT("Dizhu"));
		pDBO->dwGold=m_AccountsDB->GetValue_DWORD(TEXT("Gold"));

		//����λ��
		wPacketSize+=sizeof(STR_CMD_LC_CLUB_ROOM_LIST);

		//�ƶ���¼
		m_AccountsDB->MoveToNext();
	}
	if (wPacketSize>0) g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_ROOM_LIST,dwContextID,cbBuffer,wPacketSize);

	g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_ROOM_LIST_FINISH,dwContextID,NULL,0);

	return true;
}

//��ѯδ��Ա, �������Ȧ(���9��)
bool CDataBaseEngineSink::On_DBR_CL_CLUB_RANDOM_CLUB_LIST(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize!=sizeof(STR_SUB_CL_CLUB_RANDOM_CLUB_LIST)) return false;
	STR_SUB_CL_CLUB_RANDOM_CLUB_LIST *pDBR = (STR_SUB_CL_CLUB_RANDOM_CLUB_LIST*) pData;

	//��������
	m_AccountsDB->ResetParameter();
	//�������
	m_AccountsDB->AddParameter(TEXT("@Mystery"),_MYSTERY);
	m_AccountsDB->AddParameter(TEXT("@UserID"),pDBR->dwUserID);

	LONG lResultCode = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_CLUB_RANDOM_CLUB_LIST"),true);

	//�б���
	WORD wPacketSize=0;
	BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
	wPacketSize=0;
	STR_CMD_LC_CLUB_RANDOM_CLUB_LIST * pDBO=NULL;
	while (m_AccountsDB->IsRecordsetEnd()==false)
	{
		//������Ϣ
		if ((wPacketSize+sizeof(STR_CMD_LC_CLUB_RANDOM_CLUB_LIST))>sizeof(cbBuffer))
		{
			g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_RANDOM_CLUB_LIST,dwContextID,cbBuffer,wPacketSize);
			wPacketSize=0;
		}

		//��ȡ��Ϣ
		pDBO=(STR_CMD_LC_CLUB_RANDOM_CLUB_LIST *)(cbBuffer+wPacketSize);
		pDBO->dwClubID = m_AccountsDB->GetValue_DWORD(TEXT("ClubID"));
		m_AccountsDB->GetValue_String(TEXT("ClubName"),pDBO->szClubName,CountArray(pDBO->szClubName));
		pDBO->dwMajorKindID=m_AccountsDB->GetValue_DWORD(TEXT("MajorKindID"));
		//TODONOW ManiorKindID ��Ҫת��
		//KINDID
		pDBO->byClubLevel=m_AccountsDB->GetValue_BYTE(TEXT("ClubLevel"));
		pDBO->wMemberNum=m_AccountsDB->GetValue_WORD(TEXT("MemberNum"));
		m_AccountsDB->GetValue_String(TEXT("Notice"),pDBO->szNotice,CountArray(pDBO->szNotice));
		m_AccountsDB->GetValue_String(TEXT("Message"),pDBO->szMessage,CountArray(pDBO->szMessage));
		pDBO->dwClubOwner=m_AccountsDB->GetValue_DWORD(TEXT("ClubOwner"));
		m_AccountsDB->GetValue_String(TEXT("ClubOwnerName"),pDBO->szClubOwnerName,CountArray(pDBO->szClubOwnerName));

		//����λ��
		wPacketSize+=sizeof(STR_CMD_LC_CLUB_RANDOM_CLUB_LIST);

		//�ƶ���¼
		m_AccountsDB->MoveToNext();
	}
	if (wPacketSize>0) g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_RANDOM_CLUB_LIST,dwContextID,cbBuffer,wPacketSize);

	g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_RANDOM_CLUB_LIST_FINISH,dwContextID,NULL,0);

	return true;
}

//�����������Ȧ
bool CDataBaseEngineSink::On_DBR_CL_CLUB_JOIN_CLUB(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize != sizeof(STR_SUB_CL_CLUB_JOIN_CLUB)) return false;
	STR_SUB_CL_CLUB_JOIN_CLUB *pDbr = (STR_SUB_CL_CLUB_JOIN_CLUB*)pData;

	//��������
	m_AccountsDB->ResetParameter();
	//�������
	m_AccountsDB->AddParameter(TEXT("@Mystery"),_MYSTERY);
	m_AccountsDB->AddParameter(TEXT("@UserID"),pDbr->dwUserID);
	m_AccountsDB->AddParameter(TEXT("@ClubID"),pDbr->dwClubID);
	m_AccountsDB->AddParameter(TEXT("@Note"),pDbr->szNote);

	//�������
	TCHAR szDescribeString[128]=TEXT("");
	m_AccountsDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	LONG lResultCode = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_CLUB_JOIN_CLUB"),true);

	//�������
	CDBVarValue DBVarValue;
	m_AccountsDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	//�ṹ�幹��
	STR_CMD_LC_CLUB_JOIN_CLUB CMD;
	ZeroMemory(&CMD, sizeof(CMD));
	CMD.lResultCode = lResultCode;
	lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));

	g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_JOIN_CLUB,dwContextID,&CMD,sizeof(CMD));


	/* �����������Ȧ �㲥 */
	if(lResultCode == DB_SUCCESS)
	{
		STR_SUB_CL_CLUB_JOIN_CLUB_BDCAST CMD2;
		CMD2.dwClubID = pDbr->dwClubID;
		CMD2.dwApplicantNum = m_AccountsDB->GetValue_DWORD(TEXT("ApplicantNum"));

		//������Զ�������ֲ�, ��֪ͨ�ͻ���ˢ��
		BYTE byMask = m_AccountsDB->GetValue_BYTE(TEXT("mask"));
		if(1 == byMask )
		{
			STR_CMD_LC_CLUB_LIST_RE CMD3;
			CMD3.byMask = 1;
			g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_JOIN_CLUB_RE,dwContextID,&CMD3,sizeof(CMD3));
		}

		g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_JOIN_CLUB_BDCAST,dwContextID,&CMD2,sizeof(CMD2));
	}
	return true;
}

//����Ȧ����
bool CDataBaseEngineSink::On_DBR_CL_CLUB_NOTICE(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize != sizeof(STR_SUB_CL_CLUB_NOTICE)) return false;
	STR_SUB_CL_CLUB_NOTICE* pDbr = (STR_SUB_CL_CLUB_NOTICE*) pData;

	//��������
	m_AccountsDB->ResetParameter();

	//�������
	m_AccountsDB->AddParameter(TEXT("UserID"),pDbr->dwUserID);
	m_AccountsDB->AddParameter(TEXT("ClubID"),pDbr->dwClubID);
	m_AccountsDB->AddParameter(TEXT("Notice"),pDbr->szNotice);

	//�������
	TCHAR szDescribeString[128]=TEXT("");
	m_AccountsDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	LONG lResultCode = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_CLUB_NOTICE"),true);

	//�������
	CDBVarValue DBVarValue;
	m_AccountsDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	//�ṹ�幹��
	STR_CMD_LC_CLUB_NOTICE CMD;
	ZeroMemory(&CMD, sizeof(CMD));
	CMD.lResultCode = lResultCode;
	lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));

	g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_NOTICE,dwContextID,&CMD,sizeof(CMD));
	return true;
}

//����Ȧ���
bool CDataBaseEngineSink::On_DBR_CL_CLUB_MESSAGE(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize != sizeof(STR_SUB_CL_CLUB_MESSAGE)) return false;
	STR_SUB_CL_CLUB_MESSAGE* pDbr = (STR_SUB_CL_CLUB_MESSAGE*) pData;

	//��������
	m_AccountsDB->ResetParameter();

	//�������
	m_AccountsDB->AddParameter(TEXT("UserID"),pDbr->dwUserID);
	m_AccountsDB->AddParameter(TEXT("ClubID"),pDbr->dwClubID);
	m_AccountsDB->AddParameter(TEXT("Message"),pDbr->szMessage);

	//�������
	TCHAR szDescribeString[128]=TEXT("");
	m_AccountsDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	LONG lResultCode = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_CLUB_MESSAGE"),true);

	//�������
	CDBVarValue DBVarValue;
	m_AccountsDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	//�ṹ�幹��
	STR_CMD_LC_CLUB_MESSAGE CMD;
	ZeroMemory(&CMD, sizeof(CMD));
	CMD.lResultCode = lResultCode;
	lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));

	g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_MESSAGE,dwContextID,&CMD,sizeof(CMD));
	return true;
}

//���׷���
bool CDataBaseEngineSink::On_DBR_CL_CLUB_CONTRIBUTE_FK(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize != sizeof(STR_SUB_CL_CLUB_CONTRIBUTE_FK)) return false;
	STR_SUB_CL_CLUB_CONTRIBUTE_FK* pDbr = (STR_SUB_CL_CLUB_CONTRIBUTE_FK*) pData;

	//��������
	m_AccountsDB->ResetParameter();

	//�������
	m_AccountsDB->AddParameter(TEXT("UserID"),pDbr->dwUserID);
	m_AccountsDB->AddParameter(TEXT("ClubID"),pDbr->dwClubID);
	m_AccountsDB->AddParameter(TEXT("FK"),pDbr->dwFK);

	//�������
	TCHAR szDescribeString[128]=TEXT("");
	m_AccountsDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	LONG lResultCode = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_CLUB_CONTRIBUTE_FK"),true);

	//�������
	CDBVarValue DBVarValue;
	m_AccountsDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	//�ṹ�幹��
	STR_CMD_LC_CLUB_CONTRIBUTE_FK CMD;
	ZeroMemory(&CMD, sizeof(CMD));
	CMD.lResultCode = lResultCode;
	lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));
	CMD.dwClubFK=m_AccountsDB->GetValue_DWORD(TEXT("ClubFK"));

	g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_CONTRIBUTE_FK,dwContextID,&CMD,sizeof(CMD));
	return true;
}

//����Ȧ����
bool CDataBaseEngineSink::On_DBR_CL_CLUB_AUTO_AGREE(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize != sizeof(STR_SUB_CL_CLUB_AUTO_AGREE)) return false;
	STR_SUB_CL_CLUB_AUTO_AGREE* pDbr = (STR_SUB_CL_CLUB_AUTO_AGREE*) pData;

	//��������
	m_AccountsDB->ResetParameter();

	//�������
	m_AccountsDB->AddParameter(TEXT("UserID"),pDbr->dwUserID);
	m_AccountsDB->AddParameter(TEXT("ClubID"),pDbr->dwClubID);
	m_AccountsDB->AddParameter(TEXT("AutoAgree"),pDbr->byAutoAgree);
	m_AccountsDB->AddParameter(TEXT("Sex"),pDbr->bySex);
	m_AccountsDB->AddParameter(TEXT("Level"),pDbr->wLevel);

	//�������
	TCHAR szDescribeString[128]=TEXT("");
	m_AccountsDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	LONG lResultCode = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_CLUB_AUTO_AGREE"),true);

	//�������
	CDBVarValue DBVarValue;
	m_AccountsDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	//�ṹ�幹��
	STR_CMD_LC_CLUB_AUTO_AGREE CMD;
	ZeroMemory(&CMD, sizeof(CMD));
	CMD.lResultCode = lResultCode;
	lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));

	g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_AUTO_AGREE,dwContextID,&CMD,sizeof(CMD));
	return true;
}

//�������˼�������Ȧ
bool CDataBaseEngineSink::On_DBR_CL_CLUB_INVITE(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize != sizeof(STR_SUB_CL_CLUB_INVITE)) return false;
	STR_SUB_CL_CLUB_INVITE* pDbr = (STR_SUB_CL_CLUB_INVITE*) pData;

	//��������
	m_AccountsDB->ResetParameter();

	//�������
	m_AccountsDB->AddParameter(TEXT("Mystery"),_MYSTERY);
	m_AccountsDB->AddParameter(TEXT("UserID"),pDbr->dwUserID);
	m_AccountsDB->AddParameter(TEXT("ClubID"),pDbr->dwClubID);
	m_AccountsDB->AddParameter(TEXT("TagID"),pDbr->dwTagID);

	//�������
	TCHAR szDescribeString[128]=TEXT("");
	m_AccountsDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	LONG lResultCode = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_CLUB_INVITE"),true);

	//�������
	CDBVarValue DBVarValue;
	m_AccountsDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	//�ṹ�幹��
	STR_CMD_LC_CLUB_INVITE CMD;
	ZeroMemory(&CMD, sizeof(CMD));
	CMD.lResultCode = lResultCode;
	lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));

	g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_INVITE,dwContextID,&CMD,sizeof(CMD));


	/* �������˵����� */
	STR_CMD_LC_CLUB_INVITE_REMIND CMD2;
	CMD2.dwTagID = pDbr->dwTagID;
	g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_INVITE_REMIND,dwContextID,&CMD2,sizeof(CMD2));

	return true;
}

//�������˻ظ�
bool CDataBaseEngineSink::On_DBR_CL_CLUB_INVITE_RESULT(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize != sizeof(STR_SUB_CL_CLUB_INVITE_RESULT)) return false;
	STR_SUB_CL_CLUB_INVITE_RESULT* pDbr = (STR_SUB_CL_CLUB_INVITE_RESULT*) pData;

	//��������
	m_AccountsDB->ResetParameter();

	//�������
	m_AccountsDB->AddParameter(TEXT("UserID"),pDbr->dwUserID);
	m_AccountsDB->AddParameter(TEXT("ClubID"),pDbr->dwClubID);
	m_AccountsDB->AddParameter(TEXT("Agree"),pDbr->byAgree);

	//�������
	TCHAR szDescribeString[128]=TEXT("");
	m_AccountsDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	LONG lResultCode = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_CLUB_INVITE_RESULT"),true);

	//�������
	CDBVarValue DBVarValue;
	m_AccountsDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	//�ṹ�幹��
	STR_CMD_LC_CLUB_INVITE_RESULT CMD;
	ZeroMemory(&CMD, sizeof(CMD));
	CMD.lResultCode = lResultCode;
	lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));

	g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_INVITE_RESULT,dwContextID,&CMD,sizeof(CMD));

	return true;
}

//�������˲鿴�Լ��������б�
bool CDataBaseEngineSink::On_DBR_CL_CLUB_INQUERY_LIST(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize != sizeof(STR_SUB_CL_CLUB_INQUERY_LIST)) return false;
	STR_SUB_CL_CLUB_INQUERY_LIST* pDbr = (STR_SUB_CL_CLUB_INQUERY_LIST*) pData;

	//��������
	m_AccountsDB->ResetParameter();

	//�������
	m_AccountsDB->AddParameter(TEXT("UserID"),pDbr->dwUserID);

	LONG lResultCode = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_CLUB_INQUERY_LIST"),true);

	//�����б�
	WORD wPacketSize=0;
	BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
	wPacketSize=0;
	STR_CMD_LC_CLUB_INQUERY_LIST * pDBO=NULL;
	while (m_AccountsDB->IsRecordsetEnd()==false)
	{
		//������Ϣ
		if ((wPacketSize+sizeof(STR_CMD_LC_CLUB_INQUERY_LIST))>sizeof(cbBuffer))
		{
			g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_INQUERY_LIST,dwContextID,cbBuffer,wPacketSize);
			wPacketSize=0;
		}

		//��ȡ��Ϣ
		pDBO=(STR_CMD_LC_CLUB_INQUERY_LIST *)(cbBuffer+wPacketSize);
		pDBO->dwClubID = m_AccountsDB->GetValue_DWORD(TEXT("ClubID"));
		m_AccountsDB->GetValue_String(TEXT("ClubName"),pDBO->szClubName,CountArray(pDBO->szClubName));
		pDBO->dwMajorKindID=m_AccountsDB->GetValue_DWORD(TEXT("MajorKindID"));
		//TODONOW ManiorKindID ��Ҫת��
		//KINDID
		pDBO->byClubLevel=m_AccountsDB->GetValue_BYTE(TEXT("ClubLevel"));
		pDBO->wMemberNum=m_AccountsDB->GetValue_WORD(TEXT("MemberNum"));
		m_AccountsDB->GetValue_String(TEXT("Notice"),pDBO->szNotice,CountArray(pDBO->szNotice));
		m_AccountsDB->GetValue_String(TEXT("Message"),pDBO->szMessage,CountArray(pDBO->szMessage));
		pDBO->dwClubOwner=m_AccountsDB->GetValue_DWORD(TEXT("ClubOwner"));
		m_AccountsDB->GetValue_String(TEXT("ClubOwnerName"),pDBO->szClubOwnerName,CountArray(pDBO->szClubOwnerName));

		//����λ��
		wPacketSize+=sizeof(STR_CMD_LC_CLUB_INQUERY_LIST);

		//�ƶ���¼
		m_AccountsDB->MoveToNext();
	}
	if (wPacketSize>0) g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_INQUERY_LIST,dwContextID,cbBuffer,wPacketSize);

	g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_INQUERY_LIST_FINISH,dwContextID,NULL,0);
	return true;
}

//�������б�
bool CDataBaseEngineSink::On_DBR_CL_CLUB_APPLICANT_LIST(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize != sizeof(STR_SUB_CL_CLUB_APPLICANT_LIST)) return false;
	STR_SUB_CL_CLUB_APPLICANT_LIST* pDbr = (STR_SUB_CL_CLUB_APPLICANT_LIST*) pData;

	//��������
	m_AccountsDB->ResetParameter();

	//�������
	m_AccountsDB->AddParameter(TEXT("UserID"),pDbr->dwUserID);
	m_AccountsDB->AddParameter(TEXT("ClubID"),pDbr->dwClubID);

	LONG lResultCode = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_CLUB_APPLICANT_LIST"),true);

	//�����б�
	WORD wPacketSize=0;
	BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
	wPacketSize=0;
	STR_CMD_LC_CLUB_APPLICANT_LIST * pDBO=NULL;
	while (m_AccountsDB->IsRecordsetEnd()==false)
	{
		//������Ϣ
		if ((wPacketSize+sizeof(STR_CMD_LC_CLUB_APPLICANT_LIST))>sizeof(cbBuffer))
		{
			g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_APPLICANT_LIST,dwContextID,cbBuffer,wPacketSize);
			wPacketSize=0;
		}

		//��ȡ��Ϣ
		pDBO=(STR_CMD_LC_CLUB_APPLICANT_LIST *)(cbBuffer+wPacketSize);
		pDBO->dwUserID = m_AccountsDB->GetValue_DWORD(TEXT("UserID"));
		m_AccountsDB->GetValue_String(TEXT("UserName"),pDBO->szUserName,CountArray(pDBO->szUserName));
		pDBO->byUserSex=m_AccountsDB->GetValue_BYTE(TEXT("UserSex"));
		pDBO->byUserLevel=m_AccountsDB->GetValue_BYTE(TEXT("UserLevel"));
		m_AccountsDB->GetValue_String(TEXT("Note"),pDBO->szNote,CountArray(pDBO->szNote));
		m_AccountsDB->GetValue_String(TEXT("HeadUrl"),pDBO->szHeadUrl,CountArray(pDBO->szHeadUrl));

		//����λ��
		wPacketSize+=sizeof(STR_CMD_LC_CLUB_APPLICANT_LIST);

		//�ƶ���¼
		m_AccountsDB->MoveToNext();
	}
	if (wPacketSize>0) g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_APPLICANT_LIST,dwContextID,cbBuffer,wPacketSize);

	g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_APPLICANT_LIST_FINISH,dwContextID,NULL,0);
	return true;
}

//ְ������
bool CDataBaseEngineSink::On_DBR_CL_CLUB_APPOINTMENT(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize != sizeof(STR_SUB_CL_CLUB_APPOINTMENT)) return false;
	STR_SUB_CL_CLUB_APPOINTMENT* pDbr = (STR_SUB_CL_CLUB_APPOINTMENT*) pData;

	//��������
	m_AccountsDB->ResetParameter();

	//�������
	m_AccountsDB->AddParameter(TEXT("UserID"),pDbr->dwUserID);
	m_AccountsDB->AddParameter(TEXT("ClubID"),pDbr->dwClubID);
	m_AccountsDB->AddParameter(TEXT("TagID"),pDbr->dwTagID);
	m_AccountsDB->AddParameter(TEXT("Action"),pDbr->byAction);

	//�������
	TCHAR szDescribeString[128]=TEXT("");
	m_AccountsDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	LONG lResultCode = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_CLUB_APPOINTMENT"),true);

	//�������
	CDBVarValue DBVarValue;
	m_AccountsDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	//���ͷ���
	STR_CMD_LC_CLUB_APPOINTMENT CMD;
	ZeroMemory(&CMD, sizeof(CMD));
	CMD.lResultCode = lResultCode;
	lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));
	g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_APPOINTMENT,dwContextID,&CMD,sizeof(CMD));

	//��������
	STR_CMD_LC_CLUB_APPOINTMENT_NOTE CMD2;
	CMD2.byAction = pDbr->byAction;
	CMD2.dwUserID = pDbr->dwTagID;
	lstrcpyn(CMD2.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD2.szDescribe));
	g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_APPOINTMENT_NOTE,dwContextID,&CMD2,sizeof(CMD2));

	return true;
}

//����ս��ͳ��
bool CDataBaseEngineSink::On_DBR_CL_CLUB_RECORD_LIST(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize != sizeof(STR_SUB_CL_CLUB_RECORD_LIST)) return false;
	STR_SUB_CL_CLUB_RECORD_LIST *pDbr = (STR_SUB_CL_CLUB_RECORD_LIST * )pData;

	//��������
	m_AccountsDB->ResetParameter();
	m_AccountsDB->AddParameter(TEXT("UserID"),pDbr->dwUserID);
	m_AccountsDB->AddParameter(TEXT("dwClubID"),pDbr->dwClubID);
	m_AccountsDB->AddParameter(TEXT("tmQueryStartData"),pDbr->tmQueryStartData);
	m_AccountsDB->AddParameter(TEXT("tmQueryEndData"),pDbr->tmQueryEndData);

	LONG lResultCode2 = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_CLUB_RECORD_LIST"),true);

	//�����б�
	WORD wPacketSize=0;
	BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
	wPacketSize=0;
	STR_CMD_LC_CLUB_RECORD_LIST * pDBO=NULL;
	while ( (lResultCode2 == DB_SUCCESS) && (m_AccountsDB->IsRecordsetEnd()==false))
	{
		//������Ϣ
		if ((wPacketSize+sizeof(STR_CMD_LC_CLUB_RECORD_LIST))>sizeof(cbBuffer))
		{
			g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_RECORD_LIST,dwContextID,cbBuffer,wPacketSize);
			wPacketSize=0;
		}

		//��ȡ��Ϣ
		pDBO=(STR_CMD_LC_CLUB_RECORD_LIST *)(cbBuffer+wPacketSize);
		BYTE gameMode  = m_AccountsDB->GetValue_BYTE(TEXT("ModeID"));
		//0����  1����  2��� 3�������
		if(0 == gameMode)
		{
			pDBO->byMask = 1;
		}
		else if( (2 == gameMode) || (3 == gameMode ))
		{
			pDBO->byMask = 2;
		}

		pDBO->dwUserID=m_AccountsDB->GetValue_DWORD(TEXT("UserID"));

		m_AccountsDB->GetValue_String(TEXT("NickName"),pDBO->szNickName,CountArray(pDBO->szNickName));
		m_AccountsDB->GetValue_String(TEXT("HeadUrl"),pDBO->szHeadUrl,CountArray(pDBO->szHeadUrl));

		pDBO->dwAllCount = m_AccountsDB->GetValue_DWORD(TEXT("AllCount"));
		pDBO->lAllScore=m_AccountsDB->GetValue_LONGLONG(TEXT("AllScore"));
		pDBO->dwWinCount = m_AccountsDB->GetValue_DWORD(TEXT("WinCount"));
		pDBO->lWinScore=m_AccountsDB->GetValue_LONGLONG(TEXT("WinScore"));

		//����λ��
		wPacketSize+=sizeof(STR_CMD_LC_CLUB_RECORD_LIST);

		//�ƶ���¼
		m_AccountsDB->MoveToNext();
	}
	if (wPacketSize>0) g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_RECORD_LIST,dwContextID,cbBuffer,wPacketSize);

	//���ͽ���
	g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_RECORD_FINISH,dwContextID,NULL,0);

	return true;
}

//����Ȧ����
bool CDataBaseEngineSink::On_DBR_CL_CLUB_CHAT(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize != sizeof(STR_SUB_CL_CLUB_CHAT)) return false;
	STR_SUB_CL_CLUB_CHAT *pDbr = (STR_SUB_CL_CLUB_CHAT * )pData;

	/* 1. ��ȡ�����ߵ���Ϣ */
	TCHAR szTempUserNickName[32] = TEXT("");
	TCHAR szTempHeadUrl[256] = TEXT("");

	//��������
	m_AccountsDB->ResetParameter();
	m_AccountsDB->AddParameter(TEXT("UserID"),pDbr->dwUserID);

	LONG lResultCode2 = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_CLUB_CHAT_GETINFO"),true);

	if(lResultCode2 != DB_SUCCESS)
	{
		return true;
	}
	m_AccountsDB->GetValue_String(TEXT("UserNickName"), szTempUserNickName,CountArray(szTempUserNickName));
	m_AccountsDB->GetValue_String(TEXT("HeadUrl"),szTempHeadUrl,CountArray(szTempHeadUrl));

	/* 2.���͸����ֲ��������ߵ��� */
	//��������
	m_AccountsDB->ResetParameter();
	m_AccountsDB->AddParameter(TEXT("UserID"),pDbr->dwUserID);
	m_AccountsDB->AddParameter(TEXT("ClubID"),pDbr->dwClubID);

	LONG lResultCode = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_CLUB_CHAT"),true);

	//�����б�
	WORD wPacketSize=0;
	BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
	wPacketSize=0;
	STR_CMD_LC_CLUB_CHAT_BDCAST * pDBO=NULL;

	while (m_AccountsDB->IsRecordsetEnd()==false && lResultCode == DB_SUCCESS)
	{
		//������Ϣ
		if ((wPacketSize+sizeof(STR_CMD_LC_CLUB_CHAT_BDCAST))>sizeof(cbBuffer))
		{
			g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CHAT_ALL_BDCAST,dwContextID,cbBuffer,wPacketSize);
			wPacketSize=0;
		}

		//��ȡ��Ϣ
		pDBO=(STR_CMD_LC_CLUB_CHAT_BDCAST *)(cbBuffer+wPacketSize);
		pDBO->dwTagID = m_AccountsDB->GetValue_DWORD(TEXT("UserID"));

		//���췢���˵���Ϣ
		pDBO->byChatMode = pDbr->byChatMode;
		pDBO->dwClubID = pDbr->dwClubID;
		memcpy(pDBO->szChat, pDbr->szChat, sizeof(pDBO->szChat));
		pDBO->dwUserID = pDbr->dwUserID;
		memcpy(pDBO->szUserNickName, szTempUserNickName, sizeof(pDBO->szUserNickName));
		memcpy(pDBO->szHeadUrl, szTempHeadUrl, sizeof(pDBO->szHeadUrl));

		//����λ��
		wPacketSize+=sizeof(STR_CMD_LC_CLUB_CHAT_BDCAST);

		//�ƶ���¼
		m_AccountsDB->MoveToNext();
	}
	if (wPacketSize>0) g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CHAT_ALL_BDCAST,dwContextID,cbBuffer,wPacketSize);

	STR_CMD_LC_CLUB_CHAT CMD2;
	CMD2.byChatMode = pDbr->byChatMode;
	CMD2.lResultCode = lResultCode;
	memcpy(CMD2.szDescribe, TEXT("����ʧ��, �Ժ�����"), sizeof(CMD2));
	g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CHAT_ALL,dwContextID,&CMD2, sizeof(CMD2));
	return true;
}

//��������
bool CDataBaseEngineSink::On_DBR_CL_WORD_CHAT(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize != sizeof(STR_SUB_CL_CLUB_CHAT)) return false;
	STR_SUB_CL_CLUB_CHAT *pDbr = (STR_SUB_CL_CLUB_CHAT * )pData;

	/* 1. ��ȡ�����ߵ���Ϣ */
	TCHAR szTempUserNickName[32] = TEXT("");
	TCHAR szTempHeadUrl[256] = TEXT("");

	//��������
	m_AccountsDB->ResetParameter();
	m_AccountsDB->AddParameter(TEXT("UserID"),pDbr->dwUserID);

	LONG lResultCode2 = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_CLUB_CHAT_GETINFO"),true);

	if(lResultCode2 != DB_SUCCESS)
	{
		return true;
	}
	m_AccountsDB->GetValue_String(TEXT("UserNickName"), szTempUserNickName,CountArray(szTempUserNickName));
	m_AccountsDB->GetValue_String(TEXT("HeadUrl"),szTempHeadUrl,CountArray(szTempHeadUrl));

	/* 2. ����㲥 ���͸��õ�¼���������ߵ��� */
	STR_CMD_LC_CLUB_CHAT_BDCAST CMD3;
	CMD3.byChatMode = pDbr->byChatMode;
	CMD3.dwClubID = 0;
	CMD3.dwUserID = pDbr->dwUserID;
	memcpy(CMD3.szUserNickName, szTempUserNickName, sizeof(CMD3.szUserNickName));
	memcpy(CMD3.szHeadUrl, szTempHeadUrl, sizeof(CMD3.szHeadUrl));
	memcpy(CMD3.szChat, pDbr->szChat, sizeof(CMD3.szChat));
	g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CHAT_ALL_BDCAST,dwContextID,&CMD3, sizeof(CMD3));

	/* 3. ���췵�� */
	STR_CMD_LC_CLUB_CHAT CMD2;
	CMD2.byChatMode = pDbr->byChatMode;
	CMD2.lResultCode = lResultCode2;
	memcpy(CMD2.szDescribe, TEXT("����ʧ��, �Ժ�����"), sizeof(CMD2));
	g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CHAT_ALL,dwContextID,&CMD2, sizeof(CMD2));
	return true;
}

//ϵͳ���� -- TODONOW ��ʱ��������߼�
bool CDataBaseEngineSink::On_DBR_CL_SYSTEM_CHAT(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize != sizeof(STR_SUB_CL_CLUB_CHAT)) return false;
	STR_SUB_CL_CLUB_CHAT *pDbr = (STR_SUB_CL_CLUB_CHAT * )pData;

	/* 1. ��ȡ�����ߵ���Ϣ */
	TCHAR szTempUserNickName[32] = TEXT("");
	TCHAR szTempHeadUrl[256] = TEXT("");

	//��������
	m_AccountsDB->ResetParameter();
	m_AccountsDB->AddParameter(TEXT("UserID"),pDbr->dwUserID);

	LONG lResultCode2 = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_CLUB_CHAT_GETINFO"),true);

	if(lResultCode2 != DB_SUCCESS)
	{
		return true;
	}
	m_AccountsDB->GetValue_String(TEXT("UserNickName"), szTempUserNickName,CountArray(szTempUserNickName));
	m_AccountsDB->GetValue_String(TEXT("HeadUrl"),szTempHeadUrl,CountArray(szTempHeadUrl));


	/* 2. ����㲥 ���͸��õ�¼���������ߵ��� */
	/*
	STR_CMD_LC_CLUB_CHAT_BDCAST CMD3;
	CMD3.byChatMode = pDbr->byChatMode;
	CMD3.dwClubID = 0;
	CMD3.dwUserID = pDbr->dwUserID;
	memcpy(CMD3.szUserNickName, szTempUserNickName, sizeof(CMD3.szUserNickName));
	memcpy(CMD3.szHeadUrl, szTempHeadUrl, sizeof(CMD3.szHeadUrl));
	memcpy(CMD3.szChat, pDbr->szChat, sizeof(CMD3.szChat));
	g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_WORD_CHAT_BDCAST,dwContextID,&CMD3, sizeof(CMD3));
	*/

	/* 3. ���췵�� */
	STR_CMD_LC_CLUB_CHAT CMD2;
	CMD2.byChatMode = pDbr->byChatMode;
	CMD2.lResultCode = lResultCode2;
	memcpy(CMD2.szDescribe, TEXT("����ʧ��, �Ժ�����"), sizeof(CMD2));
	g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CHAT_ALL,dwContextID,&CMD2, sizeof(CMD2));

	return true;
}

//˽������
bool CDataBaseEngineSink::On_DBR_CL_SECRET_CHAT(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize != sizeof(STR_SUB_CL_CLUB_CHAT)) return false;
	STR_SUB_CL_CLUB_CHAT *pDbr = (STR_SUB_CL_CLUB_CHAT * )pData;

	/* 1. ��ȡ�����ߵ���Ϣ */
	TCHAR szTempUserNickName[32] = TEXT("");
	TCHAR szTempHeadUrl[256] = TEXT("");

	//��������
	m_AccountsDB->ResetParameter();
	m_AccountsDB->AddParameter(TEXT("UserID"),pDbr->dwUserID);

	LONG lResultCode2 = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_CLUB_CHAT_GETINFO"),true);

	if(lResultCode2 != DB_SUCCESS)
	{
		return true;
	}
	m_AccountsDB->GetValue_String(TEXT("UserNickName"), szTempUserNickName,CountArray(szTempUserNickName));
	m_AccountsDB->GetValue_String(TEXT("HeadUrl"),szTempHeadUrl,CountArray(szTempHeadUrl));

	/* 2. ����㲥 ���͸��õ�¼���������ߵ��� */
	STR_CMD_LC_CLUB_CHAT_BDCAST CMD3;
	CMD3.byChatMode = pDbr->byChatMode;
	CMD3.dwClubID = 0;
	CMD3.dwUserID = pDbr->dwUserID;
	CMD3.dwTagID = pDbr->dwTagUserID;
	memcpy(CMD3.szUserNickName, szTempUserNickName, sizeof(CMD3.szUserNickName));
	memcpy(CMD3.szHeadUrl, szTempHeadUrl, sizeof(CMD3.szHeadUrl));
	memcpy(CMD3.szChat, pDbr->szChat, sizeof(CMD3.szChat));
	g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CHAT_ALL_BDCAST,dwContextID,&CMD3, sizeof(CMD3));


	/* 3. ���췵�� */
	STR_CMD_LC_CLUB_CHAT CMD2;
	CMD2.byChatMode = pDbr->byChatMode;
	CMD2.lResultCode = lResultCode2;
	memcpy(CMD2.szDescribe, TEXT("����ʧ��, �Ժ�����"), sizeof(CMD2));
	g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CHAT_ALL,dwContextID,&CMD2, sizeof(CMD2));

	return true;
}

//����Ȧ�ö�
bool CDataBaseEngineSink::On_DBR_CL_STICKY_POST(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize != sizeof(STR_DBR_CL_CLUB_STICKY_POST)) return false;
	STR_DBR_CL_CLUB_STICKY_POST *pDbr = (STR_DBR_CL_CLUB_STICKY_POST * )pData;

	//��������
	m_AccountsDB->ResetParameter();
	m_AccountsDB->AddParameter(TEXT("dwUserID"),pDbr->dwUserID);
	m_AccountsDB->AddParameter(TEXT("dwClubID"),pDbr->dwClubID);
	m_AccountsDB->AddParameter(TEXT("dwStickyMark"),pDbr->cbTopFlag);

	LONG lResultCode2 = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_CLUB_STICK"),true);

	//����DBO����
	STR_DBO_LC_CLUB_STICKY_POST DBO;
	ZeroMemory(&DBO, sizeof(STR_DBO_LC_CLUB_STICKY_POST));
	g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_STICKY_POST, dwContextID, &DBO, sizeof(DBO));

	return true;
}

//����뿪���ֲ�����
bool CDataBaseEngineSink::On_DBR_CL_CLUB_ROOM_USER_LEAVE(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize != sizeof(STR_SUB_CL_CLUB_ROOM_USER_LEAVE)) return false;
	STR_SUB_CL_CLUB_ROOM_USER_LEAVE* pDbr = (STR_SUB_CL_CLUB_ROOM_USER_LEAVE*) pData;

	//��������
	m_AccountsDB->ResetParameter();

	//�������
	m_AccountsDB->AddParameter(TEXT("UserID"),pDbr->dwUserID);
	m_AccountsDB->AddParameter(TEXT("RoomID"),pDbr->dwClubRoomID);

	LONG lResultCode = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_CLUB_ROOM_USER_LEAVE"),true);

	return true;
}

//��������
bool CDataBaseEngineSink::On_DBR_CL_CLUB_ROOM_SETTING(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize != sizeof(STR_SUB_CL_CLUB_ROOM_SETTING)) return false;
	STR_SUB_CL_CLUB_ROOM_SETTING* pDbr = (STR_SUB_CL_CLUB_ROOM_SETTING*) pData;

#pragma region 1. ��ȡ֮ǰ�ķ������, ���ҹ����µķ������
	//���ݿ⴫�����
	m_AccountsDB->ResetParameter();
	m_AccountsDB->AddParameter(TEXT("@dwClubRoomID"), pDbr->dwRoomID);

	//ִ�в�ѯ
	LONG lResultCode1 = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_CLUB_ROOM_SETTING_QUERY_INFO"), true);

	if(lResultCode1 != DB_SUCCESS)
	{
		return false ;
	}

	//��ȡ������
	TCHAR szRealRoomRule[2048];
	m_AccountsDB->GetValue_String(TEXT("RealRoomRule"), szRealRoomRule, sizeof(szRealRoomRule));

	//�����µķ�������ַ���
	STR_SUB_CG_USER_CREATE_ROOM strCreateRoom;
	StrToBin(szRealRoomRule, strCreateRoom.CommonRule, 0, 255);
	StrToBin(szRealRoomRule, strCreateRoom.SubGameRule, 256, 512);

	tagTableRule *pCfg = (tagTableRule*)strCreateRoom.CommonRule;
	pCfg->bDissolve = pDbr->bDissolve;
	pCfg->dwDissolveTime = pDbr->dwDissolveTime;
	pCfg->byMask = pDbr->byMask;
	pCfg->dwAmount = pDbr->dwAmount;
	pCfg->dwOwnerPercentage = 	pDbr->dwOwnerPercentage;
	pCfg->dwDizhu = pDbr->dwDizhu;
	pCfg->CellScore = pDbr->dwDizhu;
	pCfg->dwLevelGold = pDbr->dwGold;

	//�޸ķ������� -- ����������SubGameRule��
	struct tagSubRule
	{
		TCHAR	szRoomNote[40]; //�������˵��
		TCHAR	szRoomName[16];	//��������
	};
	tagSubRule *pSub = (tagSubRule*)strCreateRoom.SubGameRule;
	memcpy(pSub->szRoomName, pDbr->szRoomName, sizeof(pSub->szRoomName));

	//16���Ƶķ������ 256���ֽ�
	CString strCommonRoomRule = toHexString(strCreateRoom.CommonRule, 128);
	CString strSubRoomRule = toHexString(strCreateRoom.SubGameRule, 127); //�������һ���ַ�
	CString strRealRoomRule = strCommonRoomRule + strSubRoomRule;


#pragma endregion

#pragma region 2. �µ����� ���ݸ����ݿ�
	//��������
	m_AccountsDB->ResetParameter();

	//�������
	m_AccountsDB->AddParameter(TEXT("UserID"),pDbr->dwUserID);
	m_AccountsDB->AddParameter(TEXT("RoomID"),pDbr->dwRoomID);	

	//��ֵת��
	BYTE byModeID = 0;
	if(1 == pDbr->byGoldOrFK)//���ֲ��ķ����� ������ͨ������
	{
		byModeID  = 0;
	}
	else if( 2 == pDbr->byGoldOrFK)//���ֲ��Ľ�ҳ� ��ʵ�� ������ҳ�
	{
		byModeID  = 3;
	}

	m_AccountsDB->AddParameter(TEXT("GoldOrFK"),byModeID);
	m_AccountsDB->AddParameter(TEXT("Dissolve"),pDbr->bDissolve);
	m_AccountsDB->AddParameter(TEXT("DissolveTime"),pDbr->dwDissolveTime);
	m_AccountsDB->AddParameter(TEXT("Amount"),pDbr->dwAmount);
	m_AccountsDB->AddParameter(TEXT("OwnerPercentage"),pDbr->dwOwnerPercentage);
	m_AccountsDB->AddParameter(TEXT("dwDizhu"),pDbr->dwDizhu);
	m_AccountsDB->AddParameter(TEXT("dwGold"),pDbr->dwGold);
	m_AccountsDB->AddParameter(TEXT("byMask"),pDbr->byMask);
	m_AccountsDB->AddParameter(TEXT("RoomName"),pDbr->szRoomName);
	m_AccountsDB->AddParameter(TEXT("RealRoomRule"),strRealRoomRule);

	//�������
	TCHAR szDescribeString[128]=TEXT("");
	m_AccountsDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	LONG lResultCode = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_CLUB_ROOM_SETTING"),true);

	//�������
	CDBVarValue DBVarValue;
	m_AccountsDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	//�ṹ�幹��
	STR_CMD_LC_CLUB_ROOM_SETTING CMD;
	ZeroMemory(&CMD, sizeof(CMD));
	CMD.lResultCode = lResultCode;
	lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));

	CMD.dwRoomID=m_AccountsDB->GetValue_DWORD(TEXT("RoomID"));

	//��ֵת��
	BYTE byResultModeID = m_AccountsDB->GetValue_BYTE(TEXT("ModeID"));;
	if(0 == byResultModeID)//���ֲ��ķ����� ������ͨ������
	{
		CMD.byGoldOrFK  = 1;
	}
	else if( 3 == byResultModeID)//���ֲ��Ľ�ҳ� ��ʵ�� ������ҳ�
	{
		CMD.byGoldOrFK  = 2;
	}

	CMD.dwDissolveTime=m_AccountsDB->GetValue_DWORD(TEXT("DissolveMinute"));
	CMD.byDissolve= m_AccountsDB->GetValue_BYTE(TEXT("DissolveMinute"));
	CMD.dwAmount=m_AccountsDB->GetValue_DWORD(TEXT("ServiceGold"));
	CMD.dwOwnerPercentage=m_AccountsDB->GetValue_DWORD(TEXT("Revenue"));
	CMD.byMask = pDbr->byMask;
	CMD.dwDizhu = pDbr->dwDizhu;
	CMD.dwGold = pDbr->dwGold;
	m_AccountsDB->GetValue_String(TEXT("RoomName"), CMD.szRoomName,CountArray(CMD.szRoomName));

	g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_ROOM_SETTING,dwContextID,&CMD,sizeof(CMD));
#pragma endregion
	return true;
}

//���󷿼�����
bool CDataBaseEngineSink::On_DBR_CL_CLUB_ROOM_QUERY_SETTING(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize != sizeof(STR_SUB_CL_CLUB_ROOM_QUERY_SETTING)) return false;
	STR_SUB_CL_CLUB_ROOM_QUERY_SETTING* pDbr = (STR_SUB_CL_CLUB_ROOM_QUERY_SETTING*) pData;

	//��������
	m_AccountsDB->ResetParameter();

	//�������
	m_AccountsDB->AddParameter(TEXT("UserID"),pDbr->dwUserID);
	m_AccountsDB->AddParameter(TEXT("RoomID"),pDbr->dwRoomID);	

	//�������
	TCHAR szDescribeString[128]=TEXT("");
	m_AccountsDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	LONG lResultCode = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_CLUB_ROOM_QUERY_SETTING"),true);

	//�������
	CDBVarValue DBVarValue;
	m_AccountsDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	//�ṹ�幹��
	STR_CMD_LC_CLUB_ROOM_QUERY_SETTING CMD;
	ZeroMemory(&CMD, sizeof(CMD));
	CMD.lResultCode = lResultCode;
	lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));

	CMD.dwRoomID=m_AccountsDB->GetValue_DWORD(TEXT("RoomID"));

	//��ֵת��
	BYTE byResultModeID = m_AccountsDB->GetValue_BYTE(TEXT("ModeID"));;
	if(0 == byResultModeID)//���ֲ��ķ����� ������ͨ������
	{
		CMD.byGoldOrFK  = 1;
	}
	else if( 3 == byResultModeID)//���ֲ��Ľ�ҳ� ��ʵ�� ������ҳ�
	{
		CMD.byGoldOrFK  = 2;
	}

	CMD.dwDissolveTime=m_AccountsDB->GetValue_DWORD(TEXT("DissolveMinute"));
	CMD.byDissolve= m_AccountsDB->GetValue_BYTE(TEXT("DissolveMinute"));
	CMD.dwAmount=m_AccountsDB->GetValue_DWORD(TEXT("ServiceGold"));
	CMD.dwOwnerPercentage=m_AccountsDB->GetValue_DWORD(TEXT("Revenue"));
	CMD.byMask = m_AccountsDB->GetValue_BYTE(TEXT("Mask"));
	CMD.dwDizhu = m_AccountsDB->GetValue_DWORD(TEXT("Dizhu"));
	CMD.dwGold = m_AccountsDB->GetValue_DWORD(TEXT("Gold"));
	m_AccountsDB->GetValue_String(TEXT("RoomName"), CMD.szRoomName,CountArray(CMD.szRoomName));

	g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_ROOM_QUERY_SETTING,dwContextID,&CMD,sizeof(CMD));
	return true;
}


//��ɢ��������
bool CDataBaseEngineSink::On_DBR_CL_CLUB_ROOM_DISSOLVE(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize != sizeof(STR_SUB_CL_CLUB_ROOM_DISSOLVE)) return false;
	STR_SUB_CL_CLUB_ROOM_DISSOLVE* pDbr = (STR_SUB_CL_CLUB_ROOM_DISSOLVE*) pData;

	//��������
	m_AccountsDB->ResetParameter();

	//�������
	m_AccountsDB->AddParameter(TEXT("UserID"),pDbr->dwUserID);
	m_AccountsDB->AddParameter(TEXT("ClubID"),pDbr->dwClubID);
	m_AccountsDB->AddParameter(TEXT("RoomID"),pDbr->dwClubRoomID);

	//�������
	TCHAR szDescribeString[128]=TEXT("");
	m_AccountsDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	LONG lResultCode = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_CLUB_ROOM_DISSOLVE"),true);

	//�������
	CDBVarValue DBVarValue;
	m_AccountsDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	//�ṹ�幹��
	STR_CMD_LC_CLUB_ROOM_DISSOLVE CMD;
	ZeroMemory(&CMD, sizeof(CMD));
	CMD.lResultCode = lResultCode;
	lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));

	g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_ROOM_DISSOLVE,dwContextID,&CMD,sizeof(CMD));
	return true;
}

//��ɢ��������
bool CDataBaseEngineSink::On_DBR_CL_CLUB_TABLE_DISSOLVE(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize != sizeof(STR_SUB_CL_CLUB_TABLE_DISSOLVE)) return false;
	STR_SUB_CL_CLUB_TABLE_DISSOLVE* pDbr = (STR_SUB_CL_CLUB_TABLE_DISSOLVE*) pData;

	//��������
	m_AccountsDB->ResetParameter();

	//�������
	m_AccountsDB->AddParameter(TEXT("UserID"),pDbr->dwUserID);
	m_AccountsDB->AddParameter(TEXT("ClubID"),pDbr->dwClubID);
	m_AccountsDB->AddParameter(TEXT("RoomID"),pDbr->dwClubRoomID);
	m_AccountsDB->AddParameter(TEXT("TableID"),pDbr->dwTableID);

	//�������
	TCHAR szDescribeString[128]=TEXT("");
	m_AccountsDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	LONG lResultCode = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_CLUB_TABLE_DISSOLVE"),true);

	if(lResultCode == DB_SUCCESS)
	{
		//���ؽṹ���ʼ��
		STR_CMD_LC_CLUB_TABLE_DISSOLVE CMD;
		ZeroMemory(&CMD, sizeof(CMD));

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));

		CMD.byMask = m_AccountsDB->GetValue_BYTE(TEXT("Mask"));
		CMD.dwGameID = m_AccountsDB->GetValue_DWORD(TEXT("GameID"));
		CMD.dwTableID = pDbr->dwTableID;

		CMD.lResultCode = lResultCode;

		g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_TABLE_DISSOLVE,dwContextID,&CMD,sizeof(CMD));

		return true;
	}

	return true;
}

//��ɢ����Ȧ
bool CDataBaseEngineSink::On_DBR_CL_CLUB_DISS_CLUB(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize != sizeof(STR_SUB_CL_CLUB_DISS_CLUB)) return false;
	STR_SUB_CL_CLUB_DISS_CLUB* pDbr = (STR_SUB_CL_CLUB_DISS_CLUB*) pData;

	//��������
	m_AccountsDB->ResetParameter();

	//�������
	m_AccountsDB->AddParameter(TEXT("UserID"),pDbr->dwUserID);
	m_AccountsDB->AddParameter(TEXT("ClubID"),pDbr->dwClubID);

	//�������
	TCHAR szDescribeString[128]=TEXT("");
	m_AccountsDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	LONG lResultCode = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_CLUB_DISS_CLUB"),true);

	//�������
	CDBVarValue DBVarValue;
	m_AccountsDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	//�ṹ�幹��
	STR_CMD_LC_CLUB_DISS_CLUB CMD;
	ZeroMemory(&CMD, sizeof(CMD));
	CMD.lResultCode = lResultCode;
	lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));

	g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_DISS_CLUB,dwContextID,&CMD,sizeof(CMD));
	return true;
}

//��������Ȧ
bool CDataBaseEngineSink::On_DBR_CL_CLUB_CREATE_CLUB(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(STR_DBR_CL_CLUB_CREATE_CLUB));
	if (wDataSize!=sizeof(STR_DBR_CL_CLUB_CREATE_CLUB)) return false;

	STR_DBR_CL_CLUB_CREATE_CLUB *pDBR = (STR_DBR_CL_CLUB_CREATE_CLUB*) pData;

	//��������
	m_AccountsDB->ResetParameter();

	//�������
	m_AccountsDB->AddParameter(TEXT("@Mystery"),_MYSTERY);
	m_AccountsDB->AddParameter(TEXT("@UserID"),pDBR->dwUserID);
	m_AccountsDB->AddParameter(TEXT("@ClubName"),pDBR->szClubName);
	m_AccountsDB->AddParameter(TEXT("@MajorKindID"),pDBR->dwMajorKindID);
	m_AccountsDB->AddParameter(TEXT("@MinorKindID"), pDBR->szMinorKindID);
	m_AccountsDB->AddParameter(TEXT("@szNotice"), pDBR->szNotice);
	m_AccountsDB->AddParameter(TEXT("@szMessag"), pDBR->szMessag);

	//�������
	TCHAR szDescribeString[128]=TEXT("");
	m_AccountsDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	LONG lResultCode = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_CLUB_CREATE_CLUB"),true);

	//�������
	CDBVarValue DBVarValue;
	m_AccountsDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	//�ṹ�幹��
	STR_CMD_LC_CLUB_CREATE_CLUB CMD;
	ZeroMemory(&CMD, sizeof(CMD));
	CMD.lResultCode = lResultCode;
	lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));

	if(lResultCode == DB_SUCCESS)
	{
		CMD.dwClubID =m_AccountsDB->GetValue_DWORD(TEXT("ClubID"));
		CMD.byClubLevel=m_AccountsDB->GetValue_BYTE(TEXT("ClubLevel"));
		CMD.dwFK=m_AccountsDB->GetValue_BYTE(TEXT("FK"));
	}

	g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_CREATE_CLUB,dwContextID,&CMD,sizeof(CMD));
	return true;
}

//������뷿��
bool CDataBaseEngineSink::On_DBR_CL_CLUB_JOIN_ROOM(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize!=sizeof(STR_SUB_CL_CLUB_JOIN_ROOM)) return false;

	STR_SUB_CL_CLUB_JOIN_ROOM * pDBbr = (STR_SUB_CL_CLUB_JOIN_ROOM*)pData;

	STR_CMD_LC_CLUB_JOIN_ROOM CMD1;

#pragma region �����б�
	/* ��һ�� �����б� */
	//��������
	m_AccountsDB->ResetParameter();

	//�������
	m_AccountsDB->AddParameter(TEXT("@UserID"),pDBbr->dwUserID);
	m_AccountsDB->AddParameter(TEXT("@RoomID"),pDBbr->dwRoomID);

	//�������
	TCHAR szDescribeString[128]=TEXT("");
	m_AccountsDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	long ResultCode = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_CLUB_JOIN_ROOM_TABLE"),true);

	//�������
	CDBVarValue DBVarValue;
	m_AccountsDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	CMD1.lResultCode = ResultCode;
	lstrcpyn(CMD1.szDescribe, CW2CT(DBVarValue.bstrVal), CountArray(CMD1.szDescribe));

	//�б���
	WORD wPacketSize=0;
	BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
	wPacketSize=0;
	STR_CMD_LC_CLUB_TABLE_LIST * pDBO=NULL;
	while ((ResultCode == DB_SUCCESS) && (m_AccountsDB->IsRecordsetEnd()==false ))
	{
		//������Ϣ
		if ((wPacketSize+sizeof(STR_CMD_LC_CLUB_TABLE_LIST))>sizeof(cbBuffer))
		{
			g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_TABLE_LIST_TABLE,dwContextID,cbBuffer,wPacketSize);
			wPacketSize=0;
		}

		//��ȡ��Ϣ
		pDBO=(STR_CMD_LC_CLUB_TABLE_LIST *)(cbBuffer+wPacketSize);

		pDBO->dwClubRoomID  = m_AccountsDB->GetValue_DWORD(TEXT("RoomID"));
		pDBO->dwTableID=m_AccountsDB->GetValue_DWORD(TEXT("TableID"));
		pDBO->dwClubID = m_AccountsDB->GetValue_DWORD(TEXT("ClubID"));
		pDBO->byMask = m_AccountsDB->GetValue_BYTE(TEXT("IsOwner"));

		pDBO->TableState=m_AccountsDB->GetValue_DWORD(TEXT("TableState"));
		pDBO->LockState=m_AccountsDB->GetValue_DWORD(TEXT("LockState"));
		pDBO->CurrentRound=m_AccountsDB->GetValue_DWORD(TEXT("CurrentRound"));
		pDBO->dwOwnerID=m_AccountsDB->GetValue_DWORD(TEXT("OwnerID"));
		pDBO->byDel = 0;

		//����λ��
		wPacketSize+=sizeof(STR_CMD_LC_CLUB_TABLE_LIST);

		//�ƶ���¼
		m_AccountsDB->MoveToNext();
	}
	if (wPacketSize>0) g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_TABLE_LIST_TABLE,dwContextID,cbBuffer,wPacketSize);

#pragma endregion


#pragma region ����б�
	/* �ڶ��� ����б� */
	//��������
	m_AccountsDB->ResetParameter();

	//�������
	m_AccountsDB->AddParameter(TEXT("@UserID"),pDBbr->dwUserID);
	m_AccountsDB->AddParameter(TEXT("@RoomID"),pDBbr->dwRoomID);

	//�������
	TCHAR szDescribeString2[128]=TEXT("");
	m_AccountsDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString2),adParamOutput);

	long ResultCode2 = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_CLUB_JOIN_ROOM_USER"),true);

	//�б���
	WORD wPacketSize2=0;
	BYTE cbBuffer2[MAX_ASYNCHRONISM_DATA/10];
	wPacketSize2=0;
	STR_CMD_LC_CLUB_TABLE_USER_LIST * pDBO2=NULL;
	while ((m_AccountsDB->IsRecordsetEnd()==false) && (ResultCode2 == DB_SUCCESS))
	{
		//������Ϣ
		if ((wPacketSize2+sizeof(STR_CMD_LC_CLUB_TABLE_USER_LIST))>sizeof(cbBuffer2))
		{
			g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_TABLE_LIST_USER,dwContextID,cbBuffer2,wPacketSize2);
			wPacketSize2=0;
		}

		//��ȡ��Ϣ
		pDBO2=(STR_CMD_LC_CLUB_TABLE_USER_LIST *)(cbBuffer2+wPacketSize2);
		pDBO2->dwClubRoomID=m_AccountsDB->GetValue_DWORD(TEXT("ClubRoomID"));
		pDBO2->dwUserID=m_AccountsDB->GetValue_DWORD(TEXT("UserID"));
		m_AccountsDB->GetValue_String(TEXT("UserName"),pDBO2->szUserName,CountArray(pDBO2->szUserName));
		pDBO2->bySex=m_AccountsDB->GetValue_BYTE(TEXT("Sex"));
		pDBO2->wLevel=m_AccountsDB->GetValue_WORD(TEXT("UserLevel"));
		m_AccountsDB->GetValue_String(TEXT("HeadUrl"),pDBO2->szHeadUrl,CountArray(pDBO2->szHeadUrl));
		pDBO2->byClubRole=m_AccountsDB->GetValue_BYTE(TEXT("ClubRole"));
		pDBO2->byClubReputation=m_AccountsDB->GetValue_BYTE(TEXT("ClubReputation"));
		pDBO2->dwTableID=m_AccountsDB->GetValue_DWORD(TEXT("TableID"));
		pDBO2->byChairID=m_AccountsDB->GetValue_BYTE(TEXT("ChairID"));
		pDBO2->byDel = 0;

		//����λ��
		wPacketSize2+=sizeof(STR_CMD_LC_CLUB_TABLE_USER_LIST);

		//�ƶ���¼
		m_AccountsDB->MoveToNext();
	}
	if (wPacketSize2>0) g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_TABLE_LIST_USER,dwContextID,cbBuffer2,wPacketSize2);

#pragma endregion

#pragma region ��������
	//��������
	m_AccountsDB->ResetParameter();
	m_AccountsDB->AddParameter(TEXT("@RoomID"),pDBbr->dwRoomID);

	ResultCode2 = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_CLUB_JOIN_ROOM_USER_NUM"),true);

	if(ResultCode2 == DB_SUCCESS)
	{
		CMD1.dwPlayerNum = m_AccountsDB->GetValue_DWORD(TEXT("PlayerNum"));
	}

#pragma endregion

	/* ������ ���뷿�䷵��*/
	//client ����������Ϣ��ˢ�½���
	g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_JOIN_ROOM, dwContextID, &CMD1, sizeof(CMD1));

	return true;
}


//Ⱥ��|�����������Ϣ�Ĵ�(ͬ��|�ܾ�)
bool CDataBaseEngineSink::On_DBR_CL_CLUB_APPLICANT_RESULT(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize != sizeof(STR_SUB_CL_CLUB_APPLICANT_RESULT)) return false;

	STR_SUB_CL_CLUB_APPLICANT_RESULT *pDBR = (STR_SUB_CL_CLUB_APPLICANT_RESULT*) pData;

	//��������
	m_AccountsDB->ResetParameter();
	//�������
	m_AccountsDB->AddParameter(TEXT("@UserID"),pDBR->dwUserID);
	m_AccountsDB->AddParameter(TEXT("@CludID"),pDBR->dwCludID);
	m_AccountsDB->AddParameter(TEXT("@Result"),pDBR->dwResult);
	m_AccountsDB->AddParameter(TEXT("@TagID"),pDBR->dwTagID);

	//�������
	TCHAR szDescribeString[128]=TEXT("");
	m_AccountsDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	LONG lResultCode = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_CLUB_APPLICANT_RESULT"),true);

	//�������
	CDBVarValue DBVarValue;
	m_AccountsDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	//�ṹ�幹��
	STR_CMD_LC_CLUB_APPLICANT_RESULT CMD;
	ZeroMemory(&CMD, sizeof(CMD));
	CMD.lResultCode = lResultCode;
	lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));

	g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_APPLICANT_RESULT,dwContextID,&CMD,sizeof(CMD));

	//TODONOWW ����Աͬ��֮�� ��Ҫ֪ͨ�ͻ���ָ�����ʵʱˢ�½���
	return true;
}

//�û��˳�����
bool CDataBaseEngineSink::On_DBR_CL_CLUB_QUIT(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize!=sizeof(STR_SUB_CL_CLUB_QUIT)) return false;

	STR_SUB_CL_CLUB_QUIT *pDBR = (STR_SUB_CL_CLUB_QUIT*) pData;

	//��������
	m_AccountsDB->ResetParameter();

	//�������
	m_AccountsDB->AddParameter(TEXT("@UserID"),pDBR->dwUserID);
	m_AccountsDB->AddParameter(TEXT("@ClubID"),pDBR->dwClubID);

	//�������
	TCHAR szDescribeString[128]=TEXT("");
	m_AccountsDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	LONG lResultCode = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_CLUB_QUIT"),true);

	//�������
	CDBVarValue DBVarValue;
	m_AccountsDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	//�ṹ�幹��
	STR_CMD_LC_CLUB_QUIT DBO;
	ZeroMemory(&DBO, sizeof(DBO));
	DBO.lResultCode = lResultCode;
	lstrcpyn(DBO.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(DBO.szDescribe));

	g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_QUIT,dwContextID,&DBO,sizeof(DBO));
	return true;
}

//�����Ա����
bool CDataBaseEngineSink::On_DBR_CL_CLUB_MEMBER_MANAGER(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//У�����
	if (wDataSize!=sizeof(STR_SUB_CL_CLUB_MEMBER_MANAGER)) return false;
	STR_SUB_CL_CLUB_MEMBER_MANAGER *pDBR = (STR_SUB_CL_CLUB_MEMBER_MANAGER*) pData;

	/* ��ȡ�������� */
	m_AccountsDB->ResetParameter();

	//�������
	m_AccountsDB->AddParameter(TEXT("@UserID"),pDBR->dwUserID);
	m_AccountsDB->AddParameter(TEXT("@ClubID"),pDBR->dwClubID);

	LONG lResultCode2 =  m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_CLUB_DATA"),true);

	if(DB_SUCCESS == lResultCode2)
	{
		STR_CMD_LC_CLUB_DATA CMD1;
		m_AccountsDB->GetValue_String(TEXT("ClubName"),CMD1.szClubName,CountArray(CMD1.szClubName));
		m_AccountsDB->GetValue_String(TEXT("ClubOwnerName"),CMD1.szClubOwnerName,CountArray(CMD1.szClubOwnerName));
		CMD1.dwClubID =m_AccountsDB->GetValue_DWORD(TEXT("ClubID"));
		CMD1.dwClubOwner =m_AccountsDB->GetValue_DWORD(TEXT("ClubOwner"));
		CMD1.dwMajorKindID =m_AccountsDB->GetValue_DWORD(TEXT("MajorKindID"));
		//TODONOW dwMinorKindID
		CMD1.byClubLevel =m_AccountsDB->GetValue_BYTE(TEXT("ClubLevel"));
		CMD1.wMemberNum =m_AccountsDB->GetValue_WORD(TEXT("MemberNum"));
		m_AccountsDB->GetValue_String(TEXT("Notice"),CMD1.szNotice,CountArray(CMD1.szNotice));
		m_AccountsDB->GetValue_String(TEXT("Message"),CMD1.szMessage,CountArray(CMD1.szMessage));
		CMD1.dwFK =m_AccountsDB->GetValue_DWORD(TEXT("FK"));
		CMD1.byApplyNum =m_AccountsDB->GetValue_BYTE(TEXT("ApplyNum"));
		CMD1.byAutoAgree =m_AccountsDB->GetValue_BYTE(TEXT("AutoAgree"));
		CMD1.bySex =m_AccountsDB->GetValue_BYTE(TEXT("Sex"));
		CMD1.wLevel =m_AccountsDB->GetValue_WORD(TEXT("Level"));

		g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_DATA,dwContextID,&CMD1,sizeof(CMD1));
	}
	else //������������Ϣ��ȡʧ��, ����ĳ�ԱҲ����Ҫ��ȡ��
	{
		return true;
	}

	/* ��Ա���ݷ��� */
	//��������
	m_AccountsDB->ResetParameter();

	//�������
	m_AccountsDB->AddParameter(TEXT("@UserID"),pDBR->dwUserID);
	m_AccountsDB->AddParameter(TEXT("@ClubID"),pDBR->dwClubID);

	LONG lResultCode = m_AccountsDB->ExecuteProcess(TEXT("GSP_CL_CLUB_MEMBER_MANAGER"),true);

	//�б���
	WORD wPacketSize=0;
	BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
	wPacketSize=0;
	STR_CMD_LC_CLUB_MEMBER_MANAGER * pCMD=NULL;
	while ((lResultCode == DB_SUCCESS) && (m_AccountsDB->IsRecordsetEnd()==false))
	{
		//������Ϣ
		if ((wPacketSize+sizeof(STR_CMD_LC_CLUB_MEMBER_MANAGER))>sizeof(cbBuffer))
		{
			g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_MEMBER_MANAGER,dwContextID,cbBuffer,wPacketSize);
			wPacketSize=0;
		}

		//��ȡ��Ϣ
		pCMD=(STR_CMD_LC_CLUB_MEMBER_MANAGER *)(cbBuffer+wPacketSize);

		pCMD->dwUserID =m_AccountsDB->GetValue_DWORD(TEXT("UserID"));
		m_AccountsDB->GetValue_String(TEXT("NickName"),pCMD->szUserName,CountArray(pCMD->szUserName));
		pCMD->byClubRole =m_AccountsDB->GetValue_BYTE(TEXT("RoleID"));
		pCMD->byUserLevel =m_AccountsDB->GetValue_BYTE(TEXT("UserLevel"));
		pCMD->byClubReputation =m_AccountsDB->GetValue_BYTE(TEXT("ClubReputation"));
		pCMD->byUserState =m_AccountsDB->GetValue_BYTE(TEXT("IsOnline"));
		m_AccountsDB->GetValue_String(TEXT("HeadUrl"),pCMD->szHeadUrl,CountArray(pCMD->szHeadUrl));

		//����λ��
		wPacketSize+=sizeof(STR_CMD_LC_CLUB_MEMBER_MANAGER);

		//�ƶ���¼
		m_AccountsDB->MoveToNext();
	}
	if (wPacketSize>0) g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_MEMBER_MANAGER,dwContextID,cbBuffer,wPacketSize);
	g_AttemperEngineSink->OnEventDataBaseResult(DBO_LC_CLUB_MEMBER_MANAGER_FINISH,dwContextID,NULL,0);
	return true;
}

#pragma endregion

#pragma region MDM_SHOP �̳ǵ���
//DBR && DBO��ѯ�̳�
bool CDataBaseEngineSink::On_DBR_CL_SHOP_QUERY(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize!=sizeof(STR_DBR_CL_SHOP_QUERY)) return false;
	STR_DBR_CL_SHOP_QUERY *pDBR = (STR_DBR_CL_SHOP_QUERY*) pData;

	//��������
	m_PlatformDB->ResetParameter();

	m_PlatformDB->AddParameter(TEXT("@Mystery"),_MYSTERY);
	m_PlatformDB->AddParameter(TEXT("@GoodsType"),pDBR->byGoodsType);
	m_PlatformDB->AddParameter(TEXT("@UserID"),pDBR->dwUserID);

	LONG lResultCode  = m_PlatformDB->ExecuteProcess(TEXT("GSP_CL_SHOP_QUERY"),true);

	//�б���
	//��������
	WORD wPacketSize=0;
	BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
	wPacketSize=0;
	STR_DBO_CL_SHOP_QUERY * pDBO=NULL;
	while ((lResultCode == DB_SUCCESS) && (m_PlatformDB->IsRecordsetEnd()==false))
	{
		//������Ϣ
		if ((wPacketSize+sizeof(STR_DBO_CL_SHOP_QUERY))>sizeof(cbBuffer))
		{
			g_AttemperEngineSink->OnEventDataBaseResult(DBO_CL_SHOP_QUERY,dwContextID,cbBuffer,wPacketSize);
			wPacketSize=0;
		}

		//��ȡ��Ϣ
		pDBO=(STR_DBO_CL_SHOP_QUERY *)(cbBuffer+wPacketSize);
		pDBO->dwGoodsID=m_PlatformDB->GetValue_DWORD(TEXT("GoodsID"));
		pDBO->byDiscount=m_PlatformDB->GetValue_BYTE(TEXT("Discount"));
		pDBO->dbSpreadScore=m_PlatformDB->GetValue_DWORD(TEXT("SpreadScore"));
		pDBO->dwPrice=m_PlatformDB->GetValue_DWORD(TEXT("Price"));

		//����λ��
		wPacketSize+=sizeof(STR_DBO_CL_SHOP_QUERY);

		//�ƶ���¼
		m_PlatformDB->MoveToNext();
	}
	if (wPacketSize>0) g_AttemperEngineSink->OnEventDataBaseResult(DBO_CL_SHOP_QUERY,dwContextID,cbBuffer,wPacketSize);

	g_AttemperEngineSink->OnEventDataBaseResult(DBO_CL_SHOP_QUERY_FINISH,dwContextID,NULL,0);

	return true;
}

//DBR && DBO��ʯ�������
bool CDataBaseEngineSink::On_DBR_CL_SHOP_DIAMOND(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize!=sizeof(STR_SUB_CL_SHOP_DIAMOND)) return false;

	STR_SUB_CL_SHOP_DIAMOND *pDBR = (STR_SUB_CL_SHOP_DIAMOND*) pData;

	STR_SUB_CL_SHOP_DIAMOND DBR;
	DBR.byGoodsType = pDBR->byGoodsType;
	DBR.dwShopper = pDBR->dwShopper;
	DBR.dwGoodsID = pDBR->dwGoodsID;
	DBR.byMask = pDBR->byMask;
	DBR.dwTargetID = pDBR->dwTargetID;
	DBR.dwGoodsNum = pDBR->dwGoodsNum;

	//��������
	m_PlatformDB->ResetParameter();

	//�������
	m_PlatformDB->AddParameter(TEXT("@Mystery"),_MYSTERY);
	m_PlatformDB->AddParameter(TEXT("@Shopper"),DBR.dwShopper);
	m_PlatformDB->AddParameter(TEXT("@GoodsType"),DBR.byGoodsType);
	m_PlatformDB->AddParameter(TEXT("@GoodsID"),DBR.dwGoodsID);
	m_PlatformDB->AddParameter(TEXT("@Mask"),DBR.byMask);
	m_PlatformDB->AddParameter(TEXT("@TargetID"), DBR.dwTargetID);
	m_PlatformDB->AddParameter(TEXT("@GoodsNum"), DBR.dwGoodsNum);

	//�������
	TCHAR szDescribeString[128]=TEXT("");
	m_PlatformDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	LONG lResultCode = m_PlatformDB->ExecuteProcess(TEXT("GSP_CL_SHOP_BUY"),true);

	//�������
	CDBVarValue DBVarValue;
	m_PlatformDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	//�ṹ�幹��
	STR_DBO_CL_SHOP_DIAMOND DBO;
	ZeroMemory(&DBO, sizeof(DBO));
	DBO.lResultCode = lResultCode;
	lstrcpyn(DBO.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(DBO.szDescribeString));

	g_AttemperEngineSink->OnEventDataBaseResult(DBO_CL_SHOP_DIAMOND,dwContextID,&DBO,sizeof(DBO));
	return true;
}

//DBR && DBO������Ʒ��ѯ
bool CDataBaseEngineSink::On_DBR_CL_BAG_QUERY(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize!=sizeof(STR_DBR_CL_BAG_QUERY)) return false;

	STR_DBR_CL_BAG_QUERY *pDBR = (STR_DBR_CL_BAG_QUERY*) pData;

	STR_DBR_CL_BAG_QUERY DBR;
	DBR.dwUserID = pDBR->dwUserID;

	//��������
	m_PlatformDB->ResetParameter();
	//�������
	m_PlatformDB->AddParameter(TEXT("@UserID"),DBR.dwUserID);

	LONG lResultCode = m_PlatformDB->ExecuteProcess(TEXT("GSP_CL_BAG_QUERY"),true);

	//�б���
	WORD wPacketSize=0;
	BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
	wPacketSize=0;
	STR_DBO_CL_BAG_QUERY * pDBO=NULL;
	while ((DB_SUCCESS == lResultCode) && (m_PlatformDB->IsRecordsetEnd()==false))
	{
		//������Ϣ
		if ((wPacketSize+sizeof(STR_DBO_CL_BAG_QUERY))>sizeof(cbBuffer))
		{
			g_AttemperEngineSink->OnEventDataBaseResult(DBO_CL_BAG_QUERY,dwContextID,cbBuffer,wPacketSize);
			wPacketSize=0;
		}

		//��ȡ��Ϣ
		pDBO=(STR_DBO_CL_BAG_QUERY *)(cbBuffer+wPacketSize);
		pDBO->dwGoodsID = m_PlatformDB->GetValue_DWORD(TEXT("GoodsID"));
		pDBO->dwGoodsNum = m_PlatformDB->GetValue_DWORD(TEXT("GoodsNum"));
		pDBO->byGoodsType = m_PlatformDB->GetValue_BYTE(TEXT("GoodsType"));

		//����λ��
		wPacketSize+=sizeof(STR_DBO_CL_BAG_QUERY);

		//�ƶ���¼
		m_PlatformDB->MoveToNext();
	}
	if (wPacketSize>0) g_AttemperEngineSink->OnEventDataBaseResult(DBO_CL_BAG_QUERY,dwContextID,cbBuffer,wPacketSize);


	//��������
	m_PlatformDB->ResetParameter();
	//�������
	m_PlatformDB->AddParameter(TEXT("@UserID"),DBR.dwUserID);

	lResultCode = m_PlatformDB->ExecuteProcess(TEXT("GSP_CL_BAG_QUERY_LOVENESS"),true);

	STR_CMD_LC_BAG_FINISH CMD2;
	ZeroMemory(&CMD2, sizeof(CMD2));

	if(DB_SUCCESS == lResultCode)
	{
		CMD2.dwLoveness = m_PlatformDB->GetValue_DWORD(TEXT("Loveness"));
	}

	g_AttemperEngineSink->OnEventDataBaseResult(DBO_CL_BAG_FINISH, dwContextID, &CMD2, sizeof(CMD2));

	return true;
}

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

#pragma endregion
