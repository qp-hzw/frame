#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#include "Stdafx.h"
#include "TableFrame.h"
#include "ServerListManager.h"
#include "DataBasePacket.h"
#include "ServerUserManager.h"

//�󶨲���
struct tagBindParameter
{
	//��������
	DWORD							dwSocketID;							//�����ʶ
	DWORD							dwClientAddr;						//���ӵ�ַ
	DWORD							dwActiveTime;						//����ʱ��
	//�û�����
	IServerUserItem *				pIServerUserItem;					//�û��ӿ�
};

//ϵͳ��Ϣ
struct tagSystemMessage
{
	DWORD							dwLastTime;						   //����ʱ��
	DBR_GR_SystemMessage            SystemMessage;                     //ϵͳ��Ϣ
};


//����˵��
typedef CWHArray<CTableFrame *>		CTableFrameArray;					//��������
typedef CList<IServerUserItem *>    CWaitDistributeList;                //�ȴ�����

//////////////////////////////////////////////////////////////////////////////////

//���ȹ���
class CAttemperEngineSink : public IAttemperEngineSink, public IMainServiceFrame,
	public IServerUserItemSink
{
	//��Ԫ����
	friend class CServiceUnits;

#pragma region ��Ա����
	//״̬����
protected:
	bool							m_bCollectUser;						//���ܱ�־
	bool							m_bNeekCorrespond;					//Э����־
	bool							m_bStopping;

	//����Ϣ
protected:
	tagBindParameter *				m_pNormalParameter;					//����Ϣ
	tagBindParameter *				m_pAndroidParameter;				//����Ϣ

	//������Ϣ
protected:
	tagGameParameter *				m_pGameParameter;					//���ò���
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//��������
	tagGameServiceOption *			m_pGameServiceOption;				//��������

	//�������
protected:
	CTableFrameArray				m_TableFrameArray;					//��������
	CServerUserManager				m_ServerUserManager;				//�û����� �ڼ����û���ʱ��ֵ
	CWaitDistributeList             m_WaitDistributeList;               //�ȴ�����

	//�������ֵ
public:
	SCORE							m_ControlValueForRoom;
	SCORE							m_ControlValueForRoomOrgi;

#pragma endregion

#pragma region ��ʼ������
	//��������
public:
	//���캯��
	CAttemperEngineSink();
	//��������
	virtual ~CAttemperEngineSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { return; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);
#pragma endregion

#pragma region ����ֹͣ�¼�(�ں�)
	//�첽�ӿ�
public:
	//�����¼�
	virtual bool OnAttemperEngineStart(IUnknownEx * pIUnknownEx);
	//ֹͣ�¼�
	virtual bool OnAttemperEngineConclude(IUnknownEx * pIUnknownEx);

    //�첽�ӿڸ�������
protected:
    //��������
	bool InitTableFrameArray();

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
	//����Socket�¼� (��Э����)
public:
	//�����¼�
	virtual bool OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode);
	//�ر��¼�
	virtual bool OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason);
	//��ȡ�¼�
	virtual bool OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize);

	//����Socket�¼� (��client && web��̨)
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

#pragma region ��Ϣ���ͽӿ�
    //����ӿ�
public:
	//��������
	virtual bool SendData(BYTE cbSendMask, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��������
	virtual bool SendData(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��������
	virtual bool SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);


	//��Ϣ�ӿ�
public:
	//������Ϣ
	virtual bool SendRoomMessage(LPCTSTR lpszMessage, WORD wType);
	//��Ϸ��Ϣ
	virtual bool SendGameMessage(LPCTSTR lpszMessage, WORD wType);
	//������Ϣ
	virtual bool SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType);
	//��Ϸ��Ϣ
	virtual bool SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType);
	//������Ϣ
	virtual bool SendRoomMessage(DWORD dwSocketID, LPCTSTR lpszMessage, WORD wType, bool bAndroid);
	//���͵�������������Ϸ ��ǰҪ���ؿ���( ���ᷢ�͸��������½�����������. ���ᷢ�͸����к���Ϸ�����������)
	virtual bool SendMessageLobbyAndAllGame(LPCTSTR lpszMessage, WORD wType ,WORD MsgID);
#pragma endregion

#pragma region �û��ӿ�
	//���ܽӿ�
public:
	//�����û�
	virtual bool InsertDistribute(IServerUserItem * pIServerUserItem);
	//�����û�
	virtual bool InsertWaitDistribute(IServerUserItem * pIServerUserItem);
	//ɾ���û�
	virtual bool DeleteWaitDistribute(IServerUserItem * pIServerUserItem);

	//�û��ӿ�
public:
	//�û�״̬
	virtual bool OnEventUserItemStatus(IServerUserItem * pIServerUserItem, WORD wOldTableID=INVALID_TABLE, WORD wOldChairID=INVALID_CHAIR);
	//�û�Ȩ��
	virtual bool OnEventUserItemRight(IServerUserItem *pIServerUserItem, DWORD dwAddRight, DWORD dwRemoveRight,bool bGameRight=true);
	//�����û��Ƹ���Ϣ
	virtual bool OnEventModifyUserTreasure(IServerUserItem *pIServerUserItem, DWORD dwTableID, BYTE byTableMode, BYTE byRound, SCORE lUserTreasuse, BYTE byWin);
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
	//ID��¼�ɹ��������û���Ϊ�˸�pIServerUserItem��ֵ������ȥ˫ָ�룬pIServerUserItem����ȥ�ľ���NULL��
	void ActiveUserItem(IServerUserItem **pIServerUserItem, DWORD dwContextID, 
		tagBindParameter *pBindParameter, STR_DBO_CG_LOGON_USERID *pDBOLogon, WORD wBindIndex);

	//�û���¼
	VOID OnEventUserLogon(IServerUserItem * pIServerUserItem, bool bAlreadyOnLine);
	//�û��ǳ�
	VOID OnEventUserLogout(IServerUserItem * pIServerUserItem, DWORD dwLeaveReason);

	//�л�����
	bool SwitchUserItemConnect(IServerUserItem *pIServerUserItem, TCHAR szMachineID[LEN_MACHINE_ID], WORD wTargetIndex, 
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


	//�û��Թ�
	bool On_SUB_CG_User_Lookon(VOID * pData, WORD wDataSize, DWORD dwSocketID);
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
	//��ʼ���������ֵyang
	virtual void InitRoomControlVal( SCORE val );
	//����Ƿ񱣴淿�����ֵyang
	virtual SCORE CheckAndSaveRoomControlValue();
	virtual SCORE GetRoomControl(){ return m_ControlValueForRoom ;}
	virtual void SetRoomContorl(SCORE val ){ m_ControlValueForRoom = val;}

	//�����¼�
protected:
	//������Ϣ
	bool OnDBGameAndroidInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//���ض�������
	bool On_DBO_GR_LOAD_OFFLINE(DWORD dwContextID, VOID * pData, WORD wDataSize);
	

	//�޸ķ����Ѷȳɹ�
	bool OnDBRoomLevelModify(DWORD dwContextID, VOID * pData, WORD wDataSize);
	bool OnDBRoomControlValModify(DWORD dwContextID, VOID * pData, WORD wDataSize);


	//���ͺ���
protected:
	//�����û���Ϣ
	bool SendUserInfoPacket(IServerUserItem * pIServerUserItem, DWORD dwSocketID);

	//��������
protected:
	//����ʧ��
	bool SendRequestFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszDescribe, LONG lErrorCode);

	//��������
public:
	//���û�
	IServerUserItem * GetBindUserItem(WORD wBindIndex);
	//�󶨲���
	tagBindParameter * GetBindParameter(WORD wBindIndex);
	//��������
	WORD GetPropertyType(WORD wPropertyIndex);
	//��ȡ���������û� 
	IServerUserItem * GetVirtualUserItem();

	//��������
protected:
	//���û���
	bool InitAndroidUser();
	//���ò���
	void SetMobileUserParameter(IServerUserItem * pIServerUserItem,BYTE cbDeviceType,WORD wBehaviorFlags,WORD wPageTableCount);
	//Ⱥ������
	virtual bool SendDataBatchToMobileUser(WORD wCmdTable, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//ֹͣ����
	void StopService();
	//֪ͨ����ֹͣ
	void SendServiceStopped();
	//���������
	bool DistributeAndroid();
	bool Distribute(CTableFrame* pTableFrame, bool bChoiceAndroid, WORD wWantUserCount, WORD& wHandleCount );
#pragma endregion

#pragma region  �������� && �������� && ��������
    //�������� && ��������
protected: 
	//��鴴�����ӵ���Ʊ
	bool CheckCreateTableTicket(tagTableRule * pCfg, IServerUserItem *pIServerUserItem);

    //������ͨ����(1. Ϊ�Լ���������  2.Ϊ���˴�������)
	bool CreateTableNormal(tagTableRule * pCfg, IServerUserItem *pIServerUserItem, STR_SUB_CG_USER_CREATE_ROOM* pCreateRoom);
	//��������Ȧ����
	bool CreateRoomClub(tagTableRule * pCfg, IServerUserItem *pIServerUserItem, STR_SUB_CG_USER_CREATE_ROOM* pCreateRoom);
	//��������Ȧ����(�ȶ�ȡ���ݿ��еķ������)
	bool CreateTableClub(STR_DBO_GC_CLUB_CREATE_TABLE* , IServerUserItem *pIServerUserItem);

	//�������� ��Ҵ�������  -- ��ҽ����Ҵ���ʱ��, ���û���ҵ����õ�����,�ͻᴴ������
	bool CreateTableHallGold(STR_DBO_CG_USER_JOIN_TABLE_HALL_GOLD* , IServerUserItem *pIServerUserItem);

	//�������� ���ֲ�����  -- ��ҽ�����ֲ�����ʱ��, ���û���ҵ����õ�����,�ͻᴴ������
	bool CreateTableAutoClub(STR_DBO_CG_USER_JOIN_TABLE_NO_PASS* , IServerUserItem *pIServerUserItem);



    //�������� && �������� ��������
protected: 
    //Ϊ�Լ���������
	bool HandleCreateTable(CTableFrame *pCurTableFrame, IServerUserItem *pIServerUserItem, tagTableRule *pCfg);
	//Ϊ���˴�������
	void HandleCreateTableForOthers(CTableFrame *pCurTableFrame, IServerUserItem *pIServerUserItem, tagTableRule *pCfg);

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
	bool CheckJoinTableTicket(tagTableRule * pCfg, IServerUserItem *pIServerUserItem);

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