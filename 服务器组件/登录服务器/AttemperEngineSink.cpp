#include "GameCtrl.h"
#include "AttemperEngineSink.h"
#include "HandleFromCenter.h"
#include "HandleFromGate.h"
#include "PlayerManager.h"


CAttemperEngineSink* g_AttemperEngineSink = NULL;
//////////////////////////////////////////////////////////////////////////////////

# pragma region ��ʱ��
#define  IDI_CONNECT_CORRESPOND		2									//������ʶ
#define	 TIME_CONNECT_CORRESPOND	30*1000L							//����ʱ��

//���캯��
CAttemperEngineSink::CAttemperEngineSink()
{
	if(!g_AttemperEngineSink)
		g_AttemperEngineSink = this;

	return;
}

//ʱ���¼�
bool CAttemperEngineSink::OnEventTimer(DWORD dwTimerID, WPARAM wBindParam)
{
	switch (dwTimerID)
	{
	case IDI_CONNECT_CORRESPOND:	//����Э��
		{
			g_GameCtrl->KillTimer(IDI_CONNECT_CORRESPOND);
			//��������
			g_TCPSocketEngine->Connect(_CPD_SERVER_ADDR, PORT_CENTER);

			return true;
		}
	}

	return false;
}

//���ݿ��¼�
bool CAttemperEngineSink::OnEventDataBaseResult(WORD wRequestID, DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
#pragma region ��¼ģ��
	case DBO_CL_LOGON_ACCOUNTS:			//�˺ŵ�¼
		{
			return On_CMD_LC_Logon_Account(dwScoketID,pData,wDataSize);
		}
	case DBO_CL_LOGON_PLATFORM:			//��������½
		{
			return On_CMD_LC_Logon_Platform(dwScoketID, pData, wDataSize);
		}
#pragma endregion

	case DBO_CL_SERVICE_USER_FEEDBACK:			//��ҷ�������
		{
			return On_CMD_LC_Service_UserFeedBack(dwScoketID, pData);
		}
	case DBO_CL_SERVICE_REFRESH_USER_INFO:		//ˢ���û���Ϣ����
		{
			return On_CMD_LC_Service_RefreshUserInfo(dwScoketID, pData);
		}
	case DBO_CL_SERVICE_QUERY_ROOM_LIST:		//��ѯ�����б���
		{
			return On_CMD_LC_Service_QueryRoomList(dwScoketID,pData,wDataSize);
		}
	case DBO_CL_SERVICE_GET_RICH_LIST:			//��ȡ�����񷵻�
		{
			return On_CMD_LC_Service_GetRichList(dwScoketID,pData,wDataSize);
		}
	case DBO_CL_SERVICE_GET_USER_RECORD_LIST:	//��ȡ�û�¼���б��أ���֣�
		{
			return On_CMD_LC_Service_GetUserRecordList(dwScoketID,pData,wDataSize);
		}
	case DBO_CL_SERVICE_GET_SPECIFIC_RECORD:	//��ȡָ��ID¼�񷵻أ�С�֣�
		{
			return On_CMD_LC_Service_GetSpecificRecord(dwScoketID,pData,wDataSize);
		}
	case DBO_CL_SERVICE_ONLINE_REWARD:			//��ȡ���߽�������
		{
			return On_CMD_LC_Service_OnlineReward(dwScoketID,pData,wDataSize);
		}
	case DBO_CL_SERVICE_GET_TASK_LIST:			//��ȡ�����б���
		{
			return On_CMD_LC_Service_GetTaskList(dwScoketID,pData,wDataSize);
		}
	case DBO_CL_SERVICE_GET_TASK_REWARD:		//��ȡ����������
		{
			return On_CMD_LC_Service_GetTaskReward(dwScoketID,pData,wDataSize);
		}
	case DBO_CL_SERVICE_REQUEST_LOTTERY:		//����齱����
		{
			return On_CMD_LC_Service_RequestLottery(dwScoketID,pData,wDataSize);
		}
	case DBO_CL_SERVICE_MODIFY_PERSONAL_INFO:	//�޸ĸ������Ϸ���
		{
			return On_CMD_LC_Service_ModifyPersonalInfo(dwScoketID,pData,wDataSize);
		}
	case DBO_CL_USER_QUERY_SCORE_INFO:			//��ѯ(���)��ҷ���
		{
			return On_CMD_LC_Service_QueryScoreInfo(dwScoketID,pData,wDataSize);
		}
	
	case DBO_CL_USER_COMMAND_RESULT:		//�����������
		{
			return On_CMD_LC_CommonOperateResult(dwScoketID,pData,wDataSize);
		}
#pragma region MDM_GIFT �������
	case DBO_CL_GIFT_GIVE_PROPS:			//�������� ����
		{
			return On_CMD_LC_GIFT_GIVE_PROPS(dwScoketID,pData,wDataSize);
		}
	case DBO_CL_GIFT_GIVE_PROPS_SHOW:		//�������� ֪ͨ������
		{
			return On_CMD_LC_GIFT_GIVE_PROPS_SHOW(dwScoketID,pData,wDataSize);
		}
#pragma endregion
	case DBO_CL_USER_RECHARGE_INFO:			//��ֵ��Ϣ����
		{
			return On_CMD_LC_Other_RechargeInfo(dwScoketID,pData,wDataSize);
		}
	case DBO_CL_USER_EXCHANGE_INFO:			//�һ����߷���
		{
			return On_CMD_LC_Other_ExchangeInfo(dwScoketID,pData,wDataSize);
		}
	case DBO_CL_SERVICE_GET_RANK_REWARD:		//������а���	
		{
			return OnDBRankRewardResult(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_PURE_STANDING_LIST:		//pure�������а� ����
		{
			return On_CMD_LC_SERVICE_PURE_STANDING_LIST(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_PURE_STANDING_FINISH:	//pure�������а� ����
		{
			return On_CMD_LC_SERVICE_PURE_STANDING_FINISH(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_PURE_RECORD_LIST:		//���ս��
		{
			return On_CMD_LC_SERVICE_PURE_RECORD_LIST(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_PURE_RECORD_LIST_PINFO:	//��������Ϣ
		{
			return On_CMD_LC_SERVICE_PURE_RECORD_LIST_PINFO(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_PURE_RECORD_FINISH://���ս������
		{
			return On_CMD_LC_SERVICE_PURE_RECORD_FINISH(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_PURE_XJ_RECORD_LIST:		//С��ս��
		{
			return On_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_PURE_XJ_RECORD_LIST_PINFO:	//С�������Ϣ
		{
			return On_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_PINFO(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_XJ_RECORD_PLAYBACK:			//С��¼��ط�
		{
			return On_CMD_LC_Service_XJRecordPlayback(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_CUSTOMER_MESSEGE:		//�ͷ���Ϣ
		{
			return On_CMD_LC_SERVICE_CUSTOMER_MESSEGE(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_PURE_XJ_RECORD_FINISH://С��ս������
		{
			return On_CMD_LC_SERVICE_PURE_XJ_RECORD_FINISH(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_GOLD_INFO:		//�����Ҵ��� ����
		{
			return On_CMD_LC_SERVICE_GOLD_INFO(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_GOLD_INFO_FINISH:	//�����Ҵ��� ����
		{
			return On_CMD_LC_SERVICE_GOLD_INFO_FINISH(dwScoketID,pData,wDataSize);
		}
			 
#pragma region MDM_CLUB ����Ȧ
	case DBO_LC_CLUB_ALL_CLUB_INFO_LIST:		//��ѯ����Ȧ�б� ����
		{
			return On_CMD_LC_CLUB_ALL_CLUB_INFO_LIST(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_ALL_INFO_FINISH:		//��ѯ����Ȧ�б� ����
		{
			return On_CMD_LC_CLUB_ALL_INFO_FINISH(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_ROOM_LIST: //��ѯָ������Ȧ�����б� ����
		{
			return On_CMD_LC_CLUB_ROOM_LIST(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_ROOM_LIST_FINISH: //��ѯָ������Ȧ�����б� ����
		{
			return On_CMD_LC_CLUB_ROOM_LIST_FINISH(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_RANDOM_CLUB_LIST : //��ѯδ��Ա�������Ȧ ����
		{
			return On_CMD_LC_CLUB_RANDOM_CLUB_LIST(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_RANDOM_CLUB_LIST_FINISH : //��ѯδ��Ա�������Ȧ  ����
		{
			return On_CMD_LC_CLUB_RANDOM_CLUB_LIST_FINISH(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_JOIN_CLUB: //�����������Ȧ
		{
			return On_CMD_LC_CLUB_JOIN_CLUB(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_JOIN_CLUB_BDCAST: //�����������Ȧ �㲥
		{
			return On_CMD_LC_CLUB_JOIN_CLUB_BDCAST(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_JOIN_CLUB_RE:	//�����������Ȧ �ͻ���ʵʱˢ�¾��ֲ�
		{
			return On_CMD_LC_CLUB_JOIN_CLUB_RE(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_DISS_CLUB : //��ɢ����Ȧ
		{
			return On_CMD_LC_CLUB_DISS_CLUB(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_ROOM_SETTING: //��������
		{
			return On_CMD_LC_CLUB_ROOM_SETTING(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_ROOM_QUERY_SETTING: //���󷿼�����
		{
			return On_CMD_LC_CLUB_ROOM_QUERY_SETTING(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_ROOM_DISSOLVE: //��ɢ�������� ����
		{
			return On_CMD_LC_CLUB_ROOM_DISSOLVE(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_TABLE_DISSOLVE: //��ɢ�������� ����
		{
			return On_CMD_LC_CLUB_TABLE_DISSOLVE(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_NOTICE: //����Ȧ����
		{
			return On_CMD_LC_CLUB_NOTICE(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_MESSAGE: //����Ȧ���
		{
			return On_CMD_LC_CLUB_MESSAGE(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_CONTRIBUTE_FK://���׷���
		{
			return On_CMD_LC_CLUB_CONTRIBUTE_FK(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_AUTO_AGREE://����Ȧ����
		{
			return On_CMD_LC_CLUB_AUTO_AGREE(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_INVITE://�������˼�������Ȧ
		{
			return On_CMD_LC_CLUB_INVITE(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_INVITE_REMIND://�������˵�����
		{
			return On_CMD_LC_CLUB_INVITE_REMIND(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_INVITE_RESULT: //�������� �ظ�
		{
			return On_CMD_LC_CLUB_INVITE_RESULT(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_INQUERY_LIST: //�������˲鿴�Լ��������б� ����
		{
			return On_CMD_LC_CLUB_INQUERY_LIST(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_APPLICANT_LIST://�������б� ����
		{
			return On_CMD_LC_CLUB_APPLICANT_LIST(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_APPLICANT_LIST_FINISH://�������б� ����
		{
			return On_CMD_LC_CLUB_APPLICANT_LIST_FINISH(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_APPOINTMENT: //ְ������
		{
			return On_CMD_LC_CLUB_APPOINTMENT(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_APPOINTMENT_NOTE: //ְ������ ����
		{
			return On_CMD_LC_CLUB_APPOINTMENT_NOTE(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CHAT_ALL://���ֲ����� ����
		{
			return On_CMD_LC_CLUB_CHAT(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CHAT_ALL_BDCAST://���ֲ����� �㲥
		{
			return On_CMD_LC_CLUB_CHAT_BDCAST(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_STICKY_POST:	//����Ȧ�ö�����
		{
			return On_CMD_LC_CLUB_STICKY_POST(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_INQUERY_LIST_FINISH: //�������˲鿴�Լ��������б� ����
		{
			return On_CMD_LC_CLUB_INQUERY_LIST_FINISH(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_TABLE_LIST_TABLE: //�����б�
		{
			return On_CMD_LC_CLUB_TABLE_LIST_TABLE(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_TABLE_LIST_USER:	//��������б�
		{
			return On_CMD_LC_CLUB_TABLE_LIST_USER(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_CREATE_CLUB://��������Ȧ ����
		{
			return On_CMD_LC_CLUB_CREATE_CLUB(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_JOIN_ROOM: //������뷿�� ����
		{
			return On_CMD_LC_CLUB_JOIN_ROOM(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_APPLICANT_RESULT: //Ⱥ��|�����������Ϣ�Ĵ�(ͬ��|�ܾ�) ����
		{
			return On_CMD_LC_CLUB_APPLICANT_RESULT(dwScoketID,pData,wDataSize);
		}
		/*
	case DBO_LC_CLUB_JOIN_CLUB_MESSAGE_FINISH: //Ⱥ��|�����������Ϣ�Ĵ�(ͬ��|�ܾ�) ����
		{
			return On_CMD_LC_CLUB_APPLICANT_LIST_FINISH(dwScoketID,pData,wDataSize);
		}
		*/
	case DBO_LC_CLUB_MEMBER_MANAGER://�����Ա���� ����
		{
			return On_CMD_LC_CLUB_MEMBER_MANAGER(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_MEMBER_MANAGER_FINISH://�����Ա���� ����
		{
			return On_CMD_LC_CLUB_MEMBER_MANAGER_FINISH(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_DATA ://���������Ϣ
		{
			return On_CMD_LC_CLUB_DATA(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_QUIT://�߳��˳����� ����
		{
			return On_CMD_LC_CLUB_QUIT(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_RECORD_LIST: //����ս��ͳ�� ����
		{
			return On_CMD_LC_CLUB_RECORD_LIST(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_RECORD_FINISH: //����ս��ͳ�� ����
		{
			return On_CMD_LC_CLUB_RECORD_FINISH(dwScoketID,pData,wDataSize);
		}
#pragma endregion
#pragma region MDM_SHOP �̳ǵ���
	case DBO_CL_SHOP_QUERY:		//��ѯ�̳�
		{
			return On_CMD_LC_SHOP_QUERY_RESULT(dwScoketID,pData,wDataSize);
		}
	case DBO_CL_SHOP_QUERY_FINISH: //��ѯ�̳ǽ���
		{
			return On_CMD_LC_SHOP_QUERY_FINISH(dwScoketID,pData,wDataSize);
		}
	case DBO_CL_SHOP_DIAMOND : //��ʯ�������
		{
			return On_CMD_LC_SHOP_DIAMOND_RESULT(dwScoketID,pData,wDataSize);
		}
	case DBO_CL_BAG_QUERY: //������Ʒ��ѯ
		{
			return On_CMD_LC_BAG_RESULT(dwScoketID,pData,wDataSize);
		}
	case DBO_CL_BAG_FINISH://������Ʒ��ѯ���
		{
			return On_CMD_LC_BAG_FINISH(dwScoketID,pData,wDataSize);
		}
#pragma endregion
	}

	return false;
}

//Ӧ���¼�
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID)
{
	/*
	//��ȡ����
	if (LOWORD(dwSocketID)>=MAX_CONTENT) return false;

	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//���ñ���
	pBindParameter->dwSocketID=dwSocketID;
	pBindParameter->dwClientAddr=dwClientAddr;
	pBindParameter->dwActiveTime=(DWORD)time(NULL);
	*/
	return true;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID)
{
	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	///Socket���ӹر� ���ݿ⴦��
	if(pBindParameter->dwUserID != 0)
	{
		DBR_GP_UserQuitInfo quitInfo;
		quitInfo.dwUserID = pBindParameter->dwUserID;

		//1�������� ��2������Ϸ��3��Ϸ���ߣ�4����
		if(bIsExsit)
		{
			quitInfo.byOnlineMask = 3;
		}
		else
		{
			quitInfo.byOnlineMask = 4;
		}
		g_GameCtrl->PostDataBaseRequest(DBR_GP_LOGON_USER_STATE,dwSocketID, &quitInfo,sizeof(quitInfo));
	}

	return false;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	return CHandleFromGate::HandlePacket(Command, pData, wDataSize, dwSocketID);
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
			//����ʱ��
			g_GameCtrl->SetTimer(IDI_CONNECT_CORRESPOND, TIME_CONNECT_CORRESPOND, 1, 0);
			return false;
		}

		//��������
		tagServerItem ServerItem;
		ZeroMemory(&ServerItem,sizeof(ServerItem));

		//��������
		ServerItem.dwServerID = 0; //TODOLATER lobby.serverid = (0 << 16)  + nodeid;  //lobby.kind = 0
		ServerItem.byServerType = LOBBY_TYPE;
		//lstrcpyn(ServerItem.szServerName,pCPR->szServerName,CountArray(ServerItem.szServerName));
		ServerItem.wServerPort = PORT_LOGON;

		//���ñ���
		TCHAR szInernet_ip[32] = TEXT("0.0.0.0");
		//GetInternetIP(szInernet_ip);
		//lstrcpyn(CPR.szServerAddr,szInernet_ip ,CountArray(CPR.szServerAddr));
		lstrcpyn(ServerItem.szServerAddr,TEXT("127.0.0.1"),CountArray(ServerItem.szServerAddr));

		//��������
		g_TCPSocketEngine->SendData(MDM_REGISTER,CPR_REGISTER_SERVER,&ServerItem,sizeof(ServerItem));

		return true;
	}

	return true;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason)
{
	//Э������
	if (wServiceID==NETWORK_CORRESPOND)
	{
		//����ʱ��
		g_GameCtrl->SetTimer(IDI_CONNECT_CORRESPOND, TIME_CONNECT_CORRESPOND, 1, 0);
		return true;
	}

	return false;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize)
{
	return CHandleFromCenter::HandlePacket(Command, pData, wDataSize, wServiceID);
}








