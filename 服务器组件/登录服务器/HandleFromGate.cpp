#include "HandleFromGate.h"
#include "GameCtrl.h"
#include "PlayerManager.h"
#include "Player.h"
#include "RankManager.h"

//��Ϣ�ַ�������
bool CHandleFromGate::HandlePacket(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
		switch (Command.wMainCmdID)
	{
	case MDM_LOGON:				//��¼����
		{
			return OnTCPNetworkMainLogon(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_SERVICE:			//��������
		{
			return OnTCPNetworkMainService(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_CLUB:				//����Ȧ(�汾2)
		{
			return On_MDM_CLUB(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_SHOP:				//�̳ǵ���
		{
			return On_MDM_SHOP(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GAME:				//��Ϸ�����
		{
			return On_MDM_GAME(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}

	}

	return false;
}

bool CHandleFromGate::HandlePacketDB(WORD wRequestID, DWORD dwScoketID, VOID * pData, WORD wDataSize)
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

	case DBO_CL_SERVICE_REFRESH_USER_INFO:		//ˢ���û���Ϣ����
		{
			return On_CMD_LC_Service_RefreshUserInfo(dwScoketID, pData);
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
	case DBO_LC_SERVICE_PURE_XJ_RECORD_FINISH://С��ս������
		{
			return On_CMD_LC_SERVICE_PURE_XJ_RECORD_FINISH(dwScoketID,pData,wDataSize);
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
	case DBO_CL_BAG_RESULT: //��ѯ��������
		{
			return On_DBO_CL_BAG_RESULT(dwScoketID,pData,wDataSize);
		}
	}
	return true;
}

//��¼ģ��
bool CHandleFromGate::OnTCPNetworkMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CL_LOGON_ACCOUNTS:		//�ʺŵ�¼
		{
			return On_SUB_CL_Logon_Accounts(pData,wDataSize,dwSocketID);
		}
	case SUB_CL_LOGON_REGISTER:		//�ʺ�ע��
		{
			return On_SUB_CL_Logon_Register(pData,wDataSize,dwSocketID);
		}
	case SUB_CL_LOGON_PLATFORM:		//ƽ̨��¼
		{
			return On_SUB_CL_Logon_Platform(pData,wDataSize,dwSocketID);
		}
	}
	return false;
}
//�û�����
bool CHandleFromGate::OnTCPNetworkMainService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CL_SERVICE_FLOWER: //��ע�� ��˿
		{
			return On_SUB_CL_SERVICE_FLOWER(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_FLOWER_QUERY:  //�鿴��ע
		{
			return On_SUB_CL_SERVICE_FLOWER_QUERY(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_FLOWING_QUERY: //�鿴��˿
		{
			return On_SUB_CL_SERVICE_FLOWING_QUERY(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_FLOWER_ACT: //��ע, ȡ����ע
		{
			return On_SUB_CL_SERVICE_FLOWER_ACT(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_REFRESH_USER_INFO:		//ˢ���û���Ϣ
		{
			return On_SUB_CL_Service_RefreshUserInfo(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_MODIFY_PERSONAL_INFO:	//�޸ĸ�������
		{
			return On_SUB_CL_Service_ModifyPersonalInfo(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_GET_DJ_RECORD_LIST:	//��ȡ�û�¼���б�
		{
			return On_SUB_CL_Service_GetUserRecordList(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_GET_XJ_RECORD_LIST:	//��ȡָ��ID¼��
		{
			return On_SUB_CL_Service_GetSpecificRecord(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_ONLINE_REWARD:			//��ȡ���߽���
		{
			return On_SUB_CL_Service_OnlineReward(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_GET_TASK_LIST:			//��ȡ�����б�
		{
			return On_SUB_CL_Service_GetTaskList(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_GET_TASK_REWARD:		//��ȡ������
		{
			return On_SUB_CL_Service_GetTaskReward(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_GET_RANK_LIST:			//��ȡ���а�
		{
			return On_SUB_CL_Service_GetRankList(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_REQUEST_LOTTERY:		//����齱
		{
			return On_SUB_CL_Service_RequestLottery(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_PURE_STANDING_LIST:		//pure�������а�
		{
			return On_SUB_CL_SERVICE_PURE_STANDING_LIST(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_PURE_RECORD_LIST:		//���ս��
		{
			return On_SUB_CL_SERVICE_PURE_RECORD_LIST(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_PURE_XJ_RECORD_LIST:	//С��ս��
		{
			return On_SUB_CL_SERVICE_PURE_XJ_RECORD_LIST(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_XJ_RECORD_PLAYBACK:	//С��¼��ط�
		{
			return On_SUB_CL_Service_XJRecordPlayback(pData, wDataSize, dwSocketID);
		}
	}

	return false;
}
//����Ȧ
bool CHandleFromGate::On_MDM_CLUB(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch(wSubCmdID)
	{
	case SUB_CL_CLUB_ALL_CLUB_INFO_LIST:	 //��ѯ����Ȧ�б�
		{
			return On_SUB_CL_CLUB_ALL_CLUB_INFO_LIST(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_ROOM_LIST:  //��ѯָ������Ȧ�����б�
		{
			return On_SUB_CL_CLUB_ROOM_LIST(pData, wDataSize, dwSocketID);	
		}
	case SUB_CL_CLUB_RANDOM_CLUB_LIST: //��ѯδ��Ա, �������Ȧ(���9��)
		{
			return On_SUB_CL_CLUB_RANDOM_CLUB_LIST(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_JOIN_CLUB: //�����������Ȧ
		{
			return On_SUB_CL_CLUB_JOIN_CLUB(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_DISS_CLUB: //��ɢ����Ȧ
		{
			return On_SUB_CL_CLUB_DISS_CLUB(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_CREATE_CLUB:	 //��������Ȧ
		{
			return On_SUB_CL_CLUB_CREATE_CLUB(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_JOIN_ROOM: //������뷿��
		{
			return On_SUB_CL_CLUB_JOIN_ROOM(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_ROOM_SETTING: //��������
		{
			return On_SUB_CL_CLUB_ROOM_SETTING(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_ROOM_QUERY_SETTING://���󷿼�����
		{
			return On_SUB_CL_CLUB_ROOM_QUERY_SETTING(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_ROOM_USER_LEAVE://����뿪����
		{
			return On_SUB_CL_CLUB_ROOM_USER_LEAVE(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_ROOM_DISSOLVE:	//��ɢ����
		{
			return On_SUB_CL_CLUB_ROOM_DISSOLVE(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_TABLE_DISSOLVE:	//��ɢ����
		{
			return On_SUB_CL_CLUB_TABLE_DISSOLVE(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_NOTICE: //����Ȧ����
		{
			return On_SUB_CL_CLUB_NOTICE(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_MESSAGE: //����Ȧ���
		{
			return On_SUB_CL_CLUB_MESSAGE(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_CONTRIBUTE_FK: //���׷���
		{
			return On_SUB_CL_CLUB_CONTRIBUTE_FK(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_AUTO_AGREE: //����Ȧ����
		{
			return On_SUB_CL_CLUB_AUTO_AGREE(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_CHAT://����Ȧ ����
		{
			return On_SUB_CL_CLUB_CHAT(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_STICKY_POST:	//����Ȧ�ö�
		{
			return On_SUB_CL_CLUBSTICKY_POST(pData, wDataSize, dwSocketID);
		}
		/*
	case SUB_CL_CLUB_CREATE_CLUB_ROOM_APPLY:  //��������Ȧ����
		{
			return On_SUB_CL_CLUB_CREATE_CLUB_ROOM_APPLY(pData, wDataSize, dwSocketID);	
		}
		*/
	case SUB_CL_CLUB_APPLICANT_RESULT:	 //Ⱥ��|�����������Ϣ�Ĵ�(ͬ��|�ܾ�)
		{
			return On_SUB_CL_CLUB_APPLICANT_RESULT(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_MEMBER_MANAGER:  //�����Ա����
		{
			return On_SUB_CL_CLUB_MEMBER_MANAGER(pData, wDataSize, dwSocketID);	
		}
	case SUB_CL_CLUB_INVITE://�������˼�������Ȧ
		{
			return On_SUB_CL_CLUB_INVITE(pData, wDataSize, dwSocketID);	
		}
	case SUB_CL_CLUB_INVITE_RESULT:// �������� �ظ�
		{
			return On_SUB_CL_CLUB_INVITE_RESULT(pData, wDataSize, dwSocketID);	
		}
	case SUB_CL_CLUB_INQUERY_LIST://�������˲鿴�Լ��������б�
		{
			return On_SUB_CL_CLUB_INQUERY_LIST(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_APPLICANT_LIST: //�������б�
		{
			return On_SUB_CL_CLUB_APPLICANT_LIST(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_QUIT: //�߳��˳�����
		{
			return On_SUB_CL_CLUB_QUIT(pData, wDataSize, dwSocketID);
		}
	case  SUB_CL_CLUB_APPOINTMENT://ְ������
		{
			return On_SUB_CL_CLUB_APPOINTMENT(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_RECORD_LIST:	//����ս��ͳ��
		{
			return On_SUB_CL_CLUB_RECORD_LIST(pData, wDataSize, dwSocketID);
		}
	}

	return false;
}

//�̳ǵ���
bool CHandleFromGate::On_MDM_SHOP(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch(wSubCmdID)
	{
	case SUB_CL_SHOP_BUY:	 //�̳ǹ���
		{
			return On_SUB_CL_SHOP_BUY(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_BAG_QUERY: //������Ʒ��ѯ
		{
			return On_SUB_CL_BAG_QUERY(pData, wDataSize, dwSocketID);
		}	
	case SUB_CL_GIFT_GIVE_PROPS:				//���͵���
		{
			return On_SUB_CL_GIFT_GIVE_PROPS(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_OTHERS_RECHARGE_INFO:				//��ֵ��Ϣ
		{
			return On_SUB_CL_Other_ReChargeInfo(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_OTHERS_EXCHANGE_INFO:				//�һ�������Ϣ
		{
			return On_SUB_CL_Other_ExchangeInfo(pData, wDataSize, dwSocketID);
		}
	}

	return false;
}
//��Ϸ�����
bool CHandleFromGate::On_MDM_GAME(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch(wSubCmdID)
	{
	case SUB_CL_GAME_QUERY_GAMEID:	 //��������֮ǰ, �Ȳ�ѯ���õ�GameID
		{
			//У������
			if (wDataSize!=sizeof(STR_SUB_CL_GAME_QUERY_GAMEID)) return true;

			STR_SUB_CL_GAME_QUERY_GAMEID *pSub = (STR_SUB_CL_GAME_QUERY_GAMEID*)pData;
			
			STR_CPR_LP_CREATE_TABLE CPR;
			CPR.dwKindID = pSub->dwKindID;
			CPR.dwSocketID = dwSocketID;

			CLog::Log(log_debug, "Game KindID: %d", CPR.dwKindID);

			g_TCPSocketEngine->SendData(CPD_MDM_TRANSFER, CPR_LP_CREATE_TABLE, &CPR, sizeof(CPR));

			return true;
		}
	}

	return false;
}


#pragma region MDM_LOGON ��¼ģ��
/***************************************** ����¼������-����Ϣ1�� *******************************************/
//�ʺŵ�¼
bool CHandleFromGate::On_SUB_CL_Logon_Accounts(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	if (wDataSize != sizeof(STR_SUB_CL_LOGON_ACCOUNTS))
	{
		return false;
	}

	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_CL_LOGON_ACCOUNTS,dwSocketID, pData, wDataSize);
	return true;
}

//�˺ŵ�¼����
bool CHandleFromGate::On_CMD_LC_Logon_Account(DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//����У��
	if(wDataSize != sizeof(STR_CMD_LC_LOGON_PLATFORM)) return false;

	//��������
	STR_CMD_LC_LOGON_PLATFORM * pCMD=(STR_CMD_LC_LOGON_PLATFORM *)pData;

	//�����¼ʧ��, ֱ�ӷ���
	if(DB_SUCCESS != pCMD->dwResultCode)
	{
		//��������
		g_GameCtrl->SendData(dwScoketID, MDM_LOGON, CMD_LC_LOGON_ACCOUNTS, pCMD, sizeof(STR_CMD_LC_LOGON_PLATFORM));
		return true;
	}

	//�ظ���¼����
	On_CMD_LC_Logon_RepeatLogon( pCMD->useInfo.dwUserID, dwScoketID );

	//������Ҽ�¼
	tagUserInfo UserInfo;
	ZeroMemory(&UserInfo, sizeof(UserInfo));
	memcpy(&UserInfo, &(pCMD->useInfo), sizeof(UserInfo));
	CPlayerManager::InsertPlayer (dwScoketID, UserInfo);

	//��ѯ��������
	STR_CPR_LP_OFFLINE_PLAYERQUERY CPR;
	CPR.dwUserID = pCMD->useInfo.dwUserID;
	g_TCPSocketEngine->SendData(CPD_MDM_TRANSFER, CPR_LP_OFFLINE_PLAYERQUERY, &CPR, sizeof(CPR));

	/* TODONOW
	//��¼����
	On_CMD_LC_Logon_Logon_Reward(dwScoketID, pCMD->LasLogonDate);
	*/

	return true;
}

//�ʺ�ע��
bool CHandleFromGate::On_SUB_CL_Logon_Register(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	if (wDataSize != sizeof(STR_SUB_CL_LOGON_REGISTER))
	{
		return false;
	}
	
	//������Ϣ
	STR_SUB_CL_LOGON_REGISTER * pSUBLogonRegister=(STR_SUB_CL_LOGON_REGISTER *)pData;

	//��������
	STR_DBR_CL_LOGON_REGISTER DBRLogonRegister;
	ZeroMemory(&DBRLogonRegister,sizeof(DBRLogonRegister));

	//���ݸ�ֵ
	lstrcpyn(DBRLogonRegister.szAccounts,pSUBLogonRegister->szAccounts,CountArray(DBRLogonRegister.szAccounts));
	lstrcpyn(DBRLogonRegister.szPassword,pSUBLogonRegister->szPassword,CountArray(DBRLogonRegister.szPassword));
	lstrcpyn(DBRLogonRegister.szNickName,pSUBLogonRegister->szNickName,CountArray(DBRLogonRegister.szNickName));
	DBRLogonRegister.cbGender = pSUBLogonRegister->cbGender;
	lstrcpyn(DBRLogonRegister.strMobilePhone,pSUBLogonRegister->strMobilePhone,CountArray(DBRLogonRegister.strMobilePhone));
	lstrcpyn(DBRLogonRegister.szMachineID,pSUBLogonRegister->szMachineID,CountArray(DBRLogonRegister.szMachineID));
	DBRLogonRegister.dwProxyID = pSUBLogonRegister->dwProxyID;

	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_CL_LOGON_REGISTER,dwSocketID,&DBRLogonRegister,sizeof(DBRLogonRegister));

	return true;
}

//ƽ̨��¼
bool CHandleFromGate::On_SUB_CL_Logon_Platform(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч�����
	if (wDataSize != sizeof(STR_SUB_CL_LOGON_PLATFORM))
	{
		return false;
	}

	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_CL_LOGON_PLATFORM, dwSocketID, pData, wDataSize);

	return true;}

//ƽ̨��¼����
bool CHandleFromGate::On_CMD_LC_Logon_Platform(DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	if(wDataSize != sizeof(STR_CMD_LC_LOGON_PLATFORM)) return false;

	//��������
	STR_CMD_LC_LOGON_PLATFORM * pCMD = (STR_CMD_LC_LOGON_PLATFORM *)pData;

	//�����¼ʧ��, ֱ�ӷ���
	if(DB_SUCCESS != pCMD->dwResultCode)
	{
		g_GameCtrl->SendData(dwScoketID, MDM_LOGON, CMD_LC_LOGON_ACCOUNTS, pCMD, sizeof(STR_CMD_LC_LOGON_PLATFORM));
		return true;
	}

	//�ظ���¼�߳�
	On_CMD_LC_Logon_RepeatLogon( pCMD->useInfo.dwUserID ,dwScoketID);

	//���������Ĵ���

	//��Ҽ�¼
	//CPlayerManager::InsertPlayer (dwScoketID, pCMD);

	//���͵�¼�ɹ�
	g_GameCtrl->SendData(dwScoketID,MDM_LOGON,CMD_LC_LOGON_PLATFORM, pCMD, sizeof(STR_CMD_LC_LOGON_PLATFORM));

	/*
	//��¼����
	On_CMD_LC_Logon_Logon_Reward(dwScoketID, pCMD->);
	*/

	return true;
}

//�ظ���¼�߳�
bool CHandleFromGate::On_CMD_LC_Logon_RepeatLogon(DWORD UserID, DWORD dwScoketID)
{
	//�ظ���¼
	bool bIsRepeatLogon = false;

	CPlayer* player = CPlayerManager::FindPlayerByID(UserID);
	if(player)
	{
		STR_CMD_LC_LOGON_REPEAT_LOGON pCMDLogonRepeatLogon;
		ZeroMemory(&pCMDLogonRepeatLogon, sizeof(STR_CMD_LC_LOGON_REPEAT_LOGON));

		//��ʾ֮ǰ�û��˺��ѵ�¼���޷���¼
		_snwprintf(pCMDLogonRepeatLogon.szDescribe, sizeof(pCMDLogonRepeatLogon.szDescribe), TEXT("�˺��Ѿ��������ط���¼"));
		g_GameCtrl->SendData(player->GetSocketID(), MDM_LOGON, CMD_LC_LOGON_REPEAT_LOGON, &pCMDLogonRepeatLogon, sizeof(STR_CMD_LC_LOGON_REPEAT_LOGON));
		//�ر����� --  �ر�֮ǰ������
		g_TCPNetworkEngine->CloseSocket(player->GetSocketID());
		//ɾ��֮ǰ���
		CPlayerManager::DeletePlayerByID(UserID);

		//��ʾ�ѵ�¼���û� TODONOW ʹ���µ���Ϣ, ������CMD_LC_LOGON_REPEAT_LOGON
		/*
		_snwprintf(pCMDLogonRepeatLogon.szDescribe, sizeof(pCMDLogonRepeatLogon.szDescribe), TEXT("�˺��������ط���¼"));
		g_GameCtrl->SendData(dwScoketID, MDM_LOGON, CMD_LC_LOGON_REPEAT_LOGON, &pCMDLogonRepeatLogon, sizeof(STR_CMD_LC_LOGON_REPEAT_LOGON));
		*/

		bIsRepeatLogon = true;
	}

	return bIsRepeatLogon;
}

//��¼����
bool CHandleFromGate::On_CMD_LC_Logon_Logon_Reward(DWORD dwScoketID, SYSTEMTIME LasLogonDate)
{ 
	// 1. ����Ӧ���Ƕ�ȡ���ݿ�
	// 2. ��¼���� -- 1)���ս���  2)����ҽ�����
	
	//���ϵͳʱ��
	SYSTEMTIME systime;
	GetLocalTime(&systime);

	CTime tmLeft(systime.wYear, systime.wMonth, systime.wDay, 0, 0, 0);
	CTime tmRight(LasLogonDate.wYear, LasLogonDate.wMonth, LasLogonDate.wDay, 0, 0, 0);

	//����ϴε�¼ʱ��
	CTimeSpan sp = tmLeft - tmRight;
	long MillisecondsL = (systime.wHour*3600 + systime.wMinute*60 + systime.wSecond)*1000 + systime.wMilliseconds;  
	long MillisecondsR = (LasLogonDate.wHour*3600 + LasLogonDate.wMinute*60 + LasLogonDate.wSecond)*1000 + LasLogonDate.wMilliseconds;  

	//����ʱ����
	LONGLONG seconds =  ((__int64)sp.GetDays()*86400000 + (MillisecondsL - MillisecondsR))/1000;//�˴�������

	//�����Ƿ���30��,����50����
	if(seconds > 30*24*3600)
	{
		//����û��Ƹ�
		DBR_GP_ModifyUserInsure modifyUserInsure;
		ZeroMemory(&modifyUserInsure,sizeof(DBR_GP_ModifyUserInsure));
		modifyUserInsure.lOpenRoomCard = 50;
		g_GameCtrl->PostDataBaseRequest(DBR_GP_MODIFY_USER_INSURE, dwScoketID, &modifyUserInsure, sizeof(DBR_GP_ModifyUserInsure));

		//����ҽ�������
		STR_CMD_LC_LOGON_LOGON_REWARD OldBackReward;
		OldBackReward.byRewardType = 2;
		OldBackReward.dwRewardCount = 50;
		lstrcpyn( OldBackReward.szDescribe,TEXT("��ӭ���������ѻ������һع齱��50����!"),CountArray(OldBackReward.szDescribe));

		g_GameCtrl->SendData(dwScoketID,MDM_SERVICE, CMD_LC_LOGON_LOGON_REWARD, &OldBackReward, sizeof(STR_CMD_LC_LOGON_LOGON_REWARD));
	}
	
	return true;
}

#pragma endregion

#pragma region MDM_SERVICE �û�����
/***************************************** ����������-����Ϣ3�� *******************************************/
//��˿��ע
 bool CHandleFromGate::On_SUB_CL_SERVICE_FLOWER(VOID * pData, WORD wDataSize, DWORD dwSocketID)
 {
	 if ( sizeof(STR_SUB_CL_SERVICE_FLOWER) != wDataSize) return true;
	 CPlayer * player = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	 if(player == NULL) return false;
	 
	 STR_SUB_CL_SERVICE_FLOWER* sub = (STR_SUB_CL_SERVICE_FLOWER*) pData;

	 g_AccountsDB->ResetParameter();
	 g_AccountsDB->AddParameter(TEXT("@dwTargetID"), sub->dwTargetID); 
	 g_AccountsDB->AddParameter(TEXT("@dwMyID"), player->GetUserID()); 

	 //ִ�в�ѯ
	 LONG lResultCode = g_AccountsDB->ExecuteProcess(TEXT("GSP_CL_FLOWER_ALL"),true);
	 if(lResultCode == DB_SUCCESS)
	 {
		 STR_CMD_LC_SERVICE_FLOWER cmd;
		 //�ҵĹ�ע
		 cmd.dwFolwerNum = g_AccountsDB->GetValue_DWORD(TEXT("FlowerNum"));
		 //�ҵķ�˿
		 cmd.dwFolwingNum = g_AccountsDB->GetValue_DWORD(TEXT("FlowingNum"));

		 int i = 0;
		 while(g_AccountsDB->IsRecordsetEnd() == false)
		 {
			 if(i >= 5) break;
			 STR_CMD_LC_SERVICE_FLOWER_P cmd_p;
			 cmd_p.dwUserID = g_AccountsDB->GetValue_DWORD(TEXT("UserID"));
			 g_AccountsDB->GetValue_String(TEXT("NickName"),cmd_p.szNickName,CountArray(cmd_p.szNickName));
			 cmd_p.cbGender = g_AccountsDB->GetValue_BYTE(TEXT("Gender"));
			 g_AccountsDB->GetValue_String(TEXT("HeadUrl"),cmd_p.szHeadUrl,CountArray(cmd_p.szHeadUrl));
			 cmd_p.dwLoveValue = g_AccountsDB->GetValue_DWORD(TEXT("LoveValue"));

			 memcpy(&cmd.folwingInfo[i], &cmd_p, sizeof(cmd_p));
			 i++;
		 }

		 //�Ƿ���ʾ��ע��ť
		 BYTE bGuanzhu	= g_AccountsDB->GetValue_DWORD(TEXT("cbIsGuanzhu")); //0��ʾû�й�ע
		 if ( sub->dwTargetID == player->GetUserID()) //�Լ�
		 {
			 cmd.cbIsGuanzhu = 0;
		 }
		 else if (bGuanzhu == 0 )
		 {
			  cmd.cbIsGuanzhu = 1;
		 }
		 else
		 {
			 cmd.cbIsGuanzhu = 2;
		 }

		 g_GameCtrl->SendData(dwSocketID, MDM_SERVICE, CMD_LC_SERVICE_FLOWER, &cmd, sizeof(cmd));
	 }
	 else
	 {
		 g_GameCtrl->SendDataMsg(dwSocketID, "��ѯʧ��");
	 }

	 return true;
 }
//�鿴�ҵĹ�ע
bool CHandleFromGate::On_SUB_CL_SERVICE_FLOWER_QUERY(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	 CPlayer * player = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	 if(player == NULL) return false;

	 g_AccountsDB->ResetParameter();
	 g_AccountsDB->AddParameter(TEXT("@dwUserID"), player->GetUserID()); 

	 //ִ�в�ѯ
	 LONG lResultCode = g_AccountsDB->ExecuteProcess(TEXT("GSP_CL_FLOWER_QUERY"),true);
	 if(lResultCode == DB_SUCCESS)
	 {
		 WORD wPacketSize=0;
		 BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
		 STR_CMD_LC_SERVICE_FLOWER_P * pCMD=NULL;
		 while(g_AccountsDB->IsRecordsetEnd() == false)
		 {
			 if ((wPacketSize+sizeof(STR_CMD_LC_SERVICE_FLOWER_P))>sizeof(cbBuffer))
			 {
				 g_GameCtrl->SendData(dwSocketID, MDM_SERVICE, CMD_LC_SERVICE_FLOWER_QUERY, cbBuffer, wPacketSize);
				 wPacketSize=0;
			 }

			 STR_CMD_LC_SERVICE_FLOWER_P cmd_p;
			 cmd_p.dwUserID = g_AccountsDB->GetValue_DWORD(TEXT("UserID"));
			 g_AccountsDB->GetValue_String(TEXT("NickName"),cmd_p.szNickName,CountArray(cmd_p.szNickName));
			 cmd_p.cbGender = g_AccountsDB->GetValue_BYTE(TEXT("Gender"));
			 g_AccountsDB->GetValue_String(TEXT("HeadUrl"),cmd_p.szHeadUrl,CountArray(cmd_p.szHeadUrl));
			 cmd_p.dwLoveValue = g_AccountsDB->GetValue_DWORD(TEXT("LoveValue"));

			 pCMD=(STR_CMD_LC_SERVICE_FLOWER_P *)(cbBuffer+wPacketSize);
			 memcpy(pCMD, &cmd_p, sizeof(cmd_p));

			 wPacketSize+=sizeof(STR_CMD_LC_SERVICE_FLOWER_P);
		 }

		 if (wPacketSize>0) g_GameCtrl->SendData(dwSocketID, MDM_SERVICE, CMD_LC_SERVICE_FLOWER_QUERY, cbBuffer, wPacketSize);
		 g_GameCtrl->SendData(dwSocketID, MDM_SERVICE, CMD_LC_SERVICE_FLOWER_QUERY_FINISH, NULL, 0);
	 }
	 else
	 {
		 g_GameCtrl->SendDataMsg(dwSocketID, "��ѯʧ��");
	 }
}
//�鿴�ҵķ�˿
bool CHandleFromGate::On_SUB_CL_SERVICE_FLOWING_QUERY(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
		 CPlayer * player = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	 if(player == NULL) return false;

	 g_AccountsDB->ResetParameter();
	 g_AccountsDB->AddParameter(TEXT("@dwUserID"), player->GetUserID()); 

	 //ִ�в�ѯ
	 LONG lResultCode = g_AccountsDB->ExecuteProcess(TEXT("GSP_CL_FLOWING_QUERY"),true);
	 if(lResultCode == DB_SUCCESS)
	 {
		 WORD wPacketSize=0;
		 BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
		 STR_CMD_LC_SERVICE_FLOWER_P * pCMD=NULL;
		 while(g_AccountsDB->IsRecordsetEnd() == false)
		 {
			 if ((wPacketSize+sizeof(STR_CMD_LC_SERVICE_FLOWER_P))>sizeof(cbBuffer))
			 {
				 g_GameCtrl->SendData(dwSocketID, MDM_SERVICE, CMD_LC_SERVICE_FLOWING_QUERY, cbBuffer, wPacketSize);
				 wPacketSize=0;
			 }

			 STR_CMD_LC_SERVICE_FLOWER_P cmd_p;
			 cmd_p.dwUserID = g_AccountsDB->GetValue_DWORD(TEXT("UserID"));
			 g_AccountsDB->GetValue_String(TEXT("NickName"),cmd_p.szNickName,CountArray(cmd_p.szNickName));
			 cmd_p.cbGender = g_AccountsDB->GetValue_BYTE(TEXT("Gender"));
			 g_AccountsDB->GetValue_String(TEXT("HeadUrl"),cmd_p.szHeadUrl,CountArray(cmd_p.szHeadUrl));
			 cmd_p.dwLoveValue = g_AccountsDB->GetValue_DWORD(TEXT("LoveValue"));

			 pCMD=(STR_CMD_LC_SERVICE_FLOWER_P *)(cbBuffer+wPacketSize);
			 memcpy(pCMD, &cmd_p, sizeof(cmd_p));

			 wPacketSize+=sizeof(STR_CMD_LC_SERVICE_FLOWER_P);
		 }

		 if (wPacketSize>0) g_GameCtrl->SendData(dwSocketID, MDM_SERVICE, CMD_LC_SERVICE_FLOWING_QUERY, cbBuffer, wPacketSize);
		 g_GameCtrl->SendData(dwSocketID, MDM_SERVICE, CMD_LC_SERVICE_FLOWING_QUERY_FINISH, NULL, 0);
	 }
	 else
	 {
		 g_GameCtrl->SendDataMsg(dwSocketID, "��ѯʧ��");
	 }
}
//��ע,ȡ����ע
bool CHandleFromGate::On_SUB_CL_SERVICE_FLOWER_ACT(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	if(sizeof(STR_SUB_CL_SERVICE_FLOWER_ACT) != wDataSize) return false;
	STR_SUB_CL_SERVICE_FLOWER_ACT* sub = (STR_SUB_CL_SERVICE_FLOWER_ACT*) pData;

	CPlayer * player = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if(player == NULL) return false;

	if(player->GetUserID() == sub->dwTargetID) return true;

	g_AccountsDB->ResetParameter();
	g_AccountsDB->AddParameter(TEXT("@dwTargetID"), sub->dwTargetID); 
	g_AccountsDB->AddParameter(TEXT("@dwMyID"), player->GetUserID()); 
	g_AccountsDB->AddParameter(TEXT("@cbMask"), sub->cbMask); 

	//ִ�в�ѯ
	LONG lResultCode = g_AccountsDB->ExecuteProcess(TEXT("GSP_CL_FLOWER_ACT"),true);

	STR_CMD_LC_SERVICE_FLOWER_ACT cmd;
	cmd.cbResult = lResultCode;
	cmd.cbMask = sub->cbMask;
	g_GameCtrl->SendData(dwSocketID, MDM_SERVICE, CMD_LC_SERVICE_FLOWER_ACT, &cmd, sizeof(cmd));
}

//ˢ���û���Ϣ
bool CHandleFromGate::On_SUB_CL_Service_RefreshUserInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//�������
	STR_DBR_CL_SERCIVR_REFRESH_INFO UserRequest;
	ZeroMemory(&UserRequest,sizeof(UserRequest));

	CPlayer *player = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if(player)
	{
		UserRequest.dwUserID = player->GetUserID();
		g_GameCtrl->PostDataBaseRequest(DBR_CL_SERVICE_REFRESH_USER_INFO, dwSocketID, &UserRequest, sizeof(UserRequest));
	}
	return true;
}

//ˢ���û���Ϣ����
bool CHandleFromGate::On_CMD_LC_Service_RefreshUserInfo( DWORD dwScoketID, VOID * pData )
{
	//��������
	STR_CMD_LC_SERVICE_REFRESH_INFO UserInfo;
	ZeroMemory(&UserInfo,sizeof(UserInfo));

	//��������
	STR_DBO_CL_SERCIVR_REFRESH_INFO * pUserInfo=(STR_DBO_CL_SERCIVR_REFRESH_INFO *)pData;

	//��������
	memcpy_s(&UserInfo, sizeof(STR_CMD_LC_SERVICE_REFRESH_INFO), pUserInfo, sizeof(STR_CMD_LC_SERVICE_REFRESH_INFO));

	//��������
	WORD wDataSize = sizeof(STR_CMD_LC_SERVICE_REFRESH_INFO);
	g_GameCtrl->SendData(dwScoketID, MDM_SERVICE, CMD_CL_SERVICE_REFRESH_USER_INFO, &UserInfo, wDataSize);

	return true;
}

//��ȡ�û�¼���б�
bool CHandleFromGate::On_SUB_CL_Service_GetUserRecordList(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	ASSERT( wDataSize == sizeof(STR_SUB_CL_SERVICE_GET_DJ_RECORDLIST));
	if(wDataSize != sizeof(STR_SUB_CL_SERVICE_GET_DJ_RECORDLIST)) return false;

	STR_SUB_CL_SERVICE_GET_DJ_RECORDLIST* pGetRecordList = (STR_SUB_CL_SERVICE_GET_DJ_RECORDLIST*)pData;

	//¼���б�����
	STR_DBR_CL_SERCIVR_GET_RECORDLIST GetRecordList;
	GetRecordList.dwUserID = pGetRecordList->dwUserID;

	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_CL_SERVICE_GET_USER_RECORD_LIST, dwSocketID, &GetRecordList, sizeof(STR_DBR_CL_SERCIVR_GET_RECORDLIST));

	return true;
}

//��ȡ�û�¼���б���
bool CHandleFromGate::On_CMD_LC_Service_GetUserRecordList(DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT(wDataSize==sizeof(STR_DBO_CL_SERCIVR_GET_RECORDLIST));
	STR_DBO_CL_SERCIVR_GET_RECORDLIST *pRecord = (STR_DBO_CL_SERCIVR_GET_RECORDLIST*)pData;

	STR_CMD_LC_SERVICE_GET_DJ_RECORDLIST Record;
	ZeroMemory(&Record,sizeof(STR_CMD_LC_SERVICE_GET_DJ_RECORDLIST));

	Record.dwTableID = pRecord->dwTableID;
	memcpy( Record.dwUserID, pRecord->dwUserID,sizeof(DWORD)*5);

	for(int i=0;i < 5;i++)
	{
		memcpy(Record.szUserName[i],pRecord->szUserName[i],sizeof(TCHAR)*LEN_NICKNAME);	
	}

	Record.byGameMode = pRecord->byGameMode;
	Record.byZhuangType = pRecord->byZhuangType;
	Record.wCurrentJuShu = pRecord->wCurrentJuShu;
	Record.wTotalJuShu = pRecord->wTotalJuShu;

	memcpy(Record.szGameTime, pRecord->szGameTime, sizeof(TCHAR)*30);
	memcpy(Record.szTotalScore, pRecord->szTotalScore, sizeof(TCHAR)*50);

	g_GameCtrl->SendData(dwScoketID, MDM_SERVICE, CMD_LC_SERVICE_GET_DJ_RECORD_LIST, &Record, sizeof(STR_CMD_LC_SERVICE_GET_DJ_RECORDLIST));

	//_bstr_t b(pRecord->szData);
	//char* output = b;

	//FILE *fp = fopen(output,"rb");
	//if(NULL != fp)
	//{
	//	if(pRecord->dwDataSize <= 3072)
	//	{
	//		fread(Record.szData,pRecord->dwDataSize,1,fp);

	//		memcpy(Record.szGameTime,pRecord->szGameTime,sizeof(TCHAR)*30);
	//		memcpy(Record.szScore,pRecord->szScore,sizeof(TCHAR)*50);
	//		memcpy(Record.szTotalScore,pRecord->szTotalScore,sizeof(TCHAR)*50);

	//		g_GameCtrl->SendData(dwScoketID, MDM_SERVICE, CMD_LC_SERVICE_GET_DJ_RECORD_LIST, &Record, sizeof(STR_CMD_LC_SERVICE_GET_DJ_RECORDLIST));

	//	}
	//	fclose(fp);
	//}

	return true;
}

//��ȡָ��ID¼��
bool CHandleFromGate::On_SUB_CL_Service_GetSpecificRecord(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	ASSERT( wDataSize == sizeof(STR_SUB_CL_SERVICE_GET_XJ_RECORDLIST));
	if(wDataSize != sizeof(STR_SUB_CL_SERVICE_GET_XJ_RECORDLIST)) return false;

	STR_SUB_CL_SERVICE_GET_XJ_RECORDLIST* pGetRecordList = (STR_SUB_CL_SERVICE_GET_XJ_RECORDLIST*)pData;

	//DBR����
	STR_DBR_CL_SERCIVR_GET_SPECIFIC_RECORD GetRecordList;
	GetRecordList.dwTableID = pGetRecordList->dwTableID;

	//У������ID
	if ( 0 != GetRecordList.dwTableID )
	{
		//Ͷ������
		g_GameCtrl->PostDataBaseRequest(DBR_CL_SERVICE_GET_SPECIFIC_RECORD,dwSocketID,&GetRecordList,sizeof(STR_DBR_CL_SERCIVR_GET_SPECIFIC_RECORD));
	}

	return true;
}

//��ȡָ��ID¼�񷵻�
bool CHandleFromGate::On_CMD_LC_Service_GetSpecificRecord(DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT(wDataSize==sizeof(STR_DBO_CL_SERCIVR_GET_SPECIFIC_RECORD));
	STR_DBO_CL_SERCIVR_GET_SPECIFIC_RECORD *pRecord = (STR_DBO_CL_SERCIVR_GET_SPECIFIC_RECORD*)pData;

	STR_CMD_LC_SERVICE_GET_XJ_RECORDLIST Record;
	memcpy(&Record, pRecord, sizeof(STR_CMD_LC_SERVICE_GET_XJ_RECORDLIST));

	g_GameCtrl->SendData(dwScoketID, MDM_SERVICE, CMD_LC_SERVICE_GET_XJ_RECORD_LIST, &Record, sizeof(STR_CMD_LC_SERVICE_GET_XJ_RECORDLIST));

	return true;
}

//��ȡ���߽���
bool CHandleFromGate::On_SUB_CL_Service_OnlineReward(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	ASSERT(wDataSize == sizeof(STR_SUB_CL_SERVICE_ONLINE_REWARD));
	if(wDataSize != sizeof(STR_SUB_CL_SERVICE_ONLINE_REWARD))
		return false;

	STR_SUB_CL_SERVICE_ONLINE_REWARD * pGetLogonReward = (STR_SUB_CL_SERVICE_ONLINE_REWARD*)pData;

	STR_DBR_CL_SERCIVR_ONLINE_REWARD GetLogonReward;
	ZeroMemory(&GetLogonReward,sizeof(STR_DBR_CL_SERCIVR_ONLINE_REWARD));

	GetLogonReward.dwUserID = pGetLogonReward->dwUserID;

	return g_GameCtrl->PostDataBaseRequest(DBR_CL_SERVICE_ONLINE_REWARD,dwSocketID,&GetLogonReward,sizeof(GetLogonReward));
}

//��ȡ���߽�������
bool CHandleFromGate::On_CMD_LC_Service_OnlineReward( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//����У��
	ASSERT(wDataSize==sizeof(STR_DBO_CL_SERCIVR_ONLINE_REWARD));
	if(wDataSize!=sizeof(STR_DBO_CL_SERCIVR_ONLINE_REWARD))
		return false;

	STR_DBO_CL_SERCIVR_ONLINE_REWARD *pLotteryResult = (STR_DBO_CL_SERCIVR_ONLINE_REWARD*)pData;

	STR_CMD_LC_SERVICE_ONLINE_REWARD LotteryResult;
	ZeroMemory(&LotteryResult,sizeof(STR_CMD_LC_SERVICE_ONLINE_REWARD));

	LotteryResult.byType = pLotteryResult->byType;
	LotteryResult.dwCount = pLotteryResult->dwCount;
	
	g_GameCtrl->SendData(dwScoketID,MDM_SERVICE, CMD_LC_SERVICE_ONLINE_REWARD, &LotteryResult, sizeof(STR_CMD_LC_SERVICE_ONLINE_REWARD));

	return true;
}

//��ȡ�����б�
bool CHandleFromGate::On_SUB_CL_Service_GetTaskList(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	ASSERT(wDataSize >= sizeof(STR_SUB_CL_SERVICE_GET_TASKLIST));
	if (wDataSize<sizeof(STR_SUB_CL_SERVICE_GET_TASKLIST)) return false;

	//������Ϣ
	STR_SUB_CL_SERVICE_GET_TASKLIST * pGetTaskList = (STR_SUB_CL_SERVICE_GET_TASKLIST *)pData;

	//��������
	STR_DBR_CL_SERCIVR_GET_TASKLIST GetTaskList;
	ZeroMemory(&GetTaskList,sizeof(GetTaskList));

	//��������
	memcpy(&GetTaskList,pGetTaskList,sizeof(GetTaskList));

	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_CL_SERVICE_GET_TASK_LIST,dwSocketID,&GetTaskList,sizeof(GetTaskList));

	return true;
}

//��ȡ�����б���
bool CHandleFromGate::On_CMD_LC_Service_GetTaskList( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//����У��
	if( 0 != (wDataSize % sizeof(STR_DBO_CL_SERCIVR_GET_TASKLIST)) )
		return false;

	//����������
	int count = wDataSize / sizeof(STR_DBO_CL_SERCIVR_GET_TASKLIST);

	//���ݿ������б�
	STR_DBO_CL_SERCIVR_GET_TASKLIST *TaskList = (STR_DBO_CL_SERCIVR_GET_TASKLIST *)pData;

	//���ص������б�
	STR_CMD_LC_SERVICE_GET_TASKLIST *pCMDTaskList = new STR_CMD_LC_SERVICE_GET_TASKLIST[count];

	//��������
	WORD wSendSize=0;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];

	//ö������
	for (int i=0; i<count; i++)
	{
		//��DBO���ݸ������ص�����,��ɱ�־������ֵ
		memcpy_s(&pCMDTaskList[i], sizeof(STR_DBO_CL_SERCIVR_GET_TASKLIST), &TaskList[i], sizeof(STR_DBO_CL_SERCIVR_GET_TASKLIST));

		//���һ�η���,������ɱ�־Ϊ1
		if ( i == (count-1) )		
		{
			pCMDTaskList[i].cbListFinishMask = 1;
		}
		else
		{
			pCMDTaskList[i].cbListFinishMask = 0;
		}
		
		//��������(��ֹ����̫�࣬һ��������)
		if ( (wSendSize+sizeof(STR_CMD_LC_SERVICE_GET_TASKLIST)) > sizeof(cbDataBuffer) )
		{
			//��������
			g_GameCtrl->SendData(dwScoketID, MDM_SERVICE, CMD_LC_SERVICE_GET_TASK_LIST, cbDataBuffer, wSendSize);			
			wSendSize=0;
		}

		//��������
		CopyMemory(cbDataBuffer+wSendSize, &pCMDTaskList[i], sizeof(STR_CMD_LC_SERVICE_GET_TASKLIST));
		wSendSize += sizeof(STR_CMD_LC_SERVICE_GET_TASKLIST);
	}

	//����ʣ��
	if (wSendSize>0) 	
	{
		g_GameCtrl->SendData(dwScoketID, MDM_SERVICE, CMD_LC_SERVICE_GET_TASK_LIST, cbDataBuffer, wSendSize);
	}

	//�ͷ��ڴ�		TODO ���������
	delete [] pData;		//ǰ������ʱnew�����ģ�������Ҫdelete������֮ǰ���ڴ洫���
	delete [] pCMDTaskList;

	return true;
}

//��ȡ������
bool CHandleFromGate::On_SUB_CL_Service_GetTaskReward(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	ASSERT(wDataSize >= sizeof(STR_SUB_CL_SERVICE_GET_TASK_REWARD));
	if (wDataSize<sizeof(STR_SUB_CL_SERVICE_GET_TASK_REWARD)) return false;

	//������Ϣ
	STR_SUB_CL_SERVICE_GET_TASK_REWARD * pSetTaskDone =(STR_SUB_CL_SERVICE_GET_TASK_REWARD *)pData;
	//��������
	STR_DBR_CL_SERCIVR_GET_TASK_REWARD SetTaskDone;
	ZeroMemory(&SetTaskDone,sizeof(SetTaskDone));

	//��������
	memcpy(&SetTaskDone,pSetTaskDone,sizeof(SetTaskDone));

	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_CL_SERVICE_GET_TASK_REWARD,dwSocketID,&SetTaskDone,sizeof(SetTaskDone));

	return true;
}

//��ȡ����������
bool CHandleFromGate::On_CMD_LC_Service_GetTaskReward( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//����У��
	ASSERT(wDataSize==sizeof(STR_DBO_CL_SERCIVR_GET_TASK_REWARD));
	if(wDataSize!=sizeof(STR_DBO_CL_SERCIVR_GET_TASK_REWARD))
		return false;

	STR_DBO_CL_SERCIVR_GET_TASK_REWARD *pTaskDone = (STR_DBO_CL_SERCIVR_GET_TASK_REWARD*)pData;

	STR_CMD_LC_SERVICE_GET_TASK_REWARD TaskDone;
	ZeroMemory(&TaskDone,sizeof(STR_CMD_LC_SERVICE_GET_TASK_REWARD));
	memcpy(&TaskDone,pTaskDone,sizeof(STR_CMD_LC_SERVICE_GET_TASK_REWARD));

	g_GameCtrl->SendData(dwScoketID, MDM_SERVICE, CMD_LC_SERVICE_GET_TASK_REWARD, &TaskDone, sizeof(STR_CMD_LC_SERVICE_GET_TASK_REWARD));

	return true;
}

//��ȡ���а�
bool CHandleFromGate::On_SUB_CL_Service_GetRankList(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//����У��
	ASSERT(wDataSize==sizeof(STR_SUB_CL_SERVICE_GET_RANKLIST));
	if(wDataSize!=sizeof(STR_SUB_CL_SERVICE_GET_RANKLIST))
		return false;

	STR_SUB_CL_SERVICE_GET_RANKLIST* pTaskList = (STR_SUB_CL_SERVICE_GET_RANKLIST*)pData;

	STR_CMD_LC_SERVICE_GET_RANKLIST rankList;
	ZeroMemory(&rankList, sizeof(STR_CMD_LC_SERVICE_GET_RANKLIST));

	//���а��������ȡ���а��б�
	int wCount = RankManager::Instance()->GetRankCountByIndex(pTaskList->byIndex);

	for(int i=0;i < 20 && i < wCount;i++)
	{
		tagRankInfo* item = RankManager::Instance()->GetRankItemByIndex(pTaskList->byIndex,i);
		if(item != NULL)
		{
			g_GameCtrl->SendData(dwSocketID,MDM_SERVICE, CMD_LC_SERVICE_GET_RANK_LIST, item, sizeof(STR_CMD_LC_SERVICE_GET_RANKLIST));
		}
	}

	return true;
}

//����齱
bool CHandleFromGate::On_SUB_CL_Service_RequestLottery(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//����У��
	ASSERT(wDataSize==sizeof(STR_SUB_CL_SERVICE_REQUEST_LOTTERY));
	if(wDataSize!=sizeof(STR_SUB_CL_SERVICE_REQUEST_LOTTERY))
		return false;

	STR_SUB_CL_SERVICE_REQUEST_LOTTERY* pGetRankReward = (STR_SUB_CL_SERVICE_REQUEST_LOTTERY*)pData;

	STR_DBR_CL_SERCIVR_REQUEST_LOTTERY QueryLottery;
	memcpy(&QueryLottery,pGetRankReward,sizeof(STR_DBR_CL_SERCIVR_REQUEST_LOTTERY));

	return g_GameCtrl->PostDataBaseRequest(DBR_CL_SERVICE_REQUEST_LOTTERY,dwSocketID,&QueryLottery,sizeof(STR_DBR_CL_SERCIVR_REQUEST_LOTTERY));

}

//����齱����
bool CHandleFromGate::On_CMD_LC_Service_RequestLottery( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//����У��
	ASSERT(wDataSize==sizeof(STR_DBO_CL_SERCIVR_REQUEST_LOTTERY));
	if(wDataSize!=sizeof(STR_DBO_CL_SERCIVR_REQUEST_LOTTERY))
		return false;

	STR_DBO_CL_SERCIVR_REQUEST_LOTTERY *pLotteryResult = (STR_DBO_CL_SERCIVR_REQUEST_LOTTERY*)pData;

	//����齱����
	STR_CMD_LC_SERVICE_REQUEST_LOTTERY LotteryResult;
	ZeroMemory(&LotteryResult,sizeof(STR_CMD_LC_SERVICE_REQUEST_LOTTERY));

	//��ֵ
	LotteryResult.byIndex = pLotteryResult->byIndex;
	LotteryResult.lResultCode = pLotteryResult->lResultCode;
	lstrcpyn(LotteryResult.szDescribeString, pLotteryResult->szDescribeString, CountArray(LotteryResult.szDescribeString));

	//��������
	g_GameCtrl->SendData(dwScoketID,MDM_SERVICE, CMD_LC_SERVICE_REQUEST_LOTTERY, &LotteryResult, sizeof(STR_CMD_LC_SERVICE_REQUEST_LOTTERY));

	return true;
}

//pure�������а� ��ѯ
bool CHandleFromGate::On_SUB_CL_SERVICE_PURE_STANDING_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//����У��
	if(wDataSize!=sizeof(STR_SUB_CL_SERVICE_PURE_STANDING_LIST))
		return false;

	return g_GameCtrl->PostDataBaseRequest(DBR_CL_SERVICE_PURE_STANDING_LIST,dwSocketID,pData, wDataSize);
}
//pure�������а� ����
bool CHandleFromGate::On_CMD_LC_SERVICE_PURE_STANDING_LIST( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//����У��
	DWORD Count = sizeof(STR_CMD_LC_SERVICE_PURE_STANDING_LIST);
	if(wDataSize<Count || (wDataSize%Count != 0))
		return false;

	//��������
	g_GameCtrl->SendData(dwScoketID,MDM_SERVICE, CMD_LC_SERVICE_PURE_STANDING_LIST, pData, wDataSize);

	return true;
}
//pure�������а� ����
bool CHandleFromGate::On_CMD_LC_SERVICE_PURE_STANDING_FINISH( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//����У��
	if(wDataSize!=sizeof(STR_CMD_LC_SERVICE_PURE_STANDING_FINISH))
		return false;

	STR_CMD_LC_SERVICE_PURE_STANDING_FINISH *pLotteryResult = (STR_CMD_LC_SERVICE_PURE_STANDING_FINISH*)pData;
	pLotteryResult->byMask = 1;

	//��������
	g_GameCtrl->SendData(dwScoketID,MDM_SERVICE, CMD_LC_SERVICE_PURE_STANDING_FINISH, pLotteryResult, sizeof(STR_CMD_LC_SERVICE_PURE_STANDING_FINISH));

	return true;
}

//pure���ս�� ��ѯ
bool CHandleFromGate::On_SUB_CL_SERVICE_PURE_RECORD_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//����У��
	if(wDataSize!=sizeof(STR_SUB_CL_SERVICE_PURE_RECORD_LIST))
		return false;

	g_GameCtrl->PostDataBaseRequest(DBR_CL_SERVICE_PURE_RECORD_LIST,dwSocketID,pData, wDataSize);

	return true;
}
//pure���ս�� ����
bool CHandleFromGate::On_CMD_LC_SERVICE_PURE_RECORD_LIST( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//����У��
	DWORD Count = sizeof(STR_CMD_LC_SERVICE_PURE_RECORD_LIST);
	if(wDataSize<Count || (wDataSize%Count != 0))
		return false;

	//��������
	g_GameCtrl->SendData(dwScoketID,MDM_SERVICE, CMD_LC_SERVICE_PURE_RECORD_LIST, pData, wDataSize);

	return true;
}
//pure��������Ϣ ����
bool CHandleFromGate::On_CMD_LC_SERVICE_PURE_RECORD_LIST_PINFO( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//����У��
	DWORD Count = sizeof(STR_CMD_LC_SERVICE_PURE_RECORD_LIST_PLAYERINFO);
	if(wDataSize<Count || (wDataSize%Count != 0))
		return false;

	//��������
	g_GameCtrl->SendData(dwScoketID,MDM_SERVICE, CMD_LC_SERVICE_PURE_RECORD_LIST_PINFO, pData, wDataSize);

	return true;
}
//pure���ս�� ����
bool CHandleFromGate::On_CMD_LC_SERVICE_PURE_RECORD_FINISH( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//����У��
	if(wDataSize!=sizeof(STR_CMD_LC_SERVICE_PURE_RECORD_LIST_FINIST))
		return false;

	STR_CMD_LC_SERVICE_PURE_RECORD_LIST_FINIST *pLotteryResult = (STR_CMD_LC_SERVICE_PURE_RECORD_LIST_FINIST*)pData;
	pLotteryResult->byMask = 1;

	//��������
	g_GameCtrl->SendData(dwScoketID,MDM_SERVICE, CMD_LC_SERVICE_PURE_RECORD_FINISH, pLotteryResult, sizeof(STR_CMD_LC_SERVICE_PURE_RECORD_LIST_FINIST));

	return true;
}

//pureС��ս�� ��ѯ
bool CHandleFromGate::On_SUB_CL_SERVICE_PURE_XJ_RECORD_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//����У��
	if(wDataSize!=sizeof(STR_SUB_CL_SERVICE_PURE_XJ_RECORD_LIST))
		return false;

	return g_GameCtrl->PostDataBaseRequest(DBR_CL_SERVICE_PURE_XJ_RECORD_LIST,dwSocketID,pData, wDataSize);
}
//pureС��ս�� ����
bool CHandleFromGate::On_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//����У��
	DWORD Count = sizeof(STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST);
	if(wDataSize<Count || (wDataSize%Count != 0))
		return false;

	STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST * pCMD = (STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST *) pData;

	//��������
	g_GameCtrl->SendData(dwScoketID,MDM_SERVICE, CMD_LC_SERVICE_PURE_XJ_RECORD_LIST, pData, wDataSize);

	return true;
}
//pureС�������Ϣ ����
bool CHandleFromGate::On_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_PINFO( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//����У��
	DWORD Count = sizeof(STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_PLAYERINFO);
	if(wDataSize<Count || (wDataSize%Count != 0))
		return false;

	//��������
	g_GameCtrl->SendData(dwScoketID,MDM_SERVICE, CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_PINFO, pData, wDataSize);

	return true;
}
//pureС��ս�� ����
bool CHandleFromGate::On_CMD_LC_SERVICE_PURE_XJ_RECORD_FINISH( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//����У��
	if(wDataSize!=sizeof(STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_FINISH))
		return false;

	STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_FINISH *pLotteryResult = (STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_FINISH*)pData;
	pLotteryResult->byMask = 1;

	//��������
	g_GameCtrl->SendData(dwScoketID,MDM_SERVICE, CMD_LC_SERVICE_PURE_XJ_RECORD_FINISH, pLotteryResult, sizeof(STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_FINISH));

	return true;
}

//С��¼��ط�
bool CHandleFromGate::On_SUB_CL_Service_XJRecordPlayback(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//���ݴ�СУ��
	if ( wDataSize != sizeof(STR_SUB_CL_SERVICE_XJ_RECORD_PLAYBACK) )
		return false;

	//SUB����
	STR_SUB_CL_SERVICE_XJ_RECORD_PLAYBACK *SUB = (STR_SUB_CL_SERVICE_XJ_RECORD_PLAYBACK *)pData;
	
	//����DBR����
	STR_DBR_CL_SERVICE_XJ_RECORD_PLAYBACK DBR;
	ZeroMemory(&DBR, sizeof(STR_DBR_CL_SERVICE_XJ_RECORD_PLAYBACK));
	DBR.dwRecordID = SUB->dwRecordID;

	return g_GameCtrl->PostDataBaseRequest(DBR_CL_SERVICE_XJ_RECORD_PLAYBACK, dwSocketID, &DBR, wDataSize);
}
//С��¼��ط� ����
bool CHandleFromGate::On_CMD_LC_Service_XJRecordPlayback( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//����У��
	if(wDataSize != sizeof(STR_DBO_LC_SERVICE_XJ_RECORD_PLAYBACK))
		return false;

	//DBO����
	STR_DBO_LC_SERVICE_XJ_RECORD_PLAYBACK *pDBO = (STR_DBO_LC_SERVICE_XJ_RECORD_PLAYBACK*)pData;

	BYTE cbBuffer[LEN_MAX_RECORD_SIZE];	
	memcpy_s(cbBuffer, sizeof(cbBuffer), pDBO->cbRecordData, sizeof(cbBuffer));

	//��������
	for (int i = 0; i < 4; i++)
	{
		STR_CMD_LC_SERVICE_XJ_RECORD_PLAYBACK CMD;
		ZeroMemory(&CMD, sizeof(STR_CMD_LC_SERVICE_XJ_RECORD_PLAYBACK));
		memcpy_s(CMD.cbRecordData, sizeof(CMD.cbRecordData), cbBuffer+i*LEN_MAX_RECORD_SIZE/4, sizeof(CMD.cbRecordData));
		CMD.cbfinish = (i==3) ? 1 : 0;

		//��������
		g_GameCtrl->SendData(dwScoketID, MDM_SERVICE, CMD_LC_SERVICE_XJ_RECORD_PLAYBACK, &CMD, sizeof(CMD));

	}

	return true;
}


//�޸ĸ�������
bool CHandleFromGate::On_SUB_CL_Service_ModifyPersonalInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	if (wDataSize != sizeof(STR_SUB_CL_SERVICE_MODIFY_PERSONAL_INFO)) return false;
	STR_SUB_CL_SERVICE_MODIFY_PERSONAL_INFO* sub = (STR_SUB_CL_SERVICE_MODIFY_PERSONAL_INFO*)pData;

	CPlayer* player = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if(player == NULL) return false;

	//�������
	g_AccountsDB->ResetParameter();
	g_AccountsDB->AddParameter(TEXT("@UserID"), player->GetUserID());
	g_AccountsDB->AddParameter(TEXT("@NickName"), sub->szNickName);
	g_AccountsDB->AddParameter(TEXT("@Gender"), sub->cbGender);
	g_AccountsDB->AddParameter(TEXT("@HeadImageUrl"), sub->szHeadImageUrl);
	g_AccountsDB->AddParameter(TEXT("@Signature"), sub->szSignature);
	g_AccountsDB->AddParameter(TEXT("@RealName"), sub->szRealName);
	g_AccountsDB->AddParameter(TEXT("@IDCardNum"), sub->szIDCardNum);
	g_AccountsDB->AddParameter(TEXT("@PhoneNum"), sub->szPhoneNum);

	//ִ�в�ѯ
	LONG lResultCode = g_AccountsDB->ExecuteProcess(TEXT("GSP_CL_Modify_UserlInfo"), true);

	if(lResultCode == 0)
	{
		g_GameCtrl->SendDataMsg(dwSocketID, "�����ɹ�");
	}
	else
	{
		g_GameCtrl->SendDataMsg(dwSocketID, "����ʧ��");
	}

	return true;
}


//�����������
bool CHandleFromGate::On_CMD_LC_CommonOperateResult( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//��������
	STR_CMD_LC_OTHER_OPERATE_RESULT OperateResult;
	ZeroMemory(&OperateResult,sizeof(OperateResult));

	//��������
	DBO_GP_OperateCommand * pOperate=(DBO_GP_OperateCommand *)pData;

	//��������
	OperateResult.lResultCode=pOperate->lResultCode;

	//��������
	WORD wHeadSize=sizeof(OperateResult);
	g_GameCtrl->SendData(dwScoketID,pOperate->mCommand.MainCommand,pOperate->mCommand.SubCommand,&OperateResult,wHeadSize);
	return true;
}

//��ȡ���񷵻�
bool CHandleFromGate::OnDBRankRewardResult( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//����У��
	ASSERT(wDataSize==sizeof(STR_DBO_CL_SERCIVR_GET_RANK_REWARD));
	if(wDataSize!=sizeof(STR_DBO_CL_SERCIVR_GET_RANK_REWARD))
		return false;

	STR_DBO_CL_SERCIVR_GET_RANK_REWARD *pRankDone = (STR_DBO_CL_SERCIVR_GET_RANK_REWARD*)pData;

	STR_CMD_LC_SERVICE_GET_RANK_REWARD RankDone;
	ZeroMemory(&RankDone,sizeof(STR_CMD_LC_SERVICE_GET_RANK_REWARD));
	memcpy(&RankDone,pRankDone,sizeof(STR_CMD_LC_SERVICE_GET_RANK_REWARD));

	g_GameCtrl->SendData(dwScoketID,MDM_SERVICE,CMD_LC_SERVICE_GET_RANK_REWARD,&RankDone,sizeof(STR_CMD_LC_SERVICE_GET_RANK_REWARD));

	return true;
}
#pragma endregion

#pragma region MDM_CLUB ����Ȧ(�汾2)
//��ѯ����Ȧ�б�
bool CHandleFromGate::On_SUB_CL_CLUB_ALL_CLUB_INFO_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_ALL_CLUB_INFO_LIST)) return false;

	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_ALL_CLUB_INFO_LIST, dwSocketID, pData, wDataSize);

	return true;
}
//��ѯ����Ȧ�б���
bool CHandleFromGate::On_CMD_LC_CLUB_ALL_CLUB_INFO_LIST( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_ALL_CLUB_INFO_LIST);
	if( (wDataSize % Size) != 0) return false;

	//������Ϣ
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_ALL_CLUB_INFO_LIST, pData, wDataSize);
	return true;
}
//��ѯ����Ȧ�б����
bool CHandleFromGate::On_CMD_LC_CLUB_ALL_INFO_FINISH( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	STR_CMD_LC_CLUB_ALL_INFO_FINISH cmd;
	cmd.byMask = 1;

	//������Ϣ
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_ALL_INFO_FINISH, &cmd, sizeof(STR_CMD_LC_CLUB_ALL_INFO_FINISH));
	return true;
}

//��ѯָ������Ȧ�����б�
bool CHandleFromGate::On_SUB_CL_CLUB_ROOM_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_ROOM_LIST)) return false;

	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_ROOM_LIST, dwSocketID, pData, wDataSize);
	return true;
}
//��ѯָ������Ȧ�����б� ����
bool CHandleFromGate::On_CMD_LC_CLUB_ROOM_LIST( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_ROOM_LIST);
	if( (wDataSize % Size ) != 0) return false;

	//������Ϣ
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_ROOM_LIST, pData, wDataSize);
	return true;
}
//��ѯָ������Ȧ�����б� ����
bool CHandleFromGate::On_CMD_LC_CLUB_ROOM_LIST_FINISH( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	STR_CMD_LC_CLUB_ROOM_LIST_FINISH cmd;
	cmd.byMask = 1;

	//������Ϣ
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_ROOM_LIST_FINISH, &cmd, sizeof(STR_CMD_LC_CLUB_ROOM_LIST_FINISH));
	return true;
}

//��ѯδ��Ա, �������Ȧ(���9��)
bool CHandleFromGate::On_SUB_CL_CLUB_RANDOM_CLUB_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_RANDOM_CLUB_LIST)) return false;

	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_RANDOM_CLUB_LIST, dwSocketID, pData, wDataSize);
	return true;
}
//��ѯδ��Ա, �������Ȧ(���9��) ����
bool CHandleFromGate::On_CMD_LC_CLUB_RANDOM_CLUB_LIST( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_RANDOM_CLUB_LIST);
	if( (wDataSize % Size) != 0) return false;

	//������Ϣ
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_RANDOM_CLUB_LIST, pData, wDataSize);
	return true;
}
//��ѯδ��Ա, �������Ȧ(���9��) ����
bool CHandleFromGate::On_CMD_LC_CLUB_RANDOM_CLUB_LIST_FINISH( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	STR_CMD_LC_CLUB_RANDOM_CLUB_LIST_FINISH cmd;
	cmd.byMask = 1;

	//������Ϣ
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_RANDOM_CLUB_LIST_FINISH, &cmd, sizeof(STR_CMD_LC_CLUB_RANDOM_CLUB_LIST_FINISH));
	return true;
}

//�����������Ȧ
bool CHandleFromGate::On_SUB_CL_CLUB_JOIN_CLUB(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_JOIN_CLUB)) return false;

	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_JOIN_CLUB, dwSocketID, pData, wDataSize);
	return true;
}
//�����������Ȧ���� 
bool CHandleFromGate::On_CMD_LC_CLUB_JOIN_CLUB( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_JOIN_CLUB);
	if( wDataSize != Size) return false;

	//������Ϣ
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_JOIN_CLUB, pData, wDataSize);
	return true;
}
//�����������Ȧ�㲥
bool CHandleFromGate::On_CMD_LC_CLUB_JOIN_CLUB_BDCAST( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_SUB_CL_CLUB_JOIN_CLUB_BDCAST);
	if( wDataSize != Size) return false;

	//TODONOW ��ʱ��ÿ���˶����ͣ������Ϊ 1.�ض����ֲ���  2.�᳤�͹���Ա����
	g_GameCtrl->SendDataBatch(MDM_CLUB, CMD_LC_CLUB_JOIN_CLUB_BDCAST, pData, wDataSize);
	return true;
}
//�����������Ȧ ֪ͨ�ͻ���ʵʱˢ��
bool CHandleFromGate::On_CMD_LC_CLUB_JOIN_CLUB_RE( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_LIST_RE);
	if( wDataSize != Size) return false;

	//������Ϣ
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_LIST_RE, pData, wDataSize);
	return true;
}


//��ɢ����Ȧ
bool CHandleFromGate::On_SUB_CL_CLUB_DISS_CLUB(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_DISS_CLUB)) return false;

	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_DISS_CLUB, dwSocketID, pData, wDataSize);
	return true;
}
//��ɢ����Ȧ ���� 
bool CHandleFromGate::On_CMD_LC_CLUB_DISS_CLUB( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_DISS_CLUB);
	if( wDataSize != Size) return false;

	//������Ϣ
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_DISS_CLUB, pData, wDataSize);
	return true;
}

//��������Ȧ
bool CHandleFromGate::On_SUB_CL_CLUB_CREATE_CLUB(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_CREATE_CLUB)) return false;

	//������Ϣ
	STR_SUB_CL_CLUB_CREATE_CLUB * pSub = (STR_SUB_CL_CLUB_CREATE_CLUB *)pData;
	//�������
	STR_DBR_CL_CLUB_CREATE_CLUB Dbr;
	ZeroMemory(&Dbr,sizeof(Dbr));

	Dbr.dwUserID = pSub->dwUserID;
	lstrcpyn(Dbr.szClubName, pSub->szClubName, CountArray(Dbr.szClubName));
	lstrcpyn(Dbr.szNotice, pSub->szNotice, CountArray(Dbr.szNotice));
	lstrcpyn(Dbr.szMessag, pSub->szMessag, CountArray(Dbr.szMessag));
	Dbr.dwMajorKindID = pSub->dwMajorKindID;

	//������Ϣ
	for (WORD i=0; i<10; i++)
	{
		INT nLength=lstrlen(Dbr.szMinorKindID);
		_sntprintf(&Dbr.szMinorKindID[nLength],CountArray(Dbr.szMinorKindID)-nLength,
			TEXT("%ld,"),
			pSub->dwMinorKindID[i]);
	}

	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_CREATE_CLUB, dwSocketID, &Dbr, sizeof(Dbr));
	return true;
}
//��������Ȧ ����
bool CHandleFromGate::On_CMD_LC_CLUB_CREATE_CLUB( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_CREATE_CLUB);
	if( wDataSize != Size ) return false;

	//������Ϣ
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_CREATE_CLUB, pData, wDataSize);
	return true;
}

//��������
bool CHandleFromGate::On_SUB_CL_CLUB_ROOM_SETTING(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_ROOM_SETTING)) return false;

	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_ROOM_SETTING, dwSocketID, pData, wDataSize);
	return true;
}
//�������� ����
bool CHandleFromGate::On_CMD_LC_CLUB_ROOM_SETTING( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_ROOM_SETTING);
	if( wDataSize != Size) return false;

	//������Ϣ
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_ROOM_SETTING, pData, wDataSize);
	return true;
}

//���󷿼�����
bool CHandleFromGate::On_SUB_CL_CLUB_ROOM_QUERY_SETTING(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_ROOM_QUERY_SETTING)) return false;

	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_ROOM_QUERY_SETTING, dwSocketID, pData, wDataSize);
	return true;
}
//���󷿼����� ����
bool CHandleFromGate::On_CMD_LC_CLUB_ROOM_QUERY_SETTING( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_ROOM_QUERY_SETTING);
	if( wDataSize != Size) return false;

	//������Ϣ
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_ROOM_QUERY_SETTING, pData, wDataSize);
	return true;
}


//����뿪���ֲ�����
bool CHandleFromGate::On_SUB_CL_CLUB_ROOM_USER_LEAVE(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_ROOM_USER_LEAVE)) return false;

	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_ROOM_USER_LEAVE, dwSocketID, pData, wDataSize);
	return true;
}

//��ɢ��������
bool CHandleFromGate::On_SUB_CL_CLUB_ROOM_DISSOLVE(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_ROOM_DISSOLVE)) return false;

	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_ROOM_DISSOLVE, dwSocketID, pData, wDataSize);
	return true;
}
//��ɢ�������� ����
bool CHandleFromGate::On_CMD_LC_CLUB_ROOM_DISSOLVE( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_ROOM_DISSOLVE);
	if( wDataSize != Size) return false;

	//������Ϣ
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_ROOM_DISSOLVE, pData, wDataSize);
	return true;
}

//��ɢ��������
bool CHandleFromGate::On_SUB_CL_CLUB_TABLE_DISSOLVE(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_TABLE_DISSOLVE)) return false;

	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_TABLE_DISSOLVE, dwSocketID, pData, wDataSize);
	return true;
}
//��ɢ�������� ����
bool CHandleFromGate::On_CMD_LC_CLUB_TABLE_DISSOLVE( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_TABLE_DISSOLVE);
	if( wDataSize != Size) return false;

	STR_CMD_LC_CLUB_TABLE_DISSOLVE* pCmd = (STR_CMD_LC_CLUB_TABLE_DISSOLVE *) pData;

	//������Ϣ
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_TABLE_DISSOLVE, pData, wDataSize);

	if( (DB_SUCCESS ==  pCmd->lResultCode) && (0 == pCmd->byMask) && (0 != pCmd->dwGameID))
	{
		//֪ͨЭ����, Э����֪ͨ��Ϸ��
		STR_CPR_LP_CLUB_TABLE_DISSOLVE CPR;
		CPR.dwGameID = pCmd->dwGameID;
		CPR.dwTableID = pCmd->dwTableID;

		g_TCPSocketEngine->SendData(CPD_MDM_TRANSFER, CPR_LP_CLUB_TABLE_DISSOLVE, &CPR, sizeof(STR_CPR_LP_CLUB_TABLE_DISSOLVE));
	}
	return true;
}


//����Ȧ����
bool CHandleFromGate::On_SUB_CL_CLUB_NOTICE(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_NOTICE)) return false;

	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_NOTICE, dwSocketID, pData, wDataSize);
	return true;
}
//����Ȧ���� ����
bool CHandleFromGate::On_CMD_LC_CLUB_NOTICE( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_NOTICE);
	if( wDataSize != Size) return false;

	//������Ϣ
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_NOTICE, pData, wDataSize);
	return true;
}
//����Ȧ���� �㲥
bool CHandleFromGate::On_CMD_LC_CLUB_NOTICE_BDCAST( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//TODONOW ��Ҫʵ��
	return true;
}

//����
bool CHandleFromGate::On_SUB_CL_CLUB_CHAT(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_CHAT)) return false;
	STR_SUB_CL_CLUB_CHAT *pSub = (STR_SUB_CL_CLUB_CHAT*)pData;

	if(0 == pSub->byChatMode) //���ֲ�����
	{
		g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_CHAT, dwSocketID, pData, wDataSize);
	}
	else if( 1 == pSub->byChatMode) //��������
	{
		g_GameCtrl->PostDataBaseRequest(DBR_CL_WORD_CHAT, dwSocketID, pData, wDataSize);
	}
	else if( 2 == pSub->byChatMode) //ϵͳ����
	{
		g_GameCtrl->PostDataBaseRequest(DBR_CL_SYSTEM_CHAT, dwSocketID, pData, wDataSize);
	}
	else if( 3 == pSub->byChatMode) //˽��
	{
		g_GameCtrl->PostDataBaseRequest(DBR_CL_SECRET_CHAT, dwSocketID, pData, wDataSize);
	}

	return true;
}
//���� ����
bool CHandleFromGate::On_CMD_LC_CLUB_CHAT( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_CHAT);
	if( wDataSize != Size) return false;

	//������Ϣ
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_CHAT, pData, wDataSize);
	return true;
}
//����Ȧ���� �㲥
bool CHandleFromGate::On_CMD_LC_CLUB_CHAT_BDCAST( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	WORD Size = sizeof(STR_CMD_LC_CLUB_CHAT_BDCAST);
	if( (wDataSize < Size) || (( wDataSize % Size) != 0 )) return false;
	STR_CMD_LC_CLUB_CHAT_BDCAST *pCmd = (STR_CMD_LC_CLUB_CHAT_BDCAST*) pData;

	switch(pCmd->byChatMode)
	{
	case 0: //���ֲ�����
		{
			auto vec_player = CPlayerManager::FindPlayerByClubID(pCmd->dwClubID);

			for(CPlayer* player : vec_player)
			{
				if(! player) continue;

				g_GameCtrl->SendData(player->GetSocketID(), MDM_CLUB, CMD_LC_CLUB_CHAT_BDCAST, 
					pCmd, sizeof(STR_CMD_LC_CLUB_CHAT_BDCAST));
			}

			break;
		}
	case 1: //��������
		{
			g_GameCtrl->SendDataBatch(MDM_CLUB, CMD_LC_CLUB_CHAT_BDCAST, pData, wDataSize);
			break;
		}
	case 2: //ϵͳ����
		{
			break;
		}
	case 3: //˽������
		{
			auto player = CPlayerManager::FindPlayerByID(pCmd->dwTagID);
			if(!player) return false;

			g_GameCtrl->SendData(player->GetSocketID(), MDM_CLUB, CMD_LC_CLUB_CHAT_BDCAST, 
						pData, wDataSize);

			break;
		}
	}
	
	return true;
}

//����Ȧ�ö�
bool CHandleFromGate::On_SUB_CL_CLUBSTICKY_POST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_STICKY_POST)) return false;
	STR_SUB_CL_CLUB_STICKY_POST *pSub = (STR_SUB_CL_CLUB_STICKY_POST*)pData;

	//����DBR����
	STR_DBR_CL_CLUB_STICKY_POST DBR;
	ZeroMemory(&DBR, sizeof(STR_DBR_CL_CLUB_STICKY_POST));
	DBR.dwClubID = pSub->dwClubID;
	DBR.cbTopFlag = pSub->cbTopFlag;
	DBR.dwUserID = pSub->dwUserID;

	//��������
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_STICKY_POST, dwSocketID, pData, wDataSize);

	return true;
}
//����Ȧ�ö� ����
bool CHandleFromGate::On_CMD_LC_CLUB_STICKY_POST( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//У�����
	if( wDataSize != sizeof(STR_CMD_LC_CLUB_STICKY_POST)) return false;

	//����CMD����
	STR_DBO_LC_CLUB_STICKY_POST *DBO = (STR_DBO_LC_CLUB_STICKY_POST *)pData;

	STR_CMD_LC_CLUB_STICKY_POST CMD;
	ZeroMemory(&CMD, sizeof(STR_CMD_LC_CLUB_STICKY_POST));
	CMD.lResultCode = DBO->lResultCode;

	//���ͷ���
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_STICKY_POST, &CMD, sizeof(CMD));

	return true;
}

//����Ȧ���
bool CHandleFromGate::On_SUB_CL_CLUB_MESSAGE(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_MESSAGE)) return false;

	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_MESSAGE, dwSocketID, pData, wDataSize);
	return true;
}
//����Ȧ��� ����
bool CHandleFromGate::On_CMD_LC_CLUB_MESSAGE( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_MESSAGE);
	if( wDataSize != Size) return false;

	//������Ϣ
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_MESSAGE, pData, wDataSize);
	return true;
}

//���׷���
bool CHandleFromGate::On_SUB_CL_CLUB_CONTRIBUTE_FK(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_CONTRIBUTE_FK)) return false;

	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_CONTRIBUTE_FK, dwSocketID, pData, wDataSize);
	return true;
}
//���׷��� ����
bool CHandleFromGate::On_CMD_LC_CLUB_CONTRIBUTE_FK( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_CONTRIBUTE_FK);
	if( wDataSize != Size) return false;

	//������Ϣ
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_CONTRIBUTE_FK, pData, wDataSize);
	return true;
}

//����Ȧ����
bool CHandleFromGate::On_SUB_CL_CLUB_AUTO_AGREE(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_AUTO_AGREE)) return false;

	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_AUTO_AGREE, dwSocketID, pData, wDataSize);
	return true;
}
//����Ȧ���� ����
bool CHandleFromGate::On_CMD_LC_CLUB_AUTO_AGREE( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_AUTO_AGREE);
	if( wDataSize != Size) return false;

	//������Ϣ
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_AUTO_AGREE, pData, wDataSize);
	return true;
}

//������뷿��
bool CHandleFromGate::On_SUB_CL_CLUB_JOIN_ROOM(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_JOIN_ROOM)) return false;

	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_JOIN_ROOM, dwSocketID, pData, wDataSize);
	return true;
}
//������뷿�� ����
bool CHandleFromGate::On_CMD_LC_CLUB_JOIN_ROOM( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_JOIN_ROOM);
	if( wDataSize != Size) return false;

	//������Ϣ
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_JOIN_ROOM, pData, wDataSize);
	return true;
}
//�����б�
bool CHandleFromGate::On_CMD_LC_CLUB_TABLE_LIST_TABLE( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_TABLE_LIST);
	if( (wDataSize % Size) != 0) return false;

	//������Ϣ
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_TABLE_LIST_TABLE, pData, wDataSize);
	return true;
}
//��������б�
bool CHandleFromGate::On_CMD_LC_CLUB_TABLE_LIST_USER( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_TABLE_USER_LIST);
	if( (wDataSize % Size) != 0) return false;

	//������Ϣ
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_TABLE_LIST_USER, pData, wDataSize);
	return true;
}



//Ⱥ��|�����������Ϣ�Ĵ�(ͬ��|�ܾ�)
bool CHandleFromGate::On_SUB_CL_CLUB_APPLICANT_RESULT(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_APPLICANT_RESULT)) return false;

	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_APPLICANT_RESULT, dwSocketID, pData, wDataSize);
	return true;
}
//Ⱥ��|�����������Ϣ�Ĵ�(ͬ��|�ܾ�) ����
bool CHandleFromGate::On_CMD_LC_CLUB_APPLICANT_RESULT( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_APPLICANT_RESULT);
	if( wDataSize != Size) return false;

	//������Ϣ
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_APPLICANT_RESULT, pData, wDataSize);
	return true;
}

//�����Ա����
bool CHandleFromGate::On_SUB_CL_CLUB_MEMBER_MANAGER(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_MEMBER_MANAGER)) return false;

	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_MEMBER_MANAGER, dwSocketID, pData, wDataSize);
	return true;
}
//�����Ա���� ����
bool CHandleFromGate::On_CMD_LC_CLUB_MEMBER_MANAGER( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_MEMBER_MANAGER);
	if( (wDataSize % Size) != 0) return false;

	//������Ϣ
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_MEMBER_MANAGER, pData, wDataSize);
	return true;
}
//�����Ա���� ����
bool CHandleFromGate::On_CMD_LC_CLUB_MEMBER_MANAGER_FINISH( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	STR_CMD_LC_CLUB_MEMBER_MANAGER_FINISH cmd;
	cmd.byMask = 1;

	//������Ϣ
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_MEMBER_MANAGER_FINISH, &cmd, sizeof(cmd));
	return true;
}
//���������Ϣ
bool CHandleFromGate::On_CMD_LC_CLUB_DATA( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_DATA);
	if( wDataSize != Size) return false;

	//������Ϣ
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_DATA, pData, wDataSize);
	return true;
}

//���������������Ȧ
bool CHandleFromGate::On_SUB_CL_CLUB_INVITE(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_INVITE)) return false;

	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_INVITE, dwSocketID, pData, wDataSize);

	return true;
}
//���������������Ȧ ����
bool CHandleFromGate::On_CMD_LC_CLUB_INVITE( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_INVITE);
	if( wDataSize != Size) return false;

	//������Ϣ
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_INVITE, pData, wDataSize);

	return true;
}

//�������˵����� 
bool CHandleFromGate::On_CMD_LC_CLUB_INVITE_REMIND( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_INVITE_REMIND);
	if( wDataSize != Size) return false;
	STR_CMD_LC_CLUB_INVITE_REMIND * pCMD = (STR_CMD_LC_CLUB_INVITE_REMIND*) pData;

	auto player = CPlayerManager::FindPlayerByID(pCMD->dwTagID);
	if( !player) return false;

	//֪ͨ�û�
	g_GameCtrl->SendData(player->GetSocketID(), MDM_CLUB, CMD_LC_CLUB_INVITE_REMIND, 
		pData, wDataSize);	

	return true;
}

//�������˵Ļظ�
bool CHandleFromGate::On_SUB_CL_CLUB_INVITE_RESULT(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_INVITE_RESULT)) return false;

	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_INVITE_RESULT, dwSocketID, pData, wDataSize);

	return true;
}
//�������˵Ļظ� ���� 
bool CHandleFromGate::On_CMD_LC_CLUB_INVITE_RESULT( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_INVITE_RESULT);
	if( wDataSize != Size) return false;

	STR_CMD_LC_CLUB_INVITE_RESULT *pCMD = (STR_CMD_LC_CLUB_INVITE_RESULT*) pData;

	//������سɹ�, ��֪ͨ�ͻ���ˢ�¹����б�
	if(pCMD->lResultCode == DB_SUCCESS)
	{
		STR_CMD_LC_CLUB_LIST_RE CMD;
		CMD.byMask = 1;

		g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_LIST_RE, &CMD, sizeof(CMD));
	}

	//������Ϣ
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_INVITE_RESULT, pData, wDataSize);

	return true;
}

//�߳��˳�����
bool CHandleFromGate::On_SUB_CL_CLUB_QUIT(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_QUIT)) return false;

	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_QUIT, dwSocketID, pData, wDataSize);
	return true;
}
//�û��˳����� ����
bool CHandleFromGate::On_CMD_LC_CLUB_QUIT( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_QUIT);
	if( wDataSize != Size) return false;

	//������Ϣ
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_QUIT, pData, wDataSize);
	return true;
}

//ְ������
bool CHandleFromGate::On_SUB_CL_CLUB_APPOINTMENT(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_APPOINTMENT)) return false;

	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_APPOINTMENT, dwSocketID, pData, wDataSize);
	return true;
}
//ְ������ ����
bool CHandleFromGate::On_CMD_LC_CLUB_APPOINTMENT( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_APPOINTMENT);
	if( wDataSize != Size) return false;

	//������Ϣ
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_APPOINTMENT, pData, wDataSize);
	return true;
}
//ְ������ ����
bool CHandleFromGate::On_CMD_LC_CLUB_APPOINTMENT_NOTE( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//У�����
	if( wDataSize != sizeof(STR_CMD_LC_CLUB_APPOINTMENT_NOTE)) return false;
	STR_CMD_LC_CLUB_APPOINTMENT_NOTE * pCMD = (STR_CMD_LC_CLUB_APPOINTMENT_NOTE*) pData;

	auto player = CPlayerManager::FindPlayerByID(pCMD->dwUserID);
	if(!player) return false;
	
	//֪ͨ�û�
	g_GameCtrl->SendData(player->GetSocketID(), MDM_CLUB, CMD_LC_CLUB_APPOINTMENT_NOTE, 
		pData, wDataSize);		

	return true;
}


//�������б�
bool CHandleFromGate::On_SUB_CL_CLUB_APPLICANT_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_APPLICANT_LIST)) return false;

	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_APPLICANT_LIST, dwSocketID, pData, wDataSize);
	return true;
}
//�������б� ����
bool CHandleFromGate::On_CMD_LC_CLUB_APPLICANT_LIST( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_APPLICANT_LIST);
	if( (wDataSize % Size) != 0) return false;

	//������Ϣ
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_APPLICANT_LIST, pData, wDataSize);
	return true;
}
//�������б� ����
bool CHandleFromGate::On_CMD_LC_CLUB_APPLICANT_LIST_FINISH( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	STR_CMD_LC_CLUB_APPLICANT_LIST_FINISH cmd;
	cmd.byMask = 1;

	//������Ϣ
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_APPLICANT_LIST_FINISH, &cmd, sizeof(cmd));
	return true;
}

//�������˲鿴�Լ��������б�
bool CHandleFromGate::On_SUB_CL_CLUB_INQUERY_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_INQUERY_LIST)) return false;

	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_INQUERY_LIST, dwSocketID, pData, wDataSize);
	return true;
}
//�������˲鿴�Լ��������б� ����
bool CHandleFromGate::On_CMD_LC_CLUB_INQUERY_LIST( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_INQUERY_LIST);
	if( (wDataSize % Size) != 0) return false;

	//������Ϣ
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_INQUERY_LIST, pData, wDataSize);
	return true;
}
//�������˲鿴�Լ��������б� ����
bool CHandleFromGate::On_CMD_LC_CLUB_INQUERY_LIST_FINISH( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	STR_CMD_LC_CLUB_INQUERY_LIST_FINISH cmd;
	cmd.byMask = 1;

	//������Ϣ
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_INQUERY_LIST_FINISH, &cmd, sizeof(cmd));
	return true;
}


//����ս��ͳ��
bool CHandleFromGate::On_SUB_CL_CLUB_RECORD_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_RECORD_LIST)) return false;

	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_RECORD_LIST, dwSocketID, pData, wDataSize);
	return true;
}
//����ս��ͳ�� ����
bool CHandleFromGate::On_CMD_LC_CLUB_RECORD_LIST( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_CMD_LC_CLUB_RECORD_LIST);
	if( (wDataSize < Size) || 
		( (wDataSize % Size) != 0))
		return false;

	//������Ϣ
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_RECORD_LIST, pData, wDataSize);
	return true;
}
//����ս��ͳ�� ����
bool CHandleFromGate::On_CMD_LC_CLUB_RECORD_FINISH( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	STR_CMD_LC_CLUB_RECORD_FINISH cmd;
	cmd.byMask = 1;

	//������Ϣ
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_RECORD_FINISH, &cmd, sizeof(cmd));
	return true;
}

#pragma endregion

#pragma region �������
//���͵���
bool CHandleFromGate::On_SUB_CL_GIFT_GIVE_PROPS(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//����У��
	if(wDataSize!=sizeof(STR_SUB_CL_GIFT_GIVE_PROPS))
		return false;

	return g_GameCtrl->PostDataBaseRequest(DBR_CL_GIFT_GIVE_PROPS,dwSocketID, pData, wDataSize);
}

//���͵��߷���
bool CHandleFromGate::On_CMD_LC_GIFT_GIVE_PROPS( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//����У��
	if(wDataSize!=sizeof(STR_CMD_LC_GIFT_GIVE_PROPS))
		return false;

	g_GameCtrl->SendData(dwScoketID, MDM_SHOP, CMD_LC_GIFT_GIVE_PROPS, pData, wDataSize);

	return true;
}

//���͵��� ֪ͨ������
bool CHandleFromGate::On_CMD_LC_GIFT_GIVE_PROPS_SHOW( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//����У��
	if(wDataSize!=sizeof(STR_CMD_LC_GIFT_GIVE_PROPS_SHOW)) return false;
		
	//��������
	STR_CMD_LC_GIFT_GIVE_PROPS_SHOW *pCMD = (STR_CMD_LC_GIFT_GIVE_PROPS_SHOW*) pData;

	auto player = CPlayerManager::FindPlayerByID(pCMD->dwTargetID);
	if(!player) return false;

	//֪ͨ�û�
	g_GameCtrl->SendData(player->GetSocketID(), MDM_SHOP, CMD_LC_GIFT_GIVE_PROPS_SHOW, 
		pData, wDataSize);	

	return true;
}


//�û���ֵ��Ϣ
bool CHandleFromGate::On_SUB_CL_Other_ReChargeInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	ASSERT( wDataSize == sizeof(STR_SUB_CL_OTHER_RECHARGE_INFO));
	if(wDataSize != sizeof(STR_SUB_CL_OTHER_RECHARGE_INFO)) return false;

	//������Ϣ
	STR_SUB_CL_OTHER_RECHARGE_INFO * pUserRequest = (STR_SUB_CL_OTHER_RECHARGE_INFO *)pData;
	//�������
	STR_DBR_CL_OTHER_RECHARGE_INFO UserRequest;
	ZeroMemory(&UserRequest,sizeof(UserRequest));

	UserRequest.dwUserID = pUserRequest->dwUserID;

	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_CL_USER_RECHARGE_INFO,dwSocketID,&UserRequest,sizeof(UserRequest));
	return true;
}

//�û���ֵ��Ϣ����
bool CHandleFromGate::On_CMD_LC_Other_RechargeInfo( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//��������
	STR_CMD_LC_OTHER_RECHARGE_INFO RechangeInfo;
	ZeroMemory(&RechangeInfo,sizeof(RechangeInfo));

	//��������
	STR_DBO_CL_OTHER_RECHARGE_INFO * pRechangeInfo = (STR_DBO_CL_OTHER_RECHARGE_INFO *)pData;

	//��������
	RechangeInfo.dwMinMoney = pRechangeInfo->dwMinMoney;
	RechangeInfo.dwChangeScale = pRechangeInfo->dwChangeScale;

	//��������
	WORD wHeadSize=sizeof(RechangeInfo);
	g_GameCtrl->SendData(dwScoketID, MDM_SERVICE, CMD_LC_OTHERS_RECHARGE_INFO, &RechangeInfo, wHeadSize);
	return true;
}

//�һ�����
bool CHandleFromGate::On_SUB_CL_Other_ExchangeInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	ASSERT( wDataSize == sizeof(STR_SUB_CL_OTHER_EXCHANGE_INFO));
	if(wDataSize != sizeof(STR_SUB_CL_OTHER_EXCHANGE_INFO)) return false;

	//������Ϣ
	STR_SUB_CL_OTHER_EXCHANGE_INFO * pUserRequest = (STR_SUB_CL_OTHER_EXCHANGE_INFO *)pData;
	//�������
	STR_DBR_CL_OTHER_EXCHANGE_INFO UserRequest;
	ZeroMemory(&UserRequest,sizeof(UserRequest));

	UserRequest.dwUserID = pUserRequest->dwUserID;

	//Ͷ������
	g_GameCtrl->PostDataBaseRequest(DBR_CL_USER_EXCHANGE_INFO, dwSocketID, &UserRequest, sizeof(UserRequest));
	return true;
}

//�һ����߷���
bool CHandleFromGate::On_CMD_LC_Other_ExchangeInfo( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//��������
	STR_DBO_CL_OTHER_EXCHANGE_INFO * pExchangeInfo=(STR_DBO_CL_OTHER_EXCHANGE_INFO *)pData;

	//��������
	STR_CMD_LC_OTHER_EXCHANGE_INFO ExchangeInfo;
	ZeroMemory(&ExchangeInfo,sizeof(ExchangeInfo));

	ExchangeInfo.dwMinMoney = pExchangeInfo->dwMinMoney;
	ExchangeInfo.dwChangeScale = pExchangeInfo->dwChangeScale;
	ExchangeInfo.dwBankMoney = pExchangeInfo->dwBankMoney;
	ExchangeInfo.dwWithdrawals = pExchangeInfo->dwWithdrawals;

	//��������
	WORD wHeadSize=sizeof(ExchangeInfo);
	g_GameCtrl->SendData(dwScoketID, MDM_SERVICE, CMD_LC_OTHERS_EXCHANGE_INFO, &ExchangeInfo, wHeadSize);

	return true;
}
#pragma endregion

#pragma region MDM_SHOP �̳ǵ���
//�̳ǹ���
bool CHandleFromGate::On_SUB_CL_SHOP_BUY(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//У�����
	if(wDataSize != sizeof(STR_SUB_CL_SHOP_BUY)) return false;
	STR_SUB_CL_SHOP_BUY * pSub = (STR_SUB_CL_SHOP_BUY *)pData;

	CPlayer *player = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if(player == NULL) return false;

	//������
	//if(0 != Chck())
	
	tagUserProperty prop;
	prop.dwPropID = pSub->dwGoodsID;
	prop.dwPropCount = pSub->dwGoodsNum;
	player->AddProp(prop);

	//���ظ�client
	STR_CMD_LC_SHOP_BUY_RESULT cmd;
	cmd.byResult = 0;
	g_GameCtrl->SendData(dwSocketID, MDM_SHOP, CMD_LC_SHOP_BUY_RESULT, &cmd, sizeof(cmd));
	return true;
}

//���߲�ѯ
bool CHandleFromGate::On_SUB_CL_BAG_QUERY(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	CPlayer *player = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if(player == NULL) return true;

	WORD wPacketSize=0;
	BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
	STR_CMD_LC_BAG_RESULT * pCMD=NULL;
	for(auto item : player->GetProps())
	{
		if ((wPacketSize+sizeof(STR_CMD_LC_BAG_RESULT))>sizeof(cbBuffer))
		{
			g_GameCtrl->SendData(dwSocketID, MDM_SHOP, CMD_LC_BAG_RESULT, cbBuffer, wPacketSize);
			wPacketSize=0;
		}
	
		pCMD=(STR_CMD_LC_BAG_RESULT *)(cbBuffer+wPacketSize);
		pCMD->dwGoodsID = item.dwPropID;
		pCMD->dwGoodsNum = item.dwPropCount;

		wPacketSize+=sizeof(STR_CMD_LC_BAG_RESULT);
	}

	if (wPacketSize>0) g_GameCtrl->SendData(dwSocketID, MDM_SHOP, CMD_LC_BAG_RESULT, cbBuffer, wPacketSize);

	g_GameCtrl->SendData(dwSocketID, MDM_SHOP, CMD_LC_BAG_FINISH, NULL, 0);
	return true;
}

//������Ʒ��ѯ ����
bool CHandleFromGate::On_DBO_CL_BAG_RESULT( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//У�����
	WORD Size = sizeof(STR_DBO_CL_BAG_RESULT);
	if( (wDataSize < Size) || 
		( (wDataSize % Size) != 0))
		return false;

	//��ø���
	int count = wDataSize / sizeof(STR_DBO_CL_BAG_RESULT);

	//���ص��б�
	STR_DBO_CL_BAG_RESULT *TaskList = (STR_DBO_CL_BAG_RESULT *)pData;
	for (int i=0; i<count; i++)
	{
		CPlayer *player = CPlayerManager::FindPlayerByID((TaskList+i)->dwUserID);
		player ->InitProp((TaskList+i)->Prop);
		CLog::Log(log_debug, "InitProp propid: %ld", (TaskList+i)->Prop.dwPropID);
	}

	return true;
}
#pragma endregion