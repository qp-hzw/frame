#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#include "Stdafx.h"
#include "TableFrame.h"
#include "DataBasePacket.h"
#include "Player.h"
#include "PlayerManager.h"

//�󶨲���
struct tagBindParameter
{
	//��������
	DWORD							dwSocketID;							//�����ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ
	DWORD							dwActiveTime;						//����ʱ��
	//�û�����
	CPlayer *				        pCPlayer;					        //�û��ӿ�
};

//ϵͳ��Ϣ
struct tagSystemMessage
{
	DWORD							dwLastTime;						   //����ʱ��
	DBR_GR_SystemMessage            SystemMessage;                     //ϵͳ��Ϣ
};


//////////////////////////////////////////////////////////////////////////////////

//���ȹ���
class CAttemperEngineSink : public IAttemperEngineSink
{
#pragma region ��Ա����
	//����Ϣ
protected:
	tagBindParameter *				m_pNormalParameter;					//����Ϣ
	tagBindParameter *				m_pAndroidParameter;				//����Ϣ

	//������Ϣ
protected:
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//��������
	tagGameServiceOption *			m_pGameServiceOption;				//��������

#pragma endregion

#pragma region ��ʼ������
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

#pragma region ����ֹͣ�¼�(�ں�)
	//�첽�ӿ�
public:
	//�����¼�
	virtual bool OnAttemperEngineStart(IUnknownEx * pIUnknownEx);
	//ֹͣ�¼�
	virtual bool OnAttemperEngineConclude(IUnknownEx * pIUnknownEx);

#pragma endregion

#pragma region �¼��ܿ�����
	//�¼��ӿ�
public:
	//�����¼�
	virtual bool OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize);

	//�ں��¼�
public:
	//��ʱ���¼�
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM wBindParam);
	//���ݿ��¼�
	virtual bool OnEventDataBaseResult(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);
#pragma endregion

#pragma region Socketϵͳ����
	//socket::client
public:
	//�����¼�
	virtual bool OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode);
	//�ر��¼�
	virtual bool OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason);
	//��ȡ�¼�
	virtual bool OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize);

	//socket::server
public:
	//Ӧ���¼�
	virtual bool OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID);
	//�ر��¼�
	virtual bool OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID);
	//��ȡ�¼� -- return falseʱ�򽫻�ر�socket����
	virtual bool OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//����Socket�¼� ��ȡ�¼� ��������
protected:
    //��¼���� MDM_LOGON
	bool OnTCPNetworkMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�û����� MDM_USER
	bool OnTCPNetworkMainUser(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ܴ��� MDM_FRAME
	bool OnTCPNetworkMainFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
    //��Ϸ���� MDM_GAME
	bool OnTCPNetworkMainGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�������� MDM_MATCH
	bool OnTCPNetworkMainMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//����Socket�¼� ��ȡ�¼� ��������
protected:
	//ע���¼�
	bool OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��ת����
	bool OnTCPSocketMainTransfer(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//�����¼�
	bool OnTCPSocketMainUserCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize);

#pragma endregion

#pragma region �û��ӿ�

	//�û��ӿ�
public:
	//�û�״̬
	virtual bool OnEventUserItemStatus(CPlayer * pCPlayer, WORD wOldTableID=INVALID_TABLE, WORD wOldChairID=INVALID_CHAIR);
	//�û�Ȩ��
	virtual bool OnEventUserItemRight(CPlayer *pCPlayer, DWORD dwAddRight, DWORD dwRemoveRight,bool bGameRight=true);
	//�����û��Ƹ���Ϣ
	virtual bool OnEventModifyUserTreasure(CPlayer *pCPlayer, DWORD dwTableID, BYTE byTableMode, BYTE byRound, SCORE lUserTreasuse, BYTE byWin);
#pragma endregion

#pragma region ��¼ģ�� MDM_LOGON
	//��¼ģ�� MDM_LOGON
protected:
	//I D ��¼
	bool On_SUB_CG_Logon_UserID(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//ID��¼����
	bool On_CMD_GC_Logon_UserID(DWORD dwContextID, VOID * pData, WORD wDataSize);

    //��¼ģ�� ��������
protected:
	//ID��¼�ɹ��������û���Ϊ�˸�pCPlayer��ֵ������ȥ˫ָ�룬pCPlayer����ȥ�ľ���NULL��
	void ActiveUserItem(CPlayer **pCPlayer, DWORD dwContextID, 
		tagBindParameter *pBindParameter, STR_DBO_CG_LOGON_USERID *pDBOLogon, WORD wBindIndex);

	//�û���¼
	VOID OnEventUserLogon(CPlayer * pCPlayer, bool bAlreadyOnLine);
	//�û��ǳ�
	VOID OnEventUserLogout(CPlayer * pCPlayer, DWORD dwLeaveReason);

	//�л�����
	bool SwitchUserItemConnect(CPlayer *pCPlayer, TCHAR szMachineID[LEN_MACHINE_ID], WORD wTargetIndex, 
								const double &dLongitude, const double &dLatitude, 
								BYTE cbDeviceType=DEVICE_TYPE_PC, WORD wBehaviorFlags=0, WORD wPageTableCount=0);
#pragma endregion

#pragma region  �û����� MDM_USER
	// �û����� MDM_USER
protected:
	//��������
	bool On_SUB_CG_USER_CREATE_ROOM(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��������(���� ����Ȧ)
	bool On_SUB_CG_USER_CREATE_TABLE(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�������� --��Ҫ����
	//��Ҫ���ݿ�У�� 1. ����ķ����� �Ƿ��㹻
	//				 2. �ǹ����Ա���ܽ��빤������
	bool On_SUB_User_JoinFkRoom(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�������� -- ����Ҫ����
	//��Ҫ���ݿ��жϲ����ؿɼ�������Ӻ�
	bool On_SUB_CG_USER_JOIN_TABLE_NO_PASS(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	
	//�������� -- ���������ҳ�����
	//��Ҫ���ݿ��жϲ����ؿɼ�������Ӻ�, ���û����ᴴ��һ���µ�����
	bool On_SUB_CG_USER_JOIN_GOLD_HALL_ROOM(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//���뷿�� -- ���������ҳ����� ����
	bool On_CMD_GC_USER_JOIN_TABLE_HALL_GOLD( DWORD dwContextID, VOID * pData, WORD wDataSize );
	

	//��������
	bool On_SUB_User_ReconnectRoom(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�����ҷ�
	bool On_SUB_User_JoinGoldRoom(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��ɢ����
	bool On_SUB_User_DismissRoom(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��������Ȧ����
	bool On_Sub_CG_User_CreateGroupRoom(VOID *pData, WORD wDataSize, DWORD dwSocketID);
	//��������Ȧ���䷵��
	bool On_CMD_GC_User_CreateGroupRoom(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//��������Ȧ����
	bool On_Sub_CG_User_JoinGroupRoom(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��������Ȧ���䷵��
	bool On_CMD_GC_User_JoinGroupRoom(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//�޸��û��Ƹ���Ϣ
	bool On_CMD_GC_User_ModifyUserTreasure(DWORD dwContextID, VOID * pData, WORD wDataSize);


	//�û�����
	bool On_SUB_CG_User_SitDown(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�û�����
	bool On_SUB_CG_User_StandUp(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�����û�
	bool On_SUB_CG_User_InviteUser(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�ܾ��������
	bool On_SUB_CG_User_RefuseSit(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�߳����û�
	bool On_SUB_CG_User_KickUser(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//�������λ��
	bool OnTCPNetworkSubUserChairReq(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//���󷿼��б�
	bool OnTCPNetworkSubGetTableList(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//����齱
	bool OnTcpNetworkQueryLottery(VOID * pData, WORD wDataSize, DWORD dwSocketID);

    //�齱���
	bool OnDBLotteryResult( DWORD dwContextID, VOID * pData, WORD wDataSize );
	
	//Club����Ȧ��������
	bool On_CMD_CG_CLUB_CREATE_TABLE( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//���뷿��  ����Ҫ����
	bool On_CMD_GC_USER_JOIN_TABLE_NO_PASS( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//���뷿��  ��Ҫ���� -- ���ݿ�У��
	bool On_CMD_GC_JOIN_TABLE( DWORD dwContextID, VOID * pData, WORD wDataSize );
	
#pragma endregion

#pragma region ������� MDM_FRAME
	//������� MDM_FRAME
protected:
	//�����ɢ����
	bool On_SUB_Common_ApplyDismissRoom(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//����Ƿ��ɢ����
	bool On_SUB_Common_VoteDismissRoom(VOID * pData, WORD wDataSize, DWORD dwSocketID);
#pragma endregion

#pragma region ������
    //������Ϣ
public:
	//�������Ƿ����
	virtual bool IsControl( SCORE expectStock );
	//�޸ĵ�ǰ���ֵ
	virtual bool SetStockScore(SCORE currentStock);
	//���ÿ��ֵ
	virtual SCORE GetChangeStockScore();

	//�����¼�
protected:
	//������Ϣ
	bool OnDBGameAndroidInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//���ض�������
	bool On_DBO_GR_LOAD_OFFLINE(DWORD dwContextID, VOID * pData, WORD wDataSize);
	
	//���ͺ���
protected:
	//�����û���Ϣ
	bool SendUserInfoPacket(CPlayer * pCPlayer, DWORD dwSocketID);

	//��������
protected:
	//����ʧ��
	bool SendRequestFailure(CPlayer * pCPlayer, LPCTSTR pszDescribe, LONG lErrorCode);

	//��������
public:
	//���û�
	CPlayer * GetBindUserItem(WORD wBindIndex);
	//�󶨲���
	tagBindParameter * GetBindParameter(WORD wBindIndex);
	//��������
	WORD GetPropertyType(WORD wPropertyIndex);
	//��ȡ���������û� 
	CPlayer * GetVirtualUserItem();

	//��������
protected:
	//���ò���
	void SetMobileUserParameter(CPlayer * pCPlayer,BYTE cbDeviceType,WORD wBehaviorFlags,WORD wPageTableCount);
	//Ⱥ������
	virtual bool SendDataBatchToMobileUser(WORD wCmdTable, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//ֹͣ����
	void StopService();
	//֪ͨ����ֹͣ
	void SendServiceStopped();
#pragma endregion

#pragma region  �������� && �������� && ��������
    //�������� && ��������
protected: 
	//��鴴�����ӵ���Ʊ
	bool CheckCreateTableTicket(tagTableRule * pCfg, CPlayer *pCPlayer);

    //������ͨ����(1. Ϊ�Լ���������  2.Ϊ���˴�������)
	bool CreateTableNormal(tagTableRule * pCfg, CPlayer *pCPlayer, STR_SUB_CG_USER_CREATE_ROOM* pCreateRoom);
	//��������Ȧ����
	bool CreateRoomClub(tagTableRule * pCfg, CPlayer *pCPlayer, STR_SUB_CG_USER_CREATE_ROOM* pCreateRoom);
	//��������Ȧ����(�ȶ�ȡ���ݿ��еķ������)
	bool CreateTableClub(STR_DBO_GC_CLUB_CREATE_TABLE* , CPlayer *pCPlayer);

	//�������� ��Ҵ�������  -- ��ҽ����Ҵ���ʱ��, ���û���ҵ����õ�����,�ͻᴴ������
	bool CreateTableHallGold(STR_DBO_CG_USER_JOIN_TABLE_HALL_GOLD* , CPlayer *pCPlayer);

	//�������� ���ֲ�����  -- ��ҽ�����ֲ�����ʱ��, ���û���ҵ����õ�����,�ͻᴴ������
	bool CreateTableAutoClub(STR_DBO_CG_USER_JOIN_TABLE_NO_PASS* , CPlayer *pCPlayer);



    //�������� && �������� ��������
protected: 
    //Ϊ�Լ���������
	bool HandleCreateTable(CTableFrame *pCurTableFrame, CPlayer *pCPlayer, tagTableRule *pCfg);
	//Ϊ���˴�������
	void HandleCreateTableForOthers(CTableFrame *pCurTableFrame, CPlayer *pCPlayer, tagTableRule *pCfg);

	//����Ȧ������Ϣ��Ҫд�����ݿ�
	bool WriteClubRoomToDB(STR_DBR_CLUB_ROOM_INFO* pTableInfo);

    //Ѱ����һ��������
	CTableFrame* GetNextEmptyTable();
    //��������ID�������뷿������
	DWORD GenerateTablePassword();

    //��������Ѱ��ָ������
	CTableFrame* GetDesignatedTable(const DWORD &dwPassword);

    //���ҽ�ҷ�������
	CTableFrame* GetGlodRoomEmptyChair(WORD &wChairID);

	//��������
protected:
	 //���������ӵ���Ʊ
	bool CheckJoinTableTicket(tagTableRule * pCfg, CPlayer *pCPlayer);

#pragma endregion 

#pragma region DB�¼�֪ͨ
	//�����˿���
public:
	//��������˿���
	bool AddOtherRoomInfo(DBR_GR_UpdateTableInfo* pTableInfo);
	//�����˿�������  && club ��Ϸ��ʼ
	virtual void ConcludeTable(DWORD dwTableID);
	//�����˿�����ʼ && clubС�ֽ���
	virtual void StartTable(DWORD dwTableID);
	//�û����������˿��� && club���ӽ�ɢ
	virtual void JoinTable(DWORD dwTableID, DWORD dwUserID);
	//�û��뿪�����˿���
	virtual void LeaveTable(DWORD dwTableID, DWORD dwUserID);


	//Club����Ȧ2  &&  ��Ҵ�����1.�û���Ϣ  2.������Ϣ��
public:
	//�û�����
	virtual void ClubPlayerSitDown(DWORD dwTableID, DWORD dwUserID, BYTE byChairID, BYTE byClubOrHallGold);
	//�û�����
	virtual void ClubPlayerSitUp(DWORD dwTableID, DWORD dwUserID, BYTE byChairID, BYTE byClubOrHallGold);
	//���һ���û�����
	virtual void ClubLastPlayerSitUp(DWORD dwTableID, DWORD dwUserID, BYTE byChairID, BYTE byClubOrHallGold);
	//�û�����
	virtual void ClubPlayerOffline(DWORD dwTableID, DWORD dwUserID, BYTE byChairID, BYTE byClubOrHallGold){}

	//�������� -- ���ֲ�
	virtual void ClubTableCreate(DWORD dwClubRoomID, DWORD dwUserID, DWORD dwTableID, DWORD dwLockState);
	//�������� -- ��Ҵ���
	void HallTableCreate(DWORD dwUserID, DWORD dwKindID,BYTE byGameType, DWORD dwTableID);
	//���俪ʼ��Ϸ
	virtual void ClubTableStart(DWORD dwTableID, BYTE byClubOrHallGold);
	//������������
	virtual void ClubTableMax(DWORD dwTableID, BYTE byClubOrHallGold);
	//�������� ����
	virtual void ClubTableNotMax(DWORD dwTableID, BYTE byClubOrHallGold);
	//С�ֽ�����Ϸ -- ���ֲ�
	virtual void ClubTableXJ(DWORD dwTableID);
	//��ֽ�����Ϸ -- ���ֲ�
	virtual void ClubTableDJ(DWORD dwTableID);

#pragma endregion

};

extern CAttemperEngineSink * g_AttemperEngineSink;

//////////////////////////////////////////////////////////////////////////////////





#endif