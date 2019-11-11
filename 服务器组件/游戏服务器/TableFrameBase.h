#ifndef GAME_SERVICE_HEAD_HEAD_FILE
#define GAME_SERVICE_HEAD_HEAD_FILE

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
	virtual bool SendGameScene(WORD  pIServerUserItem_chair, VOID * pData, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////
//�ص��ӿ�
interface ITableFrameSink : public IUnknownEx
{
	//��Ϸ�¼�
public:
	//��Ϸ��ʼ
	virtual bool OnEventGameStart()=NULL;
	//��Ϸ����
	virtual bool OnEventGameConclude(WORD wChairID, WORD pIServerUserItem_chair, BYTE cbReason)=NULL;
	//���ͳ���
	virtual bool OnEventSendGameScene(WORD wChairID, BYTE cbGameStatus, bool bSendSecret)=NULL;

	//�¼��ӿ�
public:
	//ʱ���¼�
	virtual bool OnTimerMessage(DWORD dwTimerID, WPARAM dwBindParameter)=NULL;
	//�����¼�
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize)=NULL;

	//����ӿ�
public:
	//��Ϸ��Ϣ
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, WORD  pIServerUserItem_chair)=NULL;
	//�����Ϣ
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, WORD  pIServerUserItem_chair)=NULL;

};

#endif