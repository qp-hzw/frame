#ifndef GAME_SERVICE_HEAD_HEAD_FILE
#define GAME_SERVICE_HEAD_HEAD_FILE

class CPlayer;
class CPlayerManager;
//////////////////////////////////////////////////////////////////////////////////
//导出定义

//导出定义
#ifndef GAME_SERVICE_CLASS
#ifdef  GAME_SERVICE_DLL
#define GAME_SERVICE_CLASS _declspec(dllexport)
#else
#define GAME_SERVICE_CLASS _declspec(dllimport)
#endif
#endif

//模块定义
#ifndef _DEBUG
#define GAME_SERVICE_DLL_NAME	TEXT("GameService.dll")				//组件 DLL 名字
#else
#define GAME_SERVICE_DLL_NAME	TEXT("GameServiceD.dll")			//组件 DLL 名字
#endif

//////////////////////////////////////////////////////////////////////////////////
//接口说明


//桌子接口
interface ITableFrame;
interface ITableFrameSink;
interface ITableUserAction;

//服务接口
interface IMainServiceFrame;
interface IGameServiceManager;
interface IGameServiceCustomRule;

//////////////////////////////////////////////////////////////////////////////////
//常量定义

//群发掩码
#define BG_COMPUTER					(BYTE)(0x02)						//电脑群发

//索引定义
#define INDEX_SOCKET				(WORD)(0x0000)						//网络索引
#define INDEX_ANDROID				(WORD)(0x2000)						//机器索引

//创建函数
#define GAME_SERVICE_CREATE_NAME	"CreateGameServiceManager"			//创建函数
//读取配置文件
#define GAME_SERVICE_LOAD_CONFIG	"LoadConfigServiceManager"			//创建函数

//////////////////////////////////////////////////////////////////////////////////
//常量定义

//机器类型
#define ANDROID_SIMULATE			0x01								//相互模拟
#define ANDROID_PASSIVITY			0x02								//被动陪打
#define ANDROID_INITIATIVE			0x04								//主动陪打

//结束原因
#define GER_NORMAL					0x00								//常规结束
#define GER_DISMISS					0x01								//游戏解散
#define GER_USER_LEAVE				0x02								//用户离开
#define GER_NETWORK_ERROR			0x03								//网络错误

//离开原因
#define LER_NORMAL					0x00								//常规离开
#define LER_SYSTEM					0x01								//系统原因
#define LER_NETWORK					0x02								//网络原因
#define LER_USER_IMPACT				0x03								//用户冲突
#define LER_SERVER_FULL				0x04								//人满为患
#define LER_SERVER_CONDITIONS		0x05								//条件限制
#define LER_EXCEPTION				0x06								//异常

//积分类型
#define SCORE_TYPE_NULL				0x00								//无效积分
#define SCORE_TYPE_WIN				0x01								//胜局积分
#define SCORE_TYPE_LOSE				0x02								//输局积分
#define SCORE_TYPE_DRAW				0x03								//和局积分
#define SCORE_TYPE_FLEE				0x04								//逃局积分
#define SCORE_TYPE_PRESENT			0x10								//赠送积分
#define SCORE_TYPE_SERVICE			0x11								//服务积分


//分组选项
#define DISTRIBUTE_ALLOW			0x01								//允许分组
#define DISTRIBUTE_IMMEDIATE		0x02								//入座选项
#define DISTRIBUTE_LAST_TABLE		0x04								//同桌选项
#define DISTRIBUTE_SAME_ADDRESS		0x08								//地址选项

//////////////////////////////////////////////////////////////////////////////////
//时间标识

//调度范围
#define IDI_MAIN_MODULE_START		1									//起始标识
#define IDI_MAIN_MODULE_FINISH		99									//终止标识

//机器范围
#define IDI_REBOT_MODULE_START		100									//起始标识
#define IDI_REBOT_MODULE_FINISH		199									//终止标识

//比赛范围
#define IDI_MATCH_MODULE_START		200									//起始标识
#define IDI_MATCH_MODULE_FINISH		299									//结束标识
//桌子范围
#define IDI_TABLE_MODULE_START		10000								//起始标识
#define IDI_TABLE_MODULE_FINISH		50000								//终止标识

//////////////////////////////////////////////////////////////////////////////////
//时间范围

//游戏时间
#define TIME_TABLE_SINK_RANGE		40									//标识范围
#define TIME_TABLE_MODULE_RANGE		50									//标识范围

//////////////////////////////////////////////////////////////////////////////////
//常量定义

//常量定义
#define RESERVE_USER_COUNT			40L									//保留数目

//////////////////////////////////////////////////////////////////////////////////
//结构定义

//配置参数
struct tagGameParameter
{
	//汇率信息
	WORD							wMedalRate;							//奖牌汇率
	WORD							wRevenueRate;						//银行税收

	//版本信息
	DWORD							dwClientVersion;					//客户版本
	DWORD							dwServerVersion;					//服务版本
};

//服务属性
struct tagGameServiceAttrib
{
	DWORD							dwGameID;							//游戏索引

	WORD							wChairCount;						//椅子数目 

	DWORD							dwSubGameVersion;					//子游戏版本
	TCHAR							szDllName[LEN_DLL];					//Dll名称
};

//服务配置
struct tagGameServiceOption
{
	DWORD                           dwGameID;                           //游戏标志		TODO 这个从子游戏获得tagGameServiceAttrib
	DWORD							dwServerID;							//房间标识

	//常规配置
	TCHAR							szServerName[LEN_SERVER];			//房间名称
	WORD							wGameServerPort;					//游戏服端口
	TCHAR							szGameServerAddr[LEN_DB_ADDR];		//游戏服地址    数据库中获取
};

//游戏配置
struct tagRoomRuleOption
{
	BYTE							cbGameOptionType[1024];				//配置属性
};

/*自建房规则字节对应属性值*/
enum enOption
{
	//modified by lizhihu 2017.10.9
	enOption_GameId = 0,			//游戏ID,阿拉斗为4
	enOption_GameScoreTimes,		//设置游戏加倍模式:经典或疯狂加倍模式		0-经典模式	1-疯狂模式
	enOption_RobBankType,			//增加框架传来的游戏抢庄模式				0-抢庄		1-轮庄		2-固定
	enOption_RoomCell,				//底分,客户端不用处理
	enOption_FangZhu,				//房主,客户端不用处理			
	enOption_GameCount,				//游戏局数,客户端不用处理
	enOption_PlayerCount,			//玩家数,客户端不用处理
};




//桌子参数
struct tagTableFrameParameter
{
	//配置变量
	tagGameParameter *				pGameParameter;						//配置参数
	tagGameServiceAttrib *			pGameServiceAttrib;					//桌子属性
	tagGameServiceOption *			pGameServiceOption;					//配置参数
};

//机器参数
struct tagAndroidUserParameter
{
	//配置变量
	tagGameParameter *				pGameParameter;						//配置参数
	tagGameServiceAttrib *			pGameServiceAttrib;					//桌子属性
	tagGameServiceOption *			pGameServiceOption;					//配置参数

	//内核组件
//	ITimerEngine *					pITimerEngine;						//时间引擎
	CPlayerManager *			pIServerUserManager;				//用户管理
	IGameServiceManager *			pIGameServiceManager;				//服务管理
	//ITCPNetworkEngineEvent *		pITCPNetworkEngineEvent;			//事件接口
};

//////////////////////////////////////////////////////////////////////////////////
//辅助结构

//积分信息
struct tagScoreInfo
{
	BYTE							cbType;								//积分类型
	SCORE							lScore;								//用户分数
	SCORE							lGrade;								//用户成绩
	SCORE							lRevenue;							//游戏税收
	SCORE							lChoushui;							//抽水值
	DWORD							dwChoushuiType;						//抽水类型		(默认)(台费/税收)抽水=0		|	(洗码)抽水=1
};

//变更信息
struct tagVariationInfo
{
	//积分信息
	SCORE							lScore;								//用户分数
	SCORE							lGrade;								//用户成绩
	SCORE							lInsure;							//用户银行
	SCORE							lRevenue;							//游戏税收	
	SCORE							lChoushui;							//抽水值
	DWORD							dwChoushuiType;						//抽水类型

	//游戏信息
	DWORD							dwWinCount;							//胜利盘数
	DWORD							dwLostCount;						//失败盘数
	DWORD							dwDrawCount;						//和局盘数
	DWORD							dwFleeCount;						//逃跑盘数
	DWORD							dwPlayTimeCount;					//游戏时长

	//全局信息
	DWORD							dwUserMedal;						//用户奖牌
	DWORD							dwExperience;						//用户经验
	LONG							lLoveLiness;						//用户魅力
};

//游戏记录
struct tagGameScoreRecord
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	WORD							wChairID;							//椅子号码
	BYTE							cbAndroid;							//机器标志

	//成绩信息
	SCORE							lScore;								//用户分数
	SCORE							lGrade;								//用户成绩
	SCORE							lRevenue;							//游戏税收
	SCORE							lControlScore;						//用户控制值
	SCORE							lChoushui;							//抽水
	DWORD							dwChoushuiType;						//抽水类型

	//用户信息
	DWORD							dwDBQuestID;						//请求标识
	DWORD							dwInoutIndex;						//记录索引

	//附加信息
	DWORD							dwUserMemal;						//奖牌数目
	DWORD							dwPlayTimeCount;					//游戏时长
};

//用户规则
struct tagUserRule
{
	//规则标志
	bool							bLimitSameIP;						//效验地址
	bool							bLimitWinRate;						//限制胜率
	bool							bLimitFleeRate;						//限制逃率
	bool							bLimitGameScore;					//限制分数

	//规则属性
	WORD							wMinWinRate;						//最低胜率
	WORD							wMaxFleeRate;						//最高逃率
	LONG							lMaxGameScore;						//最高分数 
	LONG							lMinGameScore;						//最低分数
	TCHAR							szPassword[LEN_PASSWORD];			//桌子密码
};

//用户信息 TODONOW added by WangChengQing 这个字段可以删除 2018/5/12
//在下次 迭代的时候删除, 现在不删除 是因为会对子游戏有影响(子依赖项)
struct tagUserInfoPlus
{
	//登录信息
	DWORD							dwLogonTime;						//登录时间
	DWORD							dwInoutIndex;						//进出标识

	//连接信息
	WORD							wBindIndex;							//绑定索引
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识

	//附加变量
	DWORD							dwUserRight;						//用户权限
	DWORD							dwMasterRight;						//管理权限
	SCORE							lRestrictScore;						//限制积分

	//辅助变量
	bool							bAndroidUser;						//机器用户
	TCHAR							szPassword[LEN_MD5];				//桌子密码
};

//请求结果
struct tagRequestResult
{
	BYTE							cbFailureCode;						//失败原因
	TCHAR							szFailureReason[128];				//失败原因
};

//桌子状况
struct tagTableUserInfo
{
	//WORD							wMinUserCount;						//最少人数
	WORD							wTableUserCount;					//用户数目
	WORD							wTableReadyCount;					//准备数目
	WORD							wTableAndroidCount;					//机器数目
};

//机器状况
struct tagAndroidUserInfo
{
	WORD							wFreeUserCount;						//用户数目
	WORD							wPlayUserCount;						//用户数目
	WORD							wSitdownUserCount;					//用户数目
};

//服务信息
struct tagAndroidService
{
	DWORD							dwResidualPlayDraw;					//剩余局数
	DWORD                           dwReposeTime;                       //服务时间 
};

//机器参数
struct tagAndroidParameter
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwServiceTime;						//服务时间
	SCORE							lMinTakeScore;						//携带积分
	SCORE							lMaxTakeScore;						//携带积分
	DWORD							dwMinPlayDraw;						//游戏局数
	DWORD							dwMaxPlayDraw;						//游戏局数
	DWORD							dwMinReposeTime;					//休息时间
	DWORD							dwMaxReposeTime;					//休息时间
	DWORD							dwServiceGender;					//服务类型
};

//比赛状态
enum EN_MatchStatus
{
	EN_NULL=0,
	EN_OUT,
};
//////////////////////////////////////////////////////////////////////////////////
//游戏接口
interface IGameServiceManager : public IUnknownEx
{
	//创建接口
public:
	//创建桌子
	virtual VOID * CreateTableFrameSink(REFGUID Guid, DWORD dwQueryVer)=NULL;
	//创建机器
	virtual VOID * CreateAndroidUserItemSink(REFGUID Guid, DWORD dwQueryVer)=NULL;
	//创建数据
	virtual VOID * CreateGameDataBaseEngineSink(REFGUID Guid, DWORD dwQueryVer)=NULL;

	//参数接口
public:
	//组件属性
	virtual bool GetServiceAttrib(tagGameServiceAttrib & GameServiceAttrib)=NULL;
	//调整参数
	virtual bool RectifyParameter(tagGameServiceOption & GameServiceOption)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////
//配置接口
interface IGameServiceCustomRule : public IUnknownEx
{
	//获取配置
	virtual bool SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)=NULL;
	//默认配置
	virtual bool DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)=NULL;
	//创建窗口
	virtual HWND CreateCustomRule(CWnd * pParentWnd, CRect rcCreate, LPBYTE pcbCustomRule, WORD wCustonSize)=NULL;
};


//////////////////////////////////////////////////////////////////////////////////
//数据接口
interface IGameDataBaseEngineSink : public IUnknownEx
{
	//配置接口
	virtual bool InitializeSink(IUnknownEx * pIUnknownEx)=NULL;
	//数据处理
	virtual bool OnGameDataBaseRequest(DWORD dwUserID, WORD wTableID, WORD wChairID);
};

//////////////////////////////////////////////////////////////////////////////////

interface IGameMatchSink;

//桌子接口
interface ITableFrame : public IUnknownEx
{
	//配置参数
public:
	//读取通用房间规则
	virtual VOID* GetCustomRule() =NULL;
	//读取子游戏特有房间规则
	virtual VOID * GetSubGameRule()=NULL;

	//写分接口
public:
	//写入录像记录 参数 小局数,数据和长度
	virtual bool WriteRecordInfo(WORD wXJCount,TCHAR strScore[],  VOID* pData, DWORD dwDataSize) = NULL;
	//处理小局结束
	virtual bool HandleXJGameEnd(BYTE cbCurGameCount, BYTE cbGameMode, SCORE *lGameScore) = NULL;

	//时间接口
public:
	//设置时间
	virtual bool SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter)=NULL;
	//删除时间
	virtual bool KillGameTimer(DWORD dwTimerID)=NULL;
	//获取定时器剩余毫秒数
	virtual DWORD GetTimerLeftTickCount(DWORD dwTimerID)=NULL;

	//功能接口
public:
	//发送场景
	virtual bool SendGameScene(CPlayer * pIServerUserItem, VOID * pData, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////
//回调接口
interface ITableFrameSink : public IUnknownEx
{
	//管理接口
public:
	//复位接口
	virtual VOID RepositionSink()=NULL;
	//配置接口
	virtual bool Initialization(IUnknownEx * pIUnknownEx)=NULL;

	//查询接口
public:
	//最少积分
	virtual SCORE QueryLessEnterScore(WORD wChairID, CPlayer * pIServerUserItem)=NULL;
	//查询是否扣服务费
	virtual bool QueryBuckleServiceCharge(WORD wChairID)=NULL;

	//游戏事件
public:
	//游戏开始
	virtual bool OnEventGameStart()=NULL;
	//游戏结束
	virtual bool OnEventGameConclude(WORD wChairID, CPlayer * pIServerUserItem, BYTE cbReason)=NULL;
	//发送场景
	virtual bool OnEventSendGameScene(WORD wChairID, CPlayer * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)=NULL;

	//事件接口
public:
	//时间事件
	virtual bool OnTimerMessage(DWORD dwTimerID, WPARAM dwBindParameter)=NULL;
	//数据事件
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize)=NULL;
	//控制值改变
	virtual void OnUserControlScoreChange( WORD chair_id, CPlayer * server_user_item, SCORE val ){};
	//游戏程度改变yang
	virtual void SetGameLevel(DWORD RoomID, DWORD val){};

	//彩金
public: 
	//彩金结果返回
	virtual bool OnRoomPrizePoolResualt(VOID * pData, WORD wDataSize){return true;};
	//总彩金结果返回
	virtual bool OnGamePrizePoolResualt(VOID * pData, WORD wDataSize){return true;};
	//彩金中奖返回
	virtual bool OnPrizePoolRewardResualt(VOID * pData, WORD wDataSize){return true;};
	//网络接口
public:
	//游戏消息
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, CPlayer * pIServerUserItem)=NULL;
	//框架消息
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, CPlayer * pIServerUserItem)=NULL;

	//比赛接口
public:
	//设置基数
	virtual void SetGameBaseScore(LONG lBaseScore)=NULL;

public:
	virtual bool OnActionUserSitDownT(WORD chair_id, CPlayer* server_user_item, bool lookon_user){ return true; }
	virtual bool OnActionUserStandUpT(WORD chair_id, CPlayer* server_user_item, bool lookon_user){ return true; }
	virtual bool OnActionUserOnReadyT(WORD chair_id, CPlayer * server_user_item, void* data, WORD data_size){ return true; }
	virtual bool OnActionUserNetCutT(WORD wChairID, CPlayer * pIServerUserItem, bool bLookonUser){ return true; }

	//用户动作
public:
	//用户坐下
	virtual bool OnActionUserSitDown(WORD wChairID, CPlayer * pIServerUserItem, bool bLookonUser)=NULL;
	//用户起来
	virtual bool OnActionUserStandUp(WORD wChairID, CPlayer * pIServerUserItem, bool bLookonUser)=NULL;
	//用户同意
	virtual bool OnActionUserOnReady(WORD wChairID, CPlayer * pIServerUserItem, VOID * pData, WORD wDataSize)=NULL;
	//用户断线
	virtual bool OnActionUserNetCut(WORD wChairID, CPlayer * pIServerUserItem, bool bLookonUser){ return true; }

	//用户请求
public:
	//旁观请求
	virtual bool OnUserRequestLookon(WORD wChairID, CPlayer * pIServerUserItem, tagRequestResult & RequestResult)=NULL;
	//坐下请求
	virtual bool OnUserRequestSitDown(WORD wChairID, CPlayer * pIServerUserItem, tagRequestResult & RequestResult)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////
//服务框架
interface IMainServiceFrame : public IUnknownEx
{
#pragma region DB事件通知
	//替他人开房
public:
	//替他人开房开始
	virtual void StartTable(DWORD dwTableID) = NULL;
	//用户加入替他人开房
	virtual void JoinTable(DWORD dwTableID, DWORD dwUserID) = NULL;
	//用户离开替他人开房
	virtual void LeaveTable(DWORD dwTableID, DWORD dwUserID) = NULL;

	//CLUB牌友圈2  && 金币大厅
public:
	//用户坐下
	virtual void ClubPlayerSitDown(DWORD dwTableID, DWORD dwUserID, BYTE byChairID, BYTE byClubOrHallGold) = NULL;
	//用户起立
	virtual void ClubPlayerSitUp(DWORD dwTableID, DWORD dwUserID, BYTE byChairID, BYTE byClubOrHallGold) = NULL;
	//最后一个用户起立
	virtual void ClubLastPlayerSitUp(DWORD dwTableID, DWORD dwUserID, BYTE byChairID, BYTE byClubOrHallGold)= NULL;
	//用户掉线
	virtual void ClubPlayerOffline(DWORD dwTableID, DWORD dwUserID, BYTE byChairID, BYTE byClubOrHallGold) = NULL;

	//创建桌子 -- 俱乐部桌子
	virtual void ClubTableCreate(DWORD dwClubRoomID, DWORD dwUserID, DWORD dwTableID, DWORD dwLockState) = NULL;
	//桌子开始游戏
	virtual void ClubTableStart(DWORD dwTableID, BYTE byClubOrHallGold) = NULL;
	//桌子人数已满
	virtual void ClubTableMax(DWORD dwTableID, BYTE byClubOrHallGold) = NULL;
	//桌子人数没有满
	virtual void ClubTableNotMax(DWORD dwTableID, BYTE byClubOrHallGold) = NULL;
	//桌子小局结束游戏
	virtual void ClubTableXJ(DWORD dwTableID) = NULL;
	//桌子大局结束游戏
	virtual void ClubTableDJ(DWORD dwTableID) = NULL;
#pragma endregion
};

#endif