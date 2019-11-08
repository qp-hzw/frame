#ifndef GAME_SERVICE_HEAD_HEAD_FILE
#define GAME_SERVICE_HEAD_HEAD_FILE

class CPlayer;
class CPlayerManager;
//////////////////////////////////////////////////////////////////////////////////
//��������

//��������
#ifndef GAME_SERVICE_CLASS
#ifdef  GAME_SERVICE_DLL
#define GAME_SERVICE_CLASS _declspec(dllexport)
#else
#define GAME_SERVICE_CLASS _declspec(dllimport)
#endif
#endif

//ģ�鶨��
#ifndef _DEBUG
#define GAME_SERVICE_DLL_NAME	TEXT("GameService.dll")				//��� DLL ����
#else
#define GAME_SERVICE_DLL_NAME	TEXT("GameServiceD.dll")			//��� DLL ����
#endif

//////////////////////////////////////////////////////////////////////////////////
//�ӿ�˵��


//���ӽӿ�
interface ITableFrame;
interface ITableFrameSink;
interface ITableUserAction;

//����ӿ�
interface IMainServiceFrame;
interface IGameServiceManager;
interface IGameServiceCustomRule;

//////////////////////////////////////////////////////////////////////////////////
//��������

//Ⱥ������
#define BG_COMPUTER					(BYTE)(0x02)						//����Ⱥ��

//��������
#define INDEX_SOCKET				(WORD)(0x0000)						//��������
#define INDEX_ANDROID				(WORD)(0x2000)						//��������

//��������
#define GAME_SERVICE_CREATE_NAME	"CreateGameServiceManager"			//��������
//��ȡ�����ļ�
#define GAME_SERVICE_LOAD_CONFIG	"LoadConfigServiceManager"			//��������

//////////////////////////////////////////////////////////////////////////////////
//��������

//��������
#define ANDROID_SIMULATE			0x01								//�໥ģ��
#define ANDROID_PASSIVITY			0x02								//�������
#define ANDROID_INITIATIVE			0x04								//�������

//����ԭ��
#define GER_NORMAL					0x00								//�������
#define GER_DISMISS					0x01								//��Ϸ��ɢ
#define GER_USER_LEAVE				0x02								//�û��뿪
#define GER_NETWORK_ERROR			0x03								//�������

//�뿪ԭ��
#define LER_NORMAL					0x00								//�����뿪
#define LER_SYSTEM					0x01								//ϵͳԭ��
#define LER_NETWORK					0x02								//����ԭ��
#define LER_USER_IMPACT				0x03								//�û���ͻ
#define LER_SERVER_FULL				0x04								//����Ϊ��
#define LER_SERVER_CONDITIONS		0x05								//��������
#define LER_EXCEPTION				0x06								//�쳣

//��������
#define SCORE_TYPE_NULL				0x00								//��Ч����
#define SCORE_TYPE_WIN				0x01								//ʤ�ֻ���
#define SCORE_TYPE_LOSE				0x02								//��ֻ���
#define SCORE_TYPE_DRAW				0x03								//�;ֻ���
#define SCORE_TYPE_FLEE				0x04								//�Ӿֻ���
#define SCORE_TYPE_PRESENT			0x10								//���ͻ���
#define SCORE_TYPE_SERVICE			0x11								//�������


//����ѡ��
#define DISTRIBUTE_ALLOW			0x01								//�������
#define DISTRIBUTE_IMMEDIATE		0x02								//����ѡ��
#define DISTRIBUTE_LAST_TABLE		0x04								//ͬ��ѡ��
#define DISTRIBUTE_SAME_ADDRESS		0x08								//��ַѡ��

//////////////////////////////////////////////////////////////////////////////////
//ʱ���ʶ

//���ȷ�Χ
#define IDI_MAIN_MODULE_START		1									//��ʼ��ʶ
#define IDI_MAIN_MODULE_FINISH		99									//��ֹ��ʶ

//������Χ
#define IDI_REBOT_MODULE_START		100									//��ʼ��ʶ
#define IDI_REBOT_MODULE_FINISH		199									//��ֹ��ʶ

//������Χ
#define IDI_MATCH_MODULE_START		200									//��ʼ��ʶ
#define IDI_MATCH_MODULE_FINISH		299									//������ʶ
//���ӷ�Χ
#define IDI_TABLE_MODULE_START		10000								//��ʼ��ʶ
#define IDI_TABLE_MODULE_FINISH		50000								//��ֹ��ʶ

//////////////////////////////////////////////////////////////////////////////////
//ʱ�䷶Χ

//��Ϸʱ��
#define TIME_TABLE_SINK_RANGE		40									//��ʶ��Χ
#define TIME_TABLE_MODULE_RANGE		50									//��ʶ��Χ

//////////////////////////////////////////////////////////////////////////////////
//��������

//��������
#define RESERVE_USER_COUNT			40L									//������Ŀ

//////////////////////////////////////////////////////////////////////////////////
//�ṹ����

//���ò���
struct tagGameParameter
{
	//������Ϣ
	WORD							wMedalRate;							//���ƻ���
	WORD							wRevenueRate;						//����˰��

	//�汾��Ϣ
	DWORD							dwClientVersion;					//�ͻ��汾
	DWORD							dwServerVersion;					//����汾
};

//��������
struct tagGameServiceAttrib
{
	DWORD							dwGameID;							//��Ϸ����

	WORD							wChairCount;						//������Ŀ 

	DWORD							dwSubGameVersion;					//����Ϸ�汾
	TCHAR							szDllName[LEN_DLL];					//Dll����
};

//��������
struct tagGameServiceOption
{
	DWORD                           dwGameID;                           //��Ϸ��־		TODO ���������Ϸ���tagGameServiceAttrib
	DWORD							dwServerID;							//�����ʶ

	//��������
	TCHAR							szServerName[LEN_SERVER];			//��������
	WORD							wGameServerPort;					//��Ϸ���˿�
	TCHAR							szGameServerAddr[LEN_DB_ADDR];		//��Ϸ����ַ    ���ݿ��л�ȡ
};

//��Ϸ����
struct tagRoomRuleOption
{
	BYTE							cbGameOptionType[1024];				//��������
};

/*�Խ��������ֽڶ�Ӧ����ֵ*/
enum enOption
{
	//modified by lizhihu 2017.10.9
	enOption_GameId = 0,			//��ϷID,������Ϊ4
	enOption_GameScoreTimes,		//������Ϸ�ӱ�ģʽ:�������ӱ�ģʽ		0-����ģʽ	1-���ģʽ
	enOption_RobBankType,			//���ӿ�ܴ�������Ϸ��ׯģʽ				0-��ׯ		1-��ׯ		2-�̶�
	enOption_RoomCell,				//�׷�,�ͻ��˲��ô���
	enOption_FangZhu,				//����,�ͻ��˲��ô���			
	enOption_GameCount,				//��Ϸ����,�ͻ��˲��ô���
	enOption_PlayerCount,			//�����,�ͻ��˲��ô���
};




//���Ӳ���
struct tagTableFrameParameter
{
	//���ñ���
	tagGameParameter *				pGameParameter;						//���ò���
	tagGameServiceAttrib *			pGameServiceAttrib;					//��������
	tagGameServiceOption *			pGameServiceOption;					//���ò���
};

//��������
struct tagAndroidUserParameter
{
	//���ñ���
	tagGameParameter *				pGameParameter;						//���ò���
	tagGameServiceAttrib *			pGameServiceAttrib;					//��������
	tagGameServiceOption *			pGameServiceOption;					//���ò���

	//�ں����
//	ITimerEngine *					pITimerEngine;						//ʱ������
	CPlayerManager *			pIServerUserManager;				//�û�����
	IGameServiceManager *			pIGameServiceManager;				//�������
	//ITCPNetworkEngineEvent *		pITCPNetworkEngineEvent;			//�¼��ӿ�
};

//////////////////////////////////////////////////////////////////////////////////
//�����ṹ

//������Ϣ
struct tagScoreInfo
{
	BYTE							cbType;								//��������
	SCORE							lScore;								//�û�����
	SCORE							lGrade;								//�û��ɼ�
	SCORE							lRevenue;							//��Ϸ˰��
	SCORE							lChoushui;							//��ˮֵ
	DWORD							dwChoushuiType;						//��ˮ����		(Ĭ��)(̨��/˰��)��ˮ=0		|	(ϴ��)��ˮ=1
};

//�����Ϣ
struct tagVariationInfo
{
	//������Ϣ
	SCORE							lScore;								//�û�����
	SCORE							lGrade;								//�û��ɼ�
	SCORE							lInsure;							//�û�����
	SCORE							lRevenue;							//��Ϸ˰��	
	SCORE							lChoushui;							//��ˮֵ
	DWORD							dwChoushuiType;						//��ˮ����

	//��Ϸ��Ϣ
	DWORD							dwWinCount;							//ʤ������
	DWORD							dwLostCount;						//ʧ������
	DWORD							dwDrawCount;						//�;�����
	DWORD							dwFleeCount;						//��������
	DWORD							dwPlayTimeCount;					//��Ϸʱ��

	//ȫ����Ϣ
	DWORD							dwUserMedal;						//�û�����
	DWORD							dwExperience;						//�û�����
	LONG							lLoveLiness;						//�û�����
};

//��Ϸ��¼
struct tagGameScoreRecord
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	WORD							wChairID;							//���Ӻ���
	BYTE							cbAndroid;							//������־

	//�ɼ���Ϣ
	SCORE							lScore;								//�û�����
	SCORE							lGrade;								//�û��ɼ�
	SCORE							lRevenue;							//��Ϸ˰��
	SCORE							lControlScore;						//�û�����ֵ
	SCORE							lChoushui;							//��ˮ
	DWORD							dwChoushuiType;						//��ˮ����

	//�û���Ϣ
	DWORD							dwDBQuestID;						//�����ʶ
	DWORD							dwInoutIndex;						//��¼����

	//������Ϣ
	DWORD							dwUserMemal;						//������Ŀ
	DWORD							dwPlayTimeCount;					//��Ϸʱ��
};

//�û�����
struct tagUserRule
{
	//�����־
	bool							bLimitSameIP;						//Ч���ַ
	bool							bLimitWinRate;						//����ʤ��
	bool							bLimitFleeRate;						//��������
	bool							bLimitGameScore;					//���Ʒ���

	//��������
	WORD							wMinWinRate;						//���ʤ��
	WORD							wMaxFleeRate;						//�������
	LONG							lMaxGameScore;						//��߷��� 
	LONG							lMinGameScore;						//��ͷ���
	TCHAR							szPassword[LEN_PASSWORD];			//��������
};

//�û���Ϣ TODONOW added by WangChengQing ����ֶο���ɾ�� 2018/5/12
//���´� ������ʱ��ɾ��, ���ڲ�ɾ�� ����Ϊ�������Ϸ��Ӱ��(��������)
struct tagUserInfoPlus
{
	//��¼��Ϣ
	DWORD							dwLogonTime;						//��¼ʱ��
	DWORD							dwInoutIndex;						//������ʶ

	//������Ϣ
	WORD							wBindIndex;							//������
	DWORD							dwClientAddr;						//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ

	//���ӱ���
	DWORD							dwUserRight;						//�û�Ȩ��
	DWORD							dwMasterRight;						//����Ȩ��
	SCORE							lRestrictScore;						//���ƻ���

	//��������
	bool							bAndroidUser;						//�����û�
	TCHAR							szPassword[LEN_MD5];				//��������
};

//������
struct tagRequestResult
{
	BYTE							cbFailureCode;						//ʧ��ԭ��
	TCHAR							szFailureReason[128];				//ʧ��ԭ��
};

//����״��
struct tagTableUserInfo
{
	//WORD							wMinUserCount;						//��������
	WORD							wTableUserCount;					//�û���Ŀ
	WORD							wTableReadyCount;					//׼����Ŀ
	WORD							wTableAndroidCount;					//������Ŀ
};

//����״��
struct tagAndroidUserInfo
{
	WORD							wFreeUserCount;						//�û���Ŀ
	WORD							wPlayUserCount;						//�û���Ŀ
	WORD							wSitdownUserCount;					//�û���Ŀ
};

//������Ϣ
struct tagAndroidService
{
	DWORD							dwResidualPlayDraw;					//ʣ�����
	DWORD                           dwReposeTime;                       //����ʱ�� 
};

//��������
struct tagAndroidParameter
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwServiceTime;						//����ʱ��
	SCORE							lMinTakeScore;						//Я������
	SCORE							lMaxTakeScore;						//Я������
	DWORD							dwMinPlayDraw;						//��Ϸ����
	DWORD							dwMaxPlayDraw;						//��Ϸ����
	DWORD							dwMinReposeTime;					//��Ϣʱ��
	DWORD							dwMaxReposeTime;					//��Ϣʱ��
	DWORD							dwServiceGender;					//��������
};

//����״̬
enum EN_MatchStatus
{
	EN_NULL=0,
	EN_OUT,
};
//////////////////////////////////////////////////////////////////////////////////
//��Ϸ�ӿ�
interface IGameServiceManager : public IUnknownEx
{
	//�����ӿ�
public:
	//��������
	virtual VOID * CreateTableFrameSink(REFGUID Guid, DWORD dwQueryVer)=NULL;
	//��������
	virtual VOID * CreateAndroidUserItemSink(REFGUID Guid, DWORD dwQueryVer)=NULL;
	//��������
	virtual VOID * CreateGameDataBaseEngineSink(REFGUID Guid, DWORD dwQueryVer)=NULL;

	//�����ӿ�
public:
	//�������
	virtual bool GetServiceAttrib(tagGameServiceAttrib & GameServiceAttrib)=NULL;
	//��������
	virtual bool RectifyParameter(tagGameServiceOption & GameServiceOption)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////
//���ýӿ�
interface IGameServiceCustomRule : public IUnknownEx
{
	//��ȡ����
	virtual bool SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)=NULL;
	//Ĭ������
	virtual bool DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)=NULL;
	//��������
	virtual HWND CreateCustomRule(CWnd * pParentWnd, CRect rcCreate, LPBYTE pcbCustomRule, WORD wCustonSize)=NULL;
};


//////////////////////////////////////////////////////////////////////////////////
//���ݽӿ�
interface IGameDataBaseEngineSink : public IUnknownEx
{
	//���ýӿ�
	virtual bool InitializeSink(IUnknownEx * pIUnknownEx)=NULL;
	//���ݴ���
	virtual bool OnGameDataBaseRequest(DWORD dwUserID, WORD wTableID, WORD wChairID);
};

//////////////////////////////////////////////////////////////////////////////////

interface IGameMatchSink;

//���ӽӿ�
interface ITableFrame : public IUnknownEx
{
	//���ò���
public:
	//��ȡͨ�÷������
	virtual VOID* GetCustomRule() =NULL;
	//��ȡ����Ϸ���з������
	virtual VOID * GetSubGameRule()=NULL;

	//д�ֽӿ�
public:
	//д��¼���¼ ���� С����,���ݺͳ���
	virtual bool WriteRecordInfo(WORD wXJCount,TCHAR strScore[],  VOID* pData, DWORD dwDataSize) = NULL;
	//����С�ֽ���
	virtual bool HandleXJGameEnd(BYTE cbCurGameCount, BYTE cbGameMode, SCORE *lGameScore) = NULL;

	//ʱ��ӿ�
public:
	//����ʱ��
	virtual bool SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter)=NULL;
	//ɾ��ʱ��
	virtual bool KillGameTimer(DWORD dwTimerID)=NULL;
	//��ȡ��ʱ��ʣ�������
	virtual DWORD GetTimerLeftTickCount(DWORD dwTimerID)=NULL;

	//���ܽӿ�
public:
	//���ͳ���
	virtual bool SendGameScene(CPlayer * pIServerUserItem, VOID * pData, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////
//�ص��ӿ�
interface ITableFrameSink : public IUnknownEx
{
	//����ӿ�
public:
	//��λ�ӿ�
	virtual VOID RepositionSink()=NULL;
	//���ýӿ�
	virtual bool Initialization(IUnknownEx * pIUnknownEx)=NULL;

	//��ѯ�ӿ�
public:
	//���ٻ���
	virtual SCORE QueryLessEnterScore(WORD wChairID, CPlayer * pIServerUserItem)=NULL;
	//��ѯ�Ƿ�۷����
	virtual bool QueryBuckleServiceCharge(WORD wChairID)=NULL;

	//��Ϸ�¼�
public:
	//��Ϸ��ʼ
	virtual bool OnEventGameStart()=NULL;
	//��Ϸ����
	virtual bool OnEventGameConclude(WORD wChairID, CPlayer * pIServerUserItem, BYTE cbReason)=NULL;
	//���ͳ���
	virtual bool OnEventSendGameScene(WORD wChairID, CPlayer * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)=NULL;

	//�¼��ӿ�
public:
	//ʱ���¼�
	virtual bool OnTimerMessage(DWORD dwTimerID, WPARAM dwBindParameter)=NULL;
	//�����¼�
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize)=NULL;
	//����ֵ�ı�
	virtual void OnUserControlScoreChange( WORD chair_id, CPlayer * server_user_item, SCORE val ){};
	//��Ϸ�̶ȸı�yang
	virtual void SetGameLevel(DWORD RoomID, DWORD val){};

	//�ʽ�
public: 
	//�ʽ�������
	virtual bool OnRoomPrizePoolResualt(VOID * pData, WORD wDataSize){return true;};
	//�ܲʽ�������
	virtual bool OnGamePrizePoolResualt(VOID * pData, WORD wDataSize){return true;};
	//�ʽ��н�����
	virtual bool OnPrizePoolRewardResualt(VOID * pData, WORD wDataSize){return true;};
	//����ӿ�
public:
	//��Ϸ��Ϣ
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, CPlayer * pIServerUserItem)=NULL;
	//�����Ϣ
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, CPlayer * pIServerUserItem)=NULL;

	//�����ӿ�
public:
	//���û���
	virtual void SetGameBaseScore(LONG lBaseScore)=NULL;

public:
	virtual bool OnActionUserSitDownT(WORD chair_id, CPlayer* server_user_item, bool lookon_user){ return true; }
	virtual bool OnActionUserStandUpT(WORD chair_id, CPlayer* server_user_item, bool lookon_user){ return true; }
	virtual bool OnActionUserOnReadyT(WORD chair_id, CPlayer * server_user_item, void* data, WORD data_size){ return true; }
	virtual bool OnActionUserNetCutT(WORD wChairID, CPlayer * pIServerUserItem, bool bLookonUser){ return true; }

	//�û�����
public:
	//�û�����
	virtual bool OnActionUserSitDown(WORD wChairID, CPlayer * pIServerUserItem, bool bLookonUser)=NULL;
	//�û�����
	virtual bool OnActionUserStandUp(WORD wChairID, CPlayer * pIServerUserItem, bool bLookonUser)=NULL;
	//�û�ͬ��
	virtual bool OnActionUserOnReady(WORD wChairID, CPlayer * pIServerUserItem, VOID * pData, WORD wDataSize)=NULL;
	//�û�����
	virtual bool OnActionUserNetCut(WORD wChairID, CPlayer * pIServerUserItem, bool bLookonUser){ return true; }

	//�û�����
public:
	//�Թ�����
	virtual bool OnUserRequestLookon(WORD wChairID, CPlayer * pIServerUserItem, tagRequestResult & RequestResult)=NULL;
	//��������
	virtual bool OnUserRequestSitDown(WORD wChairID, CPlayer * pIServerUserItem, tagRequestResult & RequestResult)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////
//������
interface IMainServiceFrame : public IUnknownEx
{
#pragma region DB�¼�֪ͨ
	//�����˿���
public:
	//�����˿�����ʼ
	virtual void StartTable(DWORD dwTableID) = NULL;
	//�û����������˿���
	virtual void JoinTable(DWORD dwTableID, DWORD dwUserID) = NULL;
	//�û��뿪�����˿���
	virtual void LeaveTable(DWORD dwTableID, DWORD dwUserID) = NULL;

	//CLUB����Ȧ2  && ��Ҵ���
public:
	//�û�����
	virtual void ClubPlayerSitDown(DWORD dwTableID, DWORD dwUserID, BYTE byChairID, BYTE byClubOrHallGold) = NULL;
	//�û�����
	virtual void ClubPlayerSitUp(DWORD dwTableID, DWORD dwUserID, BYTE byChairID, BYTE byClubOrHallGold) = NULL;
	//���һ���û�����
	virtual void ClubLastPlayerSitUp(DWORD dwTableID, DWORD dwUserID, BYTE byChairID, BYTE byClubOrHallGold)= NULL;
	//�û�����
	virtual void ClubPlayerOffline(DWORD dwTableID, DWORD dwUserID, BYTE byChairID, BYTE byClubOrHallGold) = NULL;

	//�������� -- ���ֲ�����
	virtual void ClubTableCreate(DWORD dwClubRoomID, DWORD dwUserID, DWORD dwTableID, DWORD dwLockState) = NULL;
	//���ӿ�ʼ��Ϸ
	virtual void ClubTableStart(DWORD dwTableID, BYTE byClubOrHallGold) = NULL;
	//������������
	virtual void ClubTableMax(DWORD dwTableID, BYTE byClubOrHallGold) = NULL;
	//��������û����
	virtual void ClubTableNotMax(DWORD dwTableID, BYTE byClubOrHallGold) = NULL;
	//����С�ֽ�����Ϸ
	virtual void ClubTableXJ(DWORD dwTableID) = NULL;
	//���Ӵ�ֽ�����Ϸ
	virtual void ClubTableDJ(DWORD dwTableID) = NULL;
#pragma endregion
};

#endif