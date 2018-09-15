#ifndef TABLE_FRAME_HEAD_FILE
#define TABLE_FRAME_HEAD_FILE

#include "Stdafx.h"
#pragma pack(1)
//////////////////////////////////////////////////////////////////////////////////
#define GAME_CONCLUDE_CONTINUE  0xFF //��ֽ���������
#define GAME_CONCLUDE_NORMAL    0xFE //��������

//���鶨��
typedef IServerUserItem * CTableUserItemArray[MAX_CHAIR];				//��Ϸ����
typedef CWHArray<IServerUserItem *> CLookonUserItemArray;				//�Թ�����
typedef CWHArray<tagGameScoreRecord *> CGameScoreRecordArray;			//��¼����
interface IMatchTableFrameNotify;

//����Ϸ�������
struct tagTableSubGameRule
{
	BYTE SubGameRule[LEN_PRIVATE_TABLE_RULE/2];						//����Ϸ�������
};
//////////////////////////////////////////////////////////////////////////////////

//���ӿ��
class CTableFrame : public ITableFrame
{

#pragma region ��Ա����
	//��Ϸ����
protected:
	WORD							m_wTableID;							//���Ӻ���		��0��ʼ�������ӳ�ʼ����ʱ���Ѿ���ֵ
	WORD							m_wChairCount;						//������Ŀ
	BYTE							m_cbStartMode;						//��ʼģʽ
	WORD							m_wUserCount;						//�û���Ŀ
	DWORD							m_dwTableOwner;						//�����û�����һ�����µ���ң�
	DWORD							m_dwCreateTableUser;				//���������û�
	DWORD							m_dwGroupID;						//��������ȦID
	BYTE							m_cbTableMode;						//������Ϸģʽ

	tagTableRule					m_tagTableRule;						//ͨ�÷������(client��������˴�����)
	tagTableSubGameRule				m_tagTableSubGameRule;				//����Ϸ���з������(client��������Ϸ��)

	//״̬����
protected:
	bool							m_bGameStarted;						//��Ϸ��ʼ��־
	bool							m_bDrawStarted;						//��Ϸ��־
	bool							m_bTableStarted;					//��Ϸ��־

	//״̬����
protected:
	bool							m_bAllowLookon[MAX_CHAIR];			//�Թ۱�־
	SCORE							m_lFrozenedScore[MAX_CHAIR];		//�������

	//��Ϸ����
protected:
	BYTE							m_cbGameStatus;						//��Ϸ״̬

	//�����ɢ���
	bool							m_bAgree[MAX_CHAIR];					//ͬ���ɢ
	bool							m_bResponseDismiss[MAX_CHAIR];			//��Ӧ��ɢ
	bool							m_bUnderDissState;						//�����Ƿ��ڽ�ɢ״̬

	//ʱ�����
protected:
	DWORD							m_dwDrawStartTime;					//��ʼʱ��
	SYSTEMTIME						m_SystemTimeStart;					//��ʼʱ��
	WORD                            m_wDrawCount;                       //��Ϸ����

	//��̬����
protected:	
	TCHAR							m_szEnterPassword[LEN_PASSWORD];	//��������		TODO ���������ʲô��
	DWORD							m_dwPassword;						//6λ������������


	//�û�����
protected:
	CTableUserItemArray				m_TableUserItemArray;				//��Ϸ�û�
	CLookonUserItemArray			m_LookonUserItemArray;				//�Թ��û�

	//����ӿ�
protected:
	ITimerEngine *					m_pITimerEngine;					//ʱ������
	ITableFrameSink	*				m_pITableFrameSink;					//���ӽӿ�
	IMainServiceFrame *				m_pIMainServiceFrame;				//����ӿ�
	IAndroidUserManager *			m_pIAndroidUserManager;				//�����ӿ�

	//��չ�ӿ�
protected:
	ITableUserAction *				m_pITableUserAction;				//�����ӿ�
	ITableUserRequest *				m_pITableUserRequest;				//����ӿ�
	ITableUserAction *				m_pIMatchTableAction;				//�����ӿ�(������)

	ITCPSocketService *				m_pITCPSocketService;				//�������


	//���ݽӿ�
protected:
	IDataBaseEngine *				m_pIKernelDataBaseEngine;			//�ں�����
	IDataBaseEngine *				m_pIRecordDataBaseEngine;			//��¼����

	//������Ϣ
protected:
	tagGameParameter *				m_pGameParameter;					//���ò���
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//��������
	tagGameServiceOption *			m_pGameServiceOption;				//��������

	//��Ϸ��¼
protected:
	CGameScoreRecordArray			m_GameScoreRecordActive;			//��Ϸ��¼
	static CGameScoreRecordArray	m_GameScoreRecordBuffer;			//��Ϸ��¼

	//�����ӿ�
protected:
	IGameMatchSink					* m_pIGameMatchSink;				//�����ӿ�

	//
protected:
	bool							m_ConrtolValueEnabled;				//�Ƿ���ܷ������ֵӰ��
	static SCORE					m_ControlValueForRoom;				//�������ֵyang
	static SCORE					m_ControlValueForRoomOrgi;			//�������ֵyang
#pragma endregion

#pragma region ��ʼ������
	//��������
public:
	//���캯��
	CTableFrame();
	//��������
	virtual ~CTableFrame();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);
#pragma endregion

#pragma region ���������������ȡ
public:
	//ServerID
	virtual DWORD GetServerID() { return m_pGameServiceOption->dwServerID; } 
	//���ӱ��
	virtual WORD GetTableID() { return m_wTableID; }
	//��Ϸ����
	virtual WORD GetChairCount() { return m_wChairCount; }
	//��λ����Ŀ
	virtual WORD GetNullChairCount(){return m_wChairCount - m_wUserCount;}

	//��������
	virtual tagGameServiceAttrib * GetGameServiceAttrib() { return m_pGameServiceAttrib; }
	//��������
	virtual tagGameServiceOption * GetGameServiceOption() { return m_pGameServiceOption; }

	//��ʼģʽ
	virtual BYTE GetStartMode() { return m_cbStartMode; }
	//��ʼģʽ
	virtual VOID SetStartMode(BYTE cbStartMode) { m_cbStartMode=cbStartMode; }

	//��ȡ״̬
	virtual BYTE GetGameStatus() { return m_cbGameStatus; }
	//����״̬
	virtual VOID SetGameStatus(BYTE cbGameStatus) { m_cbGameStatus=cbGameStatus; }

	//��Ϸ״̬
	virtual bool IsGameStarted() { return m_bGameStarted; }
	//��Ϸ״̬
	virtual bool IsDrawStarted() { return m_bDrawStarted; }
	//��Ϸ״̬
	virtual bool IsTableStarted() { return m_bTableStarted; }
	//����״̬
	virtual bool IsTableLocked() { return (m_szEnterPassword[0]!=0); }

	//������������һ�����µ���
	virtual void SetTableOwner(DWORD dwUserID);
	//�����������һ�����µ���
	virtual DWORD GetTableOwner();

	//��ȡ����ģʽ
	virtual BYTE GetTableMode(){return m_cbTableMode;};
	//���÷���ģʽ
	virtual void SetTableMode(BYTE mode){ m_cbTableMode = mode; };

	//���ô������ӵ��û�
	virtual void SetCreateTableUser(DWORD dwUserID){m_dwCreateTableUser = dwUserID;};
	virtual DWORD GetCreateTableUser(){return m_dwCreateTableUser;};

	//��ȡ�������û�Ȧ����
	virtual void SetGroupID(DWORD dwGroupID){m_dwGroupID = dwGroupID;};
	virtual DWORD GetGroupID(){return m_dwGroupID;};

	//��ȡ����ͨ�ù���
	virtual VOID * GetCustomRule() { return &m_tagTableRule;};
	//���÷���ͨ�ù���
	virtual void SetCommonRule(tagTableRule* pRule)
	{
		memcpy(&m_tagTableRule, pRule, sizeof(m_tagTableRule));
		m_wChairCount = pRule->PlayerCount;
		SetTableMode(pRule->GameMode);
	};

	//��ȡ����Ϸ���з������
	virtual VOID * GetSubGameRule() { return &m_tagTableSubGameRule;};
	//��������Ϸ���з������
	virtual void SetSubGameRule(tagTableSubGameRule* pRule)
	{
		memcpy(&m_tagTableSubGameRule, pRule, sizeof(m_tagTableSubGameRule));
	};
	
	//��ȡ����
	virtual DWORD GetPassword(){ return m_dwPassword; }
	//��������
	virtual void SetPassword(DWORD dwPassword){ m_dwPassword=dwPassword; }

	//���÷����Զ���ɢʱ�� added by lizhihu
	virtual void SetTableAutoDismiss(DWORD dwMinutes = 1);
#pragma endregion

#pragma region ����״̬����
	//������ã�������Ϸ�޹�
public:
	//��ʼ��Ϸ
	virtual bool StartGame();
	//��ɢ��Ϸ
	virtual bool DismissGame();
	//��������
	virtual bool ConcludeTable();
#pragma endregion

#pragma region ������Ϸ��������
	//������Ϸ��������
public:
	//С�ֽ���������
	virtual bool HandleXJGameEnd(BYTE cbCurGameCount, BYTE cbGameMode, SCORE *lGameScore);
	//��ֽ���������
	virtual bool ConcludeGame(BYTE cbGameStatus);

	//д��¼���¼(ս���ط�) ���� С����,���ݺͳ���
	bool WriteRecordInfo(WORD wXJCount,TCHAR strScore[], VOID* pData, DWORD dwDataSize);

	//TODONOW �´ε���ɾ��  ��ѯ�޶�
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem);

	
	//С�ִ�ֽ�����������
protected:	
	//ÿ����Ϸ������, �жϲ��۳��û���Ʊ
	bool XJGameTickets(BYTE byTableMode, BYTE byRound);
	//ÿ����Ϸ�����󣬸����û��Ƹ���Ϣ
	bool XJModifyUserTreasure(BYTE byTableMode, BYTE byRound, SCORE *lGameScore, tagTableRule *pCfg);	
	//ÿ����Ϸ������, ����û��Ƹ��Ƿ���Լ�����Ϸ
	bool CheckUserLeave(BYTE byTableMode, IServerUserItem* pIServerUserItem, tagTableRule* pCfg);
	//ÿ����Ϸ�����󣬸�����Ϸ����״̬
	bool XJUpdateGameTaskStatus(const BYTE &cbTableMode, const BYTE &cbCurGameCount);
#pragma endregion

#pragma region ��ʱ��
	//ʱ��ӿ�
public:
	//����ʱ��
	virtual bool SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter);
	//ɾ��ʱ��
	virtual bool KillGameTimer(DWORD dwTimerID);
	//��ȡ��ʱ��ʣ�������
	virtual DWORD GetTimerLeftTickCount(DWORD dwTimerID);

#pragma endregion

#pragma region ��Ϣ���ͺ���
	//��Ϸ�û�
public:
	//��������
	virtual bool SendTableData(WORD wChairID, WORD wSubCmdID);
	//��������
	virtual bool SendTableData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize,WORD wMainCmdID=MDM_GF_GAME);

	//���ͻ������û�
public:
	//��������
	virtual bool SendAndroidUserData(WORD wChairID, WORD wSubCmdID);
	//��������
	virtual bool SendAndroidUserData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize,WORD wMainCmdID=MDM_GF_GAME);

	//�Թ��û�
public:
	//��������
	virtual bool SendLookonData(WORD wChairID, WORD wSubCmdID);
	//��������
	virtual bool SendLookonData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//�����û�
public:
	//��������
	virtual bool SendUserItemData(IServerUserItem * pIServerUserItem, WORD wSubCmdID);
	//��������
	virtual bool SendUserItemData(IServerUserItem * pIServerUserItem, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//ϵͳ��Ϣ
public:
	//������Ϣ
	virtual bool SendGameMessage(LPCTSTR lpszMessage, WORD wType);
	//��Ϸ��Ϣ
	virtual bool SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType);
	//������Ϣ
	virtual bool SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType);
#pragma endregion

#pragma region �û��ӿ�
	//�û��ӿ�
public:
	//Ѱ���û� -- �Թ��û� +��Ϸ�û�
	virtual IServerUserItem * SearchUserItem(DWORD dwUserID);
	//��Ϸ�û�
	virtual IServerUserItem * GetTableUserItem(WORD wChairID);
	//�Թ��û�
	virtual IServerUserItem * EnumLookonUserItem(WORD wEnumIndex);

	//��������
public:
	//��������
	virtual bool PerformStandUpAction(IServerUserItem * pIServerUserItem);
	//�Թ۶���
	virtual bool PerformLookonAction(WORD wChairID, IServerUserItem * pIServerUserItem);
	//���¶���		ֻ�д����ͼ��뷿��ʱ���û�����ʱ��ҪУ��GPS���ʼӴ��ֶ�
	virtual bool PerformSitDownAction(WORD wChairID, IServerUserItem * pIServerUserItem, LPCTSTR lpszPassword=NULL, bool bCheckUserGPS = false);
#pragma endregion

	//���ܽӿ�
public:
	//���ͳ���
	virtual bool SendGameScene(IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);

#pragma region �����ӿ�
	//�����ӿ�
public:
	virtual bool SetMatchInterface(IUnknownEx * pIUnknownEx);
	//��ȡ�������ӽӿ�
	virtual IGameMatchSink* GetGameMatchSink(){return m_pIGameMatchSink;}
#pragma endregion

	//���ܺ���
public:
	//��ȡ��λ
	WORD GetNullChairID();
	//�����λ
	WORD GetRandNullChairID();
	//�û���Ŀ
	WORD GetSitUserCount();
	//�Թ���Ŀ
	WORD GetLookonUserCount();
	//�����Ƿ��ڽ�ɢ״̬
	bool GetDismissState();
	//���÷��䴦�ڽ�ɢ״̬
	void SetDismissState(bool bState);

	//���ܺ���
public:
	//�û�״��
	WORD GetTableUserInfo(tagTableUserInfo & TableUserInfo);
	//��������
	bool InitializationFrame(WORD wTableID, tagTableFrameParameter & TableFrameParameter);

	//�û��¼�
public:
	//�����¼�
	bool OnEventUserOffLine(IServerUserItem * pIServerUserItem);
	//�����¼�
	bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);

	//ͨ���¼�
public:
	//�����ɢ����
	bool OnEventApplyDismissRoom(WORD wChairID, bool bAgree);
	//����Ƿ��ɢ����
	bool OnEventVoteDismissRoom(WORD wChairID,bool bAgree);

	//ϵͳ�¼�
public:
	//ʱ���¼�
	bool OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//��Ϸ�¼�
	bool OnEventSocketGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//����¼�
	bool OnEventSocketFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//��������
public:
	//����״̬
	bool SendTableStatus();
	//����ʧ��
	bool SendRequestFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszDescribe, LONG lErrorCode);

	//Ч�麯��
public:
	//��ʼЧ��
	bool EfficacyStartGame(WORD wReadyChairID);
	//��ַЧ��
	bool EfficacyIPAddress(IServerUserItem * pIServerUserItem);
	//����Ч��
	bool EfficacyScoreRule(IServerUserItem * pIServerUserItem);
	//У���û�GPS����
	bool CheckUserDistance();
	//�����û���IP��ַ����ʱ����EfficacyIPAddress������
	bool CheckUserIpAddress();

public:
	//������
	bool CheckDistribute();
	//��Ϸ��¼
	void RecordGameScore(bool bDrawStarted, DWORD dwStartGameTime=INVALID_DWORD);

public:

	//����ȦȺ����ɢ����, ֪ͨ����Ϸ����
	void OnEventClubDismissRoom();

//�����û�����ӻ�����
public:
	virtual bool AddVirtualUser(IServerUserItem* pServerUserItem);

	//��������
protected:
	double GetDistance(double long1, double lat1, double long2, double lat2);

};

//////////////////////////////////////////////////////////////////////////////////

#endif