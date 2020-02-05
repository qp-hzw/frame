#ifndef HANDLE_FROM_CLIENT_HEAD_FILE_L
#define HANDLE_FROM_CLIENT_HEAD_FILE_L

#include "Stdafx.h"

class CHandleFromGate
{
	//�ַܷ�����
public:
	static bool HandlePacket(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	static bool HandlePacketDB(WORD wRequestID, DWORD dwScoketID, VOID * pData, WORD wDataSize);

	//һ���ַ�����
private:
	//��¼ģ��
	static bool OnTCPNetworkMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�û�����
	static bool OnTCPNetworkMainService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�̳ǵ���
	static bool On_MDM_SHOP(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//����Ȧ
	static bool On_MDM_CLUB(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��Ϸ�����
	static bool On_MDM_GAME(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//���崦��
private:
#pragma region MAIN_LOGON ��¼ģ��
	//�ʺŵ�¼
	static bool On_SUB_CL_Logon_Accounts(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�˺ŵ�¼����
	static bool On_CMD_LC_Logon_Account(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//�ʺ�ע��
	static bool On_SUB_CL_Logon_Register(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�˺�ע��ɹ� ���ص����˺ŵ�¼�ɹ� -- �ͻ���ֱ�ӽ������

	//ƽ̨��¼
	static bool On_SUB_CL_Logon_Platform(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//ƽ̨��¼����
	static bool On_CMD_LC_Logon_Platform(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//�ظ���¼�߳�
	static bool On_CMD_LC_Logon_RepeatLogon(DWORD UserID, DWORD dwContextID);

	//��¼����
	static bool On_CMD_LC_Logon_Logon_Reward(DWORD dwContextID, SYSTEMTIME LasLogonDate);
#pragma endregion

#pragma region MAIN_SERVICE
	//��˿��ע
	static bool On_SUB_CL_SERVICE_FLOWER(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�鿴��ע
	static bool On_SUB_CL_SERVICE_FLOWER_QUERY(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�鿴��˿
	static bool On_SUB_CL_SERVICE_FLOWING_QUERY(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//ˢ���û���Ϣ
	static bool On_SUB_CL_Service_RefreshUserInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//ˢ���û���Ϣ����
	static bool On_CMD_LC_Service_RefreshUserInfo( DWORD dwContextID, VOID * pData);

	//��ȡ�û�¼���б�
	static bool On_SUB_CL_Service_GetUserRecordList(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ȡ�û�¼���б���
	static bool On_CMD_LC_Service_GetUserRecordList(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//��ȡָ��ID¼��
	static bool On_SUB_CL_Service_GetSpecificRecord(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ȡָ��ID¼�񷵻�
	static bool On_CMD_LC_Service_GetSpecificRecord(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//��ȡ���߽���
	static bool On_SUB_CL_Service_OnlineReward(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ȡ���߽�������
	static bool On_CMD_LC_Service_OnlineReward( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//��ȡ�����б�
	static bool On_SUB_CL_Service_GetTaskList(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ȡ�����б���
	static bool On_CMD_LC_Service_GetTaskList( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//��ȡ������
	static bool On_SUB_CL_Service_GetTaskReward(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ȡ����������
	static bool On_CMD_LC_Service_GetTaskReward( DWORD dwContextID, VOID * pData, WORD wDataSize );
	
	//��ȡ���а񣬷���Ҳ��������
	static bool On_SUB_CL_Service_GetRankList(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//�齱����
	static bool On_SUB_CL_Service_RequestLottery(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�齱���󷵻�
	static bool On_CMD_LC_Service_RequestLottery( DWORD dwContextID, VOID * pData, WORD wDataSize );

	// pure�������а�
	static bool On_SUB_CL_SERVICE_PURE_STANDING_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//pure�������а� ����
	static bool On_CMD_LC_SERVICE_PURE_STANDING_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//pure�������а� ����
	static bool On_CMD_LC_SERVICE_PURE_STANDING_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize );

	// pure���ս��
	static bool On_SUB_CL_SERVICE_PURE_RECORD_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//pure���ս�� ����
	static bool On_CMD_LC_SERVICE_PURE_RECORD_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//pure��������Ϣ ����
	static bool On_CMD_LC_SERVICE_PURE_RECORD_LIST_PINFO( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//pure���ս�� ����
	static bool On_CMD_LC_SERVICE_PURE_RECORD_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize );

	// pureС��ս��
	static bool On_SUB_CL_SERVICE_PURE_XJ_RECORD_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//pureС��ս�� ����
	static bool On_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//pureС��ս�� ����
	static bool On_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_PINFO( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//pureС��ս�� ����
	static bool On_CMD_LC_SERVICE_PURE_XJ_RECORD_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//С��¼��ط�
	static bool On_SUB_CL_Service_XJRecordPlayback(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//С��¼��ط� ����
	static bool On_CMD_LC_Service_XJRecordPlayback( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//�����������Ϣ
	static bool On_SUB_CL_SERVICE_MATCH_INFO(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�����������Ϣ ����
	static bool On_CMD_LC_SERVICE_MATCH_INFO(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�����������Ϣ ����
	static bool On_CMD_LC_SERVICE_MATCH_INFO_FINISH(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//�޸ĸ�������
	static bool On_SUB_CL_Service_ModifyPersonalInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�޸ĸ������Ϸ���
	static bool On_CMD_LC_Service_ModifyPersonalInfo( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//���������������	
	static bool On_CMD_LC_CommonOperateResult( DWORD dwContextID, VOID * pData, WORD wDataSize );
#pragma endregion

#pragma region MDM_CLUB ����Ȧ
	//��ѯ����Ȧ�б�
	static bool On_SUB_CL_CLUB_ALL_CLUB_INFO_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ѯ����Ȧ�б���
	static bool On_CMD_LC_CLUB_ALL_CLUB_INFO_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ѯ����Ȧ�б����
	static bool On_CMD_LC_CLUB_ALL_INFO_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize);

	//��ѯָ������Ȧ�����б�
	static bool On_SUB_CL_CLUB_ROOM_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ѯָ������Ȧ�����б� ����
	static bool On_CMD_LC_CLUB_ROOM_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//��ѯָ������Ȧ�����б� ����
	static bool On_CMD_LC_CLUB_ROOM_LIST_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//��ѯδ��Ա, �������Ȧ(���9��)
	static bool On_SUB_CL_CLUB_RANDOM_CLUB_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ѯδ��Ա, �������Ȧ(���9��) ����
	static bool On_CMD_LC_CLUB_RANDOM_CLUB_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//��ѯδ��Ա, �������Ȧ(���9��) ����
	static bool On_CMD_LC_CLUB_RANDOM_CLUB_LIST_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize );
	
	//�����������Ȧ
	static bool On_SUB_CL_CLUB_JOIN_CLUB(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�����������Ȧ����
	static bool On_CMD_LC_CLUB_JOIN_CLUB(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�����������Ȧ�㲥
	static bool On_CMD_LC_CLUB_JOIN_CLUB_BDCAST(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�����������Ȧ ֪ͨ�ͻ���ʵʱˢ�¾��ֲ�
	static bool On_CMD_LC_CLUB_JOIN_CLUB_RE(DWORD dwContextID, VOID * pData, WORD wDataSize);
	
	//��ɢ����Ȧ
	static bool On_SUB_CL_CLUB_DISS_CLUB(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ɢ����Ȧ ����
	static bool On_CMD_LC_CLUB_DISS_CLUB(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//��������Ȧ
	static bool On_SUB_CL_CLUB_CREATE_CLUB(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��������Ȧ ����
	static bool On_CMD_LC_CLUB_CREATE_CLUB( DWORD dwContextID, VOID * pData, WORD wDataSize);

	//��������
	static bool On_SUB_CL_CLUB_ROOM_SETTING(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�������� ����
	static bool On_CMD_LC_CLUB_ROOM_SETTING( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//���󷿼�����
	static bool On_SUB_CL_CLUB_ROOM_QUERY_SETTING(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�������� ����
	static bool On_CMD_LC_CLUB_ROOM_QUERY_SETTING( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//����뿪���ֲ�����
	static bool On_SUB_CL_CLUB_ROOM_USER_LEAVE(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//��ɢ��������
	static bool On_SUB_CL_CLUB_ROOM_DISSOLVE(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ɢ�������� ����
	static bool On_CMD_LC_CLUB_ROOM_DISSOLVE( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//��ɢ��������
	static bool On_SUB_CL_CLUB_TABLE_DISSOLVE(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ɢ�������� ����
	static bool On_CMD_LC_CLUB_TABLE_DISSOLVE( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//������뷿��
	static bool On_SUB_CL_CLUB_JOIN_ROOM(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//������뷿�� ����
	static bool On_CMD_LC_CLUB_JOIN_ROOM( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//�����б�
	static bool On_CMD_LC_CLUB_TABLE_LIST_TABLE(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��������б�
	static bool On_CMD_LC_CLUB_TABLE_LIST_USER(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//����Ȧ����
	static bool On_SUB_CL_CLUB_NOTICE(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//����Ȧ���� ����
	static bool On_CMD_LC_CLUB_NOTICE( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//����Ȧ���� �㲥
	static bool On_CMD_LC_CLUB_NOTICE_BDCAST(DWORD dwContextID, VOID * pData, WORD wDataSize);
	
	//����Ȧ���
	static bool On_SUB_CL_CLUB_MESSAGE(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//����Ȧ��� ����
	static bool On_CMD_LC_CLUB_MESSAGE( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//���׷���
	static bool On_SUB_CL_CLUB_CONTRIBUTE_FK(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//���׷��� ����
	static bool On_CMD_LC_CLUB_CONTRIBUTE_FK( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//����Ȧ����
	static bool On_SUB_CL_CLUB_AUTO_AGREE(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//����Ȧ���� ����
	static bool On_CMD_LC_CLUB_AUTO_AGREE( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//����Ȧ����
	static bool On_SUB_CL_CLUB_CHAT(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//����Ȧ���� ����
	static bool On_CMD_LC_CLUB_CHAT( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//����Ȧ���� �㲥
	static bool On_CMD_LC_CLUB_CHAT_BDCAST(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//����Ȧ�ö�
	static bool On_SUB_CL_CLUBSTICKY_POST(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//����Ȧ�ö� ����
	static bool On_CMD_LC_CLUB_STICKY_POST( DWORD dwContextID, VOID * pData, WORD wDataSize );
	
	//Ⱥ��|�����������Ϣ�Ĵ�(ͬ��|�ܾ�)
	static bool On_SUB_CL_CLUB_APPLICANT_RESULT(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//Ⱥ��|�����������Ϣ�Ĵ�(ͬ��|�ܾ�) ����
	static bool On_CMD_LC_CLUB_APPLICANT_RESULT( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//�����Ա����
	static bool On_SUB_CL_CLUB_MEMBER_MANAGER(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�����Ա���� ����
	static bool On_CMD_LC_CLUB_MEMBER_MANAGER( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//�����Ա���� ����
	static bool On_CMD_LC_CLUB_MEMBER_MANAGER_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//���������Ϣ
	static bool On_CMD_LC_CLUB_DATA( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//���������������Ȧ
	static bool On_SUB_CL_CLUB_INVITE(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//���������������Ȧ ����
	static bool On_CMD_LC_CLUB_INVITE( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//�������˵�����
	static bool On_CMD_LC_CLUB_INVITE_REMIND( DWORD dwContextID, VOID * pData, WORD wDataSize );
	
	//�������˵Ļظ�
	static bool On_SUB_CL_CLUB_INVITE_RESULT(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�������˵Ļظ� ����
	static bool On_CMD_LC_CLUB_INVITE_RESULT( DWORD dwContextID, VOID * pData, WORD wDataSize );
	
	//�߳��˳�����
	static bool On_SUB_CL_CLUB_QUIT(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�߳��˳����� ����
	static bool On_CMD_LC_CLUB_QUIT( DWORD dwContextID, VOID * pData, WORD wDataSize );
	
	//ְ������
	static bool On_SUB_CL_CLUB_APPOINTMENT(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//ְ������ ����
	static bool On_CMD_LC_CLUB_APPOINTMENT( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//ְ������ ����
	static bool On_CMD_LC_CLUB_APPOINTMENT_NOTE( DWORD dwContextID, VOID * pData, WORD wDataSize );
	
	//�������˲鿴�Լ��������б�
	static bool On_SUB_CL_CLUB_INQUERY_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�������˲鿴�Լ��������б� ����
	static bool On_CMD_LC_CLUB_INQUERY_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//�������˲鿴�Լ��������б� ����
	static bool On_CMD_LC_CLUB_INQUERY_LIST_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//�������б�
	static bool On_SUB_CL_CLUB_APPLICANT_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�������б� ����
	static bool On_CMD_LC_CLUB_APPLICANT_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//�������б� ����
	static bool On_CMD_LC_CLUB_APPLICANT_LIST_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//����ս��ͳ��
	static bool On_SUB_CL_CLUB_RECORD_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//����ս��ͳ�� ����
	static bool On_CMD_LC_CLUB_RECORD_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//����ս��ͳ�� ����
	static bool On_CMD_LC_CLUB_RECORD_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize );

#pragma endregion

#pragma region MDM_GIFT �������
	//���͵���
	static bool On_SUB_CL_GIFT_GIVE_PROPS(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//���͵��߷���
	static bool On_CMD_LC_GIFT_GIVE_PROPS( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//���͵��߷���
	static bool On_CMD_LC_GIFT_GIVE_PROPS_SHOW( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//�û���ֵ��Ϣ
	static bool On_SUB_CL_Other_ReChargeInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�û���ֵ��Ϣ����
	static bool On_CMD_LC_Other_RechargeInfo( DWORD dwContextID, VOID * pData, WORD wDataSize);

	//�һ�����
	static bool On_SUB_CL_Other_ExchangeInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�һ����߷���
	static bool On_CMD_LC_Other_ExchangeInfo( DWORD dwContextID, VOID * pData, WORD wDataSize);

	//TODO ���а���ȡ������ʱ��������
	//��ȡ���а�������
	static bool OnDBRankRewardResult( DWORD dwContextID, VOID * pData, WORD wDataSize );
#pragma endregion

#pragma region MDM_SHOP �����̳�
	//�̳ǹ���
	static bool On_SUB_CL_SHOP_BUY(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//������Ʒ��ѯ
	static bool On_SUB_CL_BAG_QUERY(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//������Ʒ��ѯ
	static bool On_DBO_CL_BAG_RESULT( DWORD dwContextID, VOID * pData, WORD wDataSize );
	
#pragma endregion

};

#endif