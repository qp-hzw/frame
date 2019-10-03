#ifndef DATABASE_ENGINE_SINK_HEAD_FILE
#define DATABASE_ENGINE_SINK_HEAD_FILE
#include "Stdafx.h"
#include "DataBasePacket.h"


//////////////////////////////////////////////////////////////////////////////////
//1���̳��ں˵����ݿ���
//2��������Ϸ���͵�½�����������ݿ���Ϣ
//////////////////////////////////////////////////////////////////////////////////

//���ݿ���
class CDataBaseEngineSink : public IDataBaseEngineSink
{
	//��Ԫ����
	friend class CServiceUnits;

	//�û����ݿ�
protected:
	CDataBaseAide					m_AccountsDBAide;					//�û����ݿ�
	CDataBaseHelper					m_AccountsDBModule;					//�û����ݿ�

	//��Ϸ�����ݿ�
protected:
	CDataBaseAide					m_TreasureDBAide;					//��Ϸ�����ݿ�
	CDataBaseHelper					m_TreasureDBModule;					//��Ϸ�����ݿ�

	//ƽ̨���ݿ�
protected:
	CDataBaseAide					m_PlatformDBAide;					//ƽ̨���ݿ�
	CDataBaseHelper					m_PlatformDBModule;					//ƽ̨���ݿ�

	//��������
public:
	//���캯��
	CDataBaseEngineSink();
	//��������
	virtual ~CDataBaseEngineSink(){};

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { return; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//ϵͳ�¼�
public:
	//�����¼�
	virtual bool OnDataBaseEngineStart(IUnknownEx * pIUnknownEx);
	//ֹͣ�¼�
	virtual bool OnDataBaseEngineConclude(IUnknownEx * pIUnknownEx);

	//�ں��¼�
public:
	//�����¼�
	virtual bool OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);

#pragma region  MDM_LOGON ��½ģ��
	/* **************************��MAIN:1    MDM_LOGON    ��½��*************************************/
protected:
	//�ʺŵ�¼
	bool On_DBR_Logon_Accounts(DWORD dwContextID, VOID * pData, WORD wDataSize);
	bool On_DBO_Logon_Accounts(DWORD dwContextID, DWORD dwResuleCode, LPCTSTR pszErrorString);
	//�ʺ�ע��
	bool On_DBR_Logon_Register(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//ƽ̨��¼
	bool On_DBR_Logon_Platform(DWORD dwContextID, VOID * pData, WORD wDataSize);
	bool On_DBO_Logon_Platform(DWORD dwContextID, DWORD dwResuleCode, LPCTSTR pszErrorString);

	//����û��Ƹ�	TODO ��½���� + ����ҽ���
	bool OnModifyUserInsure(DWORD dwContextID, void * pData, WORD wDataSize);

#pragma endregion

#pragma region MDM_SERVICE �û�����
	/* **************************��MAIN:3    MDM_SERVICE    ����*************************************/
protected:
	//��ҷ���
	bool On_DBR_Service_UserFeedback(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ҷ�������
	bool On_DBO_Service_UserFeedback(DWORD dwContextID, DWORD dwResultCode, LPCTSTR pszErrorString);

	//ˢ���û���Ϣ
	bool On_DBR_Service_RefreshUserInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//ˢ���û���Ϣ����
	bool On_DBO_Service_RefreshUserInfo(DWORD dwContextID, DWORD dwResultCode, LPCTSTR pszErrorString);

	//��ѯ������Ϣ�б�
	bool On_DBR_Service_QueryRoomList(DWORD dwContextID, void * pData, WORD wDataSize);
	//��ѯ������Ϣ�б���
	bool On_DBO_Service_QueryRoomList(DWORD dwContextID, DWORD dwResultCode);

	//��ȡ������
	bool On_DBR_Service_GetRichList(DWORD dwContextID, void * pData, WORD wDataSize);
	//��ȡ�����񷵻�
	bool On_DBO_Service_GetRichList(DWORD dwContextID, DWORD dwResultCode);

	//��ȡ�û�¼���б���֣�
	bool On_DBR_Service_GetUserRecordList(DWORD dwContextID, void * pData, WORD wDataSize);
	//��ȡ�û�¼���б���
	bool On_DBO_Service_GetUserRecordList(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString);

	//��ȡָ��ID¼��С�֣�
	bool On_DBR_Service_GetSpecifiRecord(DWORD dwContextID, void * pData, WORD wDataSize);
	//��ȡָ��ID¼�񷵻�
	bool On_DBO_Service_GetSpecificRecord(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString);

	//��ȡ���߽���
	bool On_DBR_Service_OnlineReward(DWORD dwContextID, void * pData, WORD wDataSize);
	//��ȡ���߽�������
	bool On_DBO_Service_OnlineReward(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString);

	//��ȡ�����б�
	bool On_DBR_Service_GetTaskList(DWORD dwContextID, void * pData, WORD wDataSize);
	//��ȡ�����б���
	bool On_DBO_Service_GetTaskList(DWORD dwUserID, DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString);

	//��ȡ������
	bool On_DBR_Service_GetTaskReward(DWORD dwContextID, void * pData, WORD wDataSize);
	//��ȡ����������
	bool On_DBO_Service_GetTaskReward(DWORD dwUserID, DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString);

	//����齱
	bool On_DBR_Service_RequestLottery(DWORD dwContextID, void * pData, WORD wDataSize);
	//����齱����
	bool On_DBO_Service_RequestLottery(DWORD dwUserID, DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString);
	
	//pure�������а� ��ѯ
	bool On_DBR_CL_SERVICE_PURE_STANDING_LIST(DWORD dwContextID, void * pData, WORD wDataSize);

	//pure���ս�� ��ѯ
	bool On_DBR_CL_SERVICE_PURE_RECORD_LIST(DWORD dwContextID, void * pData, WORD wDataSize);

	//pureС��ս�� ��ѯ
	bool On_DBR_CL_SERVICE_PURE_XJ_RECORD_LIST(DWORD dwContextID, void * pData, WORD wDataSize);

	//С��¼��ط�
	bool On_DBR_CL_Service_XJRecordPlayback(DWORD dwContextID, void * pData, WORD wDataSize);

	//�ͷ���Ϣ
	bool On_DBR_CL_SERVICE_CUSTOMER_MESSEGE(DWORD dwContextID, void * pData, WORD wDataSize);
	
	//�����Ҵ�����Ϣ
	bool On_DBR_CL_SERVICE_GOLD_INFO(DWORD dwContextID, void * pData, WORD wDataSize);
	
	//�޸ĸ�������
	bool On_DBR_Service_ModifyPersonalInfo(DWORD dwContextID, void * pData, WORD wDataSize);
	//�޸ĸ������Ϸ���
	bool On_DBO_Service_ModifyPersonalInfo(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString);

	//��ѯ�������û���ҷ�����ֻҪ�漰��ҷ����ȸı����Ϣ��Ҫ�õ������������ʱ���������棩
	bool On_DBR_QueryScoreInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ѯ��ҷ������أ�����Ȧ + ���͵���ʱ���ã�
	VOID On_DBO_QueryScoreInfo(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString, bool bMobileClient);

	//�û�Socket���ӹر�
	bool On_DBR_GP_QUIT(DWORD dwContextID, VOID * pData, WORD wDataSize); 
	
#pragma endregion

#pragma region MDM_GIFT �������
	/* **************************��MAIN:5    MDM_GIFT    ������ߡ�*************************************/
protected:
	//�������û����͵���
	bool On_DBR_CL_GIFT_GIVE_PROPS(DWORD dwContextID, void * pData, WORD wDataSize);

#pragma endregion

	//��ֵ��Ϣ
	bool On_DBR_Other_RechargeInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��ֵ��Ϣ����
	bool On_DBO_Other_RechargeInfo(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString);

	//�һ�������Ϣ
	bool On_DBR_Other_ExchangeInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�һ�������Ϣ����
	bool On_DBO_Other_ExchangeInfo(DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString);

	//��Ϣ��6�����ò���ʧ�ܽ��
	VOID On_DBO_CommonOperateResult( DWORD dwContextID, DWORD dwErrorCode, LPCTSTR pszErrorString, DBR_CommandSource wRequestID );

	// **************************��������������а������Ӧ���� **************************
protected:
	//�������а��û���Ϣ
	bool OnDBUpdateRankUserItem(DWORD dwContextID, void * pData, WORD wDataSize);
	//��ȡ���а���Ϣ
	bool OnDBReadRankList(DWORD dwContextID, void * pData, WORD wDataSize);
	//��ȡ���а���
	bool OnReceiveRankReward(DWORD dwContextID, void * pData, WORD wDataSize);

#pragma region ��������
	//��������
protected:
	//�����б�
	bool OnRequestLoadGameList(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//������Ϣ
	bool OnRequestOnLineCountInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//�����������Ϣ
	bool On_DBR_UPDATA_MARQUEE(DWORD dwContextID, VOID * pData, WORD wDataSize);
#pragma endregion

#pragma region MDM_CLUB ����Ȧ(�汾2)
protected:
	//��ѯ����Ȧ�б�
	bool On_DBR_CL_CLUB_ALL_CLUB_INFO_LIST(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//��ѯָ������Ȧ�����б�
	bool On_DBR_CL_CLUB_ROOM_LIST(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//��ѯδ��Ա, �������Ȧ(���9��)
	bool On_DBR_CL_CLUB_RANDOM_CLUB_LIST(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//�����������Ȧ
	bool On_DBR_CL_CLUB_JOIN_CLUB(DWORD dwContextID, VOID * pData, WORD wDataSize);
		
	//��ɢ����Ȧ
	bool On_DBR_CL_CLUB_DISS_CLUB(DWORD dwContextID, VOID * pData, WORD wDataSize);
	
	//��������
	bool On_DBR_CL_CLUB_ROOM_SETTING(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//���󷿼�����
	bool On_DBR_CL_CLUB_ROOM_QUERY_SETTING(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//��ɢ��������
	bool On_DBR_CL_CLUB_ROOM_DISSOLVE(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//��ɢ��������
	bool On_DBR_CL_CLUB_TABLE_DISSOLVE(DWORD dwContextID, VOID * pData, WORD wDataSize);
	
	//����뿪���ֲ�����
	bool On_DBR_CL_CLUB_ROOM_USER_LEAVE(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//����Ȧ����
	bool On_DBR_CL_CLUB_NOTICE(DWORD dwContextID, VOID * pData, WORD wDataSize);
	
	//����Ȧ���
	bool On_DBR_CL_CLUB_MESSAGE(DWORD dwContextID, VOID * pData, WORD wDataSize);
	
	//���׷���
	bool On_DBR_CL_CLUB_CONTRIBUTE_FK(DWORD dwContextID, VOID * pData, WORD wDataSize);
	
	//����Ȧ����
	bool On_DBR_CL_CLUB_AUTO_AGREE(DWORD dwContextID, VOID * pData, WORD wDataSize);
	
	//�������˼�������Ȧ
	bool On_DBR_CL_CLUB_INVITE(DWORD dwContextID, VOID * pData, WORD wDataSize);
	
	//�������˼�������Ȧ
	bool On_DBR_CL_CLUB_INVITE_RESULT(DWORD dwContextID, VOID * pData, WORD wDataSize);
	
	//�������˼�������Ȧ
	bool On_DBR_CL_CLUB_INQUERY_LIST(DWORD dwContextID, VOID * pData, WORD wDataSize);
	
	//�������б�
	bool On_DBR_CL_CLUB_APPLICANT_LIST(DWORD dwContextID, VOID * pData, WORD wDataSize);
	
	//ְ������
	bool On_DBR_CL_CLUB_APPOINTMENT(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//����Ȧ����
	bool On_DBR_CL_CLUB_CHAT(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//��������
	bool On_DBR_CL_WORD_CHAT(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//ϵͳ����
	bool On_DBR_CL_SYSTEM_CHAT(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//˽������
	bool On_DBR_CL_SECRET_CHAT(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//����Ȧ�ö�
	bool On_DBR_CL_STICKY_POST(DWORD dwContextID, VOID * pData, WORD wDataSize);


	//����ս��ͳ��
	bool On_DBR_CL_CLUB_RECORD_LIST(DWORD dwContextID, VOID * pData, WORD wDataSize);
	
	//��������Ȧ
	bool On_DBR_CL_CLUB_CREATE_CLUB(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//������뷿��
	bool On_DBR_CL_CLUB_JOIN_ROOM(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//Ⱥ��|�����������Ϣ�Ĵ�(ͬ��|�ܾ�)
	bool On_DBR_CL_CLUB_APPLICANT_RESULT(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//�����Ա����
	bool On_DBR_CL_CLUB_MEMBER_MANAGER(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//�߳��˳�����
	bool On_DBR_CL_CLUB_QUIT(DWORD dwContextID, VOID * pData, WORD wDataSize);

#pragma endregion

#pragma region MDM_SHOP �̳ǵ���
protected:
	//��ѯ�̳�
	bool On_DBR_CL_SHOP_QUERY(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//��Ǯ�������
	bool On_DBR_CL_SHOP_MONEY(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//��ʯ�������
	bool On_DBR_CL_SHOP_DIAMOND(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//������Ʒ��ѯ
	bool On_DBR_CL_BAG_QUERY(DWORD dwContextID, VOID * pData, WORD wDataSize);

#pragma endregion
};

// byte����תΪ string  TODONOW ��ʱ�������ﴦ��
const CString toHexString(const byte * input, const int datasize);

// string תΪbyte����  TODONOW ��ʱ�������ﴦ��
int StrToBin(TCHAR* inWord, BYTE* OutBin, int source_len_begin, int source_len_end);

//////////////////////////////////////////////////////////////////////////////////

#endif