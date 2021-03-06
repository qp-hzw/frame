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
			 
#pragma region MDM_CLUB 牌友圈
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
	case DBO_LC_CLUB_INQUERY_LIST_FINISH: //被邀请人查看自己的邀请列表 结束
		{
			return On_CMD_LC_CLUB_INQUERY_LIST_FINISH(dwScoketID,pData,wDataSize);
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
	case DBO_CL_BAG_RESULT: //查询背包返回
		{
			return On_DBO_CL_BAG_RESULT(dwScoketID,pData,wDataSize);
		}
	}
	return true;
}

//登录模块
bool CHandleFromGate::OnTCPNetworkMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CL_LOGON_TEMP:			//游客登陆
		{
			return On_SUB_CL_LOGON_TEMP(pData,wDataSize,dwSocketID);
		}
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
	case SUB_CL_SERVICE_FLOWER: //关注， 粉丝
		{
			return On_SUB_CL_SERVICE_FLOWER(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_FLOWER_QUERY:  //查看关注
		{
			return On_SUB_CL_SERVICE_FLOWER_QUERY(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_FLOWING_QUERY: //查看粉丝
		{
			return On_SUB_CL_SERVICE_FLOWING_QUERY(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_FLOWER_ACT: //关注, 取消关注
		{
			return On_SUB_CL_SERVICE_FLOWER_ACT(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_REFRESH_USER_INFO:		//刷新用户信息
		{
			return On_SUB_CL_Service_RefreshUserInfo(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_SERVICE_MODIFY_PERSONAL_INFO:	//修改个人资料
		{
			return On_SUB_CL_Service_ModifyPersonalInfo(pData, wDataSize, dwSocketID);
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
	case SUB_CL_SERVICE_XJ_RECORD_PLAYBACK:	//小局录像回放
		{
			return On_SUB_CL_Service_XJRecordPlayback(pData, wDataSize, dwSocketID);
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
	case SUB_CL_CLUB_ROOM_SETTING: //房间设置
		{
			return On_SUB_CL_CLUB_ROOM_SETTING(pData, wDataSize, dwSocketID);
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

//商城道具
bool CHandleFromGate::On_MDM_SHOP(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch(wSubCmdID)
	{
	case SUB_CL_SHOP_BUY:	 //商城购买
		{
			return On_SUB_CL_SHOP_BUY(pData, wDataSize, dwSocketID);
		}
	case SUB_CL_BAG_QUERY: //背包物品查询
		{
			return On_SUB_CL_BAG_QUERY(pData, wDataSize, dwSocketID);
		}	
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
//游客登陆
bool CHandleFromGate::On_SUB_CL_LOGON_TEMP(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	if (wDataSize!=sizeof(STR_SUB_CL_LOGON_TEMP)) return false;

	//请求处理
	STR_SUB_CL_LOGON_TEMP * pDBRLogonAccounts=(STR_SUB_CL_LOGON_TEMP *)pData;

	//构造参数
	g_AccountsDB->ResetParameter();
	g_AccountsDB->AddParameter(TEXT("@strMachineID"),pDBRLogonAccounts->szMachineID);

	//输出参数
	TCHAR szDescribeString[127]=TEXT("");
	g_AccountsDB->AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

	//执行查询
	LONG lResultCode = g_AccountsDB->ExecuteProcess(TEXT("GSP_CL_Logon_Temp"),true);

	//结果处理
	CDBVarValue DBVarValue;
	g_AccountsDB->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

	On_DBO_Logon_Accounts(lResultCode,CW2CT(DBVarValue.bstrVal), dwSocketID);

	return true;
}
//账号登录返回
bool CHandleFromGate::On_DBO_Logon_Accounts(DWORD dwResultCode, LPCTSTR pszErrorString, DWORD dwSocketID)
{
	//变量定义
	STR_CMD_LC_LOGON_PLATFORM DBOLogonAccount;
	ZeroMemory(&DBOLogonAccount,sizeof(DBOLogonAccount));

	//构造数据
	DBOLogonAccount.dwResultCode=dwResultCode;
	lstrcpyn(DBOLogonAccount.szDescribeString,pszErrorString,CountArray(DBOLogonAccount.szDescribeString));

	//登陆失败 直接返回
	if(DB_SUCCESS != dwResultCode)
	{
		CLog::Log(log_debug, "登陆失败 %d", dwResultCode);
		//发送数据
		g_GameCtrl->SendData(dwSocketID, MDM_LOGON, CMD_LC_LOGON_ACCOUNTS, &DBOLogonAccount, sizeof(DBOLogonAccount));
		return true;
	}

	//登录成功获取信息
	if(DB_SUCCESS == dwResultCode)
	{
		//用户标志
		DBOLogonAccount.useInfo.dwUserID=g_AccountsDB->GetValue_DWORD(TEXT("UserID"));
		//用户昵称
		g_AccountsDB->GetValue_String(TEXT("NickName"),DBOLogonAccount.useInfo.szNickName,CountArray(DBOLogonAccount.useInfo.szNickName));
		//用户性别
		DBOLogonAccount.useInfo.cbGender=g_AccountsDB->GetValue_BYTE(TEXT("Gender"));
		//头像索引
		g_AccountsDB->GetValue_String(TEXT("HeadUrl"),DBOLogonAccount.useInfo.szHeadUrl,CountArray(DBOLogonAccount.useInfo.szHeadUrl));
		//个性签名
		g_AccountsDB->GetValue_String(TEXT("MySignature"),DBOLogonAccount.useInfo.szUnderWrite,CountArray(DBOLogonAccount.useInfo.szUnderWrite));

		//社团ID
		//DBOLogonAccount.useInfo.dwGroupID=g_AccountsDB->GetValue_BYTE(TEXT("GroupID"));
		//社团名字
		//g_AccountsDB->GetValue_String(TEXT("GroupName"),DBOLogonAccount.useInfo.szGroupName,CountArray(DBOLogonAccount.useInfo.szGroupName));

		//会员等级
		DBOLogonAccount.useInfo.cbMemberOrder=g_AccountsDB->GetValue_BYTE(TEXT("MemberOrder"));
		//人物等级
		DBOLogonAccount.useInfo.dwLevel=g_AccountsDB->GetValue_BYTE(TEXT("cbLevel"));
		//经验数值
		DBOLogonAccount.useInfo.dwExperience=g_AccountsDB->GetValue_DWORD(TEXT("Experience"));

		//用户房卡
		DBOLogonAccount.useInfo.lOpenRoomCard = g_AccountsDB->GetValue_LONGLONG(TEXT("UserRoomCard"));
		//钻石
		DBOLogonAccount.useInfo.lDiamond = g_AccountsDB->GetValue_LONGLONG(TEXT("UserDiamond"));
		//用户游戏币
		DBOLogonAccount.useInfo.lGold = g_AccountsDB->GetValue_LONGLONG(TEXT("UserGold"));
	}

	//重复登录处理
	On_CMD_LC_Logon_RepeatLogon( DBOLogonAccount.useInfo.dwUserID, dwSocketID );

	//插入玩家记录
	tagUserInfo UserInfo;
	ZeroMemory(&UserInfo, sizeof(UserInfo));
	memcpy(&UserInfo, &(DBOLogonAccount.useInfo), sizeof(UserInfo));
	CPlayerManager::InsertPlayer (dwSocketID, UserInfo);

	CLog::Log(log_debug, "id: %ld,  name: %s", DBOLogonAccount.useInfo.dwUserID, DBOLogonAccount.useInfo.szNickName);

	//查询断线重连
	STR_CPR_LP_OFFLINE_PLAYERQUERY CPR;
	CPR.dwUserID = DBOLogonAccount.useInfo.dwUserID;
	g_TCPSocketEngine->SendData(CPD_MDM_TRANSFER, CPR_LP_OFFLINE_PLAYERQUERY, &CPR, sizeof(CPR));
	
	return true;
}

//帐号登录
bool CHandleFromGate::On_SUB_CL_Logon_Accounts(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	if (wDataSize != sizeof(STR_SUB_CL_LOGON_ACCOUNTS))
	{
		return false;
	}

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_LOGON_ACCOUNTS,dwSocketID, pData, wDataSize);
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
	On_CMD_LC_Logon_RepeatLogon( pCMD->useInfo.dwUserID, dwScoketID );

	//插入玩家记录
	tagUserInfo UserInfo;
	ZeroMemory(&UserInfo, sizeof(UserInfo));
	memcpy(&UserInfo, &(pCMD->useInfo), sizeof(UserInfo));
	CPlayerManager::InsertPlayer (dwScoketID, UserInfo);

	//查询断线重连
	STR_CPR_LP_OFFLINE_PLAYERQUERY CPR;
	CPR.dwUserID = pCMD->useInfo.dwUserID;
	g_TCPSocketEngine->SendData(CPD_MDM_TRANSFER, CPR_LP_OFFLINE_PLAYERQUERY, &CPR, sizeof(CPR));
	
	/* TODONOW
	//登录奖励
	On_CMD_LC_Logon_Logon_Reward(dwScoketID, pCMD->LasLogonDate);
	*/

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

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_LOGON_PLATFORM, dwSocketID, pData, wDataSize);

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
	On_CMD_LC_Logon_RepeatLogon( pCMD->useInfo.dwUserID ,dwScoketID);

	//断线重连的处理

	//玩家记录
	//CPlayerManager::InsertPlayer (dwScoketID, pCMD);

	//发送登录成功
	g_GameCtrl->SendData(dwScoketID,MDM_LOGON,CMD_LC_LOGON_PLATFORM, pCMD, sizeof(STR_CMD_LC_LOGON_PLATFORM));

	/*
	//登录奖励
	On_CMD_LC_Logon_Logon_Reward(dwScoketID, pCMD->);
	*/

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
//粉丝关注
 bool CHandleFromGate::On_SUB_CL_SERVICE_FLOWER(VOID * pData, WORD wDataSize, DWORD dwSocketID)
 {
	 if ( sizeof(STR_SUB_CL_SERVICE_FLOWER) != wDataSize) return true;
	 CPlayer * player = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	 if(player == NULL) return false;
	 
	 STR_SUB_CL_SERVICE_FLOWER* sub = (STR_SUB_CL_SERVICE_FLOWER*) pData;

	 g_AccountsDB->ResetParameter();
	 g_AccountsDB->AddParameter(TEXT("@dwTargetID"), sub->dwTargetID); 
	 g_AccountsDB->AddParameter(TEXT("@dwMyID"), player->GetUserID()); 

	 //执行查询
	 LONG lResultCode = g_AccountsDB->ExecuteProcess(TEXT("GSP_CL_FLOWER_ALL"),true);
	 if(lResultCode == DB_SUCCESS)
	 {
		 STR_CMD_LC_SERVICE_FLOWER cmd;
		 //我的关注
		 cmd.dwFolwerNum = g_AccountsDB->GetValue_DWORD(TEXT("FlowerNum"));
		 //我的粉丝
		 cmd.dwFolwingNum = g_AccountsDB->GetValue_DWORD(TEXT("FlowingNum"));

		 int i = 0;
		 while(g_AccountsDB->IsRecordsetEnd() == false)
		 {
			 if(i >= 5) break;
			 STR_CMD_LC_SERVICE_FLOWER_P cmd_p;
			 cmd_p.dwUserID = g_AccountsDB->GetValue_DWORD(TEXT("UserID"));
			 g_AccountsDB->GetValue_String(TEXT("NickName"),cmd_p.szNickName,CountArray(cmd_p.szNickName));
			 cmd_p.cbGender = g_AccountsDB->GetValue_BYTE(TEXT("Gender"));
			 g_AccountsDB->GetValue_String(TEXT("HeadUrl"),cmd_p.szHeadUrl,CountArray(cmd_p.szHeadUrl));
			 cmd_p.dwLoveValue = g_AccountsDB->GetValue_DWORD(TEXT("LoveValue"));

			 memcpy(&cmd.folwingInfo[i], &cmd_p, sizeof(cmd_p));
			 i++;
		 }

		 //是否显示关注按钮
		 BYTE bGuanzhu	= g_AccountsDB->GetValue_DWORD(TEXT("cbIsGuanzhu")); //0表示没有关注
		 if ( sub->dwTargetID == player->GetUserID()) //自己
		 {
			 cmd.cbIsGuanzhu = 0;
		 }
		 else if (bGuanzhu == 0 )
		 {
			  cmd.cbIsGuanzhu = 1;
		 }
		 else
		 {
			 cmd.cbIsGuanzhu = 2;
		 }

		 g_GameCtrl->SendData(dwSocketID, MDM_SERVICE, CMD_LC_SERVICE_FLOWER, &cmd, sizeof(cmd));
	 }
	 else
	 {
		 g_GameCtrl->SendDataMsg(dwSocketID, "查询失败");
	 }

	 return true;
 }
//查看我的关注
bool CHandleFromGate::On_SUB_CL_SERVICE_FLOWER_QUERY(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	 CPlayer * player = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	 if(player == NULL) return false;

	 g_AccountsDB->ResetParameter();
	 g_AccountsDB->AddParameter(TEXT("@dwUserID"), player->GetUserID()); 

	 //执行查询
	 LONG lResultCode = g_AccountsDB->ExecuteProcess(TEXT("GSP_CL_FLOWER_QUERY"),true);
	 if(lResultCode == DB_SUCCESS)
	 {
		 WORD wPacketSize=0;
		 BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
		 STR_CMD_LC_SERVICE_FLOWER_P * pCMD=NULL;
		 while(g_AccountsDB->IsRecordsetEnd() == false)
		 {
			 if ((wPacketSize+sizeof(STR_CMD_LC_SERVICE_FLOWER_P))>sizeof(cbBuffer))
			 {
				 g_GameCtrl->SendData(dwSocketID, MDM_SERVICE, CMD_LC_SERVICE_FLOWER_QUERY, cbBuffer, wPacketSize);
				 wPacketSize=0;
			 }

			 STR_CMD_LC_SERVICE_FLOWER_P cmd_p;
			 cmd_p.dwUserID = g_AccountsDB->GetValue_DWORD(TEXT("UserID"));
			 g_AccountsDB->GetValue_String(TEXT("NickName"),cmd_p.szNickName,CountArray(cmd_p.szNickName));
			 cmd_p.cbGender = g_AccountsDB->GetValue_BYTE(TEXT("Gender"));
			 g_AccountsDB->GetValue_String(TEXT("HeadUrl"),cmd_p.szHeadUrl,CountArray(cmd_p.szHeadUrl));
			 cmd_p.dwLoveValue = g_AccountsDB->GetValue_DWORD(TEXT("LoveValue"));

			 pCMD=(STR_CMD_LC_SERVICE_FLOWER_P *)(cbBuffer+wPacketSize);
			 memcpy(pCMD, &cmd_p, sizeof(cmd_p));

			 wPacketSize+=sizeof(STR_CMD_LC_SERVICE_FLOWER_P);
		 }

		 if (wPacketSize>0) g_GameCtrl->SendData(dwSocketID, MDM_SERVICE, CMD_LC_SERVICE_FLOWER_QUERY, cbBuffer, wPacketSize);
		 g_GameCtrl->SendData(dwSocketID, MDM_SERVICE, CMD_LC_SERVICE_FLOWER_QUERY_FINISH, NULL, 0);
	 }
	 else
	 {
		 g_GameCtrl->SendDataMsg(dwSocketID, "查询失败");
	 }
}
//查看我的粉丝
bool CHandleFromGate::On_SUB_CL_SERVICE_FLOWING_QUERY(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
		 CPlayer * player = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	 if(player == NULL) return false;

	 g_AccountsDB->ResetParameter();
	 g_AccountsDB->AddParameter(TEXT("@dwUserID"), player->GetUserID()); 

	 //执行查询
	 LONG lResultCode = g_AccountsDB->ExecuteProcess(TEXT("GSP_CL_FLOWING_QUERY"),true);
	 if(lResultCode == DB_SUCCESS)
	 {
		 WORD wPacketSize=0;
		 BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
		 STR_CMD_LC_SERVICE_FLOWER_P * pCMD=NULL;
		 while(g_AccountsDB->IsRecordsetEnd() == false)
		 {
			 if ((wPacketSize+sizeof(STR_CMD_LC_SERVICE_FLOWER_P))>sizeof(cbBuffer))
			 {
				 g_GameCtrl->SendData(dwSocketID, MDM_SERVICE, CMD_LC_SERVICE_FLOWING_QUERY, cbBuffer, wPacketSize);
				 wPacketSize=0;
			 }

			 STR_CMD_LC_SERVICE_FLOWER_P cmd_p;
			 cmd_p.dwUserID = g_AccountsDB->GetValue_DWORD(TEXT("UserID"));
			 g_AccountsDB->GetValue_String(TEXT("NickName"),cmd_p.szNickName,CountArray(cmd_p.szNickName));
			 cmd_p.cbGender = g_AccountsDB->GetValue_BYTE(TEXT("Gender"));
			 g_AccountsDB->GetValue_String(TEXT("HeadUrl"),cmd_p.szHeadUrl,CountArray(cmd_p.szHeadUrl));
			 cmd_p.dwLoveValue = g_AccountsDB->GetValue_DWORD(TEXT("LoveValue"));

			 pCMD=(STR_CMD_LC_SERVICE_FLOWER_P *)(cbBuffer+wPacketSize);
			 memcpy(pCMD, &cmd_p, sizeof(cmd_p));

			 wPacketSize+=sizeof(STR_CMD_LC_SERVICE_FLOWER_P);
		 }

		 if (wPacketSize>0) g_GameCtrl->SendData(dwSocketID, MDM_SERVICE, CMD_LC_SERVICE_FLOWING_QUERY, cbBuffer, wPacketSize);
		 g_GameCtrl->SendData(dwSocketID, MDM_SERVICE, CMD_LC_SERVICE_FLOWING_QUERY_FINISH, NULL, 0);
	 }
	 else
	 {
		 g_GameCtrl->SendDataMsg(dwSocketID, "查询失败");
	 }
}
//关注,取消关注
bool CHandleFromGate::On_SUB_CL_SERVICE_FLOWER_ACT(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	if(sizeof(STR_SUB_CL_SERVICE_FLOWER_ACT) != wDataSize) return false;
	STR_SUB_CL_SERVICE_FLOWER_ACT* sub = (STR_SUB_CL_SERVICE_FLOWER_ACT*) pData;

	CPlayer * player = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if(player == NULL) return false;

	if(player->GetUserID() == sub->dwTargetID) return true;

	g_AccountsDB->ResetParameter();
	g_AccountsDB->AddParameter(TEXT("@dwTargetID"), sub->dwTargetID); 
	g_AccountsDB->AddParameter(TEXT("@dwMyID"), player->GetUserID()); 
	g_AccountsDB->AddParameter(TEXT("@cbMask"), sub->cbMask); 

	//执行查询
	LONG lResultCode = g_AccountsDB->ExecuteProcess(TEXT("GSP_CL_FLOWER_ACT"),true);

	STR_CMD_LC_SERVICE_FLOWER_ACT cmd;
	cmd.cbResult = lResultCode;
	cmd.cbMask = sub->cbMask;
	g_GameCtrl->SendData(dwSocketID, MDM_SERVICE, CMD_LC_SERVICE_FLOWER_ACT, &cmd, sizeof(cmd));
}

//刷新用户信息
bool CHandleFromGate::On_SUB_CL_Service_RefreshUserInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//玩家校验
	CPlayer *player = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if (player == NULL)
		return false;

	//执行查询
	g_AccountsDB->ResetParameter();
	g_AccountsDB->AddParameter(TEXT("@UserID"), player->GetUserID());

	//执行查询
	LONG lResultCode = g_AccountsDB->ExecuteProcess(TEXT("GSP_CL_Service_RefreshUserInfo"),true);

	//变量定义
	tagUserInfo info;
	ZeroMemory(&info, sizeof(tagUserInfo));

	//查询成功获取信息
	if (lResultCode == DB_SUCCESS)
	{
		//用户标志
		info.dwUserID = player->GetUserID();
		//用户昵称
		g_AccountsDB->GetValue_String(TEXT("NickName"),info.szNickName,CountArray(info.szNickName));
		//用户性别
		info.cbGender=g_AccountsDB->GetValue_BYTE(TEXT("Gender"));
		//头像索引
		g_AccountsDB->GetValue_String(TEXT("HeadUrl"),info.szHeadUrl,CountArray(info.szHeadUrl));
		//个性签名
		g_AccountsDB->GetValue_String(TEXT("MySignature"),info.szUnderWrite,CountArray(info.szUnderWrite));

		//会员等级
		info.cbMemberOrder=g_AccountsDB->GetValue_BYTE(TEXT("MemberOrder"));
		//人物等级
		info.dwLevel=g_AccountsDB->GetValue_BYTE(TEXT("cbLevel"));
		//经验数值
		info.dwExperience=g_AccountsDB->GetValue_DWORD(TEXT("Experience"));

		//用户房卡
		info.lOpenRoomCard = g_AccountsDB->GetValue_LONGLONG(TEXT("UserRoomCard"));
		//钻石
		info.lDiamond = g_AccountsDB->GetValue_LONGLONG(TEXT("UserDiamond"));
		//用户游戏币
		info.lGold = g_AccountsDB->GetValue_LONGLONG(TEXT("UserGold"));
	}


	//发送消息
	g_GameCtrl->SendData(dwSocketID, MDM_SERVICE, CMD_CL_SERVICE_REFRESH_USER_INFO, &info, sizeof(tagUserInfo));

	//更新player
	player->UpDataUserInfo(info);

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
	//玩家校验
	CPlayer *player = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if (player == NULL)
		return false;

	g_TreasureDB->ResetParameter();
	g_TreasureDB->AddParameter(TEXT("@dwUserID"), player->GetUserID());

	//执行查询
	LONG lResultCode = g_TreasureDB->ExecuteProcess(TEXT("GSP_CL_SERVICE_PURE_RECORD_LIST"),true);

	//列表发送
	WORD wPacketSize = 0;
	BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/4];
	STR_CMD_LC_SERVICE_PURE_RECORD_LIST *pList = NULL;

	while ( (lResultCode == DB_SUCCESS) && (g_TreasureDB->IsRecordsetEnd()==false))
	{
		//发送信息
		if ((wPacketSize+sizeof(STR_CMD_LC_SERVICE_PURE_RECORD_LIST))>sizeof(cbBuffer))
		{
			g_GameCtrl->SendData(dwSocketID, MDM_SERVICE, CMD_LC_SERVICE_PURE_RECORD_LIST, cbBuffer, wPacketSize);
			wPacketSize=0;
		}

		pList = (STR_CMD_LC_SERVICE_PURE_RECORD_LIST *)(cbBuffer + wPacketSize);
		pList->dwTableID = g_TreasureDB->GetValue_DWORD(TEXT("TableID"));
		pList->wGameMode = g_TreasureDB->GetValue_WORD(TEXT("GameMode"));
		pList->wGameCount = g_TreasureDB->GetValue_WORD(TEXT("AllCount"));
		pList->wPlayerCount = g_TreasureDB->GetValue_WORD(TEXT("PlayerCount"));
		pList->wKindID = g_TreasureDB->GetValue_WORD(TEXT("KindID"));
		g_TreasureDB->GetValue_SystemTime(TEXT("@InsertDate"), pList->szTime);
		g_TreasureDB->GetValue_String(TEXT("OnlyID"), pList->szOnlyID, CountArray(pList->szOnlyID));

		//下一个查询
		g_spTreasureDB->ResetParameter();
		g_spTreasureDB->AddParameter(TEXT("@OnlyID"), pList->szOnlyID);

		//执行查询
		LONG llResultCode = g_spTreasureDB->ExecuteProcess(TEXT("GSP_CL_SERVICE_PURE_RECORD_LIST_PLAYINFO"),true);
		for (WORD i = 0; i < pList->wPlayerCount; i++)
		{
			//结束判断
			if (g_spTreasureDB->IsRecordsetEnd()==true) break;

			pList->Info[i].dwUserID = g_spTreasureDB->GetValue_DWORD(TEXT("UserID"));
			pList->Info[i].llScore = g_spTreasureDB->GetValue_LONGLONG(TEXT("Score"));
			g_spTreasureDB->GetValue_String(TEXT("NickName"), pList->Info[i].szName, CountArray(pList->Info[i].szName));
			g_spTreasureDB->GetValue_String(TEXT("HeadUrl"), pList->Info[i].szHeadUrl, CountArray(pList->Info[i].szHeadUrl));

			//移动记录
			g_spTreasureDB->MoveToNext();
		}

		//设置位移
		wPacketSize+=sizeof(STR_CMD_LC_SERVICE_PURE_RECORD_LIST);

		//移动记录
		g_TreasureDB->MoveToNext();
	}
	if (wPacketSize > 0)	g_GameCtrl->SendData(dwSocketID, MDM_SERVICE, CMD_LC_SERVICE_PURE_RECORD_LIST, cbBuffer, wPacketSize);

	return true;
}
//pure小局战绩 查询
bool CHandleFromGate::On_SUB_CL_SERVICE_PURE_XJ_RECORD_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//参数校验
	if(wDataSize!=sizeof(STR_SUB_CL_SERVICE_PURE_XJ_RECORD_LIST))
		return false;

	//玩家校验
	CPlayer *player = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if (player == NULL)
		return false;

	//获取对象
	STR_SUB_CL_SERVICE_PURE_XJ_RECORD_LIST *data = (STR_SUB_CL_SERVICE_PURE_XJ_RECORD_LIST *)pData;

	g_TreasureDB->ResetParameter();
	g_TreasureDB->AddParameter(TEXT("@OnlyID"), data->szOnlyID);

	//执行查询
	LONG lResultCode = g_TreasureDB->ExecuteProcess(TEXT("GSP_CL_SERVICE_PURE_XJ_RECORD_LIST"),true);

	//列表发送
	WORD wPacketSize = 0;
	BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/4];
	STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST *pList = NULL;

	while ( (lResultCode == DB_SUCCESS) && (g_TreasureDB->IsRecordsetEnd()==false))
	{
		//发送信息
		if ((wPacketSize+sizeof(STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST))>sizeof(cbBuffer))
		{
			g_GameCtrl->SendData(dwSocketID, MDM_SERVICE, CMD_LC_SERVICE_PURE_XJ_RECORD_LIST, cbBuffer, wPacketSize);
			wPacketSize=0;
		}

		pList = (STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST *)(cbBuffer + wPacketSize);
		pList->dwTableID = g_TreasureDB->GetValue_DWORD(TEXT("TableID"));
		pList->wCurCount = g_TreasureDB->GetValue_WORD(TEXT("CurCount"));
		pList->wPlayerCount = g_TreasureDB->GetValue_WORD(TEXT("PlayerCount"));
		g_TreasureDB->GetValue_SystemTime(TEXT("InsertTime"), pList->szTime);
		g_TreasureDB->GetValue_String(TEXT("OnlyID"), pList->szOnlyID, CountArray(pList->szOnlyID));

		//下一个查询
		g_spTreasureDB->ResetParameter();
		g_spTreasureDB->AddParameter(TEXT("@CurCount"), pList->wCurCount);
		g_spTreasureDB->AddParameter(TEXT("@OnlyID"), pList->szOnlyID);

		//执行查询
		LONG llResultCode = g_spTreasureDB->ExecuteProcess(TEXT("GSP_CL_SERVICE_PURE_XJ_RECORD_LIST_PLAYINFO"),true);
		for (WORD i = 0; i < pList->wPlayerCount; i++)
		{
			//结束判断
			if (g_spTreasureDB->IsRecordsetEnd()==true) { CLog::Log(log_debug, "End"); break;}

			pList->Info[i].dwUserID = g_spTreasureDB->GetValue_DWORD(TEXT("UserID"));
			pList->Info[i].llScore = g_spTreasureDB->GetValue_LONGLONG(TEXT("Score"));
			pList->Info[i].wIdentity = g_spTreasureDB->GetValue_WORD(TEXT("PlayerIdentity"));
			g_spTreasureDB->GetValue_String(TEXT("NickName"), pList->Info[i].szName, CountArray(pList->Info[i].szName));
			g_spTreasureDB->GetValue_String(TEXT("HeadUrl"), pList->Info[i].szHeadUrl, CountArray(pList->Info[i].szHeadUrl));

			//移动记录
			g_spTreasureDB->MoveToNext();
		}

		//设置位移
		wPacketSize+=sizeof(STR_CMD_LC_SERVICE_PURE_XJ_RECORD_LIST);

		//移动记录
		g_TreasureDB->MoveToNext();
	}
	if (wPacketSize > 0)	g_GameCtrl->SendData(dwSocketID, MDM_SERVICE, CMD_LC_SERVICE_PURE_XJ_RECORD_LIST, cbBuffer, wPacketSize);

	return true;
}
//小局录像回放
bool CHandleFromGate::On_SUB_CL_Service_XJRecordPlayback(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//数据大小校验
	if ( wDataSize != sizeof(STR_SUB_CL_SERVICE_XJ_RECORD_PLAYBACK) )
		return false;

	//玩家校验
	CPlayer *player = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if (player == NULL)
		return false;

	//获取对象
	STR_SUB_CL_SERVICE_XJ_RECORD_PLAYBACK *data = (STR_SUB_CL_SERVICE_XJ_RECORD_PLAYBACK *)pData;

	g_TreasureDB->ResetParameter();
	g_TreasureDB->AddParameter(TEXT("@OnlyID"), data->szOnlyID);
	g_TreasureDB->AddParameter(TEXT("@CurCount"), data->wCurCount);

	//执行查询
	LONG lResultCode = g_TreasureDB->ExecuteProcess(TEXT("GSP_CL_SERVICE_PURE_XJ_RECORD_INFO"),true);

	if (lResultCode == DB_SUCCESS)
	{
		//获取录像数据
		CHAR szData[2*LEN_MAX_RECORD_SIZE];
		g_TreasureDB->GetValue_String(TEXT("VideoData"), szData, CountArray(szData));

		//数据转换
		BYTE cbBuffer[LEN_MAX_RECORD_SIZE];	
		CWConvert::StrToBin3(szData, cbBuffer, 0, LEN_MAX_RECORD_SIZE*2);

		//分批发送
		for (int i = 0; i < 4; i++)
		{
			STR_CMD_LC_SERVICE_XJ_RECORD_PLAYBACK CMD;
			ZeroMemory(&CMD, sizeof(STR_CMD_LC_SERVICE_XJ_RECORD_PLAYBACK));
			memcpy_s(CMD.cbRecordData, sizeof(CMD.cbRecordData), cbBuffer+i*LEN_MAX_RECORD_SIZE/4, sizeof(CMD.cbRecordData));
			CMD.cbCurCount = i;
			CMD.cbAllCount = 4;
			CMD.wKindID = g_TreasureDB->GetValue_WORD(TEXT("KindID"));

			//发送数据
			g_GameCtrl->SendData(dwSocketID, MDM_SERVICE, CMD_LC_SERVICE_XJ_RECORD_PLAYBACK, &CMD, sizeof(CMD));
		}
	}

	return true;
}

//修改个人资料
bool CHandleFromGate::On_SUB_CL_Service_ModifyPersonalInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	if (wDataSize != sizeof(STR_SUB_CL_SERVICE_MODIFY_PERSONAL_INFO)) return false;
	STR_SUB_CL_SERVICE_MODIFY_PERSONAL_INFO* sub = (STR_SUB_CL_SERVICE_MODIFY_PERSONAL_INFO*)pData;

	CPlayer* player = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if(player == NULL) return false;

	//输入参数
	g_AccountsDB->ResetParameter();
	g_AccountsDB->AddParameter(TEXT("@UserID"), player->GetUserID());
	g_AccountsDB->AddParameter(TEXT("@NickName"), sub->szNickName);
	g_AccountsDB->AddParameter(TEXT("@Gender"), sub->cbGender);
	g_AccountsDB->AddParameter(TEXT("@HeadImageUrl"), sub->szHeadImageUrl);
	g_AccountsDB->AddParameter(TEXT("@Signature"), sub->szSignature);
	g_AccountsDB->AddParameter(TEXT("@RealName"), sub->szRealName);
	g_AccountsDB->AddParameter(TEXT("@IDCardNum"), sub->szIDCardNum);
	g_AccountsDB->AddParameter(TEXT("@PhoneNum"), sub->szPhoneNum);

	//执行查询
	LONG lResultCode = g_AccountsDB->ExecuteProcess(TEXT("GSP_CL_Modify_UserlInfo"), true);

	if(lResultCode == 0)
	{
		g_GameCtrl->SendDataMsg(dwSocketID, "操作成功");
	}
	else
	{
		g_GameCtrl->SendDataMsg(dwSocketID, "操作失败");
	}

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
	CPlayer* player = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if(player == NULL) return false;

	//DB查询
	g_AccountsDB->ResetParameter();
	g_AccountsDB->AddParameter(TEXT("@UserID"),player->GetUserID());
	g_AccountsDB->ExecuteProcess(TEXT("GSP_CL_CLUB_QUERY_MY_CLUBS"),true);

	//列表发送
	WORD wPacketSize=0;
	BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
	wPacketSize=0;
	STR_CMD_LC_CLUB_ALL_CLUB_INFO_LIST * pCMD=NULL;
	while (g_AccountsDB->IsRecordsetEnd()==false)
	{
		//发送信息
		if ((wPacketSize+sizeof(STR_CMD_LC_CLUB_ALL_CLUB_INFO_LIST))>sizeof(cbBuffer))
		{
			g_GameCtrl->SendData(dwSocketID, MDM_CLUB, CMD_LC_CLUB_ALL_CLUB_INFO_LIST, cbBuffer, wPacketSize);
			wPacketSize=0;
		}

		//读取信息
		pCMD=(STR_CMD_LC_CLUB_ALL_CLUB_INFO_LIST *)(cbBuffer+wPacketSize);
		pCMD->dwClubID=g_AccountsDB->GetValue_DWORD(TEXT("ClubID"));	
		g_AccountsDB->GetValue_String(TEXT("ClubName"),pCMD->szClubName,CountArray(pCMD->szClubName));
		pCMD->byClubRole=g_AccountsDB->GetValue_BYTE(TEXT("ClubRole"));

		//设置位移
		wPacketSize+=sizeof(STR_CMD_LC_CLUB_ALL_CLUB_INFO_LIST);
		g_AccountsDB->MoveToNext();
	}
	if (wPacketSize>0) g_GameCtrl->SendData(dwSocketID, MDM_CLUB, CMD_LC_CLUB_ALL_CLUB_INFO_LIST, cbBuffer, wPacketSize);

	g_GameCtrl->SendData(dwSocketID, MDM_CLUB, CMD_LC_CLUB_ALL_INFO_FINISH, NULL, 0);
	
	return true;
}

//查询指定牌友圈房间列表
bool CHandleFromGate::On_SUB_CL_CLUB_ROOM_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	CPlayer * player = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if(player == NULL) return false;
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_ROOM_LIST)) return false;
	STR_DBR_CL_CLUB_ROOM_LIST *pSub = (STR_DBR_CL_CLUB_ROOM_LIST*) pData;

	//查询DB
	g_AccountsDB->ResetParameter();
	g_AccountsDB->AddParameter(TEXT("@ClubID"),pSub->dwClubID);
	LONG lResultCode = g_AccountsDB->ExecuteProcess(TEXT("GSP_CL_CLUB_ROOM_LIST"),true);

	WORD wPacketSize=0;
	BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
	wPacketSize=0;
	STR_CMD_LC_CLUB_ROOM_LIST * pDBO=NULL;
	while ((lResultCode == DB_SUCCESS) && (g_AccountsDB->IsRecordsetEnd()==false))
	{
		//发送信息
		if ((wPacketSize+sizeof(STR_CMD_LC_CLUB_ROOM_LIST))>sizeof(cbBuffer))
		{
			g_GameCtrl->SendData(dwSocketID, MDM_CLUB, CMD_LC_CLUB_ROOM_LIST, cbBuffer, wPacketSize);
			wPacketSize=0;
		}

		//读取信息
		pDBO=(STR_CMD_LC_CLUB_ROOM_LIST *)(cbBuffer+wPacketSize);
		pDBO->dwRoomID=g_AccountsDB->GetValue_DWORD(TEXT("RoomID"));
		g_AccountsDB->GetValue_String(TEXT("RoomName"),pDBO->szRoomName,CountArray(pDBO->szRoomName));
		pDBO->dwKindID =g_AccountsDB->GetValue_DWORD(TEXT("KindID"));
		pDBO->byGoldOrFK = g_AccountsDB->GetValue_BYTE(TEXT("ModeID")); //1房卡场 2金币场
		g_AccountsDB->GetValue_String(TEXT("KindName"),pDBO->szKindName,CountArray(pDBO->szKindName));

		pDBO->wPlayerNum=g_AccountsDB->GetValue_WORD(TEXT("PlayerNum"));
		g_AccountsDB->GetValue_String(TEXT("RoomRule"),pDBO->szRoomRule,CountArray(pDBO->szRoomRule));
		pDBO->byAllRound=g_AccountsDB->GetValue_BYTE(TEXT("AllRound"));
		pDBO->byChairNum=g_AccountsDB->GetValue_BYTE(TEXT("ChairNum"));

		//是否允许解散
		pDBO->bDissolve=g_AccountsDB->GetValue_BYTE(TEXT("Dissolve"));

		pDBO->dwAmount=g_AccountsDB->GetValue_DWORD(TEXT("ServiceGold"));
		pDBO->dwOwnerPercentage=g_AccountsDB->GetValue_DWORD(TEXT("Revenue"));

		pDBO->byMask=g_AccountsDB->GetValue_BYTE(TEXT("Mask"));//1 AA支付;  2大赢家支付 
		pDBO->dwDizhu=g_AccountsDB->GetValue_DWORD(TEXT("Dizhu"));
		pDBO->dwGold=g_AccountsDB->GetValue_DWORD(TEXT("Gold"));

		//设置位移
		wPacketSize+=sizeof(STR_CMD_LC_CLUB_ROOM_LIST);
		g_AccountsDB->MoveToNext();
	}
	if (wPacketSize>0) g_GameCtrl->SendData(dwSocketID, MDM_CLUB, CMD_LC_CLUB_ROOM_LIST, cbBuffer, wPacketSize);

	g_GameCtrl->SendData(dwSocketID, MDM_CLUB, CMD_LC_CLUB_ROOM_LIST_FINISH, NULL, 0);
	return true;
}

//查询未满员, 随机牌友圈(最大9个)
bool CHandleFromGate::On_SUB_CL_CLUB_RANDOM_CLUB_LIST(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	/*
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_RANDOM_CLUB_LIST)) return false;

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_RANDOM_CLUB_LIST, dwSocketID, pData, wDataSize);
	*/
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
	CPlayer * player = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if(player == NULL) return false;

	if(wDataSize != sizeof(STR_SUB_CL_CLUB_CREATE_CLUB)) return false;
	STR_SUB_CL_CLUB_CREATE_CLUB * pSub = (STR_SUB_CL_CLUB_CREATE_CLUB *)pData;

	//构造参数
	g_AccountsDB->ResetParameter();
	g_AccountsDB->AddParameter(TEXT("@UserID"),player->GetUserID());
	g_AccountsDB->AddParameter(TEXT("@ClubName"),pSub->szClubName);
	g_AccountsDB->AddParameter(TEXT("@szMessag"), pSub->szMessag);
	LONG lResultCode = g_AccountsDB->ExecuteProcess(TEXT("GSP_CL_CLUB_CREATE_CLUB"),true);

	//结构体构造
	STR_CMD_LC_CLUB_CREATE_CLUB CMD;
	ZeroMemory(&CMD, sizeof(CMD));
	CMD.lResultCode = lResultCode;
	g_GameCtrl->SendData(dwSocketID, MDM_CLUB, CMD_LC_CLUB_CREATE_CLUB, &CMD, sizeof(CMD));

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

//	if( (DB_SUCCESS ==  pCmd->lResultCode) && (0 == pCmd->byMask) && (0 != pCmd->dwGameID))
	{
		//通知协调服, 协调服通知游戏服
		STR_CPR_LP_CLUB_TABLE_DISSOLVE CPR;
	//	CPR.dwGameID = pCmd->dwGameID;
		//CPR.dwTableID = pCmd->dwTableID;

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
	/*
	STR_CMD_LC_CLUB_MEMBER_MANAGER_FINISH cmd;
	cmd.byMask = 1;

	//处理消息
	g_GameCtrl->SendData(dwScoketID, MDM_CLUB, CMD_LC_CLUB_MEMBER_MANAGER_FINISH, &cmd, sizeof(cmd));
	*/
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
	/*
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_INQUERY_LIST)) return false;

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CL_CLUB_INQUERY_LIST, dwSocketID, pData, wDataSize);
	*/
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

	g_GameCtrl->SendData(dwScoketID, MDM_SHOP, CMD_LC_GIFT_GIVE_PROPS, pData, wDataSize);

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
	g_GameCtrl->SendData(player->GetSocketID(), MDM_SHOP, CMD_LC_GIFT_GIVE_PROPS_SHOW, 
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
//商城购买
bool CHandleFromGate::On_SUB_CL_SHOP_BUY(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	if(wDataSize != sizeof(STR_SUB_CL_SHOP_BUY)) return false;
	STR_SUB_CL_SHOP_BUY * pSub = (STR_SUB_CL_SHOP_BUY *)pData;

	CPlayer *player = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if(player == NULL) return false;

	//购买检测
	//if(0 != Chck())
	
	tagUserProperty prop;
	prop.dwPropID = pSub->dwGoodsID;
	prop.dwPropCount = pSub->dwGoodsNum;
	player->AddProp(prop);

	//返回给client
	STR_CMD_LC_SHOP_BUY_RESULT cmd;
	cmd.byResult = 0;
	g_GameCtrl->SendData(dwSocketID, MDM_SHOP, CMD_LC_SHOP_BUY_RESULT, &cmd, sizeof(cmd));
	return true;
}

//道具查询
bool CHandleFromGate::On_SUB_CL_BAG_QUERY(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	CPlayer *player = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if(player == NULL) return true;

	WORD wPacketSize=0;
	BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
	STR_CMD_LC_BAG_RESULT * pCMD=NULL;
	for(auto item : player->GetProps())
	{
		if ((wPacketSize+sizeof(STR_CMD_LC_BAG_RESULT))>sizeof(cbBuffer))
		{
			g_GameCtrl->SendData(dwSocketID, MDM_SHOP, CMD_LC_BAG_RESULT, cbBuffer, wPacketSize);
			wPacketSize=0;
		}
	
		pCMD=(STR_CMD_LC_BAG_RESULT *)(cbBuffer+wPacketSize);
		pCMD->dwGoodsID = item.dwPropID;
		pCMD->dwGoodsNum = item.dwPropCount;

		wPacketSize+=sizeof(STR_CMD_LC_BAG_RESULT);
	}

	if (wPacketSize>0) g_GameCtrl->SendData(dwSocketID, MDM_SHOP, CMD_LC_BAG_RESULT, cbBuffer, wPacketSize);

	g_GameCtrl->SendData(dwSocketID, MDM_SHOP, CMD_LC_BAG_FINISH, NULL, 0);
	return true;
}

//背包物品查询 返回
bool CHandleFromGate::On_DBO_CL_BAG_RESULT( DWORD dwScoketID, VOID * pData, WORD wDataSize)
{
	//校验参数
	WORD Size = sizeof(STR_DBO_CL_BAG_RESULT);
	if( (wDataSize < Size) || 
		( (wDataSize % Size) != 0))
		return false;

	//获得个数
	int count = wDataSize / sizeof(STR_DBO_CL_BAG_RESULT);

	//返回的列表
	STR_DBO_CL_BAG_RESULT *TaskList = (STR_DBO_CL_BAG_RESULT *)pData;
	for (int i=0; i<count; i++)
	{
		CPlayer *player = CPlayerManager::FindPlayerByID((TaskList+i)->dwUserID);
		player ->InitProp((TaskList+i)->Prop);
		CLog::Log(log_debug, "InitProp propid: %ld", (TaskList+i)->Prop.dwPropID);
	}

	return true;
}
#pragma endregion