#include "GameCtrl.h"
#include "AttemperEngineSink.h"
#include "HandleFromCenter.h"
#include "HandleFromGate.h"
#include "PlayerManager.h"


CAttemperEngineSink* g_AttemperEngineSink = NULL;
//////////////////////////////////////////////////////////////////////////////////

# pragma region 定时器
#define  IDI_CONNECT_CORRESPOND		2									//重连标识
#define	 TIME_CONNECT_CORRESPOND	30*1000L							//连接时间

//构造函数
CAttemperEngineSink::CAttemperEngineSink()
{
	if(!g_AttemperEngineSink)
		g_AttemperEngineSink = this;

	return;
}

//时间事件
bool CAttemperEngineSink::OnEventTimer(DWORD dwTimerID, WPARAM wBindParam)
{
	switch (dwTimerID)
	{
	case IDI_CONNECT_CORRESPOND:	//连接协调
		{
			g_GameCtrl->KillTimer(IDI_CONNECT_CORRESPOND);
			//发起连接
			g_TCPSocketEngine->Connect(_CPD_SERVER_ADDR, PORT_CENTER);

			return true;
		}
	}

	return false;
}

//数据库事件
bool CAttemperEngineSink::OnEventDataBaseResult(WORD wRequestID, DWORD dwScoketID, VOID * pData, WORD wDataSize)
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
#pragma region MDM_SHOP 商城道具
	case DBO_CL_SHOP_QUERY:		//查询商城
		{
			return On_CMD_LC_SHOP_QUERY_RESULT(dwScoketID,pData,wDataSize);
		}
	case DBO_CL_SHOP_QUERY_FINISH: //查询商城结束
		{
			return On_CMD_LC_SHOP_QUERY_FINISH(dwScoketID,pData,wDataSize);
		}
	case DBO_CL_SHOP_DIAMOND : //钻石购买道具
		{
			return On_CMD_LC_SHOP_DIAMOND_RESULT(dwScoketID,pData,wDataSize);
		}
	case DBO_CL_BAG_QUERY: //背包物品查询
		{
			return On_CMD_LC_BAG_RESULT(dwScoketID,pData,wDataSize);
		}
	case DBO_CL_BAG_FINISH://背包物品查询完成
		{
			return On_CMD_LC_BAG_FINISH(dwScoketID,pData,wDataSize);
		}
#pragma endregion
	}

	return false;
}

//应答事件
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientAddr, DWORD dwSocketID)
{
	/*
	//获取索引
	if (LOWORD(dwSocketID)>=MAX_CONTENT) return false;

	//变量定义
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//设置变量
	pBindParameter->dwSocketID=dwSocketID;
	pBindParameter->dwClientAddr=dwClientAddr;
	pBindParameter->dwActiveTime=(DWORD)time(NULL);
	*/
	return true;
}

//关闭事件
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientAddr, DWORD dwActiveTime, DWORD dwSocketID)
{
	//变量定义
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	///Socket连接关闭 数据库处理
	if(pBindParameter->dwUserID != 0)
	{
		DBR_GP_UserQuitInfo quitInfo;
		quitInfo.dwUserID = pBindParameter->dwUserID;

		//1大厅在线 ，2正在游戏，3游戏断线，4离线
		if(bIsExsit)
		{
			quitInfo.byOnlineMask = 3;
		}
		else
		{
			quitInfo.byOnlineMask = 4;
		}
		g_GameCtrl->PostDataBaseRequest(DBR_GP_LOGON_USER_STATE,dwSocketID, &quitInfo,sizeof(quitInfo));
	}

	return false;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPNetworkRead(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	return CHandleFromGate::HandlePacket(Command, pData, wDataSize, dwSocketID);
}

//连接事件
bool CAttemperEngineSink::OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode)
{
	//协调连接
	if (wServiceID==NETWORK_CORRESPOND)
	{
		//错误判断
		if (nErrorCode!=0)
		{
			//设置时间
			g_GameCtrl->SetTimer(IDI_CONNECT_CORRESPOND, TIME_CONNECT_CORRESPOND, 1, 0);
			return false;
		}

		//变量定义
		tagServerItem ServerItem;
		ZeroMemory(&ServerItem,sizeof(ServerItem));

		//构造数据
		ServerItem.dwServerID = 0; //TODOLATER lobby.serverid = (0 << 16)  + nodeid;  //lobby.kind = 0
		ServerItem.byServerType = LOBBY_TYPE;
		//lstrcpyn(ServerItem.szServerName,pCPR->szServerName,CountArray(ServerItem.szServerName));
		ServerItem.wServerPort = PORT_LOGON;

		//设置变量
		TCHAR szInernet_ip[32] = TEXT("0.0.0.0");
		//GetInternetIP(szInernet_ip);
		//lstrcpyn(CPR.szServerAddr,szInernet_ip ,CountArray(CPR.szServerAddr));
		lstrcpyn(ServerItem.szServerAddr,TEXT("127.0.0.1"),CountArray(ServerItem.szServerAddr));

		//发送数据
		g_TCPSocketEngine->SendData(MDM_REGISTER,CPR_REGISTER_SERVER,&ServerItem,sizeof(ServerItem));

		return true;
	}

	return true;
}

//关闭事件
bool CAttemperEngineSink::OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason)
{
	//协调连接
	if (wServiceID==NETWORK_CORRESPOND)
	{
		//设置时间
		g_GameCtrl->SetTimer(IDI_CONNECT_CORRESPOND, TIME_CONNECT_CORRESPOND, 1, 0);
		return true;
	}

	return false;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPSocketRead(WORD wServiceID, TCP_Command Command, VOID * pData, WORD wDataSize)
{
	return CHandleFromCenter::HandlePacket(Command, pData, wDataSize, wServiceID);
}








