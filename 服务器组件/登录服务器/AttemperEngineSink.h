#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#include "Stdafx.h"
#include "ServerListManager.h"
#include "DataBasePacket.h"

typedef CWHArray<tagOfflineUser>COfflineUserItemArray;					//断线玩家 列表
typedef CWHArray<STR_DBO_UPDATA_MARQUEE> MarqueeMsgArray;				//跑马灯消息 列表

//绑定参数
struct tagBindParameter
{
	//网络参数
	DWORD							dwUserID;							//用户ID
	DWORD							dwSocketID;							//网络标识
	DWORD							dwClientAddr;						//连接地址
	DWORD							dwActiveTime;						//激活时间
};

//////////////////////////////////////////////////////////////////////////////////
//1、继承内核中的IAttemperEngineSink类
//2、处理具体收到的 【网络事件 + 时间事件 + 数据库事件】
//////////////////////////////////////////////////////////////////////////////////
class CAttemperEngineSink : public IAttemperEngineSink
{
	//友元定义
	friend class CServiceUnits;

	//状态变量
protected:
	bool                            m_bShowServerStatus;                //显示服务器状态

	//变量定义
protected:
	tagBindParameter *				m_pBindParameter;					//辅助数组
	void*							m_pRankManager;						//排行榜管理器

	//组件变量
protected:
	CServerListManager				m_ServerListManager;				//Server列表管理
	COfflineUserItemArray			m_OfflineUserListManager;			//断线玩家列表
	MarqueeMsgArray					m_MarqueeMsgListManager;			//跑马灯消息列表

#pragma region 构造析构
	//函数定义
public:
	//构造函数
	CAttemperEngineSink();
	//析构函数
	virtual ~CAttemperEngineSink();

	//管理接口
public:
	//启动服务
	virtual bool StartService(){return true;}
	//停止服务
	virtual bool ConcludeService(){return true;}
#pragma endregion

#pragma region 事件接口
	//异步接口
public:
	//启动事件
	virtual bool OnAttemperEngineStart(IUnknownEx * pIUnknownEx);
	//停止事件
	virtual bool OnAttemperEngineConclude(IUnknownEx * pIUnknownEx);

	//事件接口
public:
	//控制事件
	virtual bool OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize);

	//内核事件
public:
	//时间事件
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM wBindParam);
	//数据库事件
	virtual bool OnEventDataBaseResult(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize);

	//socket::client （与协调服连接）
public:
	//连接事件
	virtual bool OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode);
	//关闭事件
	virtual bool OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason);
	//读取事件
	virtual bool OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize);

	//socket::server （与client连接）
public:
	//应答事件
	virtual bool OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID);
	//关闭事件
	virtual bool OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID);
	//读取事件
	virtual bool OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID);
#pragma endregion

	//辅助函数
protected:
	//TODONOW added by WangChengQing
	//判断玩家状态, 并通知协调服  -- 1大厅在线 ，2正在游戏，3游戏断线，4离线
	void CheckUserState_Logon(DWORD dwUserID, bool bIsExist);


#pragma region 启动命令
	//列表事件（SendControlPacket的响应函数）
protected:
	//游戏种类
	bool OnDBPCGameTypeItem(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//游戏类型
	bool OnDBPCGameKindItem(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//游戏节点
	bool OnDBPCGameNodeItem(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//加载结果
	bool OnDBPCGameListResult(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//加载跑马灯消息 返回
	bool On_DBO_UPDATA_MARQUEE(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//加载跑马灯消息 结束
	bool On_DBO_UPDATA_MARQUEE_FINISH(DWORD dwContextID, VOID * pData, WORD wDataSize);
#pragma endregion

#pragma region 协调服 主消息号分发处理
	//连接处理
protected:
	//注册模块
	bool OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//列表命令
	bool OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//中转服务
	bool OnTCPSocketMainTransfer(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//后台服务
	bool OnTCPSocketMainWeb(WORD wSubCmdID, VOID * pData, WORD wDataSize);
#pragma endregion

#pragma region 客户端 主消息号分发处理
	//网络消息事件（客户端）
protected:
	//登录模块
	bool OnTCPNetworkMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//用户服务
	bool OnTCPNetworkMainService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//礼物道具
	bool OnTCPNetworkMainOther(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//商城道具
	bool On_MDM_SHOP(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//牌友圈
	bool On_MDM_CLUB(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//游戏服相关
	bool On_MDM_GAME(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
#pragma endregion

#pragma region MAIN_LOGON 登录模块
	//【网络事件处理函数】
	/* 主消息号1 MAIN_LOGON 登录模块 */
protected:
	//帐号登录
	bool On_SUB_CL_Logon_Accounts(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//账号登录返回
	bool On_CMD_LC_Logon_Account(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//帐号注册
	bool On_SUB_CL_Logon_Register(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//账号注册成功 返回的是账号登录成功 -- 客户端直接进入大厅

	//平台登录
	bool On_SUB_CL_Logon_Platform(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//平台登录返回
	bool On_CMD_LC_Logon_Platform(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//重复登录踢出
	bool On_CMD_LC_Logon_RepeatLogon(DWORD UserID, DWORD dwContextID);

	//升级提示（版本检测）
	bool On_CMD_LC_Logon_UpdateNotify(DWORD dwVersionCheck, DWORD dwSocketID);

	//登录奖励
	bool On_CMD_LC_Logon_Logon_Reward(DWORD dwContextID, SYSTEMTIME LasLogonDate);
#pragma endregion

#pragma region MAIN_LIST 列表命令
	/* 主消息号2 MAIN_LIST 列表信息 */
protected:
	//发送类型
	VOID On_CMD_LC_List_Type(DWORD dwSocketID);
	//发送种类
	VOID On_CMD_LC_List_Kind(DWORD dwSocketID);
	//发送节点
	VOID On_CMD_LC_List_Node(DWORD dwSocketID);
	//发送房间
	VOID On_CMD_LC_List_Room(DWORD dwSocketID);

#pragma endregion

#pragma region MAIN_SERVICE
	/* 主消息号3 MAIN_SERVICE 服务模块 */
protected:
	//玩家反馈
	bool On_SUB_CL_Service_UserFeedBack(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//玩家反馈返回
	bool On_CMD_LC_Service_UserFeedBack(DWORD dwContextID, VOID * pData);

	//刷新用户信息
	bool On_SUB_CL_Service_RefreshUserInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//刷新用户信息返回
	bool On_CMD_LC_Service_RefreshUserInfo( DWORD dwContextID, VOID * pData);

	//查询开房信息列表
	bool On_SUB_CL_Service_QueryRoomList(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//查询开房信息列表返回
	bool On_CMD_LC_Service_QueryRoomList(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//查询已开房（结束）信息列表返回
	bool On_CMD_LC_Service_QueryOpenedRoomList(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//获取富豪榜
	bool On_SUB_CL_Service_GetRichList(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//获取富豪榜返回
	bool On_CMD_LC_Service_GetRichList( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//获取用户录像列表
	bool On_SUB_CL_Service_GetUserRecordList(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//获取用户录像列表返回
	bool On_CMD_LC_Service_GetUserRecordList(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//获取指定ID录像
	bool On_SUB_CL_Service_GetSpecificRecord(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//获取指定ID录像返回
	bool On_CMD_LC_Service_GetSpecificRecord(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//获取在线奖励
	bool On_SUB_CL_Service_OnlineReward(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//获取在线奖励返回
	bool On_CMD_LC_Service_OnlineReward( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//获取任务列表
	bool On_SUB_CL_Service_GetTaskList(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//获取任务列表返回
	bool On_CMD_LC_Service_GetTaskList( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//领取任务奖励
	bool On_SUB_CL_Service_GetTaskReward(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//领取任务奖励返回
	bool On_CMD_LC_Service_GetTaskReward( DWORD dwContextID, VOID * pData, WORD wDataSize );
	
	//获取排行榜，返回也在这里面
	bool On_SUB_CL_Service_GetRankList(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//抽奖请求
	bool On_SUB_CL_Service_RequestLottery(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//抽奖请求返回
	bool On_CMD_LC_Service_RequestLottery( DWORD dwContextID, VOID * pData, WORD wDataSize );

	// pure大厅排行版
	bool On_SUB_CL_SERVICE_PURE_STANDING_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//pure大厅排行版 返回
	bool On_CMD_LC_SERVICE_PURE_STANDING_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//pure大厅排行版 结束
	bool On_CMD_LC_SERVICE_PURE_STANDING_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize );

	// pure大局战绩
	bool On_SUB_CL_SERVICE_PURE_RECORD_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//pure大局战绩 返回
	bool On_CMD_LC_SERVICE_PURE_RECORD_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//pure大局玩家信息 返回
	bool On_CMD_LC_SERVICE_PURE_RECORD_LIST_PINFO( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//pure大局战绩 结束
	bool On_CMD_LC_SERVICE_PURE_RECORD_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize );

	// pure小局战绩
	bool On_SUB_CL_SERVICE_PURE_XJ_RECORD_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//pure小局战绩 返回
	bool On_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//pure小局战绩 返回
	bool On_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_PINFO( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//pure小局战绩 结束
	bool On_CMD_LC_SERVICE_PURE_XJ_RECORD_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//小局录像回放
	bool On_SUB_CL_Service_XJRecordPlayback(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//小局录像回放 返回
	bool On_CMD_LC_Service_XJRecordPlayback( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//客服消息
	bool On_SUB_CL_SERVICE_CUSTOMER_MESSEGE(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//客服消息 返回
	bool On_CMD_LC_SERVICE_CUSTOMER_MESSEGE( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//请求金币大厅信息
	bool On_SUB_CL_SERVICE_GOLD_INFO(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//请求金币大厅信息 返回
	bool On_CMD_LC_SERVICE_GOLD_INFO( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//请求金币大厅信息 结束
	bool On_CMD_LC_SERVICE_GOLD_INFO_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//修改个人资料
	bool On_SUB_CL_Service_ModifyPersonalInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//修改个人资料返回
	bool On_CMD_LC_Service_ModifyPersonalInfo( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//查询（变更）金币房卡钻石返回（涉及到用户金币等改变的，都会返回这个消息，让客户端刷新用户金币信息）
	bool On_CMD_LC_Service_QueryScoreInfo(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//公共操作结果返回	
	bool On_CMD_LC_CommonOperateResult( DWORD dwContextID, VOID * pData, WORD wDataSize );
#pragma endregion

#pragma region MDM_CLUB 牌友圈
	protected:
	//查询牌友圈列表
	bool On_SUB_CL_CLUB_ALL_CLUB_INFO_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//查询牌友圈列表结果
	bool On_CMD_LC_CLUB_ALL_CLUB_INFO_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查询牌友圈列表结束
	bool On_CMD_LC_CLUB_ALL_INFO_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize);

	//查询指定牌友圈房间列表
	bool On_SUB_CL_CLUB_ROOM_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//查询指定牌友圈房间列表 返回
	bool On_CMD_LC_CLUB_ROOM_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//查询指定牌友圈房间列表 结束
	bool On_CMD_LC_CLUB_ROOM_LIST_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//查询未满员, 随机牌友圈(最大9个)
	bool On_SUB_CL_CLUB_RANDOM_CLUB_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//查询未满员, 随机牌友圈(最大9个) 返回
	bool On_CMD_LC_CLUB_RANDOM_CLUB_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//查询未满员, 随机牌友圈(最大9个) 结束
	bool On_CMD_LC_CLUB_RANDOM_CLUB_LIST_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize );
	
	//申请加入牌友圈
	bool On_SUB_CL_CLUB_JOIN_CLUB(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//申请加入牌友圈返回
	bool On_CMD_LC_CLUB_JOIN_CLUB(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//申请加入牌友圈广播
	bool On_CMD_LC_CLUB_JOIN_CLUB_BDCAST(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//申请加入牌友圈 通知客户端实时刷新俱乐部
	bool On_CMD_LC_CLUB_JOIN_CLUB_RE(DWORD dwContextID, VOID * pData, WORD wDataSize);
	
	//解散牌友圈
	bool On_SUB_CL_CLUB_DISS_CLUB(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//解散牌友圈 返回
	bool On_CMD_LC_CLUB_DISS_CLUB(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//创建牌友圈
	bool On_SUB_CL_CLUB_CREATE_CLUB(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//创建牌友圈 返回
	bool On_CMD_LC_CLUB_CREATE_CLUB( DWORD dwContextID, VOID * pData, WORD wDataSize);

	//房间设置
	bool On_SUB_CL_CLUB_ROOM_SETTING(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//房间设置 返回
	bool On_CMD_LC_CLUB_ROOM_SETTING( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//请求房间设置
	bool On_SUB_CL_CLUB_ROOM_QUERY_SETTING(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//房间设置 返回
	bool On_CMD_LC_CLUB_ROOM_QUERY_SETTING( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//玩家离开俱乐部房间
	bool On_SUB_CL_CLUB_ROOM_USER_LEAVE(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//解散房间请求
	bool On_SUB_CL_CLUB_ROOM_DISSOLVE(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//解散房间请求 返回
	bool On_CMD_LC_CLUB_ROOM_DISSOLVE( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//解散桌子请求
	bool On_SUB_CL_CLUB_TABLE_DISSOLVE(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//解散桌子请求 返回
	bool On_CMD_LC_CLUB_TABLE_DISSOLVE( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//申请加入房间
	bool On_SUB_CL_CLUB_JOIN_ROOM(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//申请加入房间 返回
	bool On_CMD_LC_CLUB_JOIN_ROOM( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//桌子列表
	bool On_CMD_LC_CLUB_TABLE_LIST_TABLE(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//桌子玩家列表
	bool On_CMD_LC_CLUB_TABLE_LIST_USER(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//牌友圈公告
	bool On_SUB_CL_CLUB_NOTICE(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//牌友圈公告 返回
	bool On_CMD_LC_CLUB_NOTICE( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//牌友圈公告 广播
	bool On_CMD_LC_CLUB_NOTICE_BDCAST(DWORD dwContextID, VOID * pData, WORD wDataSize);
	
	//牌友圈简介
	bool On_SUB_CL_CLUB_MESSAGE(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//牌友圈简介 返回
	bool On_CMD_LC_CLUB_MESSAGE( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//贡献房卡
	bool On_SUB_CL_CLUB_CONTRIBUTE_FK(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//贡献房卡 返回
	bool On_CMD_LC_CLUB_CONTRIBUTE_FK( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//牌友圈设置
	bool On_SUB_CL_CLUB_AUTO_AGREE(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//牌友圈设置 返回
	bool On_CMD_LC_CLUB_AUTO_AGREE( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//牌友圈聊天
	bool On_SUB_CL_CLUB_CHAT(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//牌友圈聊天 返回
	bool On_CMD_LC_CLUB_CHAT( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//牌友圈聊天 广播
	bool On_CMD_LC_CLUB_CHAT_BDCAST(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//牌友圈置顶
	bool On_SUB_CL_CLUBSTICKY_POST(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//牌友圈置顶 返回
	bool On_CMD_LC_CLUB_STICKY_POST( DWORD dwContextID, VOID * pData, WORD wDataSize );
	
	//群主|管理对申请消息的答复(同意|拒绝)
	bool On_SUB_CL_CLUB_APPLICANT_RESULT(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//群主|管理对申请消息的答复(同意|拒绝) 返回
	bool On_CMD_LC_CLUB_APPLICANT_RESULT( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//请求成员数据
	bool On_SUB_CL_CLUB_MEMBER_MANAGER(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//请求成员数据 返回
	bool On_CMD_LC_CLUB_MEMBER_MANAGER( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//请求成员数据 结束
	bool On_CMD_LC_CLUB_MEMBER_MANAGER_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//工会基本信息
	bool On_CMD_LC_CLUB_DATA( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//邀请他入加入牌友圈
	bool On_SUB_CL_CLUB_INVITE(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//邀请他入加入牌友圈 返回
	bool On_CMD_LC_CLUB_INVITE( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//被邀请人的提醒
	bool On_CMD_LC_CLUB_INVITE_REMIND( DWORD dwContextID, VOID * pData, WORD wDataSize );
	
	//被邀请人的回复
	bool On_SUB_CL_CLUB_INVITE_RESULT(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//被邀请人的回复 返回
	bool On_CMD_LC_CLUB_INVITE_RESULT( DWORD dwContextID, VOID * pData, WORD wDataSize );
	
	//踢出退出请求
	bool On_SUB_CL_CLUB_QUIT(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//踢出退出请求 返回
	bool On_CMD_LC_CLUB_QUIT( DWORD dwContextID, VOID * pData, WORD wDataSize );
	
	//职务任免
	bool On_SUB_CL_CLUB_APPOINTMENT(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//职务任免 返回
	bool On_CMD_LC_CLUB_APPOINTMENT( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//职务任免 提醒
	bool On_CMD_LC_CLUB_APPOINTMENT_NOTE( DWORD dwContextID, VOID * pData, WORD wDataSize );
	
	//被邀请人查看自己的邀请列表
	bool On_SUB_CL_CLUB_INQUERY_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//被邀请人查看自己的邀请列表 返回
	bool On_CMD_LC_CLUB_INQUERY_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//被邀请人查看自己的邀请列表 结束
	bool On_CMD_LC_CLUB_INQUERY_LIST_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//申请人列表
	bool On_SUB_CL_CLUB_APPLICANT_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//申请人列表 返回
	bool On_CMD_LC_CLUB_APPLICANT_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//申请人列表 结束
	bool On_CMD_LC_CLUB_APPLICANT_LIST_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//工会战绩统计
	bool On_SUB_CL_CLUB_RECORD_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//工会战绩统计 返回
	bool On_CMD_LC_CLUB_RECORD_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//工会战绩统计 结束
	bool On_CMD_LC_CLUB_RECORD_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize );

#pragma endregion

#pragma region MDM_GIFT 礼物道具
protected:
	//赠送道具
	bool On_SUB_CL_GIFT_GIVE_PROPS(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//赠送道具返回
	bool On_CMD_LC_GIFT_GIVE_PROPS( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//赠送道具返回
	bool On_CMD_LC_GIFT_GIVE_PROPS_SHOW( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//用户充值信息
	bool On_SUB_CL_Other_ReChargeInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//用户充值信息返回
	bool On_CMD_LC_Other_RechargeInfo( DWORD dwContextID, VOID * pData, WORD wDataSize);

	//兑换道具
	bool On_SUB_CL_Other_ExchangeInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//兑换道具返回
	bool On_CMD_LC_Other_ExchangeInfo( DWORD dwContextID, VOID * pData, WORD wDataSize);

	//TODO 排行榜领取奖励暂时放在这里
	//领取排行榜奖励返回
	bool OnDBRankRewardResult( DWORD dwContextID, VOID * pData, WORD wDataSize );
#pragma endregion

#pragma region MDM_SHOP 道具商城
	protected:
	//查询商城
	bool On_SUB_CL_SHOP_QUERY(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//查询商城返回
	bool On_CMD_LC_SHOP_QUERY_RESULT( DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查询商城结束
	bool On_CMD_LC_SHOP_QUERY_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize);

	//金钱购买道具
	bool On_SUB_CL_SHOP_MONEY(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//金钱购买道具返回
	bool On_CMD_LC_SHOP_MONEY_RESULT( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//钻石购买道具
	bool On_SUB_CL_SHOP_DIAMOND(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//钻石购买道具返回
	bool On_CMD_LC_SHOP_DIAMOND_RESULT( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//背包物品查询
	bool On_SUB_CL_BAG_QUERY(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//背包物品查询结果CMD
	bool On_CMD_LC_BAG_RESULT( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//背包物品查询完成
	bool On_CMD_LC_BAG_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize );
#pragma endregion

};


extern CAttemperEngineSink* g_AttemperEngineSink;

//////////////////////////////////////////////////////////////////////////////////

#endif