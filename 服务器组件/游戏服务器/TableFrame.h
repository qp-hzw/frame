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
//时间标识

//调度事件范围
#define IDI_MAIN_MODULE_START		1									//起始标识
#define IDI_MAIN_MODULE_FINISH		99									//终止标识

//桌子事件范围
#define IDI_TABLE_MODULE_START		10000								//起始标识
#define IDI_TABLE_MODULE_FINISH		50010000							//终止标识

//比赛场事件范围
#define IDI_MATCH_MODULE_START		150									//起始
#define IDI_MATCH_MODULE_FINISH		200									//终止

//回调事件范围
#define TIME_TABLE_SINK_RANGE		40									//标识范围
#define TIME_TABLE_MODULE_RANGE		50									//标识范围
//////////////////////////////////////////////////////////////////////////////////

//数组定义
typedef CWHArray<tagGameScoreRecord *> CGameScoreRecordArray;			//记录数组

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
	/****************************************** 静态数据 **************************************************/
	//组件接口
protected:
	ITableFrameSink	*				m_pITableFrameSink;					//桌子接口

private:
	DWORD							m_wTableID;							//桌子标志 6位
	WORD							m_wChairCount;						//椅子数目

	tagTableRule					m_tagTableRule;						//通用房间规则(client传给服务端大厅的)
	tagClubRoomRule                 m_tagClubRoomRule;					//工会房间规则

	DWORD							m_dwTableOwner;						//房主（第一个坐下的玩家）

	std::string						m_OnlyID;							//战绩记录的唯一标识

	CSubRecord						m_Record;							//录像回放

	/****************************************** 动态数据 **************************************************/
private:
	WORD                            m_wCurGameRound;                    //当前游戏局数
	BYTE							m_cbGameStatus;						//游戏状态

	std::list<CPlayer*>				m_user_list;				        //所有玩家(包含旁观)
	std::vector<CPlayer*>			m_player_list;				        //游戏用户(坐在椅子上的玩家)

	//比赛相关
	std::vector<SCORE>				m_total_score;						//总分

	//房间解散
public:
	bool							m_bAgree[MAX_CHAIR];				//同意解散
	bool							m_bResponseDismiss[MAX_CHAIR];		//响应解散
	bool							m_bUnderDissState;					//桌子是否处于解散状态
	WORD							m_dissmisserChaiID;					//解散发起者

	//游戏记录
protected:
	CGameScoreRecordArray			m_GameScoreRecordActive;			//游戏记录

#pragma endregion

#pragma region 初始化函数
	//函数定义
public:
	//构造函数
	CTableFrame();
	//析构函数
	virtual ~CTableFrame();
	//拷贝构造
	CTableFrame(const CTableFrame& table);

	//管理接口
public:
	//启动服务
	virtual bool StartService(){return true;}
	//停止服务
	virtual bool ConcludeService(){return true;}
#pragma endregion

#pragma region 桌子只读属性
public:
	//获取房间模式
	virtual BYTE GetTableMode(){return m_tagTableRule.GameMode;};
	//获取金币场场次
	BYTE GetGoldType() { return m_tagTableRule.GameRoomLevel; }

	//获取椅子数目
	WORD GetChairCount() { return m_wChairCount; }

#pragma endregion
#pragma region 桌子属性设置与获取
public:
	//桌子编号
	virtual DWORD GetTableID() { return m_wTableID; }
	//设置密码
	virtual void SetTableID(DWORD dwTableID){ m_wTableID=dwTableID; }

	//当前游戏局数
	virtual DWORD GetCurGameRound() { return m_wCurGameRound; }

	//获取状态
	virtual BYTE GetGameStatus() { return m_cbGameStatus; }
	//设置状态
	virtual VOID SetGameStatus(BYTE cbGameStatus) { m_cbGameStatus=cbGameStatus; }

	//设置桌主，第一个坐下的人
	virtual void SetTableOwner(DWORD dwUserID) {m_dwTableOwner = dwUserID;}
	//获得桌主，第一个坐下的人
	virtual DWORD GetTableOwner() {return m_dwTableOwner;}

	//读取房间通用规则
	virtual tagTableRule* GetCustomRule() { return &m_tagTableRule;};
	//设置房间通用规则
	virtual void SetCommonRule(tagTableRule* pRule)
	{
		//初始化大厅规则
		memcpy(&m_tagTableRule, pRule, sizeof(m_tagTableRule));
		m_wChairCount = pRule->PlayerCount;
		m_player_list.resize(m_wChairCount, NULL);
		m_total_score.resize(m_wChairCount, 0);

		//初始化子游戏规则
		if (m_pITableFrameSink != NULL)
			m_pITableFrameSink->Initialization(this, &m_tagTableRule);
	};

	//读取工会房间规则
	virtual tagClubRoomRule* GetClubRoomRule() { return &m_tagClubRoomRule;};
	//设置工会房间规则
	virtual void SetClubRoomRule(tagClubRoomRule* pRule)
	{
		memcpy(&m_tagClubRoomRule, pRule, sizeof(m_tagClubRoomRule));
	};

	//检测房间
	void CheckRoomTruePlayer();

	//比赛场相关
public:
	//房间是否满了
	bool IsRoomFull();

	//玩家自动准备定时器
	void SetPlayerAutoReady();
	//获取桌子里所有玩家
	std::vector<CPlayer *> GetPlayer_list() { return m_player_list; }

	//设置开始下一阶段定时器
	void SetStageTimer();

	//获取总分
	std::vector<SCORE> GetTotalScore() { return m_total_score; }
	//获取OnlyID
	std::string GetOnlyID()	{ return m_OnlyID; }

	//开始下一阶段比赛
	virtual void StartNextStage() { CLog::Log(log_debug, "no"); }

#pragma endregion

	//子游戏与框架接口函数
public:
	// 0房卡 1比赛; 2金币; 3房卡金币 4牌友圈
	virtual int GameType() { return m_tagTableRule.GameMode; }
	//is roboot
	virtual bool IsRobot(WORD wChairID);

	//发送托管
	virtual bool SendPlayerTuoGuan(WORD wChairID);

#pragma region 游戏流程函数
	//自身调用，与子游戏无关
protected:
	//开始游戏
	virtual bool StartGame();

	//与子游戏交互函数
public:
	//小局结束处理函数
	virtual bool HandleXJGameEnd(BYTE cbCurGameCount,WORD *wIdentity, SCORE *lGameScore);
	//大局结束处理函数  之前名字为: HandleDJGameEnd
	virtual bool HandleDJGameEnd(BYTE cbGameStatus);

	//小局大局结束辅助函数
protected:	
	//每局游戏结束后, 判断并扣除用户门票
	bool XJTickets();
	//每局游戏结束后，更新用户财富信息
	bool XJModifyUserTreasure(SCORE *lGameScore);
	//每局游戏结束后，更新游戏任务状态
	bool XJUpdateGameTaskStatus(const BYTE &cbTableMode, const BYTE &cbCurGameCount);

	//每局游戏结束后, 更新桌子战绩表
	bool XJUpdateTableRecord(BYTE round, string OnlyID);
	//每局游戏结束后, 更新桌子录像表
	bool XJUpdateTableVideo(BYTE round, string OnlyID, VOID* pData, DWORD dwDataSize);

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
public:
	//发送数据
	virtual bool SendTableData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize, WORD wMainCmdID = MDM_GF_GAME);
	//发送场景
	virtual bool SendGameScene(WORD wChairID, VOID * pData, WORD wDataSize);

	//系统消息
public:
	//发送消息
	virtual bool SendGameMessage(LPCTSTR lpszMessage, WORD wType);
	//游戏消息
	virtual bool SendGameMessage(CPlayer * pIServerUserItem, LPCTSTR lpszMessage, WORD wType);
	//房间消息
	virtual bool SendRoomMessage(CPlayer * pIServerUserItem, LPCTSTR lpszMessage, WORD wType);
#pragma endregion

#pragma region 玩家动作
	//用户接口
public:
	//游戏用户
	virtual CPlayer * GetTableUserItem(WORD wChairID);
	//旁观用户
	virtual CPlayer * EnumLookonUserItem(WORD wEnumIndex);

	//玩家动作
public:
	//玩家加入
	virtual int PlayerEnterTable(CPlayer* pPlayer);
	//玩家坐下		只有创建和加入房间时，用户坐下时需要校验GPS，故加此字段
	virtual int PlayerSitTable(CPlayer * pIServerUserItem, WORD wChairID = INVALID_CHAIR, bool bCheckUserGPS = false);
	//玩家起立
	virtual bool PlayerUpTable(CPlayer * pIServerUserItem);
	//玩家离开
	virtual int PlayerLeaveTable(CPlayer* pPlayer);
	//玩家准备
	virtual int PlayerReady(CPlayer* pPlayer);
	//玩家取消准备
	virtual int PlayerCancelReady(CPlayer* pPlayer);
	//玩家断线
	virtual int PlayerOffline(CPlayer* pPlayer);

	//玩家动作辅助函数
private:
	int CanPlayerEnterTable(CPlayer* pPlayer);
	int CanPlayerSitTable(CPlayer* pPlayer, WORD &wChairID);
	int CanPlayerUpTable(CPlayer* pPlayer);
	int CanPlayerLeaveTable(CPlayer* pPlayer);
	int CanPlayerReady(CPlayer* pPlayer);
	int CanPlayerCancelReady(CPlayer* pPlayer);

#pragma endregion

#pragma region 桌子动作
public:
	//获取空位
	WORD GetNullChairID();
	//用户数目
	WORD GetSitUserCount();

	//获取本桌玩家
	WORD GetPlayerChair(CPlayer* pPlayer);
	//桌子中是否有真实玩家
	bool IsTruePlayer();

#pragma endregion

#pragma region 解散房间
public:
	//申请解散房间
	bool OnEventApplyDismissRoom(WORD wChairID, bool bAgree);
	//表决是否解散房间
	bool OnEventVoteDismissRoom(WORD wChairID,bool bAgree);

	//房间是否处于解散状态
	bool GetDismissState();
	//设置房间处于解散状态
	void SetDismissState(bool bState);

	//设置表决解散房间定时器
	void SetVoteDismissRoom();
	//取消表决解散房间定时器
	void KillVoteDismissRoom();

	//解散状态断线重连
	bool OnEventDismissOffline(WORD wChairID);

	//设置房间自动解散时间 added by lizhihu
	virtual void SetTableAutoDismiss(DWORD dwMinutes = 1);
#pragma endregion

	//系统事件
public:
	//时间事件
	bool OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//游戏事件
	bool OnEventSocketGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, CPlayer * pIServerUserItem);
	//框架事件
	bool OnEventSocketFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, CPlayer * pIServerUserItem);

	//辅助函数
public:
	//请求失败
	bool SendRequestFailure(CPlayer * pIServerUserItem, LPCTSTR pszDescribe, LONG lErrorCode);

#pragma region 效验函数
public:
	//开始效验
	bool EfficacyStartGame(WORD wReadyChairID);
	//地址效验
	bool EfficacyIPAddress(CPlayer * pIServerUserItem);
	//积分效验
	bool EfficacyScoreRule(CPlayer * pIServerUserItem);
	//校验用户GPS距离
	bool CheckUserDistance();
	//检验用户的IP地址（暂时不用EfficacyIPAddress函数）
	bool CheckUserIpAddress();

		//辅助函数
protected:
	double GetDistance(double long1, double lat1, double long2, double lat2);

	//判断大赢家
	bool GetBigWinner(WORD wChairID);
	//金币场算金币
	bool SettleGold(WORD *wIdentity);

#pragma endregion

public:
	//牌友圈群主解散房间, 通知子游戏处理
	void OnEventClubDismissRoom();
};

#endif