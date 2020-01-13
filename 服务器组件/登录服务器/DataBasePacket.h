#ifndef DATA_BASE_PACKET_HEAD_FILE
#define DATA_BASE_PACKET_HEAD_FILE

#include "../../消息定义/CMD_LogonServer.h"
#pragma pack(1)

//发送给客户端的最大任务个数
#define MAX_SEND_CLIENT_TASK_NUM			5

#pragma region MDM_LIST 列表命令
/* *********************************************************************************
**   启动命令（启动登陆服时，服务单元发送的消息）				(201-300)

#define DBR_GP_ONLINE_COUNT_INFO	2									//统计在线 -- 发给数据库

#define DBR_UPDATA_MARQUEE			3									//加载跑马灯消息 查询
#define DBO_UPDATA_MARQUEE			1003									//加载跑马灯消息 返回
#define DBO_UPDATA_MARQUEE_FINISH	2003									//加载跑马灯消息 结束

//在线信息
struct DBR_GP_OnLineCountInfo
{
	DWORD							dwGameCount;						//类型数目
	DWORD							dwOnLineCountSum;					//总在线数
	tagOnLineInfoGame				OnLineCountGame[MAX_KIND];			//类型在线
};

//加载结果
struct DBO_GP_GameListResult
{
	BYTE							cbSuccess;							//成功标志
};

//加载跑马灯消息 返回
struct STR_DBO_UPDATA_MARQUEE
{
	BYTE							byMask;								//1代表新增;  2代表删除  3忽略此消息  4发送此消息
	DWORD							dwMarqueeID;						//跑马灯索引
	DWORD							dwMaruqeeTime;						//跑马灯 滚动间隔
	SYSTEMTIME						timeStartTime;						//跑马灯 开始时间
	SYSTEMTIME						timeEndTime;						//跑马灯 结束时间
	TCHAR							szMarqueeMsg[1024];					//跑马灯消息
};
#pragma endregion

#pragma region MDM_LOGON 登录命令
/* *********************************************************************************
**      MAIN:1   MDM_LOGON    登录命令
** *********************************************************************************/
//登录命令
#define	DBR_CL_LOGON_ACCOUNTS		101									//帐号登录
#define DBO_CL_LOGON_ACCOUNTS       1101                                 //账号登录

#define DBR_CL_LOGON_REGISTER		102									//帐号注册

#define DBR_CL_LOGON_PLATFORM       103                                 //平台登录
#define DBO_CL_LOGON_PLATFORM       1103                                 //平台登录

//帐号登录
struct STR_DBR_CL_LOGON_ACCOUNTS
{
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号
	TCHAR							szPassword[LEN_MD5];				//登录密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
	DWORD							dwProxyID;							//代理ID
};

//帐号注册
struct STR_DBR_CL_LOGON_REGISTER
{
	//Socket校验
	//LPVOID							pBindParameter;						//绑定参数
	
	//账号信息
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号
	TCHAR							szPassword[LEN_MD5];				//登录密码
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称
	BYTE							cbGender;							//用户性别
	TCHAR                           strMobilePhone[LEN_MOBILE_PHONE];   //手机号码 可以为null

	//机器标识
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
	DWORD							dwProxyID;							//代理ID
};
//账号注册返回
struct STR_DBO_CL_LOGON_REGISTER
{
	//操作结果
	DWORD                           dwResultCode;                       //结果标识
	//描述信息
	TCHAR							szDescribeString[128];				//描述消息

	//用户标识
	DWORD							dwUserID;							//用户标识
	//基本属性
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称
	BYTE							cbGender;							//用户性别
	WORD							wFaceID;							//头像索引
	TCHAR							szMySignature[LEN_MY_SIGNATURE];	//个性签名
	//荣誉属性
	SCORE                           dwUserDiamond;                      //用户钻石|元宝
	DWORD							dwUserMedal;						//用户奖牌
	DWORD							dwExperience;						//经验数值
	DWORD							dwLoveLiness;						//用户魅力
	//用户权限
	BYTE                            cbMasterOrder;                      //管理员等级 0表示非管理员
	BYTE							cbMemberOrder;						//会员等级   0表示非管理员
	SYSTEMTIME						MemberOverDate;						//会员到期时间
	//联系方式
	TCHAR                           szIdentityName[LEN_IDENTITY_NAME];  //真实姓名
	TCHAR                           szIdentityNum[LEN_IDENTITY_NUM];    //身份证号
	TCHAR                           szMobilePhone[LEN_MOBILE_PHONE];    //手机号码
	//账号信息
	TCHAR							szLasLogonIp[LEN_IP_ADDR];			//最后登录地址
	SYSTEMTIME						LasLogonDate;						//最后上线时间
	//道具信息
	SCORE							lUserScore;							//用户积分
	SCORE							lUserGold;							//用户金币
	SCORE							lOpenRoomCard;						//用户房卡
};

//平台登录
struct STR_DBR_CL_LOGON_PLATFORM
{
	LPVOID							pBindParameter;						//绑定参数
	TCHAR							szOpenID[33];						//OpenID | tokenID
	TCHAR							szUnionID[33];						//UnionID
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称
	BYTE							cbGender;							//用户性别
	TCHAR							strHeadUrl[LEN_HEAD_URL];			//头像地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
	DWORD							dwProxyID;							//代理ID
};

#pragma endregion

#pragma region MDM_SERVICE 服务命令
/* *********************************************************************************
**      MAIN:3   MDM_SERVICE    服务命令 
** *********************************************************************************/
#define DBR_CL_SERVICE_USER_FEEDBACK		301									//玩家反馈
#define	DBO_CL_SERVICE_USER_FEEDBACK		1301								//玩家反馈返回

#define DBR_CL_SERVICE_REFRESH_USER_INFO	302									//刷新用户信息
#define DBO_CL_SERVICE_REFRESH_USER_INFO	1302								//刷新用户信息返回

#define DBR_CL_SERVICE_QUERY_ROOM_LIST		303									//查询开房信息列表
#define DBO_CL_SERVICE_QUERY_ROOM_LIST		1303								//查询开房信息列表返回

#define DBR_CL_SERVICE_GET_RICH_LIST			304								//获取富豪榜
#define DBO_CL_SERVICE_GET_RICH_LIST			1304							//获取富豪榜返回

#define DBR_CL_SERVICE_GET_USER_RECORD_LIST		305								//获取用户录像列表（大局）
#define DBO_CL_SERVICE_GET_USER_RECORD_LIST		1305							//获取用户录像列表返回

#define DBR_CL_SERVICE_GET_SPECIFIC_RECORD		306								//获取指定ID录像（小局）
#define DBO_CL_SERVICE_GET_SPECIFIC_RECORD		1306							//获取指定ID录像返回

#define DBR_CL_SERVICE_ONLINE_REWARD			307								//获取在线奖励
#define DBO_CL_SERVICE_ONLINE_REWARD			1307							//获取在线奖励返回

#define DBR_CL_SERVICE_GET_TASK_LIST			308								//获取任务列表
#define DBO_CL_SERVICE_GET_TASK_LIST			1308							//获取任务列表返回

#define DBR_CL_SERVICE_GET_TASK_REWARD			309								//领取任务奖励
#define DBO_CL_SERVICE_GET_TASK_REWARD			1309							//任务完成返回

#define DBR_CL_SERVICE_REQUEST_LOTTERY			310								//请求抽奖
#define DBO_CL_SERVICE_REQUEST_LOTTERY			1310							//请求抽奖返回

#define DBR_CL_SERVICE_MODIFY_PERSONAL_INFO		311								//修改个人资料
#define DBO_CL_SERVICE_MODIFY_PERSONAL_INFO		1311							//修改个人资料返回

#define DBR_CL_USER_QUERY_SCORE_INFO			312								//查询(变更)金币房卡信息
#define DBO_CL_USER_QUERY_SCORE_INFO			1312							//查询（变更）金币房卡信息返回

//下面这些排行榜的消息号，是自身的排行榜类调用的消息 TODO 暂时放在这里
#define DBR_CL_SERVICE_GET_RANK_REWARD			313								//领取排行榜奖励
#define DBO_CL_SERVICE_GET_RANK_REWARD			1313							//领取排行榜奖励返回
#define DBR_GP_UPDATE_RANK_VALUE				2313							//更新排行榜用户数据 TODO 流程处理有问题
#define DBR_GP_READ_RANK_LIST					3313							//读取数据库排行榜


#define DBR_GP_MODIFY_USER_INSURE				314								//变更用户财富		TODO 在登录奖励中用到，暂时注释了

#define DBR_GP_READ_VERSION_AND_URL				315								//读取版本号		//暂时注释，后面可能会用到

#define DBR_CL_SERVICE_PURE_STANDING_LIST		316								//pure大厅排行版
#define DBO_LC_SERVICE_PURE_STANDING_LIST		1316							//pure大厅排行版 返回
#define DBO_LC_SERVICE_PURE_STANDING_FINISH		2316							//pure大厅排行版 结束

#define DBR_CL_SERVICE_PURE_RECORD_LIST			317								//pure大局战绩
#define DBO_LC_SERVICE_PURE_RECORD_LIST			1317							//pure大局战绩 返回
#define DBO_LC_SERVICE_PURE_RECORD_LIST_PINFO	2317							//pure大局玩家信息 返回
#define DBO_LC_SERVICE_PURE_RECORD_FINISH		3317							//pure大局战绩 结束

#define DBR_CL_SERVICE_PURE_XJ_RECORD_LIST		318								//pure小局战绩
#define DBO_LC_SERVICE_PURE_XJ_RECORD_LIST		1318							//pure小局战绩 返回
#define DBO_LC_SERVICE_PURE_XJ_RECORD_LIST_PINFO 2318							//pure小局玩家信息 返回
#define DBO_LC_SERVICE_PURE_XJ_RECORD_FINISH	3318							//pure小局战绩 结束

#define DBR_CL_SERVICE_GOLD_INFO				319								//请求金币大厅信息
#define DBO_LC_SERVICE_GOLD_INFO				1319							//请求金币大厅信息 返回
#define DBO_LC_SERVICE_GOLD_INFO_FINISH			2319							//请求金币大厅信息 结束

#define DBR_CL_SERVICE_XJ_RECORD_PLAYBACK		320								//小局录像回放
#define DBO_LC_SERVICE_XJ_RECORD_PLAYBACK		1320							//小局录像回放返回

#define DBR_CL_SERVICE_CUSTOMER_MESSEGE			321								//客服消息
#define DBO_LC_SERVICE_CUSTOMER_MESSEGE			1321							//客服消息返回

//////////////////////////////////////////////////////////////////////////////////
//【结构体定义】

//玩家反馈
struct  STR_DBR_CL_SERVICE_FEEDBACK
{
	DWORD							dwUserID;							//用户 ID
	TCHAR							szFB_Title[LEN_USERNOTE];			//反馈标题
	TCHAR							szFB_Content[LEN_USER_NOTE];		//反馈内容
	TCHAR							szContact[LEN_MOBILE_PHONE];		//联系方式
};
//玩家反馈返回
struct STR_DBO_CL_SERVICE_FEEDBACK
{
	LONG							lResultCode;						//操作代码
	TCHAR							szDescribeString[128];				//成功消息
};

//刷新用户信息
struct STR_DBR_CL_SERCIVR_REFRESH_INFO
{
	DWORD							dwUserID;							//用户ID
};
//刷新用户信息返回
struct STR_DBO_CL_SERCIVR_REFRESH_INFO
{
	DWORD                           dwResultCode;                       //结果标识
	TCHAR							szDescribeString[128];				//描述消息

	/* *****************************    用户信息     ****************************/
	//用户标识
	DWORD							dwUserID;							//用户标识
	//基本属性
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称
	BYTE							cbGender;							//用户性别
	WORD							wFaceID;							//头像索引
	TCHAR							szMySignature[LEN_MY_SIGNATURE];	//个性签名
	//荣誉属性
	SCORE                           dwUserDiamond;						//用户元宝
	DWORD							dwUserMedal;						//用户奖牌
	DWORD							dwExperience;						//经验数值
	DWORD							dwLoveLiness;						//用户魅力
	//用户权限
	BYTE                            cbMasterOrder;                      //管理员等级 0表示非管理员
	BYTE							cbMemberOrder;						//会员等级   0表示非管理员
	SYSTEMTIME						MemberOverDate;						//会员到期时间
	//联系方式
	TCHAR                           szIdentityName[LEN_IDENTITY_NAME];  //真实姓名
	TCHAR                           szIdentityNum[LEN_IDENTITY_NUM];    //身份证号
	TCHAR                           szMobilePhone[LEN_MOBILE_PHONE];    //手机号码
	
	/* *****************************    账号信息     ****************************/
	TCHAR							szLasLogonIp[LEN_IP_ADDR];			//最后登录地址
	SYSTEMTIME						LasLogonDate;						//最后上线时间
	
	/* *****************************    附加数据     ****************************/
	//牌友圈 
	DWORD							dwGroupID;							//社团标识
	TCHAR							szGroupName[LEN_GROUP_NAME];		//社团名字

	//道具信息
	SCORE							lUserScore;							//用户积分
	SCORE							lUserGold;							//用户金币
	SCORE							lUserRoomCard;						//用户房卡
};

//查询开房列表
struct STR_DBR_CL_SERCIVR_QUERY_ROOMLIST
{
	DWORD dwUserID;
};
//查询开房列表返回
struct STR_DBO_CL_SERCIVR_QUERY_ROOMLIST
{
	DWORD dwTableID;					//桌子ID
	DWORD dwPassword;					//桌子密码
	DWORD dwCreaterID;					//创建玩家ID
	WORD  wJuShu;						//局数
	BYTE  byMode;						//模式
	BYTE  byZhuangType;					//庄类型
	BYTE  byPlayerCount;				//玩家数
	BYTE  byMaxPlayerCount;				//最大玩家数
	BYTE  byIsStart;					//是否开始
	BYTE  byCost;						//扣卡数
	TCHAR szTime[24];					//创建时间
	DWORD dwUserID[5];					//玩家ID
	TCHAR szNickName[5][LEN_NICKNAME];	//玩家昵称
	DWORD dwScore[5];					//用户积分
};

//获取富豪榜
struct STR_DBR_CL_SERCIVR_GET_RICHLIST
{
};
//获取富豪榜返回
struct STR_DBO_CL_SERCIVR_GET_RICHLIST
{
	BYTE byCount;						//排行榜数量
	tagSingleRichInfo RegalInfoList[15];	//排行列表
};

//获取用户录像列表
struct STR_DBR_CL_SERCIVR_GET_RECORDLIST
{
	DWORD							dwUserID;		//用户ID	
};
//获取用户录像列表返回
struct STR_DBO_CL_SERCIVR_GET_RECORDLIST
{
	DWORD							dwTableID;				//桌子ID
	DWORD							dwUserID[5];			//玩家ID
	TCHAR							szUserName[5][LEN_NICKNAME];//用户昵称
	BYTE							byGameMode;			//游戏模式
	BYTE							byZhuangType;		//坐庄类型
	WORD							wCurrentJuShu;		//当前局数
	WORD							wTotalJuShu;		//总局数
	TCHAR							szGameTime[30];		//游戏时间
	TCHAR							szScore[50];		//玩家单局分数
	TCHAR							szTotalScore[50];	//玩家总分数
	DWORD							dwDataSize;			//数据长度
};

//获取小局录像列表
struct STR_DBR_CL_SERCIVR_GET_SPECIFIC_RECORD
{
	DWORD dwTableID;	   //桌子ID
};
//获取小局录像列表返回
struct STR_DBO_CL_SERCIVR_GET_SPECIFIC_RECORD
{
	WORD							wCurrentJuShu;					//当前局数
	TCHAR							szNickName[5][LEN_NICKNAME];	//用户昵称
	TCHAR							szScore[50];					//玩家小局分数
};

//小局录像回放
struct STR_DBR_CL_SERVICE_XJ_RECORD_PLAYBACK
{
	DWORD		dwRecordID;				//录像ID
};
//小局录像回放返回
struct STR_DBO_LC_SERVICE_XJ_RECORD_PLAYBACK
{
	WORD							wCurrentCount;						//当前局数
	DWORD							dwDataSize;							//数据长度
	BYTE							cbRecordData[LEN_MAX_RECORD_SIZE];	//录像数据
};

//客服消息
struct STR_DBR_CL_SERVICE_CUSTOMER_MESSEGE
{
	BYTE					cbMessegeFlag;						//消息标识	0-表示消息提示	>=1表示具体消息请求
};
//客服提示消息 返回
struct STR_DBO_LC_SERVICE_CUSTOMER_MESSEGE
{
	BYTE					cbMessegeFlag;						//消息标识
	TCHAR					szMessege[LEN_MAX_SERVICE_MEG_SIZE];//消息内容
};

//获取在线奖励
struct STR_DBR_CL_SERCIVR_ONLINE_REWARD
{
	DWORD dwUserID;
};
//获取在线奖励返回
struct STR_DBO_CL_SERCIVR_ONLINE_REWARD
{
	BYTE  byType;		//奖励类型
	DWORD dwCount;		//奖励数量
	TCHAR szDescribe[128]; //描述
};

//获取任务列表
struct STR_DBR_CL_SERCIVR_GET_TASKLIST
{
	DWORD	dwUserID;									//用户ID
};
//获取任务列表返回
struct STR_DBO_CL_SERCIVR_GET_TASKLIST
{
	WORD	wTaskID;							//任务ID
	BYTE	cbTaskType;							//任务类型 1日常; 2周常; 3月常; 4成就
	BYTE	cbActionType;						//任务动作类型 ，1绑定信息; 2微信分享; 3充值; 4房卡游戏; 5金币游戏	
	DWORD	dwNeedCount;						//任务完成所需的数量, 比如完成5局房卡游戏, 这里就是5	
	TCHAR	szDescribe[128];					//任务描述
	BYTE	byRewardType;						//奖励类型1金币; 2房卡; 3钻石
	DWORD	dwRewardCount;						//奖励数量
	BYTE	cbTaskStatus;						//任务状态 ，0-进行中  1-完成但未领取 2-完成已领取
	DWORD	dwFinishedCount;					//已完成的任务所需数量				
};

//领取任务奖励
struct STR_DBR_CL_SERCIVR_GET_TASK_REWARD
{
	DWORD	dwUserID;
	WORD	wTaskID;	
};
//领取任务奖励返回
struct STR_DBO_CL_SERCIVR_GET_TASK_REWARD
{
	LONG							lResultCode;						//错误码 0-成功 其他失败
	TCHAR							szDescribe[128];					//描述
};

//请求抽奖
struct STR_DBR_CL_SERCIVR_REQUEST_LOTTERY
{
	DWORD	dwUserID;				//用户ID
};
//请求抽奖返回
struct STR_DBO_CL_SERCIVR_REQUEST_LOTTERY
{
	LONG							lResultCode;						//操作代码
	TCHAR							szDescribeString[128];				//描述消息
	BYTE							byIndex;							//摇奖序号，0-11为摇奖
};

//修改个人资料返回
struct STR_DBO_CL_MODIFY_PERSONL_INFO
{
	LONG							lResultCode;						//操作代码
	TCHAR							szDescribeString[128];				//成功消息
};

//查询用户金币房卡
struct STR_DBR_CL_SERCIVR_QUERY_SCORE_INFO
{
	DWORD							dwUserID;							//用户ID
};
//查询用户金币房卡返回
struct STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO
{
	LONG						lResultCode;						//操作代码
	TCHAR						szDescribeString[128];				//成功消息

	DWORD						dwUserID;							//用户ID
	SCORE						lOpenRoomCard;						//开房卡
	SCORE						lGold;								//金币	
	SCORE						lDiamond;							//钻石
	SCORE						lRewardCard;						//奖牌
	SCORE						lScore;								//积分
};

//领取排行榜奖励
struct STR_DBR_CL_SERCIVR_GET_RANK_REWARD
{
	DWORD	dwUserID;
	BYTE	byIndex;
};
//领取排行榜奖励返回   
struct STR_DBO_CL_SERCIVR_GET_RANK_REWARD				
{
	LONG	lResultCode;												//0-领取成功  其他失败
	TCHAR	szDescribe[128];											//完成描述，例如 领取成功,获得10000金币
};

//变更用户财富
struct DBR_GP_ModifyUserInsure
{
	DWORD	dwUserID;	
	SCORE	lGold;				//金币
	SCORE	lOpenRoomCard;		//房卡
	SCORE	lDiamond;			//钻石
	SCORE	lRewardCard;		//奖牌
	SCORE	lScore;				//积分
};

//公用消息-失败返回
struct STR_DBO_CL_SERCIVR_FAIL
{
	LONG							lResultCode;						//操作代码
	TCHAR							szDescribeString[128];				//描述消息
};
#pragma endregion

#pragma region MDM_CLUB  牌友圈
//////////////////////////////////////////////////////////////////////////////////
/* *******************************************************************************
**          MAIN:6      MDM_CLUB  牌友圈(版本2)
** ******************************************************************************/
#define DBR_CL_CLUB_ALL_CLUB_INFO_LIST			601							//查询牌友圈列表
#define DBO_LC_CLUB_ALL_CLUB_INFO_LIST			1601						//查询牌友圈列表 返回
#define DBO_LC_CLUB_ALL_INFO_FINISH				2601						//查询牌友圈列表 结束

#define DBR_CL_CLUB_ROOM_LIST					602							//查询指定牌友圈房间列表
#define DBO_LC_CLUB_ROOM_LIST					1602						//查询指定牌友圈房间列表 
#define DBO_LC_CLUB_ROOM_LIST_FINISH			2602						//查询指定牌友圈房间列表 结束

#define DBR_CL_CLUB_RANDOM_CLUB_LIST			603							//查询未满员, 随机牌友圈(最大9个)
#define DBO_LC_CLUB_RANDOM_CLUB_LIST			1603						//查询未满员随机牌友圈 
#define DBO_LC_CLUB_RANDOM_CLUB_LIST_FINISH		2603						//查询未满员随机牌友圈 结束

#define DBR_CL_CLUB_JOIN_CLUB					604							//申请加入牌友圈
#define DBO_LC_CLUB_JOIN_CLUB					1604						//申请加入牌友圈 返回
#define DBO_LC_CLUB_JOIN_CLUB_BDCAST			2604						//申请加入牌友圈 广播
#define DBO_LC_CLUB_JOIN_CLUB_RE				3604						//申请加入牌友圈 自动加入, 通知客户端实时刷新

#define DBR_CL_CLUB_CREATE_CLUB					605							//创建牌友圈
#define DBO_LC_CLUB_CREATE_CLUB					1605						//创建牌友圈返回

#define DBR_CL_CLUB_JOIN_ROOM					607							//申请加入房间
#define DBO_LC_CLUB_JOIN_ROOM					1607						//申请加入房间 返回

#define DBR_CL_CLUB_APPLICANT_RESULT			609							//群主|管理对申请消息的答复(同意|拒绝)
#define DBO_LC_CLUB_APPLICANT_RESULT			1609						//群主|管理对申请消息的答复(同意|拒绝) 返回
//#define DBO_LC_CLUB_JOIN_CLUB_MESSAGE_FINISH	2609						//群主|管理对申请消息的答复(同意|拒绝) 结束

#define DBR_CL_CLUB_MEMBER_MANAGER				610							//请求成员数据
#define DBO_LC_CLUB_MEMBER_MANAGER				1610						//请求成员数据 返回
#define DBO_LC_CLUB_MEMBER_MANAGER_FINISH		2610						//请求成员数据 结束
#define DBO_LC_CLUB_DATA						3610						//工会基本信息

#define DBR_CL_CLUB_QUIT						611							//踢出退出请求
#define DBO_LC_CLUB_QUIT						1611						//踢出退出请求 返回

#define DBR_CL_CLUB_DISS_CLUB					612							//解散牌友圈
#define DBO_LC_CLUB_DISS_CLUB					1612						//解散牌友圈 返回

#define DBO_LC_CLUB_TABLE_LIST_TABLE			1613						//桌子列表 返回
#define DBO_LC_CLUB_TABLE_LIST_USER				2613						//桌子玩家列表 返回
//#define DBO_LC_CLUB_TABLE_LIST_FINISH					1613					//桌子列表 结束 client不依赖次消息号显示

#define DBR_CL_CLUB_ROOM_SETTING				614							//房间设置
#define DBO_LC_CLUB_ROOM_SETTING				1614						//房间设置 返回

#define DBR_CL_CLUB_NOTICE						615							//牌友圈公告
#define DBO_LC_CLUB_NOTICE						1615						//牌友圈公告 返回

#define DBR_CL_CLUB_MESSAGE						616							//牌友圈简介 -- 对外部展示的信息
#define DBO_LC_CLUB_MESSAGE						1616						//牌友圈简介 返回  注:牌友圈简介 无须广播

#define DBR_CL_CLUB_CONTRIBUTE_FK				617							//贡献房卡 -- 对外部展示的信息
#define DBO_LC_CLUB_CONTRIBUTE_FK				1617						//贡献房卡 返回

#define DBR_CL_CLUB_AUTO_AGREE					618							//牌友圈设置 -- 申请加入自动同意
#define DBO_LC_CLUB_AUTO_AGREE					1618						//牌友圈设置 返回

#define DBR_CL_CLUB_INVITE						619							//邀请他入加入牌友圈 
#define DBO_LC_CLUB_INVITE						1619						//邀请他人加入牌友圈 返回
#define DBO_LC_CLUB_INVITE_REMIND				2619						//邀请人的提醒

#define DBR_CL_CLUB_INVITE_RESULT				620							//被邀请人的回复
#define DBO_LC_CLUB_INVITE_RESULT				1620						//被邀请人的回复 返回

#define DBR_CL_CLUB_INQUERY_LIST				621							//被邀请人查看自己的邀请列表
#define DBO_LC_CLUB_INQUERY_LIST				1621						//被邀请人查看自己的邀请列表 返回
#define DBO_LC_CLUB_INQUERY_LIST_FINISH			2621						//被邀请人查看自己的邀请列表 结束

#define DBR_CL_CLUB_APPLICANT_LIST				622							//申请人列表
#define DBO_LC_CLUB_APPLICANT_LIST				1622						//申请人列表 返回
#define DBO_LC_CLUB_APPLICANT_LIST_FINISH		2622						//申请人列表 结束

#define DBR_CL_CLUB_APPOINTMENT					623							//职务任免 0普通 1管理员 
#define DBO_LC_CLUB_APPOINTMENT					1623						//职务任免 返回
#define DBO_LC_CLUB_APPOINTMENT_NOTE			2623						//职务任免 提醒

#define DBR_CL_CLUB_CHAT						624							//牌友圈聊天
#define DBR_CL_WORD_CHAT						1624						//世界聊天
#define DBR_CL_SYSTEM_CHAT						2624						//系统聊天
#define DBR_CL_SECRET_CHAT						3624						//私密聊天
#define DBO_LC_CHAT_ALL							4624						//聊天 返回
#define DBO_LC_CHAT_ALL_BDCAST					5624						//聊天 广播


#define DBR_CL_CLUB_ROOM_USER_LEAVE				625							//玩家离开俱乐部房间

#define DBR_CL_CLUB_RECORD_LIST					626							//工会战绩统计
#define DBO_LC_CLUB_RECORD_LIST					1626						//工会战绩统计 返回
#define DBO_LC_CLUB_RECORD_FINISH				2626						//工会战绩统计 结束

#define DBR_CL_CLUB_ROOM_DISSOLVE				627							//解散房间请求
#define DBO_LC_CLUB_ROOM_DISSOLVE				1627						//解散房间请求 返回

#define DBR_CL_CLUB_TABLE_DISSOLVE				628							//解散桌子请求
#define DBO_LC_CLUB_TABLE_DISSOLVE				1628						//解散桌子请求 返回

#define DBR_CL_CLUB_STICKY_POST					629							//牌友圈置顶请求
#define DBO_LC_CLUB_STICKY_POST					1629						//牌友圈置顶请求 返回

#define DBR_CL_CLUB_ROOM_QUERY_SETTING			630							//请求房间设置
#define DBO_LC_CLUB_ROOM_QUERY_SETTING			1630						//请求房间设置 返回
#pragma region 结构体	

//请求牌友圈列表 返回
struct STR_DBO_LC_CLUB_ALL_CLUB_INFO_LIST
{
	DWORD	dwClubID;									//牌友圈ID
	TCHAR	szClubName[LEN_NICKNAME];					//牌友圈名字
	DWORD	dwMajorKindID;								//主打游戏ID
	TCHAR	szMinorKindID[128];							//次要游戏 最多10个
	BYTE	byClubLevel;								//牌友圈等级
	WORD	wMemberNum;									//当前人数
	TCHAR	szNotice[128];								//牌友圈 公告
	TCHAR	szMessage[128];								//牌友圈 简介
	DWORD	dwClubOwner;								//群主ID
	BYTE	byClubRole;								//玩家职务 0群主 1管理 2一般成员
};

//查询特定牌友圈房间列表
struct STR_DBR_CL_CLUB_ROOM_LIST
{
	DWORD	dwClubID;									//牌友圈ID
};

//创建牌友圈
struct STR_DBR_CL_CLUB_CREATE_CLUB
{
	DWORD	dwUserID;									//玩家ID
	TCHAR	szClubName[LEN_NICKNAME];					//牌友圈名字
	DWORD	dwMajorKindID;								//主打游戏ID
	TCHAR	szMinorKindID[128];							//次要游戏 最多10个

	TCHAR	szNotice[128];								//牌友圈公告
	TCHAR	szMessag[128];								//牌友圈简介
};

//创建牌友圈房间 返回
struct STR_DBO_CL_CLUB_CREATE_CLUB_ROOM_APPLY
{
	LONG	lResultCode;								//0-购买成功  其他失败
	TCHAR	szDescribe[128];							//失败原因
};

//申请加入牌友圈 返回
struct STR_DBO_LC_CLUB_JOIN_CLUB
{
	LONG	lResultCode;								//0-购买成功  其他失败
	TCHAR	szDescribe[128];							//失败原因
};

//申请加入房间
struct STR_DBR_CL_CLUB_JOIN_ROOM
{
	DWORD	dwUserID;									//玩家ID
	DWORD	dwRoomID;									//房间ID
};

//申请加入桌子
struct STR_DBR_CL_CLUB_JOIN_TABLE
{
	DWORD	dwUserID;									//玩家ID
	int		JoinType;									//加入类型
	DWORD	dwTableID;									//桌子ID
	DWORD	dwChairID;									//椅子ID
	DWORD	dwCreatePassword;							//创建密码房密码
	DWORD	dwJoinPassword;								//加入密码房密码
};

//请求成员数据
struct STR_DBR_CL_CLUB_MEMBER_MANAGER
{
	DWORD	dwUserID;									//玩家ID
	DWORD	dwClubID;									//牌友圈ID
};

//群主|管理 对每一条申请消息的答复
struct STR_DBR_CL_CLUB_JOIN_CLUB_MESSAGE
{
	DWORD	dwResult;									//0同意 其他拒绝
	DWORD	dwUserID;									//玩家ID
};

//群主|管理 接收玩家申请入群的消息
struct STR_DBO_LC_CLUB_JOIN_CLUB_MESSAGE
{
	DWORD	dwUserID;									//申请玩家ID
	TCHAR	szUserName[LEN_NICKNAME];					//申请玩家昵称

	DWORD	dwCustomID;									//自定义标识
	TCHAR	szHeadUrl[LEN_HEAD_URL];							//头像地址
};

//踢出退出 请求
struct STR_DBR_CL_CLUB_QUIT
{
	DWORD	dwClubID;									//牌友圈ID
	DWORD	dwUserID;									//玩家ID
	DWORD	dwQuitPlayerID;								//被踢出玩家ID
};

//牌友圈置顶
struct STR_DBR_CL_CLUB_STICKY_POST
{
	DWORD	dwUserID;									//玩家ID
	DWORD	dwClubID;									//俱乐部ID
	BYTE	cbTopFlag;									//是否置顶	1-置顶	0-取消置顶
};

//牌友圈置顶返回
struct STR_DBO_LC_CLUB_STICKY_POST
{
	LONG	lResultCode;								//0成功  其他失败
};

#pragma endregion
#pragma endregion

#pragma region MDM_SHOP 商城道具
#define DBR_CL_SHOP_QUERY						701							//商城查询 -- 查询商品打折情况
#define DBO_CL_SHOP_QUERY						702							//商城查询 -- 查询商品打折情况
#define DBO_CL_SHOP_QUERY_FINISH				703							//商城查询 -- 查询商品打折情况


#define DBR_CL_SHOP_MONEY						704							//(暂未用到)金钱购买道具
#define DBO_CL_SHOP_MONEY						705							//(暂未用到)金钱购买道具

#define DBR_CL_SHOP_DIAMOND						706							//钻石购买道具
#define DBO_CL_SHOP_DIAMOND						707							//钻石购买道具

#define DBR_CL_BAG_QUERY						708							//背包物品查询
#define DBO_CL_BAG_QUERY						709							//背包物品查询
#define DBO_CL_BAG_FINISH						710							//背包物品查询结束


//查询商城DBR
struct STR_DBR_CL_SHOP_QUERY
{
	DWORD dwUserID;			//玩家ID
	BYTE byGoodsType;		//物品类型 1rmb购买房卡  2rmb购买金币 3金币购买礼物 4积分购买礼物
};

//钻石购买道具DBO
struct STR_DBO_CL_SHOP_DIAMOND
{
	LONG							lResultCode;						//操作代码
	TCHAR							szDescribeString[128];				//成功消息
};

//背包物品查询
struct STR_DBR_CL_BAG_QUERY
{
	DWORD dwUserID;			//玩家ID
};

//背包物品查询 DBO
struct STR_DBO_CL_BAG_QUERY
{
	DWORD dwGoodsID;		//物品ID
	DWORD dwGoodsNum;		//物品数量
	BYTE  byGoodsType;		//物品类型
};
#pragma endregion

#pragma region 待整理

/* *******************************************************************************
**        MAIN:6       MDM_GIFT   礼物道具
** ******************************************************************************/
#define DBR_CL_GIFT_GIVE_PROPS				801							//赠送道具
#define DBO_CL_GIFT_GIVE_PROPS				1801						//赠送道具返回
#define DBO_CL_GIFT_GIVE_PROPS_SHOW			2801						//赠送道具 通知接受者


#define DBR_CL_USER_RECHARGE_INFO			802							//充值信息
#define DBO_CL_USER_RECHARGE_INFO			1802						//充值信息

#define DBR_CL_USER_EXCHANGE_INFO			803							//兑换信息
#define DBO_CL_USER_EXCHANGE_INFO			1803						//兑换信息

#define	DBO_CL_USER_COMMAND_RESULT			804							//公用操作结果


//公共操作
struct DBR_CommandSource
{
	WORD							MainCommand;						//操作主ID
	WORD							SubCommand;							//操作子ID
};

//充值信息
struct STR_DBR_CL_OTHER_RECHARGE_INFO
{
	DWORD							dwUserID;							//用户ID
};
//充值信息返回
struct STR_DBO_CL_OTHER_RECHARGE_INFO
{
	SCORE								dwMinMoney;///最少充值钱  
	UINT32								dwChangeScale;  ///兑换比例
};

//兑换道具信息
struct STR_DBR_CL_OTHER_EXCHANGE_INFO
{
	DWORD							dwUserID;							//用户ID
};
//兑换道具信息返回
struct STR_DBO_CL_OTHER_EXCHANGE_INFO
{
	SCORE								dwMinMoney;		///最少充值钱  
	UINT32								dwChangeScale;  ///兑换比例
	SCORE								dwWithdrawals;  ///待提现RMB
	SCORE								dwBankMoney;	///银行金币
};


//公共操作返回	
struct DBO_GP_OperateCommand
{
	DBR_CommandSource				mCommand;							//协议消息
	LONG							lResultCode;						//操作代码
	TCHAR							szDescribeString[128];				//成功消息
};

/* *******************************************************************************
**       其他服务器发送自身发送数据库消息
** ******************************************************************************/
struct DBR_GP_UserRankItem
{
	DWORD dwUserID;		
	DWORD dwCount;		//消费或胜利数量
	BYTE byRankNum;		//名次
	BYTE byReceived;	//已领取
	BYTE byIndex;		//排行榜序号
};

#pragma endregion

#endif