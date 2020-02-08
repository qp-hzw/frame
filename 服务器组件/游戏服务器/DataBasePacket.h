#ifndef DATA_BASE_PACKET_HEAD_FILE
#define DATA_BASE_PACKET_HEAD_FILE

#include "../../全局定义/Define.h"
#pragma pack(1)

//用户Socket关闭事件
#define DBR_GP_GAME_USER_STATE					1									//玩家退出
//玩家退出
struct DBR_GP_UserQuitInfo
{
	DWORD							dwUserID;							//用户ID
	BYTE							byOnlineMask;						//在线标志 1大厅在线 ，2正在游戏，3游戏断线，4离线
};
//////////////////////////////////////////////////////////////////////////////////
//请求数据包
//USERID登录
#define	DBR_CG_LOGON_USERID			100									//ID登录
#define DBO_CG_LOGON_USERID			100									//ID登录返回

//创建牌友圈
#define DBR_CG_USER_CREATE_GROUP_ROOM	104								//创建牌友圈房间
#define DBO_CG_USER_CREATE_GROUP_ROOM	104								//创建牌友圈房间返回

//加入牌友圈
#define DBR_CG_USER_JOIN_GROUP_ROOM		106								//加入牌友圈房间
#define DBO_CG_USER_JOIN_GROUP_ROOM		106								//加入牌友圈房间返回

#define DBR_GR_MODIFY_USER_TREASURE		405								//更新用户财富信息
#define DBO_SG_MODIFY_USER_TREASURE		406                            //修改用户财富信息 返回


#define DBR_GR_QUERY_LOTTERY		103									//抽奖请求

//系统事件
#define DBR_GR_WRITE_GAME_SCORE		200									//游戏积分
#define DBR_GR_LEAVE_GAME_SERVER	201									//离开游戏服
#define DBR_GR_GAME_SCORE_RECORD	202									//积分记录
#define DBR_GR_MANAGE_USER_RIGHT	203									//权限管理

//配置事件
#define DBR_GR_LOAD_ANDROID_USER	302									//加载机器


#define DBR_GR_ADD_TABLE_INFO		413									//更新桌子信息
#define DBR_GR_START_TABLE			414									//开始桌子
#define DBR_GR_END_TABLE			415									//结束桌子
#define DBR_GR_ADD_TABLEUSER		416									//添加房间用户
#define DBR_GR_DELETE_TABLEUSER		417									//删除房间用户

//Club牌友圈桌子信息
#define DBR_CLUB_ROOM_INFO			420									//Club牌友圈房间信息
#define DBR_CLUB_TABLE_INFO			421									//Club牌友圈桌子信息
#define DBR_CLUB_PLAYER_INFO		422									//Club牌友圈玩家信息

//金币大厅 桌子信息
#define DBR_HALL_GOLD_TABLE_INFO	423									//金币大厅 桌子信息
#define DBR_HALL_GOLD_PLAYER_INFO	424									//金币大厅 玩家信息

//游戏事件
#define DBR_GR_GAME_FRAME_REQUEST	502									//游戏请求

//比赛配置
#define DBR_GR_MATCH_CONFIG			605									//配置比赛场

//游戏控制
#define DBR_GR_WRITE_CURRENT_STOCK		1006							//写当前库存值
//////////////////////////////////////////////////////////////////////////////////
//输出信息

//逻辑事件
#define DBO_GR_LOTTERY_RESULT		105


//游戏事件
#define DBO_GR_PROPERTY_FAILURE		401									//道具失败
#define DBO_GR_GAME_FRAME_RESULT	402									//游戏结果

#define DBO_GR_UPDATE_TABLE_INFO	413									//更新开房信息
#define DBO_GR_DELETE_TABLE_INFO	414									//删除开房信息


//比赛事件
#define DBO_GR_MATCH_EVENT_START	500									//事件范围
#define DBO_GR_MATCH_EVENT_END		599									//事件范围
#define DBO_GR_MATCH_FEE_RESULT		500									//比赛费用
#define DBO_GR_MATCH_RANK			501									//比赛名次
#define DBO_GR_MATCH_QUIT_RESULT	502									//退赛结果

#define DBO_GR_USERCONTROLVALMODIFY		1002							//玩家杀放控制
#
//桌子记录信息 added by lizhihu  700-800
#define DBR_SC_TABLE_UPDATE_TASK_STATUS			700						//更新游戏任务状态

//
#define	DBR_CG_CLUB_CREATE_TABLE				1100					//club牌友圈创建房间
#define	DBO_GC_CLUB_CREATE_TABLE				2100					//club牌友圈创建房间

#define	DBR_CG_USER_JOIN_TABLE_NO_PASS			1101					//club牌友圈加入桌子 -- 不需要密码
#define	DBO_GC_USER_JOIN_TABLE_NO_PASS			2101					//club牌友圈加入桌子 -- 不需要密码

#define	DBO_GC_CLUB_UPDATE_TABLE_INFO			1102					//club牌友圈更新桌子信息
#define	DBO_GC_CLUB_UPDATE_PLAYER_INFO			2102					//club牌友圈更新玩家信息

//////////////////////////////////////////////////////////////////////////////////

//ID 登录
struct STR_DBR_CG_LOGON_USERID
{		
	//登录信息
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_MD5];				//登录密码

	//附加信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识
};
//ID 登录返回
struct STR_DBO_CG_LOGON_USERID
{		
	LONG							lResultCode;								//错误代码
	TCHAR							szDescribeString[LEN_MESSAGE_DESCRIBE];		//消息描述

	tagUserInfo                     useInfo;                                    //玩家信息
};

//创建牌友圈房间
struct STR_DBR_CG_USER_CREATE_GROUP_ROOM
{		
	DWORD		dwUserID;						//创建房间用户ID
	DWORD		dwGroupID;						//牌友圈ID
	BYTE		rule[LEN_PRIVATE_TABLE_RULE];	//创建房间规则
};
//创建牌友圈房间返回
struct STR_DBO_CG_USER_CREATE_GROUP_ROOM
{		
	DWORD       dwResultCode;							 //结果标识
	TCHAR		szDescribeString[LEN_MESSAGE_DESCRIBE];	 //描述消息
	DWORD		dwUserID;								 //创建房间用户ID
	DWORD		dwGroupID;								 //牌友圈ID
	BYTE		rule[LEN_PRIVATE_TABLE_RULE];			 //创建房间规则
};

//加入牌友圈房间
struct STR_DBR_CG_USER_JOIN_GROUP_ROOM
{		
	DWORD		dwGroupID;			//牌友圈ID
	DWORD		dwUserID;			//加入用户ID
};
//加入牌友圈房间返回
struct STR_DBO_CG_USER_JOIN_GROUP_ROOM
{		
	DWORD       dwResultCode;							 //结果标识
	TCHAR		szDescribeString[LEN_MESSAGE_DESCRIBE];	 //描述消息
	DWORD		dwGroupID;								//牌友圈ID
};

//更新游戏任务状态
struct STR_DBR_CG_TABLE_UPDATE_TASK_STATUS
{		
	DWORD		dwUserID;			//玩家ID
	BYTE		cbTableMode;		//桌子模式（房卡、金币、钻石）
	BYTE		cbCurGameCount;		//当前游戏局数
};

// 更新用户财富信息
struct STR_DBR_GR_MODIFY_USER_TREASURE
{
	DWORD		dwUserId;			//玩家ID
	DWORD		dwKind;				//游戏索引 数据库统计需要
	DWORD		dwTableID;			//桌子ID
	BYTE		byTableMode;		//桌子模式
	BYTE		byRound;			//当前游戏局数  0表示创建桌子  FF(255)表示桌子结束(大局结束)
	SCORE		lUserTreasure;		//玩家财富变更  负数表示扣除;  正数表示增加
	BYTE		byWin;				//小局意义: 0平 1赢 2负    大局意义: 1表示大赢家

};

// 增加用户房卡、金币、钻石返回
struct STR_DBO_GR_MODIFY_USER_TREASURE
{
	DWORD			dwResultCode;							 //结果标识
	TCHAR			szDescribeString[LEN_MESSAGE_DESCRIBE];	 //描述消息
	DWORD			dwUserID;								 //用户ID
	SCORE           lUserDiamond;							 //用户钻石
	SCORE			lUserGold;								 //用户金币
	SCORE			lOpenRoomCard;							 //用户房卡
};

//游戏积分
struct DBR_GR_WriteGameScore
{
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

	//用户信息
	DWORD							dwUserID;							//用户标识
	DWORD							dwClientAddr;						//连接地址

	//用户信息
	DWORD							dwDBQuestID;						//请求标识
	DWORD							dwInoutIndex;						//进出索引

	//成绩变量
	tagVariationInfo				VariationInfo;						//提取信息

	//控制值
	SCORE							lControlScore;						//控制值
};


//写游戏胜场记录
struct DBR_GR_WriteUserWinCount
{
	DWORD dwUserID;
	DWORD dwCount;
};

//请求抽奖
struct DBR_GR_QueryLottery
{
	DWORD	dwUserID;				//用户ID
	BYTE	byType;					//抽奖方式 0-红包 1-摇奖
};

//抽奖返回
struct DBO_GR_LotteryResult
{
	BYTE	byType;					//红包类型  0-没有抽到 1-兑换码 2-金币  3-房卡 4-钻石 5-奖牌 其他-其他预留
	BYTE	byIndex;				//摇奖序号，-1表示为红包， 0-11为摇奖
	DWORD	dwRewardCount;			//奖励道具数量
	TCHAR	szPacketID[128];				//兑换码
	TCHAR	szDescribe[128];		//描述
};

//Club牌友圈创建桌子
struct STR_DBO_GC_CLUB_CREATE_TABLE
{
	STR_SUB_CG_USER_CREATE_ROOM strCreateRoom;				//房间规则
	BYTE							byTableMode;			//1.普通桌子  2.私密桌子
	DWORD							dwClubRoomID;			//俱乐部房间ID
	long							lResultCode;			//非0表示 工会房卡不足
};

//加入桌子 不需要密码
struct STR_DBO_CG_USER_JOIN_TABLE_NO_PASS
{
	DWORD							dwPassword;		//房间密码
	double							dLongitude;		//经度
	double							dLatitude;		//纬度
	long							lResultCode;	//非0表示 没有找到可以加入的房间
	long							lResultCode2;	//非0表示 房卡不足

	DWORD							dwClubRoomID;	//俱乐部房间编号
	STR_SUB_CG_USER_CREATE_ROOM		strCreateRoom;	//房间规则
};

//加入桌子 金币大厅桌子
struct STR_DBO_CG_USER_JOIN_TABLE_HALL_GOLD
{
	DWORD							dwKindID;		//游戏ID
	BYTE							byGameType;		//初级场, 中级场, 高级场, 富豪场
	//DWORD							dwPassword;		//房间密码
	long							lResultCode;	//非0表示 没有找到可以加入的房间
	DWORD							dwMinGold;		//进入房间所需的最小金币

	STR_SUB_CG_USER_CREATE_ROOM		strCreateRoom;	//房间规则
};

//离开房间
struct DBR_GR_LeaveGameServer
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	DWORD							dwInoutIndex;						//记录索引
	DWORD							dwLeaveReason;						//离开原因
	DWORD							dwOnLineTimeCount;					//在线时长

	//系统信息
	DWORD							dwClientAddr;						//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识
};

//游戏记录
struct DBR_GR_GameScoreRecord
{
	//桌子信息
	WORD							wTableID;							//桌子号码
	WORD							wUserCount;							//用户数目
	WORD							wAndroidCount;						//机器数目

	//损耗税收
	SCORE							lWasteCount;						//损耗数目
	SCORE							lRevenueCount;						//税收数目
	//SCORE							wCurrentStockScore;					//当前库存
	SCORE							wChangeStockScore;					//当前库存改变值

	//统计信息
	DWORD							dwUserMemal;						//奖牌数目
	DWORD							dwPlayTimeCount;					//游戏时间 TODOLATER 删除

	//时间信息
	SYSTEMTIME						SystemTimeStart;					//开始时间
	SYSTEMTIME						SystemTimeConclude;					//结束时间

	//积分记录
	WORD							wRecordCount;						//记录数目
	tagGameScoreRecord				GameScoreRecord[MAX_CHAIR];			//游戏记录
};

//////////////////////////////////////////////////////////////////////////////////

//配置信息
struct DBO_GR_GameParameter
{
	//汇率信息
	WORD							wMedalRate;							//奖牌汇率
	WORD							wRevenueRate;						//银行税收

	//版本信息
	DWORD							dwClientVersion;					//客户版本
	DWORD							dwServerVersion;					//服务版本
};

//机器信息
struct DBO_GR_GameAndroidInfo
{
	//机器参数
	struct tagAndroidParameter
	{
		//属性资料
		DWORD							dwUserID;							//用户标识
		TCHAR							szNickName[LEN_NICKNAME];			//帐号昵称
		TCHAR							szHeadUrl[256];						//头像地址

		//用户资料
		BYTE							cbGender;							//用户性别
		SCORE							lOpenRoomCard;						//开房卡
		SCORE							lDiamond;							//钻石
		SCORE							lGold;								//金币
	};

	LONG							lResultCode;						//结果代码
	WORD							wAndroidCount;						//用户数目
	tagAndroidParameter				AndroidParameter[MAX_ANDROID];		//机器信息
};


//比赛名次
struct DBO_GR_MatchRank
{
	BYTE							cbRank;								//比赛名次
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称
	LONG							lMatchScore;						//用户得分
};

//权限管理
struct DBR_GR_ManageUserRight
{
	DWORD							dwUserID;							//目标用户
	DWORD							dwAddRight;							//添加权限
	DWORD							dwRemoveRight;						//删除权限
	bool							bGameRight;							//游戏权限
};

//比赛报名
struct DBR_GR_MatchFee
{
	//报名费用
	DWORD							dwUserID;							//用户ID
	DWORD							dwMatchFee;							//报名费用

	//系统信息
	DWORD							dwInoutIndex;						//进入标识
	DWORD							dwClientAddr;						//连接地址
	DWORD							dwMatchID;							//比赛ID
	DWORD							dwMatchNO;							//比赛场次
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//比赛开始
struct DBR_GR_MatchStart
{
	DWORD							dwMatchID;						//比赛标识
	DWORD							dwMatchNo;						//比赛场次
	WORD							wMatchCount;					//总场次
	LONG							lInitScore;						//初始积分
};

//比赛结束
struct DBR_GR_MatchOver
{
	DWORD							dwMatchID;						//比赛标识
	DWORD							dwMatchNo;						//比赛场次
	WORD							wMatchCount;					//总场次
};

//比赛奖励
struct DBR_GR_MatchReward
{
	DWORD								dwUserID;							//用户 I D
	DWORD								dwMatchID;							//比赛 I D 
	DWORD								dwMatchNO;							//比赛场次
	WORD								wRank;								//比赛名次
	SCORE								lMatchScore;						//比赛得分
	DWORD								dwExperience;						//用户经验
	DWORD								dwGold;								//游戏币
	DWORD								dwMedal;							//用户奖牌
	DWORD								dwClientAddr;						//连接地址
};

////单个比赛场配置
//struct MATCH_CONFIG
//{
//	//阶段信息
//	struct STAGE_INFO
//	{
//		TCHAR	szName[MAX_MATCH_TITLE];	//初赛、半决赛、决赛..
//		WORD	wXJCount;					//小局数
//		WORD	wPlayerCount;				//人数
//		WORD	wJinJi_Rule;				//晋级规则   同桌比较/全桌比较
//		WORD	wJinJi_Count;				//晋级名额
//	};
//
//	//比赛场信息
//	WORD		wMatchID;							//比赛场ID标识
//	TCHAR		szTitle[MAX_MATCH_TITLE];			//标题
//	TCHAR		szDescribe[MAX_MATCH_TITLE];		//比赛描述
//	STAGE_INFO	stage[MAX_MATCH_STAGE];				//阶段信息
//	WORD		wStageSize;							//阶段个数
//	SCORE		llBaseScore;						//底分
//
//	//比赛场属性
//	DWORD		dwStartTime;						//开始时间 （-1表示人满即开）
//	WORD		wStartType;							//开始类型  (0-人满开始  1-定时开始)
//	DWORD		dwLowestPlayer;						//最少参赛人数
//	SCORE		llFee;								//报名费
//	SCORE		llReword;							//奖励
//
//	//机器人信息
//	DWORD		dwRobotCount;						//机器人数量
//	DWORD		dwRobotJoinMin;						//机器人最小加入周期
//	DWORD		dwRobotJoinMax;						//机器人最大加入周期
//};

//比赛配置
struct STR_DBR_GR_MATCH_CONFIG
{
	MATCH_CONFIG	match_list[MAX_MATCH];
	WORD			match_size; 
};

//系统消息
struct DBR_GR_SystemMessage
{
	BYTE                            cbMessageID;                        //消息ID
	BYTE							cbMessageType;						//消息类型
	BYTE                            cbAllRoom;                          //全体房间
	DWORD							dwTimeRate;						    //时间频率
	TCHAR							szSystemMessage[LEN_USER_CHAT];		//系统消息
};

//房间难度调整
struct DBR_GR_RoomLevelModify
{
	WORD							wSeverID;							//服务器ID
	WORD							wGameLevel;							//游戏难度
};

//房间控制值调整
struct DBR_GR_RoomControlValModify
{
	DWORD							wSeverID;							//服务器ID
	SCORE							wControlVal;						//控制值
};

//房间控制信息
struct DBR_GR_RoomControlInfo
{
	WORD							wSeverID;							//服务器ID
	SCORE							wControlVal;						//控制值
	DWORD							wGameLevel;							//房间难易度
};

//查询房间控制值
struct DBR_GR_ReadRoomControlVal
 {
	DWORD							wSeverID;							//服务器ID
};
//用户控制值调整
struct DBR_GR_UserControlValModify
{
	DWORD							dwUserID;							//用户 I D
	SCORE							wControlVal;						//控制值
};

//读房间配置
struct DBR_GR_GetGameRoomConfig
{
	WORD							wSeverID;							//服务器ID
};

//读房间配置
struct DBR_GR_GameRoomConfig
{
	WORD							wSeverID;							//服务器ID
	TCHAR							szConfigName[50];					//配置名称
	TCHAR							szConfigValue[1024*10];				//配置描述	
	SCORE							wCurrentStock;						//当前库存
};

//写当前控制值
struct DBR_GR_WriteCurrentStock
{
	SCORE							wCurrentStock;						//当前库存
};
//////////////////////////////////////////////////////////////////////////////////

//更新开房信息
struct DBR_GR_UpdateTableInfo
{
	DWORD dwTableID;			//桌子ID
	DWORD dwPassword;			//桌子密码
	DWORD dwCreaterID;			//创建玩家ID
	WORD  wJuShu;				//局数
	BYTE  byMode;				//模式
	BYTE  byZhuangType;			//庄类型
	BYTE  byPlayerCount;		//玩家数
	BYTE  byMaxPlayerCount;		//最大玩家数
	BYTE  byIsStart;			//是否开始
	BYTE  byCost;				//扣卡数
	TCHAR szTime[24];			//创建时间
};


//添加用户到房间
struct DBR_GP_AddTableUser
{
	DWORD dwUserID;
	DWORD dwTableID;
};

//删除房间用户
struct DBR_GP_DeleteTableUser
{
	DWORD dwUserID;
	DWORD dwTableID;	
};

struct DBR_GP_EndTable
{
	DWORD dwTableID;			//桌子ID
	DWORD dwUserID[5];			//用户ID
	DWORD dwScore[5];			//用户分数
};

//返回开房信息
struct DBO_GP_TableInfo
{
	DWORD dwTableID;			//桌子ID
	DWORD dwPassword;			//桌子密码
	DWORD dwCreaterID;			//创建玩家ID
	WORD  wJuShu;				//局数
	BYTE  byMode;				//模式
	BYTE  byZhuangType;			//庄类型
	BYTE  byPlayerCount;		//玩家数
	BYTE  byMaxPlayerCount;		//最大玩家数
	BYTE  byIsStart;			//是否开始
	BYTE  byCost;				//扣卡数
	TCHAR szTime[24];			//创建时间

	DWORD dwUserID[5];			//玩家ID
	TCHAR szNickName[5][LEN_NICKNAME];		//玩家昵称
};


//返回开房信息
struct DBO_GP_EndTableInfo
{
	DWORD dwTableID;			//桌子ID
	DWORD dwPassword;			//桌子密码
	DWORD dwCreaterID;			//创建玩家ID
	WORD  wJuShu;				//局数
	BYTE  byMode;				//模式
	BYTE  byZhuangType;			//庄类型
	BYTE  byPlayerCount;		//玩家数
	BYTE  byMaxPlayerCount;		//最大玩家数
	BYTE  byIsStart;			//是否开始
	BYTE  byCost;				//扣卡数
	TCHAR szTime[24];			//创建时间

	DWORD dwUserID[5];			//玩家ID
	TCHAR szNickName[5][LEN_NICKNAME];		//玩家昵称
	DWORD szScore[5];
};



//pure牌友圈房间信息
struct STR_DBR_CLUB_ROOM_INFO
{
	DWORD	dwUserID;						//玩家ID
	DWORD	dwClubID;						//俱乐部标识
	DWORD	dwGameID;						//游戏ID
	BYTE	byModeID;						//1房卡 2金币
	TCHAR	szKindName[32];					//游戏名字

	//金币场特用
	DWORD	dwServiceGold;					//服务费，即固定抽水
	DWORD	dwRevenue;						//对胜者的固定抽水

	//底注 && 最低身价
	BYTE	byMask;							//1 AA支付;  2大赢家支付
	DWORD	dwDizhu;						//底注
	DWORD	dwGold;							//最低身价

	BYTE	byPlayerNum;					//当前房间的游戏人数
	BYTE	DissolveMinute;					//群主控制房间解散时间，0为不控制
	BYTE	byAllRound;						//游戏总局数
	BYTE	byChairNum;						//椅子数目

	//金币场的设置需要以 数据库里面的单个字段为准, 不能直接解析该字段 (服务费, 抽水, 支付方式, 底注, 最低身价)
	BYTE	szRealRoomRule[256];			//16进制规则字符串
};


//pure牌友圈桌子信息
struct STR_DBR_CLUB_TABLE_INFO
{
	DWORD		dwUserID;		//玩家ID
	DWORD		dwClubRoomID;	//俱乐部房间ID
	DWORD		dwTableID;		//桌子密码
	DWORD		dwTableState;	//0.桌子空闲 1.桌子已经开局  2.人数已满
	DWORD		dwLockState;	//0.桌子需要密码 1.桌子不需要密码
	BYTE		byCurrentRound;	//游戏当前局数
	BYTE		byMask;			//1创建 2更新 3解散

	//added by WangChengQing 服务器重新启动时候, 会通知数据库置空数据, 依赖标准是以下字段
	BYTE		byCompanyID;	//神秘字段
	DWORD		dwGameID;		//gameID
};

//pure牌友圈用户信息
struct STR_DBR_CLUB_PLAYER_INFO
{
	DWORD		dwUserID;		//玩家ID
	DWORD		dwTableID;		//桌子密码
	BYTE		byChairID;		//椅子号
	BYTE		byMask;			//1坐下 2离开
};


//数据库判断是否可以进入桌子   1.如果桌子是工会的桌子, 但玩家不在工会, 则不能进入桌子
struct STR_DBR_JOIN_ROOM
{
	DWORD							  dwTableID;	//桌子ID
	DWORD							  dwUserID;		//玩家ID
	long							  lResultCode;	//错误码
};

//金币大厅 桌子信息
struct STR_DBR_HALL_GOLD_TABLE_INFO
{
	DWORD		dwUserID;		//玩家ID
	DWORD		dwKindID;		//游戏ID
	BYTE		byGameType;		//游戏场类型  1.初级场  2.中级场  3.高级场  4.富豪场
	DWORD		dwTableID;		//桌子密码
	DWORD		dwTableState;	//0.桌子空闲 1.桌子已经开局  2.人数已满
	BYTE		byMask;			//1创建 2更新 3解散

	//added by WangChengQing 服务器重新启动时候, 会通知数据库置空数据, 依赖标准是以下字段
	DWORD		dwGameID;		//gameID
};

//金币大厅 用户信息
struct STR_DBR_HALL_GOLD_PLAYER_INFO
{
	DWORD		dwUserID;		//玩家ID
	DWORD		dwTableID;		//桌子密码
	BYTE		byChairID;		//椅子号
	BYTE		byMask;			//1坐下 2离开
};

//加载断线重连
struct STR_DBR_GR_LOAD_OFFLINE
{
	BYTE		byMystery;		//神秘字段
	DWORD		dwGameID;		//游戏ID
};

#pragma pack()

#endif
