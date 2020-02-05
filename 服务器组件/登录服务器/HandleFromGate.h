#ifndef HANDLE_FROM_CLIENT_HEAD_FILE_L
#define HANDLE_FROM_CLIENT_HEAD_FILE_L

#include "Stdafx.h"

class CHandleFromGate
{
	//总分发函数
public:
	static bool HandlePacket(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	static bool HandlePacketDB(WORD wRequestID, DWORD dwScoketID, VOID * pData, WORD wDataSize);

	//一级分发函数
private:
	//登录模块
	static bool OnTCPNetworkMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//用户服务
	static bool OnTCPNetworkMainService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//商城道具
	static bool On_MDM_SHOP(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//牌友圈
	static bool On_MDM_CLUB(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//游戏服相关
	static bool On_MDM_GAME(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//具体处理
private:
#pragma region MAIN_LOGON 登录模块
	//帐号登录
	static bool On_SUB_CL_Logon_Accounts(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//账号登录返回
	static bool On_CMD_LC_Logon_Account(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//帐号注册
	static bool On_SUB_CL_Logon_Register(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//账号注册成功 返回的是账号登录成功 -- 客户端直接进入大厅

	//平台登录
	static bool On_SUB_CL_Logon_Platform(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//平台登录返回
	static bool On_CMD_LC_Logon_Platform(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//重复登录踢出
	static bool On_CMD_LC_Logon_RepeatLogon(DWORD UserID, DWORD dwContextID);

	//登录奖励
	static bool On_CMD_LC_Logon_Logon_Reward(DWORD dwContextID, SYSTEMTIME LasLogonDate);
#pragma endregion

#pragma region MAIN_SERVICE
	//粉丝关注
	static bool On_SUB_CL_SERVICE_FLOWER(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//查看关注
	static bool On_SUB_CL_SERVICE_FLOWER_QUERY(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//查看粉丝
	static bool On_SUB_CL_SERVICE_FLOWING_QUERY(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//刷新用户信息
	static bool On_SUB_CL_Service_RefreshUserInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//刷新用户信息返回
	static bool On_CMD_LC_Service_RefreshUserInfo( DWORD dwContextID, VOID * pData);

	//获取用户录像列表
	static bool On_SUB_CL_Service_GetUserRecordList(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//获取用户录像列表返回
	static bool On_CMD_LC_Service_GetUserRecordList(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//获取指定ID录像
	static bool On_SUB_CL_Service_GetSpecificRecord(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//获取指定ID录像返回
	static bool On_CMD_LC_Service_GetSpecificRecord(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//获取在线奖励
	static bool On_SUB_CL_Service_OnlineReward(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//获取在线奖励返回
	static bool On_CMD_LC_Service_OnlineReward( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//获取任务列表
	static bool On_SUB_CL_Service_GetTaskList(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//获取任务列表返回
	static bool On_CMD_LC_Service_GetTaskList( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//领取任务奖励
	static bool On_SUB_CL_Service_GetTaskReward(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//领取任务奖励返回
	static bool On_CMD_LC_Service_GetTaskReward( DWORD dwContextID, VOID * pData, WORD wDataSize );
	
	//获取排行榜，返回也在这里面
	static bool On_SUB_CL_Service_GetRankList(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//抽奖请求
	static bool On_SUB_CL_Service_RequestLottery(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//抽奖请求返回
	static bool On_CMD_LC_Service_RequestLottery( DWORD dwContextID, VOID * pData, WORD wDataSize );

	// pure大厅排行版
	static bool On_SUB_CL_SERVICE_PURE_STANDING_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//pure大厅排行版 返回
	static bool On_CMD_LC_SERVICE_PURE_STANDING_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//pure大厅排行版 结束
	static bool On_CMD_LC_SERVICE_PURE_STANDING_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize );

	// pure大局战绩
	static bool On_SUB_CL_SERVICE_PURE_RECORD_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//pure大局战绩 返回
	static bool On_CMD_LC_SERVICE_PURE_RECORD_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//pure大局玩家信息 返回
	static bool On_CMD_LC_SERVICE_PURE_RECORD_LIST_PINFO( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//pure大局战绩 结束
	static bool On_CMD_LC_SERVICE_PURE_RECORD_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize );

	// pure小局战绩
	static bool On_SUB_CL_SERVICE_PURE_XJ_RECORD_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//pure小局战绩 返回
	static bool On_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//pure小局战绩 返回
	static bool On_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_PINFO( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//pure小局战绩 结束
	static bool On_CMD_LC_SERVICE_PURE_XJ_RECORD_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//小局录像回放
	static bool On_SUB_CL_Service_XJRecordPlayback(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//小局录像回放 返回
	static bool On_CMD_LC_Service_XJRecordPlayback( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//请求比赛场信息
	static bool On_SUB_CL_SERVICE_MATCH_INFO(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//请求比赛场信息 返回
	static bool On_CMD_LC_SERVICE_MATCH_INFO(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//请求比赛场信息 结束
	static bool On_CMD_LC_SERVICE_MATCH_INFO_FINISH(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//修改个人资料
	static bool On_SUB_CL_Service_ModifyPersonalInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//修改个人资料返回
	static bool On_CMD_LC_Service_ModifyPersonalInfo( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//公共操作结果返回	
	static bool On_CMD_LC_CommonOperateResult( DWORD dwContextID, VOID * pData, WORD wDataSize );
#pragma endregion

#pragma region MDM_CLUB 牌友圈
	//查询牌友圈列表
	static bool On_SUB_CL_CLUB_ALL_CLUB_INFO_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//查询牌友圈列表结果
	static bool On_CMD_LC_CLUB_ALL_CLUB_INFO_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize);
	//查询牌友圈列表结束
	static bool On_CMD_LC_CLUB_ALL_INFO_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize);

	//查询指定牌友圈房间列表
	static bool On_SUB_CL_CLUB_ROOM_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//查询指定牌友圈房间列表 返回
	static bool On_CMD_LC_CLUB_ROOM_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//查询指定牌友圈房间列表 结束
	static bool On_CMD_LC_CLUB_ROOM_LIST_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//查询未满员, 随机牌友圈(最大9个)
	static bool On_SUB_CL_CLUB_RANDOM_CLUB_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//查询未满员, 随机牌友圈(最大9个) 返回
	static bool On_CMD_LC_CLUB_RANDOM_CLUB_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//查询未满员, 随机牌友圈(最大9个) 结束
	static bool On_CMD_LC_CLUB_RANDOM_CLUB_LIST_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize );
	
	//申请加入牌友圈
	static bool On_SUB_CL_CLUB_JOIN_CLUB(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//申请加入牌友圈返回
	static bool On_CMD_LC_CLUB_JOIN_CLUB(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//申请加入牌友圈广播
	static bool On_CMD_LC_CLUB_JOIN_CLUB_BDCAST(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//申请加入牌友圈 通知客户端实时刷新俱乐部
	static bool On_CMD_LC_CLUB_JOIN_CLUB_RE(DWORD dwContextID, VOID * pData, WORD wDataSize);
	
	//解散牌友圈
	static bool On_SUB_CL_CLUB_DISS_CLUB(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//解散牌友圈 返回
	static bool On_CMD_LC_CLUB_DISS_CLUB(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//创建牌友圈
	static bool On_SUB_CL_CLUB_CREATE_CLUB(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//创建牌友圈 返回
	static bool On_CMD_LC_CLUB_CREATE_CLUB( DWORD dwContextID, VOID * pData, WORD wDataSize);

	//房间设置
	static bool On_SUB_CL_CLUB_ROOM_SETTING(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//房间设置 返回
	static bool On_CMD_LC_CLUB_ROOM_SETTING( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//请求房间设置
	static bool On_SUB_CL_CLUB_ROOM_QUERY_SETTING(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//房间设置 返回
	static bool On_CMD_LC_CLUB_ROOM_QUERY_SETTING( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//玩家离开俱乐部房间
	static bool On_SUB_CL_CLUB_ROOM_USER_LEAVE(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//解散房间请求
	static bool On_SUB_CL_CLUB_ROOM_DISSOLVE(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//解散房间请求 返回
	static bool On_CMD_LC_CLUB_ROOM_DISSOLVE( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//解散桌子请求
	static bool On_SUB_CL_CLUB_TABLE_DISSOLVE(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//解散桌子请求 返回
	static bool On_CMD_LC_CLUB_TABLE_DISSOLVE( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//申请加入房间
	static bool On_SUB_CL_CLUB_JOIN_ROOM(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//申请加入房间 返回
	static bool On_CMD_LC_CLUB_JOIN_ROOM( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//桌子列表
	static bool On_CMD_LC_CLUB_TABLE_LIST_TABLE(DWORD dwContextID, VOID * pData, WORD wDataSize);
	//桌子玩家列表
	static bool On_CMD_LC_CLUB_TABLE_LIST_USER(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//牌友圈公告
	static bool On_SUB_CL_CLUB_NOTICE(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//牌友圈公告 返回
	static bool On_CMD_LC_CLUB_NOTICE( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//牌友圈公告 广播
	static bool On_CMD_LC_CLUB_NOTICE_BDCAST(DWORD dwContextID, VOID * pData, WORD wDataSize);
	
	//牌友圈简介
	static bool On_SUB_CL_CLUB_MESSAGE(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//牌友圈简介 返回
	static bool On_CMD_LC_CLUB_MESSAGE( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//贡献房卡
	static bool On_SUB_CL_CLUB_CONTRIBUTE_FK(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//贡献房卡 返回
	static bool On_CMD_LC_CLUB_CONTRIBUTE_FK( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//牌友圈设置
	static bool On_SUB_CL_CLUB_AUTO_AGREE(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//牌友圈设置 返回
	static bool On_CMD_LC_CLUB_AUTO_AGREE( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//牌友圈聊天
	static bool On_SUB_CL_CLUB_CHAT(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//牌友圈聊天 返回
	static bool On_CMD_LC_CLUB_CHAT( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//牌友圈聊天 广播
	static bool On_CMD_LC_CLUB_CHAT_BDCAST(DWORD dwContextID, VOID * pData, WORD wDataSize);

	//牌友圈置顶
	static bool On_SUB_CL_CLUBSTICKY_POST(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//牌友圈置顶 返回
	static bool On_CMD_LC_CLUB_STICKY_POST( DWORD dwContextID, VOID * pData, WORD wDataSize );
	
	//群主|管理对申请消息的答复(同意|拒绝)
	static bool On_SUB_CL_CLUB_APPLICANT_RESULT(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//群主|管理对申请消息的答复(同意|拒绝) 返回
	static bool On_CMD_LC_CLUB_APPLICANT_RESULT( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//请求成员数据
	static bool On_SUB_CL_CLUB_MEMBER_MANAGER(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//请求成员数据 返回
	static bool On_CMD_LC_CLUB_MEMBER_MANAGER( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//请求成员数据 结束
	static bool On_CMD_LC_CLUB_MEMBER_MANAGER_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//工会基本信息
	static bool On_CMD_LC_CLUB_DATA( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//邀请他入加入牌友圈
	static bool On_SUB_CL_CLUB_INVITE(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//邀请他入加入牌友圈 返回
	static bool On_CMD_LC_CLUB_INVITE( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//被邀请人的提醒
	static bool On_CMD_LC_CLUB_INVITE_REMIND( DWORD dwContextID, VOID * pData, WORD wDataSize );
	
	//被邀请人的回复
	static bool On_SUB_CL_CLUB_INVITE_RESULT(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//被邀请人的回复 返回
	static bool On_CMD_LC_CLUB_INVITE_RESULT( DWORD dwContextID, VOID * pData, WORD wDataSize );
	
	//踢出退出请求
	static bool On_SUB_CL_CLUB_QUIT(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//踢出退出请求 返回
	static bool On_CMD_LC_CLUB_QUIT( DWORD dwContextID, VOID * pData, WORD wDataSize );
	
	//职务任免
	static bool On_SUB_CL_CLUB_APPOINTMENT(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//职务任免 返回
	static bool On_CMD_LC_CLUB_APPOINTMENT( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//职务任免 提醒
	static bool On_CMD_LC_CLUB_APPOINTMENT_NOTE( DWORD dwContextID, VOID * pData, WORD wDataSize );
	
	//被邀请人查看自己的邀请列表
	static bool On_SUB_CL_CLUB_INQUERY_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//被邀请人查看自己的邀请列表 返回
	static bool On_CMD_LC_CLUB_INQUERY_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//被邀请人查看自己的邀请列表 结束
	static bool On_CMD_LC_CLUB_INQUERY_LIST_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//申请人列表
	static bool On_SUB_CL_CLUB_APPLICANT_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//申请人列表 返回
	static bool On_CMD_LC_CLUB_APPLICANT_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//申请人列表 结束
	static bool On_CMD_LC_CLUB_APPLICANT_LIST_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//工会战绩统计
	static bool On_SUB_CL_CLUB_RECORD_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//工会战绩统计 返回
	static bool On_CMD_LC_CLUB_RECORD_LIST( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//工会战绩统计 结束
	static bool On_CMD_LC_CLUB_RECORD_FINISH( DWORD dwContextID, VOID * pData, WORD wDataSize );

#pragma endregion

#pragma region MDM_GIFT 礼物道具
	//赠送道具
	static bool On_SUB_CL_GIFT_GIVE_PROPS(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//赠送道具返回
	static bool On_CMD_LC_GIFT_GIVE_PROPS( DWORD dwContextID, VOID * pData, WORD wDataSize );
	//赠送道具返回
	static bool On_CMD_LC_GIFT_GIVE_PROPS_SHOW( DWORD dwContextID, VOID * pData, WORD wDataSize );

	//用户充值信息
	static bool On_SUB_CL_Other_ReChargeInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//用户充值信息返回
	static bool On_CMD_LC_Other_RechargeInfo( DWORD dwContextID, VOID * pData, WORD wDataSize);

	//兑换道具
	static bool On_SUB_CL_Other_ExchangeInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//兑换道具返回
	static bool On_CMD_LC_Other_ExchangeInfo( DWORD dwContextID, VOID * pData, WORD wDataSize);

	//TODO 排行榜领取奖励暂时放在这里
	//领取排行榜奖励返回
	static bool OnDBRankRewardResult( DWORD dwContextID, VOID * pData, WORD wDataSize );
#pragma endregion

#pragma region MDM_SHOP 道具商城
	//商城购买
	static bool On_SUB_CL_SHOP_BUY(VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//背包物品查询
	static bool On_SUB_CL_BAG_QUERY(VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//背包物品查询
	static bool On_DBO_CL_BAG_RESULT( DWORD dwContextID, VOID * pData, WORD wDataSize );
	
#pragma endregion

};

#endif