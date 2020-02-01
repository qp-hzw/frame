#include "HandleFromGate.h"
#include "GameCtrl.h"
#include "PlayerManager.h"
#include "Player.h"
#include "RankManager.h"

//消息分发处理函数
bool CHandleFromGate::HandlePacket(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
		switch (Command.wMainCmdID)
	{
	case MDM_LOGON:				//登录命令
		{
			return OnTCPNetworkMainLogon(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_SERVICE:			//服务命令
		{
			return OnTCPNetworkMainService(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_CLUB:				//牌友圈(版本2)
		{
			return On_MDM_CLUB(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GIFT:				//礼物道具	
		{
			return OnTCPNetworkMainOther(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_SHOP:				//商城道具
		{
			return On_MDM_SHOP(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GAME:				//游戏服相关
		{
			return On_MDM_GAME(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}

	}

	return false;
}

bool CHandleFromGate::HandlePacketDB(WORD wRequestID, DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
#pragma region 登录模块
	case DBO_CL_LOGON_ACCOUNTS:			//账号登录
		{
			return On_CMD_LC_Logon_Account(dwScoketID,pData,wDataSize);
		}
	case DBO_CL_LOGON_PLATFORM:			//第三方登陆
		{
			return On_CMD_LC_Logon_Platform(dwScoketID, pData, wDataSize);
		}
#pragma endregion

	case DBO_CL_SERVICE_USER_FEEDBACK:			//玩家反馈返回
		{
			return On_CMD_LC_Service_UserFeedBack(dwScoketID, pData);
		}
	case DBO_CL_SERVICE_REFRESH_USER_INFO:		//刷新用户信息返回
		{
			return On_CMD_LC_Service_RefreshUserInfo(dwScoketID, pData);
		}
	case DBO_CL_SERVICE_QUERY_ROOM_LIST:		//查询开房列表返回
		{
			return On_CMD_LC_Service_QueryRoomList(dwScoketID,pData,wDataSize);
		}
	case DBO_CL_SERVICE_GET_RICH_LIST:			//获取富豪榜返回
		{
			return On_CMD_LC_Service_GetRichList(dwScoketID,pData,wDataSize);
		}
	case DBO_CL_SERVICE_GET_USER_RECORD_LIST:	//获取用户录像列表返回（大局）
		{
			return On_CMD_LC_Service_GetUserRecordList(dwScoketID,pData,wDataSize);
		}
	case DBO_CL_SERVICE_GET_SPECIFIC_RECORD:	//获取指定ID录像返回（小局）
		{
			return On_CMD_LC_Service_GetSpecificRecord(dwScoketID,pData,wDataSize);
		}
	case DBO_CL_SERVICE_ONLINE_REWARD:			//获取在线奖励返回
		{
			return On_CMD_LC_Service_OnlineReward(dwScoketID,pData,wDataSize);
		}
	case DBO_CL_SERVICE_GET_TASK_LIST:			//获取任务列表返回
		{
			return On_CMD_LC_Service_GetTaskList(dwScoketID,pData,wDataSize);
		}
	case DBO_CL_SERVICE_GET_TASK_REWARD:		//领取任务奖励返回
		{
			return On_CMD_LC_Service_GetTaskReward(dwScoketID,pData,wDataSize);
		}
	case DBO_CL_SERVICE_REQUEST_LOTTERY:		//请求抽奖返回
		{
			return On_CMD_LC_Service_RequestLottery(dwScoketID,pData,wDataSize);
		}
	case DBO_CL_SERVICE_MODIFY_PERSONAL_INFO:	//修改个人资料返回
		{
			return On_CMD_LC_Service_ModifyPersonalInfo(dwScoketID,pData,wDataSize);
		}
	case DBO_CL_USER_QUERY_SCORE_INFO:			//查询(变更)金币房卡
		{
			return On_CMD_LC_Service_QueryScoreInfo(dwScoketID,pData,wDataSize);
		}
	
	case DBO_CL_USER_COMMAND_RESULT:		//公共操作结果
		{
			return On_CMD_LC_CommonOperateResult(dwScoketID,pData,wDataSize);
		}
#pragma region MDM_GIFT 礼物道具
	case DBO_CL_GIFT_GIVE_PROPS:			//道具赠送 返回
		{
			return On_CMD_LC_GIFT_GIVE_PROPS(dwScoketID,pData,wDataSize);
		}
	case DBO_CL_GIFT_GIVE_PROPS_SHOW:		//道具赠送 通知接收人
		{
			return On_CMD_LC_GIFT_GIVE_PROPS_SHOW(dwScoketID,pData,wDataSize);
		}
#pragma endregion
	case DBO_CL_USER_RECHARGE_INFO:			//充值信息返回
		{
			return On_CMD_LC_Other_RechargeInfo(dwScoketID,pData,wDataSize);
		}
	case DBO_CL_USER_EXCHANGE_INFO:			//兑换道具返回
		{
			return On_CMD_LC_Other_ExchangeInfo(dwScoketID,pData,wDataSize);
		}
	case DBO_CL_SERVICE_GET_RANK_REWARD:		//获得排行榜奖励	
		{
			return OnDBRankRewardResult(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_PURE_STANDING_LIST:		//pure大厅排行榜 返回
		{
			return On_CMD_LC_SERVICE_PURE_STANDING_LIST(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_PURE_STANDING_FINISH:	//pure大厅排行榜 结束
		{
			return On_CMD_LC_SERVICE_PURE_STANDING_FINISH(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_PURE_RECORD_LIST:		//大局战绩
		{
			return On_CMD_LC_SERVICE_PURE_RECORD_LIST(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_PURE_RECORD_LIST_PINFO:	//大局玩家信息
		{
			return On_CMD_LC_SERVICE_PURE_RECORD_LIST_PINFO(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_PURE_RECORD_FINISH://大局战绩结束
		{
			return On_CMD_LC_SERVICE_PURE_RECORD_FINISH(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_PURE_XJ_RECORD_LIST:		//小局战绩
		{
			return On_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_PURE_XJ_RECORD_LIST_PINFO:	//小局玩家信息
		{
			return On_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_PINFO(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_XJ_RECORD_PLAYBACK:			//小局录像回放
		{
			return On_CMD_LC_Service_XJRecordPlayback(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_CUSTOMER_MESSEGE:		//客服消息
		{
			return On_CMD_LC_SERVICE_CUSTOMER_MESSEGE(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_PURE_XJ_RECORD_FINISH://小局战绩结束
		{
			return On_CMD_LC_SERVICE_PURE_XJ_RECORD_FINISH(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_GOLD_INFO:		//请求金币大厅 返回
		{
			return On_CMD_LC_SERVICE_GOLD_INFO(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_SERVICE_GOLD_INFO_FINISH:	//请求金币大厅 结束
		{
			return On_CMD_LC_SERVICE_GOLD_INFO_FINISH(dwScoketID,pData,wDataSize);
		}
			 
#pragma region MDM_CLUB 牌友圈
	case DBO_LC_CLUB_ALL_CLUB_INFO_LIST:		//查询牌友圈列表 返回
		{
			return On_CMD_LC_CLUB_ALL_CLUB_INFO_LIST(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_ALL_INFO_FINISH:		//查询牌友圈列表 结束
		{
			return On_CMD_LC_CLUB_ALL_INFO_FINISH(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_ROOM_LIST: //查询指定牌友圈房间列表 返回
		{
			return On_CMD_LC_CLUB_ROOM_LIST(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_ROOM_LIST_FINISH: //查询指定牌友圈房间列表 结束
		{
			return On_CMD_LC_CLUB_ROOM_LIST_FINISH(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_RANDOM_CLUB_LIST : //查询未满员随机牌友圈 返回
		{
			return On_CMD_LC_CLUB_RANDOM_CLUB_LIST(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_RANDOM_CLUB_LIST_FINISH : //查询未满员随机牌友圈  结束
		{
			return On_CMD_LC_CLUB_RANDOM_CLUB_LIST_FINISH(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_JOIN_CLUB: //申请加入牌友圈
		{
			return On_CMD_LC_CLUB_JOIN_CLUB(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_JOIN_CLUB_BDCAST: //申请加入牌友圈 广播
		{
			return On_CMD_LC_CLUB_JOIN_CLUB_BDCAST(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_JOIN_CLUB_RE:	//申请加入牌友圈 客户端实时刷新俱乐部
		{
			return On_CMD_LC_CLUB_JOIN_CLUB_RE(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_DISS_CLUB : //解散牌友圈
		{
			return On_CMD_LC_CLUB_DISS_CLUB(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_ROOM_SETTING: //房间设置
		{
			return On_CMD_LC_CLUB_ROOM_SETTING(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_ROOM_QUERY_SETTING: //请求房间设置
		{
			return On_CMD_LC_CLUB_ROOM_QUERY_SETTING(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_ROOM_DISSOLVE: //解散房间请求 返回
		{
			return On_CMD_LC_CLUB_ROOM_DISSOLVE(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_TABLE_DISSOLVE: //解散桌子请求 返回
		{
			return On_CMD_LC_CLUB_TABLE_DISSOLVE(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_NOTICE: //牌友圈公告
		{
			return On_CMD_LC_CLUB_NOTICE(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_MESSAGE: //牌友圈简介
		{
			return On_CMD_LC_CLUB_MESSAGE(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_CONTRIBUTE_FK://贡献房卡
		{
			return On_CMD_LC_CLUB_CONTRIBUTE_FK(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_AUTO_AGREE://牌友圈设置
		{
			return On_CMD_LC_CLUB_AUTO_AGREE(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_INVITE://邀请他人加入牌友圈
		{
			return On_CMD_LC_CLUB_INVITE(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_INVITE_REMIND://被邀请人的提醒
		{
			return On_CMD_LC_CLUB_INVITE_REMIND(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_INVITE_RESULT: //被邀请人 回复
		{
			return On_CMD_LC_CLUB_INVITE_RESULT(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_INQUERY_LIST: //被邀请人查看自己的邀请列表 返回
		{
			return On_CMD_LC_CLUB_INQUERY_LIST(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_APPLICANT_LIST://申请人列表 返回
		{
			return On_CMD_LC_CLUB_APPLICANT_LIST(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_APPLICANT_LIST_FINISH://申请人列表 结束
		{
			return On_CMD_LC_CLUB_APPLICANT_LIST_FINISH(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_APPOINTMENT: //职务任免
		{
			return On_CMD_LC_CLUB_APPOINTMENT(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_APPOINTMENT_NOTE: //职务任免 提醒
		{
			return On_CMD_LC_CLUB_APPOINTMENT_NOTE(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CHAT_ALL://俱乐部聊天 返回
		{
			return On_CMD_LC_CLUB_CHAT(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CHAT_ALL_BDCAST://俱乐部聊天 广播
		{
			return On_CMD_LC_CLUB_CHAT_BDCAST(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_STICKY_POST:	//牌友圈置顶返回
		{
			return On_CMD_LC_CLUB_STICKY_POST(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_INQUERY_LIST_FINISH: //被邀请人查看自己的邀请列表 结束
		{
			return On_CMD_LC_CLUB_INQUERY_LIST_FINISH(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_TABLE_LIST_TABLE: //桌子列表
		{
			return On_CMD_LC_CLUB_TABLE_LIST_TABLE(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_TABLE_LIST_USER:	//桌子玩家列表
		{
			return On_CMD_LC_CLUB_TABLE_LIST_USER(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_CREATE_CLUB://创建牌友圈 返回
		{
			return On_CMD_LC_CLUB_CREATE_CLUB(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_JOIN_ROOM: //申请加入房间 返回
		{
			return On_CMD_LC_CLUB_JOIN_ROOM(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_APPLICANT_RESULT: //群主|管理对申请消息的答复(同意|拒绝) 返回
		{
			return On_CMD_LC_CLUB_APPLICANT_RESULT(dwScoketID,pData,wDataSize);
		}
		/*
	case DBO_LC_CLUB_JOIN_CLUB_MESSAGE_FINISH: //群主|管理对申请消息的答复(同意|拒绝) 结束
		{
			return On_CMD_LC_CLUB_APPLICANT_LIST_FINISH(dwScoketID,pData,wDataSize);
		}
		*/
	case DBO_LC_CLUB_MEMBER_MANAGER://请求成员数据 返回
		{
			return On_CMD_LC_CLUB_MEMBER_MANAGER(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_MEMBER_MANAGER_FINISH://请求成员数据 结束
		{
			return On_CMD_LC_CLUB_MEMBER_MANAGER_FINISH(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_DATA ://工会基本信息
		{
			return On_CMD_LC_CLUB_DATA(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_QUIT://踢出退出请求 返回
		{
			return On_CMD_LC_CLUB_QUIT(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_RECORD_LIST: //工会战绩统计 返回
		{
			return On_CMD_LC_CLUB_RECORD_LIST(dwScoketID,pData,wDataSize);
		}
	case DBO_LC_CLUB_RECORD_FINISH: //工会战绩统计 结束
		{
			return On_CMD_LC_CLUB_RECORD_FINISH(dwScoketID,pData,wDataSize);
		}
#pragma endregion
	}

}

//登录模块
bool CHandleFromGate::OnTCPNetworkMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CL_LOGON_ACCOUNTS:		//帐号登录
		{
			return On_SUB_CL_Logon_Accounts(pData,wDataSize,dwSocketID);
		}
	case SUB_CL_LOGON_REGISTER:		//帐号注册
		{
			return On_SUB_CL_Logon_Register(pData,wDataSize,dwSocketID);
		}
	case SUB_CL_LOGON_PLATFORM:		//平台登录
		{
			return On_SUB_CL_Logon_Platform(pData,wDataSize,dwSocketID);
		}
	}
	return false;
}
//用户服务
bool CHandleFromGate::OnTCPNetworkMainService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CL_SERVICE_USER_FEEDBACK:			//玩家反馈
		{
			return On_SUB_CL_Service_UserFeedBack(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_REFRESH_USER_INFO:		//刷新用户信息
		{
			return On_SUB_CL_Service_RefreshUserInfo(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_QUERY_ROOM_LIST:		//查询开房信息列表
		{
			return On_SUB_CL_Service_QueryRoomList(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_MODIFY_PERSONAL_INFO:	//修改个人资料
		{
			return On_SUB_CL_Service_ModifyPersonalInfo(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_GET_RICH_LIST:			//获取富豪榜
		{
			return On_SUB_CL_Service_GetRichList(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_GET_DJ_RECORD_LIST:	//获取用户录像列表
		{
			return On_SUB_CL_Service_GetUserRecordList(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_GET_XJ_RECORD_LIST:	//获取指定ID录像
		{
			return On_SUB_CL_Service_GetSpecificRecord(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_ONLINE_REWARD:			//获取在线奖励
		{
			return On_SUB_CL_Service_OnlineReward(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_GET_TASK_LIST:			//获取任务列表
		{
			return On_SUB_CL_Service_GetTaskList(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_GET_TASK_REWARD:		//领取任务奖励
		{
			return On_SUB_CL_Service_GetTaskReward(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_GET_RANK_LIST:			//获取排行榜
		{
			return On_SUB_CL_Service_GetRankList(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_REQUEST_LOTTERY:		//请求抽奖
		{
			return On_SUB_CL_Service_RequestLottery(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_PURE_STANDING_LIST:		//pure大厅排行版
		{
			return On_SUB_CL_SERVICE_PURE_STANDING_LIST(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_PURE_RECORD_LIST:		//大局战绩
		{
			return On_SUB_CL_SERVICE_PURE_RECORD_LIST(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_PURE_XJ_RECORD_LIST:	//小局战绩
		{
			return On_SUB_CL_SERVICE_PURE_XJ_RECORD_LIST(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_GOLD_INFO: //金币场信息
		{
			return On_SUB_CL_SERVICE_GOLD_INFO(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_XJ_RECORD_PLAYBACK:	//小局录像回放
		{
			return On_SUB_CL_Service_XJRecordPlayback(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_CUSTOMER_MESSEGE:	//客服消息
		{
			return On_SUB_CL_SERVICE_CUSTOMER_MESSEGE(pData, wDataSize, dwSocketID);
		}
	}

	return false;
}
//牌友圈
bool CHandleFromGate::On_MDM_CLUB(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch(wSubCmdID)
	{
	case SUB_CL_CLUB_ALL_CLUB_INFO_LIST:	 //查询牌友圈列表
		{
			return On_SUB_CL_CLUB_ALL_CLUB_INFO_LIST(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_ROOM_LIST:  //查询指定牌友圈房间列表
		{
			return On_SUB_CL_CLUB_ROOM_LIST(pData, wDataSize, dwSocketID);	
		}
	case SUB_CL_CLUB_RANDOM_CLUB_LIST: //查询未满员, 随机牌友圈(最大9个)
		{
			return On_SUB_CL_CLUB_RANDOM_CLUB_LIST(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_JOIN_CLUB: //申请加入牌友圈
		{
			return On_SUB_CL_CLUB_JOIN_CLUB(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_DISS_CLUB: //解散牌友圈
		{
			return On_SUB_CL_CLUB_DISS_CLUB(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_CREATE_CLUB:	 //创建牌友圈
		{
			return On_SUB_CL_CLUB_CREATE_CLUB(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_JOIN_ROOM: //申请加入房间
		{
			return On_SUB_CL_CLUB_JOIN_ROOM(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_ROOM_SETTING: //房间设置
		{
			return On_SUB_CL_CLUB_ROOM_SETTING(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_ROOM_QUERY_SETTING://请求房间设置
		{
			return On_SUB_CL_CLUB_ROOM_QUERY_SETTING(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_ROOM_USER_LEAVE://玩家离开房间
		{
			return On_SUB_CL_CLUB_ROOM_USER_LEAVE(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_ROOM_DISSOLVE:	//解散房间
		{
			return On_SUB_CL_CLUB_ROOM_DISSOLVE(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_TABLE_DISSOLVE:	//解散桌子
		{
			return On_SUB_CL_CLUB_TABLE_DISSOLVE(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_NOTICE: //牌友圈公告
		{
			return On_SUB_CL_CLUB_NOTICE(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_MESSAGE: //牌友圈简介
		{
			return On_SUB_CL_CLUB_MESSAGE(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_CONTRIBUTE_FK: //贡献房卡
		{
			return On_SUB_CL_CLUB_CONTRIBUTE_FK(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_AUTO_AGREE: //牌友圈设置
		{
			return On_SUB_CL_CLUB_AUTO_AGREE(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_CHAT://牌友圈 聊天
		{
			return On_SUB_CL_CLUB_CHAT(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_STICKY_POST:	//牌友圈置顶
		{
			return On_SUB_CL_CLUBSTICKY_POST(pData, wDataSize, dwSocketID);
		}
		/*
	case SUB_CL_CLUB_CREATE_CLUB_ROOM_APPLY:  //创建牌友圈房间
		{
			return On_SUB_CL_CLUB_CREATE_CLUB_ROOM_APPLY(pData, wDataSize, dwSocketID);	
		}
		*/
	case SUB_CL_CLUB_APPLICANT_RESULT:	 //群主|管理对申请消息的答复(同意|拒绝)
		{
			return On_SUB_CL_CLUB_APPLICANT_RESULT(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_MEMBER_MANAGER:  //请求成员数据
		{
			return On_SUB_CL_CLUB_MEMBER_MANAGER(pData, wDataSize, dwSocketID);	
		}
	case SUB_CL_CLUB_INVITE://邀请他人加入牌友圈
		{
			return On_SUB_CL_CLUB_INVITE(pData, wDataSize, dwSocketID);	
		}
	case SUB_CL_CLUB_INVITE_RESULT:// 被邀请人 回复
		{
			return On_SUB_CL_CLUB_INVITE_RESULT(pData, wDataSize, dwSocketID);	
		}
	case SUB_CL_CLUB_INQUERY_LIST://被邀请人查看自己的邀请列表
		{
			return On_SUB_CL_CLUB_INQUERY_LIST(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_APPLICANT_LIST: //申请人列表
		{
			return On_SUB_CL_CLUB_APPLICANT_LIST(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_QUIT: //踢出退出请求
		{
			return On_SUB_CL_CLUB_QUIT(pData, wDataSize, dwSocketID);
		}
	case  SUB_CL_CLUB_APPOINTMENT://职务任免
		{
			return On_SUB_CL_CLUB_APPOINTMENT(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_CLUB_RECORD_LIST:	//工会战绩统计
		{
			return On_SUB_CL_CLUB_RECORD_LIST(pData, wDataSize, dwSocketID);
		}
	}

	return false;
}
//礼物道具
bool CHandleFromGate::OnTCPNetworkMainOther(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CL_GIFT_GIVE_PROPS:				//赠送道具
		{
			return On_SUB_CL_GIFT_GIVE_PROPS(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_OTHERS_RECHARGE_INFO:				//充值信息
		{
			return On_SUB_CL_Other_ReChargeInfo(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_OTHERS_EXCHANGE_INFO:				//兑换道具信息
		{
			return On_SUB_CL_Other_ExchangeInfo(pData, wDataSize, dwSocketID);
		}
	}

	return false;
}
//商城道具
bool CHandleFromGate::On_MDM_SHOP(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch(wSubCmdID)
	{
	case SUB_CL_SHOP_QUERY:	 //查询商城
		{
			return On_SUB_CL_SHOP_QUERY(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_BAG_QUERY: //背包物品查询
		{
			return On_SUB_CL_BAG_QUERY(pData, wDataSize, dwSocketID);
		}	
	}

	return false;
}
//游戏服相关
bool CHandleFromGate::On_MDM_GAME(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch(wSubCmdID)
	{
	case SUB_CL_GAME_QUERY_GAMEID:	 //创建房间之前, 先查询可用的GameID
		{
			//校验数据
			if (wDataSize!=sizeof(STR_SUB_CL_GAME_QUERY_GAMEID)) return true;

			STR_SUB_CL_GAME_QUERY_GAMEID *pSub = (STR_SUB_CL_GAME_QUERY_GAMEID*)pData;
			
			STR_CPR_LP_CREATE_TABLE CPR;
			CPR.dwKindID = pSub->dwKindID;
			CPR.dwSocketID = dwSocketID;

			CLog::Log(log_debug, "Game KindID: %d", CPR.dwKindID);

			g_TCPSocketEngine->SendData(CPD_MDM_TRANSFER, CPR_LP_CREATE_TABLE, &CPR, sizeof(CPR));

			return true;
		}
	}

	return false;
}


#pragma region MDM_LOGON 登录模块
/***************************************** 【登录处理函数-主消息1】 *******************************************/
//帐号登录
bool CHandleFromGate::On_SUB_CL_Logon_Accounts(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	if (wDataSize != sizeof(STR_SUB_CL_LOGON_ACCOUNTS))
	{
		return false;
	}

	//处理消息
	STR_SUB_CL_LOGON_ACCOUNTS * pSUBLogonAccounts=(STR_SUB_CL_LOGON_ACCOUNTS *)pData;

	//版本判断（版本不对，直接退出）
	if ( !On_CMD_LC_Logon_UpdateNotify(pSUBLogonAccounts->dwVersionCheck, dwSocketID) )
	{
		return true;
	}

	//变量构造
	STR_DBR_CL_LOGON_ACCOUNTS DBRLogonAccounts;
	ZeroMemory(&DBRLogonAccounts,sizeof(DBRLogonAccounts));

	//数据赋值
	lstrcpyn(DBRLogonAccounts.szAccounts,pSUBLogonAccounts->szAccounts,CountArray(DBRLogonAccounts.szAccounts));
	lstrcpyn(DBRLogonAccounts.szPassword,pSUBLogonAccounts->szPassword,CountArray(DBRLogonAccounts.szPassword));
	lstrcpyn(DBRLogonAccounts.szMachineID,pSUBLogonAccounts->szMachineID,CountArray(DBRLogonAccounts.szMachineID));
	DBRLogonAccounts.dwProxyID = pSUBLogonAccounts->dwProxyID;

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_LOGON_ACCOUNTS,dwSocketID,&DBRLogonAccounts,sizeof(DBRLogonAccounts));
	return true;
}

//账号登录返回
bool CHandleFromGate::On_CMD_LC_Logon_Account(DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//数据校验
	if(wDataSize != sizeof(STR_CMD_LC_LOGON_PLATFORM)) return false;

	//变量定义
	STR_CMD_LC_LOGON_PLATFORM * pCMD=(STR_CMD_LC_LOGON_PLATFORM *)pData;

	//如果登录失败, 直接返回
	if(DB_SUCCESS != pCMD->dwResultCode)
	{
		//发送数据
		g_GameCtrl->SendData(dwScoketID, MDM_LOGON, CMD_LC_LOGON_ACCOUNTS, pCMD, sizeof(STR_CMD_LC_LOGON_PLATFORM));
		return true;
	}

	//重复登录处理
	On_CMD_LC_Logon_RepeatLogon( pCMD->dwUserID, dwScoketID );

	//断线重连的处理
	pCMD->dwOffLineGameID  = CPlayerManager::FindOfflineGameID(pCMD->dwUserID);

	//玩家记录
	tagUserInfo UserInfo;
	ZeroMemory(&UserInfo, sizeof(UserInfo));
	UserInfo.dwUserID = pCMD->dwUserID;

	CPlayerManager::InsertPlayer (dwScoketID, UserInfo);
	CPlayer* player = CPlayerManager::FindPlayerBySocketID(dwScoketID);

	//发送登录成功
	g_GameCtrl->SendData(dwScoketID, MDM_LOGON, CMD_LC_LOGON_ACCOUNTS, pCMD, sizeof(STR_CMD_LC_LOGON_PLATFORM));

	//登录奖励
	On_CMD_LC_Logon_Logon_Reward(dwScoketID, pCMD->LasLogonDate);

	return true;
}

//帐号注册
bool CHandleFromGate::On_SUB_CL_Logon_Register(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	if (wDataSize != sizeof(STR_SUB_CL_LOGON_REGISTER))
	{
		return false;
	}
	
	//处理消息
	STR_SUB_CL_LOGON_REGISTER * pSUBLogonRegister=(STR_SUB_CL_LOGON_REGISTER *)pData;

	/*
	//效验版本
	if ( !On_CMD_LC_Logon_UpdateNotify(pSUBLogonRegister->dwVersionCheck, dwSocketID) )
	{
		return true;
	}
	*/

	//变量定义
	STR_DBR_CL_LOGON_REGISTER DBRLogonRegister;
	ZeroMemory(&DBRLogonRegister,sizeof(DBRLogonRegister));

	//数据赋值
	lstrcpyn(DBRLogonRegister.szAccounts,pSUBLogonRegister->szAccounts,CountArray(DBRLogonRegister.szAccounts));
	lstrcpyn(DBRLogonRegister.szPassword,pSUBLogonRegister->szPassword,CountArray(DBRLogonRegister.szPassword));
	lstrcpyn(DBRLogonRegister.szNickName,pSUBLogonRegister->szNickName,CountArray(DBRLogonRegister.szNickName));
	DBRLogonRegister.cbGender = pSUBLogonRegister->cbGender;
	lstrcpyn(DBRLogonRegister.strMobilePhone,pSUBLogonRegister->strMobilePhone,CountArray(DBRLogonRegister.strMobilePhone));
	lstrcpyn(DBRLogonRegister.szMachineID,pSUBLogonRegister->szMachineID,CountArray(DBRLogonRegister.szMachineID));
	DBRLogonRegister.dwProxyID = pSUBLogonRegister->dwProxyID;

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_LOGON_REGISTER,dwSocketID,&DBRLogonRegister,sizeof(DBRLogonRegister));

	return true;
}

//平台登录
bool CHandleFromGate::On_SUB_CL_Logon_Platform(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	if (wDataSize != sizeof(STR_SUB_CL_LOGON_PLATFORM))
	{
		return false;
	}

	//处理消息
	STR_SUB_CL_LOGON_PLATFORM * pSUBLogonPlatform=(STR_SUB_CL_LOGON_PLATFORM *)pData;

	//效验版本
	if ( !On_CMD_LC_Logon_UpdateNotify(pSUBLogonPlatform->dwVersionCheck, dwSocketID) )
	{
		return true;
	}

	//变量构造
	STR_DBR_CL_LOGON_PLATFORM DBRLogonPlatform;
	ZeroMemory(&DBRLogonPlatform,sizeof(DBRLogonPlatform));

	DBRLogonPlatform.cbGender = pSUBLogonPlatform->cbGender;
	lstrcpyn(DBRLogonPlatform.szNickName, pSUBLogonPlatform->szNickName, CountArray(DBRLogonPlatform.szNickName));
	lstrcpyn(DBRLogonPlatform.strHeadUrl, pSUBLogonPlatform->strHeadUrl, CountArray(DBRLogonPlatform.strHeadUrl));
	lstrcpyn(DBRLogonPlatform.szOpenID, pSUBLogonPlatform->szOpenID, CountArray(DBRLogonPlatform.szOpenID));
	lstrcpyn(DBRLogonPlatform.szUnionID, pSUBLogonPlatform->szUnionID, CountArray(DBRLogonPlatform.szUnionID));
	lstrcpyn(DBRLogonPlatform.szMachineID,pSUBLogonPlatform->szMachineID,CountArray(DBRLogonPlatform.szMachineID));
	DBRLogonPlatform.dwProxyID = pSUBLogonPlatform->dwProxyID;

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_LOGON_PLATFORM, dwSocketID, &DBRLogonPlatform, sizeof(DBRLogonPlatform));

	return true;}

//平台登录返回
bool CHandleFromGate::On_CMD_LC_Logon_Platform(DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//效验参数
	if(wDataSize != sizeof(STR_CMD_LC_LOGON_PLATFORM)) return false;

	//变量定义
	STR_CMD_LC_LOGON_PLATFORM * pCMD = (STR_CMD_LC_LOGON_PLATFORM *)pData;

	//如果登录失败, 直接返回
	if(DB_SUCCESS != pCMD->dwResultCode)
	{
		g_GameCtrl->SendData(dwScoketID, MDM_LOGON, CMD_LC_LOGON_ACCOUNTS, pCMD, sizeof(STR_CMD_LC_LOGON_PLATFORM));
		return true;
	}

	//重复登录踢出
	On_CMD_LC_Logon_RepeatLogon( pCMD->dwUserID ,dwScoketID);

	//断线重连的处理

	//玩家记录
	//CPlayerManager::InsertPlayer (dwScoketID, pCMD);

	//发送登录成功
	g_GameCtrl->SendData(dwScoketID,MDM_LOGON,CMD_LC_LOGON_PLATFORM, pCMD, sizeof(STR_CMD_LC_LOGON_PLATFORM));

	//登录奖励
	On_CMD_LC_Logon_Logon_Reward(dwScoketID, pCMD->LasLogonDate);

	return true;
}

//重复登录踢出
bool CHandleFromGate::On_CMD_LC_Logon_RepeatLogon(DWORD UserID, DWORD dwScoketID)
{
	//重复登录
	bool bIsRepeatLogon = false;

	CPlayer* player = CPlayerManager::FindPlayerByID(UserID);
	if(player)
	{
		STR_CMD_LC_LOGON_REPEAT_LOGON pCMDLogonRepeatLogon;
		ZeroMemory(&pCMDLogonRepeatLogon, sizeof(STR_CMD_LC_LOGON_REPEAT_LOGON));

		//提示之前用户账号已登录，无法登录
		_snwprintf(pCMDLogonRepeatLogon.szDescribe, sizeof(pCMDLogonRepeatLogon.szDescribe), TEXT("账号已经在其它地方登录"));
		g_GameCtrl->SendData(player->GetSocketID(), MDM_LOGON, CMD_LC_LOGON_REPEAT_LOGON, &pCMDLogonRepeatLogon, sizeof(STR_CMD_LC_LOGON_REPEAT_LOGON));
		//关闭连接 --  关闭之前的连接
		g_TCPNetworkEngine->CloseSocket(player->GetSocketID());
		//删除之前玩家
		CPlayerManager::DeletePlayerByID(UserID);

		//提示已登录的用户 TODONOW 使用新的消息, 不能是CMD_LC_LOGON_REPEAT_LOGON
		/*
		_snwprintf(pCMDLogonRepeatLogon.szDescribe, sizeof(pCMDLogonRepeatLogon.szDescribe), TEXT("账号在其它地方登录"));
		g_GameCtrl->SendData(dwScoketID, MDM_LOGON, CMD_LC_LOGON_REPEAT_LOGON, &pCMDLogonRepeatLogon, sizeof(STR_CMD_LC_LOGON_REPEAT_LOGON));
		*/

		bIsRepeatLogon = true;
	}

	return bIsRepeatLogon;
}

//升级提示（版本校验）
bool CHandleFromGate::On_CMD_LC_Logon_UpdateNotify(DWORD dwVersionCheck, DWORD dwSocketID)
{
	/*
	DWORD serverFrameVersion = Get_Framework_Version(PLATFORM_VERSION);  //服务端 frame 版本
	DWORD clientFrameVersion = dwVersionCheck; //client  Hall 版本

	byte ret = Compate_Hall_LogonServer(clientFrameVersion, serverFrameVersion);
	CLog::Log(log_debug, "版本校验结果:%d  服务器版本:%ld  客户端版本:%ld", ret, serverFrameVersion, clientFrameVersion);

	//版本匹配, 则直接退出, 不需要发送消息
	if(0 == ret )
	{
		return false;
	}

	//构造数据
	STR_CMD_LC_LOGON_UPDATE_NOTIFY UpdateNotify;
	ZeroMemory(&UpdateNotify,sizeof(UpdateNotify));
	UpdateNotify.cbUpdateNotify = ret;
	UpdateNotify.dwCurrentServerVersion = serverFrameVersion;
	//发送消息
	g_GameCtrl->SendData(dwSocketID, MDM_LOGON, CMD_LC_LOGON_UPDATE_NOTIFY, &UpdateNotify, sizeof(UpdateNotify));
	*/

	return true;
}

//登录奖励
bool CHandleFromGate::On_CMD_LC_Logon_Logon_Reward(DWORD dwScoketID, SYSTEMTIME LasLogonDate)
{ 
	// 1. 奖励应该是读取数据库
	// 2. 登录奖励 -- 1)节日奖励  2)老玩家奖励等
	
	//获得系统时间
	SYSTEMTIME systime;
	GetLocalTime(&systime);

	CTime tmLeft(systime.wYear, systime.wMonth, systime.wDay, 0, 0, 0);
	CTime tmRight(LasLogonDate.wYear, LasLogonDate.wMonth, LasLogonDate.wDay, 0, 0, 0);

	//获得上次登录时间
	CTimeSpan sp = tmLeft - tmRight;
	long MillisecondsL = (systime.wHour*3600 + systime.wMinute*60 + systime.wSecond)*1000 + systime.wMilliseconds;  
	long MillisecondsR = (LasLogonDate.wHour*3600 + LasLogonDate.wMinute*60 + LasLogonDate.wSecond)*1000 + LasLogonDate.wMilliseconds;  

	//计算时间间隔
	LONGLONG seconds =  ((__int64)sp.GetDays()*86400000 + (MillisecondsL - MillisecondsR))/1000;//此处返回秒

	//计算是否满30天,奖励50房卡
	if(seconds > 30*24*3600)
	{
		//变更用户财富
		DBR_GP_ModifyUserInsure modifyUserInsure;
		ZeroMemory(&modifyUserInsure,sizeof(DBR_GP_ModifyUserInsure));
		modifyUserInsure.lOpenRoomCard = 50;
		g_GameCtrl->PostDataBaseRequest(DBR_GP_MODIFY_USER_INSURE, dwScoketID, &modifyUserInsure, sizeof(DBR_GP_ModifyUserInsure));

		//老玩家奖励返回
		STR_CMD_LC_LOGON_LOGON_REWARD OldBackReward;
		OldBackReward.byRewardType = 2;
		OldBackReward.dwRewardCount = 50;
		lstrcpyn( OldBackReward.szDescribe,TEXT("欢迎回来，您已获得老玩家回归奖励50房卡!"),CountArray(OldBackReward.szDescribe));

		g_GameCtrl->SendData(dwScoketID,MDM_SERVICE, CMD_LC_LOGON_LOGON_REWARD, &OldBackReward, sizeof(STR_CMD_LC_LOGON_LOGON_REWARD));
	}
	
	return true;
}

#pragma endregion

#pragma region MDM_SERVICE 用户服务
/***************************************** 【服务处理函数-主消息3】 *******************************************/
//玩家反馈
bool CHandleFromGate::On_SUB_CL_Service_UserFeedBack(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	ASSERT( wDataSize == sizeof(STR_SUB_CL_SERVICE_FEEDBACK));
	if(wDataSize != sizeof(STR_SUB_CL_SERVICE_FEEDBACK)) return false;

	//处理消息
	STR_SUB_CL_SERVICE_FEEDBACK * pUserSuggestion = (STR_SUB_CL_SERVICE_FEEDBACK *)pData;

	//构造玩家反馈数据
	STR_DBR_CL_SERVICE_FEEDBACK UserSuggestion;
	ZeroMemory(&UserSuggestion,sizeof(UserSuggestion));
	UserSuggestion.dwUserID = pUserSuggestion->dwUserID;
	lstrcpyn(UserSuggestion.szFB_Title,pUserSuggestion->szFB_Title,CountArray(UserSuggestion.szFB_Title));
	lstrcpyn(UserSuggestion.szContact,pUserSuggestion->szContact,CountArray(UserSuggestion.szContact));
	lstrcpyn(UserSuggestion.szFB_Content,pUserSuggestion->szFB_Content,CountArray(UserSuggestion.szFB_Content));

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_SERVICE_USER_FEEDBACK, dwSocketID, &UserSuggestion, sizeof(UserSuggestion));

	return true;
}

//玩家反馈返回
bool CHandleFromGate::On_CMD_LC_Service_UserFeedBack( DWORD dwScoketID, VOID * pData)
{
	//变量定义
	STR_CMD_LC_SERVICE_FEEDBACK FeedBack;
	ZeroMemory(&FeedBack, sizeof(FeedBack));

	//变量定义
	STR_DBO_CL_SERVICE_FEEDBACK *pOperate = (STR_DBO_CL_SERVICE_FEEDBACK *)pData;

	//构造数据
	FeedBack.lResultCode=pOperate->lResultCode;
	lstrcpyn(FeedBack.szDescribeString, pOperate->szDescribeString, CountArray(FeedBack.szDescribeString));

	//发送数据
	WORD wDataSize = sizeof(FeedBack);
	g_GameCtrl->SendData(dwScoketID, MDM_SERVICE, CMD_LC_SERVICE_USER_FEEDBACK, &FeedBack, wDataSize);

	return true;
}

//刷新用户信息
bool CHandleFromGate::On_SUB_CL_Service_RefreshUserInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//定义变量
	STR_DBR_CL_SERCIVR_REFRESH_INFO UserRequest;
	ZeroMemory(&UserRequest,sizeof(UserRequest));

	CPlayer *player = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if(player)
	{
		UserRequest.dwUserID = player->GetUserID();
		g_GameCtrl->PostDataBaseRequest(DBR_CL_SERVICE_REFRESH_USER_INFO, dwSocketID, &UserRequest, sizeof(UserRequest));
	}
	return true;
}

//刷新用户信息返回
bool CHandleFromGate::On_CMD_LC_Service_RefreshUserInfo( DWORD dwScoketID, VOID * pData )
{
	//变量定义
	STR_CMD_LC_SERVICE_REFRESH_INFO UserInfo;
	ZeroMemory(&UserInfo,sizeof(UserInfo));

	//变量定义
	STR_DBO_CL_SERCIVR_REFRESH_INFO * pUserInfo=(STR_DBO_CL_SERCIVR_REFRESH_INFO *)pData;

	//构造数据
	memcpy_s(&UserInfo, sizeof(STR_CMD_LC_SERVICE_REFRESH_INFO), pUserInfo, sizeof(STR_CMD_LC_SERVICE_REFRESH_INFO));

	//发送数据
	WORD wDataSize = sizeof(STR_CMD_LC_SERVICE_REFRESH_INFO);
	g_GameCtrl->SendData(dwScoketID, MDM_SERVICE, CMD_CL_SERVICE_REFRESH_USER_INFO, &UserInfo, wDataSize);

	return true;
}

//查询开房信息列表
bool CHandleFromGate::On_SUB_CL_Service_QueryRoomList(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	ASSERT( wDataSize == sizeof(STR_SUB_CL_SERVICE_QUERY_ROOMLIST));
	if(wDataSize != sizeof(STR_SUB_CL_SERVICE_QUERY_ROOMLIST)) return false;

	STR_SUB_CL_SERVICE_QUERY_ROOMLIST* pTableInfoList = (STR_SUB_CL_SERVICE_QUERY_ROOMLIST*)pData;

	
	STR_DBR_CL_SERCIVR_QUERY_ROOMLIST GetTableInfoList;
	GetTableInfoList.dwUserID = pTableInfoList->dwUserID;

	//查询开房列表
	g_GameCtrl->PostDataBaseRequest(DBR_CL_SERVICE_QUERY_ROOM_LIST,dwSocketID,&GetTableInfoList,sizeof(STR_DBR_CL_SERCIVR_QUERY_ROOMLIST));

	return true;
}

//查询开房列表返回
bool CHandleFromGate::On_CMD_LC_Service_QueryRoomList(DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT(wDataSize==sizeof(STR_DBO_CL_SERCIVR_QUERY_ROOMLIST));
	STR_DBO_CL_SERCIVR_QUERY_ROOMLIST *pRecord = (STR_DBO_CL_SERCIVR_QUERY_ROOMLIST*)pData;

	STR_CMD_LC_SERVICE_QUERY_ROOMLIST TableInfo;
	ZeroMemory(&TableInfo,sizeof(STR_CMD_LC_SERVICE_QUERY_ROOMLIST));

	memcpy(&TableInfo, pRecord, sizeof(STR_CMD_LC_SERVICE_QUERY_ROOMLIST));

	g_GameCtrl->SendData(dwScoketID, MDM_SERVICE, CMD_LC_SERVICE_QUERY_ROOM_LIST, &TableInfo, sizeof(STR_CMD_LC_SERVICE_QUERY_ROOMLIST));

	return true;
}

//获取富豪榜
bool CHandleFromGate::On_SUB_CL_Service_GetRichList(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验
	ASSERT( wDataSize == sizeof(STR_SUB_CL_SERVICE_GET_RICHLIST));
	if( wDataSize != sizeof(STR_SUB_CL_SERVICE_GET_RICHLIST) ) 		
	{
		return false;
	}
	
	return g_GameCtrl->PostDataBaseRequest(DBR_CL_SERVICE_GET_RICH_LIST, dwSocketID, pData, 0);
}

//获取富豪榜返回
bool CHandleFromGate::On_CMD_LC_Service_GetRichList( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//参数校验
	ASSERT(wDataSize==sizeof(STR_DBO_CL_SERCIVR_GET_RICHLIST));
	if(wDataSize!=sizeof(STR_DBO_CL_SERCIVR_GET_RICHLIST))
		return false;

	STR_DBO_CL_SERCIVR_GET_RICHLIST *pLotteryResult = (STR_DBO_CL_SERCIVR_GET_RICHLIST*)pData;

	STR_CMD_LC_SERVICE_GET_RICHLIST LotteryResult;
	ZeroMemory(&LotteryResult,sizeof(STR_CMD_LC_SERVICE_GET_RICHLIST));

	memcpy(&LotteryResult,pLotteryResult,sizeof(STR_DBO_CL_SERCIVR_GET_RICHLIST));		//修改bug，原来的sizeof里面不是这个结构体

	g_GameCtrl->SendData(dwScoketID,MDM_SERVICE, CMD_LC_SERVICE_GET_RICH_LIST, &LotteryResult, sizeof(STR_CMD_LC_SERVICE_GET_RICHLIST));

	return true;
}

//获取用户录像列表
bool CHandleFromGate::On_SUB_CL_Service_GetUserRecordList(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	ASSERT( wDataSize == sizeof(STR_SUB_CL_SERVICE_GET_DJ_RECORDLIST));
	if(wDataSize != sizeof(STR_SUB_CL_SERVICE_GET_DJ_RECORDLIST)) return false;

	STR_SUB_CL_SERVICE_GET_DJ_RECORDLIST* pGetRecordList = (STR_SUB_CL_SERVICE_GET_DJ_RECORDLIST*)pData;

	//录像列表请求
	STR_DBR_CL_SERCIVR_GET_RECORDLIST GetRecordList;
	GetRecordList.dwUserID = pGetRecordList->dwUserID;

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_SERVICE_GET_USER_RECORD_LIST, dwSocketID, &GetRecordList, sizeof(STR_DBR_CL_SERCIVR_GET_RECORDLIST));

	return true;
}

//获取用户录像列表返回
bool CHandleFromGate::On_CMD_LC_Service_GetUserRecordList(DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT(wDataSize==sizeof(STR_DBO_CL_SERCIVR_GET_RECORDLIST));
	STR_DBO_CL_SERCIVR_GET_RECORDLIST *pRecord = (STR_DBO_CL_SERCIVR_GET_RECORDLIST*)pData;

	STR_CMD_LC_SERVICE_GET_DJ_RECORDLIST Record;
	ZeroMemory(&Record,sizeof(STR_CMD_LC_SERVICE_GET_DJ_RECORDLIST));

	Record.dwTableID = pRecord->dwTableID;
	memcpy( Record.dwUserID, pRecord->dwUserID,sizeof(DWORD)*5);

	for(int i=0;i < 5;i++)
	{
		memcpy(Record.szUserName[i],pRecord->szUserName[i],sizeof(TCHAR)*LEN_NICKNAME);	
	}

	Record.byGameMode = pRecord->byGameMode;
	Record.byZhuangType = pRecord->byZhuangType;
	Record.wCurrentJuShu = pRecord->wCurrentJuShu;
	Record.wTotalJuShu = pRecord->wTotalJuShu;

	memcpy(Record.szGameTime, pRecord->szGameTime, sizeof(TCHAR)*30);
	memcpy(Record.szTotalScore, pRecord->szTotalScore, sizeof(TCHAR)*50);

	g_GameCtrl->SendData(dwScoketID, MDM_SERVICE, CMD_LC_SERVICE_GET_DJ_RECORD_LIST, &Record, sizeof(STR_CMD_LC_SERVICE_GET_DJ_RECORDLIST));

	//_bstr_t b(pRecord->szData);
	//char* output = b;

	//FILE *fp = fopen(output,"rb");
	//if(NULL != fp)
	//{
	//	if(pRecord->dwDataSize <= 3072)
	//	{
	//		fread(Record.szData,pRecord->dwDataSize,1,fp);

	//		memcpy(Record.szGameTime,pRecord->szGameTime,sizeof(TCHAR)*30);
	//		memcpy(Record.szScore,pRecord->szScore,sizeof(TCHAR)*50);
	//		memcpy(Record.szTotalScore,pRecord->szTotalScore,sizeof(TCHAR)*50);

	//		g_GameCtrl->SendData(dwScoketID, MDM_SERVICE, CMD_LC_SERVICE_GET_DJ_RECORD_LIST, &Record, sizeof(STR_CMD_LC_SERVICE_GET_DJ_RECORDLIST));

	//	}
	//	fclose(fp);
	//}

	return true;
}

//获取指定ID录像
bool CHandleFromGate::On_SUB_CL_Service_GetSpecificRecord(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	ASSERT( wDataSize == sizeof(STR_SUB_CL_SERVICE_GET_XJ_RECORDLIST));
	if(wDataSize != sizeof(STR_SUB_CL_SERVICE_GET_XJ_RECORDLIST)) return false;

	STR_SUB_CL_SERVICE_GET_XJ_RECORDLIST* pGetRecordList = (STR_SUB_CL_SERVICE_GET_XJ_RECORDLIST*)pData;

	//DBR数据
	STR_DBR_CL_SERCIVR_GET_SPECIFIC_RECORD GetRecordList;
	GetRecordList.dwTableID = pGetRecordList->dwTableID;

	//校验桌子ID
	if ( 0 != GetRecordList.dwTableID )
	{
		//投递请求
		g_GameCtrl->PostDataBaseRequest(DBR_CL_SERVICE_GET_SPECIFIC_RECORD,dwSocketID,&GetRecordList,sizeof(STR_DBR_CL_SERCIVR_GET_SPECIFIC_RECORD));
	}

	return true;
}

//获取指定ID录像返回
bool CHandleFromGate::On_CMD_LC_Service_GetSpecificRecord(DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT(wDataSize==sizeof(STR_DBO_CL_SERCIVR_GET_SPECIFIC_RECORD));
	STR_DBO_CL_SERCIVR_GET_SPECIFIC_RECORD *pRecord = (STR_DBO_CL_SERCIVR_GET_SPECIFIC_RECORD*)pData;

	STR_CMD_LC_SERVICE_GET_XJ_RECORDLIST Record;
	memcpy(&Record, pRecord, sizeof(STR_CMD_LC_SERVICE_GET_XJ_RECORDLIST));

	g_GameCtrl->SendData(dwScoketID, MDM_SERVICE, CMD_LC_SERVICE_GET_XJ_RECORD_LIST, &Record, sizeof(STR_CMD_LC_SERVICE_GET_XJ_RECORDLIST));

	return true;
}

//获取在线奖励
bool CHandleFromGate::On_SUB_CL_Service_OnlineReward(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	ASSERT(wDataSize == sizeof(STR_SUB_CL_SERVICE_ONLINE_REWARD));
	if(wDataSize != sizeof(STR_SUB_CL_SERVICE_ONLINE_REWARD))
		return false;

	STR_SUB_CL_SERVICE_ONLINE_REWARD * pGetLogonReward = (STR_SUB_CL_SERVICE_ONLINE_REWARD*)pData;

	STR_DBR_CL_SERCIVR_ONLINE_REWARD GetLogonReward;
	ZeroMemory(&GetLogonReward,sizeof(STR_DBR_CL_SERCIVR_ONLINE_REWARD));

	GetLogonReward.dwUserID = pGetLogonReward->dwUserID;

	return g_GameCtrl->PostDataBaseRequest(DBR_CL_SERVICE_ONLINE_REWARD,dwSocketID,&GetLogonReward,sizeof(GetLogonReward));
}

//获取在线奖励返回
bool CHandleFromGate::On_CMD_LC_Service_OnlineReward( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//参数校验
	ASSERT(wDataSize==sizeof(STR_DBO_CL_SERCIVR_ONLINE_REWARD));
	if(wDataSize!=sizeof(STR_DBO_CL_SERCIVR_ONLINE_REWARD))
		return false;

	STR_DBO_CL_SERCIVR_ONLINE_REWARD *pLotteryResult = (STR_DBO_CL_SERCIVR_ONLINE_REWARD*)pData;

	STR_CMD_LC_SERVICE_ONLINE_REWARD LotteryResult;
	ZeroMemory(&LotteryResult,sizeof(STR_CMD_LC_SERVICE_ONLINE_REWARD));

	LotteryResult.byType = pLotteryResult->byType;
	LotteryResult.dwCount = pLotteryResult->dwCount;
	
	g_GameCtrl->SendData(dwScoketID,MDM_SERVICE, CMD_LC_SERVICE_ONLINE_REWARD, &LotteryResult, sizeof(STR_CMD_LC_SERVICE_ONLINE_REWARD));

	return true;
}

//获取任务列表
bool CHandleFromGate::On_SUB_CL_Service_GetTaskList(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	ASSERT(wDataSize >= sizeof(STR_SUB_CL_SERVICE_GET_TASKLIST));
	if (wDataSize<sizeof(STR_SUB_CL_SERVICE_GET_TASKLIST)) return false;

	//处理消息
	STR_SUB_CL_SERVICE_GET_TASKLIST * pGetTaskList = (STR_SUB_CL_SERVICE_GET_TASKLIST *)pData;

	//变量定义
	STR_DBR_CL_SERCIVR_GET_TASKLIST GetTaskList;
	ZeroMemory(&GetTaskList,sizeof(GetTaskList));

	//构造数据
	memcpy(&GetTaskList,pGetTaskList,sizeof(GetTaskList));

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_SERVICE_GET_TASK_LIST,dwSocketID,&GetTaskList,sizeof(GetTaskList));

	return true;
}

//获取任务列表返回
bool CHandleFromGate::On_CMD_LC_Service_GetTaskList( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//参数校验
	if( 0 != (wDataSize % sizeof(STR_DBO_CL_SERCIVR_GET_TASKLIST)) )
		return false;

	//获得任务个数
	int count = wDataSize / sizeof(STR_DBO_CL_SERCIVR_GET_TASKLIST);

	//数据库任务列表
	STR_DBO_CL_SERCIVR_GET_TASKLIST *TaskList = (STR_DBO_CL_SERCIVR_GET_TASKLIST *)pData;

	//返回的任务列表
	STR_CMD_LC_SERVICE_GET_TASKLIST *pCMDTaskList = new STR_CMD_LC_SERVICE_GET_TASKLIST[count];

	//网络数据
	WORD wSendSize=0;
	BYTE cbDataBuffer[SOCKET_TCP_PACKET];

	//枚举数据
	for (int i=0; i<count; i++)
	{
		//将DBO数据赋给返回的数据,完成标志单独赋值
		memcpy_s(&pCMDTaskList[i], sizeof(STR_DBO_CL_SERCIVR_GET_TASKLIST), &TaskList[i], sizeof(STR_DBO_CL_SERCIVR_GET_TASKLIST));

		//最后一次发送,设置完成标志为1
		if ( i == (count-1) )		
		{
			pCMDTaskList[i].cbListFinishMask = 1;
		}
		else
		{
			pCMDTaskList[i].cbListFinishMask = 0;
		}
		
		//发送数据(防止数据太多，一批批发送)
		if ( (wSendSize+sizeof(STR_CMD_LC_SERVICE_GET_TASKLIST)) > sizeof(cbDataBuffer) )
		{
			//发送数据
			g_GameCtrl->SendData(dwScoketID, MDM_SERVICE, CMD_LC_SERVICE_GET_TASK_LIST, cbDataBuffer, wSendSize);			
			wSendSize=0;
		}

		//拷贝数据
		CopyMemory(cbDataBuffer+wSendSize, &pCMDTaskList[i], sizeof(STR_CMD_LC_SERVICE_GET_TASKLIST));
		wSendSize += sizeof(STR_CMD_LC_SERVICE_GET_TASKLIST);
	}

	//发送剩余
	if (wSendSize>0) 	
	{
		g_GameCtrl->SendData(dwScoketID, MDM_SERVICE, CMD_LC_SERVICE_GET_TASK_LIST, cbDataBuffer, wSendSize);
	}

	//释放内存		TODO 曾经报错过
	delete [] pData;		//前面数据时new出来的，所以需要delete，进程之前是内存传输的
	delete [] pCMDTaskList;

	return true;
}

//领取任务奖励
bool CHandleFromGate::On_SUB_CL_Service_GetTaskReward(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	ASSERT(wDataSize >= sizeof(STR_SUB_CL_SERVICE_GET_TASK_REWARD));
	if (wDataSize<sizeof(STR_SUB_CL_SERVICE_GET_TASK_REWARD)) return false;

	//处理消息
	STR_SUB_CL_SERVICE_GET_TASK_REWARD * pSetTaskDone =(STR_SUB_CL_SERVICE_GET_TASK_REWARD *)pData;
	//变量定义
	STR_DBR_CL_SERCIVR_GET_TASK_REWARD SetTaskDone;
	ZeroMemory(&SetTaskDone,sizeof(SetTaskDone));

	//构造数据
	memcpy(&SetTaskDone,pSetTaskDone,sizeof(SetTaskDone));

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_SERVICE_GET_TASK_REWARD,dwSocketID,&SetTaskDone,sizeof(SetTaskDone));

	return true;
}

//领取任务奖励返回
bool CHandleFromGate::On_CMD_LC_Service_GetTaskReward( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//参数校验
	ASSERT(wDataSize==sizeof(STR_DBO_CL_SERCIVR_GET_TASK_REWARD));
	if(wDataSize!=sizeof(STR_DBO_CL_SERCIVR_GET_TASK_REWARD))
		return false;

	STR_DBO_CL_SERCIVR_GET_TASK_REWARD *pTaskDone = (STR_DBO_CL_SERCIVR_GET_TASK_REWARD*)pData;

	STR_CMD_LC_SERVICE_GET_TASK_REWARD TaskDone;
	ZeroMemory(&TaskDone,sizeof(STR_CMD_LC_SERVICE_GET_TASK_REWARD));
	memcpy(&TaskDone,pTaskDone,sizeof(STR_CMD_LC_SERVICE_GET_TASK_REWARD));

	g_GameCtrl->SendData(dwScoketID, MDM_SERVICE, CMD_LC_SERVICE_GET_TASK_REWARD, &TaskDone, sizeof(STR_CMD_LC_SERVICE_GET_TASK_REWARD));

	return true;
}

//获取排行榜
bool CHandleFromGate::On_SUB_CL_Service_GetRankList(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//参数校验
	ASSERT(wDataSize==sizeof(STR_SUB_CL_SERVICE_GET_RANKLIST));
	if(wDataSize!=sizeof(STR_SUB_CL_SERVICE_GET_RANKLIST))
		return false;

	STR_SUB_CL_SERVICE_GET_RANKLIST* pTaskList = (STR_SUB_CL_SERVICE_GET_RANKLIST*)pData;

	STR_CMD_LC_SERVICE_GET_RANKLIST rankList;
	ZeroMemory(&rankList, sizeof(STR_CMD_LC_SERVICE_GET_RANKLIST));

	//排行榜管理器获取排行榜列表
	int wCount = RankManager::Instance()->GetRankCountByIndex(pTaskList->byIndex);

	for(int i=0;i < 20 && i < wCount;i++)
	{
		tagRankInfo* item = RankManager::Instance()->GetRankItemByIndex(pTaskList->byIndex,i);
		if(item != NULL)
		{
			g_GameCtrl->SendData(dwSocketID,MDM_SERVICE, CMD_LC_SERVICE_GET_RANK_LIST, item, sizeof(STR_CMD_LC_SERVICE_GET_RANKLIST));
		}
	}

	return true;
}

//请求抽奖
bool CHandleFromGate::On_SUB_CL_Service_RequestLottery(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//参数校验
	ASSERT(wDataSize==sizeof(STR_SUB_CL_SERVICE_REQUEST_LOTTERY));
	if(wDataSize!=sizeof(STR_SUB_CL_SERVICE_REQUEST_LOTTERY))
		return false;

	STR_SUB_CL_SERVICE_REQUEST_LOTTERY* pGetRankReward = (STR_SUB_CL_SERVICE_REQUEST_LOTTERY*)pData;

	STR_DBR_CL_SERCIVR_REQUEST_LOTTERY QueryLottery;
	memcpy(&QueryLottery,pGetRankReward,sizeof(STR_DBR_CL_SERCIVR_REQUEST_LOTTERY));

	return g_GameCtrl->PostDataBaseRequest(DBR_CL_SERVICE_REQUEST_LOTTERY,dwSocketID,&QueryLottery,sizeof(STR_DBR_CL_SERCIVR_REQUEST_LOTTERY));

}

//请求抽奖返回
bool CHandleFromGate::On_CMD_LC_Service_RequestLottery( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//参数校验
	ASSERT(wDataSize==sizeof(STR_DBO_CL_SERCIVR_REQUEST_LOTTERY));
	if(wDataSize!=sizeof(STR_DBO_CL_SERCIVR_REQUEST_LOTTERY))
		return false;

	STR_DBO_CL_SERCIVR_REQUEST_LOTTERY *pLotteryResult = (STR_DBO_CL_SERCIVR_REQUEST_LOTTERY*)pData;

	//构造抽奖数据
	STR_CMD_LC_SERVICE_REQUEST_LOTTERY LotteryResult;
	ZeroMemory(&LotteryResult,sizeof(STR_CMD_LC_SERVICE_REQUEST_LOTTERY));

	//赋值
	LotteryResult.byIndex = pLotteryResult->byIndex;
	LotteryResult.lResultCode = pLotteryResult->lResultCode;
	lstrcpyn(LotteryResult.szDescribeString, pLotteryResult->szDescribeString, CountArray(LotteryResult.szDescribeString));

	//发送数据
	g_GameCtrl->SendData(dwScoketID,MDM_SERVICE, CMD_LC_SERVICE_REQUEST_LOTTERY, &LotteryResult, sizeof(STR_CMD_LC_SERVICE_REQUEST_LOTTERY));

	return true;
}

//pure大厅排行版 查询
bool CHandleFromGate::On_SUB_CL_SERVICE_PURE_STANDING_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//参数校验
	if(wDataSize!=sizeof(STR_SUB_CL_SERVICE_PURE_STANDING_LIST))
		return false;

	return g_GameCtrl->PostDataBaseRequest(DBR_CL_SERVICE_PURE_STANDING_LIST,dwSocketID,pData, wDataSize);
}
//pure大厅排行版 返回
bool CHandleFromGate::On_CMD_LC_SERVICE_PURE_STANDING_LIST( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//参数校验
	DWORD Count = sizeof(STR_CMD_LC_SERVICE_PURE_STANDING_LIST);
	if(wDataSize<Count || (wDataSize%Count != 0))
		return false;

	//发送数据
	g_GameCtrl->SendData(dwScoketID,MDM_SERVICE, CMD_LC_SERVICE_PURE_STANDING_LIST, pData, wDataSize);

	return true;
}
//pure大厅排行版 结束
bool CHandleFromGate::On_CMD_LC_SERVICE_PURE_STANDING_FINISH( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//参数校验
	if(wDataSize!=sizeof(STR_CMD_LC_SERVICE_PURE_STANDING_FINISH))
		return false;

	STR_CMD_LC_SERVICE_PURE_STANDING_FINISH *pLotteryResult = (STR_CMD_LC_SERVICE_PURE_STANDING_FINISH*)pData;
	pLotteryResult->byMask = 1;

	//发送数据
	g_GameCtrl->SendData(dwScoketID,MDM_SERVICE, CMD_LC_SERVICE_PURE_STANDING_FINISH, pLotteryResult, sizeof(STR_CMD_LC_SERVICE_PURE_STANDING_FINISH));

	return true;
}

//pure大局战绩 查询
bool CHandleFromGate::On_SUB_CL_SERVICE_PURE_RECORD_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//参数校验
	if(wDataSize!=sizeof(STR_SUB_CL_SERVICE_PURE_RECORD_LIST))
		return false;

	g_GameCtrl->PostDataBaseRequest(DBR_CL_SERVICE_PURE_RECORD_LIST,dwSocketID,pData, wDataSize);

	return true;
}
//pure大局战绩 返回
bool CHandleFromGate::On_CMD_LC_SERVICE_PURE_RECORD_LIST( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//参数校验
	DWORD Count = sizeof(STR_CMD_LC_SERVICE_PURE_RECORD_LIST);
	if(wDataSize<Count || (wDataSize%Count != 0))
		return false;

	//发送数据
	g_GameCtrl->SendData(dwScoketID,MDM_SERVICE, CMD_LC_SERVICE_PURE_RECORD_LIST, pData, wDataSize);

	return true;
}
//pure大局玩家信息 返回
bool CHandleFromGate::On_CMD_LC_SERVICE_PURE_RECORD_LIST_PINFO( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//参数校验
	DWORD Count = sizeof(STR_CMD_LC_SERVICE_PURE_RECORD_LIST_PLAYERINFO);
	if(wDataSize<Count || (wDataSize%Count != 0))
		return false;

	//发送数据
	g_GameCtrl->SendData(dwScoketID,MDM_SERVICE, CMD_LC_SERVICE_PURE_RECORD_LIST_PINFO, pData, wDataSize);

	return true;
}
//pure大局战绩 结束
bool CHandleFromGate::On_CMD_LC_SERVICE_PURE_RECORD_FINISH( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//参数校验
	if(wDataSize!=sizeof(STR_CMD_LC_SERVICE_PURE_RECORD_LIST_FINIST))
		return false;

	STR_CMD_LC_SERVICE_PURE_RECORD_LIST_FINIST *pLotteryResult = (STR_CMD_LC_SERVICE_PURE_RECORD_LIST_FINIST*)pData;
	pLotteryResult->byMask = 1;

	//发送数据
	g_GameCtrl->SendData(dwScoketID,MDM_SERVICE, CMD_LC_SERVICE_PURE_RECORD_FINISH, pLotteryResult, sizeof(STR_CMD_LC_SERVICE_PURE_RECORD_LIST_FINIST));

	return true;
}

//pure小局战绩 查询
bool CHandleFromGate::On_SUB_CL_SERVICE_PURE_XJ_RECORD_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//参数校验
	if(wDataSize!=sizeof(STR_SUB_CL_SERVICE_PURE_XJ_RECORD_LIST))
		return false;

	return g_GameCtrl->PostDataBaseRequest(DBR_CL_SERVICE_PURE_XJ_RECORD_LIST,dwSocketID,pData, wDataSize);
}
//pure小局战绩 返回
bool CHandleFromGate::On_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//参数校验
	DWORD Count = sizeof(STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST);
	if(wDataSize<Count || (wDataSize%Count != 0))
		return false;

	STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST * pCMD = (STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST *) pData;

	//发送数据
	g_GameCtrl->SendData(dwScoketID,MDM_SERVICE, CMD_LC_SERVICE_PURE_XJ_RECORD_LIST, pData, wDataSize);

	return true;
}
//pure小局玩家信息 返回
bool CHandleFromGate::On_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_PINFO( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//参数校验
	DWORD Count = sizeof(STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_PLAYERINFO);
	if(wDataSize<Count || (wDataSize%Count != 0))
		return false;

	//发送数据
	g_GameCtrl->SendData(dwScoketID,MDM_SERVICE, CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_PINFO, pData, wDataSize);

	return true;
}
//pure小局战绩 结束
bool CHandleFromGate::On_CMD_LC_SERVICE_PURE_XJ_RECORD_FINISH( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//参数校验
	if(wDataSize!=sizeof(STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_FINISH))
		return false;

	STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_FINISH *pLotteryResult = (STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_FINISH*)pData;
	pLotteryResult->byMask = 1;

	//发送数据
	g_GameCtrl->SendData(dwScoketID,MDM_SERVICE, CMD_LC_SERVICE_PURE_XJ_RECORD_FINISH, pLotteryResult, sizeof(STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST_FINISH));

	return true;
}

//小局录像回放
bool CHandleFromGate::On_SUB_CL_Service_XJRecordPlayback(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//数据大小校验
	if ( wDataSize != sizeof(STR_SUB_CL_SERVICE_XJ_RECORD_PLAYBACK) )
		return false;

	//SUB数据
	STR_SUB_CL_SERVICE_XJ_RECORD_PLAYBACK *SUB = (STR_SUB_CL_SERVICE_XJ_RECORD_PLAYBACK *)pData;
	
	//构造DBR数据
	STR_DBR_CL_SERVICE_XJ_RECORD_PLAYBACK DBR;
	ZeroMemory(&DBR, sizeof(STR_DBR_CL_SERVICE_XJ_RECORD_PLAYBACK));
	DBR.dwRecordID = SUB->dwRecordID;

	return g_GameCtrl->PostDataBaseRequest(DBR_CL_SERVICE_XJ_RECORD_PLAYBACK, dwSocketID, &DBR, wDataSize);
}
//小局录像回放 返回
bool CHandleFromGate::On_CMD_LC_Service_XJRecordPlayback( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//参数校验
	if(wDataSize != sizeof(STR_DBO_LC_SERVICE_XJ_RECORD_PLAYBACK))
		return false;

	//DBO数据
	STR_DBO_LC_SERVICE_XJ_RECORD_PLAYBACK *pDBO = (STR_DBO_LC_SERVICE_XJ_RECORD_PLAYBACK*)pData;

	BYTE cbBuffer[LEN_MAX_RECORD_SIZE];	
	memcpy_s(cbBuffer, sizeof(cbBuffer), pDBO->cbRecordData, sizeof(cbBuffer));

	//分批发送
	for (int i = 0; i < 4; i++)
	{
		STR_CMD_LC_SERVICE_XJ_RECORD_PLAYBACK CMD;
		ZeroMemory(&CMD, sizeof(STR_CMD_LC_SERVICE_XJ_RECORD_PLAYBACK));
		memcpy_s(CMD.cbRecordData, sizeof(CMD.cbRecordData), cbBuffer+i*LEN_MAX_RECORD_SIZE/4, sizeof(CMD.cbRecordData));
		CMD.cbfinish = (i==3) ? 1 : 0;

		//发送数据
		g_GameCtrl->SendData(dwScoketID, MDM_SERVICE, CMD_LC_SERVICE_XJ_RECORD_PLAYBACK, &CMD, sizeof(CMD));

	}

	return true;
}

//客服消息
bool CHandleFromGate::On_SUB_CL_SERVICE_CUSTOMER_MESSEGE(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//数据大小校验
	if ( wDataSize != sizeof(STR_SUB_CL_SERVICE_CUSTOMER_MESSEGE) )
		return false;

	//SUB数据
	STR_SUB_CL_SERVICE_CUSTOMER_MESSEGE *SUB = (STR_SUB_CL_SERVICE_CUSTOMER_MESSEGE *)pData;
	
	//构造DBR数据
	STR_DBR_CL_SERVICE_CUSTOMER_MESSEGE DBR;
	ZeroMemory(&DBR, sizeof(STR_DBR_CL_SERVICE_CUSTOMER_MESSEGE));
	DBR.cbMessegeFlag = SUB->cbMessegeFlag;

	return g_GameCtrl->PostDataBaseRequest(DBR_CL_SERVICE_CUSTOMER_MESSEGE, dwSocketID, &DBR, sizeof(DBR));
}
//客服消息 返回
bool CHandleFromGate::On_CMD_LC_SERVICE_CUSTOMER_MESSEGE( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//参数校验
	if(wDataSize != sizeof(STR_DBO_LC_SERVICE_CUSTOMER_MESSEGE))
		return false;

	//DBO数据
	STR_DBO_LC_SERVICE_CUSTOMER_MESSEGE *pDBO = (STR_DBO_LC_SERVICE_CUSTOMER_MESSEGE*)pData;

	//构造CMD数据
	STR_CMD_LC_SERVICE_CUSTOMER_MESSEGE CMD;
	ZeroMemory(&CMD, sizeof(STR_CMD_LC_SERVICE_CUSTOMER_MESSEGE));
	CMD.cbMessegeFlag = pDBO->cbMessegeFlag;
	lstrcpyn(CMD.szMessege, pDBO->szMessege, CountArray(CMD.szMessege));

	//发送数据
	g_GameCtrl->SendData(dwScoketID, MDM_SERVICE, CMD_LC_SERVICE_CUSTOMER_MESSEGE, &CMD, sizeof(STR_CMD_LC_SERVICE_CUSTOMER_MESSEGE));

	return true;
}

//请求金币大厅信息
bool CHandleFromGate::On_SUB_CL_SERVICE_GOLD_INFO(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//参数校验
	if(wDataSize!=sizeof(STR_SUB_CL_SERVICE_GOLD_INFO))
		return false;

	return g_GameCtrl->PostDataBaseRequest(DBR_CL_SERVICE_GOLD_INFO,dwSocketID,pData, wDataSize);
}
//请求金币大厅信息 返回
bool CHandleFromGate::On_CMD_LC_SERVICE_GOLD_INFO( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//参数校验
	DWORD Count = sizeof(STR_CMD_LC_SERVICE_GOLD_INFO);
	if(wDataSize<Count || (wDataSize%Count != 0))
		return false;

	//发送数据
	g_GameCtrl->SendData(dwScoketID,MDM_SERVICE, CMD_LC_SERVICE_GOLD_INFO, pData, wDataSize);

	return true;
}
//请求金币大厅信息 结束
bool CHandleFromGate::On_CMD_LC_SERVICE_GOLD_INFO_FINISH( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//参数校验
	if(wDataSize!=sizeof(STR_CMD_LC_SERVICE_GOLD_INFO_FINISH))
		return false;

	STR_CMD_LC_SERVICE_GOLD_INFO_FINISH *pLotteryResult = (STR_CMD_LC_SERVICE_GOLD_INFO_FINISH*)pData;
	pLotteryResult->byMask = 1;

	//发送数据
	g_GameCtrl->SendData(dwScoketID,MDM_SERVICE, CMD_LC_SERVICE_GOLD_INFO_FINISH, pLotteryResult, sizeof(STR_CMD_LC_SERVICE_GOLD_INFO_FINISH));

	return true;
}

//修改个人资料
bool CHandleFromGate::On_SUB_CL_Service_ModifyPersonalInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验
	if (wDataSize != sizeof(STR_SUB_CL_SERVICE_MODIFY_PERSONAL_INFO)) return false;

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_SERVICE_MODIFY_PERSONAL_INFO, dwSocketID, pData, wDataSize);

	return true;
}

//修改个人资料返回
bool CHandleFromGate::On_CMD_LC_Service_ModifyPersonalInfo( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//参数校验
	if(wDataSize!=sizeof(STR_DBO_CL_MODIFY_PERSONL_INFO))
		return false;

	STR_DBO_CL_MODIFY_PERSONL_INFO *pModifyInfo = (STR_DBO_CL_MODIFY_PERSONL_INFO*)pData;

	//构造数据
	STR_CMD_LC_SERVICE_MODIFY_PERSONL_INFO PersonalInfo;
	ZeroMemory(&PersonalInfo,sizeof(STR_CMD_LC_SERVICE_MODIFY_PERSONL_INFO));
	CopyMemory(&PersonalInfo, pModifyInfo, sizeof(PersonalInfo));

	//发送数据
	g_GameCtrl->SendData(dwScoketID, MDM_SERVICE, CMD_LC_SERVICE_MODIFY_PERSONAL_INFO, &PersonalInfo, sizeof(STR_CMD_LC_SERVICE_MODIFY_PERSONL_INFO));

	return true;
}

//查询金币房卡返回
bool CHandleFromGate::On_CMD_LC_Service_QueryScoreInfo(DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//变量定义
	STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO * pScoreInfo =(STR_DBO_CL_SERCIVR_QUERY_SCORE_INFO *)pData;

	//变量定义
	STR_CMD_LC_QUERY_SCORE_INFO ScoreInfo;
	ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));

	//描述信息
	ScoreInfo.lResultCode = pScoreInfo->lResultCode;
	lstrcpyn(ScoreInfo.szDescribeString, pScoreInfo->szDescribeString, CountArray(ScoreInfo.szDescribeString));

	//财富信息
	ScoreInfo.dwUserID = pScoreInfo->dwUserID;
	ScoreInfo.lGold = pScoreInfo->lGold;
	ScoreInfo.lOpenRoomCard = pScoreInfo->lOpenRoomCard;
	ScoreInfo.lDiamond = pScoreInfo->lDiamond;

	//发送数据
	g_GameCtrl->SendData(dwScoketID, MDM_SERVICE, CMD_LC_USER_QUERY_SCORE_INFO, &ScoreInfo, sizeof(STR_CMD_LC_QUERY_SCORE_INFO));

	return true;
}

//公共操作结果
bool CHandleFromGate::On_CMD_LC_CommonOperateResult( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//变量定义
	STR_CMD_LC_OTHER_OPERATE_RESULT OperateResult;
	ZeroMemory(&OperateResult,sizeof(OperateResult));

	//变量定义
	DBO_GP_OperateCommand * pOperate=(DBO_GP_OperateCommand *)pData;

	//构造数据
	OperateResult.lResultCode=pOperate->lResultCode;

	//发送数据
	WORD wHeadSize=sizeof(OperateResult);
	g_GameCtrl->SendData(dwScoketID,pOperate->mCommand.MainCommand,pOperate->mCommand.SubCommand,&OperateResult,wHeadSize);
	return true;
}

//领取任务返回
bool CHandleFromGate::OnDBRankRewardResult( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//参数校验
	ASSERT(wDataSize==sizeof(STR_DBO_CL_SERCIVR_GET_RANK_REWARD));
	if(wDataSize!=sizeof(STR_DBO_CL_SERCIVR_GET_RANK_REWARD))
		return false;

	STR_DBO_CL_SERCIVR_GET_RANK_REWARD *pRankDone = (STR_DBO_CL_SERCIVR_GET_RANK_REWARD*)pData;

	STR_CMD_LC_SERVICE_GET_RANK_REWARD RankDone;
	ZeroMemory(&RankDone,sizeof(STR_CMD_LC_SERVICE_GET_RANK_REWARD));
	memcpy(&RankDone,pRankDone,sizeof(STR_CMD_LC_SERVICE_GET_RANK_REWARD));

	g_GameCtrl->SendData(dwScoketID,MDM_SERVICE,CMD_LC_SERVICE_GET_RANK_REWARD,&RankDone,sizeof(STR_CMD_LC_SERVICE_GET_RANK_REWARD));

	return true;
}
#pragma endregion

#pragma region MDM_CLUB 牌友圈(版本2)
//查询牌友圈列表
bool CHandleFromGate::On_SUB_CL_CLUB_ALL_CLUB_INFO_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_ALL_CLUB_INFO_LIST)) return false;

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_ALL_CLUB_INFO_LIST, dwSocketID, pData, wDataSize);

	return true;
}
//查询牌友圈列表结果
bool CHandleFromGate::On_CMD_LC_CLUB_ALL_CLUB_INFO_LIST( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_ALL_CLUB_INFO_LIST);
	if( (wDataSize % Size) != 0) return false;

	//处理消息
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_ALL_CLUB_INFO_LIST, pData, wDataSize);
	return true;
}
//查询牌友圈列表结束
bool CHandleFromGate::On_CMD_LC_CLUB_ALL_INFO_FINISH( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	STR_CMD_LC_CLUB_ALL_INFO_FINISH cmd;
	cmd.byMask = 1;

	//处理消息
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_ALL_INFO_FINISH, &cmd, sizeof(STR_CMD_LC_CLUB_ALL_INFO_FINISH));
	return true;
}

//查询指定牌友圈房间列表
bool CHandleFromGate::On_SUB_CL_CLUB_ROOM_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_ROOM_LIST)) return false;

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_ROOM_LIST, dwSocketID, pData, wDataSize);
	return true;
}
//查询指定牌友圈房间列表 返回
bool CHandleFromGate::On_CMD_LC_CLUB_ROOM_LIST( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_ROOM_LIST);
	if( (wDataSize % Size ) != 0) return false;

	//处理消息
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_ROOM_LIST, pData, wDataSize);
	return true;
}
//查询指定牌友圈房间列表 结束
bool CHandleFromGate::On_CMD_LC_CLUB_ROOM_LIST_FINISH( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	STR_CMD_LC_CLUB_ROOM_LIST_FINISH cmd;
	cmd.byMask = 1;

	//处理消息
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_ROOM_LIST_FINISH, &cmd, sizeof(STR_CMD_LC_CLUB_ROOM_LIST_FINISH));
	return true;
}

//查询未满员, 随机牌友圈(最大9个)
bool CHandleFromGate::On_SUB_CL_CLUB_RANDOM_CLUB_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_RANDOM_CLUB_LIST)) return false;

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_RANDOM_CLUB_LIST, dwSocketID, pData, wDataSize);
	return true;
}
//查询未满员, 随机牌友圈(最大9个) 返回
bool CHandleFromGate::On_CMD_LC_CLUB_RANDOM_CLUB_LIST( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_RANDOM_CLUB_LIST);
	if( (wDataSize % Size) != 0) return false;

	//处理消息
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_RANDOM_CLUB_LIST, pData, wDataSize);
	return true;
}
//查询未满员, 随机牌友圈(最大9个) 结束
bool CHandleFromGate::On_CMD_LC_CLUB_RANDOM_CLUB_LIST_FINISH( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	STR_CMD_LC_CLUB_RANDOM_CLUB_LIST_FINISH cmd;
	cmd.byMask = 1;

	//处理消息
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_RANDOM_CLUB_LIST_FINISH, &cmd, sizeof(STR_CMD_LC_CLUB_RANDOM_CLUB_LIST_FINISH));
	return true;
}

//申请加入牌友圈
bool CHandleFromGate::On_SUB_CL_CLUB_JOIN_CLUB(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_JOIN_CLUB)) return false;

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_JOIN_CLUB, dwSocketID, pData, wDataSize);
	return true;
}
//申请加入牌友圈返回 
bool CHandleFromGate::On_CMD_LC_CLUB_JOIN_CLUB( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_JOIN_CLUB);
	if( wDataSize != Size) return false;

	//处理消息
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_JOIN_CLUB, pData, wDataSize);
	return true;
}
//申请加入牌友圈广播
bool CHandleFromGate::On_CMD_LC_CLUB_JOIN_CLUB_BDCAST( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_SUB_CL_CLUB_JOIN_CLUB_BDCAST);
	if( wDataSize != Size) return false;

	//TODONOW 暂时是每个人都发送，后面改为 1.特定俱乐部的  2.会长和管理员发送
	g_GameCtrl->SendDataBatch(MDM_CLUB, CMD_LC_CLUB_JOIN_CLUB_BDCAST, pData, wDataSize);
	return true;
}
//申请加入牌友圈 通知客户端实时刷新
bool CHandleFromGate::On_CMD_LC_CLUB_JOIN_CLUB_RE( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_LIST_RE);
	if( wDataSize != Size) return false;

	//处理消息
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_LIST_RE, pData, wDataSize);
	return true;
}


//解散牌友圈
bool CHandleFromGate::On_SUB_CL_CLUB_DISS_CLUB(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_DISS_CLUB)) return false;

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_DISS_CLUB, dwSocketID, pData, wDataSize);
	return true;
}
//解散牌友圈 返回 
bool CHandleFromGate::On_CMD_LC_CLUB_DISS_CLUB( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_DISS_CLUB);
	if( wDataSize != Size) return false;

	//处理消息
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_DISS_CLUB, pData, wDataSize);
	return true;
}

//创建牌友圈
bool CHandleFromGate::On_SUB_CL_CLUB_CREATE_CLUB(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_CREATE_CLUB)) return false;

	//处理消息
	STR_SUB_CL_CLUB_CREATE_CLUB * pSub = (STR_SUB_CL_CLUB_CREATE_CLUB *)pData;
	//定义变量
	STR_DBR_CL_CLUB_CREATE_CLUB Dbr;
	ZeroMemory(&Dbr,sizeof(Dbr));

	Dbr.dwUserID = pSub->dwUserID;
	lstrcpyn(Dbr.szClubName, pSub->szClubName, CountArray(Dbr.szClubName));
	lstrcpyn(Dbr.szNotice, pSub->szNotice, CountArray(Dbr.szNotice));
	lstrcpyn(Dbr.szMessag, pSub->szMessag, CountArray(Dbr.szMessag));
	Dbr.dwMajorKindID = pSub->dwMajorKindID;

	//构造信息
	for (WORD i=0; i<10; i++)
	{
		INT nLength=lstrlen(Dbr.szMinorKindID);
		_sntprintf(&Dbr.szMinorKindID[nLength],CountArray(Dbr.szMinorKindID)-nLength,
			TEXT("%ld,"),
			pSub->dwMinorKindID[i]);
	}

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_CREATE_CLUB, dwSocketID, &Dbr, sizeof(Dbr));
	return true;
}
//创建牌友圈 返回
bool CHandleFromGate::On_CMD_LC_CLUB_CREATE_CLUB( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_CREATE_CLUB);
	if( wDataSize != Size ) return false;

	//处理消息
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_CREATE_CLUB, pData, wDataSize);
	return true;
}

//房间设置
bool CHandleFromGate::On_SUB_CL_CLUB_ROOM_SETTING(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_ROOM_SETTING)) return false;

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_ROOM_SETTING, dwSocketID, pData, wDataSize);
	return true;
}
//房间设置 返回
bool CHandleFromGate::On_CMD_LC_CLUB_ROOM_SETTING( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_ROOM_SETTING);
	if( wDataSize != Size) return false;

	//处理消息
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_ROOM_SETTING, pData, wDataSize);
	return true;
}

//请求房间设置
bool CHandleFromGate::On_SUB_CL_CLUB_ROOM_QUERY_SETTING(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_ROOM_QUERY_SETTING)) return false;

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_ROOM_QUERY_SETTING, dwSocketID, pData, wDataSize);
	return true;
}
//请求房间设置 返回
bool CHandleFromGate::On_CMD_LC_CLUB_ROOM_QUERY_SETTING( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_ROOM_QUERY_SETTING);
	if( wDataSize != Size) return false;

	//处理消息
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_ROOM_QUERY_SETTING, pData, wDataSize);
	return true;
}


//玩家离开俱乐部房间
bool CHandleFromGate::On_SUB_CL_CLUB_ROOM_USER_LEAVE(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_ROOM_USER_LEAVE)) return false;

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_ROOM_USER_LEAVE, dwSocketID, pData, wDataSize);
	return true;
}

//解散房间请求
bool CHandleFromGate::On_SUB_CL_CLUB_ROOM_DISSOLVE(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_ROOM_DISSOLVE)) return false;

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_ROOM_DISSOLVE, dwSocketID, pData, wDataSize);
	return true;
}
//解散房间请求 返回
bool CHandleFromGate::On_CMD_LC_CLUB_ROOM_DISSOLVE( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_ROOM_DISSOLVE);
	if( wDataSize != Size) return false;

	//处理消息
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_ROOM_DISSOLVE, pData, wDataSize);
	return true;
}

//解散房间请求
bool CHandleFromGate::On_SUB_CL_CLUB_TABLE_DISSOLVE(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_TABLE_DISSOLVE)) return false;

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_TABLE_DISSOLVE, dwSocketID, pData, wDataSize);
	return true;
}
//解散房间请求 返回
bool CHandleFromGate::On_CMD_LC_CLUB_TABLE_DISSOLVE( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_TABLE_DISSOLVE);
	if( wDataSize != Size) return false;

	STR_CMD_LC_CLUB_TABLE_DISSOLVE* pCmd = (STR_CMD_LC_CLUB_TABLE_DISSOLVE *) pData;

	//处理消息
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_TABLE_DISSOLVE, pData, wDataSize);

	if( (DB_SUCCESS ==  pCmd->lResultCode) && (0 == pCmd->byMask) && (0 != pCmd->dwGameID))
	{
		//通知协调服, 协调服通知游戏服
		STR_CPR_LP_CLUB_TABLE_DISSOLVE CPR;
		CPR.dwGameID = pCmd->dwGameID;
		CPR.dwTableID = pCmd->dwTableID;

		g_TCPSocketEngine->SendData(CPD_MDM_TRANSFER, CPR_LP_CLUB_TABLE_DISSOLVE, &CPR, sizeof(STR_CPR_LP_CLUB_TABLE_DISSOLVE));
	}
	return true;
}


//牌友圈公告
bool CHandleFromGate::On_SUB_CL_CLUB_NOTICE(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_NOTICE)) return false;

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_NOTICE, dwSocketID, pData, wDataSize);
	return true;
}
//牌友圈公告 返回
bool CHandleFromGate::On_CMD_LC_CLUB_NOTICE( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_NOTICE);
	if( wDataSize != Size) return false;

	//处理消息
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_NOTICE, pData, wDataSize);
	return true;
}
//牌友圈公告 广播
bool CHandleFromGate::On_CMD_LC_CLUB_NOTICE_BDCAST( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//TODONOW 需要实现
	return true;
}

//聊天
bool CHandleFromGate::On_SUB_CL_CLUB_CHAT(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_CHAT)) return false;
	STR_SUB_CL_CLUB_CHAT *pSub = (STR_SUB_CL_CLUB_CHAT*)pData;

	if(0 == pSub->byChatMode) //俱乐部聊天
	{
		g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_CHAT, dwSocketID, pData, wDataSize);
	}
	else if( 1 == pSub->byChatMode) //世界聊天
	{
		g_GameCtrl->PostDataBaseRequest(DBR_CL_WORD_CHAT, dwSocketID, pData, wDataSize);
	}
	else if( 2 == pSub->byChatMode) //系统聊天
	{
		g_GameCtrl->PostDataBaseRequest(DBR_CL_SYSTEM_CHAT, dwSocketID, pData, wDataSize);
	}
	else if( 3 == pSub->byChatMode) //私聊
	{
		g_GameCtrl->PostDataBaseRequest(DBR_CL_SECRET_CHAT, dwSocketID, pData, wDataSize);
	}

	return true;
}
//聊天 返回
bool CHandleFromGate::On_CMD_LC_CLUB_CHAT( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_CHAT);
	if( wDataSize != Size) return false;

	//处理消息
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_CHAT, pData, wDataSize);
	return true;
}
//牌友圈聊天 广播
bool CHandleFromGate::On_CMD_LC_CLUB_CHAT_BDCAST( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	WORD Size = sizeof(STR_CMD_LC_CLUB_CHAT_BDCAST);
	if( (wDataSize < Size) || (( wDataSize % Size) != 0 )) return false;
	STR_CMD_LC_CLUB_CHAT_BDCAST *pCmd = (STR_CMD_LC_CLUB_CHAT_BDCAST*) pData;

	switch(pCmd->byChatMode)
	{
	case 0: //俱乐部聊天
		{
			auto vec_player = CPlayerManager::FindPlayerByClubID(pCmd->dwClubID);

			for(CPlayer* player : vec_player)
			{
				if(! player) continue;

				g_GameCtrl->SendData(player->GetSocketID(), MDM_CLUB, CMD_LC_CLUB_CHAT_BDCAST, 
					pCmd, sizeof(STR_CMD_LC_CLUB_CHAT_BDCAST));
			}

			break;
		}
	case 1: //世界聊天
		{
			g_GameCtrl->SendDataBatch(MDM_CLUB, CMD_LC_CLUB_CHAT_BDCAST, pData, wDataSize);
			break;
		}
	case 2: //系统聊天
		{
			break;
		}
	case 3: //私密聊天
		{
			auto player = CPlayerManager::FindPlayerByID(pCmd->dwTagID);
			if(!player) return false;

			g_GameCtrl->SendData(player->GetSocketID(), MDM_CLUB, CMD_LC_CLUB_CHAT_BDCAST, 
						pData, wDataSize);

			break;
		}
	}
	
	return true;
}

//牌友圈置顶
bool CHandleFromGate::On_SUB_CL_CLUBSTICKY_POST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_STICKY_POST)) return false;
	STR_SUB_CL_CLUB_STICKY_POST *pSub = (STR_SUB_CL_CLUB_STICKY_POST*)pData;

	//构造DBR数据
	STR_DBR_CL_CLUB_STICKY_POST DBR;
	ZeroMemory(&DBR, sizeof(STR_DBR_CL_CLUB_STICKY_POST));
	DBR.dwClubID = pSub->dwClubID;
	DBR.cbTopFlag = pSub->cbTopFlag;
	DBR.dwUserID = pSub->dwUserID;

	//发送请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_STICKY_POST, dwSocketID, pData, wDataSize);

	return true;
}
//牌友圈置顶 返回
bool CHandleFromGate::On_CMD_LC_CLUB_STICKY_POST( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//校验参数
	if( wDataSize != sizeof(STR_CMD_LC_CLUB_STICKY_POST)) return false;

	//构造CMD数据
	STR_DBO_LC_CLUB_STICKY_POST *DBO = (STR_DBO_LC_CLUB_STICKY_POST *)pData;

	STR_CMD_LC_CLUB_STICKY_POST CMD;
	ZeroMemory(&CMD, sizeof(STR_CMD_LC_CLUB_STICKY_POST));
	CMD.lResultCode = DBO->lResultCode;

	//发送返回
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_STICKY_POST, &CMD, sizeof(CMD));

	return true;
}

//牌友圈简介
bool CHandleFromGate::On_SUB_CL_CLUB_MESSAGE(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_MESSAGE)) return false;

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_MESSAGE, dwSocketID, pData, wDataSize);
	return true;
}
//牌友圈简介 返回
bool CHandleFromGate::On_CMD_LC_CLUB_MESSAGE( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_MESSAGE);
	if( wDataSize != Size) return false;

	//处理消息
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_MESSAGE, pData, wDataSize);
	return true;
}

//贡献房卡
bool CHandleFromGate::On_SUB_CL_CLUB_CONTRIBUTE_FK(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_CONTRIBUTE_FK)) return false;

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_CONTRIBUTE_FK, dwSocketID, pData, wDataSize);
	return true;
}
//贡献房卡 返回
bool CHandleFromGate::On_CMD_LC_CLUB_CONTRIBUTE_FK( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_CONTRIBUTE_FK);
	if( wDataSize != Size) return false;

	//处理消息
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_CONTRIBUTE_FK, pData, wDataSize);
	return true;
}

//牌友圈设置
bool CHandleFromGate::On_SUB_CL_CLUB_AUTO_AGREE(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_AUTO_AGREE)) return false;

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_AUTO_AGREE, dwSocketID, pData, wDataSize);
	return true;
}
//牌友圈设置 返回
bool CHandleFromGate::On_CMD_LC_CLUB_AUTO_AGREE( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_AUTO_AGREE);
	if( wDataSize != Size) return false;

	//处理消息
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_AUTO_AGREE, pData, wDataSize);
	return true;
}

//申请加入房间
bool CHandleFromGate::On_SUB_CL_CLUB_JOIN_ROOM(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_JOIN_ROOM)) return false;

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_JOIN_ROOM, dwSocketID, pData, wDataSize);
	return true;
}
//申请加入房间 返回
bool CHandleFromGate::On_CMD_LC_CLUB_JOIN_ROOM( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_JOIN_ROOM);
	if( wDataSize != Size) return false;

	//处理消息
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_JOIN_ROOM, pData, wDataSize);
	return true;
}
//桌子列表
bool CHandleFromGate::On_CMD_LC_CLUB_TABLE_LIST_TABLE( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_TABLE_LIST);
	if( (wDataSize % Size) != 0) return false;

	//处理消息
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_TABLE_LIST_TABLE, pData, wDataSize);
	return true;
}
//桌子玩家列表
bool CHandleFromGate::On_CMD_LC_CLUB_TABLE_LIST_USER( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_TABLE_USER_LIST);
	if( (wDataSize % Size) != 0) return false;

	//处理消息
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_TABLE_LIST_USER, pData, wDataSize);
	return true;
}



//群主|管理对申请消息的答复(同意|拒绝)
bool CHandleFromGate::On_SUB_CL_CLUB_APPLICANT_RESULT(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_APPLICANT_RESULT)) return false;

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_APPLICANT_RESULT, dwSocketID, pData, wDataSize);
	return true;
}
//群主|管理对申请消息的答复(同意|拒绝) 返回
bool CHandleFromGate::On_CMD_LC_CLUB_APPLICANT_RESULT( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_APPLICANT_RESULT);
	if( wDataSize != Size) return false;

	//处理消息
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_APPLICANT_RESULT, pData, wDataSize);
	return true;
}

//请求成员数据
bool CHandleFromGate::On_SUB_CL_CLUB_MEMBER_MANAGER(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_MEMBER_MANAGER)) return false;

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_MEMBER_MANAGER, dwSocketID, pData, wDataSize);
	return true;
}
//请求成员数据 返回
bool CHandleFromGate::On_CMD_LC_CLUB_MEMBER_MANAGER( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_MEMBER_MANAGER);
	if( (wDataSize % Size) != 0) return false;

	//处理消息
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_MEMBER_MANAGER, pData, wDataSize);
	return true;
}
//请求成员数据 结束
bool CHandleFromGate::On_CMD_LC_CLUB_MEMBER_MANAGER_FINISH( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	STR_CMD_LC_CLUB_MEMBER_MANAGER_FINISH cmd;
	cmd.byMask = 1;

	//处理消息
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_MEMBER_MANAGER_FINISH, &cmd, sizeof(cmd));
	return true;
}
//工会基本信息
bool CHandleFromGate::On_CMD_LC_CLUB_DATA( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_DATA);
	if( wDataSize != Size) return false;

	//处理消息
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_DATA, pData, wDataSize);
	return true;
}

//邀请他入加入牌友圈
bool CHandleFromGate::On_SUB_CL_CLUB_INVITE(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_INVITE)) return false;

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_INVITE, dwSocketID, pData, wDataSize);

	return true;
}
//邀请他入加入牌友圈 返回
bool CHandleFromGate::On_CMD_LC_CLUB_INVITE( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_INVITE);
	if( wDataSize != Size) return false;

	//处理消息
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_INVITE, pData, wDataSize);

	return true;
}

//被邀请人的提醒 
bool CHandleFromGate::On_CMD_LC_CLUB_INVITE_REMIND( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_INVITE_REMIND);
	if( wDataSize != Size) return false;
	STR_CMD_LC_CLUB_INVITE_REMIND * pCMD = (STR_CMD_LC_CLUB_INVITE_REMIND*) pData;

	auto player = CPlayerManager::FindPlayerByID(pCMD->dwTagID);
	if( !player) return false;

	//通知用户
	g_GameCtrl->SendData(player->GetSocketID(), MDM_CLUB, CMD_LC_CLUB_INVITE_REMIND, 
		pData, wDataSize);	

	return true;
}

//被邀请人的回复
bool CHandleFromGate::On_SUB_CL_CLUB_INVITE_RESULT(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_INVITE_RESULT)) return false;

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_INVITE_RESULT, dwSocketID, pData, wDataSize);

	return true;
}
//被邀请人的回复 返回 
bool CHandleFromGate::On_CMD_LC_CLUB_INVITE_RESULT( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_INVITE_RESULT);
	if( wDataSize != Size) return false;

	STR_CMD_LC_CLUB_INVITE_RESULT *pCMD = (STR_CMD_LC_CLUB_INVITE_RESULT*) pData;

	//如果返回成功, 则通知客户端刷新工会列表
	if(pCMD->lResultCode == DB_SUCCESS)
	{
		STR_CMD_LC_CLUB_LIST_RE CMD;
		CMD.byMask = 1;

		g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_LIST_RE, &CMD, sizeof(CMD));
	}

	//处理消息
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_INVITE_RESULT, pData, wDataSize);

	return true;
}

//踢出退出请求
bool CHandleFromGate::On_SUB_CL_CLUB_QUIT(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_QUIT)) return false;

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_QUIT, dwSocketID, pData, wDataSize);
	return true;
}
//用户退出请求 返回
bool CHandleFromGate::On_CMD_LC_CLUB_QUIT( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_QUIT);
	if( wDataSize != Size) return false;

	//处理消息
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_QUIT, pData, wDataSize);
	return true;
}

//职务任免
bool CHandleFromGate::On_SUB_CL_CLUB_APPOINTMENT(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_APPOINTMENT)) return false;

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_APPOINTMENT, dwSocketID, pData, wDataSize);
	return true;
}
//职务任免 返回
bool CHandleFromGate::On_CMD_LC_CLUB_APPOINTMENT( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_APPOINTMENT);
	if( wDataSize != Size) return false;

	//处理消息
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_APPOINTMENT, pData, wDataSize);
	return true;
}
//职务任免 提醒
bool CHandleFromGate::On_CMD_LC_CLUB_APPOINTMENT_NOTE( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//校验参数
	if( wDataSize != sizeof(STR_CMD_LC_CLUB_APPOINTMENT_NOTE)) return false;
	STR_CMD_LC_CLUB_APPOINTMENT_NOTE * pCMD = (STR_CMD_LC_CLUB_APPOINTMENT_NOTE*) pData;

	auto player = CPlayerManager::FindPlayerByID(pCMD->dwUserID);
	if(!player) return false;
	
	//通知用户
	g_GameCtrl->SendData(player->GetSocketID(), MDM_CLUB, CMD_LC_CLUB_APPOINTMENT_NOTE, 
		pData, wDataSize);		

	return true;
}


//申请人列表
bool CHandleFromGate::On_SUB_CL_CLUB_APPLICANT_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_APPLICANT_LIST)) return false;

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_APPLICANT_LIST, dwSocketID, pData, wDataSize);
	return true;
}
//申请人列表 返回
bool CHandleFromGate::On_CMD_LC_CLUB_APPLICANT_LIST( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_APPLICANT_LIST);
	if( (wDataSize % Size) != 0) return false;

	//处理消息
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_APPLICANT_LIST, pData, wDataSize);
	return true;
}
//申请人列表 结束
bool CHandleFromGate::On_CMD_LC_CLUB_APPLICANT_LIST_FINISH( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	STR_CMD_LC_CLUB_APPLICANT_LIST_FINISH cmd;
	cmd.byMask = 1;

	//处理消息
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_APPLICANT_LIST_FINISH, &cmd, sizeof(cmd));
	return true;
}

//被邀请人查看自己的邀请列表
bool CHandleFromGate::On_SUB_CL_CLUB_INQUERY_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_INQUERY_LIST)) return false;

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_INQUERY_LIST, dwSocketID, pData, wDataSize);
	return true;
}
//被邀请人查看自己的邀请列表 返回
bool CHandleFromGate::On_CMD_LC_CLUB_INQUERY_LIST( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_INQUERY_LIST);
	if( (wDataSize % Size) != 0) return false;

	//处理消息
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_INQUERY_LIST, pData, wDataSize);
	return true;
}
//被邀请人查看自己的邀请列表 结束
bool CHandleFromGate::On_CMD_LC_CLUB_INQUERY_LIST_FINISH( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	STR_CMD_LC_CLUB_INQUERY_LIST_FINISH cmd;
	cmd.byMask = 1;

	//处理消息
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_INQUERY_LIST_FINISH, &cmd, sizeof(cmd));
	return true;
}


//工会战绩统计
bool CHandleFromGate::On_SUB_CL_CLUB_RECORD_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_RECORD_LIST)) return false;

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_RECORD_LIST, dwSocketID, pData, wDataSize);
	return true;
}
//工会战绩统计 返回
bool CHandleFromGate::On_CMD_LC_CLUB_RECORD_LIST( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_CMD_LC_CLUB_RECORD_LIST);
	if( (wDataSize < Size) || 
		( (wDataSize % Size) != 0))
		return false;

	//处理消息
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_RECORD_LIST, pData, wDataSize);
	return true;
}
//工会战绩统计 结束
bool CHandleFromGate::On_CMD_LC_CLUB_RECORD_FINISH( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	STR_CMD_LC_CLUB_RECORD_FINISH cmd;
	cmd.byMask = 1;

	//处理消息
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_RECORD_FINISH, &cmd, sizeof(cmd));
	return true;
}

#pragma endregion

#pragma region 礼物道具
//赠送道具
bool CHandleFromGate::On_SUB_CL_GIFT_GIVE_PROPS(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//参数校验
	if(wDataSize!=sizeof(STR_SUB_CL_GIFT_GIVE_PROPS))
		return false;

	return g_GameCtrl->PostDataBaseRequest(DBR_CL_GIFT_GIVE_PROPS,dwSocketID, pData, wDataSize);
}

//赠送道具返回
bool CHandleFromGate::On_CMD_LC_GIFT_GIVE_PROPS( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//参数校验
	if(wDataSize!=sizeof(STR_CMD_LC_GIFT_GIVE_PROPS))
		return false;

	g_GameCtrl->SendData(dwScoketID, MDM_GIFT, CMD_LC_GIFT_GIVE_PROPS, pData, wDataSize);

	return true;
}

//赠送道具 通知接受人
bool CHandleFromGate::On_CMD_LC_GIFT_GIVE_PROPS_SHOW( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//参数校验
	if(wDataSize!=sizeof(STR_CMD_LC_GIFT_GIVE_PROPS_SHOW)) return false;
		
	//变量定义
	STR_CMD_LC_GIFT_GIVE_PROPS_SHOW *pCMD = (STR_CMD_LC_GIFT_GIVE_PROPS_SHOW*) pData;

	auto player = CPlayerManager::FindPlayerByID(pCMD->dwTargetID);
	if(!player) return false;

	//通知用户
	g_GameCtrl->SendData(player->GetSocketID(), MDM_GIFT, CMD_LC_GIFT_GIVE_PROPS_SHOW, 
		pData, wDataSize);	

	return true;
}


//用户充值信息
bool CHandleFromGate::On_SUB_CL_Other_ReChargeInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	ASSERT( wDataSize == sizeof(STR_SUB_CL_OTHER_RECHARGE_INFO));
	if(wDataSize != sizeof(STR_SUB_CL_OTHER_RECHARGE_INFO)) return false;

	//处理消息
	STR_SUB_CL_OTHER_RECHARGE_INFO * pUserRequest = (STR_SUB_CL_OTHER_RECHARGE_INFO *)pData;
	//定义变量
	STR_DBR_CL_OTHER_RECHARGE_INFO UserRequest;
	ZeroMemory(&UserRequest,sizeof(UserRequest));

	UserRequest.dwUserID = pUserRequest->dwUserID;

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_USER_RECHARGE_INFO,dwSocketID,&UserRequest,sizeof(UserRequest));
	return true;
}

//用户充值信息返回
bool CHandleFromGate::On_CMD_LC_Other_RechargeInfo( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//变量定义
	STR_CMD_LC_OTHER_RECHARGE_INFO RechangeInfo;
	ZeroMemory(&RechangeInfo,sizeof(RechangeInfo));

	//变量定义
	STR_DBO_CL_OTHER_RECHARGE_INFO * pRechangeInfo = (STR_DBO_CL_OTHER_RECHARGE_INFO *)pData;

	//构造数据
	RechangeInfo.dwMinMoney = pRechangeInfo->dwMinMoney;
	RechangeInfo.dwChangeScale = pRechangeInfo->dwChangeScale;

	//发送数据
	WORD wHeadSize=sizeof(RechangeInfo);
	g_GameCtrl->SendData(dwScoketID, MDM_SERVICE, CMD_LC_OTHERS_RECHARGE_INFO, &RechangeInfo, wHeadSize);
	return true;
}




//兑换道具
bool CHandleFromGate::On_SUB_CL_Other_ExchangeInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	ASSERT( wDataSize == sizeof(STR_SUB_CL_OTHER_EXCHANGE_INFO));
	if(wDataSize != sizeof(STR_SUB_CL_OTHER_EXCHANGE_INFO)) return false;

	//处理消息
	STR_SUB_CL_OTHER_EXCHANGE_INFO * pUserRequest = (STR_SUB_CL_OTHER_EXCHANGE_INFO *)pData;
	//定义变量
	STR_DBR_CL_OTHER_EXCHANGE_INFO UserRequest;
	ZeroMemory(&UserRequest,sizeof(UserRequest));

	UserRequest.dwUserID = pUserRequest->dwUserID;

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_USER_EXCHANGE_INFO, dwSocketID, &UserRequest, sizeof(UserRequest));
	return true;
}

//兑换道具返回
bool CHandleFromGate::On_CMD_LC_Other_ExchangeInfo( DWORD dwScoketID, VOID * pData, WORD wDataSize )
{
	//变量定义
	STR_DBO_CL_OTHER_EXCHANGE_INFO * pExchangeInfo=(STR_DBO_CL_OTHER_EXCHANGE_INFO *)pData;

	//构造数据
	STR_CMD_LC_OTHER_EXCHANGE_INFO ExchangeInfo;
	ZeroMemory(&ExchangeInfo,sizeof(ExchangeInfo));

	ExchangeInfo.dwMinMoney = pExchangeInfo->dwMinMoney;
	ExchangeInfo.dwChangeScale = pExchangeInfo->dwChangeScale;
	ExchangeInfo.dwBankMoney = pExchangeInfo->dwBankMoney;
	ExchangeInfo.dwWithdrawals = pExchangeInfo->dwWithdrawals;

	//发送数据
	WORD wHeadSize=sizeof(ExchangeInfo);
	g_GameCtrl->SendData(dwScoketID, MDM_SERVICE, CMD_LC_OTHERS_EXCHANGE_INFO, &ExchangeInfo, wHeadSize);

	return true;
}
#pragma endregion

#pragma region MDM_SHOP 商城道具
//查询商城
bool CHandleFromGate::On_SUB_CL_SHOP_QUERY(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_SHOP_QUERY)) return false;

	//购买检测

	//购买结果

	//通知client

	//写入数据库 TODONOW
	/*
	//处理消息
	STR_SUB_CL_SHOP_QUERY * pSub = (STR_SUB_CL_SHOP_QUERY *)pData;
	//定义变量
	STR_DBR_CL_SHOP_QUERY Dbr;
	ZeroMemory(&Dbr,sizeof(Dbr));

	Dbr.byGoodsType = pSub->byGoodsType;
	Dbr.dwUserID = pSub->dwUserID;

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_SHOP_QUERY, dwSocketID, &Dbr, sizeof(Dbr));
	*/
	return true;
}

//背包物品查询
bool CHandleFromGate::On_SUB_CL_BAG_QUERY(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	CPlayer* player = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if(!player) return true;

	//std::vector<STR_CMD_LC_BAG_RESULT> vec_cmd;// = player->GetUserProp() TODONOW 带增加

	std::vector<STR_CMD_LC_BAG_RESULT> vec_cmd;
	STR_CMD_LC_BAG_RESULT item;

	item.dwGoodsID = 1;
	item.dwGoodsNum =1;
	vec_cmd.push_back(item);

	item.dwGoodsID = 2;
	item.dwGoodsNum =20;
	vec_cmd.push_back(item);

	item.dwGoodsID = 3;
	item.dwGoodsNum =99;
	vec_cmd.push_back(item);

	//列表发送
	WORD wPacketSize=0;
	BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
	STR_CMD_LC_BAG_RESULT * pCMD=NULL;
	for(int i=0; i < vec_cmd.size(); i++)
	{
		//发送信息
		if ((wPacketSize+sizeof(STR_CMD_LC_BAG_RESULT))>sizeof(cbBuffer))
		{
			g_GameCtrl->SendData(dwSocketID, MDM_SHOP, CMD_LC_BAG_RESULT, cbBuffer, wPacketSize);
			wPacketSize=0;
		}

		//读取信息
		pCMD=(STR_CMD_LC_BAG_RESULT *)(cbBuffer+wPacketSize);
		pCMD->dwGoodsID = vec_cmd.at(i).dwGoodsID; 
		pCMD->dwGoodsNum = vec_cmd.at(i).dwGoodsNum;

		//设置位移
		wPacketSize+=sizeof(STR_CMD_LC_BAG_RESULT);

	}
	if (wPacketSize>0) g_GameCtrl->SendData(dwSocketID, MDM_SHOP, CMD_LC_BAG_RESULT, cbBuffer, wPacketSize);
	
	g_GameCtrl->SendData(dwSocketID, MDM_SHOP, CMD_LC_BAG_FINISH, pData, wDataSize);
	return true;
}

#pragma endregion