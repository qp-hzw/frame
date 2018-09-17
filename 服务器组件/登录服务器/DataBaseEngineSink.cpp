#include "StdAfx.h"
#include <vector>
#include <string>
#include <algorithm> 
#include "ServiceUnits.h"
#include "DataBaseEngineSink.h"
#include "LogicTraceHelper.h"
#include "RankManager.h"


#include "..\..\������\�ں�����\TraceService.h"
#include "..\..\������\�������\WHService.h"
#include "..\..\������\ȫ�ֶ���\Array.h"
#include "..\..\ȫ�ֶ���\Define.h"
#include "..\..\��Ϣ����\STR_CMD_GameServer.h"

//���캯��
CDataBaseEngineSink::CDataBaseEngineSink()
{
	//�������
	m_pIDataBaseEngineEvent=NULL;

	return;
}
//�ӿڲ�ѯ
VOID * CDataBaseEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBaseEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseEngineSink,Guid,dwQueryVer);
	return NULL;
}

//�����¼�
bool CDataBaseEngineSink::OnDataBaseEngineStart(IUnknownEx * pIUnknownEx)
{
	//��������
	if ((m_AccountsDBModule.GetInterface()==NULL)&&(m_AccountsDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	if ((m_TreasureDBModule.GetInterface()==NULL)&&(m_TreasureDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	if ((m_PlatformDBModule.GetInterface()==NULL)&&(m_PlatformDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	try
	{
		//��������
		m_PlatformDBModule->SetConnectionInfo(1);
		m_AccountsDBModule->SetConnectionInfo(2);
		m_TreasureDBModule->SetConnectionInfo(3);

		//��������
		m_AccountsDBModule->OpenConnection();
		m_AccountsDBAide.SetDataBase(m_AccountsDBModule.GetInterface());

		//��������
		m_TreasureDBModule->OpenConnection();
		m_TreasureDBAide.SetDataBase(m_TreasureDBModule.GetInterface());

		//��������
		m_PlatformDBModule->OpenConnection();
		m_PlatformDBAide.SetDataBase(m_PlatformDBModule.GetInterface());

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		return false;
	}

	return true;
}

//ֹͣ�¼�
bool CDataBaseEngineSink::OnDataBaseEngineConclude(IUnknownEx * pIUnknownEx)
{
	//���ö���
	m_AccountsDBAide.SetDataBase(NULL);
	m_TreasureDBAide.SetDataBase(NULL);
	m_PlatformDBAide.SetDataBase(NULL);

	//�ر�����
	if (m_AccountsDBModule.GetInterface()!=NULL)
	{
		m_AccountsDBModule->CloseConnection();
		m_AccountsDBModule.CloseInstance();
	}

	//�ر�����
	if (m_TreasureDBModule.GetInterface()!=NULL)
	{
		m_TreasureDBModule->CloseConnection();
		m_TreasureDBModule.CloseInstance();
	}

	//�ر�����
	if (m_PlatformDBModule.GetInterface()!=NULL)
	{
		m_PlatformDBModule->CloseConnection();
		m_PlatformDBModule.CloseInstance();
	}

	//�������
	m_pIDataBaseEngineEvent=NULL;

	return true;
}

//���ݿ������¼�
bool CDataBaseEngineSink::OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
	case DBR_GP_LOGON_USER_STATE: //�û�Sockt���ӹر�
		{
			return On_DBR_GP_QUIT(dwContextID,pData,wDataSize);
		}
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
#pragma region ��������
	case DBR_GP_LOAD_GAME_LIST:				//�����б�
		{
			return OnRequestLoadGameList(dwContextID,pData,wDataSize);
		}
	case DBR_GP_ONLINE_COUNT_INFO:			//����ͳ�� -- �������ݿ�
		{
			return OnRequestOnLineCountInfo(dwContextID,pData,wDataSize);
		}
	case DBR_UPDATA_MARQUEE:				//�����������Ϣ
		{
			return On_DBR_UPDATA_MARQUEE(dwContextID,pData,wDataSize);
		}
#pragma endregion 
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

	try
	{
		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@iUserID"),pUserSuggestion->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@chFB_Title"),pUserSuggestion->szFB_Title);
		m_AccountsDBAide.AddParameter(TEXT("@chFB_Content"),pUserSuggestion->szFB_Content);
		m_AccountsDBAide.AddParameter(TEXT("@chContact"),pUserSuggestion->szContact);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_Service_Feedback"));	

		//�������
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_Service_Feedback"),true);		//��QPAccountsDB���ݿ��AccountsInfo���в����û�����

		//���ؽ������
		On_DBO_Service_UserFeedback(dwContextID, lResultCode, TEXT("��ҷ����������!"));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//�������
		On_DBO_Service_UserFeedback(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"));

		return false;
	}

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
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_SERVICE_USER_FEEDBACK,dwContextID,&OperateResult,wHeadSize+wDataSize);

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

	try
	{
		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pUserRequest->dwUserID);

		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_Service_RefreshUserInfo"));		

		//�������
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_Service_RefreshUserInfo"),true);

		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);


		//�������
		On_DBO_Service_RefreshUserInfo(dwContextID, lResultCode, CW2CT(DBVarValue.bstrVal));
	}
	catch(IDataBaseException * pIException)		//�쳣ͨ�ô���
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		////�쳣�������
		//DBR_CommandSource wRequestID ;
		//ZeroMemory(&wRequestID,sizeof(wRequestID));
		//wRequestID.MainCommand = MDM_SERVICE;
		//wRequestID.SubCommand  = SUB_CL_SERVICE_REFRESH_USER_INFO;

		//On_DBO_CommonOperateResult(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),wRequestID);

		return false;
	}
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
		UserInfo.dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
		//�û��ǳ�
		m_AccountsDBAide.GetValue_String(TEXT("NickName"),UserInfo.szNickName,CountArray(UserInfo.szNickName));
		//�û��Ա�
		UserInfo.cbGender=m_AccountsDBAide.GetValue_BYTE(TEXT("Gender"));
		//ͷ������
		UserInfo.wFaceID=m_AccountsDBAide.GetValue_WORD(TEXT("FaceID"));
		//����ǩ��
		m_AccountsDBAide.GetValue_String(TEXT("MySignature"),UserInfo.szMySignature,CountArray(UserInfo.szMySignature));

		//�û�Ԫ��
		UserInfo.dwUserDiamond=m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserDiamond"));
		//�û�����
		UserInfo.dwUserMedal=m_AccountsDBAide.GetValue_DWORD(TEXT("UserMedal"));
		//������ֵ
		UserInfo.dwExperience=m_AccountsDBAide.GetValue_DWORD(TEXT("Experience"));
		//�û�����
		UserInfo.dwLoveLiness=m_AccountsDBAide.GetValue_DWORD(TEXT("LoveLiness"));

		//����Ա�ȼ�
		UserInfo.cbMasterOrder=m_AccountsDBAide.GetValue_BYTE(TEXT("MasterOrder"));
		//��Ա�ȼ�
		UserInfo.cbMemberOrder=m_AccountsDBAide.GetValue_BYTE(TEXT("MemberOrder"));
		//��Ա����ʱ
		m_AccountsDBAide.GetValue_SystemTime(TEXT("MemberOverDate"),UserInfo.MemberOverDate);

		//��ʵ����
		m_AccountsDBAide.GetValue_String(TEXT("IdentityName"),UserInfo.szIdentityName,CountArray(UserInfo.szIdentityName));
		//���֤��
		m_AccountsDBAide.GetValue_String(TEXT("IdentityNum"),UserInfo.szIdentityNum,CountArray(UserInfo.szIdentityNum));
		//�ֻ�����
		m_AccountsDBAide.GetValue_String(TEXT("MobilePhone"),UserInfo.szMobilePhone,CountArray(UserInfo.szMobilePhone));
		/* *****************************    �˺���Ϣ     ****************************/
		//����¼��ַ
		m_AccountsDBAide.GetValue_String(TEXT("LastLogonIP"),UserInfo.szLasLogonIp,CountArray(UserInfo.szLasLogonIp));
		//�������ʱ�� 
		m_AccountsDBAide.GetValue_SystemTime(TEXT("LastLogonDate"),UserInfo.LasLogonDate);
		/* *****************************    ��������     ****************************/
		//���ű�־ -- ����Ȧ
		UserInfo.dwGroupID=m_AccountsDBAide.GetValue_DWORD(TEXT("GroupID"));
		//�������� -- ����Ȧ
		m_AccountsDBAide.GetValue_String(TEXT("GroupName"),UserInfo.szGroupName,CountArray(UserInfo.szGroupName));

		//�û�����
		UserInfo.lUserScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserScore"));
		//�û���Ϸ��
		UserInfo.lUserGold = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserGold"));
		//�û�����
		UserInfo.lUserRoomCard = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserRoomCard"));

		//��������
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_SERVICE_REFRESH_USER_INFO, dwContextID, &UserInfo, sizeof(UserInfo));
	}
	else								//ʧ�ܴ���
	{
		//��������
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_SERVICE_REFRESH_USER_INFO, dwContextID, &UserInfo, sizeof(UserInfo));
	}

	return true;
}

//��ѯ������Ϣ�б�
bool CDataBaseEngineSink::On_DBR_Service_QueryRoomList(DWORD dwContextID, void * pData, WORD wDataSize)
{
	try
	{
		STR_DBR_CL_SERCIVR_QUERY_ROOMLIST* pGetTableInfo = (STR_DBR_CL_SERCIVR_QUERY_ROOMLIST* )pData;
		if(pData == NULL || wDataSize != sizeof(STR_DBR_CL_SERCIVR_QUERY_ROOMLIST))
			return false;

		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("dwOwnerID"),pGetTableInfo->dwUserID);


		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_Service_GetRoomList"));		

		//ִ�в�ѯ
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_CL_Service_GetRoomList"),true);	//��ѯQPTreasure���ݿ��TableInfo��

		//�������
		On_DBO_Service_QueryRoomList(dwContextID, lResultCode);
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//TODO ����Ĵ���

		return false;
	}

	return true;

}

//��ѯ������Ϣ�б���
bool CDataBaseEngineSink::On_DBO_Service_QueryRoomList(DWORD dwContextID, DWORD dwResultCode)
{
	//�������
	if (dwResultCode == DB_SUCCESS)		//�ɹ�
	{
		for(int i=0;i < m_TreasureDBModule->GetRecordCount() && i < 10;i++,m_TreasureDBModule->MoveToNext())
		{
			//��������
			STR_DBO_CL_SERCIVR_QUERY_ROOMLIST TableInfo;
			ZeroMemory(&TableInfo,sizeof(TableInfo));

			TableInfo.dwTableID = m_TreasureDBAide.GetValue_DWORD(TEXT("TableID"));
			TableInfo.dwCreaterID = m_TreasureDBAide.GetValue_DWORD(TEXT("CreaterID"));
			TableInfo.dwPassword = m_TreasureDBAide.GetValue_DWORD(TEXT("Password"));
			TableInfo.wJuShu = m_TreasureDBAide.GetValue_WORD(TEXT("JuShu"));
			TableInfo.byMode = m_TreasureDBAide.GetValue_BYTE(TEXT("GameMode"));
			TableInfo.byZhuangType = m_TreasureDBAide.GetValue_BYTE(TEXT("ZhuangType"));
			TableInfo.byPlayerCount = m_TreasureDBAide.GetValue_BYTE(TEXT("PlayerCount"));
			TableInfo.byMaxPlayerCount = m_TreasureDBAide.GetValue_BYTE(TEXT("MaxPlayerCount"));
			TableInfo.byCost = m_TreasureDBAide.GetValue_BYTE(TEXT("PayCost"));
			TableInfo.byIsStart = m_TreasureDBAide.GetValue_BYTE(TEXT("IsStart"));

			//�û�ID
			TableInfo.dwUserID[0] = m_TreasureDBAide.GetValue_DWORD(TEXT("UserID1"));
			TableInfo.dwUserID[1] = m_TreasureDBAide.GetValue_DWORD(TEXT("UserID2"));
			TableInfo.dwUserID[2] = m_TreasureDBAide.GetValue_DWORD(TEXT("UserID3"));
			TableInfo.dwUserID[3] = m_TreasureDBAide.GetValue_DWORD(TEXT("UserID4"));
			TableInfo.dwUserID[4] = m_TreasureDBAide.GetValue_DWORD(TEXT("UserID5"));

			//�û��ǳ�
			m_TreasureDBAide.GetValue_String(TEXT("NickName1"),TableInfo.szNickName[0],CountArray(TableInfo.szNickName[0]));
			m_TreasureDBAide.GetValue_String(TEXT("NickName2"),TableInfo.szNickName[1],CountArray(TableInfo.szNickName[1]));
			m_TreasureDBAide.GetValue_String(TEXT("NickName3"),TableInfo.szNickName[2],CountArray(TableInfo.szNickName[2]));
			m_TreasureDBAide.GetValue_String(TEXT("NickName4"),TableInfo.szNickName[3],CountArray(TableInfo.szNickName[3]));
			m_TreasureDBAide.GetValue_String(TEXT("NickName5"),TableInfo.szNickName[4],CountArray(TableInfo.szNickName[4]));

			//�û�����
			TableInfo.dwScore[0] = m_TreasureDBAide.GetValue_DWORD(TEXT("Score1"));
			TableInfo.dwScore[1] = m_TreasureDBAide.GetValue_DWORD(TEXT("Score2"));
			TableInfo.dwScore[2] = m_TreasureDBAide.GetValue_DWORD(TEXT("Score3"));
			TableInfo.dwScore[3] = m_TreasureDBAide.GetValue_DWORD(TEXT("Score4"));
			TableInfo.dwScore[4] = m_TreasureDBAide.GetValue_DWORD(TEXT("Score5"));

			m_TreasureDBAide.GetValue_String(TEXT("CreateTime"),TableInfo.szTime,CountArray(TableInfo.szTime));	

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_SERVICE_QUERY_ROOM_LIST, dwContextID, &TableInfo, sizeof(STR_DBO_CL_SERCIVR_QUERY_ROOMLIST));
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
	try
	{
		STR_DBR_CL_SERCIVR_GET_RICHLIST* pRichList = (STR_DBR_CL_SERCIVR_GET_RICHLIST* )pData;
		if(pData == NULL || wDataSize != sizeof(STR_DBR_CL_SERCIVR_GET_RICHLIST))
			return false;

		m_AccountsDBAide.ResetParameter();

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_Service_GetRichList")); 
		//ִ�в�ѯ
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_Service_GetRichList"), true);		//��ѯ���ݿ�[QPAccountsDB]�е�RegalRank��

		//�������
		On_DBO_Service_GetRichList(dwContextID, lResultCode);

	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;

}

//��ȡ�����񷵻�
bool CDataBaseEngineSink::On_DBO_Service_GetRichList(DWORD dwContextID, DWORD dwResultCode)
{
	if(dwResultCode == DB_SUCCESS)
	{
		STR_DBO_CL_SERCIVR_GET_RICHLIST result;
		ZeroMemory(&result,sizeof(STR_DBO_CL_SERCIVR_GET_RICHLIST));

		for(int i=0;i < m_AccountsDBModule->GetRecordCount() && i < 15;i++)
		{
			result.byCount ++;
			m_AccountsDBAide.GetValue_String(TEXT("Name"),result.RegalInfoList[i].szName,CountArray(result.RegalInfoList[i].szName));
			result.RegalInfoList[i].dwMoney = m_AccountsDBAide.GetValue_DWORD(TEXT("Money"));
			m_AccountsDBAide.GetValue_String(TEXT("Wechat"),result.RegalInfoList[i].szWechatAccount,CountArray(result.RegalInfoList[i].szWechatAccount));
			result.RegalInfoList[i].dwRegistID = m_AccountsDBAide.GetValue_DWORD(TEXT("RegistID"));	
			m_AccountsDBAide.GetValue_String(TEXT("QQ"),result.RegalInfoList[i].szQQ,CountArray(result.RegalInfoList[i].szQQ));
			m_AccountsDBAide.GetValue_String(TEXT("MobilePhone"),result.RegalInfoList[i].szMobilePhone,CountArray(result.RegalInfoList[i].szMobilePhone));

			m_AccountsDBModule->MoveToNext();
		}

		if(result.byCount > 0)
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_SERVICE_GET_RICH_LIST,dwContextID,&result,sizeof(STR_DBO_CL_SERCIVR_GET_RICHLIST));
	}

	return true;
}

//��ȡ¼���б�
bool CDataBaseEngineSink::On_DBR_Service_GetUserRecordList(DWORD dwContextID, void * pData, WORD wDataSize)
{
	try
	{
		if (sizeof(STR_DBR_CL_SERCIVR_GET_RECORDLIST) < wDataSize)
		{
			return false;
		}

		STR_DBR_CL_SERCIVR_GET_RECORDLIST* pDbReq = (STR_DBR_CL_SERCIVR_GET_RECORDLIST*)pData;

		m_TreasureDBAide.ResetParameter();

		m_TreasureDBAide.AddParameter(TEXT("@UserID"),pDbReq->dwUserID);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_Service_GetRecordList"));

		//ִ�в�ѯ
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_CL_Service_GetRecordList"),true);		//��ѯ���ݿ�[QPTreasureDB]�е�GameRecord��

		//�������
		On_DBO_Service_GetUserRecordList(dwContextID,lResultCode,NULL);
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//��ȡ¼���б���
bool CDataBaseEngineSink::On_DBO_Service_GetUserRecordList(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString)
{
	if (dwErrorCode == DB_SUCCESS)
	{
		//�����������ID
		vector <DWORD> dwTableID;
		for (int i = 0; i < m_TreasureDBModule->GetRecordCount(); i++)
		{
			//ɾ���ظ�������
			DWORD dwTmpTableID = m_TreasureDBAide.GetValue_DWORD(TEXT("TableID"));
			std::vector <DWORD>::iterator result = find(dwTableID.begin(), dwTableID.end(), dwTmpTableID);
			if ( result == dwTableID.end() )
			{
				dwTableID.push_back(dwTmpTableID);
			}
			m_TreasureDBModule->MoveToNext();
		}

		//�Ƶ���ͷ
		m_TreasureDBModule->MoveToFirst();
		int nRecordListNum = m_TreasureDBModule->GetRecordCount();

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
			m_TreasureDBAide.ResetParameter();

			//�������ID
			DWORD dwTempTableID = m_TreasureDBAide.GetValue_DWORD(TEXT("TableID"));

			//��ø����ӵ�����
			for (DWORD j = 0; j < dwTableID.size(); j++)
			{
				if ( dwTempTableID == dwTableID[j] )
				{
					RecordList[i].dwUserID[0] = m_TreasureDBAide.GetValue_DWORD(TEXT("UserID0"));
					RecordList[i].dwUserID[1] = m_TreasureDBAide.GetValue_DWORD(TEXT("UserID1"));
					RecordList[i].dwUserID[2] = m_TreasureDBAide.GetValue_DWORD(TEXT("UserID2"));
					RecordList[i].dwUserID[3] = m_TreasureDBAide.GetValue_DWORD(TEXT("UserID3"));
					RecordList[i].dwUserID[4] = m_TreasureDBAide.GetValue_DWORD(TEXT("UserID4"));
					m_TreasureDBAide.GetValue_String(TEXT("Nick0"),RecordList[i].szUserName[0],LEN_NICKNAME);
					m_TreasureDBAide.GetValue_String(TEXT("Nick1"),RecordList[i].szUserName[1],LEN_NICKNAME);
					m_TreasureDBAide.GetValue_String(TEXT("Nick2"),RecordList[i].szUserName[2],LEN_NICKNAME);
					m_TreasureDBAide.GetValue_String(TEXT("Nick3"),RecordList[i].szUserName[3],LEN_NICKNAME);
					m_TreasureDBAide.GetValue_String(TEXT("Nick4"),RecordList[i].szUserName[4],LEN_NICKNAME);
					RecordList[i].byGameMode = m_TreasureDBAide.GetValue_BYTE(TEXT("GameMode"));
					RecordList[i].byZhuangType = m_TreasureDBAide.GetValue_BYTE(TEXT("ZhuangType"));
					RecordList[i].wCurrentJuShu = m_TreasureDBAide.GetValue_WORD(TEXT("CurrentJuShu"));
					RecordList[i].wTotalJuShu = m_TreasureDBAide.GetValue_WORD(TEXT("TotalJuShu"));
					m_TreasureDBAide.GetValue_String(TEXT("GameTime"),RecordList[i].szGameTime,30);
					m_TreasureDBAide.GetValue_String(TEXT("Score"),RecordList[i].szScore,50);

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

			m_TreasureDBModule->MoveToNext();
		}

		//���ʹ��¼������
		for (DWORD i = 0; i < dwTableID.size(); i++)
		{
			//���Ӵ�ַ���
			_sntprintf_s(RecordList[i].szTotalScore, CountArray(RecordList[i].szTotalScore), TEXT("%lld,%lld,%lld,%lld,%lld"),
				lTotalscore[i][0], lTotalscore[i][1], lTotalscore[i][2], lTotalscore[i][3], lTotalscore[i][4]);

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_SERVICE_GET_USER_RECORD_LIST, dwContextID, &RecordList[i], 
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
	try
	{
		if (sizeof(STR_DBR_CL_SERCIVR_GET_SPECIFIC_RECORD) != wDataSize)
		{
			return false;
		}

		STR_DBR_CL_SERCIVR_GET_SPECIFIC_RECORD* pDbReq = (STR_DBR_CL_SERCIVR_GET_SPECIFIC_RECORD*)pData;

		m_TreasureDBAide.ResetParameter();

		m_TreasureDBAide.AddParameter(TEXT("@dwTableID"),pDbReq->dwTableID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);
		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_Service_GetSpecificRecord"));

		//ִ�в�ѯ
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_CL_Service_GetSpecificRecord"),true);		//��ѯ���ݿ�[QPTreasureDB]�е�GameRecord��

		//�������
		CDBVarValue DBVarValue;
		m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		On_DBO_Service_GetSpecificRecord(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal));
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;

}

//��ȡָ��ID¼�񷵻�
bool CDataBaseEngineSink::On_DBO_Service_GetSpecificRecord(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString)
{
	if (dwErrorCode==DB_SUCCESS)
	{
		for(int i=0; i < m_TreasureDBModule->GetRecordCount(); i++)
		{
			//��������
			STR_DBO_CL_SERCIVR_GET_SPECIFIC_RECORD RecordInfo;
			ZeroMemory(&RecordInfo,sizeof(RecordInfo));

			//�������
			RecordInfo.wCurrentJuShu = m_TreasureDBAide.GetValue_WORD(TEXT("CurrentJuShu"));
			m_TreasureDBAide.GetValue_String(TEXT("Nick0"), RecordInfo.szNickName[0], LEN_NICKNAME);
			m_TreasureDBAide.GetValue_String(TEXT("Nick1"), RecordInfo.szNickName[1], LEN_NICKNAME);
			m_TreasureDBAide.GetValue_String(TEXT("Nick2"), RecordInfo.szNickName[2], LEN_NICKNAME);
			m_TreasureDBAide.GetValue_String(TEXT("Nick3"), RecordInfo.szNickName[3], LEN_NICKNAME);
			m_TreasureDBAide.GetValue_String(TEXT("Nick4"), RecordInfo.szNickName[4], LEN_NICKNAME);
			m_TreasureDBAide.GetValue_String(TEXT("Score"), RecordInfo.szScore, 50);

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_SERVICE_GET_SPECIFIC_RECORD, dwContextID, &RecordInfo, sizeof(STR_DBO_CL_SERCIVR_GET_SPECIFIC_RECORD));

			m_TreasureDBModule->MoveToNext();
		}
	}

	return true;
}

//��ȡ���߽���
bool CDataBaseEngineSink::On_DBR_Service_OnlineReward(DWORD dwContextID, void * pData, WORD wDataSize)
{
	try
	{
		if (sizeof(STR_DBR_CL_SERCIVR_ONLINE_REWARD) < wDataSize)
		{
			return false;
		}

		STR_DBR_CL_SERCIVR_ONLINE_REWARD* pDbReq = (STR_DBR_CL_SERCIVR_ONLINE_REWARD*)pData;

		//��¼����200���

		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@UserID"), pDbReq->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@lGold"), 200);
		m_TreasureDBAide.AddParameter(TEXT("@lOpenRoomCard"), 0);
		m_TreasureDBAide.AddParameter(TEXT("@lDiamond"), 0);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_Service_ModifyUserMoney")); 
		//ִ�в�ѯ
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_CL_Service_ModifyUserMoney"), true);	//�޸�[QPTreasureDB]���ݿ��е�GameScoreInfo��

		//�������
		On_DBO_Service_OnlineReward(dwContextID, lResultCode, TEXT("��ȡ��¼����ʧ�ܣ�"));

	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;

}

//��ȡ���߽�������
bool CDataBaseEngineSink::On_DBO_Service_OnlineReward(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString)
{
	if(dwErrorCode == DB_SUCCESS)
	{
		STR_DBO_CL_SERCIVR_ONLINE_REWARD result;
		result.byType = 1;
		result.dwCount = m_TreasureDBAide.GetValue_DWORD(TEXT("Gold"));
		lstrcpyn(result.szDescribe,TEXT("�ɹ���ȡ��¼������"),CountArray(result.szDescribe));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_SERVICE_ONLINE_REWARD, dwContextID, &result, sizeof(STR_DBO_CL_SERCIVR_ONLINE_REWARD));

		//�Ƹ��������
		STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO UserScoreInfo;
		UserScoreInfo.dwUserID = m_TreasureDBAide.GetValue_DWORD(TEXT("UserID"));
		UserScoreInfo.lGold = m_TreasureDBAide.GetValue_LONGLONG(TEXT("Gold"));
		UserScoreInfo.lOpenRoomCard = m_TreasureDBAide.GetValue_LONGLONG(TEXT("OpenRoomCard"));
		UserScoreInfo.lDiamond = m_TreasureDBAide.GetValue_LONGLONG(TEXT("Diamond"));

		//���ͽ��
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_USER_QUERY_SCORE_INFO,dwContextID,&UserScoreInfo,sizeof(STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO));

	}
	else
	{
		STR_DBO_CL_SERCIVR_ONLINE_REWARD result;
		result.byType = 0;
		result.dwCount = 0;
		lstrcpyn(result.szDescribe, pszErrorString, CountArray(result.szDescribe));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_SERVICE_ONLINE_REWARD,dwContextID,&result,sizeof(STR_DBO_CL_SERCIVR_ONLINE_REWARD));
	}

	return true;
}

//��ȡ�����б�
bool CDataBaseEngineSink::On_DBR_Service_GetTaskList(DWORD dwContextID, void * pData, WORD wDataSize)
{
	try
	{
		if (sizeof(STR_DBR_CL_SERCIVR_GET_TASKLIST) != wDataSize)
		{
			return false;
		}

		STR_DBR_CL_SERCIVR_GET_TASKLIST* pDbReq = (STR_DBR_CL_SERCIVR_GET_TASKLIST*)pData;

		//��ȡ�����б�
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pDbReq->dwUserID);
		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_Service_GetTaskList"));

		//ִ�в�ѯ
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_Service_GetTaskList"), true);		//��ѯ[QPAccountsDB]���ݿ��TaskList��

		//�������
		On_DBO_Service_GetTaskList(pDbReq->dwUserID, dwContextID, lResultCode, TEXT("������ʧ�ܣ����Ժ�����!"));

	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;

}

//��ȡ�����б���
bool CDataBaseEngineSink::On_DBO_Service_GetTaskList(DWORD dwUserID, DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString)
{
	//��������
	STR_DBO_CL_SERCIVR_GET_TASKLIST taskItem;
	ZeroMemory(&taskItem,sizeof(STR_DBO_CL_SERCIVR_GET_TASKLIST));

	//��ʱ����
	int nTaskNum = m_AccountsDBModule->GetRecordCount();
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
			taskItem.wTaskID = m_AccountsDBAide.GetValue_WORD(TEXT("TaskID"));
			taskItem.cbTaskType = m_AccountsDBAide.GetValue_BYTE(TEXT("TaskType"));
			taskItem.cbActionType = m_AccountsDBAide.GetValue_BYTE(TEXT("ActionType"));
			taskItem.dwNeedCount = m_AccountsDBAide.GetValue_DWORD(TEXT("NeedCount"));
			m_AccountsDBAide.GetValue_String(TEXT("Describe"), taskItem.szDescribe, CountArray(taskItem.szDescribe));		
			taskItem.byRewardType = m_AccountsDBAide.GetValue_BYTE(TEXT("RewardType"));
			taskItem.dwRewardCount = m_AccountsDBAide.GetValue_DWORD(TEXT("RewardCount"));

			//�������״̬
			taskItem.cbTaskStatus = m_AccountsDBAide.GetValue_BYTE(TEXT("TaskStatus"));
			taskItem.dwFinishedCount = m_AccountsDBAide.GetValue_DWORD(TEXT("LeftCount"));

			//��ֵ
			memcpy_s(&arTaskList[i], sizeof(STR_DBO_CL_SERCIVR_GET_TASKLIST), &taskItem, sizeof(STR_DBO_CL_SERCIVR_GET_TASKLIST));

			m_AccountsDBModule->MoveToNext();
		}		
	}

	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_SERVICE_GET_TASK_LIST, dwContextID, arTaskList, nTaskNum*sizeof(STR_DBO_CL_SERCIVR_GET_TASKLIST));

	//TODO �ں���ʹ�õ�ʱ�����
	/*delete [] arTaskList;*/

	return true;
}

//��ȡ������
bool CDataBaseEngineSink::On_DBR_Service_GetTaskReward(DWORD dwContextID, void * pData, WORD wDataSize)
{
	try
	{
		if (sizeof(STR_DBR_CL_SERCIVR_GET_TASK_REWARD) < wDataSize)
		{
			return false;
		}

		STR_DBR_CL_SERCIVR_GET_TASK_REWARD* pDbReq = (STR_DBR_CL_SERCIVR_GET_TASK_REWARD*)pData;

		m_AccountsDBAide.ResetParameter();

		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pDbReq->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@wTaskID"), pDbReq->wTaskID);

		//�������
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_Service_GetTaskReward"));

		//ִ�в�ѯ
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_Service_GetTaskReward"), true);		//��ѯ���ݿ�[QPAccountsDB]�е�AccountsTaskStatus���������ݿ��е�GameScoreInfo��
		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//��ȡ����������
		On_DBO_Service_GetTaskReward(pDbReq->wTaskID, dwContextID, lResultCode, CW2CT(DBVarValue.bstrVal));

	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

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
		UserScoreInfo.lGold = m_AccountsDBAide.GetValue_LONG(TEXT("Gold"));
		UserScoreInfo.lOpenRoomCard = m_AccountsDBAide.GetValue_LONG(TEXT("OpenRoomCard"));
		UserScoreInfo.lDiamond = m_AccountsDBAide.GetValue_LONG(TEXT("Diamond"));

		//�����ʲ�������Ϣ
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_USER_QUERY_SCORE_INFO, dwContextID, &UserScoreInfo, sizeof(STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO));
	}

	//�յ�������
	STR_DBO_CL_SERCIVR_GET_TASK_REWARD result;
	ZeroMemory(&result,sizeof(result));
	result.lResultCode = dwErrorCode;
	lstrcpyn( result.szDescribe, pszErrorString, CountArray( result.szDescribe));

	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_SERVICE_GET_TASK_REWARD,dwContextID,&result,sizeof(result));

	return true;
}

//����齱
bool CDataBaseEngineSink::On_DBR_Service_RequestLottery(DWORD dwContextID, void * pData, WORD wDataSize)
{
	try
	{
		if (sizeof(STR_DBR_CL_SERCIVR_REQUEST_LOTTERY) != wDataSize)
		{
			return false;
		}

		STR_DBR_CL_SERCIVR_REQUEST_LOTTERY* pDbReq = (STR_DBR_CL_SERCIVR_REQUEST_LOTTERY*)pData;

		//��ȡһ��1-100�������
		srand((unsigned int)time(NULL)) ;
		int index = rand() % 100 + 1;

		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@UserID"), pDbReq->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@Rand"), index);
		//�������
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_Service_RequestLottery")); 
		//ִ�в�ѯ,ִ�г齱
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_Service_RequestLottery"), true);		//��ѯ���ݿ�[QPAccountsDB]��LotteryList��ִ�г齱��������Ϸ��Ҳ�и�����齱��
		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		On_DBO_Service_RequestLottery(pDbReq->dwUserID, dwContextID, lResultCode, CW2CT(DBVarValue.bstrVal));
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

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
		LotteryResult.byIndex = m_AccountsDBAide.GetValue_INT(TEXT("RandIndex"));
		if ( LotteryResult.byIndex >=1 && LotteryResult.byIndex <= 11 )
		{
			if ( LotteryResult.byIndex <= 6 )
			{
				LotteryResult.byIndex = LotteryResult.byIndex - 1;
			}

			//���ͳ齱���
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_SERVICE_REQUEST_LOTTERY, dwContextID, &LotteryResult, sizeof(STR_DBO_CL_SERCIVR_REQUEST_LOTTERY));

		}
		else		//δ�н�����6
		{
			//��ֵ
			LotteryResult.byIndex = 6;

			//���ͳ齱���
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_SERVICE_REQUEST_LOTTERY, dwContextID, &LotteryResult, sizeof(STR_DBO_CL_SERCIVR_REQUEST_LOTTERY));
		}	
	}
	else	//ʧ�ܣ�ֱ�ӷ��ͳ齱���
	{
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_SERVICE_REQUEST_LOTTERY, dwContextID, &LotteryResult, sizeof(STR_DBO_CL_SERCIVR_REQUEST_LOTTERY));
	}

	//����Ƹ��������
	STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO UserScoreInfo;
	ZeroMemory(&UserScoreInfo,sizeof(STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO));

	UserScoreInfo.dwUserID = dwUserID;
	UserScoreInfo.lGold = m_AccountsDBAide.GetValue_LONG(TEXT("Gold"));
	UserScoreInfo.lOpenRoomCard = m_AccountsDBAide.GetValue_LONG(TEXT("OpenRoomCard"));
	UserScoreInfo.lDiamond = m_AccountsDBAide.GetValue_LONG(TEXT("Diamond"));
	UserScoreInfo.lRewardCard = m_AccountsDBAide.GetValue_LONG(TEXT("RewardCard"));
	UserScoreInfo.lScore = m_AccountsDBAide.GetValue_LONG(TEXT("Score"));

	//���ͲƸ������Ϣ
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_USER_QUERY_SCORE_INFO, dwContextID, &UserScoreInfo, sizeof(STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO));

	return true;
}

//pure���� ���а� 
bool CDataBaseEngineSink::On_DBR_CL_SERVICE_PURE_STANDING_LIST(DWORD dwContextID, void * pData, WORD wDataSize)
{
	try
	{
		if (sizeof(STR_SUB_CL_SERVICE_PURE_STANDING_LIST) != wDataSize)
		{
			return false;
		}

		STR_SUB_CL_SERVICE_PURE_STANDING_LIST* pDbReq = (STR_SUB_CL_SERVICE_PURE_STANDING_LIST*)pData;

		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("mystery"), _MYSTERY);
		m_AccountsDBAide.AddParameter(TEXT("dwUserID"), pDbReq->dwUserID);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_SERVICE_PURE_STANDING_LIST")); 
		//ִ�в�ѯ,ִ�г齱
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_SERVICE_PURE_STANDING_LIST"), true);		//��ѯ���ݿ�[QPAccountsDB]��LotteryList��ִ�г齱��������Ϸ��Ҳ�и�����齱��

		//�б���
		WORD wPacketSize=0;
		BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
		wPacketSize=0;
		STR_CMD_LC_SERVICE_PURE_STANDING_LIST * pDBO=NULL;
		STR_CMD_LC_SERVICE_PURE_STANDING_FINISH DBOFinish;
		ZeroMemory(&DBOFinish, sizeof(DBOFinish));

		while (m_AccountsDBModule->IsRecordsetEnd()==false)
		{
			//������Ϣ
			if ((wPacketSize+sizeof(STR_CMD_LC_SERVICE_PURE_STANDING_LIST))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_SERVICE_PURE_STANDING_LIST,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}
			//��ȡ��Ϣ
			pDBO=(STR_CMD_LC_SERVICE_PURE_STANDING_LIST *)(cbBuffer+wPacketSize);
			pDBO->byRanking = m_AccountsDBAide.GetValue_BYTE(TEXT("Ranking"));
			pDBO->dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
			pDBO->dwLoveLiness=m_AccountsDBAide.GetValue_DWORD(TEXT("LoveLiness"));
			m_AccountsDBAide.GetValue_String(TEXT("NickName"),pDBO->szNickName,CountArray(pDBO->szNickName));
			m_AccountsDBAide.GetValue_String(TEXT("HeadUrl"),pDBO->szHeadUrl,CountArray(pDBO->szHeadUrl));

			if(pDBO->dwUserID == pDbReq->dwUserID)
			{
				DBOFinish.dwRanking = pDBO->byRanking;
				DBOFinish.dwLoveLiness = pDBO->dwLoveLiness;
			}
			//����λ��
			wPacketSize+=sizeof(STR_CMD_LC_SERVICE_PURE_STANDING_LIST);

			//�ƶ���¼
			m_AccountsDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_SERVICE_PURE_STANDING_LIST,dwContextID,cbBuffer,wPacketSize);

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_SERVICE_PURE_STANDING_FINISH,dwContextID,&DBOFinish,sizeof(DBOFinish));
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;
}

//pure���� ���ս�� 
bool CDataBaseEngineSink::On_DBR_CL_SERVICE_PURE_RECORD_LIST(DWORD dwContextID, void * pData, WORD wDataSize)
{
	try
	{
		if (sizeof(STR_SUB_CL_SERVICE_PURE_RECORD_LIST) != wDataSize)
		{
			return false;
		}

		STR_SUB_CL_SERVICE_PURE_RECORD_LIST* pDbReq = (STR_SUB_CL_SERVICE_PURE_RECORD_LIST*)pData;

#pragma region �����Ϣ
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("dwUserID"), pDbReq->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("dwClubID"), pDbReq->dwClubID);
		m_TreasureDBAide.AddParameter(TEXT("tmQueryStartData"), pDbReq->tmQueryStartData);
		m_TreasureDBAide.AddParameter(TEXT("byMask"), pDbReq->byMask);
		//m_TreasureDBAide.AddParameter(TEXT("tmQueryEndData"), pDbReq->tmQueryEndData);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_SERVICE_PURE_RECORD_LIST")); 
		//ִ�в�ѯ,ִ�г齱
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_CL_SERVICE_PURE_RECORD_LIST"), true);

		//�б���
		WORD wPacketSize=0;
		BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
		wPacketSize=0;
		STR_CMD_LC_SERVICE_PURE_RECORD_LIST * pDBO=NULL;

		while ((lResultCode == DB_SUCCESS) && (m_TreasureDBModule->IsRecordsetEnd()==false))
		{
			//������Ϣ
			if ((wPacketSize+sizeof(STR_CMD_LC_SERVICE_PURE_RECORD_LIST))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_SERVICE_PURE_RECORD_LIST,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}
			//��ȡ��Ϣ
			pDBO=(STR_CMD_LC_SERVICE_PURE_RECORD_LIST *)(cbBuffer+wPacketSize);
			BYTE gameMode = m_TreasureDBAide.GetValue_BYTE(TEXT("ModeID"));
			//0���� 1����  2���  3�������
			if(0 == gameMode)
			{
				pDBO->byMask = 1;
			}
			else if((3 == gameMode) || (2 ==gameMode) )
			{
				pDBO->byMask = 2;
			}
			
			pDBO->dwDrawID = m_TreasureDBAide.GetValue_DWORD(TEXT("DrawID"));
			m_TreasureDBAide.GetValue_String(TEXT("KindName"),pDBO->szKindName,CountArray(pDBO->szKindName));
			pDBO->dwTableID=m_TreasureDBAide.GetValue_DWORD(TEXT("TableID"));
			SYSTEMTIME timeStartTime,tiemEndTime;
			m_TreasureDBAide.GetValue_SystemTime(TEXT("StartTime"), timeStartTime);
			m_TreasureDBAide.GetValue_SystemTime(TEXT("EndTime"), tiemEndTime);

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
			m_TreasureDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_SERVICE_PURE_RECORD_LIST,dwContextID,cbBuffer,wPacketSize);

#pragma endregion

#pragma region ����ϵ������Ϣ
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("dwUserID"), pDbReq->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("dwClubID"), pDbReq->dwClubID);
		m_TreasureDBAide.AddParameter(TEXT("tmQueryStartData"), pDbReq->tmQueryStartData);
		m_TreasureDBAide.AddParameter(TEXT("byMask"), pDbReq->byMask);
		//m_TreasureDBAide.AddParameter(TEXT("tmQueryEndData"), pDbReq->tmQueryEndData);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_SERVICE_PURE_RECORD_LIST_PLAYINFO")); 
		//ִ�в�ѯ,ִ�г齱
		LONG lResultCode2 = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_CL_SERVICE_PURE_RECORD_LIST_PLAYINFO"), true);

		//�б���
		WORD wPacketSize2=0;
		BYTE cbBuffer2[MAX_ASYNCHRONISM_DATA/10];
		wPacketSize2=0;
		STR_CMD_LC_SERVICE_PURE_RECORD_LIST_PLAYERINFO * pDBO2=NULL;
	
		while ((lResultCode2 == DB_SUCCESS ) && (m_TreasureDBModule->IsRecordsetEnd()==false))
		{
			//������Ϣ
			if ((wPacketSize2+sizeof(STR_CMD_LC_SERVICE_PURE_RECORD_LIST_PLAYERINFO))>sizeof(cbBuffer2))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_SERVICE_PURE_RECORD_LIST_PINFO,dwContextID,cbBuffer2,wPacketSize2);
				wPacketSize2=0;
			}
			//��ȡ��Ϣ
			pDBO2=(STR_CMD_LC_SERVICE_PURE_RECORD_LIST_PLAYERINFO*)(cbBuffer2+wPacketSize2);
			pDBO2->dwDrawID = m_TreasureDBAide.GetValue_DWORD(TEXT("DrawID"));
			pDBO2->dwUserID = m_TreasureDBAide.GetValue_DWORD(TEXT("UserID"));
			m_TreasureDBAide.GetValue_String(TEXT("NickName"),pDBO2->szNickName,CountArray(pDBO2->szNickName));
			m_TreasureDBAide.GetValue_String(TEXT("HeadUrl"),pDBO2->szHeadUrl,CountArray(pDBO2->szHeadUrl));
			pDBO2->lScore=m_TreasureDBAide.GetValue_LONGLONG(TEXT("Score"));

			//����λ��
			wPacketSize2+=sizeof(STR_CMD_LC_SERVICE_PURE_RECORD_LIST_PLAYERINFO);

			//�ƶ���¼
			m_TreasureDBModule->MoveToNext();
		}
		if (wPacketSize2>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_SERVICE_PURE_RECORD_LIST_PINFO,dwContextID,cbBuffer2,wPacketSize2);

#pragma endregion

#pragma region �����Ϣ����
		STR_CMD_LC_SERVICE_PURE_RECORD_LIST_FINIST DBOFinish;
		DBOFinish.byMask = 1;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_SERVICE_PURE_RECORD_FINISH,dwContextID,&DBOFinish,sizeof(DBOFinish));

		return true;
#pragma endregion
		}
		
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;
}

//pure���� С��ս�� 
bool CDataBaseEngineSink::On_DBR_CL_SERVICE_PURE_XJ_RECORD_LIST(DWORD dwContextID, void * pData, WORD wDataSize)
{
	try
	{
		if (sizeof(STR_SUB_CL_SERVICE_PURE_XJ_RECORD_LIST) != wDataSize)
		{
			return false;
		}

		STR_SUB_CL_SERVICE_PURE_XJ_RECORD_LIST* pDbReq = (STR_SUB_CL_SERVICE_PURE_XJ_RECORD_LIST*)pData;

#pragma region С����Ϣ
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("dwDrawID"), pDbReq->dwDrawID);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_SERVICE_PURE_XJ_RECORD_LIST")); 
		//ִ�в�ѯ
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_CL_SERVICE_PURE_XJ_RECORD_LIST"), true);

		//�б���
		WORD wPacketSize=0;
		BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
		wPacketSize=0;
		STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST * pDBO=NULL;

		if(lResultCode == DB_SUCCESS)
		{
			while (m_TreasureDBModule->IsRecordsetEnd()==false)
			{
				//������Ϣ
				if ((wPacketSize+sizeof(STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST))>sizeof(cbBuffer))
				{
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_SERVICE_PURE_XJ_RECORD_LIST,dwContextID,cbBuffer,wPacketSize);
					wPacketSize=0;
				}
				//��ȡ��Ϣ
				pDBO=(STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST *)(cbBuffer+wPacketSize);
				pDBO->bRoundCount = m_TreasureDBAide.GetValue_BYTE(TEXT("RoundCount"));
				pDBO->dwRecordID = m_TreasureDBAide.GetValue_DWORD(TEXT("RoundID"));

				//����λ��
				wPacketSize+=sizeof(STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST);

				//�ƶ���¼
				m_TreasureDBModule->MoveToNext();
			}
			if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_SERVICE_PURE_XJ_RECORD_LIST,dwContextID,cbBuffer,wPacketSize);
		}
		else
		{
			TCHAR szString[512]=TEXT("");
			_sntprintf_s(szString,CountArray(szString),TEXT("lResultCode = %ld"), lResultCode);

			//��ʾ��Ϣ
			CTraceService::TraceString(szString,TraceLevel_Warning);
		}
#pragma endregion

#pragma region С���ϵ������Ϣ
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("dwDrawID"), pDbReq->dwDrawID);
	
		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_SERVICE_PURE_XJ_RECORD_LIST_PLAYINFO")); 
		//ִ�в�ѯ,ִ�г齱
		LONG lResultCode2 = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_CL_SERVICE_PURE_XJ_RECORD_LIST_PLAYINFO"), true);

		//�б���
		WORD wPacketSize2=0;
		BYTE cbBuffer2[MAX_ASYNCHRONISM_DATA/10];
		STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_PLAYERINFO * pDBO2=NULL;
	
		if(lResultCode2 == DB_SUCCESS)
		{
			while (m_TreasureDBModule->IsRecordsetEnd()==false)
			{
				//������Ϣ
				if ((wPacketSize2+sizeof(STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_PLAYERINFO))>sizeof(cbBuffer2))
				{
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_SERVICE_PURE_XJ_RECORD_LIST_PINFO,dwContextID,cbBuffer2,static_cast<WORD>(wPacketSize2));
					wPacketSize2=0;
				}
				//��ȡ��Ϣ
				pDBO2=(STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_PLAYERINFO*)(cbBuffer2+wPacketSize2);
				pDBO2->bRoundCount = m_TreasureDBAide.GetValue_BYTE(TEXT("RoundCount"));
				pDBO2->dwUserID = m_TreasureDBAide.GetValue_DWORD(TEXT("UserID"));
				m_TreasureDBAide.GetValue_String(TEXT("NickName"),pDBO2->szNickName,CountArray(pDBO2->szNickName));
				pDBO2->lScore=m_TreasureDBAide.GetValue_LONGLONG(TEXT("Score"));

				//����λ��
				wPacketSize2+=sizeof(STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_PLAYERINFO);

				//�ƶ���¼
				m_TreasureDBModule->MoveToNext();
			}
			if (wPacketSize2>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_SERVICE_PURE_XJ_RECORD_LIST_PINFO,dwContextID,cbBuffer2,wPacketSize2);
		}
		else
		{
			TCHAR szString[512]=TEXT("");
			_sntprintf_s(szString,CountArray(szString),TEXT("lResultCode = %ld"), lResultCode2);

			//��ʾ��Ϣ
			CTraceService::TraceString(szString,TraceLevel_Warning);
		}
#pragma endregion

#pragma region С����Ϣ����
		STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_FINISH DBOFinish;
		DBOFinish.byMask = 1;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_SERVICE_PURE_XJ_RECORD_FINISH,dwContextID,&DBOFinish,sizeof(DBOFinish));
#pragma endregion
		}
		
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;
}

//С��¼��ط�
bool CDataBaseEngineSink::On_DBR_CL_Service_XJRecordPlayback(DWORD dwContextID, void * pData, WORD wDataSize)
{
	try
	{
		if (sizeof(STR_DBR_CL_SERVICE_XJ_RECORD_PLAYBACK) != wDataSize)
		{
			return false;
		}

		STR_DBR_CL_SERVICE_XJ_RECORD_PLAYBACK* pDbReq = (STR_DBR_CL_SERVICE_XJ_RECORD_PLAYBACK*)pData;

		//�������
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwRoundID"), pDbReq->dwRecordID);

		//�������
		WCHAR szDescribe[128] = L"";
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_GETVIDEORECORD"));
		//ִ�в�ѯ
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_CL_GETVIDEORECORD"), true);
		if (lResultCode == DB_SUCCESS)
		{
			//�������
			CDBVarValue DBVarValue;
			m_TreasureDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//����DBO����
			STR_DBO_LC_SERVICE_XJ_RECORD_PLAYBACK DBO;
			ZeroMemory(&DBO, sizeof(STR_DBO_LC_SERVICE_XJ_RECORD_PLAYBACK));

			//��ȡ¼������
			TCHAR szData[2*LEN_MAX_RECORD_SIZE];
			m_TreasureDBAide.GetValue_String(TEXT("VideoData"), szData, CountArray(szData));
			StrToBin(szData, DBO.cbRecordData, 0, LEN_MAX_RECORD_SIZE*2-1);

			DBO.dwDataSize = m_TreasureDBAide.GetValue_DWORD(TEXT("VideoSize"));
			DBO.wCurrentCount =  m_TreasureDBAide.GetValue_WORD(TEXT("CurrentRound"));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_SERVICE_XJ_RECORD_PLAYBACK, dwContextID, &DBO, sizeof(STR_DBO_LC_SERVICE_XJ_RECORD_PLAYBACK));
	
		}
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;
}

//�ͷ���Ϣ
bool CDataBaseEngineSink::On_DBR_CL_SERVICE_CUSTOMER_MESSEGE(DWORD dwContextID, void * pData, WORD wDataSize)
{
	try
	{
		if (sizeof(STR_DBR_CL_SERVICE_CUSTOMER_MESSEGE) != wDataSize)
		{
			return false;
		}

		STR_DBR_CL_SERVICE_CUSTOMER_MESSEGE* pDbReq = (STR_DBR_CL_SERVICE_CUSTOMER_MESSEGE*)pData;

		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@FirmID"), _MYSTERY);
		m_PlatformDBAide.AddParameter(TEXT("@MessageType"), pDbReq->cbMessegeFlag);
		
		//���ִ��
		LogicTraceHelper(TEXT("GSP_GP_GET_SERVICE_MESSAGE"));
		//ִ�в�ѯ
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_GET_SERVICE_MESSAGE"), true);
		if (lResultCode == DB_SUCCESS)
		{
			//����DBO����
			STR_DBO_LC_SERVICE_CUSTOMER_MESSEGE DBO;
			ZeroMemory(&DBO, sizeof(STR_DBO_LC_SERVICE_CUSTOMER_MESSEGE));

			//��ȡ¼������
			m_PlatformDBAide.GetValue_String(TEXT("MessageText"), DBO.szMessege, CountArray(DBO.szMessege));
			DBO.cbMessegeFlag = m_PlatformDBAide.GetValue_BYTE(TEXT("MessageType"));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_SERVICE_CUSTOMER_MESSEGE, dwContextID, &DBO, sizeof(STR_DBO_LC_SERVICE_CUSTOMER_MESSEGE));
		}
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;
}

//�����Ҵ�����Ϣ 
bool CDataBaseEngineSink::On_DBR_CL_SERVICE_GOLD_INFO(DWORD dwContextID, void * pData, WORD wDataSize)
{
	try
	{
		if (sizeof(STR_SUB_CL_SERVICE_GOLD_INFO) != wDataSize)
		{
			return false;
		}

		STR_SUB_CL_SERVICE_GOLD_INFO* pDbReq = (STR_SUB_CL_SERVICE_GOLD_INFO*)pData;

#pragma region �����Ҵ�����Ϣ
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("dwUserID"), pDbReq->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("dwModID"), pDbReq->dwModID);
		m_TreasureDBAide.AddParameter(TEXT("dwKindID"), pDbReq->dwKindID);
		m_TreasureDBAide.AddParameter(TEXT("mystery"), _MYSTERY);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_SERVICE_GOLD_INFO")); 
		//ִ�в�ѯ
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_CL_SERVICE_GOLD_INFO"), true);

		//�б���
		WORD wPacketSize=0;
		BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
		wPacketSize=0;
		STR_CMD_LC_SERVICE_GOLD_INFO * pDBO=NULL;

		while ((lResultCode == DB_SUCCESS) && m_TreasureDBModule->IsRecordsetEnd()==false)
		{
			//������Ϣ
			if ((wPacketSize+sizeof(STR_CMD_LC_SERVICE_GOLD_INFO))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_SERVICE_GOLD_INFO,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}
			//��ȡ��Ϣ
			pDBO=(STR_CMD_LC_SERVICE_GOLD_INFO *)(cbBuffer+wPacketSize);
			pDBO->byGoldMod = m_TreasureDBAide.GetValue_BYTE(TEXT("GoldMod"));
			pDBO->dwScore = m_TreasureDBAide.GetValue_DWORD(TEXT("GoldScore"));
			pDBO->dwGold = m_TreasureDBAide.GetValue_DWORD(TEXT("Gold"));
			pDBO->dwServiceGold = m_TreasureDBAide.GetValue_DWORD(TEXT("ServiceGold"));

			//����λ��
			wPacketSize+=sizeof(STR_CMD_LC_SERVICE_GOLD_INFO);

			//�ƶ���¼
			m_TreasureDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_SERVICE_GOLD_INFO,dwContextID,cbBuffer,wPacketSize);

#pragma endregion

#pragma region �����Ҵ�����Ϣ ����
		STR_CMD_LC_SERVICE_GOLD_INFO_FINISH DBOFinish;
		DBOFinish.byMask = 1;

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_SERVICE_GOLD_INFO_FINISH,dwContextID,&DBOFinish,sizeof(DBOFinish));
#pragma endregion
		}
		
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;
}


//�޸ĸ�������
bool CDataBaseEngineSink::On_DBR_Service_ModifyPersonalInfo(DWORD dwContextID, void * pData, WORD wDataSize)
{
	try
	{
		if (sizeof(STR_SUB_CL_SERVICE_MODIFY_PERSONAL_INFO) != wDataSize)
		{
			return false;
		}

		STR_SUB_CL_SERVICE_MODIFY_PERSONAL_INFO* pDbReq = (STR_SUB_CL_SERVICE_MODIFY_PERSONAL_INFO*)pData;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@UserID"), pDbReq->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@OldLogonPassword"), pDbReq->szOldLogonPassword);
		m_AccountsDBAide.AddParameter(TEXT("@NewLogonPassword"), pDbReq->szNewLogonPassword);		
		m_AccountsDBAide.AddParameter(TEXT("@NickName"), pDbReq->szNickName);
		m_AccountsDBAide.AddParameter(TEXT("@Gender"), pDbReq->cbGender);
		m_AccountsDBAide.AddParameter(TEXT("@HeadImageUrl"), pDbReq->szHeadImageUrl);
		m_AccountsDBAide.AddParameter(TEXT("@Signature"), pDbReq->szSignature);
		m_AccountsDBAide.AddParameter(TEXT("@RealName"), pDbReq->szRealName);
		m_AccountsDBAide.AddParameter(TEXT("@IDCardNum"), pDbReq->szIDCardNum);
		m_AccountsDBAide.AddParameter(TEXT("@PhoneNum"), pDbReq->szPhoneNum);
		m_AccountsDBAide.AddParameter(TEXT("@ProxyUserID"), pDbReq->dwProxyUserID);		//TODO ����ID�����ݿ�����ʱδ���ӣ���������

		//�������
		WCHAR szDescribe[128] = L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_Service_ModifyPersonalInfo"));
		//ִ�в�ѯ
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_Service_ModifyPersonalInfo"), true);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//����
		On_DBO_Service_ModifyPersonalInfo(dwContextID, lResultCode, CW2CT(DBVarValue.bstrVal));
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

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
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_SERVICE_MODIFY_PERSONAL_INFO, dwContextID, &PersonalInfo, sizeof(STR_DBO_CL_MODIFY_PERSONL_INFO));

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

	try
	{
		//�������
		m_AccountsDBAide.ResetParameter();

		//���ִ��
		LogicTraceHelper(TEXT("GSP_GP_UserRechangeInfo"));

		//�������
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserRechangeInfo"),true);

		On_DBO_Other_RechargeInfo(dwContextID, lResultCode, TEXT("���ݿ⴦�����!"));
	}
	catch(IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//������
		DBR_CommandSource wRequestID ;
		ZeroMemory(&wRequestID,sizeof(wRequestID));
		wRequestID.MainCommand = MDM_GIFT;
		wRequestID.SubCommand  = SUB_CL_OTHERS_RECHARGE_INFO;

		//�������
		On_DBO_CommonOperateResult(dwContextID, DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"), wRequestID);

		return false;
	}

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

		RechangeInfo.dwMinMoney = m_AccountsDBAide.GetValue_LONGLONG(TEXT("MinMoney"));
		RechangeInfo.dwChangeScale = m_AccountsDBAide.GetValue_UINT(TEXT("ChangeScale"));

		//��������
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_USER_RECHARGE_INFO, dwContextID, &RechangeInfo, sizeof(RechangeInfo));
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

	try
	{
		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@UserID"),pUserRequest->dwUserID);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_GP_UserExchangeInfo"));

		//�������
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserExchangeInfo"),true);

		On_DBO_Other_ExchangeInfo(dwContextID, lResultCode, TEXT("���ݿ����!"));
	}
	catch(IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//�������
		DBR_CommandSource wRequestID ;
		ZeroMemory(&wRequestID,sizeof(wRequestID));
		wRequestID.MainCommand = MDM_GIFT;
		wRequestID.SubCommand  = SUB_CL_OTHERS_EXCHANGE_INFO;

		On_DBO_CommonOperateResult(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),wRequestID);

		return false;
	}
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

		ExchangeInfo.dwMinMoney = m_AccountsDBAide.GetValue_LONGLONG(TEXT("MinMoney"));
		ExchangeInfo.dwChangeScale = m_AccountsDBAide.GetValue_UINT(TEXT("ChangeScale"));
		ExchangeInfo.dwBankMoney = m_AccountsDBAide.GetValue_LONGLONG(TEXT("BankMoney"));
		ExchangeInfo.dwWithdrawals = m_AccountsDBAide.GetValue_LONGLONG(TEXT("Withdrawals"));

		//��������
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_USER_EXCHANGE_INFO,dwContextID,&ExchangeInfo,sizeof(ExchangeInfo));
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

#pragma region ��������
//�����б�
bool CDataBaseEngineSink::OnRequestLoadGameList(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//��������
		WORD wPacketSize=0;
		BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];

		//��������
		m_PlatformDBAide.ResetParameter();

		//���ִ��
		LogicTraceHelper(TEXT("GSP_GP_LoadGameTypeItem"));

		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadGameTypeItem"),true);

		//��������
		wPacketSize=0;
		tagGameType * pGameType=NULL;
		while (m_PlatformDBModule->IsRecordsetEnd()==false)
		{
			//������Ϣ
			if ((wPacketSize+sizeof(tagGameType))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_TYPE_ITEM,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//��ȡ��Ϣ
			pGameType=(tagGameType *)(cbBuffer+wPacketSize);
			pGameType->wTypeID=m_PlatformDBAide.GetValue_WORD(TEXT("TypeID"));
			m_PlatformDBAide.GetValue_String(TEXT("TypeName"),pGameType->szTypeName,CountArray(pGameType->szTypeName));

			//����λ��
			wPacketSize+=sizeof(tagGameType);

			//�ƶ���¼
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_TYPE_ITEM,dwContextID,cbBuffer,wPacketSize);

		//��ȡ����
		m_PlatformDBAide.ResetParameter();

		//���ִ��
		LogicTraceHelper(TEXT("GSP_GP_LoadGameKindItem"));

		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadGameKindItem"),true);

		//��������
		wPacketSize=0;
		tagGameKind * pGameKind=NULL;
		while (m_PlatformDBModule->IsRecordsetEnd()==false)
		{
			//������Ϣ
			if ((wPacketSize+sizeof(tagGameKind))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_KIND_ITEM,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//��ȡ��Ϣ
			pGameKind=(tagGameKind *)(cbBuffer+wPacketSize);
			pGameKind->wKindID=m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
			pGameKind->wTypeID=m_PlatformDBAide.GetValue_WORD(TEXT("TypeID"));
			m_PlatformDBAide.GetValue_String(TEXT("KindName"),pGameKind->szKindName,CountArray(pGameKind->szKindName));

			//����λ��
			wPacketSize+=sizeof(tagGameKind);

			//�ƶ���¼
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_KIND_ITEM,dwContextID,cbBuffer,wPacketSize);

		//��ȡ�ڵ�
		m_PlatformDBAide.ResetParameter();

		//���ִ��
		LogicTraceHelper(TEXT("GSP_GP_LoadGameNodeItem"));

		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadGameNodeItem"),true);

		//���ͽڵ�
		wPacketSize=0;
		tagGameNode * pGameNode=NULL;
		while (m_PlatformDBModule->IsRecordsetEnd()==false)
		{
			//������Ϣ
			if ((wPacketSize+sizeof(tagGameNode))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_NODE_ITEM,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//��ȡ��Ϣ
			pGameNode=(tagGameNode *)(cbBuffer+wPacketSize);
			pGameNode->wKindID=m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
			pGameNode->wNodeID=m_PlatformDBAide.GetValue_WORD(TEXT("NodeID"));
			m_PlatformDBAide.GetValue_String(TEXT("NodeName"),pGameNode->szNodeName,CountArray(pGameNode->szNodeName));

			//����λ��
			wPacketSize+=sizeof(tagGameNode);

			//�ƶ���¼
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_NODE_ITEM,dwContextID,cbBuffer,wPacketSize);

		//��������
		DBO_GP_GameListResult GameListResult;
		ZeroMemory(&GameListResult,sizeof(GameListResult));

		//���ñ���
		GameListResult.cbSuccess=TRUE;

		//������Ϣ
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_LIST_RESULT,dwContextID,&GameListResult,sizeof(GameListResult));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//��������
		DBO_GP_GameListResult GameListResult;
		ZeroMemory(&GameListResult,sizeof(GameListResult));

		//���ñ���
		GameListResult.cbSuccess=FALSE;

		//������Ϣ
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_LIST_RESULT,dwContextID,&GameListResult,sizeof(GameListResult));

		return false;
	}

	return true;
}

//����ͳ�� -- �������ݿ�
bool CDataBaseEngineSink::OnRequestOnLineCountInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		if (wDataSize != sizeof(DBR_GP_OnLineCountInfo)) return false;

		//��������
		DBR_GP_OnLineCountInfo * pOnLineCountInfo=(DBR_GP_OnLineCountInfo *)pData;

		//������Ϣ
		TCHAR szOnLineCountGame[2048]=TEXT("");
		for (WORD i=0; i<pOnLineCountInfo->dwGameCount; i++)
		{
			INT nLength=lstrlen(szOnLineCountGame);
			_sntprintf(&szOnLineCountGame[nLength],CountArray(szOnLineCountGame)-nLength,
				TEXT("%ld:%ld;"),
				pOnLineCountInfo->OnLineCountGame[i].dwGameID,
				pOnLineCountInfo->OnLineCountGame[i].dwOnLineCount);
		}


		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@byCompanyID"),_MYSTERY);
		m_PlatformDBAide.AddParameter(TEXT("@dwGameCount"),pOnLineCountInfo->dwGameCount);
		m_PlatformDBAide.AddParameter(TEXT("@dwOnLineCountSum"),pOnLineCountInfo->dwOnLineCountSum);
		m_PlatformDBAide.AddParameter(TEXT("@strOnLineCountGame"),szOnLineCountGame);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_GP_OnLineCountInfo"));

		//ִ������
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_OnLineCountInfo"),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

//�����������Ϣ
bool CDataBaseEngineSink::On_DBR_UPDATA_MARQUEE(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@byCompanyID"),_MYSTERY);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_GP_UPDATA_MARQUEE"));
		//ִ������
		long lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_UPDATA_MARQUEE"),true);

		//��������
		WORD wPacketSize=0;
		BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
		STR_DBO_UPDATA_MARQUEE * pDbo=NULL;
		while ((lResultCode == DB_SUCCESS) && (m_PlatformDBModule->IsRecordsetEnd()==false))
		{
			//������Ϣ
			if ((wPacketSize+sizeof(STR_DBO_UPDATA_MARQUEE))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_UPDATA_MARQUEE,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//��ȡ��Ϣ
			pDbo=(STR_DBO_UPDATA_MARQUEE *)(cbBuffer+wPacketSize);
			pDbo->byMask  = 1; //��������
			pDbo->dwMarqueeID  =  m_PlatformDBAide.GetValue_DWORD(TEXT("MarqueeID"));
			pDbo->dwMaruqeeTime = m_PlatformDBAide.GetValue_DWORD(TEXT("MaruqeeTime"));
			m_PlatformDBAide.GetValue_SystemTime(TEXT("StartTime"), pDbo->timeStartTime);
			m_PlatformDBAide.GetValue_SystemTime(TEXT("EndTime"), pDbo->timeEndTime);
			m_PlatformDBAide.GetValue_String(TEXT("MarqueeMsg"),pDbo->szMarqueeMsg,CountArray(pDbo->szMarqueeMsg));

			//����λ��
			wPacketSize+=sizeof(STR_DBO_UPDATA_MARQUEE);

			//�ƶ���¼
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_UPDATA_MARQUEE,dwContextID,cbBuffer,wPacketSize);

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_UPDATA_MARQUEE_FINISH,dwContextID,NULL,0);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}

	return true;
}

#pragma endregion

//��ѯ�û���ҷ���
bool CDataBaseEngineSink::On_DBR_QueryScoreInfo(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(STR_DBR_CL_SERCIVR_QUERY_SCORE_INFO));
		if (wDataSize!=sizeof(STR_DBR_CL_SERCIVR_QUERY_SCORE_INFO)) return false;

		//������
		STR_DBR_CL_SERCIVR_QUERY_SCORE_INFO * pQueryScoreInfo =(STR_DBR_CL_SERCIVR_QUERY_SCORE_INFO *)pData;

		//�������
		m_TreasureDBAide.ResetParameter();
		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"),pQueryScoreInfo->dwUserID);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_Service_GetUserScoreInfo"));

		//ִ�в�ѯ
		LONG lResultCode=m_TreasureDBAide.ExecuteProcess(TEXT("GSP_CL_Service_GetUserScoreInfo"),true);

		//�������
		On_DBO_QueryScoreInfo(dwContextID,lResultCode,NULL,false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//�������
		//OnOperateDisposeResult(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����ԣ�"),false);

		return false;
	}

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
		UserScoreInfo.dwUserID = m_TreasureDBAide.GetValue_DWORD(TEXT("UserID"));
		UserScoreInfo.lGold = m_TreasureDBAide.GetValue_LONGLONG(TEXT("Gold"));
		UserScoreInfo.lOpenRoomCard = m_TreasureDBAide.GetValue_LONGLONG(TEXT("OpenRoomCard"));
		UserScoreInfo.lDiamond = m_TreasureDBAide.GetValue_LONGLONG(TEXT("Diamond"));

		//���ͽ��
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_USER_QUERY_SCORE_INFO,dwContextID,&UserScoreInfo,sizeof(STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO));
	}
	else
	{
		//���ͽ��
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_USER_QUERY_SCORE_INFO,dwContextID,&UserScoreInfo,sizeof(STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO));
	}

	return;
}

//�û�Socket���ӹر�
bool CDataBaseEngineSink::On_DBR_GP_QUIT(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		if (wDataSize!=sizeof(DBR_GP_UserQuitInfo)) return false;

		DBR_GP_UserQuitInfo* groupInfo = (DBR_GP_UserQuitInfo*)pData;
		//�������
		m_PlatformDBAide.ResetParameter();

		m_PlatformDBAide.AddParameter(TEXT("@dwUserID"),groupInfo->dwUserID);
		m_PlatformDBAide.AddParameter(TEXT("@byOnlineMask"),groupInfo->byOnlineMask);

		//ִ�����
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_CL_Quit"),true);	

		return true;
	}
	catch(IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);
		return false;
	}

}

//�������а��û���Ϣ
bool CDataBaseEngineSink::OnDBUpdateRankUserItem(DWORD dwContextID, void * pData, WORD wDataSize)
{
	try
	{
		if (sizeof(DBR_GP_UserRankItem) < wDataSize)
		{
			return false;
		}

		DBR_GP_UserRankItem* pDbReq = (DBR_GP_UserRankItem*)pData;

		m_AccountsDBAide.ResetParameter();

		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pDbReq->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@wRankNum"), pDbReq->byRankNum);
		m_AccountsDBAide.AddParameter(TEXT("@wReceived"), pDbReq->byReceived);
		m_AccountsDBAide.AddParameter(TEXT("@dwCount"), pDbReq->dwCount);
		m_AccountsDBAide.AddParameter(TEXT("@wIndex"), pDbReq->byIndex);

		//�������
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_GP_UpdateRankUserItem"));

		//ִ�в�ѯ
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UpdateRankUserItem"), true);
		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		if(lResultCode == DB_SUCCESS)
		{

		}
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;
}


//��ȡ���а���Ϣ
bool CDataBaseEngineSink::OnDBReadRankList(DWORD dwContextID, void * pData, WORD wDataSize)
{
	try
	{
		if (sizeof(RankManager) != wDataSize && NULL != pData)
		{
			return false;
		}

		RankManager* pRankManager = (RankManager*)pData;
		m_AccountsDBAide.ResetParameter();
		//���ִ��
		//LogicTraceHelper(TEXT("GSP_GP_GetRankList"));

		/* ɾ�� ���ΰ� ������ ���а��� added by WangChengQing 2018/6/4
		//ִ�в�ѯ
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_GetRankList"), true);
		//�������
		if(lResultCode == DB_SUCCESS)
		{
			pRankManager->InitRankList(m_AccountsDBAide,m_AccountsDBModule);
		}
		*/
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;

}

//��ȡ������
bool CDataBaseEngineSink::OnReceiveRankReward(DWORD dwContextID, void * pData, WORD wDataSize)
{
	try
	{
		if (sizeof(STR_DBR_CL_SERCIVR_GET_RANK_REWARD) < wDataSize)
		{
			return false;
		}

		STR_DBR_CL_SERCIVR_GET_RANK_REWARD* pDbReq = (STR_DBR_CL_SERCIVR_GET_RANK_REWARD*)pData;

		m_AccountsDBAide.ResetParameter();

		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pDbReq->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@byIndex"), pDbReq->byIndex);

		//�������
		WCHAR szDescribe[128]=L"";
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_GP_GetRankReward"));

		//ִ�в�ѯ
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_GetRankReward"), true);
		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		if(lResultCode == DB_SUCCESS)
		{
			//��ȡ�����û���ҷ�����ʯ
			STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO UserScoreInfo;
			ZeroMemory(&UserScoreInfo,sizeof(STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO));

			UserScoreInfo.dwUserID = pDbReq->dwUserID;
			UserScoreInfo.lGold = m_AccountsDBAide.GetValue_LONG(TEXT("@Gold"));
			UserScoreInfo.lOpenRoomCard = m_AccountsDBAide.GetValue_LONG(TEXT("@OpenRoomCard"));
			UserScoreInfo.lDiamond = m_AccountsDBAide.GetValue_LONG(TEXT("@Diamond"));

			//�����ʲ�������Ϣ
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_USER_QUERY_SCORE_INFO,dwContextID,&UserScoreInfo,sizeof(STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO));
		}

		//�յ�������
		STR_DBO_CL_SERCIVR_GET_TASK_REWARD result;
		ZeroMemory(&result,sizeof(result));
		result.lResultCode = lResultCode;
		lstrcpyn( result.szDescribe, CW2CT(DBVarValue.bstrVal), CountArray( result.szDescribe));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_SERVICE_GET_RANK_REWARD,dwContextID,&result,sizeof(result));

	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;

}

//�޸��û��Ƹ���Ϣ
bool CDataBaseEngineSink::OnModifyUserInsure(DWORD dwContextID, void * pData, WORD wDataSize)
{
	try
	{
		if (sizeof(DBR_GP_ModifyUserInsure) < wDataSize)
		{
			return false;
		}

		DBR_GP_ModifyUserInsure* pDbReq = (DBR_GP_ModifyUserInsure*)pData;

		m_TreasureDBAide.ResetParameter();

		m_TreasureDBAide.AddParameter(TEXT("@dwUserID"), pDbReq->dwUserID);
		m_TreasureDBAide.AddParameter(TEXT("@lGold"), pDbReq->lGold);
		m_TreasureDBAide.AddParameter(TEXT("@lOpenRoomCard"), pDbReq->lOpenRoomCard);
		m_TreasureDBAide.AddParameter(TEXT("@lDiamond"), pDbReq->lDiamond);
		m_TreasureDBAide.AddParameter(TEXT("@lRewardCard"), pDbReq->lRewardCard);
		m_TreasureDBAide.AddParameter(TEXT("@lScore"), pDbReq->lScore);

		//�������
		WCHAR szDescribe[128]=L"";
		m_TreasureDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_GP_GetRankReward"));

		//ִ�в�ѯ
		LONG lResultCode = m_TreasureDBAide.ExecuteProcess(TEXT("GSP_GP_GetRankReward"), true);
		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		if(lResultCode == DB_SUCCESS)
		{
			//��ȡ�����û���ҷ�����ʯ
			STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO UserScoreInfo;
			ZeroMemory(&UserScoreInfo,sizeof(STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO));

			UserScoreInfo.dwUserID = pDbReq->dwUserID;
			UserScoreInfo.lGold = m_AccountsDBAide.GetValue_LONG(TEXT("@Gold"));
			UserScoreInfo.lOpenRoomCard = m_AccountsDBAide.GetValue_LONG(TEXT("@OpenRoomCard"));
			UserScoreInfo.lDiamond = m_AccountsDBAide.GetValue_LONG(TEXT("@Diamond"));
			UserScoreInfo.lRewardCard = m_AccountsDBAide.GetValue_LONG(TEXT("@RewardCard"));
			UserScoreInfo.lScore = m_AccountsDBAide.GetValue_LONG(TEXT("@Score"));

			//���ͲƸ������Ϣ
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_USER_QUERY_SCORE_INFO,dwContextID,&UserScoreInfo,sizeof(STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO));
		}
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;

}

//�������û����͵���
bool CDataBaseEngineSink::On_DBR_CL_GIFT_GIVE_PROPS(DWORD dwContextID, void * pData, WORD wDataSize)
{
	try
	{
		if (sizeof(STR_SUB_CL_GIFT_GIVE_PROPS) != wDataSize)
		{
			return false;
		}

		STR_SUB_CL_GIFT_GIVE_PROPS* pDbReq = (STR_SUB_CL_GIFT_GIVE_PROPS*)pData;

		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwSrcUserID"), pDbReq->dwSrcUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwDstUserID"), pDbReq->dwDstUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwPropsID"), pDbReq->dwPropsID);
		m_AccountsDBAide.AddParameter(TEXT("@dwCount"), pDbReq->dwCount);

		//�������
		TCHAR szDescribe[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_GIFT_GIVE_PROPS")); 
		//ִ�в�ѯ
		long lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_GIFT_GIVE_PROPS"), true);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		STR_CMD_LC_GIFT_GIVE_PROPS CMD;
		CMD.dwResultCode = lResultCode;
		lstrcpyn( CMD.szDescribeString, CW2CT(DBVarValue.bstrVal), CountArray(CMD.szDescribeString));

		if(lResultCode == DB_SUCCESS)
		{
			CMD.dwPropsID = pDbReq->dwPropsID;
			CMD.dwCount = m_AccountsDBAide.GetValue_DWORD(TEXT("GoodsCount"));
			CMD.dwLoveness = m_AccountsDBAide.GetValue_DWORD(TEXT("Loveness"));

			STR_CMD_LC_GIFT_GIVE_PROPS_SHOW DBO;
			DBO.dwTargetID = pDbReq->dwDstUserID;
			lstrcpyn( DBO.szDescribeString, CW2CT(DBVarValue.bstrVal), CountArray(DBO.szDescribeString));

			//����DBO
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_GIFT_GIVE_PROPS_SHOW,dwContextID,&DBO,sizeof(DBO));
		}

		//����DBO
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_GIFT_GIVE_PROPS,dwContextID,&CMD,sizeof(CMD));

	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;
}

#pragma region MDM_LOGON ��¼ģ��
/***************************** ��¼����*******************************************/
//�ʺŵ�¼
bool CDataBaseEngineSink::On_DBR_Logon_Accounts(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		if (wDataSize!=sizeof(STR_DBR_CL_LOGON_ACCOUNTS)) return false;

		//������
		STR_DBR_CL_LOGON_ACCOUNTS * pDBRLogonAccounts=(STR_DBR_CL_LOGON_ACCOUNTS *)pData;

		//Socket�ж�
		tagBindParameter * pBindParameter=(tagBindParameter *)pDBRLogonAccounts->pBindParameter;
		if (pBindParameter->dwSocketID!=dwContextID) return true;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pBindParameter->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);
		
		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@mystery"),_MYSTERY);
		m_AccountsDBAide.AddParameter(TEXT("@strAccounts"),pDBRLogonAccounts->szAccounts);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pDBRLogonAccounts->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pDBRLogonAccounts->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@ProxyID"),pDBRLogonAccounts->dwProxyID);

		//������ʾ
		TCHAR szString[512]=TEXT("");
		_sntprintf_s(szString,CountArray(szString),
			TEXT("�˺ŵ�¼ ���� = %ld"),
			pDBRLogonAccounts->dwProxyID);
		CTraceService::TraceString(szString,TraceLevel_Normal);

		//�������ssss
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_Logon_Accounts"));
		//ִ�в�ѯ
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_Logon_Accounts"),true);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		On_DBO_Logon_Accounts(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal));
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//������
		On_DBO_Logon_Accounts(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����Ի�ѡ����һ��������¼��"));

		return false;
	}

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
		DBOLogonAccount.dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
		//�û��ǳ�
		m_AccountsDBAide.GetValue_String(TEXT("NickName"),DBOLogonAccount.szNickName,CountArray(DBOLogonAccount.szNickName));
		//�û��Ա�
		DBOLogonAccount.cbGender=m_AccountsDBAide.GetValue_BYTE(TEXT("Gender"));
		//ͷ������
		m_AccountsDBAide.GetValue_String(TEXT("HeadUrl"),DBOLogonAccount.szHeadUrl,CountArray(DBOLogonAccount.szHeadUrl));
		//����ǩ��
		m_AccountsDBAide.GetValue_String(TEXT("MySignature"),DBOLogonAccount.szMySignature,CountArray(DBOLogonAccount.szMySignature));

		//�û�Ԫ��
		DBOLogonAccount.dwUserDiamond=m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserDiamond"));
		//�û�����
		DBOLogonAccount.dwUserMedal=m_AccountsDBAide.GetValue_DWORD(TEXT("UserMedal"));
		//������ֵ
		DBOLogonAccount.byLevel=m_AccountsDBAide.GetValue_BYTE(TEXT("UserLevel"));
		//�û�����
		DBOLogonAccount.dwLoveLiness=m_AccountsDBAide.GetValue_DWORD(TEXT("LoveLiness"));

		//����Ա�ȼ�
		DBOLogonAccount.cbMasterOrder=m_AccountsDBAide.GetValue_BYTE(TEXT("MasterOrder"));
		//��Ա�ȼ�
		DBOLogonAccount.cbMemberOrder=m_AccountsDBAide.GetValue_BYTE(TEXT("MemberOrder"));
		//��Ա����ʱ��
		m_AccountsDBAide.GetValue_SystemTime(TEXT("MemberOverDate"),DBOLogonAccount.MemberOverDate);

		//��ʵ����
		m_AccountsDBAide.GetValue_String(TEXT("IdentityName"),DBOLogonAccount.szIdentityName,CountArray(DBOLogonAccount.szIdentityName));
		//���֤��
		m_AccountsDBAide.GetValue_String(TEXT("IdentityNum"),DBOLogonAccount.szIdentityNum,CountArray(DBOLogonAccount.szIdentityNum));
		//�ֻ�����
		m_AccountsDBAide.GetValue_String(TEXT("MobilePhone"),DBOLogonAccount.szMobilePhone,CountArray(DBOLogonAccount.szMobilePhone));
		/* *****************************    �˺���Ϣ     ****************************/
		//����¼��ַ
		m_AccountsDBAide.GetValue_String(TEXT("LastLogonIP"),DBOLogonAccount.szLasLogonIp,CountArray(DBOLogonAccount.szLasLogonIp));
		//�������ʱ�� 
		m_AccountsDBAide.GetValue_SystemTime(TEXT("LastLogonDate"),DBOLogonAccount.LasLogonDate);

		/* *****************************    ��������     ****************************/
		//�û�����
		DBOLogonAccount.lUserScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserScore"));
		//�û���Ϸ��
		DBOLogonAccount.lUserGold = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserGold"));
		//�û�����
		DBOLogonAccount.lOpenRoomCard = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserRoomCard"));
	}

	//��������
	DBOLogonAccount.dwResultCode=dwResultCode;
	lstrcpyn(DBOLogonAccount.szDescribeString,pszErrorString,CountArray(DBOLogonAccount.szDescribeString));

	//���ͽ��
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_LOGON_ACCOUNTS,dwContextID,&DBOLogonAccount, sizeof(DBOLogonAccount));

	return true;
}

//ע�ᴦ��
bool CDataBaseEngineSink::On_DBR_Logon_Register(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		if (wDataSize!=sizeof(STR_DBR_CL_LOGON_REGISTER)) return false;

		//������
		STR_DBR_CL_LOGON_REGISTER * pDBRLogonRegister=(STR_DBR_CL_LOGON_REGISTER *)pData;

		//Socket�ж�
		tagBindParameter * pBindParameter=(tagBindParameter *)pDBRLogonRegister->pBindParameter;
		if (pBindParameter->dwSocketID!=dwContextID) return true;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pBindParameter->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@byMYSTERY"), _MYSTERY);
		m_AccountsDBAide.AddParameter(TEXT("@strAccounts"),pDBRLogonRegister->szAccounts);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pDBRLogonRegister->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strNickName"),pDBRLogonRegister->szNickName);
		m_AccountsDBAide.AddParameter(TEXT("@cbGender"),pDBRLogonRegister->cbGender);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"),pDBRLogonRegister->strMobilePhone);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pDBRLogonRegister->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@dwProxyID"),pDBRLogonRegister->dwProxyID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_Logon_Register"));

		//ִ�в�ѯ
		LONG lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_Logon_Register"),true);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		On_DBO_Logon_Accounts(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//������
		On_DBO_Logon_Accounts(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����Ի�ѡ����һ��������¼��"));

		return false;
	}

	return true;
}

//ƽ̨��¼
bool CDataBaseEngineSink::On_DBR_Logon_Platform(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(STR_DBR_CL_LOGON_PLATFORM));
		if (wDataSize!=sizeof(STR_DBR_CL_LOGON_PLATFORM)) return false;

		//������
		STR_DBR_CL_LOGON_PLATFORM * pDBRLogonPlatform=(STR_DBR_CL_LOGON_PLATFORM *)pData;

		//Socket�ж�
		tagBindParameter * pBindParameter=(tagBindParameter *)pDBRLogonPlatform->pBindParameter;
		if (pBindParameter->dwSocketID!=dwContextID) return true;

		//ת����ַ
		TCHAR szClientAddr[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pBindParameter->dwClientAddr;
		_sntprintf_s(szClientAddr,CountArray(szClientAddr),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@byMystery"), _MYSTERY);
		m_AccountsDBAide.AddParameter(TEXT("@strOpenID"), pDBRLogonPlatform->szOpenID);
		m_AccountsDBAide.AddParameter(TEXT("@szUnionID"), pDBRLogonPlatform->szUnionID);
		m_AccountsDBAide.AddParameter(TEXT("@strNickName"), pDBRLogonPlatform->szNickName);
		m_AccountsDBAide.AddParameter(TEXT("@cbGender"), pDBRLogonPlatform->cbGender);
		m_AccountsDBAide.AddParameter(TEXT("@strHeadUrl"), pDBRLogonPlatform->strHeadUrl);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pDBRLogonPlatform->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@dwProxyID"), pDBRLogonPlatform->dwProxyID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_Logon_Platform"));
		//ִ�в�ѯ			
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_Logon_Platform"),true);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		On_DBO_Logon_Platform(dwContextID,lResultCode,CW2CT(DBVarValue.bstrVal));
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		//������
		On_DBO_Logon_Platform(dwContextID,DB_ERROR,TEXT("�������ݿ�����쳣�������Ժ����Ի�ѡ����һ��������¼��"));

		return false;
	}

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
		DBOLogonPlatform.dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
		//�û��ǳ�
		m_AccountsDBAide.GetValue_String(TEXT("NickName"),DBOLogonPlatform.szNickName,CountArray(DBOLogonPlatform.szNickName));
		//�û��Ա�
		DBOLogonPlatform.cbGender=m_AccountsDBAide.GetValue_BYTE(TEXT("Gender"));
		//ͷ������
		m_AccountsDBAide.GetValue_String(TEXT("HeadUrl"),DBOLogonPlatform.szHeadUrl,CountArray(DBOLogonPlatform.szHeadUrl));
		//����ǩ��
		m_AccountsDBAide.GetValue_String(TEXT("MySignature"),DBOLogonPlatform.szMySignature,CountArray(DBOLogonPlatform.szMySignature));

		//�û�Ԫ��
		DBOLogonPlatform.dwUserDiamond=m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserDiamond"));
		//�û�����
		DBOLogonPlatform.dwUserMedal=m_AccountsDBAide.GetValue_DWORD(TEXT("UserMedal"));
		//������ֵ
		DBOLogonPlatform.byLevel=m_AccountsDBAide.GetValue_BYTE(TEXT("UserLevel"));
		//�û�����
		DBOLogonPlatform.dwLoveLiness=m_AccountsDBAide.GetValue_DWORD(TEXT("LoveLiness"));

		//����Ա�ȼ�
		DBOLogonPlatform.cbMasterOrder=m_AccountsDBAide.GetValue_BYTE(TEXT("MasterOrder"));
		//��Ա�ȼ�
		DBOLogonPlatform.cbMemberOrder=m_AccountsDBAide.GetValue_BYTE(TEXT("MemberOrder"));
		//��Ա����ʱ��
		m_AccountsDBAide.GetValue_SystemTime(TEXT("MemberOverDate"),DBOLogonPlatform.MemberOverDate);

		//��ʵ����
		m_AccountsDBAide.GetValue_String(TEXT("IdentityName"),DBOLogonPlatform.szIdentityName,CountArray(DBOLogonPlatform.szIdentityName));
		//���֤��
		m_AccountsDBAide.GetValue_String(TEXT("IdentityNum"),DBOLogonPlatform.szIdentityNum,CountArray(DBOLogonPlatform.szIdentityNum));
		//�ֻ�����
		m_AccountsDBAide.GetValue_String(TEXT("MobilePhone"),DBOLogonPlatform.szMobilePhone,CountArray(DBOLogonPlatform.szMobilePhone));
		/* *****************************    �˺���Ϣ     ****************************/
		//����¼��ַ
		m_AccountsDBAide.GetValue_String(TEXT("LastLogonIP"),DBOLogonPlatform.szLasLogonIp,CountArray(DBOLogonPlatform.szLasLogonIp));
		//�������ʱ��
		m_AccountsDBAide.GetValue_SystemTime(TEXT("LastLogonDate"),DBOLogonPlatform.LasLogonDate);

		/* *****************************    ��������     ****************************/
		//�û����� TODONOW ���ݿ�ȱ�ٸ��ֶ�
		//DBOLogonPlatform.lUserScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserScore"));
		//�û���Ϸ��
		DBOLogonPlatform.lUserGold = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserGold"));
		//�û�����
		DBOLogonPlatform.lOpenRoomCard = m_AccountsDBAide.GetValue_LONGLONG(TEXT("OpenRoomCard"));
	}

	//��������
	DBOLogonPlatform.dwResultCode=dwResultCode;
	lstrcpyn(DBOLogonPlatform.szDescribeString,pszErrorString,CountArray(DBOLogonPlatform.szDescribeString));

	//���ͽ��
	WORD wDataSize=CountStringBuffer(DBOLogonPlatform.szDescribeString);
	WORD wHeadSize=sizeof(DBOLogonPlatform)-sizeof(DBOLogonPlatform.szDescribeString);

	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_LOGON_PLATFORM,dwContextID,&DBOLogonPlatform,wHeadSize+wDataSize);
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
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_USER_COMMAND_RESULT,dwContextID,&OperateResult,wHeadSize+wDataSize);

	return;
}
#pragma region MDM_CLUB ����Ȧ(�汾2)
//��ѯ����Ȧ�б�
bool CDataBaseEngineSink::On_DBR_CL_CLUB_ALL_CLUB_INFO_LIST(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		if (wDataSize!=sizeof(STR_SUB_CL_CLUB_ALL_CLUB_INFO_LIST)) return false;
		STR_SUB_CL_CLUB_ALL_CLUB_INFO_LIST *pDbr = (STR_SUB_CL_CLUB_ALL_CLUB_INFO_LIST *)pData;

		//��������
		m_AccountsDBAide.ResetParameter();

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_CLUB_ALL_CLUB_INFO_LIST"));

		m_AccountsDBAide.AddParameter(TEXT("@UserID"),pDbr->dwUserID);
		m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_ALL_CLUB_INFO_LIST"),true);

		//�б���
		WORD wPacketSize=0;
		BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
		wPacketSize=0;
		STR_CMD_LC_CLUB_ALL_CLUB_INFO_LIST * pCMD=NULL;
		while (m_AccountsDBModule->IsRecordsetEnd()==false)
		{
			//������Ϣ
			if ((wPacketSize+sizeof(STR_CMD_LC_CLUB_ALL_CLUB_INFO_LIST))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_ALL_CLUB_INFO_LIST,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//��ȡ��Ϣ
			pCMD=(STR_CMD_LC_CLUB_ALL_CLUB_INFO_LIST *)(cbBuffer+wPacketSize);
			pCMD->dwClubID=m_AccountsDBAide.GetValue_DWORD(TEXT("ClubID"));	
			m_AccountsDBAide.GetValue_String(TEXT("ClubName"),pCMD->szClubName,CountArray(pCMD->szClubName));
			pCMD->byClubRole=m_AccountsDBAide.GetValue_BYTE(TEXT("ClubRole"));

			//����λ��
			wPacketSize+=sizeof(STR_CMD_LC_CLUB_ALL_CLUB_INFO_LIST);

			//�ƶ���¼
			m_AccountsDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_ALL_CLUB_INFO_LIST,dwContextID,cbBuffer,wPacketSize);

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_ALL_INFO_FINISH,dwContextID,NULL,0);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}
}

//��ѯָ������Ȧ�����б�
bool CDataBaseEngineSink::On_DBR_CL_CLUB_ROOM_LIST(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		if (wDataSize!=sizeof(STR_DBR_CL_CLUB_ROOM_LIST)) return false;

		STR_DBR_CL_CLUB_ROOM_LIST *pDBR = (STR_DBR_CL_CLUB_ROOM_LIST*) pData;

		STR_DBR_CL_CLUB_ROOM_LIST DBR;
		DBR.dwClubID = pDBR->dwClubID;

		//��������
		m_AccountsDBAide.ResetParameter();

		//�������
		m_AccountsDBAide.AddParameter(TEXT("@ClubID"),DBR.dwClubID);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_CLUB_ROOM_LIST"));

		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_ROOM_LIST"),true);

		//�б���
		//��������
		WORD wPacketSize=0;
		BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
		wPacketSize=0;
		STR_CMD_LC_CLUB_ROOM_LIST * pDBO=NULL;
		while ((lResultCode == DB_SUCCESS) && (m_AccountsDBModule->IsRecordsetEnd()==false))
		{
			//������Ϣ
			if ((wPacketSize+sizeof(STR_CMD_LC_CLUB_ROOM_LIST))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_ROOM_LIST,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//��ȡ��Ϣ
			pDBO=(STR_CMD_LC_CLUB_ROOM_LIST *)(cbBuffer+wPacketSize);
			pDBO->dwRoomID=m_AccountsDBAide.GetValue_DWORD(TEXT("RoomID"));
			DWORD dwGameID =m_AccountsDBAide.GetValue_DWORD(TEXT("GameID"));
			pDBO->dwKindID = (dwGameID >> 16);

			BYTE byMode = m_AccountsDBAide.GetValue_BYTE(TEXT("ModeID"));
			pDBO->byGoldOrFK = 1;
			if(0 == byMode) //������  �Ǿ��ֲ��еķ�����
			{
				pDBO->byGoldOrFK = 1;
			}
			else if(3 == byMode) //������ҳ�  �Ǿ��ֲ��н�ҳ�
			{
				pDBO->byGoldOrFK = 2;
			}

			m_AccountsDBAide.GetValue_String(TEXT("KindName"),pDBO->szKindName,CountArray(pDBO->szKindName));
			m_AccountsDBAide.GetValue_String(TEXT("RoomName"),pDBO->szRoomName,CountArray(pDBO->szRoomName));

			pDBO->wPlayerNum=m_AccountsDBAide.GetValue_WORD(TEXT("PlayerNum"));
			m_AccountsDBAide.GetValue_String(TEXT("RoomRule"),pDBO->szRoomRule,CountArray(pDBO->szRoomRule));
			pDBO->byAllRound=m_AccountsDBAide.GetValue_BYTE(TEXT("AllRound"));
			pDBO->byChairNum=m_AccountsDBAide.GetValue_BYTE(TEXT("ChairNum"));

			pDBO->bDissolve=m_AccountsDBAide.GetValue_BYTE(TEXT("DissolveMinute"));
			pDBO->dwDissolveTime=m_AccountsDBAide.GetValue_DWORD(TEXT("DissolveMinute"));

			pDBO->dwAmount=m_AccountsDBAide.GetValue_DWORD(TEXT("ServiceGold"));
			pDBO->dwOwnerPercentage=m_AccountsDBAide.GetValue_DWORD(TEXT("Revenue"));

			pDBO->byMask=m_AccountsDBAide.GetValue_BYTE(TEXT("Mask"));
			pDBO->dwDizhu=m_AccountsDBAide.GetValue_DWORD(TEXT("Dizhu"));
			pDBO->dwGold=m_AccountsDBAide.GetValue_DWORD(TEXT("Gold"));

			//����λ��
			wPacketSize+=sizeof(STR_CMD_LC_CLUB_ROOM_LIST);

			//�ƶ���¼
			m_AccountsDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_ROOM_LIST,dwContextID,cbBuffer,wPacketSize);

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_ROOM_LIST_FINISH,dwContextID,NULL,0);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}
}

//��ѯδ��Ա, �������Ȧ(���9��)
bool CDataBaseEngineSink::On_DBR_CL_CLUB_RANDOM_CLUB_LIST(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		if (wDataSize!=sizeof(STR_SUB_CL_CLUB_RANDOM_CLUB_LIST)) return false;
		STR_SUB_CL_CLUB_RANDOM_CLUB_LIST *pDBR = (STR_SUB_CL_CLUB_RANDOM_CLUB_LIST*) pData;

		//��������
		m_AccountsDBAide.ResetParameter();
		//�������
		m_AccountsDBAide.AddParameter(TEXT("@Mystery"),_MYSTERY);
		m_AccountsDBAide.AddParameter(TEXT("@UserID"),pDBR->dwUserID);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_CLUB_RANDOM_CLUB_LIST"));
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_RANDOM_CLUB_LIST"),true);

		//�б���
		WORD wPacketSize=0;
		BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
		wPacketSize=0;
		STR_CMD_LC_CLUB_RANDOM_CLUB_LIST * pDBO=NULL;
		while (m_AccountsDBModule->IsRecordsetEnd()==false)
		{
			//������Ϣ
			if ((wPacketSize+sizeof(STR_CMD_LC_CLUB_RANDOM_CLUB_LIST))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_RANDOM_CLUB_LIST,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//��ȡ��Ϣ
			pDBO=(STR_CMD_LC_CLUB_RANDOM_CLUB_LIST *)(cbBuffer+wPacketSize);
			pDBO->dwClubID = m_AccountsDBAide.GetValue_DWORD(TEXT("ClubID"));
			m_AccountsDBAide.GetValue_String(TEXT("ClubName"),pDBO->szClubName,CountArray(pDBO->szClubName));
			pDBO->dwMajorKindID=m_AccountsDBAide.GetValue_DWORD(TEXT("MajorKindID"));
			//TODONOW ManiorKindID ��Ҫת��
			//KINDID
			pDBO->byClubLevel=m_AccountsDBAide.GetValue_BYTE(TEXT("ClubLevel"));
			pDBO->wMemberNum=m_AccountsDBAide.GetValue_WORD(TEXT("MemberNum"));
			m_AccountsDBAide.GetValue_String(TEXT("Notice"),pDBO->szNotice,CountArray(pDBO->szNotice));
			m_AccountsDBAide.GetValue_String(TEXT("Message"),pDBO->szMessage,CountArray(pDBO->szMessage));
			pDBO->dwClubOwner=m_AccountsDBAide.GetValue_DWORD(TEXT("ClubOwner"));
			m_AccountsDBAide.GetValue_String(TEXT("ClubOwnerName"),pDBO->szClubOwnerName,CountArray(pDBO->szClubOwnerName));

			//����λ��
			wPacketSize+=sizeof(STR_CMD_LC_CLUB_RANDOM_CLUB_LIST);

			//�ƶ���¼
			m_AccountsDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_RANDOM_CLUB_LIST,dwContextID,cbBuffer,wPacketSize);

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_RANDOM_CLUB_LIST_FINISH,dwContextID,NULL,0);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}
}

//�����������Ȧ
bool CDataBaseEngineSink::On_DBR_CL_CLUB_JOIN_CLUB(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_JOIN_CLUB)) return false;
		STR_SUB_CL_CLUB_JOIN_CLUB *pDbr = (STR_SUB_CL_CLUB_JOIN_CLUB*)pData;

		//��������
		m_AccountsDBAide.ResetParameter();
		//�������
		m_AccountsDBAide.AddParameter(TEXT("@Mystery"),_MYSTERY);
		m_AccountsDBAide.AddParameter(TEXT("@UserID"),pDbr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@ClubID"),pDbr->dwClubID);
		m_AccountsDBAide.AddParameter(TEXT("@Note"),pDbr->szNote);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_CLUB_JOIN_CLUB"));
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_JOIN_CLUB"),true);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//�ṹ�幹��
		STR_CMD_LC_CLUB_JOIN_CLUB CMD;
		ZeroMemory(&CMD, sizeof(CMD));
		CMD.lResultCode = lResultCode;
		lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_JOIN_CLUB,dwContextID,&CMD,sizeof(CMD));


		/* �����������Ȧ �㲥 */
		if(lResultCode == DB_SUCCESS)
		{
			STR_SUB_CL_CLUB_JOIN_CLUB_BDCAST CMD2;
			CMD2.dwClubID = pDbr->dwClubID;
			CMD2.dwApplicantNum = m_AccountsDBAide.GetValue_DWORD(TEXT("ApplicantNum"));

			//������Զ�������ֲ�, ��֪ͨ�ͻ���ˢ��
			BYTE byMask = m_AccountsDBAide.GetValue_BYTE(TEXT("mask"));
			if(1 == byMask )
			{
				STR_CMD_LC_CLUB_LIST_RE CMD3;
				CMD3.byMask = 1;
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_JOIN_CLUB_RE,dwContextID,&CMD3,sizeof(CMD3));
			}

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_JOIN_CLUB_BDCAST,dwContextID,&CMD2,sizeof(CMD2));
		}
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//����Ȧ����
bool CDataBaseEngineSink::On_DBR_CL_CLUB_NOTICE(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_NOTICE)) return false;
		STR_SUB_CL_CLUB_NOTICE* pDbr = (STR_SUB_CL_CLUB_NOTICE*) pData;

		//��������
		m_AccountsDBAide.ResetParameter();

		//�������
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("ClubID"),pDbr->dwClubID);
		m_AccountsDBAide.AddParameter(TEXT("Notice"),pDbr->szNotice);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_CLUB_NOTICE"));
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_NOTICE"),true);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//�ṹ�幹��
		STR_CMD_LC_CLUB_NOTICE CMD;
		ZeroMemory(&CMD, sizeof(CMD));
		CMD.lResultCode = lResultCode;
		lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_NOTICE,dwContextID,&CMD,sizeof(CMD));
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//����Ȧ���
bool CDataBaseEngineSink::On_DBR_CL_CLUB_MESSAGE(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_MESSAGE)) return false;
		STR_SUB_CL_CLUB_MESSAGE* pDbr = (STR_SUB_CL_CLUB_MESSAGE*) pData;

		//��������
		m_AccountsDBAide.ResetParameter();

		//�������
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("ClubID"),pDbr->dwClubID);
		m_AccountsDBAide.AddParameter(TEXT("Message"),pDbr->szMessage);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_CLUB_MESSAGE"));
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_MESSAGE"),true);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//�ṹ�幹��
		STR_CMD_LC_CLUB_MESSAGE CMD;
		ZeroMemory(&CMD, sizeof(CMD));
		CMD.lResultCode = lResultCode;
		lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_MESSAGE,dwContextID,&CMD,sizeof(CMD));
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//���׷���
bool CDataBaseEngineSink::On_DBR_CL_CLUB_CONTRIBUTE_FK(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_CONTRIBUTE_FK)) return false;
		STR_SUB_CL_CLUB_CONTRIBUTE_FK* pDbr = (STR_SUB_CL_CLUB_CONTRIBUTE_FK*) pData;

		//��������
		m_AccountsDBAide.ResetParameter();

		//�������
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("ClubID"),pDbr->dwClubID);
		m_AccountsDBAide.AddParameter(TEXT("FK"),pDbr->dwFK);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_CLUB_CONTRIBUTE_FK"));
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_CONTRIBUTE_FK"),true);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//�ṹ�幹��
		STR_CMD_LC_CLUB_CONTRIBUTE_FK CMD;
		ZeroMemory(&CMD, sizeof(CMD));
		CMD.lResultCode = lResultCode;
		lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));
		CMD.dwClubFK=m_AccountsDBAide.GetValue_DWORD(TEXT("ClubFK"));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_CONTRIBUTE_FK,dwContextID,&CMD,sizeof(CMD));
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//����Ȧ����
bool CDataBaseEngineSink::On_DBR_CL_CLUB_AUTO_AGREE(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_AUTO_AGREE)) return false;
		STR_SUB_CL_CLUB_AUTO_AGREE* pDbr = (STR_SUB_CL_CLUB_AUTO_AGREE*) pData;

		//��������
		m_AccountsDBAide.ResetParameter();

		//�������
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("ClubID"),pDbr->dwClubID);
		m_AccountsDBAide.AddParameter(TEXT("AutoAgree"),pDbr->byAutoAgree);
		m_AccountsDBAide.AddParameter(TEXT("Sex"),pDbr->bySex);
		m_AccountsDBAide.AddParameter(TEXT("Level"),pDbr->wLevel);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_CLUB_AUTO_AGREE"));
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_AUTO_AGREE"),true);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//�ṹ�幹��
		STR_CMD_LC_CLUB_AUTO_AGREE CMD;
		ZeroMemory(&CMD, sizeof(CMD));
		CMD.lResultCode = lResultCode;
		lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_AUTO_AGREE,dwContextID,&CMD,sizeof(CMD));
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//�������˼�������Ȧ
bool CDataBaseEngineSink::On_DBR_CL_CLUB_INVITE(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_INVITE)) return false;
		STR_SUB_CL_CLUB_INVITE* pDbr = (STR_SUB_CL_CLUB_INVITE*) pData;

		//��������
		m_AccountsDBAide.ResetParameter();

		//�������
		m_AccountsDBAide.AddParameter(TEXT("Mystery"),_MYSTERY);
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("ClubID"),pDbr->dwClubID);
		m_AccountsDBAide.AddParameter(TEXT("TagID"),pDbr->dwTagID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_CLUB_INVITE"));
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_INVITE"),true);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//�ṹ�幹��
		STR_CMD_LC_CLUB_INVITE CMD;
		ZeroMemory(&CMD, sizeof(CMD));
		CMD.lResultCode = lResultCode;
		lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_INVITE,dwContextID,&CMD,sizeof(CMD));


		/* �������˵����� */
		STR_CMD_LC_CLUB_INVITE_REMIND CMD2;
		CMD2.dwTagID = pDbr->dwTagID;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_INVITE_REMIND,dwContextID,&CMD2,sizeof(CMD2));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//�������˻ظ�
bool CDataBaseEngineSink::On_DBR_CL_CLUB_INVITE_RESULT(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_INVITE_RESULT)) return false;
		STR_SUB_CL_CLUB_INVITE_RESULT* pDbr = (STR_SUB_CL_CLUB_INVITE_RESULT*) pData;

		//��������
		m_AccountsDBAide.ResetParameter();

		//�������
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("ClubID"),pDbr->dwClubID);
		m_AccountsDBAide.AddParameter(TEXT("Agree"),pDbr->byAgree);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_CLUB_INVITE_RESULT"));
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_INVITE_RESULT"),true);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//�ṹ�幹��
		STR_CMD_LC_CLUB_INVITE_RESULT CMD;
		ZeroMemory(&CMD, sizeof(CMD));
		CMD.lResultCode = lResultCode;
		lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_INVITE_RESULT,dwContextID,&CMD,sizeof(CMD));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//�������˲鿴�Լ��������б�
bool CDataBaseEngineSink::On_DBR_CL_CLUB_INQUERY_LIST(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_INQUERY_LIST)) return false;
		STR_SUB_CL_CLUB_INQUERY_LIST* pDbr = (STR_SUB_CL_CLUB_INQUERY_LIST*) pData;

		//��������
		m_AccountsDBAide.ResetParameter();

		//�������
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_CLUB_INQUERY_LIST"));
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_INQUERY_LIST"),true);

		//�����б�
		WORD wPacketSize=0;
		BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
		wPacketSize=0;
		STR_CMD_LC_CLUB_INQUERY_LIST * pDBO=NULL;
		while (m_AccountsDBModule->IsRecordsetEnd()==false)
		{
			//������Ϣ
			if ((wPacketSize+sizeof(STR_CMD_LC_CLUB_INQUERY_LIST))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_INQUERY_LIST,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//��ȡ��Ϣ
			pDBO=(STR_CMD_LC_CLUB_INQUERY_LIST *)(cbBuffer+wPacketSize);
			pDBO->dwClubID = m_AccountsDBAide.GetValue_DWORD(TEXT("ClubID"));
			m_AccountsDBAide.GetValue_String(TEXT("ClubName"),pDBO->szClubName,CountArray(pDBO->szClubName));
			pDBO->dwMajorKindID=m_AccountsDBAide.GetValue_DWORD(TEXT("MajorKindID"));
			//TODONOW ManiorKindID ��Ҫת��
			//KINDID
			pDBO->byClubLevel=m_AccountsDBAide.GetValue_BYTE(TEXT("ClubLevel"));
			pDBO->wMemberNum=m_AccountsDBAide.GetValue_WORD(TEXT("MemberNum"));
			m_AccountsDBAide.GetValue_String(TEXT("Notice"),pDBO->szNotice,CountArray(pDBO->szNotice));
			m_AccountsDBAide.GetValue_String(TEXT("Message"),pDBO->szMessage,CountArray(pDBO->szMessage));
			pDBO->dwClubOwner=m_AccountsDBAide.GetValue_DWORD(TEXT("ClubOwner"));
			m_AccountsDBAide.GetValue_String(TEXT("ClubOwnerName"),pDBO->szClubOwnerName,CountArray(pDBO->szClubOwnerName));

			//����λ��
			wPacketSize+=sizeof(STR_CMD_LC_CLUB_INQUERY_LIST);

			//�ƶ���¼
			m_AccountsDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_INQUERY_LIST,dwContextID,cbBuffer,wPacketSize);

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_INQUERY_LIST_FINISH,dwContextID,NULL,0);
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//�������б�
bool CDataBaseEngineSink::On_DBR_CL_CLUB_APPLICANT_LIST(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_APPLICANT_LIST)) return false;
		STR_SUB_CL_CLUB_APPLICANT_LIST* pDbr = (STR_SUB_CL_CLUB_APPLICANT_LIST*) pData;

		//��������
		m_AccountsDBAide.ResetParameter();

		//�������
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("ClubID"),pDbr->dwClubID);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_CLUB_APPLICANT_LIST"));
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_APPLICANT_LIST"),true);

		//�����б�
		WORD wPacketSize=0;
		BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
		wPacketSize=0;
		STR_CMD_LC_CLUB_APPLICANT_LIST * pDBO=NULL;
		while (m_AccountsDBModule->IsRecordsetEnd()==false)
		{
			//������Ϣ
			if ((wPacketSize+sizeof(STR_CMD_LC_CLUB_APPLICANT_LIST))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_APPLICANT_LIST,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//��ȡ��Ϣ
			pDBO=(STR_CMD_LC_CLUB_APPLICANT_LIST *)(cbBuffer+wPacketSize);
			pDBO->dwUserID = m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
			m_AccountsDBAide.GetValue_String(TEXT("UserName"),pDBO->szUserName,CountArray(pDBO->szUserName));
			pDBO->byUserSex=m_AccountsDBAide.GetValue_BYTE(TEXT("UserSex"));
			pDBO->byUserLevel=m_AccountsDBAide.GetValue_BYTE(TEXT("UserLevel"));
			m_AccountsDBAide.GetValue_String(TEXT("Note"),pDBO->szNote,CountArray(pDBO->szNote));
			m_AccountsDBAide.GetValue_String(TEXT("HeadUrl"),pDBO->szHeadUrl,CountArray(pDBO->szHeadUrl));

			//����λ��
			wPacketSize+=sizeof(STR_CMD_LC_CLUB_APPLICANT_LIST);

			//�ƶ���¼
			m_AccountsDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_APPLICANT_LIST,dwContextID,cbBuffer,wPacketSize);

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_APPLICANT_LIST_FINISH,dwContextID,NULL,0);
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//ְ������
bool CDataBaseEngineSink::On_DBR_CL_CLUB_APPOINTMENT(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_APPOINTMENT)) return false;
		STR_SUB_CL_CLUB_APPOINTMENT* pDbr = (STR_SUB_CL_CLUB_APPOINTMENT*) pData;

		//��������
		m_AccountsDBAide.ResetParameter();

		//�������
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("ClubID"),pDbr->dwClubID);
		m_AccountsDBAide.AddParameter(TEXT("TagID"),pDbr->dwTagID);
		m_AccountsDBAide.AddParameter(TEXT("Action"),pDbr->byAction);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_CLUB_APPOINTMENT"));
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_APPOINTMENT"),true);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//���ͷ���
		STR_CMD_LC_CLUB_APPOINTMENT CMD;
		ZeroMemory(&CMD, sizeof(CMD));
		CMD.lResultCode = lResultCode;
		lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_APPOINTMENT,dwContextID,&CMD,sizeof(CMD));

		//��������
		STR_CMD_LC_CLUB_APPOINTMENT_NOTE CMD2;
		CMD2.byAction = pDbr->byAction;
		CMD2.dwUserID = pDbr->dwTagID;
		lstrcpyn(CMD2.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD2.szDescribe));
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_APPOINTMENT_NOTE,dwContextID,&CMD2,sizeof(CMD2));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//����ս��ͳ��
bool CDataBaseEngineSink::On_DBR_CL_CLUB_RECORD_LIST(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_RECORD_LIST)) return false;
		STR_SUB_CL_CLUB_RECORD_LIST *pDbr = (STR_SUB_CL_CLUB_RECORD_LIST * )pData;

		//��������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("dwClubID"),pDbr->dwClubID);
		m_AccountsDBAide.AddParameter(TEXT("tmQueryStartData"),pDbr->tmQueryStartData);
		m_AccountsDBAide.AddParameter(TEXT("tmQueryEndData"),pDbr->tmQueryEndData);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_CLUB_RECORD_LIST"));
		LONG lResultCode2 = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_RECORD_LIST"),true);

		//�����б�
		WORD wPacketSize=0;
		BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
		wPacketSize=0;
		STR_CMD_LC_CLUB_RECORD_LIST * pDBO=NULL;
		while ( (lResultCode2 == DB_SUCCESS) && (m_AccountsDBModule->IsRecordsetEnd()==false))
		{
			//������Ϣ
			if ((wPacketSize+sizeof(STR_CMD_LC_CLUB_RECORD_LIST))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_RECORD_LIST,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//��ȡ��Ϣ
			pDBO=(STR_CMD_LC_CLUB_RECORD_LIST *)(cbBuffer+wPacketSize);
			BYTE gameMode  = m_AccountsDBAide.GetValue_BYTE(TEXT("ModeID"));
			//0����  1����  2��� 3�������
			if(0 == gameMode)
			{
				pDBO->byMask = 1;
			}
			else if( (2 == gameMode) || (3 == gameMode ))
			{
				pDBO->byMask = 2;
			}

			pDBO->dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));

			m_AccountsDBAide.GetValue_String(TEXT("NickName"),pDBO->szNickName,CountArray(pDBO->szNickName));
			m_AccountsDBAide.GetValue_String(TEXT("HeadUrl"),pDBO->szHeadUrl,CountArray(pDBO->szHeadUrl));
			
			pDBO->dwAllCount = m_AccountsDBAide.GetValue_DWORD(TEXT("AllCount"));
			pDBO->lAllScore=m_AccountsDBAide.GetValue_LONGLONG(TEXT("AllScore"));
			pDBO->dwWinCount = m_AccountsDBAide.GetValue_DWORD(TEXT("WinCount"));
			pDBO->lWinScore=m_AccountsDBAide.GetValue_LONGLONG(TEXT("WinScore"));
			
			//����λ��
			wPacketSize+=sizeof(STR_CMD_LC_CLUB_RECORD_LIST);

			//�ƶ���¼
			m_AccountsDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_RECORD_LIST,dwContextID,cbBuffer,wPacketSize);

		//���ͽ���
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_RECORD_FINISH,dwContextID,NULL,0);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//����Ȧ����
bool CDataBaseEngineSink::On_DBR_CL_CLUB_CHAT(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_CHAT)) return false;
		STR_SUB_CL_CLUB_CHAT *pDbr = (STR_SUB_CL_CLUB_CHAT * )pData;

		/* 1. ��ȡ�����ߵ���Ϣ */
		TCHAR szTempUserNickName[32] = TEXT("");
		TCHAR szTempHeadUrl[256] = TEXT("");

		//��������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_CLUB_CHAT_GETINFO"));
		LONG lResultCode2 = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_CHAT_GETINFO"),true);

		if(lResultCode2 != DB_SUCCESS)
		{
			return true;
		}
		m_AccountsDBAide.GetValue_String(TEXT("UserNickName"), szTempUserNickName,CountArray(szTempUserNickName));
		m_AccountsDBAide.GetValue_String(TEXT("HeadUrl"),szTempHeadUrl,CountArray(szTempHeadUrl));

		/* 2.���͸����ֲ��������ߵ��� */
		//��������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("ClubID"),pDbr->dwClubID);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_CLUB_CHAT"));
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_CHAT"),true);

		//�����б�
		WORD wPacketSize=0;
		BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
		wPacketSize=0;
		STR_CMD_LC_CLUB_CHAT_BDCAST * pDBO=NULL;

		while (m_AccountsDBModule->IsRecordsetEnd()==false && lResultCode == DB_SUCCESS)
		{
			//������Ϣ
			if ((wPacketSize+sizeof(STR_CMD_LC_CLUB_CHAT_BDCAST))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CHAT_ALL_BDCAST,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//��ȡ��Ϣ
			pDBO=(STR_CMD_LC_CLUB_CHAT_BDCAST *)(cbBuffer+wPacketSize);
			pDBO->dwTagID = m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));

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
			m_AccountsDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CHAT_ALL_BDCAST,dwContextID,cbBuffer,wPacketSize);

		STR_CMD_LC_CLUB_CHAT CMD2;
		CMD2.byChatMode = pDbr->byChatMode;
		CMD2.lResultCode = lResultCode;
		memcpy(CMD2.szDescribe, TEXT("����ʧ��, �Ժ�����"), sizeof(CMD2));
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CHAT_ALL,dwContextID,&CMD2, sizeof(CMD2));
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//��������
bool CDataBaseEngineSink::On_DBR_CL_WORD_CHAT(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_CHAT)) return false;
		STR_SUB_CL_CLUB_CHAT *pDbr = (STR_SUB_CL_CLUB_CHAT * )pData;

		/* 1. ��ȡ�����ߵ���Ϣ */
		TCHAR szTempUserNickName[32] = TEXT("");
		TCHAR szTempHeadUrl[256] = TEXT("");

		//��������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_CLUB_CHAT_GETINFO"));
		LONG lResultCode2 = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_CHAT_GETINFO"),true);

		if(lResultCode2 != DB_SUCCESS)
		{
			return true;
		}
		m_AccountsDBAide.GetValue_String(TEXT("UserNickName"), szTempUserNickName,CountArray(szTempUserNickName));
		m_AccountsDBAide.GetValue_String(TEXT("HeadUrl"),szTempHeadUrl,CountArray(szTempHeadUrl));

		/* 2. ����㲥 ���͸��õ�¼���������ߵ��� */
		STR_CMD_LC_CLUB_CHAT_BDCAST CMD3;
		CMD3.byChatMode = pDbr->byChatMode;
		CMD3.dwClubID = 0;
		CMD3.dwUserID = pDbr->dwUserID;
		memcpy(CMD3.szUserNickName, szTempUserNickName, sizeof(CMD3.szUserNickName));
		memcpy(CMD3.szHeadUrl, szTempHeadUrl, sizeof(CMD3.szHeadUrl));
		memcpy(CMD3.szChat, pDbr->szChat, sizeof(CMD3.szChat));
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CHAT_ALL_BDCAST,dwContextID,&CMD3, sizeof(CMD3));

		/* 3. ���췵�� */
		STR_CMD_LC_CLUB_CHAT CMD2;
		CMD2.byChatMode = pDbr->byChatMode;
		CMD2.lResultCode = lResultCode2;
		memcpy(CMD2.szDescribe, TEXT("����ʧ��, �Ժ�����"), sizeof(CMD2));
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CHAT_ALL,dwContextID,&CMD2, sizeof(CMD2));
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//ϵͳ���� -- TODONOW ��ʱ��������߼�
bool CDataBaseEngineSink::On_DBR_CL_SYSTEM_CHAT(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_CHAT)) return false;
		STR_SUB_CL_CLUB_CHAT *pDbr = (STR_SUB_CL_CLUB_CHAT * )pData;

		/* 1. ��ȡ�����ߵ���Ϣ */
		TCHAR szTempUserNickName[32] = TEXT("");
		TCHAR szTempHeadUrl[256] = TEXT("");

		//��������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_CLUB_CHAT_GETINFO"));
		LONG lResultCode2 = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_CHAT_GETINFO"),true);

		if(lResultCode2 != DB_SUCCESS)
		{
			return true;
		}
		m_AccountsDBAide.GetValue_String(TEXT("UserNickName"), szTempUserNickName,CountArray(szTempUserNickName));
		m_AccountsDBAide.GetValue_String(TEXT("HeadUrl"),szTempHeadUrl,CountArray(szTempHeadUrl));

		
		/* 2. ����㲥 ���͸��õ�¼���������ߵ��� */
		/*
		STR_CMD_LC_CLUB_CHAT_BDCAST CMD3;
		CMD3.byChatMode = pDbr->byChatMode;
		CMD3.dwClubID = 0;
		CMD3.dwUserID = pDbr->dwUserID;
		memcpy(CMD3.szUserNickName, szTempUserNickName, sizeof(CMD3.szUserNickName));
		memcpy(CMD3.szHeadUrl, szTempHeadUrl, sizeof(CMD3.szHeadUrl));
		memcpy(CMD3.szChat, pDbr->szChat, sizeof(CMD3.szChat));
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_WORD_CHAT_BDCAST,dwContextID,&CMD3, sizeof(CMD3));
		*/

		/* 3. ���췵�� */
		STR_CMD_LC_CLUB_CHAT CMD2;
		CMD2.byChatMode = pDbr->byChatMode;
		CMD2.lResultCode = lResultCode2;
		memcpy(CMD2.szDescribe, TEXT("����ʧ��, �Ժ�����"), sizeof(CMD2));
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CHAT_ALL,dwContextID,&CMD2, sizeof(CMD2));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//˽������
bool CDataBaseEngineSink::On_DBR_CL_SECRET_CHAT(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_CHAT)) return false;
		STR_SUB_CL_CLUB_CHAT *pDbr = (STR_SUB_CL_CLUB_CHAT * )pData;

		/* 1. ��ȡ�����ߵ���Ϣ */
		TCHAR szTempUserNickName[32] = TEXT("");
		TCHAR szTempHeadUrl[256] = TEXT("");

		//��������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_CLUB_CHAT_GETINFO"));
		LONG lResultCode2 = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_CHAT_GETINFO"),true);

		if(lResultCode2 != DB_SUCCESS)
		{
			return true;
		}
		m_AccountsDBAide.GetValue_String(TEXT("UserNickName"), szTempUserNickName,CountArray(szTempUserNickName));
		m_AccountsDBAide.GetValue_String(TEXT("HeadUrl"),szTempHeadUrl,CountArray(szTempHeadUrl));

		/* 2. ����㲥 ���͸��õ�¼���������ߵ��� */
		STR_CMD_LC_CLUB_CHAT_BDCAST CMD3;
		CMD3.byChatMode = pDbr->byChatMode;
		CMD3.dwClubID = 0;
		CMD3.dwUserID = pDbr->dwUserID;
		CMD3.dwTagID = pDbr->dwTagUserID;
		memcpy(CMD3.szUserNickName, szTempUserNickName, sizeof(CMD3.szUserNickName));
		memcpy(CMD3.szHeadUrl, szTempHeadUrl, sizeof(CMD3.szHeadUrl));
		memcpy(CMD3.szChat, pDbr->szChat, sizeof(CMD3.szChat));
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CHAT_ALL_BDCAST,dwContextID,&CMD3, sizeof(CMD3));


		/* 3. ���췵�� */
		STR_CMD_LC_CLUB_CHAT CMD2;
		CMD2.byChatMode = pDbr->byChatMode;
		CMD2.lResultCode = lResultCode2;
		memcpy(CMD2.szDescribe, TEXT("����ʧ��, �Ժ�����"), sizeof(CMD2));
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CHAT_ALL,dwContextID,&CMD2, sizeof(CMD2));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//����Ȧ�ö�
bool CDataBaseEngineSink::On_DBR_CL_STICKY_POST(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		if (wDataSize != sizeof(STR_DBR_CL_CLUB_STICKY_POST)) return false;
		STR_DBR_CL_CLUB_STICKY_POST *pDbr = (STR_DBR_CL_CLUB_STICKY_POST * )pData;

		//��������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("dwUserID"),pDbr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("dwClubID"),pDbr->dwClubID);
		m_AccountsDBAide.AddParameter(TEXT("dwStickyMark"),pDbr->cbTopFlag);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_CLUB_STICK"));
		LONG lResultCode2 = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_STICK"),true);

		//����DBO����
		STR_DBO_LC_CLUB_STICKY_POST DBO;
		ZeroMemory(&DBO, sizeof(STR_DBO_LC_CLUB_STICKY_POST));
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_STICKY_POST, dwContextID, &DBO, sizeof(DBO));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//����뿪���ֲ�����
bool CDataBaseEngineSink::On_DBR_CL_CLUB_ROOM_USER_LEAVE(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_ROOM_USER_LEAVE)) return false;
		STR_SUB_CL_CLUB_ROOM_USER_LEAVE* pDbr = (STR_SUB_CL_CLUB_ROOM_USER_LEAVE*) pData;

		//��������
		m_AccountsDBAide.ResetParameter();

		//�������
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("RoomID"),pDbr->dwClubRoomID);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_CLUB_ROOM_USER_LEAVE"));
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_ROOM_USER_LEAVE"),true);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//��������
bool CDataBaseEngineSink::On_DBR_CL_CLUB_ROOM_SETTING(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_ROOM_SETTING)) return false;
		STR_SUB_CL_CLUB_ROOM_SETTING* pDbr = (STR_SUB_CL_CLUB_ROOM_SETTING*) pData;

#pragma region 1. ��ȡ֮ǰ�ķ������, ���ҹ����µķ������
		//���ݿ⴫�����
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwClubRoomID"), pDbr->dwRoomID);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_CLUB_ROOM_SETTING_QUERY_INFO")); 
		//ִ�в�ѯ
		LONG lResultCode1 = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_ROOM_SETTING_QUERY_INFO"), true);

		if(lResultCode1 != DB_SUCCESS)
		{
			CTraceService::TraceString(TEXT("GSP_CL_CLUB_ROOM_SETTING_QUERY_INFO Return ��0"), TraceLevel_Warning);
			return false ;
		}

		//��ȡ������
		TCHAR szRealRoomRule[2048];
		m_AccountsDBAide.GetValue_String(TEXT("RealRoomRule"), szRealRoomRule, sizeof(szRealRoomRule));

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
		m_AccountsDBAide.ResetParameter();

		//�������
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("RoomID"),pDbr->dwRoomID);	
		
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

		m_AccountsDBAide.AddParameter(TEXT("GoldOrFK"),byModeID);
		m_AccountsDBAide.AddParameter(TEXT("Dissolve"),pDbr->bDissolve);
		m_AccountsDBAide.AddParameter(TEXT("DissolveTime"),pDbr->dwDissolveTime);
		m_AccountsDBAide.AddParameter(TEXT("Amount"),pDbr->dwAmount);
		m_AccountsDBAide.AddParameter(TEXT("OwnerPercentage"),pDbr->dwOwnerPercentage);
		m_AccountsDBAide.AddParameter(TEXT("dwDizhu"),pDbr->dwDizhu);
		m_AccountsDBAide.AddParameter(TEXT("dwGold"),pDbr->dwGold);
		m_AccountsDBAide.AddParameter(TEXT("byMask"),pDbr->byMask);
		m_AccountsDBAide.AddParameter(TEXT("RoomName"),pDbr->szRoomName);
		m_AccountsDBAide.AddParameter(TEXT("RealRoomRule"),strRealRoomRule);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_CLUB_ROOM_SETTING"));
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_ROOM_SETTING"),true);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//�ṹ�幹��
		STR_CMD_LC_CLUB_ROOM_SETTING CMD;
		ZeroMemory(&CMD, sizeof(CMD));
		CMD.lResultCode = lResultCode;
		lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));

		CMD.dwRoomID=m_AccountsDBAide.GetValue_DWORD(TEXT("RoomID"));

		//��ֵת��
		BYTE byResultModeID = m_AccountsDBAide.GetValue_BYTE(TEXT("ModeID"));;
		if(0 == byResultModeID)//���ֲ��ķ����� ������ͨ������
		{
			CMD.byGoldOrFK  = 1;
		}
		else if( 3 == byResultModeID)//���ֲ��Ľ�ҳ� ��ʵ�� ������ҳ�
		{
			CMD.byGoldOrFK  = 2;
		}

		CMD.dwDissolveTime=m_AccountsDBAide.GetValue_DWORD(TEXT("DissolveMinute"));
		CMD.byDissolve= m_AccountsDBAide.GetValue_BYTE(TEXT("DissolveMinute"));
		CMD.dwAmount=m_AccountsDBAide.GetValue_DWORD(TEXT("ServiceGold"));
		CMD.dwOwnerPercentage=m_AccountsDBAide.GetValue_DWORD(TEXT("Revenue"));
		CMD.byMask = pDbr->byMask;
		CMD.dwDizhu = pDbr->dwDizhu;
		CMD.dwGold = pDbr->dwGold;
		m_AccountsDBAide.GetValue_String(TEXT("RoomName"), CMD.szRoomName,CountArray(CMD.szRoomName));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_ROOM_SETTING,dwContextID,&CMD,sizeof(CMD));
#pragma endregion
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//���󷿼�����
bool CDataBaseEngineSink::On_DBR_CL_CLUB_ROOM_QUERY_SETTING(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_ROOM_QUERY_SETTING)) return false;
		STR_SUB_CL_CLUB_ROOM_QUERY_SETTING* pDbr = (STR_SUB_CL_CLUB_ROOM_QUERY_SETTING*) pData;

		//��������
		m_AccountsDBAide.ResetParameter();

		//�������
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("RoomID"),pDbr->dwRoomID);	

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_CLUB_ROOM_QUERY_SETTING"));
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_ROOM_QUERY_SETTING"),true);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//�ṹ�幹��
		STR_CMD_LC_CLUB_ROOM_QUERY_SETTING CMD;
		ZeroMemory(&CMD, sizeof(CMD));
		CMD.lResultCode = lResultCode;
		lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));

		CMD.dwRoomID=m_AccountsDBAide.GetValue_DWORD(TEXT("RoomID"));

		//��ֵת��
		BYTE byResultModeID = m_AccountsDBAide.GetValue_BYTE(TEXT("ModeID"));;
		if(0 == byResultModeID)//���ֲ��ķ����� ������ͨ������
		{
			CMD.byGoldOrFK  = 1;
		}
		else if( 3 == byResultModeID)//���ֲ��Ľ�ҳ� ��ʵ�� ������ҳ�
		{
			CMD.byGoldOrFK  = 2;
		}

		CMD.dwDissolveTime=m_AccountsDBAide.GetValue_DWORD(TEXT("DissolveMinute"));
		CMD.byDissolve= m_AccountsDBAide.GetValue_BYTE(TEXT("DissolveMinute"));
		CMD.dwAmount=m_AccountsDBAide.GetValue_DWORD(TEXT("ServiceGold"));
		CMD.dwOwnerPercentage=m_AccountsDBAide.GetValue_DWORD(TEXT("Revenue"));
		CMD.byMask = m_AccountsDBAide.GetValue_BYTE(TEXT("Mask"));
		CMD.dwDizhu = m_AccountsDBAide.GetValue_DWORD(TEXT("Dizhu"));
		CMD.dwGold = m_AccountsDBAide.GetValue_DWORD(TEXT("Gold"));
		m_AccountsDBAide.GetValue_String(TEXT("RoomName"), CMD.szRoomName,CountArray(CMD.szRoomName));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_ROOM_QUERY_SETTING,dwContextID,&CMD,sizeof(CMD));
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}


//��ɢ��������
bool CDataBaseEngineSink::On_DBR_CL_CLUB_ROOM_DISSOLVE(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_ROOM_DISSOLVE)) return false;
		STR_SUB_CL_CLUB_ROOM_DISSOLVE* pDbr = (STR_SUB_CL_CLUB_ROOM_DISSOLVE*) pData;

		//��������
		m_AccountsDBAide.ResetParameter();

		//�������
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("ClubID"),pDbr->dwClubID);
		m_AccountsDBAide.AddParameter(TEXT("RoomID"),pDbr->dwClubRoomID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_CLUB_ROOM_DISSOLVE"));
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_ROOM_DISSOLVE"),true);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//�ṹ�幹��
		STR_CMD_LC_CLUB_ROOM_DISSOLVE CMD;
		ZeroMemory(&CMD, sizeof(CMD));
		CMD.lResultCode = lResultCode;
		lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_ROOM_DISSOLVE,dwContextID,&CMD,sizeof(CMD));
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//��ɢ��������
bool CDataBaseEngineSink::On_DBR_CL_CLUB_TABLE_DISSOLVE(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_TABLE_DISSOLVE)) return false;
		STR_SUB_CL_CLUB_TABLE_DISSOLVE* pDbr = (STR_SUB_CL_CLUB_TABLE_DISSOLVE*) pData;

		//��������
		m_AccountsDBAide.ResetParameter();

		//�������
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("ClubID"),pDbr->dwClubID);
		m_AccountsDBAide.AddParameter(TEXT("RoomID"),pDbr->dwClubRoomID);
		m_AccountsDBAide.AddParameter(TEXT("TableID"),pDbr->dwTableID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_CLUB_TABLE_DISSOLVE"));
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_TABLE_DISSOLVE"),true);

		if(lResultCode == DB_SUCCESS)
		{
			//���ؽṹ���ʼ��
			STR_CMD_LC_CLUB_TABLE_DISSOLVE CMD;
			ZeroMemory(&CMD, sizeof(CMD));

			//�������
			CDBVarValue DBVarValue;
			m_AccountsDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
			lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));

			CMD.byMask = m_AccountsDBAide.GetValue_BYTE(TEXT("Mask"));
			CMD.dwGameID = m_AccountsDBAide.GetValue_DWORD(TEXT("GameID"));
			CMD.dwTableID = pDbr->dwTableID;

			CMD.lResultCode = lResultCode;
			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_TABLE_DISSOLVE,dwContextID,&CMD,sizeof(CMD));

			return true;
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//��ɢ����Ȧ
bool CDataBaseEngineSink::On_DBR_CL_CLUB_DISS_CLUB(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		if (wDataSize != sizeof(STR_SUB_CL_CLUB_DISS_CLUB)) return false;
		STR_SUB_CL_CLUB_DISS_CLUB* pDbr = (STR_SUB_CL_CLUB_DISS_CLUB*) pData;

		//��������
		m_AccountsDBAide.ResetParameter();

		//�������
		m_AccountsDBAide.AddParameter(TEXT("UserID"),pDbr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("ClubID"),pDbr->dwClubID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_CLUB_DISS_CLUB"));
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_DISS_CLUB"),true);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//�ṹ�幹��
		STR_CMD_LC_CLUB_DISS_CLUB CMD;
		ZeroMemory(&CMD, sizeof(CMD));
		CMD.lResultCode = lResultCode;
		lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_DISS_CLUB,dwContextID,&CMD,sizeof(CMD));
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//��������Ȧ
bool CDataBaseEngineSink::On_DBR_CL_CLUB_CREATE_CLUB(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(STR_DBR_CL_CLUB_CREATE_CLUB));
		if (wDataSize!=sizeof(STR_DBR_CL_CLUB_CREATE_CLUB)) return false;

		STR_DBR_CL_CLUB_CREATE_CLUB *pDBR = (STR_DBR_CL_CLUB_CREATE_CLUB*) pData;

		//��������
		m_AccountsDBAide.ResetParameter();

		//�������
		m_AccountsDBAide.AddParameter(TEXT("@Mystery"),_MYSTERY);
		m_AccountsDBAide.AddParameter(TEXT("@UserID"),pDBR->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@ClubName"),pDBR->szClubName);
		m_AccountsDBAide.AddParameter(TEXT("@MajorKindID"),pDBR->dwMajorKindID);
		m_AccountsDBAide.AddParameter(TEXT("@MinorKindID"), pDBR->szMinorKindID);
		m_AccountsDBAide.AddParameter(TEXT("@szNotice"), pDBR->szNotice);
		m_AccountsDBAide.AddParameter(TEXT("@szMessag"), pDBR->szMessag);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_CLUB_CREATE_CLUB"));
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_CREATE_CLUB"),true);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//�ṹ�幹��
		STR_CMD_LC_CLUB_CREATE_CLUB CMD;
		ZeroMemory(&CMD, sizeof(CMD));
		CMD.lResultCode = lResultCode;
		lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));

		if(lResultCode == DB_SUCCESS)
		{
			CMD.dwClubID =m_AccountsDBAide.GetValue_DWORD(TEXT("ClubID"));
			CMD.byClubLevel=m_AccountsDBAide.GetValue_BYTE(TEXT("ClubLevel"));
			CMD.dwFK=m_AccountsDBAide.GetValue_BYTE(TEXT("FK"));
		}

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_CREATE_CLUB,dwContextID,&CMD,sizeof(CMD));
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//������뷿��
bool CDataBaseEngineSink::On_DBR_CL_CLUB_JOIN_ROOM(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		if (wDataSize!=sizeof(STR_SUB_CL_CLUB_JOIN_ROOM)) return false;

		STR_SUB_CL_CLUB_JOIN_ROOM * pDBbr = (STR_SUB_CL_CLUB_JOIN_ROOM*)pData;

		STR_CMD_LC_CLUB_JOIN_ROOM CMD1;

#pragma region �����б�
		/* ��һ�� �����б� */
		//��������
		m_AccountsDBAide.ResetParameter();

		//�������
		m_AccountsDBAide.AddParameter(TEXT("@UserID"),pDBbr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@RoomID"),pDBbr->dwRoomID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_CLUB_JOIN_ROOM_TABLE"));
		long ResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_JOIN_ROOM_TABLE"),true);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		CMD1.lResultCode = ResultCode;
		lstrcpyn(CMD1.szDescribe, CW2CT(DBVarValue.bstrVal), CountArray(CMD1.szDescribe));

		//�б���
		WORD wPacketSize=0;
		BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
		wPacketSize=0;
		STR_CMD_LC_CLUB_TABLE_LIST * pDBO=NULL;
		while ((ResultCode == DB_SUCCESS) && (m_AccountsDBModule->IsRecordsetEnd()==false ))
		{
			//������Ϣ
			if ((wPacketSize+sizeof(STR_CMD_LC_CLUB_TABLE_LIST))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_TABLE_LIST_TABLE,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//��ȡ��Ϣ
			pDBO=(STR_CMD_LC_CLUB_TABLE_LIST *)(cbBuffer+wPacketSize);
			
			pDBO->dwClubRoomID  = m_AccountsDBAide.GetValue_DWORD(TEXT("RoomID"));
			pDBO->dwTableID=m_AccountsDBAide.GetValue_DWORD(TEXT("TableID"));
			pDBO->dwClubID = m_AccountsDBAide.GetValue_DWORD(TEXT("ClubID"));
			pDBO->byMask = m_AccountsDBAide.GetValue_BYTE(TEXT("IsOwner"));

			pDBO->TableState=m_AccountsDBAide.GetValue_DWORD(TEXT("TableState"));
			pDBO->LockState=m_AccountsDBAide.GetValue_DWORD(TEXT("LockState"));
			pDBO->CurrentRound=m_AccountsDBAide.GetValue_DWORD(TEXT("CurrentRound"));
			pDBO->dwOwnerID=m_AccountsDBAide.GetValue_DWORD(TEXT("OwnerID"));
			pDBO->byDel = 0;

			//����λ��
			wPacketSize+=sizeof(STR_CMD_LC_CLUB_TABLE_LIST);

			//�ƶ���¼
			m_AccountsDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_TABLE_LIST_TABLE,dwContextID,cbBuffer,wPacketSize);

#pragma endregion


#pragma region ����б�
		/* �ڶ��� ����б� */
		//��������
		m_AccountsDBAide.ResetParameter();

		//�������
		m_AccountsDBAide.AddParameter(TEXT("@UserID"),pDBbr->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@RoomID"),pDBbr->dwRoomID);

		//�������
		TCHAR szDescribeString2[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString2),adParamOutput);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_CLUB_JOIN_ROOM_USER"));
		long ResultCode2 = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_JOIN_ROOM_USER"),true);

		//�б���
		WORD wPacketSize2=0;
		BYTE cbBuffer2[MAX_ASYNCHRONISM_DATA/10];
		wPacketSize2=0;
		STR_CMD_LC_CLUB_TABLE_USER_LIST * pDBO2=NULL;
		while ((m_AccountsDBModule->IsRecordsetEnd()==false) && (ResultCode2 == DB_SUCCESS))
		{
			//������Ϣ
			if ((wPacketSize2+sizeof(STR_CMD_LC_CLUB_TABLE_USER_LIST))>sizeof(cbBuffer2))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_TABLE_LIST_USER,dwContextID,cbBuffer2,wPacketSize2);
				wPacketSize2=0;
			}

			//��ȡ��Ϣ
			pDBO2=(STR_CMD_LC_CLUB_TABLE_USER_LIST *)(cbBuffer2+wPacketSize2);
			pDBO2->dwClubRoomID=m_AccountsDBAide.GetValue_DWORD(TEXT("ClubRoomID"));
			pDBO2->dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
			m_AccountsDBAide.GetValue_String(TEXT("UserName"),pDBO2->szUserName,CountArray(pDBO2->szUserName));
			pDBO2->bySex=m_AccountsDBAide.GetValue_BYTE(TEXT("Sex"));
			pDBO2->wLevel=m_AccountsDBAide.GetValue_WORD(TEXT("UserLevel"));
			m_AccountsDBAide.GetValue_String(TEXT("HeadUrl"),pDBO2->szHeadUrl,CountArray(pDBO2->szHeadUrl));
			pDBO2->byClubRole=m_AccountsDBAide.GetValue_BYTE(TEXT("ClubRole"));
			pDBO2->byClubReputation=m_AccountsDBAide.GetValue_BYTE(TEXT("ClubReputation"));
			pDBO2->dwTableID=m_AccountsDBAide.GetValue_DWORD(TEXT("TableID"));
			pDBO2->byChairID=m_AccountsDBAide.GetValue_BYTE(TEXT("ChairID"));
			pDBO2->byDel = 0;

			//����λ��
			wPacketSize2+=sizeof(STR_CMD_LC_CLUB_TABLE_USER_LIST);

			//�ƶ���¼
			m_AccountsDBModule->MoveToNext();
		}
		if (wPacketSize2>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_TABLE_LIST_USER,dwContextID,cbBuffer2,wPacketSize2);

#pragma endregion

#pragma region ��������
		//��������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@RoomID"),pDBbr->dwRoomID);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_CLUB_JOIN_ROOM_USER_NUM"));
		ResultCode2 = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_JOIN_ROOM_USER_NUM"),true);

		if(ResultCode2 == DB_SUCCESS)
		{
			CMD1.dwPlayerNum = m_AccountsDBAide.GetValue_DWORD(TEXT("PlayerNum"));
		}

#pragma endregion

		/* ������ ���뷿�䷵��*/
		//client ����������Ϣ��ˢ�½���
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_JOIN_ROOM, dwContextID, &CMD1, sizeof(CMD1));

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}


//Ⱥ��|�����������Ϣ�Ĵ�(ͬ��|�ܾ�)
bool CDataBaseEngineSink::On_DBR_CL_CLUB_APPLICANT_RESULT(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if (wDataSize != sizeof(STR_SUB_CL_CLUB_APPLICANT_RESULT)) return false;

	STR_SUB_CL_CLUB_APPLICANT_RESULT *pDBR = (STR_SUB_CL_CLUB_APPLICANT_RESULT*) pData;

	//��������
	m_AccountsDBAide.ResetParameter();
	//�������
	m_AccountsDBAide.AddParameter(TEXT("@UserID"),pDBR->dwUserID);
	m_AccountsDBAide.AddParameter(TEXT("@CludID"),pDBR->dwCludID);
	m_AccountsDBAide.AddParameter(TEXT("@Result"),pDBR->dwResult);
	m_AccountsDBAide.AddParameter(TEXT("@TagID"),pDBR->dwTagID);

	//�������
	TCHAR szDescribeString[128]=TEXT("");
	m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	//���ִ��
	LogicTraceHelper(TEXT("GSP_CL_CLUB_APPLICANT_RESULT"));
	LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_APPLICANT_RESULT"),true);

	//�������
	CDBVarValue DBVarValue;
	m_AccountsDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	//�ṹ�幹��
	STR_CMD_LC_CLUB_APPLICANT_RESULT CMD;
	ZeroMemory(&CMD, sizeof(CMD));
	CMD.lResultCode = lResultCode;
	lstrcpyn(CMD.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(CMD.szDescribe));

	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_APPLICANT_RESULT,dwContextID,&CMD,sizeof(CMD));

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
	m_AccountsDBAide.ResetParameter();

	//�������
	m_AccountsDBAide.AddParameter(TEXT("@UserID"),pDBR->dwUserID);
	m_AccountsDBAide.AddParameter(TEXT("@ClubID"),pDBR->dwClubID);

	//�������
	TCHAR szDescribeString[128]=TEXT("");
	m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	//���ִ��
	LogicTraceHelper(TEXT("GSP_CL_CLUB_QUIT"));
	LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_QUIT"),true);

	//�������
	CDBVarValue DBVarValue;
	m_AccountsDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	//�ṹ�幹��
	STR_CMD_LC_CLUB_QUIT DBO;
	ZeroMemory(&DBO, sizeof(DBO));
	DBO.lResultCode = lResultCode;
	lstrcpyn(DBO.szDescribe,CW2CT(DBVarValue.bstrVal),CountArray(DBO.szDescribe));

	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_QUIT,dwContextID,&DBO,sizeof(DBO));
	return true;
}

//�����Ա����
bool CDataBaseEngineSink::On_DBR_CL_CLUB_MEMBER_MANAGER(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//У�����
		if (wDataSize!=sizeof(STR_SUB_CL_CLUB_MEMBER_MANAGER)) return false;
		STR_SUB_CL_CLUB_MEMBER_MANAGER *pDBR = (STR_SUB_CL_CLUB_MEMBER_MANAGER*) pData;

		/* ��ȡ�������� */
		m_AccountsDBAide.ResetParameter();

		//�������
		m_AccountsDBAide.AddParameter(TEXT("@UserID"),pDBR->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@ClubID"),pDBR->dwClubID);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_CLUB_DATA"));
		LONG lResultCode2 =  m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_DATA"),true);

		if(DB_SUCCESS == lResultCode2)
		{
			STR_CMD_LC_CLUB_DATA CMD1;
			m_AccountsDBAide.GetValue_String(TEXT("ClubName"),CMD1.szClubName,CountArray(CMD1.szClubName));
			m_AccountsDBAide.GetValue_String(TEXT("ClubOwnerName"),CMD1.szClubOwnerName,CountArray(CMD1.szClubOwnerName));
			CMD1.dwClubID =m_AccountsDBAide.GetValue_DWORD(TEXT("ClubID"));
			CMD1.dwClubOwner =m_AccountsDBAide.GetValue_DWORD(TEXT("ClubOwner"));
			CMD1.dwMajorKindID =m_AccountsDBAide.GetValue_DWORD(TEXT("MajorKindID"));
			//TODONOW dwMinorKindID
			CMD1.byClubLevel =m_AccountsDBAide.GetValue_BYTE(TEXT("ClubLevel"));
			CMD1.wMemberNum =m_AccountsDBAide.GetValue_WORD(TEXT("MemberNum"));
			m_AccountsDBAide.GetValue_String(TEXT("Notice"),CMD1.szNotice,CountArray(CMD1.szNotice));
			m_AccountsDBAide.GetValue_String(TEXT("Message"),CMD1.szMessage,CountArray(CMD1.szMessage));
			CMD1.dwFK =m_AccountsDBAide.GetValue_DWORD(TEXT("FK"));
			CMD1.byApplyNum =m_AccountsDBAide.GetValue_BYTE(TEXT("ApplyNum"));
			CMD1.byAutoAgree =m_AccountsDBAide.GetValue_BYTE(TEXT("AutoAgree"));
			CMD1.bySex =m_AccountsDBAide.GetValue_BYTE(TEXT("Sex"));
			CMD1.wLevel =m_AccountsDBAide.GetValue_WORD(TEXT("Level"));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_DATA,dwContextID,&CMD1,sizeof(CMD1));
		}
		else //������������Ϣ��ȡʧ��, ����ĳ�ԱҲ����Ҫ��ȡ��
		{
			return true;
		}

		/* ��Ա���ݷ��� */
		//��������
		m_AccountsDBAide.ResetParameter();

		//�������
		m_AccountsDBAide.AddParameter(TEXT("@UserID"),pDBR->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@ClubID"),pDBR->dwClubID);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_CLUB_MEMBER_MANAGER"));
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_CL_CLUB_MEMBER_MANAGER"),true);

		//�б���
		WORD wPacketSize=0;
		BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
		wPacketSize=0;
		STR_CMD_LC_CLUB_MEMBER_MANAGER * pCMD=NULL;
		while ((lResultCode == DB_SUCCESS) && (m_AccountsDBModule->IsRecordsetEnd()==false))
		{
			//������Ϣ
			if ((wPacketSize+sizeof(STR_CMD_LC_CLUB_MEMBER_MANAGER))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_MEMBER_MANAGER,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//��ȡ��Ϣ
			pCMD=(STR_CMD_LC_CLUB_MEMBER_MANAGER *)(cbBuffer+wPacketSize);

			pCMD->dwUserID =m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
			m_AccountsDBAide.GetValue_String(TEXT("NickName"),pCMD->szUserName,CountArray(pCMD->szUserName));
			pCMD->byClubRole =m_AccountsDBAide.GetValue_BYTE(TEXT("RoleID"));
			pCMD->byUserLevel =m_AccountsDBAide.GetValue_BYTE(TEXT("UserLevel"));
			pCMD->byClubReputation =m_AccountsDBAide.GetValue_BYTE(TEXT("ClubReputation"));
			pCMD->byUserState =m_AccountsDBAide.GetValue_BYTE(TEXT("IsOnline"));
			m_AccountsDBAide.GetValue_String(TEXT("HeadUrl"),pCMD->szHeadUrl,CountArray(pCMD->szHeadUrl));

			//����λ��
			wPacketSize+=sizeof(STR_CMD_LC_CLUB_MEMBER_MANAGER);

			//�ƶ���¼
			m_AccountsDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_MEMBER_MANAGER,dwContextID,cbBuffer,wPacketSize);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LC_CLUB_MEMBER_MANAGER_FINISH,dwContextID,NULL,0);
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}

}

#pragma endregion

#pragma region MDM_SHOP �̳ǵ���
//DBR && DBO��ѯ�̳�
bool CDataBaseEngineSink::On_DBR_CL_SHOP_QUERY(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
	{
		//Ч�����
		if (wDataSize!=sizeof(STR_DBR_CL_SHOP_QUERY)) return false;
		STR_DBR_CL_SHOP_QUERY *pDBR = (STR_DBR_CL_SHOP_QUERY*) pData;

		//��������
		m_PlatformDBAide.ResetParameter();

		m_PlatformDBAide.AddParameter(TEXT("@Mystery"),_MYSTERY);
		m_PlatformDBAide.AddParameter(TEXT("@GoodsType"),pDBR->byGoodsType);
		m_PlatformDBAide.AddParameter(TEXT("@UserID"),pDBR->dwUserID);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_SHOP_QUERY"));

		LONG lResultCode  = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_CL_SHOP_QUERY"),true);

		//�б���
		//��������
		WORD wPacketSize=0;
		BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
		wPacketSize=0;
		STR_DBO_CL_SHOP_QUERY * pDBO=NULL;
		while ((lResultCode == DB_SUCCESS) && (m_PlatformDBModule->IsRecordsetEnd()==false))
		{
			//������Ϣ
			if ((wPacketSize+sizeof(STR_DBO_CL_SHOP_QUERY))>sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_SHOP_QUERY,dwContextID,cbBuffer,wPacketSize);
				wPacketSize=0;
			}

			//��ȡ��Ϣ
			pDBO=(STR_DBO_CL_SHOP_QUERY *)(cbBuffer+wPacketSize);
			pDBO->dwGoodsID=m_PlatformDBAide.GetValue_DWORD(TEXT("GoodsID"));
			pDBO->byDiscount=m_PlatformDBAide.GetValue_BYTE(TEXT("Discount"));
			pDBO->dbSpreadScore=m_PlatformDBAide.GetValue_DWORD(TEXT("SpreadScore"));
			pDBO->dwPrice=m_PlatformDBAide.GetValue_DWORD(TEXT("Price"));

			//����λ��
			wPacketSize+=sizeof(STR_DBO_CL_SHOP_QUERY);

			//�ƶ���¼
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_SHOP_QUERY,dwContextID,cbBuffer,wPacketSize);

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_SHOP_QUERY_FINISH,dwContextID,NULL,0);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
}

//DBR && DBO��ʯ�������
bool CDataBaseEngineSink::On_DBR_CL_SHOP_DIAMOND(DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	try
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
		m_PlatformDBAide.ResetParameter();

		//�������
		m_PlatformDBAide.AddParameter(TEXT("@Mystery"),_MYSTERY);
		m_PlatformDBAide.AddParameter(TEXT("@Shopper"),DBR.dwShopper);
		m_PlatformDBAide.AddParameter(TEXT("@GoodsType"),DBR.byGoodsType);
		m_PlatformDBAide.AddParameter(TEXT("@GoodsID"),DBR.dwGoodsID);
		m_PlatformDBAide.AddParameter(TEXT("@Mask"),DBR.byMask);
		m_PlatformDBAide.AddParameter(TEXT("@TargetID"), DBR.dwTargetID);
		m_PlatformDBAide.AddParameter(TEXT("@GoodsNum"), DBR.dwGoodsNum);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//���ִ��
		LogicTraceHelper(TEXT("GSP_CL_SHOP_BUY"));

		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_CL_SHOP_BUY"),true);

		//�������
		CDBVarValue DBVarValue;
		m_PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//�ṹ�幹��
		STR_DBO_CL_SHOP_DIAMOND DBO;
		ZeroMemory(&DBO, sizeof(DBO));
		DBO.lResultCode = lResultCode;
		lstrcpyn(DBO.szDescribeString,CW2CT(DBVarValue.bstrVal),CountArray(DBO.szDescribeString));

		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_SHOP_DIAMOND,dwContextID,&DBO,sizeof(DBO));
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
		return false;
	}
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
	m_PlatformDBAide.ResetParameter();
	//�������
	m_PlatformDBAide.AddParameter(TEXT("@UserID"),DBR.dwUserID);

	//���ִ��
	LogicTraceHelper(TEXT("GSP_CL_BAG_QUERY"));
	LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_CL_BAG_QUERY"),true);

	//�б���
	WORD wPacketSize=0;
	BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
	wPacketSize=0;
	STR_DBO_CL_BAG_QUERY * pDBO=NULL;
	while ((DB_SUCCESS == lResultCode) && (m_PlatformDBModule->IsRecordsetEnd()==false))
	{
		//������Ϣ
		if ((wPacketSize+sizeof(STR_DBO_CL_BAG_QUERY))>sizeof(cbBuffer))
		{
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_BAG_QUERY,dwContextID,cbBuffer,wPacketSize);
			wPacketSize=0;
		}

		//��ȡ��Ϣ
		pDBO=(STR_DBO_CL_BAG_QUERY *)(cbBuffer+wPacketSize);
		pDBO->dwGoodsID = m_PlatformDBAide.GetValue_DWORD(TEXT("GoodsID"));
		pDBO->dwGoodsNum = m_PlatformDBAide.GetValue_DWORD(TEXT("GoodsNum"));
		pDBO->byGoodsType = m_PlatformDBAide.GetValue_BYTE(TEXT("GoodsType"));

		//����λ��
		wPacketSize+=sizeof(STR_DBO_CL_BAG_QUERY);

		//�ƶ���¼
		m_PlatformDBModule->MoveToNext();
	}
	if (wPacketSize>0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_BAG_QUERY,dwContextID,cbBuffer,wPacketSize);


	//��������
	m_PlatformDBAide.ResetParameter();
	//�������
	m_PlatformDBAide.AddParameter(TEXT("@UserID"),DBR.dwUserID);

	//���ִ��
	LogicTraceHelper(TEXT("GSP_CL_BAG_QUERY_LOVENESS"));
	lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_CL_BAG_QUERY_LOVENESS"),true);

	STR_CMD_LC_BAG_FINISH CMD2;
	ZeroMemory(&CMD2, sizeof(CMD2));

	if(DB_SUCCESS == lResultCode)
	{
		CMD2.dwLoveness = m_PlatformDBAide.GetValue_DWORD(TEXT("Loveness"));
	}

	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_CL_BAG_FINISH, dwContextID, &CMD2, sizeof(CMD2));

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