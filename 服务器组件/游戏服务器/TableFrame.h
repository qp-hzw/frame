#ifndef TABLE_FRAME_HEAD_FILE
#define TABLE_FRAME_HEAD_FILE

#include <vector>
#include "Stdafx.h"
#include "Player.h"
#include "TableFrameBase.h"

//////////////////////////////////////////////////////////////////////////////////
#define GAME_CONCLUDE_CONTINUE  0xFF //��ֽ���������
#define GAME_CONCLUDE_NORMAL    0xFE //��������

//��Ϸ״̬
#define GAME_STATUS_FREE			0									//����״̬
#define GAME_STATUS_PLAY			100									//��Ϸ״̬	TODO ϸ�����ĳ�����״̬

//////////////////////////////////////////////////////////////////////////////////
//ʱ���ʶ

//�����¼���Χ
#define IDI_MAIN_MODULE_START		1									//��ʼ��ʶ
#define IDI_MAIN_MODULE_FINISH		99									//��ֹ��ʶ

//�����¼���Χ
#define IDI_TABLE_MODULE_START		10000								//��ʼ��ʶ
#define IDI_TABLE_MODULE_FINISH		50000								//��ֹ��ʶ

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
private:
	WORD							m_wTableID;							//���Ӻ���		��0��ʼ�������ӳ�ʼ����ʱ���Ѿ���ֵ
	WORD							m_wChairCount;						//������Ŀ

	BYTE							m_cbTableMode;						//������Ϸģʽ  ������, ��ҳ�, ������ҳ�, ������

	tagTableRule					m_tagTableRule;						//ͨ�÷������(client��������˴�����)
	tagTableSubGameRule				m_tagTableSubGameRule;				//����Ϸ���з������(client��������Ϸ��)

	//TODOLATER ֻ����һ���ֶ�
	DWORD							m_dwTableOwner;						//��������һ�����µ���ң�
	DWORD							m_dwCreateTableUser;				//���������û�

	//TODOLATER ������
	DWORD							m_dwGroupID;						//��������ȦID


	/****************************************** ��̬���� **************************************************/
private:
	WORD                            m_wCurGameRound;                    //��ǰ��Ϸ����
	BYTE							m_cbGameStatus;						//��Ϸ״̬

	std::vector<CPlayer*>			m_user_list;				        //�������(�����Թ�)
	std::vector<CPlayer*>			m_player_list;				        //��Ϸ�û�(���������ϵ����)
	
	//�����ɢ
private:
	bool							m_bAgree[MAX_CHAIR];					//ͬ���ɢ
	bool							m_bResponseDismiss[MAX_CHAIR];			//��Ӧ��ɢ
	bool							m_bUnderDissState;						//�����Ƿ��ڽ�ɢ״̬
	WORD							m_dissmisserChaiID;						//��ɢ������

	//ʱ�����
protected:
	SYSTEMTIME						m_SystemTimeStart;					//��ʼʱ��

	//����ӿ�
protected:
	ITableFrameSink	*				m_pITableFrameSink;					//���ӽӿ�

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

	//����ӿ�
public:
	//��������
	virtual bool StartService(){return true;}
	//ֹͣ����
	virtual bool ConcludeService(){return true;}
#pragma endregion

#pragma region ���������������ȡ
public:
	//���ӱ��
	virtual WORD GetTableID() { return m_wTableID; }
	//��������
	virtual void SetTableID(DWORD dwTableID){ m_wTableID=dwTableID; }

	//��ȡ״̬
	virtual BYTE GetGameStatus() { return m_cbGameStatus; }
	//����״̬
	virtual VOID SetGameStatus(BYTE cbGameStatus) { m_cbGameStatus=cbGameStatus; }

	//������������һ�����µ���
	virtual void SetTableOwner(DWORD dwUserID) {m_dwTableOwner = dwUserID;}
	//�����������һ�����µ���
	virtual DWORD GetTableOwner() {return m_dwTableOwner;}

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

	//�����Ƿ��ڽ�ɢ״̬
	bool GetDismissState();
	//���÷��䴦�ڽ�ɢ״̬
	void SetDismissState(bool bState);

	//���÷����Զ���ɢʱ�� added by lizhihu
	virtual void SetTableAutoDismiss(DWORD dwMinutes = 1);

#pragma endregion

	//����Ϸ���ܽӿں���
public:
	// 1���; 2˽��; 3����
	virtual int GameType() { return 2; }
	//is roboot
	virtual bool IsRobot(WORD wChairID) { return true; }
	//get player base info
	virtual BASE_PLAYERINFO GetPlayerBaseInfo(WORD wChairID) { BASE_PLAYERINFO i; return i; }

#pragma region ��Ϸ���̺���
	//������ã�������Ϸ�޹�
public:
	//��ʼ��Ϸ
	virtual bool StartGame();

	//������Ϸ��������
public:
	//С�ֽ���������
	virtual bool HandleXJGameEnd(BYTE cbCurGameCount, BYTE cbGameMode, SCORE *lGameScore);
	//��ֽ���������  ֮ǰ����Ϊ: HandleDJGameEnd
	virtual bool HandleDJGameEnd(BYTE cbGameStatus);

	//д��¼���¼(ս���ط�) ���� С����,���ݺͳ���
	bool WriteRecordInfo(WORD wXJCount,TCHAR strScore[], VOID* pData, DWORD dwDataSize);

	//С�ִ�ֽ�����������
protected:	
	//ÿ����Ϸ������, �жϲ��۳��û���Ʊ
	bool XJGameTickets(BYTE byTableMode, BYTE byRound);
	//ÿ����Ϸ�����󣬸����û��Ƹ���Ϣ
	bool XJModifyUserTreasure(BYTE byTableMode, BYTE byRound, SCORE *lGameScore, tagTableRule *pCfg);	
	//ÿ����Ϸ������, ����û��Ƹ��Ƿ���Լ�����Ϸ
	bool CheckUserLeave(BYTE byTableMode, CPlayer* pIServerUserItem, tagTableRule* pCfg);
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
	virtual bool SendUserItemData(CPlayer * pIServerUserItem, WORD wSubCmdID);
	//��������
	virtual bool SendUserItemData(CPlayer * pIServerUserItem, WORD wSubCmdID, VOID * pData, WORD wDataSize);

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
	int PlayerEnterTable(CPlayer* pPlayer);
	//�������		ֻ�д����ͼ��뷿��ʱ���û�����ʱ��ҪУ��GPS���ʼӴ��ֶ�
	int PlayerSitTable(WORD wChairID, CPlayer * pIServerUserItem, LPCTSTR lpszPassword=NULL, bool bCheckUserGPS = false);
	//�������
	bool PlayerUpTable(CPlayer * pIServerUserItem);
	//����뿪
	int PlayerLeaveTable(CPlayer* pPlayer);
	//���׼��
	int PlayerReady(WORD wChairID, CPlayer* pPlayer);

	//��Ҷ�����������
private:
	int CanPlayerEnterTable(CPlayer* pPlayer);
	int CanPlayerSitTable(CPlayer* pPlayer, WORD &wChairID);
	int CanPlayerUpTable(CPlayer* pPlayer);
	int CanPlayerLeaveTable(CPlayer* pPlayer);
	int CanPlayerReady(CPlayer* pPlayer);

#pragma endregion

	//���ܽӿ�
public:
	//���ͳ���
	virtual bool SendGameScene(IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);

#pragma region ���Ӷ���
public:
	//��ȡ��λ
	WORD GetNullChairID();
	//�û���Ŀ
	WORD GetSitUserCount();

	//��ȡ�������
	WORD GetPlayerChair(CPlayer* pPlayer);

#pragma endregion

	//�û��¼�
public:
	//�����¼�
	bool OnEventUserOffLine(CPlayer * pIServerUserItem);

#pragma region ��ɢ����
public:
	//�����ɢ����
	bool OnEventApplyDismissRoom(WORD wChairID, bool bAgree);
	//����Ƿ��ɢ����
	bool OnEventVoteDismissRoom(WORD wChairID,bool bAgree);
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
	//����״̬ -- ?? ֪ͨcenter??
	bool SendTableStatus();
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

#pragma endregion

public:
	//��Ϸ��¼
	void RecordGameScore( DWORD dwStartGameTime=INVALID_DWORD);

public:
	//����ȦȺ����ɢ����, ֪ͨ����Ϸ����
	void OnEventClubDismissRoom();
};

#endif