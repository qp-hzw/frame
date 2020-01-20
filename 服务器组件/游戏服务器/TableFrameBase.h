#ifndef GAME_SERVICE_HEAD_HEAD_FILE
#define GAME_SERVICE_HEAD_HEAD_FILE

#define GAME_CONCLUDE_CONTINUE  0xFF //��ֽ���������
#define GAME_CONCLUDE_NORMAL    0xFE //��������

//��Ϸ״̬
#define GAME_STATUS_FREE			0									//����״̬
#define GAME_STATUS_PLAY			100									//��Ϸ״̬	TODO ϸ�����ĳ�����״̬

//����ԭ��
#define GER_NORMAL					0x00								//�������
#define GER_DISMISS					0x01								//��Ϸ��ɢ
#define GER_USER_LEAVE				0x02								//�û��뿪
#define GER_NETWORK_ERROR			0x03								//�������


//ͨ�÷������
struct tagTableRule
{
	/********************************* ����ʹ�� ************************************/
	BYTE	GameMode;				//��Ϸģʽ 0-����ģʽ; 1-����ģʽ;  2-���ģʽ;  3-�������;
	BYTE	RobBankType;			//��ׯģʽ 0-��ׯ����ע������ׯ�� 1-��ׯ��������ׯ��2-�̶����̶�������ׯ��  3-Ӯׯ��Ӯ������ׯ�� 4-ҡ���Ӷ�ׯ�����ׯ�ң�
	BYTE	GameCount;				//��Ϸ���� 0-���޾�
	BYTE	GameCountType;			//������� �ǵ�һ���, �ڶ����, ���ǵ������
	BYTE	PlayerCount;			//������� 0-���������ɿ�
	BYTE	cbPayType;				//֧����ʽ��0����֧����1AA��
	DWORD	lSinglePayCost;			//��ҳ� �볡��Ʊ  TODONOW ���ֶν��ڿ۳���Ʊ��ʱ�����ã������ĵ��þ�����ǰ��, ����ɾ��
	WORD	CellScore;				//*�׷�	  Ĭ��Ϊ1 -- ����Ϸʹ�õ��Ǹ��ֶ�
	DWORD	MaxFan;					//*�ⶥ���� 0-���ⶥ 
	DWORD	LeaveScore;				//*�볡���� 0-������         -- ������
	DWORD	JoinScore;				//*�볡�趨 0-������         -- ������
	BYTE	bRefuseSameIP;			//����ͬIP    0-������ 1-����
	BYTE	bDistanceIn300;			//����300��	  0-������ 1-����
	double	dLongitude;				//����	
	double	dLatitude;				//γ��	
	BYTE	bCreateToOther;			//�Ƿ�Ϊ���˿��� 0-�� 1-��
	DWORD	FangZhu;				//*����
	BYTE	bAllowStranger;			//����İ���˼���

	/********************************** ����Ȧ��� ************************************/
	BYTE	byClubCreate;			//0�������� 1 ����Ȧ��ͨ����  2����Ȧ˽�ܴ���
	DWORD	dwUserID;				//Ⱥ��ID
    DWORD	dwClubID;				//����Ⱥ/���ֲ����
	DWORD	dwKindID;			    //��ϷID
	BYTE	byGoldOrFK;				//(2.��� 1.����)

	BYTE	bDissolve;				//�Ƿ������ɢ 0���� 1������
	DWORD	dwDissolveTime;			//��ɢʱ�� (����)

	//��ҷ�����
	DWORD	dwAmount;				//��Ͷ��
	DWORD	dwOwnerPercentage;		//Ⱥ�����
    
	BYTE	byMask;					//1 AA֧��;  2��Ӯ��֧��
	DWORD	dwDizhu;				//��ע
	//TODONOW ������ھ��ֲ��Ľ�ҳ�,������Ƿ������õ�; 
	//������ڴ����ķ�����ҳ�,�������ϵͳ���õ�; 
	//������ڴ����Ľ�ҳ�, �������ϵͳ���õ�
    DWORD	dwLevelGold;			//��������С��� 

	/************************************ �����ֶ� ************************************/
	BYTE byRetain1;
    BYTE byRetain2;
    BYTE byRetain3;
	BYTE byRetain4;

	DWORD szRetain1;
    DWORD szRetain2;
	DWORD szRetain3;
    DWORD szRetain4;
	DWORD szRetain5;
    DWORD szRetain6;
	DWORD szRetain7;
    DWORD szRetain8;
    DWORD szRetain9;
    DWORD szRetain10;
};

/////////////////////////////////////////////////////////////////////
//����ͨ����  

//�û��ӿ�
interface IServerUserItem
{
public:
//	//���Ӻ���
	virtual WORD GetChairID() = NULL;
};
//��һ�����Ϣ DB use player base info
struct BASE_PLAYERINFO
{
	char					m_szAccID[54 + 1];
	int				m_PlayerID;
	char					m_szPlayerName[54 + 1];
	unsigned int		m_LV;
	unsigned int		m_dwExp;
	//unsigned int		m_goldCoin;
	int		m_goldCoin;
	unsigned int		m_diamond;
	unsigned int		m_vip;
	time_t				m_registerDate;
	char					m_szPassword[54 + 1];
	unsigned int		m_sex;
	char					m_szFaceUrl[54 + 1];
	time_t				m_receiveTime;
	unsigned int      m_RechTimes;
	unsigned int      m_gameNumber;
	unsigned int      m_winNumber;
	unsigned int      m_invitecode;
	char              m_szRegisterAddrip[54 + 1];
	unsigned int      m_VoucherNum;								//���������
	char              m_szLoginip[54 + 1];
	char              m_szRealName[54 + 1];   //ʵ��
	int					m_PlayerModelID;							//���ģ��ID
	int					m_Permissions;								//Ȩ��
	int                 m_creditpoint;                              //�������÷�
	//int          m_match_score;                                 //��������
};
/////////////////////////////////////////////////////////////////////

//���ӽӿ�
interface ITableFrame : public IUnknownEx
{
public:
	//��Ϸģʽ 0-����ģʽ; 1-����ģʽ;  2-���ģʽ;  3-�������;
	virtual int GameType() = 0;
	//is roboot
	virtual bool IsRobot(WORD wChairID) = 0;
	//get player base info
	virtual BASE_PLAYERINFO GetPlayerBaseInfo(WORD wChairID) = 0;

		//���ò���
public:
	//��ȡͨ�÷������
	virtual VOID* GetCustomRule() = NULL;
	//��ȡ����Ϸ���з������
	virtual VOID * GetSubGameRule() = NULL;

	//״̬�ӿ�
public:
	//��ȡ״̬
	virtual BYTE GetGameStatus() = NULL;
	//����״̬
	virtual VOID SetGameStatus(BYTE bGameStatus) = NULL;

	//���ƽӿ�
public:
	//��ʼ��Ϸ
	virtual bool StartGame() = NULL;

	//д�ֽӿ�
public:
	//д��¼���¼ ���� С����,���ݺͳ���
	virtual bool WriteRecordInfo(WORD wXJCount, TCHAR strScore[], VOID* pData, DWORD dwDataSize) = NULL;
	//����С�ֽ���
	virtual bool HandleXJGameEnd(BYTE cbCurGameCount, BYTE cbGameMode, SCORE *lGameScore) = NULL;
	//�����ֽ���
	virtual bool HandleDJGameEnd(BYTE cbGameStatus) = NULL;

	//ʱ��ӿ�
public:
	//����ʱ��
	virtual bool SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter) = NULL;
	//ɾ��ʱ��
	virtual bool KillGameTimer(DWORD dwTimerID) = NULL;
	//��ȡ��ʱ��ʣ�������
	virtual DWORD GetTimerLeftTickCount(DWORD dwTimerID) = NULL;

	//����ӿ�
public:
	//��������
	virtual bool SendTableData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize, WORD wMainCmd =200) = NULL;

	//���ܽӿ�
public:
	//���ͳ���
	virtual bool SendGameScene(IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize) = NULL;
};

//////////////////////////////////////////////////////////////////////////////////
//�ص��ӿ�
interface ITableFrameSink : public IUnknownEx
{
	//����ӿ�
public:
	//��������
	virtual bool StartService(){return true;}
	//ֹͣ����
	virtual bool ConcludeService(){return true;}

	//����ӿ�
public:
	//��λ�ӿ�
	virtual VOID RepositionSink() = NULL;
	//���ýӿ�
	virtual bool Initialization(ITableFrame *pTableFrame) = NULL;

	//��ѯ�ӿ�
public:
	//��ȡKindID	frame����
	virtual DWORD GetKindIDToFrame() = NULL;

	//��Ϸ�¼�
public:
	//��Ϸ��ʼ
	virtual bool OnEventGameStart() = NULL;
	//��Ϸ����
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason) = NULL;
	//���ͳ���
	virtual bool OnEventSendGameScene(WORD wChairID, IServerUserItem *pIServerUserItem, BYTE cbGameStatus, bool bSendSecret) = NULL;

	//�¼��ӿ�
public:
	//ʱ���¼�
	virtual bool OnTimerMessage(DWORD dwTimerID, WPARAM dwBindParameter) = NULL;

	//����ӿ�
public:
	//��Ϸ��Ϣ
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, WORD wChairID) = NULL;

public:
	//�û�����
	virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser) = NULL;
	//�û�����
	virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser) = NULL;
	//�û�׼��
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize) = NULL;

};

#endif