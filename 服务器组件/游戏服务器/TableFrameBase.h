#ifndef GAME_SERVICE_HEAD_HEAD_FILE
#define GAME_SERVICE_HEAD_HEAD_FILE

/////////////////////////////////////////////////////////////////////
//����ͨ����  

//�û��ӿ�
interface IServerUserItem
{
//	//������Ϣ
//public:
//	//�û�����
//	virtual WORD GetBindIndex() = NULL;
//	//�û���ַ
//	virtual DWORD GetClientAddr() = NULL;
//	//������ʶ
//	virtual LPCTSTR GetMachineID() = NULL;
//	//���ö��߱�־ GameID  TODONOW
//	virtual void SetOfflineGameID(DWORD) = NULL;
//	//��ȡ���߱�־ GameID  TODONOW
//	virtual DWORD GetOfflineGameID() = NULL;
//
//	//��¼��Ϣ
//public:
//	//�����ʶ
//	virtual DWORD GetDBQuestID() = NULL;
//	//��¼ʱ��
//	virtual DWORD GetLogonTime() = NULL;
//	//��¼����
//	virtual DWORD GetInoutIndex() = NULL;
//
//	//�û���Ϣ
//public:
//	//�û���Ϣ
//	virtual tagUserInfo * GetUserInfo() = NULL;
//	//������Ϣ
//	virtual tagUserProperty * GetUserProperty() = NULL;
//
//	//������Ϣ
//public:
//	//�û��Ա�
//	virtual BYTE GetGender() = NULL;
//	//�û���ʶ
//	virtual DWORD GetUserID() = NULL;
//	//�û��ǳ�
//	virtual LPCTSTR GetNickName() = NULL;
//
//	//�û�����֮ǰ��״̬
//	virtual void SetOldGameStatus(BYTE gamestatus) = NULL;
//
//	//�û�����֮ǰ��״̬
//	virtual BYTE GetOldGameStatus() = NULL;
//
//	//״̬�ӿ�
public:
//	//���Ӻ���
//	virtual WORD GetTableID() = NULL;
//	//���Ӻ���
	virtual WORD GetChairID() = NULL;
//	//�û�״̬
//	virtual BYTE GetUserStatus() = NULL;
//
//	//Ȩ����Ϣ
//public:
//	//�û�Ȩ��
//	virtual DWORD GetUserRight() = NULL;
//	//����Ȩ��
//	virtual DWORD GetMasterRight() = NULL;
//
//	//Ȩ����Ϣ
//public:
//	//�û�Ȩ��
//	virtual BYTE GetMemberOrder() = NULL;
//	//����Ȩ��
//	virtual BYTE GetMasterOrder() = NULL;
//
//	//������Ϣ
//public:
//	//�û�����
//	virtual SCORE GetUserScore() = NULL;
//	//�û��ɼ�
//	virtual SCORE GetUserGrade() = NULL;
//
//	//����û�����
//	virtual SCORE GetUserRoomCard() = NULL;
//	//�����û�����
//	virtual void SetUserRoomCard(SCORE lRoomCard) = NULL;
//
//	//����û���ʯ
//	virtual SCORE GetUserDiamond() = NULL;
//	//�����û���ʯ
//	virtual void SetUserDiamond(SCORE lDiamod) = NULL;
//
//	//����û����
//	virtual SCORE GetUserGold() = NULL;
//	//�����û����
//	virtual void SetUserGold(SCORE lGold) = NULL;
//
//	//�û�����ֵ
//	virtual SCORE GetUserControlScore() = NULL;
//	virtual VOID  SetUserControlScore(SCORE val) = NULL;
//
//
//	//�й���Ϣ
//public:
//	//�йܻ���
//	virtual SCORE GetTrusteeScore() = NULL;
//	//��������
//	virtual SCORE GetFrozenedScore() = NULL;
//
//	//������Ϣ
//public:
//	//�û�ʤ��
//	virtual WORD GetUserWinRate() = NULL;
//	//�û�����
//	virtual WORD GetUserLostRate() = NULL;
//	//�û�����
//	virtual WORD GetUserDrawRate() = NULL;
//	//�û�����
//	virtual WORD GetUserFleeRate() = NULL;
//	//��Ϸ����
//	virtual DWORD GetUserPlayCount() = NULL;
//
//	//Ч��ӿ�
//public:
//	//�Ա��ʺ�
//	virtual bool ContrastNickName(LPCTSTR pszNickName) = NULL;
//
//	//��Ϸ״̬
//public:
//	//����״̬
//	virtual bool IsClientReady() = NULL;
//	//��������
//	virtual VOID SetClientReady(bool bClientReady) = NULL;
//
//	//�����û�
//public:
//	//����״̬
//	virtual bool IsAndroidUser() = NULL;
//	//���ÿ���
//	virtual VOID SetAndroidUser(bool bbMachineUser) = NULL;
//
//public:
//	//��¼�ӿ�
//public:
//	//����ж�
//	virtual bool IsVariation() = NULL;
//	//��ѯ��¼
//	//virtual bool QueryRecordInfo(tagVariationInfo & UserRecordInfo) = NULL;
//	//��ȡ���
//	//virtual bool DistillVariation(tagVariationInfo & UserVariationInfo) = NULL;
//
//	//����ӿ�
//public:
//	//����״̬
//	virtual bool SetUserStatus(BYTE cbUserStatus, WORD wTableID, WORD wChairID, bool bNotify = true) = NULL;
//	//�޸�Ȩ��
//	virtual VOID ModifyUserRight(DWORD dwAddRight, DWORD dwRemoveRight, bool bGameRight = true) = NULL;
//
//	//�����û��Ƹ���Ϣ
//	virtual bool ModifyUserTreasure(DWORD dwTableID, BYTE byTableMode, BYTE byRound, SCORE lUserTreasuse, BYTE byWin) = NULL;
//
//	//�߼��ӿ�
//public:
//	//�����
//	virtual bool DetachBindStatus() = NULL;
//	//���в���
//	virtual bool ModifyUserInsure(SCORE lScore, SCORE lInsure, SCORE lRevenue) = NULL;
//	//���ò���
//	virtual bool SetUserParameter(DWORD dwClientAddr, WORD wBindIndex, TCHAR szMachineID[LEN_MACHINE_ID], bool bAndroidUser,
//		bool bClientReady, const double &dLongitude, const double &dLatitude) = NULL;
//
//	//�ֻ�����
//public:
//	//�ֻ�����
//	virtual WORD GetMobileUserRule() = NULL;
//	//���ö���
//	virtual VOID SetMobileUserRule(WORD wMobileUserRule) = NULL;
//	//��ǰ��ҳ
//	virtual WORD GetMobileUserDeskPos() = NULL;
//	//��ǰ��ҳ
//	virtual VOID SetMobileUserDeskPos(WORD wMobileUserDeskPos) = NULL;
//	//��ҳ����
//	virtual WORD GetMobileUserDeskCount() = NULL;
//	//��ҳ����
//	virtual VOID SetMobileUserDeskCount(WORD wMobileUserDeskCount) = NULL;
//
//public:
//	//����Ϊ�����û�
//	virtual VOID SetVirtualUser(bool bVirtual) = NULL;
//	//��ȡ�Ƿ�Ϊ�����û�
//	virtual bool IsVirtualUser() = NULL;

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
	// 1���; 2˽��; 3����
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
	virtual bool SendTableData(WORD wChairID, WORD wSubCmdID) = NULL;
	//��������
	virtual bool SendTableData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize, WORD wMainCmdID = MDM_GF_GAME) = NULL;

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
	//��λ�ӿ�
	virtual VOID RepositionSink() = NULL;
	//���ýӿ�
	virtual bool Initialization(IUnknownEx * pIUnknownEx) = NULL;

	//��ѯ�ӿ�
public:
	//��ѯ�޶�
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem) = NULL;
	//���ٻ���
	virtual SCORE QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem) = NULL;
	//��ѯ�Ƿ�۷����
	virtual bool QueryBuckleServiceCharge(WORD wChairID) = NULL;

	//��Ϸ�¼�
public:
	//��Ϸ��ʼ
	virtual bool OnEventGameStart() = NULL;
	//��Ϸ����
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason) = NULL;
	//���ͳ���
	virtual bool OnEventSendGameScene(WORD wChairID, BYTE cbGameStatus, bool bSendSecret) = NULL;

	//�¼��ӿ�
public:
	//ʱ���¼�
	virtual bool OnTimerMessage(DWORD dwTimerID, WPARAM dwBindParameter) = NULL;
	//�����¼�
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize) = NULL;
	//�����¼�
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason) = NULL;

	//����ӿ�
public:
	//��Ϸ��Ϣ
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, WORD wChairID) = NULL;
	//�����Ϣ
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, WORD wChairID) = NULL;

	//�����ӿ�
public:
	//���û���
	virtual void SetGameBaseScore(LONG lBaseScore) = NULL;

public:
	//�û�����
	virtual bool OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser) = NULL;
	//�û�����
	virtual bool OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser) = NULL;
	//�û�׼��
	virtual bool OnActionUserOnReady(WORD wChairID, IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize) = NULL;

};

#endif