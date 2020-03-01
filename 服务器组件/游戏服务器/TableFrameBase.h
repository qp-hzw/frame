#ifndef TABLE_FRAME_BASE_H
#define TABLE_FRAME_BASE_H

#include <string>
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
	BYTE	GameMode;				//��Ϸģʽ 0����Լ��; 1����ģʽ; 2���ģʽ; 3���Լ��; 4����Ȧ
	BYTE    GameRoomLevel;          //����ȼ� ����ҳ�ר�ã�

	BYTE	GameCount;				//��Ϸ���� 0-���޾�
	BYTE	PlayerCount;			//������� 0-���������ɿ�

	BYTE	cbPayType;				//֧����ʽ��0����֧����1AA�� -- ����ģʽ����Ҫ

	BYTE	bRefuseSameIP;			//����ͬIP    0-������ 1-����
	BYTE	bDistanceIn300;			//����300��	  0-������ 1-����
	BYTE	bAllowStranger;			//����İ���˼���
};

//���᷿�����
struct tagClubRoomRule
{
    DWORD	dwClubID;				//����
	DWORD	dwRoomID;				//����
	DWORD   dwPasswd;               //����

	BYTE	byGoldOrFK;				//(2.��� 1.����)

	BYTE	bDissolve;				//�Ƿ������ɢ 0���� 1������

	//��ҷ�����
	DWORD	dwAmount;				//��Ͷ��
	DWORD	dwOwnerPercentage;		//Ⱥ�����
    
	BYTE	byMask;					//1 AA֧��;  2��Ӯ��֧��
	DWORD	dwDizhu;				//��ע
};

/////////////////////////////////////////////////////////////////////

//���ӽӿ�
class ITableFrame : public IUnknownEx
{
public:
	//��Ϸģʽ 0-����ģʽ; 1-����ģʽ;  2-���ģʽ;  3-�������;
	virtual int GameType() = 0;
	//is roboot
	virtual bool IsRobot(WORD wChairID) = 0;

	//���̽ӿ�
public:
	//����С�ֽ���
	virtual bool HandleXJGameEnd(BYTE cbCurGameCount, WORD *wIdentity, SCORE *lGameScore) = NULL;
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
	virtual bool SendTableData(WORD wChairID, WORD wSubCmdID, void * pData, WORD wDataSize, WORD wMainCmd =200) = NULL;
	//���ͳ���
	virtual bool SendGameScene(WORD wChairID, VOID * pData, WORD wDataSize) = NULL;
};

//////////////////////////////////////////////////////////////////////////////////
//����Ϸ�ӿ�
class ITableFrameSink : public IUnknownEx
{
	//����ӿ�
public:
	//��������
	virtual bool StartService(){return true;}
	//ֹͣ����
	virtual bool ConcludeService(){return true;}

	//����ӿ�
public:
	//���ýӿ�
	virtual bool Initialization(ITableFrame *pTableFrame, tagTableRule *comRule) = NULL;

	//��Ϸ�¼�
public:
	//��Ϸ��ʼ
	virtual bool OnEventGameStart() = NULL;
	//��Ϸ����
	virtual bool OnEventGameConclude(BYTE cbReason) = NULL;
	//���ͳ���
	virtual bool OnEventSendGameScene(WORD wChairID) = NULL;

	//�¼��ӿ�
public:
	//ʱ���¼�
	virtual bool OnTimerMessage(DWORD dwTimerID, WPARAM dwBindParameter) = NULL;
	//��Ϸ��Ϣ
	virtual bool OnGameMessage(WORD wSubCmdID, void * pData, WORD wDataSize, WORD wChairID) = NULL;
};

//����Ϸ�����ȡ
class ISubRoomRuleManager : public IUnknownEx
{
	//����ӿ�
public:
	//��������
	virtual bool StartService(){return true;}
	//ֹͣ����
	virtual bool ConcludeService(){return true;}

	//
public:
	//�����ֶ�����, Ϊ�ṹ���Ӧ�ֶθ�ֵ
	virtual void SetRoomRule(std::string key, std::string value)= NULL;
	//
	virtual void Init() = NULL;
	//����Gold������
	virtual void SetGoldRule(BYTE byType) = NULL;
	//����match����
	virtual void SetMatchRule(VOID *config) = NULL;
};

#endif