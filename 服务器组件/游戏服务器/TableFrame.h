#ifndef TABLE_FRAME_HEAD_FILE
#define TABLE_FRAME_HEAD_FILE

#include <vector>
#include <list>
#include "Stdafx.h"
#include "Player.h"
#include "TableFrameBase.h"
#include "SubRecord.h"

//////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////////
//ʱ���ʶ

//�����¼���Χ
#define IDI_MAIN_MODULE_START		1									//��ʼ��ʶ
#define IDI_MAIN_MODULE_FINISH		99									//��ֹ��ʶ

//�����¼���Χ
#define IDI_TABLE_MODULE_START		10000								//��ʼ��ʶ
#define IDI_TABLE_MODULE_FINISH		50010000							//��ֹ��ʶ

//�������¼���Χ
#define IDI_MATCH_MODULE_START		150									//��ʼ
#define IDI_MATCH_MODULE_FINISH		200									//��ֹ

//�ص��¼���Χ
#define TIME_TABLE_SINK_RANGE		40									//��ʶ��Χ
#define TIME_TABLE_MODULE_RANGE		50									//��ʶ��Χ
//////////////////////////////////////////////////////////////////////////////////

//���鶨��
typedef CWHArray<tagGameScoreRecord *> CGameScoreRecordArray;			//��¼����

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
	/****************************************** ��̬���� **************************************************/
	//����ӿ�
protected:
	ITableFrameSink	*				m_pITableFrameSink;					//���ӽӿ�

private:
	DWORD							m_wTableID;							//���ӱ�־ 6λ
	WORD							m_wChairCount;						//������Ŀ

	tagTableRule					m_tagTableRule;						//ͨ�÷������(client��������˴�����)
	tagClubRoomRule                 m_tagClubRoomRule;					//���᷿�����

	DWORD							m_dwTableOwner;						//��������һ�����µ���ң�

	std::string						m_OnlyID;							//ս����¼��Ψһ��ʶ

	CSubRecord						m_Record;							//¼��ط�

	/****************************************** ��̬���� **************************************************/
private:
	WORD                            m_wCurGameRound;                    //��ǰ��Ϸ����
	BYTE							m_cbGameStatus;						//��Ϸ״̬

	std::list<CPlayer*>				m_user_list;				        //�������(�����Թ�)
	std::vector<CPlayer*>			m_player_list;				        //��Ϸ�û�(���������ϵ����)

	//�������
	std::vector<SCORE>				m_total_score;						//�ܷ�

	//�����ɢ
public:
	bool							m_bAgree[MAX_CHAIR];				//ͬ���ɢ
	bool							m_bResponseDismiss[MAX_CHAIR];		//��Ӧ��ɢ
	bool							m_bUnderDissState;					//�����Ƿ��ڽ�ɢ״̬
	WORD							m_dissmisserChaiID;					//��ɢ������

	//��Ϸ��¼
protected:
	CGameScoreRecordArray			m_GameScoreRecordActive;			//��Ϸ��¼

#pragma endregion

#pragma region ��ʼ������
	//��������
public:
	//���캯��
	CTableFrame();
	//��������
	virtual ~CTableFrame();
	//��������
	CTableFrame(const CTableFrame& table);

	//����ӿ�
public:
	//��������
	virtual bool StartService(){return true;}
	//ֹͣ����
	virtual bool ConcludeService(){return true;}
#pragma endregion

#pragma region ����ֻ������
public:
	//��ȡ����ģʽ
	virtual BYTE GetTableMode(){return m_tagTableRule.GameMode;};
	//��ȡ��ҳ�����
	BYTE GetGoldType() { return m_tagTableRule.GameRoomLevel; }

	//��ȡ������Ŀ
	WORD GetChairCount() { return m_wChairCount; }

#pragma endregion
#pragma region ���������������ȡ
public:
	//���ӱ��
	virtual DWORD GetTableID() { return m_wTableID; }
	//��������
	virtual void SetTableID(DWORD dwTableID){ m_wTableID=dwTableID; }

	//��ǰ��Ϸ����
	virtual DWORD GetCurGameRound() { return m_wCurGameRound; }

	//��ȡ״̬
	virtual BYTE GetGameStatus() { return m_cbGameStatus; }
	//����״̬
	virtual VOID SetGameStatus(BYTE cbGameStatus) { m_cbGameStatus=cbGameStatus; }

	//������������һ�����µ���
	virtual void SetTableOwner(DWORD dwUserID) {m_dwTableOwner = dwUserID;}
	//�����������һ�����µ���
	virtual DWORD GetTableOwner() {return m_dwTableOwner;}

	//��ȡ����ͨ�ù���
	virtual tagTableRule* GetCustomRule() { return &m_tagTableRule;};
	//���÷���ͨ�ù���
	virtual void SetCommonRule(tagTableRule* pRule)
	{
		//��ʼ����������
		memcpy(&m_tagTableRule, pRule, sizeof(m_tagTableRule));
		m_wChairCount = pRule->PlayerCount;
		m_player_list.resize(m_wChairCount, NULL);
		m_total_score.resize(m_wChairCount, 0);

		//��ʼ������Ϸ����
		if (m_pITableFrameSink != NULL)
			m_pITableFrameSink->Initialization(this, &m_tagTableRule);
	};

	//��ȡ���᷿�����
	virtual tagClubRoomRule* GetClubRoomRule() { return &m_tagClubRoomRule;};
	//���ù��᷿�����
	virtual void SetClubRoomRule(tagClubRoomRule* pRule)
	{
		memcpy(&m_tagClubRoomRule, pRule, sizeof(m_tagClubRoomRule));
	};

	//��ⷿ��
	void CheckRoomTruePlayer();

	//���������
public:
	//�����Ƿ�����
	bool IsRoomFull();

	//����Զ�׼����ʱ��
	void SetPlayerAutoReady();
	//��ȡ�������������
	std::vector<CPlayer *> GetPlayer_list() { return m_player_list; }

	//���ÿ�ʼ��һ�׶ζ�ʱ��
	void SetStageTimer();

	//��ȡ�ܷ�
	std::vector<SCORE> GetTotalScore() { return m_total_score; }
	//��ȡOnlyID
	std::string GetOnlyID()	{ return m_OnlyID; }

	//��ʼ��һ�׶α���
	virtual void StartNextStage() { CLog::Log(log_debug, "no"); }

#pragma endregion

	//����Ϸ���ܽӿں���
public:
	// 0���� 1����; 2���; 3������� 4����Ȧ
	virtual int GameType() { return m_tagTableRule.GameMode; }
	//is roboot
	virtual bool IsRobot(WORD wChairID);

	//�����й�
	virtual bool SendPlayerTuoGuan(WORD wChairID);

#pragma region ��Ϸ���̺���
	//������ã�������Ϸ�޹�
protected:
	//��ʼ��Ϸ
	virtual bool StartGame();

	//������Ϸ��������
public:
	//С�ֽ���������
	virtual bool HandleXJGameEnd(BYTE cbCurGameCount,WORD *wIdentity, SCORE *lGameScore);
	//��ֽ���������  ֮ǰ����Ϊ: HandleDJGameEnd
	virtual bool HandleDJGameEnd(BYTE cbGameStatus);

	//С�ִ�ֽ�����������
protected:	
	//ÿ����Ϸ������, �жϲ��۳��û���Ʊ
	bool XJTickets();
	//ÿ����Ϸ�����󣬸����û��Ƹ���Ϣ
	bool XJModifyUserTreasure(SCORE *lGameScore);
	//ÿ����Ϸ�����󣬸�����Ϸ����״̬
	bool XJUpdateGameTaskStatus(const BYTE &cbTableMode, const BYTE &cbCurGameCount);

	//ÿ����Ϸ������, ��������ս����
	bool XJUpdateTableRecord(BYTE round, string OnlyID);
	//ÿ����Ϸ������, ��������¼���
	bool XJUpdateTableVideo(BYTE round, string OnlyID, VOID* pData, DWORD dwDataSize);

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
public:
	//��������
	virtual bool SendTableData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize, WORD wMainCmdID = MDM_GF_GAME);
	//���ͳ���
	virtual bool SendGameScene(WORD wChairID, VOID * pData, WORD wDataSize);

	//ϵͳ��Ϣ
public:
	//������Ϣ
	virtual bool SendGameMessage(LPCTSTR lpszMessage, WORD wType);
	//��Ϸ��Ϣ
	virtual bool SendGameMessage(CPlayer * pIServerUserItem, LPCTSTR lpszMessage, WORD wType);
	//������Ϣ
	virtual bool SendRoomMessage(CPlayer * pIServerUserItem, LPCTSTR lpszMessage, WORD wType);
#pragma endregion

#pragma region ��Ҷ���
	//�û��ӿ�
public:
	//��Ϸ�û�
	virtual CPlayer * GetTableUserItem(WORD wChairID);
	//�Թ��û�
	virtual CPlayer * EnumLookonUserItem(WORD wEnumIndex);

	//��Ҷ���
public:
	//��Ҽ���
	virtual int PlayerEnterTable(CPlayer* pPlayer);
	//�������		ֻ�д����ͼ��뷿��ʱ���û�����ʱ��ҪУ��GPS���ʼӴ��ֶ�
	virtual int PlayerSitTable(CPlayer * pIServerUserItem, WORD wChairID = INVALID_CHAIR, bool bCheckUserGPS = false);
	//�������
	virtual bool PlayerUpTable(CPlayer * pIServerUserItem);
	//����뿪
	virtual int PlayerLeaveTable(CPlayer* pPlayer);
	//���׼��
	virtual int PlayerReady(CPlayer* pPlayer);
	//���ȡ��׼��
	virtual int PlayerCancelReady(CPlayer* pPlayer);
	//��Ҷ���
	virtual int PlayerOffline(CPlayer* pPlayer);

	//��Ҷ�����������
private:
	int CanPlayerEnterTable(CPlayer* pPlayer);
	int CanPlayerSitTable(CPlayer* pPlayer, WORD &wChairID);
	int CanPlayerUpTable(CPlayer* pPlayer);
	int CanPlayerLeaveTable(CPlayer* pPlayer);
	int CanPlayerReady(CPlayer* pPlayer);
	int CanPlayerCancelReady(CPlayer* pPlayer);

#pragma endregion

#pragma region ���Ӷ���
public:
	//��ȡ��λ
	WORD GetNullChairID();
	//�û���Ŀ
	WORD GetSitUserCount();

	//��ȡ�������
	WORD GetPlayerChair(CPlayer* pPlayer);
	//�������Ƿ�����ʵ���
	bool IsTruePlayer();

#pragma endregion

#pragma region ��ɢ����
public:
	//�����ɢ����
	bool OnEventApplyDismissRoom(WORD wChairID, bool bAgree);
	//����Ƿ��ɢ����
	bool OnEventVoteDismissRoom(WORD wChairID,bool bAgree);

	//�����Ƿ��ڽ�ɢ״̬
	bool GetDismissState();
	//���÷��䴦�ڽ�ɢ״̬
	void SetDismissState(bool bState);

	//���ñ����ɢ���䶨ʱ��
	void SetVoteDismissRoom();
	//ȡ�������ɢ���䶨ʱ��
	void KillVoteDismissRoom();

	//��ɢ״̬��������
	bool OnEventDismissOffline(WORD wChairID);

	//���÷����Զ���ɢʱ�� added by lizhihu
	virtual void SetTableAutoDismiss(DWORD dwMinutes = 1);
#pragma endregion

	//ϵͳ�¼�
public:
	//ʱ���¼�
	bool OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//��Ϸ�¼�
	bool OnEventSocketGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, CPlayer * pIServerUserItem);
	//����¼�
	bool OnEventSocketFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, CPlayer * pIServerUserItem);

	//��������
public:
	//����ʧ��
	bool SendRequestFailure(CPlayer * pIServerUserItem, LPCTSTR pszDescribe, LONG lErrorCode);

#pragma region Ч�麯��
public:
	//��ʼЧ��
	bool EfficacyStartGame(WORD wReadyChairID);
	//��ַЧ��
	bool EfficacyIPAddress(CPlayer * pIServerUserItem);
	//����Ч��
	bool EfficacyScoreRule(CPlayer * pIServerUserItem);
	//У���û�GPS����
	bool CheckUserDistance();
	//�����û���IP��ַ����ʱ����EfficacyIPAddress������
	bool CheckUserIpAddress();

		//��������
protected:
	double GetDistance(double long1, double lat1, double long2, double lat2);

	//�жϴ�Ӯ��
	bool GetBigWinner(WORD wChairID);
	//��ҳ�����
	bool SettleGold(WORD *wIdentity);

#pragma endregion

public:
	//����ȦȺ����ɢ����, ֪ͨ����Ϸ����
	void OnEventClubDismissRoom();
};

#endif