#include "HandleFromGate.h"
#include "GameCtrl.h"
#include "TableManager.h"
#include <iostream>
#include "RoomRuleManager.h"
#include "RobotManager.h"
#include "MatchManager.h"

bool CHandleFromGate::HandlePacket(TCP_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (Command.wMainCmdID)
	{
	case MDM_GR_LOGON:		//登录命令
		{
			return OnTCPNetworkMainLogon(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_USER:			//用户命令
		{
			return OnTCPNetworkMainUser(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_G_FRAME:		//框架命令
		{
			return OnTCPNetworkMainFrame(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GF_GAME:		//游戏命令
		{
			return OnTCPNetworkMainGame(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_GR_MATCH:		//比赛命令
		{
			return OnTCPNetworkMainMatch(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	case MDM_CLUB:			//工会
		{
			return OnTCPNetworkMainClub(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	}

	return true;
}

bool CHandleFromGate::HandlePacketDB(WORD wRequestID, DWORD dwSocketID, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
	case DBO_CG_LOGON_USERID:			//ID登录返回
		{
			return On_CMD_GC_Logon_UserID(dwSocketID, pData, wDataSize);
		}
	case DBO_CG_USER_JOIN_GROUP_ROOM:	//加入牌友圈房间返回
		{
			return On_CMD_GC_User_JoinGroupRoom(dwSocketID, pData, wDataSize);
		}
	case DBO_SG_MODIFY_USER_TREASURE:	//修改用户财富信息返回
		{
			return On_CMD_GC_User_ModifyUserTreasure(dwSocketID, pData, wDataSize);
		}
	case DBO_GC_USER_JOIN_TABLE_NO_PASS://加入桌子 不需要密码
		{
			return On_CMD_GC_USER_JOIN_TABLE_NO_PASS(dwSocketID,pData,wDataSize);
		}
	}

	return true;
}

//登录处理
bool CHandleFromGate::OnTCPNetworkMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CG_LOGON_USERID:		//I D 登录
		{
			return On_SUB_CG_Logon_UserID(pData,wDataSize,dwSocketID);
		}
	}

	return true;
}
//用户处理
bool CHandleFromGate::OnTCPNetworkMainUser(WORD wSubCmdID, VOID *pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CG_USER_SITDOWN:		//用户坐下
		{
			return On_SUB_CG_User_SitDown(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_STANDUP:		//用户起立
		{
			return On_SUB_CG_User_StandUp(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_READY:			//用户准备
		{
			return On_SUB_CG_User_Ready(pData, wDataSize, dwSocketID);
		}
	case SUB_CG_USER_INVITE_USER:	//邀请用户
		{
			return On_SUB_CG_User_InviteUser(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_KICK_USER:		//踢出用户
		{
			return On_SUB_CG_User_KickUser(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_CHAIR_REQ:    //请求更换位置
		{
			return OnTCPNetworkSubUserChairReq(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_CREATE_ROOM:	//申请创建房间
		{
			return On_SUB_CG_USER_CREATE_ROOM(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_SET_ROOM_RULE:	//房间规则 设置
		{
			return On_SUB_CG_USER_SET_ROOM_RULE(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_JOIN_FK_ROOM:		//加入桌子 需要密码
		{
			return On_SUB_User_JoinFkRoom(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_JOIN_TABLE_NO_PASS://加入桌子 不需要密码
		{
			return On_SUB_CG_USER_JOIN_TABLE_NO_PASS(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_JOIN_GOLD_HALL_ROOM:	//加入金币大厅桌子
		{
			return On_SUB_CG_USER_JOIN_GOLD_HALL_ROOM(pData,wDataSize,dwSocketID);
		}
	case SUB_RG_USER_ASK_DISMISS:		//发起申请解散房间
		{
			return On_SUB_RG_USER_ASK_DISMISS(pData, wDataSize, dwSocketID);
		}
	case SUB_RG_USER_VOTE_DISMISS:		//表决解散房间
		{
			return On_SUB_RG_USER_VOTE_DISMISS(pData, wDataSize, dwSocketID);
		}
	case SUB_CG_USER_GOLD_INFO:    //请求金币场房间信息
		{
			return On_SUB_CG_USER_GOLD_INFO(pData, wDataSize, dwSocketID);
		}
	}

	return false;
}
//游戏处理
bool CHandleFromGate::OnTCPNetworkMainGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//获取信息
	CPlayer * pIServerUserItem= CPlayerManager::FindPlayerBySocketID(dwSocketID);

	//用户效验
	if (pIServerUserItem==NULL) return false;

	//处理过虑
	DWORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	if ((wTableID==INVALID_TABLE)||(wChairID==INVALID_CHAIR)) return true;

	//消息处理 
	CTableFrame * pTableFrame=CTableManager::FindTableByTableID(wTableID);
	if (pTableFrame == NULL)
	{
		CLog::Log(log_error, "pTableFrame == NULL 未找到桌子");
		return false;
	}

	return pTableFrame->OnEventSocketGame(wSubCmdID,pData,wDataSize,pIServerUserItem);
}

//框架处理
bool CHandleFromGate::OnTCPNetworkMainFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//获取信息
	CPlayer * pIServerUserItem=CPlayerManager::FindPlayerBySocketID(dwSocketID);

	//用户效验
	if (pIServerUserItem==NULL)
	{
		CLog::Log(log_debug, "100, 1 玩家不存在");
		return false;
	}

	//桌子处理
	DWORD wTableID=pIServerUserItem->GetTableID();
	CTableFrame * pTableFrame=CTableManager::FindTableByTableID(wTableID);
	if (pTableFrame == NULL)
	{
		CLog::Log(log_debug, "100, 1 桌子不存在");
		return false;
	}

	return pTableFrame->OnEventSocketFrame(wSubCmdID,pData,wDataSize,pIServerUserItem);
}

//比赛处理
bool CHandleFromGate::OnTCPNetworkMainMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CG_MATCH_INFO:		//请求比赛场信息
		{
			return On_SUB_CG_MATCH_INFO(pData, wDataSize, dwSocketID);
		}
	case SUB_CG_MATCH_APPLY:	//玩家报名
		{
			return On_SUB_CG_MATCH_APPLY(pData, wDataSize, dwSocketID);
		}
	case SUB_CG_MATCH_UNAPPLY:	//取消报名
		{
			return On_SUB_CG_MATCH_UNAPPLY(pData, wDataSize, dwSocketID);
		}
	case SUB_CG_MATCH_QUERY_PLAYER:		//玩家请求信息
		{
			return On_SUB_CG_MATCH_QUERY_PLAYER(pData, wDataSize, dwSocketID);
		}
	case SUB_CG_MATCH_RANKING:	//更新所有人排名
		{
			return On_SUB_CG_MATCH_RANKING(pData, wDataSize, dwSocketID);
		}
	case SUB_CG_MATCH_RANKING_MY:	//更新自己排名
		{
			return On_SUB_CG_MATCH_RANKING_MY(pData, wDataSize, dwSocketID);
		}
	}
}

//工会
bool CHandleFromGate::OnTCPNetworkMainClub(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CG_CLUB_ROOM_CREATE://创建房间
		{
			return On_SUB_CG_CLUB_ROOM_CREATE(pData, wDataSize, dwSocketID);
		}
	case SUB_CG_CLUB_TABLE_LIST_TABLE: //请求该房间的桌子信息
		{
			return On_SUB_CG_CLUB_TABLE_LIST_TABLE(pData, wDataSize, dwSocketID);
		}
	case SUB_CG_CLUB_CREATE_TABLE: //创建桌子
		{
			return On_SUB_CG_CLUB_CREATE_TABLE(pData, wDataSize, dwSocketID);
		}
	}
}

#pragma region 登录模块 MDM_LOGON
//I D 登录
bool CHandleFromGate::On_SUB_CG_Logon_UserID(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	if (wDataSize != sizeof(STR_SUB_CG_LOGON_USERID)) return false;
	STR_SUB_CG_LOGON_USERID *pLogonUserID = (STR_SUB_CG_LOGON_USERID *)pData;
	
	//变量定义
	STR_DBR_CG_LOGON_USERID LogonUserID;
	ZeroMemory(&LogonUserID,sizeof(LogonUserID));

	//构造数据
	LogonUserID.dwUserID = pLogonUserID->dwUserID;
	//LogonUserID.dwClientAddr = pBindParameter->dwClientAddr;
	lstrcpyn(LogonUserID.szPassword,pLogonUserID->szPassword,CountArray(LogonUserID.szPassword));
	lstrcpyn(LogonUserID.szMachineID,pLogonUserID->szMachineID,CountArray(LogonUserID.szMachineID));

	//投递请求
	return g_GameCtrl->PostDataBaseRequest(DBR_CG_LOGON_USERID, dwSocketID, &LogonUserID,sizeof(LogonUserID));
}

//ID登录返回
bool CHandleFromGate::On_CMD_GC_Logon_UserID(DWORD dwSocketID, VOID * pData, WORD wDataSize)
{
	//效验参数
	if (wDataSize != sizeof(STR_DBO_CG_LOGON_USERID)) return false;
	STR_DBO_CG_LOGON_USERID *pDBOLogon = (STR_DBO_CG_LOGON_USERID *)pData;

	//发送登录结果
	STR_CMD_GC_LOGON_USERID logon;
	ZeroMemory(&logon, sizeof(STR_CMD_GC_LOGON_USERID));
	logon.lResultCode = pDBOLogon->lResultCode;
	logon.dwKindID = g_GameCtrl->GetKindID();
	lstrcpyn(logon.szDescribeString, pDBOLogon->szDescribeString, CountArray(logon.szDescribeString));

	//发送数据
	g_GameCtrl->SendData(dwSocketID, MDM_GR_LOGON, CMD_GC_LOGON_USERID, &logon, sizeof(STR_CMD_GC_LOGON_USERID));

	//登录失败, 退出
	if(pDBOLogon->lResultCode != 0)
		return true;
	
	//用户掉线判断
	CPlayer *player = CPlayerManager::FindPlayerByID(pDBOLogon->useInfo.dwUserID);
	if(player == NULL)
	{
		tagUserInfo useInfo;
		ZeroMemory(&useInfo, sizeof(useInfo));
		memcpy(&useInfo, &(pDBOLogon->useInfo), sizeof(useInfo));

		CPlayerManager::InsertPlayer(dwSocketID, useInfo);
	}
	else //断线情形 && 重复登录
	{
		player->SetSocketID(dwSocketID);
	}

	return true;
}

#pragma endregion


/*********************************************【主消息 3 用户命令】*********************************************************/
//用户坐下
bool CHandleFromGate::On_SUB_CG_User_SitDown(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	return true;
}

//用户起立
bool CHandleFromGate::On_SUB_CG_User_StandUp(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//数据包校验
	ASSERT(wDataSize == sizeof(STR_SUB_CG_USER_STANDUP));
	if (wDataSize != sizeof(STR_SUB_CG_USER_STANDUP)) 
		return false;

	//获取用户
	CPlayer *pIServerUserItem = CPlayerManager::FindPlayerBySocketID(dwSocketID);

	//用户效验
	if (pIServerUserItem == NULL) return false;

	//起立数据
	STR_SUB_CG_USER_STANDUP *pUserStandUp = (STR_SUB_CG_USER_STANDUP *)pData;

	//效验桌子和椅子
	if (pUserStandUp->wTableID >= (WORD)CTableManager::TableCount()) 
		return false;

	//桌子号和椅子号校验
	DWORD wTableID = pIServerUserItem->GetTableID();
	WORD wChairID = pIServerUserItem->GetChairID();
	if ((wTableID !=pUserStandUp->wTableID) || (wChairID != pUserStandUp->wChairID)) 
		return true;
		
	//动态加入处理
	bool bDynamicJoin = false;
	if (wTableID != INVALID_TABLE)
	{
		//校验桌子
		CTableFrame *pTableFrame = CTableManager::FindTableByTableID(wTableID);
		if ( NULL == pTableFrame )
			return false;

		//房间规则：金币或电玩模式则动态加入
		tagTableRule *pTableCfg = (tagTableRule*)pTableFrame->GetCustomRule();
		if(pTableCfg != NULL &&  pTableCfg->GameMode == TABLE_MODE_GOLD)
			bDynamicJoin = true;
	}

	//用户判断
	if (!bDynamicJoin && (pIServerUserItem->GetUserStatus()==US_PLAYING))
	{
		SendRequestFailure(pIServerUserItem, TEXT("您正在游戏中，暂时不能离开，请先结束当前游戏！"), REQUEST_FAILURE_NORMAL);
		return true;
	}

	//离开处理
	if (wTableID != INVALID_TABLE)
	{
		//获得桌子
		CTableFrame *pTableFrame = CTableManager::FindTableByTableID(wTableID);
		if (NULL == pTableFrame) 
			return true;
		//用户站起
		if ( !pTableFrame->PlayerUpTable(pIServerUserItem) ) 
			return true;
	}

	return true;
}

//用户准备
bool CHandleFromGate::On_SUB_CG_User_Ready(VOID * pData, WORD wDataSize, DWORD dwSocketID) 
{
	//获取用户
	CPlayer * pIServerUserItem = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	//校验
	if (pIServerUserItem == NULL)
		return false;

	//获取桌子
	CTableFrame *pTableFrame = CTableManager::FindTableByTableID(pIServerUserItem->GetTableID());
	if (pTableFrame == NULL)
		return false;

	//准备处理
	pTableFrame->PlayerReady(pIServerUserItem);

	return true;
}

//邀请用户
bool CHandleFromGate::On_SUB_CG_User_InviteUser(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_GR_UserInviteReq));
	if (wDataSize!=sizeof(CMD_GR_UserInviteReq)) return false;

	//消息处理
	CMD_GR_UserInviteReq * pUserInviteReq=(CMD_GR_UserInviteReq *)pData;

	//获取用户
	CPlayer * pIServerUserItem= CPlayerManager::FindPlayerBySocketID (dwSocketID);
	if (pIServerUserItem==NULL) return false;

	//效验状态
	if (pUserInviteReq->wTableID==INVALID_TABLE) return true;
	if (pIServerUserItem->GetTableID()!=pUserInviteReq->wTableID) return true;
	if (pIServerUserItem->GetUserStatus()==US_PLAYING) return true;
	if (pIServerUserItem->GetUserStatus()==US_OFFLINE) return true;

	//目标用户
	CPlayer * pITargetUserItem=CPlayerManager::FindPlayerByID(pUserInviteReq->dwUserID);
	if (pITargetUserItem==NULL) return true;
	if (pITargetUserItem->GetUserStatus()==US_PLAYING) return true;

	//发送消息
	CMD_GR_UserInvite UserInvite;
	memset(&UserInvite,0,sizeof(UserInvite));
	UserInvite.wTableID=pUserInviteReq->wTableID;
	UserInvite.dwUserID=pIServerUserItem->GetUserID();
	g_GameCtrl->SendData(pITargetUserItem, MDM_USER, CMD_GC_USER_INVITE_USER, &UserInvite, sizeof(UserInvite));

	return true;
}

//踢出用户
bool CHandleFromGate::On_SUB_CG_User_KickUser(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	if (wDataSize!=sizeof(CMD_GR_KickUser)) return false;

	//变量定义
	CMD_GR_KickUser * pKickUser=(CMD_GR_KickUser *)pData;

	//获取用户
	CPlayer * pIServerUserItem=CPlayerManager::FindPlayerBySocketID (dwSocketID);

	//目标用户
	CPlayer * pITargetUserItem = CPlayerManager::FindPlayerByID(pKickUser->dwTargetUserID);
	if(pITargetUserItem==NULL) return true;

	//用户状态
	if(pITargetUserItem->GetUserStatus()==US_PLAYING)
	{
		//变量定义
		TCHAR szMessage[256]=TEXT("");
		_sntprintf_s(szMessage,CountArray(szMessage),TEXT("由于玩家 [ %s ] 正在游戏中,您不能将它踢出游戏！"),pITargetUserItem->GetNickName());

		//发送消息
		g_GameCtrl->SendRoomMessage(pIServerUserItem,szMessage,0);
		return true;
	}

	//请离桌子
	DWORD wTargerTableID = pITargetUserItem->GetTableID();
	if(wTargerTableID != INVALID_TABLE)
	{
		//定义变量
		TCHAR szMessage[64]=TEXT("");
		_sntprintf_s(szMessage,CountArray(szMessage),TEXT("你已被%s请离桌子！"),pIServerUserItem->GetNickName());

		//发送消息
		g_GameCtrl->SendGameMessage(pITargetUserItem,szMessage,0);

		CTableFrame * pTableFrame=CTableManager::FindTableByTableID(wTargerTableID);
		if (pTableFrame->PlayerUpTable(pITargetUserItem)==false) return true;
	}

	return true;
}

//请求更换位置  TODONOW 切换桌子逻辑  WangChengQing
bool CHandleFromGate::OnTCPNetworkSubUserChairReq(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	return true;
}


//发起申请解散房间
bool CHandleFromGate::On_SUB_RG_USER_ASK_DISMISS(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//2、用户校验
	CPlayer *pIServerUserItem = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if (pIServerUserItem == NULL)
		return false;

	//3、房间校验
	CTableFrame *pTableFrame = CTableManager::FindTableByTableID(pIServerUserItem->GetTableID());
	if (pTableFrame == NULL)
		return false;

	STR_SUB_RG_FRAME_ASK_DISMISS *pApply = (STR_SUB_RG_FRAME_ASK_DISMISS*)pData;

	CLog::Log(log_debug, "DISMISS UserID: %d", pIServerUserItem->GetUserID());

	//2. 校验房间是否已经处于解散状态
	if (pTableFrame->m_bUnderDissState)
	{
		return true;
	}

	//重新点击解散按钮，需要删除定时器，防止一个人不停点击解散，定时器到自动退出房间
	pTableFrame->KillVoteDismissRoom();

	pTableFrame->OnEventApplyDismissRoom(pIServerUserItem->GetChairID(), true); //0为false  其他为true

	return true;
}

//表决解散房间
bool CHandleFromGate::On_SUB_RG_USER_VOTE_DISMISS(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//1、数据包校验
	if (sizeof(STR_SUB_RG_FRAME_VOTE_DISMISS) != wDataSize)
		return false;

	//2、用户校验
	CPlayer *pIServerUserItem = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if (pIServerUserItem == NULL)
		return false;

	//3、房间校验
	CTableFrame *pTableFrame = CTableManager::FindTableByTableID(pIServerUserItem->GetTableID());
	if (pTableFrame == NULL)
		return false;

	//数据包校验
	if (wDataSize != sizeof(STR_SUB_RG_FRAME_VOTE_DISMISS))
		return false;

	//变量定义
	STR_SUB_RG_FRAME_VOTE_DISMISS *pVote = (STR_SUB_RG_FRAME_VOTE_DISMISS*)pData;

	//解散成功
	pTableFrame->OnEventVoteDismissRoom(pIServerUserItem->GetChairID(), (pVote->cbAgree != 0));
	return true;
}

/***************************************【主消息 2】*******************************************************/
//申请创建房间
bool CHandleFromGate::On_SUB_CG_USER_CREATE_ROOM(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//获取用户
	CPlayer *player = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if (player==NULL)  return false;

	//校验数据包
	if(wDataSize != sizeof(STR_SUB_CG_USER_CREATE_ROOM)) return false;

	//创建房间 校验
	STR_SUB_CG_USER_CREATE_ROOM *pCreateRoom = (STR_SUB_CG_USER_CREATE_ROOM *)pData;
	if( !CheckCreateRoom(player, pCreateRoom->byGameMode))
	{
		SendRequestFailure(player, TEXT("创建房间 失败"), REQUEST_FAILURE_NORMAL);
		return true;
	}

	//发送房间规则选择 给client
	g_GameCtrl->SendData(player, MDM_USER, CMD_GC_USER_GET_ROOM_RULE, &RoomRuleManager::GetRoomRuleSetting(), sizeof(rule_arry));

	return true;
}

//设置房间规则
bool CHandleFromGate::On_SUB_CG_USER_SET_ROOM_RULE(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//获取用户
	CPlayer *player = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if (player==NULL)  return false;

	//校验数据包
	if(wDataSize != sizeof(STR_SUB_CG_USER_SET_ROOM_RULE)) return false;
	STR_SUB_CG_USER_SET_ROOM_RULE *pCmd = (STR_SUB_CG_USER_SET_ROOM_RULE *)pData;

	//获取完整房间规则
	tagTableRule * cfg = &RoomRuleManager::GetFKRoomRule(pCmd->byChoose, pCmd->byGameMode);

	//门票校验
	if(0 != RoomRuleManager::CheckTickt(cfg, player))
	{
		g_GameCtrl->SendDataMsg(dwSocketID, "门票不符");
		return true;
	}

	//创建桌子
	CTableFrame *pTableFrame = CTableManager::CreateTable(cfg, player->GetUserID());
	if(NULL == pTableFrame)
	{
		g_GameCtrl->SendDataMsg(dwSocketID, "创建失败");
		return true ; 
	}
	
	//用户坐下
	if(pTableFrame->PlayerSitTable(player) != 0)
	{
		g_GameCtrl->SendDataMsg(dwSocketID, "坐下失败");
		return false;
	}

	//发送创建房间成功消息
	g_GameCtrl->SendData(player, MDM_USER, CMD_GC_USER_ENTER_SUBGAME_ROOM, NULL, 0);

	return true;
}

//Club牌友圈房间信息 写入数据库
bool CHandleFromGate::WriteClubRoomToDB(STR_DBR_CLUB_ROOM_INFO* pTableInfo)
{
	//无效过滤
	if (pTableInfo==NULL) return false;

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CLUB_ROOM_INFO, 0, pTableInfo, sizeof(STR_DBR_CLUB_ROOM_INFO));

	return true;
}

//加入桌子 -- 需要密码
bool CHandleFromGate::On_SUB_User_JoinFkRoom(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验用户
	CPlayer *player = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if (NULL == player)
	{
		return false;
	}

	//校验数据包
	if(wDataSize != sizeof(STR_SUB_CG_USER_JOIN_FK_ROOM)) 
	{
		return false;
	}

	//数据定义
	STR_SUB_CG_USER_JOIN_FK_ROOM *pJoin = (STR_SUB_CG_USER_JOIN_FK_ROOM *)pData;

	//房间校验
	CTableFrame* pTable = CTableManager::FindTableByTableID(pJoin->dwPassword);
	if(!pTable)
	{
		g_GameCtrl->SendDataMsg(dwSocketID, "房间不存在");
		return true;
	}

	//门票检测
	if(0 != RoomRuleManager::CheckTickt(pTable->GetCustomRule(), player))
	{
		g_GameCtrl->SendDataMsg(dwSocketID, "门票不足");
		return true;
	}

	//玩家坐下
	if(pTable->PlayerSitTable(player) != 0)
	{
		g_GameCtrl->SendDataMsg(dwSocketID, "坐下失败");
		return true;
	}	

	//发送加入房间成功
	g_GameCtrl->SendData(player, MDM_USER, CMD_GC_USER_ENTER_SUBGAME_ROOM, NULL, 0);
	return true;
}

//加入桌子 -- 不需要密码, 即快速开始
bool CHandleFromGate::On_SUB_CG_USER_JOIN_TABLE_NO_PASS(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验用户
	CPlayer *pIServerUserItem = CPlayerManager::FindPlayerBySocketID (dwSocketID);
	if (NULL == pIServerUserItem) return false;

	//校验数据包
	if(wDataSize != sizeof(STR_SUB_CG_USER_JOIN_TABLE_NO_PASS))
	{
		SendRequestFailure(pIServerUserItem,TEXT("加入房间 结构体数据错误！"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//数据定义
	STR_SUB_CG_USER_JOIN_TABLE_NO_PASS *pJoin = (STR_SUB_CG_USER_JOIN_TABLE_NO_PASS *)pData;

	//投递请求
	return g_GameCtrl->PostDataBaseRequest(DBR_CG_USER_JOIN_TABLE_NO_PASS, dwSocketID, pData,wDataSize);
}

//加入桌子 -- 不需要密码, 即快速开始
bool CHandleFromGate::On_CMD_GC_USER_JOIN_TABLE_NO_PASS( DWORD dwSocketID, VOID * pData, WORD wDataSize)
{
	/* 1. 校验用户 */
	CPlayer *pIServerUserItem = CPlayerManager::FindPlayerBySocketID (dwSocketID);
	if (NULL == pIServerUserItem) return false;

	/* 2. 校验数据包 */
	if(wDataSize != sizeof(STR_DBO_CG_USER_JOIN_TABLE_NO_PASS))
	{
		SendRequestFailure(pIServerUserItem,TEXT("加入房间数据错误！"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 3. 数据库校验 */

	STR_DBO_CG_USER_JOIN_TABLE_NO_PASS *pJoin = (STR_DBO_CG_USER_JOIN_TABLE_NO_PASS *)pData;
	if( pJoin->lResultCode != DB_SUCCESS)//返回值不为0, 则认为身上金币不足 或者 不是本公司的人
	{
		SendRequestFailure(pIServerUserItem,TEXT("未找到合适房间, 请稍后重试"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	if( pJoin->lResultCode2 != DB_SUCCESS)//返回值不为0, 则工会房卡不足
	{
		SendRequestFailure(pIServerUserItem,TEXT("工会房卡不足, 请联系管理员充值"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	if( pJoin->dwPassword == 0)//桌子号为空, 则认为没有找到对应的桌子
	{
		//如果没有找到俱乐部的桌子, 则直接创建一个桌子
		//return CreateTableAutoClub(pJoin ,  pIServerUserItem);
	}


	/* 4. 桌子校验 */
	DWORD dwPassword = pJoin->dwPassword;
	if(dwPassword == 0)
	{
		SendRequestFailure(pIServerUserItem, TEXT("桌子号错误,请重新尝试"), REQUEST_FAILURE_PASSWORD);
		return false;
	}

	/* 5. 校验是否在之前的游戏中 */
	DWORD wOldTableID = pIServerUserItem->GetTableID(); //旧桌子号	
	if(wOldTableID != INVALID_TABLE)
	{
		if(wOldTableID > CTableManager::TableCount())
		{
			wOldTableID = INVALID_TABLE;
		}
		else
		{
			CTableFrame* pOldTable = CTableManager::FindTableByTableID(wOldTableID);
			if(pOldTable == NULL || pOldTable->GetGameStatus() != GAME_STATUS_PLAY)
			{
				wOldTableID = INVALID_TABLE;
			}
		}	
	}

	if (INVALID_TABLE != wOldTableID)
	{
		SendRequestFailure(pIServerUserItem,TEXT("您已经在游戏中,不能进入其他房间"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 6. 寻找指定桌子 */
	CTableFrame *pCurrTableFrame = CTableManager::FindTableByTableID(dwPassword); 
	if(NULL == pCurrTableFrame)
	{
		SendRequestFailure(pIServerUserItem, TEXT("加入失败, 桌子不存在"), REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 7. 房间规则 */
	tagTableRule * pCfg = (tagTableRule*)pCurrTableFrame->GetCustomRule();
	if (NULL == pCfg)
	{
		SendRequestFailure(pIServerUserItem, TEXT("加入失败, 房间规则不存在!"), REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 8. 判断桌子是否在游戏中 */
	if(GAME_STATUS_FREE != pCurrTableFrame->GetGameStatus())
	{
		SendRequestFailure(pIServerUserItem, TEXT("房间正在游戏中,无法加入！"), REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 9. 门票校验 */
	if(!CheckJoinTableTicket(pCfg, pIServerUserItem))
	{
		return false;
	}

	/* 10. 用户坐下 */
	if(pCurrTableFrame->PlayerSitTable(pIServerUserItem) != 0)
	{
		SendRequestFailure(pIServerUserItem,TEXT("加入失败, 坐下失败!"),REQUEST_FAILURE_PASSWORD);
		return false;
	}


	//发送加入房卡房间成功
	g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_ENTER_SUBGAME_ROOM, NULL, 0);

	return true;
}

//加入桌子 -- 金币大厅桌子
bool CHandleFromGate::On_SUB_CG_USER_JOIN_GOLD_HALL_ROOM(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验用户
	CPlayer *player = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if (NULL == player) return false;

	//校验数据包
	if(wDataSize != sizeof(STR_SUB_CG_USER_JOIN_GOLD_HALL_ROOM))
	{
		return false;
	}

	//数据定义
	STR_SUB_CG_USER_JOIN_GOLD_HALL_ROOM *pJoin = (STR_SUB_CG_USER_JOIN_GOLD_HALL_ROOM *)pData;

	//门票检测
	if(0 != RoomRuleManager::CheckTickt(&RoomRuleManager::GetGoldRoomRule(pJoin->byType), player))
	{
		g_GameCtrl->SendDataMsg(dwSocketID, "金币不足");
		return true;
	}
	
	//获取table
	CTableFrame *pTable = CTableManager::GetGlodTable(pJoin->byType);
	if(pTable == NULL)
	{
		g_GameCtrl->SendDataMsg(dwSocketID, "没有桌子");
		return true;
	}

	//用户坐下
	if(pTable->PlayerSitTable(player) != 0)
	{
		g_GameCtrl->SendDataMsg(dwSocketID, "坐下失败");
		return false;
	}

	//发送加入房间成功
	g_GameCtrl->SendData(player, MDM_USER, CMD_GC_USER_ENTER_SUBGAME_ROOM, NULL, 0);
	return true;
}

//加入牌友圈房间返回
bool CHandleFromGate::On_CMD_GC_User_JoinGroupRoom(DWORD dwSocketID, VOID * pData, WORD wDataSize)
{
	//判断在线

	//数据校验
	ASSERT(wDataSize == sizeof(STR_DBO_CG_USER_JOIN_GROUP_ROOM));
	if(wDataSize != sizeof(STR_DBO_CG_USER_JOIN_GROUP_ROOM))
		return false;

	//获取用户
	CPlayer *pIServerUserItem = CPlayerManager::FindPlayerBySocketID (dwSocketID);
	if ( NULL == pIServerUserItem )
	{
		return false;
	}

	//定义
	STR_DBO_CG_USER_JOIN_GROUP_ROOM *pDBOGroup = (STR_DBO_CG_USER_JOIN_GROUP_ROOM *)pData;

	//失败返回
	if ( DB_SUCCESS != pDBOGroup->dwResultCode )
	{
		SendRequestFailure(pIServerUserItem, pDBOGroup->szDescribeString, pDBOGroup->dwResultCode);
		return true;
	}

	//判断该圈子的房间是否有空椅子的桌子
	bool bHaveRoom = false;
	for(size_t i= 0;i < CTableManager::TableCount();i++)
	{
		CTableFrame *pTableFrame; // = CTableManager::FindTableByIndex(i);
		//校验桌子
		if( (NULL != pTableFrame) && 
		//	(pTableFrame->GetGroupID() == pDBOGroup->dwGroupID) && 
			(GAME_STATUS_FREE == pTableFrame->GetGameStatus()) && 
			(0 != pTableFrame->GetTableID()) )
		{
							//用户坐下
				if(pTableFrame->PlayerSitTable( pIServerUserItem) != 0)
				{
					SendRequestFailure(pIServerUserItem, TEXT("房间号不对!"), REQUEST_FAILURE_PASSWORD);
					return true;
				}	

								//用户可以加入牌友圈房间
				bHaveRoom = true;

				//发送加入成功消息
				g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_ENTER_SUBGAME_ROOM, NULL, 0);
				break;
		}
	}

	//用户无法加入，发送错误提示
	if ( !bHaveRoom )
	{
		SendRequestFailure(pIServerUserItem, TEXT("牌友圈房间已满或不存在已开房间，请联系管理"), REQUEST_FAILURE_PASSWORD);
	}

	return true;
}

//修改用户财富信息
bool CHandleFromGate::On_CMD_GC_User_ModifyUserTreasure(DWORD dwSocketID, VOID * pData, WORD wDataSize)
{
	//这里不能校验socketID,因为传入的就是0
	//数据校验
	ASSERT(wDataSize == sizeof(STR_DBO_GR_MODIFY_USER_TREASURE));
	if(wDataSize != sizeof(STR_DBO_GR_MODIFY_USER_TREASURE))
		return false;

	//定义
	STR_DBO_GR_MODIFY_USER_TREASURE *pDBOModify = (STR_DBO_GR_MODIFY_USER_TREASURE *)pData;

	//修改失败，直接返回
	if ( DB_SUCCESS != pDBOModify->dwResultCode )
	{
		return true;
	}

	//获取用户
	CPlayer *pIServerUserItem = CPlayerManager::FindPlayerByID(pDBOModify->dwUserID);

	//用户检验
	if ( NULL == pIServerUserItem )
	{
		return true;
	}
	
	//构造返回数据
	STR_CMD_GC_USER_MODIFY_TREASURE CMDModify;
	ZeroMemory(&CMDModify, sizeof(STR_CMD_GC_USER_MODIFY_TREASURE));

	//赋值
	CMDModify.lOpenRoomCard = pDBOModify->lOpenRoomCard;
	CMDModify.lUserDiamond = pDBOModify->lUserDiamond;
	CMDModify.lUserGold = pDBOModify->lUserGold;

	//发送修改成功消息
	g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_MODIFY_TREASURE, &CMDModify, sizeof(STR_CMD_GC_USER_MODIFY_TREASURE));

	//修改成功后，修改内存中的用户数据
	pIServerUserItem->SetUserRoomCard(CMDModify.lOpenRoomCard);
	pIServerUserItem->SetUserGold(CMDModify.lUserGold);
	pIServerUserItem->SetUserDiamond(CMDModify.lUserDiamond);

	return true;
}

//请求失败
bool CHandleFromGate::SendRequestFailure(CPlayer * pIServerUserItem, LPCTSTR pszDescribe, LONG lErrorCode)
{
	//变量定义
	CMD_GR_RequestFailure RequestFailure;
	ZeroMemory(&RequestFailure,sizeof(RequestFailure));

	//设置变量
	RequestFailure.lErrorCode=lErrorCode;
	lstrcpyn(RequestFailure.szDescribeString,pszDescribe,CountArray(RequestFailure.szDescribeString));

	g_GameCtrl->SendData(pIServerUserItem, MDM_USER, SUB_GR_REQUEST_FAILURE, &RequestFailure, sizeof(CMD_GR_RequestFailure));

	std::wcout << "HFG: " << pszDescribe << std::endl;

	return true;
}


//请求金币房间信息
bool CHandleFromGate::On_SUB_CG_USER_GOLD_INFO(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//列表发送
	WORD wPacketSize=0;
	BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
	wPacketSize=0;
	STR_CMD_GC_USER_GOLD_INFO * pDBO=NULL;

	for (auto item : RoomRuleManager::GetAllRoomInfo())
	{
		//发送信息
		if ((wPacketSize+sizeof(STR_CMD_GC_USER_GOLD_INFO))>sizeof(cbBuffer))
		{
			g_GameCtrl->SendData(dwSocketID, MDM_USER, CMD_GC_USER_GOLD_INFO,  cbBuffer, wPacketSize);
			wPacketSize=0;
		}
		//读取信息
		pDBO=(STR_CMD_GC_USER_GOLD_INFO *)(cbBuffer+wPacketSize);
		memcpy(pDBO, &item, sizeof(STR_CMD_GC_USER_GOLD_INFO));

		//设置位移
		wPacketSize+=sizeof(STR_CMD_GC_USER_GOLD_INFO);
	}
	if (wPacketSize>0) g_GameCtrl->SendData(dwSocketID, MDM_USER, CMD_GC_USER_GOLD_INFO,  cbBuffer, wPacketSize);

	g_GameCtrl->SendData(dwSocketID, MDM_USER, CMD_GC_USER_GOLD_INFO_FINISH,  NULL, 0);
	return true;
}

#pragma region Club牌友圈2 事件通知(1.桌子信息 2.玩家信息)
//创建房间
bool CHandleFromGate::On_SUB_CG_CLUB_ROOM_CREATE(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验参数
	CPlayer * player = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if(player == NULL) return false;
	if(wDataSize != sizeof(STR_SUB_CL_CLUB_ROOM_CREATE)) return false;
	STR_SUB_CL_CLUB_ROOM_CREATE *pSub = (STR_SUB_CL_CLUB_ROOM_CREATE*) pData;

	//获取到房间规则
	tagTableRule roomRule = RoomRuleManager::GetFKRoomRule(pSub->byChoose, TABLE_MODE_CLUB);
	string  roomRuleMsg = "游戏局数: " + std::to_string(roomRule.GameCount) + "\n" + "椅子数: " + std::to_string(roomRule.PlayerCount);
	string  RealRoomRule = RoomRuleManager::GetRuleHexString(pSub->byChoose);

	//写入数据库
	g_TreasureDB->ResetParameter();
	g_TreasureDB->AddParameter(TEXT("@RoomName"),pSub->setting.szRoomName);
	g_TreasureDB->AddParameter(TEXT("@ClubID"),pSub->setting.dwClubID);
	g_TreasureDB->AddParameter(TEXT("@KindID"),pSub->setting.dwKindID);
	g_TreasureDB->AddParameter(TEXT("@KindName")," ");
	g_TreasureDB->AddParameter(TEXT("@ModeID"),pSub->setting.byGoldOrFK);
	g_TreasureDB->AddParameter(TEXT("@PlayerNum"),0);
	g_TreasureDB->AddParameter(TEXT("@AllRound"),roomRule.GameCount);
	g_TreasureDB->AddParameter(TEXT("@ChairNum"),roomRule.PlayerCount);
	
	g_TreasureDB->AddParameter(TEXT("@RoomRule"), roomRuleMsg.c_str()); //房间规则描述
	g_TreasureDB->AddParameter(TEXT("@RealRoomRule"), RealRoomRule.c_str()); //房间规则value

	g_TreasureDB->AddParameter(TEXT("@Dissolve"),pSub->setting.bDissolve);	//是否可以解散
	g_TreasureDB->AddParameter(TEXT("@Gold"),pSub->setting.dwGold);			//入场最低身价
	g_TreasureDB->AddParameter(TEXT("@Revenue"),pSub->setting.dwOwnerPercentage); //固定抽税金额

	g_TreasureDB->AddParameter(TEXT("@Mask"),pSub->setting.byMask);			//1 AA支付  2大赢家支付
	g_TreasureDB->AddParameter(TEXT("@Dizhu"),pSub->setting.dwDizhu);		//低分
	g_TreasureDB->AddParameter(TEXT("@ServiceGold"),pSub->setting.dwAmount); //抽水下限


	long ResultCode = g_TreasureDB->ExecuteProcess(TEXT("GSP_CG_CLUB_CREATE_ROOM"),true);

	STR_CMD_LC_CLUB_ROOM_CREATE cmd;
	cmd.lResultCode = ResultCode;

	g_GameCtrl->SendData(dwSocketID, MDM_CLUB, CMD_LC_CLUB_ROOM_CREATE, &cmd, sizeof(cmd));
	return true;
}

//请求该房间的桌子信息
bool CHandleFromGate::On_SUB_CG_CLUB_TABLE_LIST_TABLE(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	if (wDataSize!=sizeof(STR_SUB_CG_CLUB_TABLE_LIST_TABLE)) return false;
	STR_SUB_CG_CLUB_TABLE_LIST_TABLE * pSub = (STR_SUB_CG_CLUB_TABLE_LIST_TABLE*)pData;

	//列表发送
	WORD wPacketSize=0;
	BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
	wPacketSize=0;
	STR_CMD_LC_CLUB_TABLE_LIST * pCMD=NULL;
	for (auto table : CTableManager::GetAllClubRoomTable(pSub->dwClubID, pSub->dwRoomID))
	{
		tagClubRoomRule *rule = table->GetClubRoomRule();

		//发送信息
		if ((wPacketSize+sizeof(STR_CMD_LC_CLUB_TABLE_LIST))>sizeof(cbBuffer))
		{
			g_GameCtrl->SendData(dwSocketID, MDM_CLUB, CMD_GC_CLUB_TABLE_LIST_TABLE, cbBuffer, wPacketSize);
			wPacketSize=0;
		}

		//读取信息
		pCMD=(STR_CMD_LC_CLUB_TABLE_LIST *)(cbBuffer+wPacketSize);

		pCMD->byDel = 0;
		pCMD->dwClubID = rule->dwClubID;
		pCMD->dwRoomID = rule->dwRoomID;
		pCMD->dwTableID = table->GetTableID();
		
		pCMD->TableState = (table->GetGameStatus() == GAME_STATUS_FREE) ?  0 : 1;
		pCMD->LockState = (rule->dwPasswd != 0) ? 0 : 1;
		pCMD->CurrentRound = table->GetCurGameRound();
		pCMD->AllRound = table->GetCustomRule()->GameCount;
		pCMD->ChairCount = table->GetCustomRule()->PlayerCount;

		STR_CMD_LC_CLUB_TABLE_USER_LIST * player_info;
		int i = 0;
		for(auto player : table->GetPlayer_list())
		{
			if(player == NULL) continue;
			if(i >= 6) continue;
			player_info->dwUserID = player->GetUserID();
			memcpy(player_info->szUserName, player->GetNickName(), sizeof(TCHAR) * LEN_NICKNAME);
			player_info->bySex = player->GetSex();
			memcpy(player_info->szHeadUrl, player->GetUserInfo()->szHeadUrl, sizeof(TCHAR) * LEN_HEAD_URL);
			player_info->wLevel = player->GetUserInfo()->dwLevel;
			//BYTE	byClubRole;									//玩家职务 0 群主; 1管理; 2一般成员
			//BYTE	byClubReputation;							//玩家在工会中的声望等级
			player_info->byChairID = player->GetChairID();
		}
		//设置位移
		wPacketSize+=sizeof(STR_CMD_LC_CLUB_TABLE_LIST);
	}
	if (wPacketSize>0) g_GameCtrl->SendData(dwSocketID, MDM_CLUB, CMD_GC_CLUB_TABLE_LIST_TABLE, cbBuffer, wPacketSize);

	return true;
}

//创建工会桌子
bool CHandleFromGate::On_SUB_CG_CLUB_CREATE_TABLE(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	CPlayer* player = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if(player == NULL) return false;
	if (wDataSize!=sizeof(STR_SUB_CG_CLUB_CREATE_TABLE)) return false;
	STR_SUB_CG_CLUB_CREATE_TABLE * pSub = (STR_SUB_CG_CLUB_CREATE_TABLE*)pData;

	tagTableRule cfg = RoomRuleManager::GetClubComRoomRule(pSub->dwClubID, pSub->dwRoomID);
	tagClubRoomRule clubCfg = RoomRuleManager::GetClubRoomRule(pSub->dwClubID, pSub->dwRoomID, pSub->dwPasswd);

	CTableFrame* table = CTableManager::CreateClubTable(&cfg, &clubCfg);
	if(table == NULL)
	{
		g_GameCtrl->SendDataMsg(dwSocketID, "获取桌子失败");
		return false;
	}

	//用户坐下
	if(table->PlayerSitTable(player) != 0)
	{
		g_GameCtrl->SendDataMsg(dwSocketID, "坐下失败");
		return false;
	}

	//发送创建房间成功消息
	g_GameCtrl->SendData(player, MDM_USER, CMD_GC_USER_ENTER_SUBGAME_ROOM, NULL, 0);

	return true;
}

//club 桌子开始游戏
void CHandleFromGate::ClubTableStart(DWORD dwTableID, BYTE byClubOrHallGold)
{
	//俱乐部桌子信息  备注: 桌子开始游戏后, 更新当前局数为 (0 + 1)
	if(1 == byClubOrHallGold)
	{
		STR_DBR_CLUB_TABLE_INFO Dbr;
		ZeroMemory(&Dbr, sizeof(Dbr));
		Dbr.dwTableID = dwTableID;
		Dbr.dwTableState = 1;
		Dbr.byCurrentRound = 1;//这是一个增量 TODONOW 后面修改
		Dbr.byMask = 2;

		g_GameCtrl->PostDataBaseRequest(DBR_CLUB_TABLE_INFO,0,&Dbr,sizeof(Dbr));
	}

	//金币大厅桌子信息
	if(2 == byClubOrHallGold)
	{
		STR_DBR_HALL_GOLD_TABLE_INFO HallDbr;
		ZeroMemory(&HallDbr, sizeof(HallDbr));
		HallDbr.dwTableID = dwTableID;
		HallDbr.dwTableState = 1;
		HallDbr.byMask = 2;

		g_GameCtrl->PostDataBaseRequest(DBR_HALL_GOLD_TABLE_INFO,0,&HallDbr,sizeof(HallDbr));
	}
}

//club 桌子已经满员
void CHandleFromGate::ClubTableMax(DWORD dwTableID, BYTE byClubOrHallGold)
{
	//备注: 桌子开始游戏后, 更新当前局数为 (0 + 1)
	if(1 == byClubOrHallGold)
	{
		STR_DBR_CLUB_TABLE_INFO Dbr;
		ZeroMemory(&Dbr, sizeof(Dbr));
		Dbr.dwTableID = dwTableID;
		Dbr.dwTableState = 2;
		Dbr.byMask = 2;

		g_GameCtrl->PostDataBaseRequest(DBR_CLUB_TABLE_INFO,0,&Dbr,sizeof(Dbr));
	}

	//金币大厅桌子信息
	if(2 == byClubOrHallGold)
	{
		STR_DBR_HALL_GOLD_TABLE_INFO HallDbr;
		ZeroMemory(&HallDbr, sizeof(HallDbr));
		HallDbr.dwTableID = dwTableID;
		HallDbr.dwTableState = 2;
		HallDbr.byMask = 2;

		g_GameCtrl->PostDataBaseRequest(DBR_HALL_GOLD_TABLE_INFO,0,&HallDbr,sizeof(HallDbr));
	}
}

//club 桌子没有满员
void CHandleFromGate::ClubTableNotMax(DWORD dwTableID, BYTE byClubOrHallGold)
{
	//备注: 桌子开始游戏后, 更新当前局数为 (0 + 1)
	if(1 ==  byClubOrHallGold)
	{
		STR_DBR_CLUB_TABLE_INFO Dbr;
		ZeroMemory(&Dbr, sizeof(Dbr));
		Dbr.dwTableID = dwTableID;
		Dbr.dwTableState = 0;
		Dbr.byMask = 2;

		g_GameCtrl->PostDataBaseRequest(DBR_CLUB_TABLE_INFO,0,&Dbr,sizeof(Dbr));
	}

	//金币大厅桌子信息
	if( 2 ==  byClubOrHallGold)
	{
		STR_DBR_HALL_GOLD_TABLE_INFO HallDbr;
		ZeroMemory(&HallDbr, sizeof(HallDbr));
		HallDbr.dwTableID = dwTableID;
		HallDbr.dwTableState = 0;
		HallDbr.byMask = 2;

		g_GameCtrl->PostDataBaseRequest(DBR_HALL_GOLD_TABLE_INFO,0,&HallDbr,sizeof(HallDbr));
	}
}

//club 桌子小局结束
void CHandleFromGate::ClubTableXJ(DWORD dwTableID)
{
	//备注: 桌子小局结束后, 更新当前局数为 (数据库 + 1)
	STR_DBR_CLUB_TABLE_INFO Dbr;
	ZeroMemory(&Dbr, sizeof(Dbr));
	Dbr.dwTableID = dwTableID;
	Dbr.dwTableState = 1;
	Dbr.byCurrentRound = 1;//这是一个增量 TODONOW 后面修改
	Dbr.byMask = 2;
	
	g_GameCtrl->PostDataBaseRequest(DBR_CLUB_TABLE_INFO,0,&Dbr,sizeof(Dbr));
}

//club 桌子大局结束
void CHandleFromGate::ClubTableDJ(DWORD dwTableID)
{
    //备注: 桌子大局结束, 解散桌子
    STR_DBR_CLUB_TABLE_INFO Dbr;
	ZeroMemory(&Dbr, sizeof(Dbr));
	Dbr.dwTableID = dwTableID;
	Dbr.byMask = 3;
	
	g_GameCtrl->PostDataBaseRequest(DBR_CLUB_TABLE_INFO,0,&Dbr,sizeof(Dbr));
}

#pragma endregion

//用户坐下
void CHandleFromGate::ClubPlayerSitDown(DWORD dwTableID, DWORD dwUserID, BYTE byChairID, BYTE byClubOrHallGold)
{
	//俱乐部桌子
	if(1 == byClubOrHallGold)
	{
		STR_DBR_CLUB_PLAYER_INFO Dbr;
		ZeroMemory(&Dbr, sizeof(Dbr));
		Dbr.dwTableID = dwTableID;
		Dbr.dwUserID = dwUserID;
		Dbr.byChairID = byChairID;
		Dbr.byMask = 1; 

		g_GameCtrl->PostDataBaseRequest(DBR_CLUB_PLAYER_INFO,0,&Dbr,sizeof(Dbr));
	}

	//金币大厅桌子
	if(2 ==  byClubOrHallGold)
	{
		STR_DBR_HALL_GOLD_PLAYER_INFO HallDbr;
		ZeroMemory(&HallDbr, sizeof(HallDbr));
		HallDbr.dwTableID = dwTableID;
		HallDbr.dwUserID = dwUserID;
		HallDbr.byChairID = byChairID;
		HallDbr.byMask = 1; 

		g_GameCtrl->PostDataBaseRequest(DBR_HALL_GOLD_PLAYER_INFO,0,&HallDbr,sizeof(HallDbr));
	}
}

//用户起立
void CHandleFromGate::ClubPlayerSitUp(DWORD dwTableID, DWORD dwUserID, BYTE byChairID, BYTE byClubOrHallGold)
{
	//俱乐部桌子
	if( 1 == byClubOrHallGold)
	{
		STR_DBR_CLUB_PLAYER_INFO Dbr;
		ZeroMemory(&Dbr, sizeof(Dbr));
		Dbr.dwTableID = dwTableID;
		Dbr.dwUserID = dwUserID;
		Dbr.byMask = 2; 

		g_GameCtrl->PostDataBaseRequest(DBR_CLUB_PLAYER_INFO,0,&Dbr,sizeof(Dbr));
	}

	//金币大厅
	if( 2 == byClubOrHallGold)
	{
		STR_DBR_HALL_GOLD_PLAYER_INFO HallDbr;
		ZeroMemory(&HallDbr, sizeof(HallDbr));
		HallDbr.dwTableID = dwTableID;
		HallDbr.dwUserID = dwUserID;
		HallDbr.byChairID = byChairID;
		HallDbr.byMask = 2; 

		g_GameCtrl->PostDataBaseRequest(DBR_HALL_GOLD_PLAYER_INFO,0,&HallDbr,sizeof(HallDbr));
	}
}

//最后一个用户起立
void CHandleFromGate::ClubLastPlayerSitUp(DWORD dwTableID, DWORD dwUserID, BYTE byChairID, BYTE byClubOrHallGold)
{
	//俱乐部桌子
	if( 1 == byClubOrHallGold)
	{
		STR_DBR_CLUB_TABLE_INFO Dbr;
		ZeroMemory(&Dbr, sizeof(Dbr));
		Dbr.dwTableID = dwTableID;
		Dbr.byMask = 3;

		g_GameCtrl->PostDataBaseRequest(DBR_CLUB_TABLE_INFO,0,&Dbr,sizeof(Dbr));
	}

	//金币大厅桌子
	if( 2 == byClubOrHallGold)
	{
		STR_DBR_HALL_GOLD_TABLE_INFO HallDbr;
		ZeroMemory(&HallDbr, sizeof(HallDbr));
		HallDbr.dwTableID = dwTableID;
		HallDbr.byMask = 3;

		g_GameCtrl->PostDataBaseRequest(DBR_HALL_GOLD_TABLE_INFO,0,&HallDbr,sizeof(HallDbr));
	}
}

#pragma region 辅助函数
//********************************************辅助函数*******************************************//

//校验是否可以创建房间
bool CHandleFromGate::CheckCreateRoom(CPlayer * player, BYTE gameMode)
{
	//用户效验
	if (INVALID_CHAIR != player->GetChairID()) 
	{
		SendRequestFailure(player, TEXT("正在游戏中,无法创建房间！"), REQUEST_FAILURE_NORMAL);
		return false;
	}
	if(INVALID_TABLE != player->GetTableID())
	{
		SendRequestFailure(player, TEXT("正在房间中,无法创建房间！"), REQUEST_FAILURE_NORMAL);
		return false;
	}

	switch(gameMode)
	{
	case TABLE_MODE_FK:
		{
			return true;
		}
	case TABLE_MODE_MATCH:
		{
			return true;
		}
	case TABLE_MODE_GOLD:
		{
			return true;
		}
	case TABLE_MODE_FK_GOLD:
		{
			//校验门票
			return true;
		}
	case TABLE_MODE_CLUB:
		{
			//校验
			return true;
		}
	}

	return false;
}

//检查加入桌子门票
bool CHandleFromGate::CheckJoinTableTicket(tagTableRule *pCfg, CPlayer *pIServerUserItem)
{
	//内部调用, 不校验指针
	return true;

	switch( pCfg->GameMode )
	{
	case TABLE_MODE_FK://房卡模式
		{
			//大厅的房卡场：AA支付，且用户房卡不足
			/*
			if( (1 == pCfg->cbPayType) )
			{
				SendRequestFailure(pIServerUserItem, TEXT("您正在进入AA支付的房卡房, 因房卡不足,无法加入"), REQUEST_FAILURE_NORMAL);
				return false;
			}
			*/
			break;
		}
	case TABLE_MODE_GOLD://金币模式
		{
			break;
		}
	case TABLE_MODE_FK_GOLD://房卡金币场
		{
			//大厅的房卡金币场  校验一:  AA支付，且用户房卡不足
			/*
			if( (1 == pCfg->cbPayType) && 
				(pIServerUserItem->GetUserRoomCard() < pCfg->GameCountType) )
			{
				SendRequestFailure(pIServerUserItem, TEXT("您正在进入的为AA支付的房卡金币房, 因房卡不足,无法加入"), REQUEST_FAILURE_NORMAL);
				return false;
			}
			*/

			//俱乐部的房卡金币场 或者 大厅的房卡金币场
			break;
		}
	}

	return true;
}

//检查创建桌子门票
bool CHandleFromGate::CheckCreateTableTicket(tagTableRule *pCfg, CPlayer *pIServerUserItem)
{
	switch( pCfg->GameMode )
	{
	case TABLE_MODE_FK://房卡模式
		{
			//校验用户房卡: 房主支付，且用户房卡不足
			/*
			if(0 == pCfg->cbPayType && 
				(pIServerUserItem->GetUserRoomCard() < ((pCfg->GameCountType) * (pCfg->PlayerCount)) ))	  
			{
				SendRequestFailure(pIServerUserItem, TEXT("您正在创建房主支付的房卡房, 因房卡不足,无法创建"), REQUEST_FAILURE_NORMAL);
				return false;
			}
			*/
			/*
			//校验用户房卡：AA支付，且用户房卡不足
			if( (1 == pCfg->cbPayType) && 
				(pIServerUserItem->GetUserRoomCard() < pCfg->GameCountType) )
			{
				SendRequestFailure(pIServerUserItem, TEXT("您正在创建AA支付的房卡房, 因房卡不足,无法创建"), REQUEST_FAILURE_NORMAL);
				return false;
			}
			*/

			return true;
		}
	case TABLE_MODE_FK_GOLD://房卡金币场
		{
			//校验用户房卡: 房主支付，且用户房卡不足
			/*
			if(0 == pCfg->cbPayType && 
				(pIServerUserItem->GetUserRoomCard() < ((pCfg->GameCountType) * (pCfg->PlayerCount)) ))	  
			{
				SendRequestFailure(pIServerUserItem, TEXT("您正在创建房主支付的房卡金币房, 因房卡不足,无法创建"), REQUEST_FAILURE_NORMAL);
				return false;
			}

			//校验用户房卡：AA支付，且用户房卡不足
			if( (1 == pCfg->cbPayType) && 
				(pIServerUserItem->GetUserRoomCard() < pCfg->GameCountType) )
			{
				SendRequestFailure(pIServerUserItem, TEXT("您正在创建AA支付的房卡金币房, 因房卡不足,无法创建"), REQUEST_FAILURE_NORMAL);
				return false;
			}
			*/
			//大厅的房卡金币场
			return true;
		}
	default:
		{
			SendRequestFailure(pIServerUserItem, TEXT("创建房间 模式不正确, 无法创建"), REQUEST_FAILURE_NORMAL);
			return false;
		}
	}
}

//创建桌子 -- 金币大厅
void CHandleFromGate::HallTableCreate(DWORD dwUserID, DWORD dwKindID, BYTE byGameType, DWORD dwTableID)
{
    STR_DBR_HALL_GOLD_TABLE_INFO Dbr;
	ZeroMemory(&Dbr, sizeof(Dbr));
	Dbr.dwUserID = dwUserID;
	Dbr.dwKindID = dwKindID;	
	Dbr.byGameType = byGameType;
	Dbr.dwTableID =dwTableID;
	Dbr.dwTableState = 0;
	Dbr.byMask =1;

	Dbr.dwGameID = (g_GameCtrl->GetServerID()) & 0xFFFFFF00;

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_HALL_GOLD_TABLE_INFO, 0, &Dbr, sizeof(Dbr));
}

#pragma endregion


//更新用户财富信息
bool CHandleFromGate::OnEventModifyUserTreasure(CPlayer *pIServerUserItem, DWORD dwTableID, BYTE byTableMode, BYTE byRound, SCORE lUserTreasuse, BYTE byWin)
{
	//效验参数
	if (pIServerUserItem==NULL) return false;

	//构造数据
	STR_DBR_GR_MODIFY_USER_TREASURE DBR;
	ZeroMemory(&DBR, sizeof(DBR));
	DBR.dwUserId  =  pIServerUserItem->GetUserID(); //pUserInfo->dwUserID;
	DBR.dwKind = ((g_GameCtrl->GetServerID()) >> 16) & 0xFF;
	DBR.dwTableID = dwTableID;
	DBR.byTableMode = byTableMode;
	DBR.byRound = byRound;
	DBR.lUserTreasure = lUserTreasuse;
	DBR.byWin = byWin;

	//发送数据
	g_GameCtrl->PostDataBaseRequest(DBR_GR_MODIFY_USER_TREASURE, pIServerUserItem->GetSocketID(), &DBR, sizeof(DBR));

	return true;
}

/***************************************【主消息 7】*******************************************************/
//请求比赛场信息
bool CHandleFromGate::On_SUB_CG_MATCH_INFO(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验
	CPlayer *player = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if (player == NULL)
		return false;

	//获取比赛项
	std::list<MATCH_CONFIG> array = CMatchManager::GetAllMatchConfig();

	WORD wPacketSize=0;
	BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
	MATCH_CONFIG * pcfg = NULL;
	for (auto cfg : array)
	{
		//发送信息
		if ((wPacketSize+sizeof(MATCH_CONFIG))>sizeof(cbBuffer))
		{
			g_GameCtrl->SendData(dwSocketID, MDM_GR_MATCH, CMD_GC_MATCH_INFO, cbBuffer, wPacketSize);
			wPacketSize=0;
		}
		
		//读取信息
		pcfg=(MATCH_CONFIG *)(cbBuffer+wPacketSize);
		CopyMemory(pcfg, &cfg, sizeof(MATCH_CONFIG));

		wPacketSize+=sizeof(MATCH_CONFIG);
	}
	if (wPacketSize>0) g_GameCtrl->SendData(dwSocketID, MDM_GR_MATCH, CMD_GC_MATCH_INFO, cbBuffer, wPacketSize);

	On_SUB_CG_MATCH_QUERY_PLAYER(NULL, 0, dwSocketID);

	return true;
}

//玩家报名
bool CHandleFromGate::On_SUB_CG_MATCH_APPLY(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//大小检验
	if (sizeof(STR_SUB_CG_MATCH_APPLY) != wDataSize)
		return false;

	//玩家校验
	CPlayer *player = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if (player == NULL)
		return false;

	//获取数据
	STR_SUB_CG_MATCH_APPLY *apply = (STR_SUB_CG_MATCH_APPLY *)pData;

	//构造数据
	STR_CMD_GC_MATCH_APPLY cmdApply;
	ZeroMemory(&cmdApply, sizeof(STR_CMD_GC_MATCH_APPLY));

	cmdApply.byResult = 0;

	//获取比赛场
	CMatchItem *match = CMatchManager::Find_Match_ByItemID(apply->wMatchID);
	if (match->IsMatchStart())
	{
		cmdApply.byResult = 1;
		g_GameCtrl->SendData(player, MDM_GR_MATCH, CMD_GC_MATCH_APPLY, &cmdApply, sizeof(STR_CMD_GC_MATCH_APPLY));
		return false;
	}

	//玩家报名
	if (!match->On_User_Apply(player))
	{
		cmdApply.byResult = 1;
		g_GameCtrl->SendData(player, MDM_GR_MATCH, CMD_GC_MATCH_APPLY, &cmdApply, sizeof(STR_CMD_GC_MATCH_APPLY));
		return false;
	}

	//发送数据
	g_GameCtrl->SendData(player, MDM_GR_MATCH, CMD_GC_MATCH_APPLY, &cmdApply, sizeof(STR_CMD_GC_MATCH_APPLY));

	return true;
}

//玩家取消报名
bool CHandleFromGate::On_SUB_CG_MATCH_UNAPPLY(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//大小检验
	if (sizeof(STR_SUB_CG_MATCH_UNAPPLY) != wDataSize)
		return false;

	//玩家校验
	CPlayer *player = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if (player == NULL)
		return false;

	//获取数据
	STR_SUB_CG_MATCH_UNAPPLY *apply = (STR_SUB_CG_MATCH_UNAPPLY *)pData;

	//构造数据
	STR_CMD_GC_MATCH_UNAPPLY cmdApply;
	ZeroMemory(&cmdApply, sizeof(STR_CMD_GC_MATCH_UNAPPLY));

	cmdApply.byResult = 0;

	//获取比赛场
	CMatchItem *match = CMatchManager::Find_Match_ByItemID(apply->wMatchID);
	if (match->IsMatchStart())
	{
		cmdApply.byResult = 1;
		g_GameCtrl->SendData(player, MDM_GR_MATCH, CMD_GC_MATCH_UNAPPLY, &cmdApply, sizeof(STR_CMD_GC_MATCH_UNAPPLY));
		return false;
	}

	//玩家报名
	if (!match->On_User_UnApply(player))
	{
		cmdApply.byResult = 1;
		g_GameCtrl->SendData(player, MDM_GR_MATCH, CMD_GC_MATCH_UNAPPLY, &cmdApply, sizeof(STR_CMD_GC_MATCH_UNAPPLY));
		return false;
	}

	//发送数据
	g_GameCtrl->SendData(player, MDM_GR_MATCH, CMD_GC_MATCH_UNAPPLY, &cmdApply, sizeof(STR_CMD_GC_MATCH_UNAPPLY));

	return true;
}

//玩家请求信息
bool CHandleFromGate::On_SUB_CG_MATCH_QUERY_PLAYER(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//玩家校验
	CPlayer *player = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if (player == NULL)
		return false;

	//获取比赛项
	std::list<CMatchItem *> array = CMatchManager::Find_Match_All();

	//构造数据
	WORD wPacketSize=0;
	BYTE cbBuffer[MAX_ASYNCHRONISM_DATA/10];
	STR_CMD_GC_MATCH_QUERY_PLAYER * pinfo = NULL;
	for (auto match : array)
	{
		CLog::Log(log_debug, "match ID: %d", match->GetMatchID());
		if (match->IsMatchStart())	continue;

		//发送信息
		if ((wPacketSize+sizeof(STR_CMD_GC_MATCH_QUERY_PLAYER))>sizeof(cbBuffer))
		{
			g_GameCtrl->SendData(dwSocketID, MDM_GR_MATCH, CMD_GC_MATCH_QUERY_PLAYER, cbBuffer, wPacketSize);
			wPacketSize=0;
		}
		
		//读取信息
		pinfo=(STR_CMD_GC_MATCH_QUERY_PLAYER *)(cbBuffer+wPacketSize);
		pinfo->wMatchID = match->GetMatchID();
		pinfo->dwApplyCount = match->GetApplyCount();
		pinfo->dwTimer = match->GetConfig().dwStartTime - time(0);

		if (match->GetConfig().wStartType == MATCH_START_TYPE_COUNT)
			pinfo->dwTimer = 0;
		CLog::Log(log_debug, "leave time: %d", pinfo->dwTimer);
		CLog::Log(log_debug, "apply count: %d", pinfo->dwApplyCount);

		wPacketSize+=sizeof(STR_CMD_GC_MATCH_QUERY_PLAYER);
	}
	if (wPacketSize>0) g_GameCtrl->SendData(dwSocketID, MDM_GR_MATCH, CMD_GC_MATCH_QUERY_PLAYER, cbBuffer, wPacketSize);

	return true;
}

//更新所有人排名
bool CHandleFromGate::On_SUB_CG_MATCH_RANKING(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//玩家校验
	CPlayer *player = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if (player == NULL)
		return false;

	//比赛场房间校验
	CMatchRoom *room = (CMatchRoom *)CTableManager::FindTableByTableID(player->GetTableID());
	if (room == NULL)
		return false;

	//比赛校验
	CMatchItem *match = room->GetMatchItem();
	if (match == NULL)
		return false;

	//发送排名
	match->Send_Ranking(player);

	return true;
}

//更新自己排名
bool CHandleFromGate::On_SUB_CG_MATCH_RANKING_MY(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//玩家校验
	CPlayer *player = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if (player == NULL)
		return false;

	//比赛场房间校验
	CMatchRoom *room = (CMatchRoom *)CTableManager::FindTableByTableID(player->GetTableID());
	if (room == NULL)
		return false;

	//比赛校验
	CMatchItem *match = room->GetMatchItem();
	if (match == NULL)
		return false;

	//发送排名
	match->Send_Self_Ranking(player);

	return true;
}