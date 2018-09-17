#ifndef TABLE_FRAME_HEAD_FILE
#define TABLE_FRAME_HEAD_FILE

#include "Stdafx.h"
#pragma pack(1)
//////////////////////////////////////////////////////////////////////////////////
#define GAME_CONCLUDE_CONTINUE  0xFF //大局结束并续费
#define GAME_CONCLUDE_NORMAL    0xFE //正常结束

//数组定义
typedef IServerUserItem * CTableUserItemArray[MAX_CHAIR];				//游戏数组
typedef CWHArray<IServerUserItem *> CLookonUserItemArray;				//旁观数组
typedef CWHArray<tagGameScoreRecord *> CGameScoreRecordArray;			//记录数组
interface IMatchTableFrameNotify;

//子游戏房间规则
struct tagTableSubGameRule
{
	BYTE SubGameRule[LEN_PRIVATE_TABLE_RULE/2];						//子游戏房间规则
};
//////////////////////////////////////////////////////////////////////////////////

//桌子框架
class CTableFrame : public ITableFrame
{

#pragma region 成员变量
	//游戏属性
protected:
	WORD							m_wTableID;							//桌子号码		从0开始，在桌子初始化的时候已经赋值
	WORD							m_wChairCount;						//椅子数目
	BYTE							m_cbStartMode;						//开始模式
	WORD							m_wUserCount;						//用户数目
	DWORD							m_dwTableOwner;						//桌主用户（第一个坐下的玩家）
	DWORD							m_dwCreateTableUser;				//创建桌子用户
	DWORD							m_dwGroupID;						//所在牌友圈ID
	BYTE							m_cbTableMode;						//桌子游戏模式

	tagTableRule					m_tagTableRule;						//通用房间规则(client传给服务端大厅的)
	tagTableSubGameRule				m_tagTableSubGameRule;				//子游戏特有房间规则(client传给子游戏的)

	//状态变量
protected:
	bool							m_bGameStarted;						//游戏开始标志
	bool							m_bDrawStarted;						//游戏标志
	bool							m_bTableStarted;					//游戏标志

	//状态变量
protected:
	bool							m_bAllowLookon[MAX_CHAIR];			//旁观标志
	SCORE							m_lFrozenedScore[MAX_CHAIR];		//服务费用

	//游戏变量
protected:
	BYTE							m_cbGameStatus;						//游戏状态

	//房间解散添加
	bool							m_bAgree[MAX_CHAIR];					//同意解散
	bool							m_bResponseDismiss[MAX_CHAIR];			//响应解散
	bool							m_bUnderDissState;						//桌子是否处于解散状态

	//时间变量
protected:
	DWORD							m_dwDrawStartTime;					//开始时间
	SYSTEMTIME						m_SystemTimeStart;					//开始时间
	WORD                            m_wDrawCount;                       //游戏局数

	//动态属性
protected:	
	TCHAR							m_szEnterPassword[LEN_PASSWORD];	//进入密码		TODO 这个密码有什么用
	DWORD							m_dwPassword;						//6位房卡房间密码


	//用户数组
protected:
	CTableUserItemArray				m_TableUserItemArray;				//游戏用户
	CLookonUserItemArray			m_LookonUserItemArray;				//旁观用户

	//组件接口
protected:
	ITimerEngine *					m_pITimerEngine;					//时间引擎
	ITableFrameSink	*				m_pITableFrameSink;					//桌子接口
	IMainServiceFrame *				m_pIMainServiceFrame;				//服务接口
	IAndroidUserManager *			m_pIAndroidUserManager;				//机器接口

	//扩展接口
protected:
	ITableUserAction *				m_pITableUserAction;				//动作接口
	ITableUserRequest *				m_pITableUserRequest;				//请求接口
	ITableUserAction *				m_pIMatchTableAction;				//动作接口(比赛用)

	ITCPSocketService *				m_pITCPSocketService;				//网络服务


	//数据接口
protected:
	IDataBaseEngine *				m_pIKernelDataBaseEngine;			//内核数据
	IDataBaseEngine *				m_pIRecordDataBaseEngine;			//记录数据

	//配置信息
protected:
	tagGameParameter *				m_pGameParameter;					//配置参数
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//服务属性
	tagGameServiceOption *			m_pGameServiceOption;				//服务配置

	//游戏记录
protected:
	CGameScoreRecordArray			m_GameScoreRecordActive;			//游戏记录
	static CGameScoreRecordArray	m_GameScoreRecordBuffer;			//游戏记录

	//比赛接口
protected:
	IGameMatchSink					* m_pIGameMatchSink;				//比赛接口

	//
protected:
	bool							m_ConrtolValueEnabled;				//是否接受房间控制值影响
	static SCORE					m_ControlValueForRoom;				//房间控制值yang
	static SCORE					m_ControlValueForRoomOrgi;			//房间控制值yang
#pragma endregion

#pragma region 初始化函数
	//函数定义
public:
	//构造函数
	CTableFrame();
	//析构函数
	virtual ~CTableFrame();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);
#pragma endregion

#pragma region 桌子属性设置与获取
public:
	//ServerID
	virtual DWORD GetServerID() { return m_pGameServiceOption->dwServerID; } 
	//桌子编号
	virtual WORD GetTableID() { return m_wTableID; }
	//游戏人数
	virtual WORD GetChairCount() { return m_wChairCount; }
	//空位置数目
	virtual WORD GetNullChairCount(){return m_wChairCount - m_wUserCount;}

	//服务属性
	virtual tagGameServiceAttrib * GetGameServiceAttrib() { return m_pGameServiceAttrib; }
	//服务配置
	virtual tagGameServiceOption * GetGameServiceOption() { return m_pGameServiceOption; }

	//开始模式
	virtual BYTE GetStartMode() { return m_cbStartMode; }
	//开始模式
	virtual VOID SetStartMode(BYTE cbStartMode) { m_cbStartMode=cbStartMode; }

	//获取状态
	virtual BYTE GetGameStatus() { return m_cbGameStatus; }
	//设置状态
	virtual VOID SetGameStatus(BYTE cbGameStatus) { m_cbGameStatus=cbGameStatus; }

	//游戏状态
	virtual bool IsGameStarted() { return m_bGameStarted; }
	//游戏状态
	virtual bool IsDrawStarted() { return m_bDrawStarted; }
	//游戏状态
	virtual bool IsTableStarted() { return m_bTableStarted; }
	//锁定状态
	virtual bool IsTableLocked() { return (m_szEnterPassword[0]!=0); }

	//设置桌主，第一个坐下的人
	virtual void SetTableOwner(DWORD dwUserID);
	//获得桌主，第一个坐下的人
	virtual DWORD GetTableOwner();

	//获取房间模式
	virtual BYTE GetTableMode(){return m_cbTableMode;};
	//设置房间模式
	virtual void SetTableMode(BYTE mode){ m_cbTableMode = mode; };

	//设置创建桌子的用户
	virtual void SetCreateTableUser(DWORD dwUserID){m_dwCreateTableUser = dwUserID;};
	virtual DWORD GetCreateTableUser(){return m_dwCreateTableUser;};

	//获取、设置用户圈房卡
	virtual void SetGroupID(DWORD dwGroupID){m_dwGroupID = dwGroupID;};
	virtual DWORD GetGroupID(){return m_dwGroupID;};

	//读取房间通用规则
	virtual VOID * GetCustomRule() { return &m_tagTableRule;};
	//设置房间通用规则
	virtual void SetCommonRule(tagTableRule* pRule)
	{
		memcpy(&m_tagTableRule, pRule, sizeof(m_tagTableRule));
		m_wChairCount = pRule->PlayerCount;
		SetTableMode(pRule->GameMode);
	};

	//读取子游戏特有房间规则
	virtual VOID * GetSubGameRule() { return &m_tagTableSubGameRule;};
	//设置子游戏特有房间规则
	virtual void SetSubGameRule(tagTableSubGameRule* pRule)
	{
		memcpy(&m_tagTableSubGameRule, pRule, sizeof(m_tagTableSubGameRule));
	};
	
	//获取密码
	virtual DWORD GetPassword(){ return m_dwPassword; }
	//设置密码
	virtual void SetPassword(DWORD dwPassword){ m_dwPassword=dwPassword; }

	//设置房间自动解散时间 added by lizhihu
	virtual void SetTableAutoDismiss(DWORD dwMinutes = 1);
#pragma endregion

#pragma region 桌子状态函数
	//自身调用，与子游戏无关
public:
	//开始游戏
	virtual bool StartGame();
	//解散游戏
	virtual bool DismissGame();
	//结束桌子
	virtual bool ConcludeTable();
#pragma endregion

#pragma region 与子游戏交互函数
	//与子游戏交互函数
public:
	//小局结束处理函数
	virtual bool HandleXJGameEnd(BYTE cbCurGameCount, BYTE cbGameMode, SCORE *lGameScore);
	//大局结束处理函数
	virtual bool ConcludeGame(BYTE cbGameStatus);

	//写入录像记录(战绩回放) 参数 小局数,数据和长度
	bool WriteRecordInfo(WORD wXJCount,TCHAR strScore[], VOID* pData, DWORD dwDataSize);

	//TODONOW 下次迭代删除  查询限额
	virtual SCORE QueryConsumeQuota(IServerUserItem * pIServerUserItem);

	
	//小局大局结束辅助函数
protected:	
	//每局游戏结束后, 判断并扣除用户门票
	bool XJGameTickets(BYTE byTableMode, BYTE byRound);
	//每局游戏结束后，更新用户财富信息
	bool XJModifyUserTreasure(BYTE byTableMode, BYTE byRound, SCORE *lGameScore, tagTableRule *pCfg);	
	//每局游戏结束后, 检测用户财富是否可以继续游戏
	bool CheckUserLeave(BYTE byTableMode, IServerUserItem* pIServerUserItem, tagTableRule* pCfg);
	//每局游戏结束后，更新游戏任务状态
	bool XJUpdateGameTaskStatus(const BYTE &cbTableMode, const BYTE &cbCurGameCount);
#pragma endregion

#pragma region 定时器
	//时间接口
public:
	//设置时间
	virtual bool SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter);
	//删除时间
	virtual bool KillGameTimer(DWORD dwTimerID);
	//获取定时器剩余毫秒数
	virtual DWORD GetTimerLeftTickCount(DWORD dwTimerID);

#pragma endregion

#pragma region 消息发送函数
	//游戏用户
public:
	//发送数据
	virtual bool SendTableData(WORD wChairID, WORD wSubCmdID);
	//发送数据
	virtual bool SendTableData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize,WORD wMainCmdID=MDM_GF_GAME);

	//发送机器人用户
public:
	//发送数据
	virtual bool SendAndroidUserData(WORD wChairID, WORD wSubCmdID);
	//发送数据
	virtual bool SendAndroidUserData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize,WORD wMainCmdID=MDM_GF_GAME);

	//旁观用户
public:
	//发送数据
	virtual bool SendLookonData(WORD wChairID, WORD wSubCmdID);
	//发送数据
	virtual bool SendLookonData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//所有用户
public:
	//发送数据
	virtual bool SendUserItemData(IServerUserItem * pIServerUserItem, WORD wSubCmdID);
	//发送数据
	virtual bool SendUserItemData(IServerUserItem * pIServerUserItem, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//系统消息
public:
	//发送消息
	virtual bool SendGameMessage(LPCTSTR lpszMessage, WORD wType);
	//游戏消息
	virtual bool SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType);
	//房间消息
	virtual bool SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType);
#pragma endregion

#pragma region 用户接口
	//用户接口
public:
	//寻找用户 -- 旁观用户 +游戏用户
	virtual IServerUserItem * SearchUserItem(DWORD dwUserID);
	//游戏用户
	virtual IServerUserItem * GetTableUserItem(WORD wChairID);
	//旁观用户
	virtual IServerUserItem * EnumLookonUserItem(WORD wEnumIndex);

	//动作处理
public:
	//起立动作
	virtual bool PerformStandUpAction(IServerUserItem * pIServerUserItem);
	//旁观动作
	virtual bool PerformLookonAction(WORD wChairID, IServerUserItem * pIServerUserItem);
	//坐下动作		只有创建和加入房间时，用户坐下时需要校验GPS，故加此字段
	virtual bool PerformSitDownAction(WORD wChairID, IServerUserItem * pIServerUserItem, LPCTSTR lpszPassword=NULL, bool bCheckUserGPS = false);
#pragma endregion

	//功能接口
public:
	//发送场景
	virtual bool SendGameScene(IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);

#pragma region 比赛接口
	//比赛接口
public:
	virtual bool SetMatchInterface(IUnknownEx * pIUnknownEx);
	//获取比赛桌子接口
	virtual IGameMatchSink* GetGameMatchSink(){return m_pIGameMatchSink;}
#pragma endregion

	//功能函数
public:
	//获取空位
	WORD GetNullChairID();
	//随机空位
	WORD GetRandNullChairID();
	//用户数目
	WORD GetSitUserCount();
	//旁观数目
	WORD GetLookonUserCount();
	//房间是否处于解散状态
	bool GetDismissState();
	//设置房间处于解散状态
	void SetDismissState(bool bState);

	//功能函数
public:
	//用户状况
	WORD GetTableUserInfo(tagTableUserInfo & TableUserInfo);
	//配置桌子
	bool InitializationFrame(WORD wTableID, tagTableFrameParameter & TableFrameParameter);

	//用户事件
public:
	//断线事件
	bool OnEventUserOffLine(IServerUserItem * pIServerUserItem);
	//积分事件
	bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);

	//通用事件
public:
	//申请解散房间
	bool OnEventApplyDismissRoom(WORD wChairID, bool bAgree);
	//表决是否解散房间
	bool OnEventVoteDismissRoom(WORD wChairID,bool bAgree);

	//系统事件
public:
	//时间事件
	bool OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//游戏事件
	bool OnEventSocketGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//框架事件
	bool OnEventSocketFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//辅助函数
public:
	//桌子状态
	bool SendTableStatus();
	//请求失败
	bool SendRequestFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszDescribe, LONG lErrorCode);

	//效验函数
public:
	//开始效验
	bool EfficacyStartGame(WORD wReadyChairID);
	//地址效验
	bool EfficacyIPAddress(IServerUserItem * pIServerUserItem);
	//积分效验
	bool EfficacyScoreRule(IServerUserItem * pIServerUserItem);
	//校验用户GPS距离
	bool CheckUserDistance();
	//检验用户的IP地址（暂时不用EfficacyIPAddress函数）
	bool CheckUserIpAddress();

public:
	//检查分配
	bool CheckDistribute();
	//游戏记录
	void RecordGameScore(bool bDrawStarted, DWORD dwStartGameTime=INVALID_DWORD);

public:

	//牌友圈群主解散房间, 通知子游戏处理
	void OnEventClubDismissRoom();

//虚拟用户，添加机器人
public:
	virtual bool AddVirtualUser(IServerUserItem* pServerUserItem);

	//辅助函数
protected:
	double GetDistance(double long1, double lat1, double long2, double lat2);

};

//////////////////////////////////////////////////////////////////////////////////

#endif