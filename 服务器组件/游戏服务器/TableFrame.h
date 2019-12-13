#ifndef TABLE_FRAME_HEAD_FILE
#define TABLE_FRAME_HEAD_FILE

#include <vector>
#include "Stdafx.h"
#include "Player.h"
#include "TableFrameBase.h"

//////////////////////////////////////////////////////////////////////////////////
#define GAME_CONCLUDE_CONTINUE  0xFF //大局结束并续费
#define GAME_CONCLUDE_NORMAL    0xFE //正常结束

//游戏状态
#define GAME_STATUS_FREE			0									//空闲状态
#define GAME_STATUS_PLAY			100									//游戏状态	TODO 细看，改成桌子状态

//////////////////////////////////////////////////////////////////////////////////
//时间标识

//调度事件范围
#define IDI_MAIN_MODULE_START		1									//起始标识
#define IDI_MAIN_MODULE_FINISH		99									//终止标识

//桌子事件范围
#define IDI_TABLE_MODULE_START		10000								//起始标识
#define IDI_TABLE_MODULE_FINISH		50000								//终止标识

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
private:
	WORD							m_wTableID;							//桌子号码		从0开始，在桌子初始化的时候已经赋值
	WORD							m_wChairCount;						//椅子数目

	BYTE							m_cbTableMode;						//桌子游戏模式  房卡场, 金币场, 房卡金币场, 比赛场

	tagTableRule					m_tagTableRule;						//通用房间规则(client传给服务端大厅的)
	tagTableSubGameRule				m_tagTableSubGameRule;				//子游戏特有房间规则(client传给子游戏的)

	//TODOLATER 只保留一个字段
	DWORD							m_dwTableOwner;						//房主（第一个坐下的玩家）
	DWORD							m_dwCreateTableUser;				//创建桌子用户

	//TODOLATER 待整理
	DWORD							m_dwGroupID;						//所在牌友圈ID


	/****************************************** 动态数据 **************************************************/
private:
	WORD                            m_wCurGameRound;                    //当前游戏局数
	BYTE							m_cbGameStatus;						//游戏状态

	std::vector<CPlayer*>			m_user_list;				        //所有玩家(包含旁观)
	std::vector<CPlayer*>			m_player_list;				        //游戏用户(坐在椅子上的玩家)
	
	//房间解散
private:
	bool							m_bAgree[MAX_CHAIR];					//同意解散
	bool							m_bResponseDismiss[MAX_CHAIR];			//响应解散
	bool							m_bUnderDissState;						//桌子是否处于解散状态
	WORD							m_dissmisserChaiID;						//解散发起者

	//时间变量
protected:
	SYSTEMTIME						m_SystemTimeStart;					//开始时间

	//组件接口
protected:
	ITableFrameSink	*				m_pITableFrameSink;					//桌子接口

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

	//管理接口
public:
	//启动服务
	virtual bool StartService(){return true;}
	//停止服务
	virtual bool ConcludeService(){return true;}
#pragma endregion

#pragma region 桌子属性设置与获取
public:
	//桌子编号
	virtual WORD GetTableID() { return m_wTableID; }
	//设置密码
	virtual void SetTableID(DWORD dwTableID){ m_wTableID=dwTableID; }

	//获取状态
	virtual BYTE GetGameStatus() { return m_cbGameStatus; }
	//设置状态
	virtual VOID SetGameStatus(BYTE cbGameStatus) { m_cbGameStatus=cbGameStatus; }

	//设置桌主，第一个坐下的人
	virtual void SetTableOwner(DWORD dwUserID) {m_dwTableOwner = dwUserID;}
	//获得桌主，第一个坐下的人
	virtual DWORD GetTableOwner() {return m_dwTableOwner;}

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

	//房间是否处于解散状态
	bool GetDismissState();
	//设置房间处于解散状态
	void SetDismissState(bool bState);

	//设置房间自动解散时间 added by lizhihu
	virtual void SetTableAutoDismiss(DWORD dwMinutes = 1);

#pragma endregion

	//子游戏与框架接口函数
public:
	// 1金币; 2私有; 3比赛
	virtual int GameType() { return 2; }
	//is roboot
	virtual bool IsRobot(WORD wChairID) { return true; }
	//get player base info
	virtual BASE_PLAYERINFO GetPlayerBaseInfo(WORD wChairID) { BASE_PLAYERINFO i; return i; }

#pragma region 游戏流程函数
	//自身调用，与子游戏无关
public:
	//开始游戏
	virtual bool StartGame();

	//与子游戏交互函数
public:
	//小局结束处理函数
	virtual bool HandleXJGameEnd(BYTE cbCurGameCount, BYTE cbGameMode, SCORE *lGameScore);
	//大局结束处理函数  之前名字为: HandleDJGameEnd
	virtual bool HandleDJGameEnd(BYTE cbGameStatus);

	//写入录像记录(战绩回放) 参数 小局数,数据和长度
	bool WriteRecordInfo(WORD wXJCount,TCHAR strScore[], VOID* pData, DWORD dwDataSize);

	//小局大局结束辅助函数
protected:	
	//每局游戏结束后, 判断并扣除用户门票
	bool XJGameTickets(BYTE byTableMode, BYTE byRound);
	//每局游戏结束后，更新用户财富信息
	bool XJModifyUserTreasure(BYTE byTableMode, BYTE byRound, SCORE *lGameScore, tagTableRule *pCfg);	
	//每局游戏结束后, 检测用户财富是否可以继续游戏
	bool CheckUserLeave(BYTE byTableMode, CPlayer* pIServerUserItem, tagTableRule* pCfg);
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
	virtual bool SendUserItemData(CPlayer * pIServerUserItem, WORD wSubCmdID);
	//发送数据
	virtual bool SendUserItemData(CPlayer * pIServerUserItem, WORD wSubCmdID, VOID * pData, WORD wDataSize);

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
	int PlayerEnterTable(CPlayer* pPlayer);
	//玩家坐下		只有创建和加入房间时，用户坐下时需要校验GPS，故加此字段
	int PlayerSitTable(WORD wChairID, CPlayer * pIServerUserItem, LPCTSTR lpszPassword=NULL, bool bCheckUserGPS = false);
	//玩家起立
	bool PlayerUpTable(CPlayer * pIServerUserItem);
	//玩家离开
	int PlayerLeaveTable(CPlayer* pPlayer);
	//玩家准备
	int PlayerReady(WORD wChairID, CPlayer* pPlayer);

	//玩家动作辅助函数
private:
	int CanPlayerEnterTable(CPlayer* pPlayer);
	int CanPlayerSitTable(CPlayer* pPlayer, WORD &wChairID);
	int CanPlayerUpTable(CPlayer* pPlayer);
	int CanPlayerLeaveTable(CPlayer* pPlayer);
	int CanPlayerReady(CPlayer* pPlayer);

#pragma endregion

	//功能接口
public:
	//发送场景
	virtual bool SendGameScene(IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);

#pragma region 桌子动作
public:
	//获取空位
	WORD GetNullChairID();
	//用户数目
	WORD GetSitUserCount();

	//获取本桌玩家
	WORD GetPlayerChair(CPlayer* pPlayer);

#pragma endregion

	//用户事件
public:
	//断线事件
	bool OnEventUserOffLine(CPlayer * pIServerUserItem);

#pragma region 解散房间
public:
	//申请解散房间
	bool OnEventApplyDismissRoom(WORD wChairID, bool bAgree);
	//表决是否解散房间
	bool OnEventVoteDismissRoom(WORD wChairID,bool bAgree);
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
	//桌子状态 -- ?? 通知center??
	bool SendTableStatus();
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

#pragma endregion

public:
	//游戏记录
	void RecordGameScore( DWORD dwStartGameTime=INVALID_DWORD);

public:
	//牌友圈群主解散房间, 通知子游戏处理
	void OnEventClubDismissRoom();
};

#endif