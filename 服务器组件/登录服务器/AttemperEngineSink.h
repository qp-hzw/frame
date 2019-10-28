#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#include "Stdafx.h"
#include "ServerListManager.h"
#include "DataBasePacket.h"

typedef CWHArray<tagOfflineUser>COfflineUserItemArray;					//������� �б�
typedef CWHArray<STR_DBO_UPDATA_MARQUEE> MarqueeMsgArray;				//�������Ϣ �б�

//�󶨲���
struct tagBindParameter
{
	//�������
	DWORD							dwUserID;							//�û�ID
	DWORD							dwSocketID;							//�����ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ
	DWORD							dwActiveTime;						//����ʱ��
};

//////////////////////////////////////////////////////////////////////////////////
//1���̳��ں��е�IAttemperEngineSink��
//2����������յ��� �������¼� + ʱ���¼� + ���ݿ��¼���
//////////////////////////////////////////////////////////////////////////////////
class CAttemperEngineSink : public IAttemperEngineSink
{
	//��Ԫ����
	friend class CServiceUnits;

	//״̬����
protected:
	bool                            m_bShowServerStatus;                //��ʾ������״̬

	//��������
protected:
	tagBindParameter *				m_pBindParameter;					//��������
	void*							m_pRankManager;						//���а������

	//�������
protected:
	CServerListManager				m_ServerListManager;				//Server�б����
	COfflineUserItemArray			m_OfflineUserListManager;			//��������б�
	MarqueeMsgArray					m_MarqueeMsgListManager;			//�������Ϣ�б�

#pragma region ��������
	//��������
public:
	//���캯��
	CAttemperEngineSink();
	//��������
	virtual ~CAttemperEngineSink();

	//����ӿ�
public:
	//��������
	virtual bool StartService(){return true;}
	//ֹͣ����
	virtual bool ConcludeService(){return true;}
#pragma endregion

#pragma region �¼��ӿ�
	//�첽�ӿ�
public:
	//�����¼�
	virtual bool OnAttemperEngineStart(IUnknownEx * pIUnknownEx);
	//ֹͣ�¼�
	virtual bool OnAttemperEngineConclude(IUnknownEx * pIUnknownEx);

	//�¼��ӿ�
public:
	//�����¼�
	virtual bool OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize);

	//�ں��¼�
public:
	//ʱ���¼�
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM wBindParam);
	//���ݿ��¼�
	virtual bool OnEventDataBaseResult(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);

	//socket::client ����Э�������ӣ�
public:
	//�����¼�
	virtual bool OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode);
	//�ر��¼�
	virtual bool OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason);
	//��ȡ�¼�
	virtual bool OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize);

	//socket::server ����client���ӣ�
public:
	//Ӧ���¼�
	virtual bool OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID);
	//�ر��¼�
	virtual bool OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID);
	//��ȡ�¼�
	virtual bool OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID);
#pragma endregion

	//��������
protected:
	//TODONOW added by WangChengQing
	//�ж����״̬, ��֪ͨЭ����  -- 1�������� ��2������Ϸ��3��Ϸ���ߣ�4����
	void CheckUserState_Logon(DWORD dwUserID, bool bIsExist);


#pragma region ��������
	//�б��¼���SendControlPacket����Ӧ������
protected:
	//��Ϸ����
	bool OnDBPCGameTypeItem(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��Ϸ����
	bool OnDBPCGameKindItem(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��Ϸ�ڵ�
	bool OnDBPCGameNodeItem(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//���ؽ��
	bool OnDBPCGameListResult(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//�����������Ϣ ����
	bool On_DBO_UPDATA_MARQUEE(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�����������Ϣ ����
	bool On_DBO_UPDATA_MARQUEE_FINISH(DWORD dwContextID, VOID * pData, WORD wDataSize);
#pragma endregion

#pragma region Э���� ����Ϣ�ŷַ�����
	//���Ӵ���
protected:
	//ע��ģ��
	bool OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//�б�����
	bool OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��ת����
	bool OnTCPSocketMainTransfer(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��̨����
	bool OnTCPSocketMainWeb(WORD wSubCmdID, VOID * pData, WORD wDataSize);
#pragma endregion

#pragma region �ͻ��� ����Ϣ�ŷַ�����
	//������Ϣ�¼����ͻ��ˣ�
protected:
	//��¼ģ��
	bool OnTCPNetworkMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�û�����
	bool OnTCPNetworkMainService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�������
	bool OnTCPNetworkMainOther(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�̳ǵ���
	bool On_MDM_SHOP(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//����Ȧ
	bool On_MDM_CLUB(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��Ϸ�����
	bool On_MDM_GAME(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
#pragma endregion

#pragma region MAIN_LOGON ��¼ģ��
	//�������¼���������
	/* ����Ϣ��1 MAIN_LOGON ��¼ģ�� */
protected:
	//�ʺŵ�¼
	bool On_SUB_CL_Logon_Accounts(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�˺ŵ�¼����
	bool On_CMD_LC_Logon_Account(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//�ʺ�ע��
	bool On_SUB_CL_Logon_Register(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�˺�ע��ɹ� ���ص����˺ŵ�¼�ɹ� -- �ͻ���ֱ�ӽ������

	//ƽ̨��¼
	bool On_SUB_CL_Logon_Platform(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//ƽ̨��¼����
	bool On_CMD_LC_Logon_Platform(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//�ظ���¼�߳�
	bool On_CMD_LC_Logon_RepeatLogon(DWORD UserID, DWORD dwContextID);

	//������ʾ���汾��⣩
	bool On_CMD_LC_Logon_UpdateNotify(DWORD dwVersionCheck, DWORD dwSocketID);

	//��¼����
	bool On_CMD_LC_Logon_Logon_Reward(DWORD dwContextID, SYSTEMTIME LasLogonDate);
#pragma endregion

#pragma region MAIN_LIST �б�����
	/* ����Ϣ��2 MAIN_LIST �б���Ϣ */
protected:
	//��������
	VOID On_CMD_LC_List_Type(DWORD dwSocketID);
	//��������
	VOID On_CMD_LC_List_Kind(DWORD dwSocketID);
	//���ͽڵ�
	VOID On_CMD_LC_List_Node(DWORD dwSocketID);
	//���ͷ���
	VOID On_CMD_LC_List_Room(DWORD dwSocketID);

#pragma endregion

#pragma region MAIN_SERVICE
	/* ����Ϣ��3 MAIN_SERVICE ����ģ�� */
protected:
	//��ҷ���
	bool On_SUB_CL_Service_UserFeedBack(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ҷ�������
	bool On_CMD_LC_Service_UserFeedBack(DWORD dwContextID, VOID * pData);

	//ˢ���û���Ϣ
	bool On_SUB_CL_Service_RefreshUserInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//ˢ���û���Ϣ����
	bool On_CMD_LC_Service_RefreshUserInfo( DWORD dwContextID, VOID * pData);

	//��ѯ������Ϣ�б�
	bool On_SUB_CL_Service_QueryRoomList(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ѯ������Ϣ�б���
	bool On_CMD_LC_Service_QueryRoomList(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//��ѯ�ѿ�������������Ϣ�б���
	bool On_CMD_LC_Service_QueryOpenedRoomList(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//��ȡ������
	bool On_SUB_CL_Service_GetRichList(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ȡ�����񷵻�
	bool On_CMD_LC_Service_GetRichList( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//��ȡ�û�¼���б�
	bool On_SUB_CL_Service_GetUserRecordList(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ȡ�û�¼���б���
	bool On_CMD_LC_Service_GetUserRecordList(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//��ȡָ��ID¼��
	bool On_SUB_CL_Service_GetSpecificRecord(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ȡָ��ID¼�񷵻�
	bool On_CMD_LC_Service_GetSpecificRecord(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//��ȡ���߽���
	bool On_SUB_CL_Service_OnlineReward(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ȡ���߽�������
	bool On_CMD_LC_Service_OnlineReward( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//��ȡ�����б�
	bool On_SUB_CL_Service_GetTaskList(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ȡ�����б���
	bool On_CMD_LC_Service_GetTaskList( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//��ȡ������
	bool On_SUB_CL_Service_GetTaskReward(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ȡ����������
	bool On_CMD_LC_Service_GetTaskReward( DWORD dwContextID, VOID * pData, WORD wDataSize );
	
	//��ȡ���а񣬷���Ҳ��������
	bool On_SUB_CL_Service_GetRankList(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//�齱����
	bool On_SUB_CL_Service_RequestLottery(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�齱���󷵻�
	bool On_CMD_LC_Service_RequestLottery( DWORD dwContextID, VOID * pData, WORD wDataSize );

	// pure�������а�
	bool On_SUB_CL_SERVICE_PURE_STANDING_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//pure�������а� ����
	bool On_CMD_LC_SERVICE_PURE_STANDING_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//pure�������а� ����
	bool On_CMD_LC_SERVICE_PURE_STANDING_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize );

	// pure���ս��
	bool On_SUB_CL_SERVICE_PURE_RECORD_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//pure���ս�� ����
	bool On_CMD_LC_SERVICE_PURE_RECORD_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//pure��������Ϣ ����
	bool On_CMD_LC_SERVICE_PURE_RECORD_LIST_PINFO( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//pure���ս�� ����
	bool On_CMD_LC_SERVICE_PURE_RECORD_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize );

	// pureС��ս��
	bool On_SUB_CL_SERVICE_PURE_XJ_RECORD_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//pureС��ս�� ����
	bool On_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//pureС��ս�� ����
	bool On_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_PINFO( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//pureС��ս�� ����
	bool On_CMD_LC_SERVICE_PURE_XJ_RECORD_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//С��¼��ط�
	bool On_SUB_CL_Service_XJRecordPlayback(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//С��¼��ط� ����
	bool On_CMD_LC_Service_XJRecordPlayback( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//�ͷ���Ϣ
	bool On_SUB_CL_SERVICE_CUSTOMER_MESSEGE(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�ͷ���Ϣ ����
	bool On_CMD_LC_SERVICE_CUSTOMER_MESSEGE( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//�����Ҵ�����Ϣ
	bool On_SUB_CL_SERVICE_GOLD_INFO(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�����Ҵ�����Ϣ ����
	bool On_CMD_LC_SERVICE_GOLD_INFO( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//�����Ҵ�����Ϣ ����
	bool On_CMD_LC_SERVICE_GOLD_INFO_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//�޸ĸ�������
	bool On_SUB_CL_Service_ModifyPersonalInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�޸ĸ������Ϸ���
	bool On_CMD_LC_Service_ModifyPersonalInfo( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//��ѯ���������ҷ�����ʯ���أ��漰���û���ҵȸı�ģ����᷵�������Ϣ���ÿͻ���ˢ���û������Ϣ��
	bool On_CMD_LC_Service_QueryScoreInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//���������������	
	bool On_CMD_LC_CommonOperateResult( DWORD dwContextID, VOID * pData, WORD wDataSize );
#pragma endregion

#pragma region MDM_CLUB ����Ȧ
	protected:
	//��ѯ����Ȧ�б�
	bool On_SUB_CL_CLUB_ALL_CLUB_INFO_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ѯ����Ȧ�б���
	bool On_CMD_LC_CLUB_ALL_CLUB_INFO_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ѯ����Ȧ�б����
	bool On_CMD_LC_CLUB_ALL_INFO_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize);

	//��ѯָ������Ȧ�����б�
	bool On_SUB_CL_CLUB_ROOM_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ѯָ������Ȧ�����б� ����
	bool On_CMD_LC_CLUB_ROOM_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//��ѯָ������Ȧ�����б� ����
	bool On_CMD_LC_CLUB_ROOM_LIST_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//��ѯδ��Ա, �������Ȧ(���9��)
	bool On_SUB_CL_CLUB_RANDOM_CLUB_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ѯδ��Ա, �������Ȧ(���9��) ����
	bool On_CMD_LC_CLUB_RANDOM_CLUB_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//��ѯδ��Ա, �������Ȧ(���9��) ����
	bool On_CMD_LC_CLUB_RANDOM_CLUB_LIST_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize );
	
	//�����������Ȧ
	bool On_SUB_CL_CLUB_JOIN_CLUB(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�����������Ȧ����
	bool On_CMD_LC_CLUB_JOIN_CLUB(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�����������Ȧ�㲥
	bool On_CMD_LC_CLUB_JOIN_CLUB_BDCAST(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�����������Ȧ ֪ͨ�ͻ���ʵʱˢ�¾��ֲ�
	bool On_CMD_LC_CLUB_JOIN_CLUB_RE(DWORD dwContextID, VOID * pData, WORD wDataSize);
	
	//��ɢ����Ȧ
	bool On_SUB_CL_CLUB_DISS_CLUB(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ɢ����Ȧ ����
	bool On_CMD_LC_CLUB_DISS_CLUB(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//��������Ȧ
	bool On_SUB_CL_CLUB_CREATE_CLUB(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��������Ȧ ����
	bool On_CMD_LC_CLUB_CREATE_CLUB( DWORD dwContextID, VOID * pData, WORD wDataSize);

	//��������
	bool On_SUB_CL_CLUB_ROOM_SETTING(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�������� ����
	bool On_CMD_LC_CLUB_ROOM_SETTING( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//���󷿼�����
	bool On_SUB_CL_CLUB_ROOM_QUERY_SETTING(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�������� ����
	bool On_CMD_LC_CLUB_ROOM_QUERY_SETTING( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//����뿪���ֲ�����
	bool On_SUB_CL_CLUB_ROOM_USER_LEAVE(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//��ɢ��������
	bool On_SUB_CL_CLUB_ROOM_DISSOLVE(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ɢ�������� ����
	bool On_CMD_LC_CLUB_ROOM_DISSOLVE( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//��ɢ��������
	bool On_SUB_CL_CLUB_TABLE_DISSOLVE(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ɢ�������� ����
	bool On_CMD_LC_CLUB_TABLE_DISSOLVE( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//������뷿��
	bool On_SUB_CL_CLUB_JOIN_ROOM(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//������뷿�� ����
	bool On_CMD_LC_CLUB_JOIN_ROOM( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//�����б�
	bool On_CMD_LC_CLUB_TABLE_LIST_TABLE(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��������б�
	bool On_CMD_LC_CLUB_TABLE_LIST_USER(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//����Ȧ����
	bool On_SUB_CL_CLUB_NOTICE(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//����Ȧ���� ����
	bool On_CMD_LC_CLUB_NOTICE( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//����Ȧ���� �㲥
	bool On_CMD_LC_CLUB_NOTICE_BDCAST(DWORD dwContextID, VOID * pData, WORD wDataSize);
	
	//����Ȧ���
	bool On_SUB_CL_CLUB_MESSAGE(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//����Ȧ��� ����
	bool On_CMD_LC_CLUB_MESSAGE( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//���׷���
	bool On_SUB_CL_CLUB_CONTRIBUTE_FK(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//���׷��� ����
	bool On_CMD_LC_CLUB_CONTRIBUTE_FK( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//����Ȧ����
	bool On_SUB_CL_CLUB_AUTO_AGREE(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//����Ȧ���� ����
	bool On_CMD_LC_CLUB_AUTO_AGREE( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//����Ȧ����
	bool On_SUB_CL_CLUB_CHAT(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//����Ȧ���� ����
	bool On_CMD_LC_CLUB_CHAT( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//����Ȧ���� �㲥
	bool On_CMD_LC_CLUB_CHAT_BDCAST(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//����Ȧ�ö�
	bool On_SUB_CL_CLUBSTICKY_POST(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//����Ȧ�ö� ����
	bool On_CMD_LC_CLUB_STICKY_POST( DWORD dwContextID, VOID * pData, WORD wDataSize );
	
	//Ⱥ��|�����������Ϣ�Ĵ�(ͬ��|�ܾ�)
	bool On_SUB_CL_CLUB_APPLICANT_RESULT(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//Ⱥ��|�����������Ϣ�Ĵ�(ͬ��|�ܾ�) ����
	bool On_CMD_LC_CLUB_APPLICANT_RESULT( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//�����Ա����
	bool On_SUB_CL_CLUB_MEMBER_MANAGER(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�����Ա���� ����
	bool On_CMD_LC_CLUB_MEMBER_MANAGER( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//�����Ա���� ����
	bool On_CMD_LC_CLUB_MEMBER_MANAGER_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//���������Ϣ
	bool On_CMD_LC_CLUB_DATA( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//���������������Ȧ
	bool On_SUB_CL_CLUB_INVITE(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//���������������Ȧ ����
	bool On_CMD_LC_CLUB_INVITE( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//�������˵�����
	bool On_CMD_LC_CLUB_INVITE_REMIND( DWORD dwContextID, VOID * pData, WORD wDataSize );
	
	//�������˵Ļظ�
	bool On_SUB_CL_CLUB_INVITE_RESULT(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�������˵Ļظ� ����
	bool On_CMD_LC_CLUB_INVITE_RESULT( DWORD dwContextID, VOID * pData, WORD wDataSize );
	
	//�߳��˳�����
	bool On_SUB_CL_CLUB_QUIT(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�߳��˳����� ����
	bool On_CMD_LC_CLUB_QUIT( DWORD dwContextID, VOID * pData, WORD wDataSize );
	
	//ְ������
	bool On_SUB_CL_CLUB_APPOINTMENT(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//ְ������ ����
	bool On_CMD_LC_CLUB_APPOINTMENT( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//ְ������ ����
	bool On_CMD_LC_CLUB_APPOINTMENT_NOTE( DWORD dwContextID, VOID * pData, WORD wDataSize );
	
	//�������˲鿴�Լ��������б�
	bool On_SUB_CL_CLUB_INQUERY_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�������˲鿴�Լ��������б� ����
	bool On_CMD_LC_CLUB_INQUERY_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//�������˲鿴�Լ��������б� ����
	bool On_CMD_LC_CLUB_INQUERY_LIST_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//�������б�
	bool On_SUB_CL_CLUB_APPLICANT_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�������б� ����
	bool On_CMD_LC_CLUB_APPLICANT_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//�������б� ����
	bool On_CMD_LC_CLUB_APPLICANT_LIST_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//����ս��ͳ��
	bool On_SUB_CL_CLUB_RECORD_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//����ս��ͳ�� ����
	bool On_CMD_LC_CLUB_RECORD_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//����ս��ͳ�� ����
	bool On_CMD_LC_CLUB_RECORD_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize );

#pragma endregion

#pragma region MDM_GIFT �������
protected:
	//���͵���
	bool On_SUB_CL_GIFT_GIVE_PROPS(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//���͵��߷���
	bool On_CMD_LC_GIFT_GIVE_PROPS( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//���͵��߷���
	bool On_CMD_LC_GIFT_GIVE_PROPS_SHOW( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//�û���ֵ��Ϣ
	bool On_SUB_CL_Other_ReChargeInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�û���ֵ��Ϣ����
	bool On_CMD_LC_Other_RechargeInfo( DWORD dwContextID, VOID * pData, WORD wDataSize);

	//�һ�����
	bool On_SUB_CL_Other_ExchangeInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�һ����߷���
	bool On_CMD_LC_Other_ExchangeInfo( DWORD dwContextID, VOID * pData, WORD wDataSize);

	//TODO ���а���ȡ������ʱ��������
	//��ȡ���а�������
	bool OnDBRankRewardResult( DWORD dwContextID, VOID * pData, WORD wDataSize );
#pragma endregion

#pragma region MDM_SHOP �����̳�
	protected:
	//��ѯ�̳�
	bool On_SUB_CL_SHOP_QUERY(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ѯ�̳Ƿ���
	bool On_CMD_LC_SHOP_QUERY_RESULT( DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ѯ�̳ǽ���
	bool On_CMD_LC_SHOP_QUERY_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize);

	//��Ǯ�������
	bool On_SUB_CL_SHOP_MONEY(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��Ǯ������߷���
	bool On_CMD_LC_SHOP_MONEY_RESULT( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//��ʯ�������
	bool On_SUB_CL_SHOP_DIAMOND(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ʯ������߷���
	bool On_CMD_LC_SHOP_DIAMOND_RESULT( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//������Ʒ��ѯ
	bool On_SUB_CL_BAG_QUERY(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//������Ʒ��ѯ���CMD
	bool On_CMD_LC_BAG_RESULT( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//������Ʒ��ѯ���
	bool On_CMD_LC_BAG_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize );
#pragma endregion

};


extern CAttemperEngineSink* g_AttemperEngineSink;

//////////////////////////////////////////////////////////////////////////////////

#endif