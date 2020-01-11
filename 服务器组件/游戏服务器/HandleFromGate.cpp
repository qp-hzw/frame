#include "HandleFromGate.h"
#include "GameCtrl.h"
#include "TableManager.h"
#include <iostream>

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
	}
}

bool CHandleFromGate::HandlePacketDB(WORD wRequestID, DWORD dwSocketID, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
	case DBO_CG_LOGON_USERID:			//ID登录返回
		{
			return On_CMD_GC_Logon_UserID(dwSocketID, pData, wDataSize);
		}
	case DBO_CG_USER_CREATE_GROUP_ROOM:	//创建牌友圈房间返回
		{
			return On_CMD_GC_User_CreateGroupRoom(dwSocketID, pData, wDataSize);
		}
	case DBO_CG_USER_JOIN_GROUP_ROOM:	//加入牌友圈房间返回
		{
			return On_CMD_GC_User_JoinGroupRoom(dwSocketID, pData, wDataSize);
		}
	case DBO_SG_MODIFY_USER_TREASURE:	//修改用户财富信息返回
		{
			return On_CMD_GC_User_ModifyUserTreasure(dwSocketID, pData, wDataSize);
		}

	case DBO_GR_LOAD_OFFLINE:		//加载断线重连返回
		{
			return On_DBO_GR_LOAD_OFFLINE(dwSocketID,pData,wDataSize);
		}

	case DBO_GR_LOTTERY_RESULT:
		{
			return OnDBLotteryResult(dwSocketID,pData,wDataSize);
		}
	case DBO_GC_CLUB_CREATE_TABLE:	//club牌友圈创建桌子
		{
			return On_CMD_CG_CLUB_CREATE_TABLE(dwSocketID,pData,wDataSize);
		}
	case DBO_GC_USER_JOIN_TABLE_NO_PASS://加入桌子 不需要密码
		{
			return On_CMD_GC_USER_JOIN_TABLE_NO_PASS(dwSocketID,pData,wDataSize);
		}
	case DBO_GC_CLUB_UPDATE_TABLE_INFO:// 更新桌子信息
		{
			if(sizeof(STR_CMD_LC_CLUB_TABLE_LIST) != wDataSize) return false;
			g_TCPSocketEngine->SendData(MDM_TRANSFER, CPR_GP_CLUB_TABLE_INFO, pData, wDataSize);
			return true;;
		}
	case DBO_GC_CLUB_UPDATE_PLAYER_INFO://更新玩家信息
		{
			if(sizeof(STR_CMD_LC_CLUB_TABLE_USER_LIST) != wDataSize) return false;
			g_TCPSocketEngine->SendData(MDM_TRANSFER, CPR_GP_CLUB_PLAYER_INFO, pData, wDataSize);
			return true;
		}
	case DBO_GC_JOIN_TABLE://加入桌子 数据库校验
		{
			return On_CMD_GC_JOIN_TABLE(dwSocketID,pData,wDataSize);
		}
	case DBO_GC_USER_JOIN_TABLE_HALL_GOLD: //加入桌子 -- 金币大厅桌子
		{
			return On_CMD_GC_USER_JOIN_TABLE_HALL_GOLD(dwSocketID,pData,wDataSize);
		}
	}
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
	case SUB_CG_USER_REFUSE_SIT:	//拒绝玩家坐下
		{
			return On_SUB_CG_User_RefuseSit(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_KICK_USER:		//踢出用户
		{
			return On_SUB_CG_User_KickUser(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_USER_CHAIR_REQ:    //请求更换位置
		{
			return OnTCPNetworkSubUserChairReq(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_CREATE_ROOM:	//创建房间
		{
			return On_SUB_CG_USER_CREATE_ROOM(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_CLUB_CREATE_TABLE:	//创建桌子	牌友圈的桌子
		{
			return On_SUB_CG_USER_CREATE_TABLE(pData,wDataSize,dwSocketID);
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
	case SUB_CG_USER_RECONNECT_ROOM:	//断线重连
		{
			return On_SUB_User_ReconnectRoom(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_CREATE_GROUP_ROOM:	//创建牌友圈房间
		{
			return On_Sub_CG_User_CreateGroupRoom(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_JOIN_GROUP_ROOM:	//加入牌友圈房间
		{
			return On_Sub_CG_User_JoinGroupRoom(pData,wDataSize,dwSocketID);
		}
	case SUB_CG_USER_JOIN_GOLD_ROOM:	//加入金币房
		{
			return On_SUB_User_JoinGoldRoom(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_GET_TABLELIST:			//获得开房列表
		{
			return OnTCPNetworkSubGetTableList(pData,wDataSize,dwSocketID);
		}
	case SUB_GR_QUERY_LOTTERY:			//请求抽奖
		{
			return OnTcpNetworkQueryLottery(pData, wDataSize, dwSocketID);
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
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//处理过虑
	DWORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	if ((wTableID==INVALID_TABLE)||(wChairID==INVALID_CHAIR)) return true;

	//消息处理 
	CTableFrame * pTableFrame=CTableManager::FindTableByTableID(wTableID);
	if (pTableFrame == NULL)
	{
		CLog::Log(log_error, "pTableFrame == NULL");
		return false;
	}

	return pTableFrame->OnEventSocketFrame(wSubCmdID,pData,wDataSize,pIServerUserItem);
}


#pragma region 登录模块 MDM_LOGON
//I D 登录
bool CHandleFromGate::On_SUB_CG_Logon_UserID(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	if (wDataSize != sizeof(STR_SUB_CG_LOGON_USERID)) return false;
	STR_SUB_CG_LOGON_USERID *pLogonUserID = (STR_SUB_CG_LOGON_USERID *)pData;

	//在用户列表获取用户
	CPlayer *pIServerUserItem = CPlayerManager::FindPlayerByID(pLogonUserID->dwUserID);

	//非正常登录的用户不允许踢出
	if (pIServerUserItem!=NULL)
	{
		//非正常登录的两种情况
		//1. 该用户如果是机器用户, 但是从外地登录
		//2. 该机器非机器用户, 但是在该进程上登录
	}

	
	//切换判断 TODONOW 重点查看
	if( NULL != pIServerUserItem )
	{
		//return SwitchUserItemConnect(pIServerUserItem, pLogonUserID->szMachineID,
			//pLogonUserID->dLongitude, pLogonUserID->dLatitude);

	}
	

	//变量定义
	STR_DBR_CG_LOGON_USERID LogonUserID;
	ZeroMemory(&LogonUserID,sizeof(LogonUserID));

	//构造数据
	LogonUserID.dwUserID = pLogonUserID->dwUserID;
	//LogonUserID.dwClientAddr = pBindParameter->dwClientAddr;
	LogonUserID.dLatitude = pLogonUserID->dLatitude;
	LogonUserID.dLongitude = pLogonUserID->dLongitude;
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

	//判断在线

	//用户登录失败，直接返回
	if ( DB_SUCCESS != pDBOLogon->lResultCode )
	{
		//构造返回数据
		STR_CMD_GC_LOGON_USERID logonFail;
		ZeroMemory(&logonFail, sizeof(STR_CMD_GC_LOGON_USERID));
		logonFail.lResultCode = 0;
		lstrcpyn(logonFail.szDescribeString, pDBOLogon->szDescribeString, CountArray(logonFail.szDescribeString));

		//发送数据
		g_GameCtrl->SendData(dwSocketID, MDM_GR_LOGON, CMD_GC_LOGON_USERID, &logonFail, sizeof(STR_CMD_GC_LOGON_USERID));
		return true;
	}

	//在用户列表中获取 用户
	CPlayer *pIServerUserItem = CPlayerManager::FindPlayerByID(pDBOLogon->dwUserID);

	
	////重复登录判断 TODONOW
	//if (pIServerUserItem!=NULL)
	//{
	//	//切换用户
	//	SwitchUserItemConnect(pIServerUserItem, pDBOLogon->szMachineID, pDBOLogon->dLongitude, pDBOLogon->dLatitude, 
	//							pDBOLogon->cbDeviceType, pDBOLogon->wBehaviorFlags, pDBOLogon->wPageTableCount);

	//	//提示消息
	//	TCHAR szDescribe[128]=TEXT("");
	//	_sntprintf_s(szDescribe,CountArray(szDescribe),
	//		TEXT("【ID登录】【%ld重复登录2】"),
	//		pIServerUserItem->GetUserID());

	//	return true;
	//}
	

	//激活用户
	ActiveUserItem(&pIServerUserItem, dwSocketID, pDBOLogon);

	return true;
}

//ID登录成功，激活用户
void CHandleFromGate::ActiveUserItem(CPlayer **pIServerUserItem, DWORD dwSocketID, 
		STR_DBO_CG_LOGON_USERID *pDBOLogon)
{
	if ( NULL == pDBOLogon )
	{
		return;
	}

	//用户变量
	tagUserInfo UserInfo;
	ZeroMemory(&UserInfo,sizeof(UserInfo));

	//属性资料
	UserInfo.dwUserID = pDBOLogon->dwUserID;
	UserInfo.dwGroupID = pDBOLogon->dwGroupID;
	UserInfo.dwCustomID = pDBOLogon->dwCustomID;
	lstrcpyn(UserInfo.szNickName,pDBOLogon->szNickName,CountArray(UserInfo.szNickName));
	lstrcpyn(UserInfo.szHeadUrl,pDBOLogon->szHeadUrl,CountArray(UserInfo.szHeadUrl));


	//用户资料
	UserInfo.cbGender=pDBOLogon->cbGender;
	UserInfo.cbMemberOrder=pDBOLogon->cbMemberOrder;
	UserInfo.cbMasterOrder=pDBOLogon->cbMasterOrder;
	lstrcpyn(UserInfo.szGroupName,pDBOLogon->szGroupName,CountArray(UserInfo.szGroupName));
	lstrcpyn(UserInfo.szUnderWrite,pDBOLogon->szUnderWrite,CountArray(UserInfo.szUnderWrite));

	//状态设置
	UserInfo.cbUserStatus=US_FREE;
	UserInfo.wTableID=INVALID_TABLE;
	UserInfo.wChairID=INVALID_CHAIR;

	//积分信息
	UserInfo.lScore=pDBOLogon->lScore;
	UserInfo.lGrade=pDBOLogon->lGrade;
	UserInfo.lOpenRoomCard=pDBOLogon->lOpenRoomCard;
	UserInfo.lDiamond=pDBOLogon->lDiamond;
	UserInfo.lGold=pDBOLogon->lGold;
	UserInfo.lControlScore=pDBOLogon->lControlScore;
	UserInfo.dwWinCount=pDBOLogon->dwWinCount;
	UserInfo.dwLostCount=pDBOLogon->dwLostCount;
	UserInfo.dwDrawCount=pDBOLogon->dwDrawCount;
	UserInfo.dwFleeCount=pDBOLogon->dwFleeCount;
	UserInfo.dwUserMedal=pDBOLogon->dwUserMedal;
	UserInfo.dwExperience=pDBOLogon->dwExperience;
	UserInfo.lLoveLiness=pDBOLogon->lLoveLiness;

	//位置信息
	UserInfo.dLatitude = pDBOLogon->dLatitude;
	UserInfo.dLongitude = pDBOLogon->dLongitude;

	//登录信息
	UserInfo.dwLogonTime=(DWORD)time(NULL);
	UserInfo.dwInoutIndex=pDBOLogon->dwInoutIndex;

	//用户权限
	UserInfo.dwUserRight=pDBOLogon->dwUserRight;
	UserInfo.dwMasterRight=pDBOLogon->dwMasterRight;

	//辅助变量
	bool bAndroidUser = false;						//用户类型，真人/机器人
	UserInfo.bAndroidUser = bAndroidUser;
	UserInfo.lRestrictScore=0L;//屏蔽每局封顶
	lstrcpyn(UserInfo.szPassword,pDBOLogon->szPassword,CountArray(UserInfo.szPassword));

	UserInfo.dwSocketId = dwSocketID;

	//连接信息
	//UserInfo.wBindIndex=wBindIndex;
	//UserInfo.dwClientAddr=pBindParameter->dwClientAddr;
	lstrcpyn(UserInfo.szMachineID,pDBOLogon->szMachineID,CountArray(UserInfo.szMachineID));

	//激活用户 -- 设置用户信息
	CPlayerManager::InsertPlayer(pIServerUserItem, UserInfo);

	//错误判断 -- 设置用户pIServerUserItem信息失败
	if (pIServerUserItem == NULL)
	{
		//断开用户
		if (bAndroidUser==true)
		{
			//m_AndroidUserManager.DeleteAndroidUserItem(dwSocketID);
		}
		else
		{
			g_TCPNetworkEngine->CloseSocket(dwSocketID);
		}
		return;
	}

	//登录事件
	OnEventUserLogon(*pIServerUserItem, false);
}

//用户登录 
VOID CHandleFromGate::OnEventUserLogon(CPlayer * pIServerUserItem, bool bAlreadyOnLine)
{
	//获取参数
	WORD wBindIndex = pIServerUserItem->GetBindIndex();
	bool bAndroidUser = pIServerUserItem->IsAndroidUser();

	/* 1. 构造返回数据 */
	STR_CMD_GC_LOGON_USERID logon;
	ZeroMemory(&logon, sizeof(STR_CMD_GC_LOGON_USERID));
	logon.lResultCode = 0;
	logon.dwKindID = g_GameCtrl->GetKindID();
	lstrcpyn(logon.szDescribeString, TEXT("用户登录成功"), CountArray(logon.szDescribeString));
	
	//TODONOW
	logon.dwOffLineGameID = pIServerUserItem -> GetOfflineGameID();

	//发送数据
	g_GameCtrl->SendData(pIServerUserItem->GetSocketID(), MDM_GR_LOGON, CMD_GC_LOGON_USERID, &logon, sizeof(STR_CMD_GC_LOGON_USERID));
	

	/* 11. 通知数据库改变 玩家状态 */
	//TODONOWW
	DBR_GP_UserQuitInfo quitInfo;
	quitInfo.dwUserID = pIServerUserItem->GetUserID();
	quitInfo.byOnlineMask = 2;
	g_GameCtrl->PostDataBaseRequest(DBR_GP_GAME_USER_STATE,0, &quitInfo,sizeof(quitInfo));
}

//切换连接
bool CHandleFromGate::SwitchUserItemConnect(CPlayer * pIServerUserItem, TCHAR szMachineID[LEN_MACHINE_ID],
												const double &dLongitude, const double &dLatitude,
												BYTE cbDeviceType, WORD wBehaviorFlags, WORD wPageTableCount)
{
	//效验参数
	if (NULL == pIServerUserItem) 
		return false;

	//用户有效，断开已绑定的用户
	if (INVALID_WORD != pIServerUserItem->GetBindIndex())	
	{
		//断开用户
		if (pIServerUserItem->IsAndroidUser()==true)
		{
			//m_AndroidUserManager.DeleteAndroidUserItem(pSourceParameter->dwSocketID);
		}
		else
		{
			g_TCPNetworkEngine->CloseSocket(pIServerUserItem->GetSocketID());
		}
	}

	//机器判断
	LPCTSTR pszMachineID = pIServerUserItem->GetMachineID();
	bool bSameMachineID = (lstrcmp(pszMachineID, szMachineID)==0);

	//重新激活用户
	pIServerUserItem->SetUserParameter(0, szMachineID, false, false, dLongitude, dLatitude);

	//登录事件
	OnEventUserLogon(pIServerUserItem,true);

	return true;
}

#pragma endregion


//加载断线重连返回
bool CHandleFromGate::On_DBO_GR_LOAD_OFFLINE(DWORD dwSocketID, VOID * pData, WORD wDataSize)
{
	//参数校验
	if(sizeof(BYTE ) != wDataSize) return false;

	//m_GameProgress =  *((BYTE*)pData);

	return true;
}

/*********************************************【主消息 3 用户命令】*********************************************************/
//用户坐下
bool CHandleFromGate::On_SUB_CG_User_SitDown(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	if (wDataSize!=sizeof(CMD_GR_UserSitDown)) return false;

	//效验数据
	CMD_GR_UserSitDown * pUserSitDown=(CMD_GR_UserSitDown *)pData;

	//获取用户
	CPlayer * pIServerUserItem=CPlayerManager::FindPlayerBySocketID(dwSocketID);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//消息处理
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

	//重复判断
	//if (pUserSitDown->wTableID < MAX_TABLE)
	{
		CTableFrame * pTableFrame=CTableManager::FindTableByTableID(pUserSitDown->wTableID);
		//if (pTableFrame->GetTableUserItem(pUserSitDown->wChairID)==pIServerUserItem) return true;
	}

	//用户判断
	if (cbUserStatus==US_PLAYING)
	{
		SendRequestFailure(pIServerUserItem,TEXT("您正在游戏中，暂时不能离开，请先结束当前游戏！"),0);
		return true;
	}


	//离开处理
	if (wTableID!=INVALID_TABLE)
	{
		CTableFrame * pTableFrame=CTableManager::FindTableByTableID(wTableID);
		if (pTableFrame->PlayerUpTable(pIServerUserItem)==false) return true;
	}

	//请求调整
	WORD wRequestTableID=pUserSitDown->wTableID;
	WORD wRequestChairID=pUserSitDown->wChairID;

	//桌子调整
	if (wRequestTableID>=CTableManager::TableCount())
	{
		//起始桌子
		WORD wStartTableID=0;

		//动态加入
		bool bDynamicJoin=true;

		//寻找位置
		for (WORD i=wStartTableID;i<CTableManager::TableCount();i++)
		{
			CTableFrame* pTable = CTableManager::FindTableByIndex(i);

			//游戏状态
			if(NULL == pTable )
				continue;

			//获取空位
			WORD wNullChairID=pTable->GetNullChairID();

			//调整结果
			if (wNullChairID!=INVALID_CHAIR)
			{
				//设置变量
				wRequestTableID=i;
				wRequestChairID=wNullChairID;

				break;
			}
		}

		//结果判断
		if ((wRequestTableID==INVALID_CHAIR)||(wRequestChairID==INVALID_CHAIR))
		{
			SendRequestFailure(pIServerUserItem,TEXT("当前游戏房间已经人满为患了，暂时没有可以让您加入的位置，请稍后再试！"),0);
			return true;
		}
	}

	//椅子调整
	{
		//效验参数
		if (wRequestTableID>=CTableManager::TableCount()) return false;

		CTableFrame* pTable = CTableManager::FindTableByTableID(wRequestTableID);
		if(NULL == pTable) return false;
		//查找空位
		wRequestChairID=pTable->GetNullChairID();

		//结果判断
		if (wRequestChairID==INVALID_CHAIR)
		{
			SendRequestFailure(pIServerUserItem,TEXT("由于此游戏桌暂时没有可以让您加入的位置了，请选择另外的游戏桌！"),0);
			return true;
		}
	}

	//坐下处理
	CTableFrame * pTableFrame=CTableManager::FindTableByTableID(wRequestTableID);
	if(pTableFrame != NULL) 
		pTableFrame->PlayerSitTable(wRequestChairID,pIServerUserItem,pUserSitDown->szPassword, NULL);

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
	WORD wTableID = pIServerUserItem->GetTableID();
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
		if(pTableCfg != NULL && (pTableCfg->GameMode == TABLE_MODE_DW || pTableCfg->GameMode == TABLE_MODE_GOLD))
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
	pTableFrame->PlayerReady(pIServerUserItem->GetChairID(), pIServerUserItem);

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
	WORD wBindIndex = LOWORD(dwSocketID);
	CPlayer * pIServerUserItem=GetBindUserItem(wBindIndex);
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

//拒绝玩家坐下
bool CHandleFromGate::On_SUB_CG_User_RefuseSit(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_GR_UserRepulseSit));
	if (wDataSize!=sizeof(CMD_GR_UserRepulseSit)) return false;

	//消息处理
	CMD_GR_UserRepulseSit * pUserRepulseSit=(CMD_GR_UserRepulseSit *)pData;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	CPlayer * pIServerUserItem=GetBindUserItem(wBindIndex);
	if (pIServerUserItem==NULL) return false;

	//获取桌子
	CTableFrame * pTableFrame=CTableManager::FindTableByTableID(pUserRepulseSit->wTableID);

	//获取用户
	CPlayer * pRepulseIServerUserItem = pTableFrame->GetTableUserItem(pUserRepulseSit->wChairID);
	if (pRepulseIServerUserItem==NULL) return true;
	if(pRepulseIServerUserItem->GetUserID() != pUserRepulseSit->dwRepulseUserID)return true;

	//发送消息
	TCHAR szDescribe[256]=TEXT("");
	lstrcpyn(szDescribe,TEXT("此桌有玩家设置了不与您同桌游戏！"),CountArray(szDescribe));
	g_GameCtrl->SendRoomMessage(pRepulseIServerUserItem,szDescribe,0);

	//弹起玩家
	pTableFrame->PlayerUpTable(pRepulseIServerUserItem);

	return true;
}

//踢出用户
bool CHandleFromGate::On_SUB_CG_User_KickUser(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_KickUser));
	if (wDataSize!=sizeof(CMD_GR_KickUser)) return false;

	//变量定义
	CMD_GR_KickUser * pKickUser=(CMD_GR_KickUser *)pData;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	CPlayer * pIServerUserItem=GetBindUserItem(wBindIndex);

	//目标用户
	CPlayer * pITargetUserItem = CPlayerManager::FindPlayerByID(pKickUser->dwTargetUserID);
	if(pITargetUserItem==NULL) return true;

	//用户效验
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->GetMemberOrder()>pITargetUserItem->GetMemberOrder()));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->GetMemberOrder()<=pITargetUserItem->GetMemberOrder())) return false;

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

	//防踢判断
	if((pITargetUserItem->GetUserProperty()->wPropertyUseMark&PT_USE_MARK_GUARDKICK_CARD)!=0)
	{
		//变量定义
		DWORD dwCurrentTime=(DWORD)time(NULL);
		tagUserProperty * pUserProperty = pITargetUserItem->GetUserProperty();

		//时效判断
		DWORD dwValidTime=pUserProperty->PropertyInfo[2].wPropertyCount*pUserProperty->PropertyInfo[2].dwValidNum;
		if(pUserProperty->PropertyInfo[2].dwEffectTime+dwValidTime>dwCurrentTime)
		{
			//变量定义
			TCHAR szMessage[256]=TEXT("");
			_sntprintf_s(szMessage,CountArray(szMessage),TEXT("由于玩家 [ %s ] 正在使用防踢卡,您无法将它踢出游戏！"),pITargetUserItem->GetNickName());

			//发送消息
			g_GameCtrl->SendRoomMessage(pIServerUserItem,szMessage,0);

			return true; 
		}
		else
			pUserProperty->wPropertyUseMark &= ~PT_USE_MARK_GUARDKICK_CARD;
	}

	//请离桌子
	WORD wTargerTableID = pITargetUserItem->GetTableID();
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

//请求房间列表
bool CHandleFromGate::OnTCPNetworkSubGetTableList(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize>=sizeof(CMD_GR_GetTableList));
	if (wDataSize<sizeof(CMD_GR_GetTableList)) return false;

	//处理消息
	CMD_GR_GetTableList * pGetTableList = (CMD_GR_GetTableList *)pData;

	CMD_GR_TableListResult pTableList;
	ZeroMemory(&pTableList,sizeof(CMD_GR_TableListResult));


	int iCount = 20;
	for(int i=0;i < CTableManager::TableCount(),i < iCount;i++)
	{
		CTableFrame* pTableFrame = CTableManager::FindTableByIndex(i);
		if(NULL == pTableFrame) continue;

		if( pTableFrame->GetTableMode() == pGetTableList->GameMode)
		{
			tagTableRule* pCfg = (tagTableRule*)pTableFrame->GetCustomRule();

			//不允许陌生人加入则不发送
			if(pCfg->bAllowStranger == 0)
				continue;

			if((pGetTableList->GoldRank == 0  && pCfg->CellScore <= 100)
				||(pGetTableList->GoldRank == 1  && pCfg->CellScore <= 500 && pCfg->CellScore > 100 )
				||(pGetTableList->GoldRank == 2  && pCfg->CellScore <= 5000 && pCfg->CellScore > 500 ))
			{
				pTableList.byCount++;
				pTableList.tableInfoList[i].byCurrPlayerCount =  static_cast<BYTE>(pTableFrame->GetSitUserCount());
				pTableList.tableInfoList[i].byMaxPlayerCount = pCfg->PlayerCount;
				pTableList.tableInfoList[i].dwJoinScore = pCfg->JoinScore;
				pTableList.tableInfoList[i].dwLeaveScore = pCfg->LeaveScore;

				//pTableList.tableInfoList[i].dwTablePassword = pCfg->KaiHuaGang;
				//pTableList.tableInfoList[i].QingYiSe = pCfg->QingYiSe;
				//pTableList.tableInfoList[i].QiXiaoDui = pCfg->QiXiaoDui;
				pTableList.tableInfoList[i].wCellScore = pCfg->CellScore;
				//pTableList.tableInfoList[i].XuanFengGang = pCfg->XuanFengGang;
				//pTableList.tableInfoList[i].ZhangMao = pCfg->ZhangMao;
			}
		}
	}

	if(pTableList.byCount > 0)
	{
		pTableList.GoldRank = pGetTableList->GoldRank;
		//发送房间列表
		g_GameCtrl->SendData(dwSocketID,MDM_USER,SUB_GR_GET_TABLELIST_RESULT,&pTableList,sizeof(CMD_GR_TableListResult));	
	}

	return true;
}

//抽奖请求
bool CHandleFromGate::OnTcpNetworkQueryLottery(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//参数校验
	ASSERT(wDataSize==sizeof(CMD_GR_QueryLottery));
	if(wDataSize!=sizeof(CMD_GR_QueryLottery))
		return false;

	CMD_GR_QueryLottery* pGetRankReward = (CMD_GR_QueryLottery*)pData;

	DBR_GR_QueryLottery QueryLottery;
	memcpy(&QueryLottery,pGetRankReward,sizeof(DBR_GR_QueryLottery));

	return g_GameCtrl->PostDataBaseRequest(DBR_GR_QUERY_LOTTERY,dwSocketID,&QueryLottery,sizeof(DBR_GR_QueryLottery));

}

/***************************************【主消息 3】*******************************************************/
//创建房间
bool CHandleFromGate::On_SUB_CG_USER_CREATE_ROOM(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//获取用户
	CPlayer *pIServerUserItem = CPlayerManager::FindPlayerBySocketID(dwSocketID);

	//用户校验
	if (pIServerUserItem==NULL) 
	{
		SendRequestFailure(pIServerUserItem,TEXT("创建房间数据大小不匹配！"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//校验数据包
	if(wDataSize != sizeof(STR_SUB_CG_USER_CREATE_ROOM))
	{
		SendRequestFailure(pIServerUserItem,TEXT("创建房间数据大小不匹配！"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//房间规则
	STR_SUB_CG_USER_CREATE_ROOM *pCreateRoom = (STR_SUB_CG_USER_CREATE_ROOM *)pData;

	//转为桌子规则
	tagTableRule *pCfg = (tagTableRule*)pCreateRoom->CommonRule;
	if ( NULL == pCfg )
	{
		return false;
	}
	
	//桌子类型
	bool bRet = true;
	if(0 == pCfg->byClubCreate)//创建普通桌子
	{
		bRet = CreateTableNormal(pCfg, pIServerUserItem, pCreateRoom);
	}
	else if(1 == pCfg->byClubCreate || 2 == pCfg->byClubCreate)//创建牌友圈房间
	{
		bRet = CreateRoomClub(pCfg, pIServerUserItem, pCreateRoom);
	}

	return bRet;
}

//创建桌子
bool CHandleFromGate::On_SUB_CG_USER_CREATE_TABLE(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//获取用户
	WORD wBindIndex = LOWORD(dwSocketID);
	CPlayer *pIServerUserItem = GetBindUserItem(wBindIndex);

	//用户校验
	if (pIServerUserItem==NULL) return false;

	//校验数据包
	if(wDataSize != sizeof(STR_SUB_CG_USER_CREATE_TABLE))
	{
		SendRequestFailure(pIServerUserItem,TEXT("创建桌子数据大小不匹配！"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//结构体勾走
	STR_SUB_CG_USER_CREATE_TABLE *pCreateRoom = (STR_SUB_CG_USER_CREATE_TABLE *)pData;

	//投递请求
	return g_GameCtrl->PostDataBaseRequest(DBR_CG_CLUB_CREATE_TABLE, dwSocketID, pData, wDataSize);
}

//创建普通桌子
bool CHandleFromGate::CreateTableNormal(tagTableRule * pCfg, CPlayer *pIServerUserItem, STR_SUB_CG_USER_CREATE_ROOM* pCreateRoom)
{
	//检查加入门票
	//if(!CheckCreateTableTicket(pCfg, pIServerUserItem))
	{
		CLog::Log(log_debug, "门票不够: %d", pCfg->GameMode);
		//return true; //TODONOW 如果为false 客户端就断线重连了， 之后修改掉
	} 

	//用户效验
	if (INVALID_CHAIR != pIServerUserItem->GetChairID())   //有问题
	{
		SendRequestFailure(pIServerUserItem, TEXT("正在游戏中,无法创建房间！"), REQUEST_FAILURE_NORMAL);
		return true;//TODONOW 如果为false 客户端就断线重连了， 之后修改掉
	}
	if(INVALID_TABLE != pIServerUserItem->GetTableID())
	{
		SendRequestFailure(pIServerUserItem, TEXT("正在房间中,无法创建房间！"), REQUEST_FAILURE_NORMAL);
		return true;//TODONOW 如果为false 客户端就断线重连了， 之后修改掉
	}

	//寻找空闲房间
	CTableFrame *pCurrTableFrame = GetNextEmptyTable();  

	//桌子判断
	if(NULL == pCurrTableFrame)
	{
		SendRequestFailure(pIServerUserItem,TEXT("服务器桌子已满,请稍后重试！"),REQUEST_FAILURE_NORMAL);
		return true ; //TODONOW 如果为false 客户端就断线重连了， 之后修改掉
	}

	//设置房间状态
	pCurrTableFrame->SetGameStatus(GAME_STATUS_FREE);

	//设置通用房间规则  
	pCurrTableFrame->SetCommonRule(pCfg);

	//设置子游戏房间规则
	tagTableSubGameRule *pSubGameCfg = (tagTableSubGameRule*)pCreateRoom->SubGameRule;
	if(pSubGameCfg != NULL)
	{
		pCurrTableFrame->SetSubGameRule(pSubGameCfg);
	}

	//替他人开房
	if(1 == pCfg->bCreateToOther)
	{	
		//TODO 替他人开房写数据库流程在这里处理
		HandleCreateTableForOthers(pCurrTableFrame, pIServerUserItem, pCfg);
	}
	else	//正常开房
	{
		//开房失败, 则return false
		if( !HandleCreateTable(pCurrTableFrame, pIServerUserItem, pCfg) )
		{
			return false;
		}
	}

	//发送创建房间成功消息
	g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_CREATE_ROOM_SUCCESS, NULL, 0);

	return true;
}

//创建牌友圈房间
bool CHandleFromGate::CreateRoomClub(tagTableRule * pCfg, CPlayer *pIServerUserItem, STR_SUB_CG_USER_CREATE_ROOM* pCreateRoom)
{
	/* 第二步  校验用户 1.未在其他桌子中  2.未在椅子上 */
	/* added by WangChengQing 2018/5/11  
	** 牌友圈创建房间的时候, 因为创建者不需要进入桌子中, 所以不需要处理该判断
	//用户效验
	if (INVALID_CHAIR != pIServerUserItem->GetChairID()) 
	{
		SendRequestFailure(pIServerUserItem, TEXT("正在游戏中,无法创建房间！"), REQUEST_FAILURE_NORMAL);
		return false;
	}
	if(INVALID_TABLE != pIServerUserItem->GetTableID())
	{
		SendRequestFailure(pIServerUserItem, TEXT("正在房间中,无法创建房间！"), REQUEST_FAILURE_NORMAL);
		return false;
	}
	*/

	/* 第三步  寻找空闲桌子 */
	CTableFrame *pCurrTableFrame = GetNextEmptyTable();
	if(NULL == pCurrTableFrame)
	{
		SendRequestFailure(pIServerUserItem,TEXT("服务器桌子已满,请稍后重试"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 第四步 处理创建桌子流程 */
	//生成桌子密码，房间号 = 前面 + TABLEID
	srand(static_cast<unsigned int >(time(NULL)));
	DWORD dwPassword = GenerateTablePassword();

	//设置桌子属性
	//pCurTableFrame->SetTableOwner(pIServerUserItem->GetUserID());
	pCurrTableFrame->SetTableID(dwPassword);

	//设置房间自动解散，默认一分钟 -- 这里是指不开始游戏 自动一分钟后解散
	pCurrTableFrame->SetTableAutoDismiss();
	
	//获得空椅子
	WORD wChairID = pCurrTableFrame->GetNullChairID(); 

	/* 第五步 设置房间规则 */
	//设置通用房间规则  
	pCurrTableFrame->SetCommonRule(pCfg);

	//设置子游戏房间规则
	tagTableSubGameRule *pSubGameCfg = (tagTableSubGameRule*)pCreateRoom->SubGameRule;
	if(pSubGameCfg != NULL)
	{
		pCurrTableFrame->SetSubGameRule(pSubGameCfg);
	}

	/* 第六步 房间信息保存到数据库 */
	STR_DBR_CLUB_ROOM_INFO Dbr;
	Dbr.dwUserID = pCfg->dwUserID;
	Dbr.dwClubID = pCfg->dwClubID;
	Dbr.dwGameID = (pCfg->dwKindID) << 16;
	Dbr.byModeID = 0;

	if(1 == pCfg->byGoldOrFK)//俱乐部的房卡场 就是普通房卡场
	{
		Dbr.byModeID  = 0;
	}
	else if( 2 == pCfg->byGoldOrFK)//俱乐部的金币场 其实是 房卡金币场
	{
		Dbr.byModeID  = 3;
	}

	//Dbr.szKindName ; 数据库自己查询
	Dbr.dwServiceGold = pCfg->dwAmount;
	Dbr.dwRevenue = pCfg->dwOwnerPercentage;
	Dbr.byMask = pCfg->byMask;
	Dbr.dwDizhu = pCfg->dwDizhu;
	Dbr.dwGold = pCfg->dwLevelGold;
	Dbr.byAllRound = pCfg->GameCount;
	Dbr.byChairNum = pCfg->PlayerCount;
	Dbr.DissolveMinute = pCfg->bDissolve;
	memcpy(Dbr.szRealRoomRule, pCfg, 256);

	WriteClubRoomToDB(&Dbr);

	//发送创建房间成功消息
	g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_CREATE_ROOM_SUCCESS, NULL, 0);	

	//TODONOWW 需要发送给协调服, 然后协调服 发送给登录服.  登录服通知客户端实时刷新俱乐部房间
	STR_CMD_LC_CLUB_ROOM_RE RECMD;
	RECMD.dwClubID = pCfg->dwClubID;
	g_GameCtrl->SendData(pIServerUserItem, MDM_CLUB, CMD_LC_CLUB_ROOM_RE, &RECMD, sizeof(STR_CMD_LC_CLUB_ROOM_RE));	
	
	//added by WangChengQing 2018/7/2   牌友圈创建房间完成之后, 应该断开与client的连接
	//added by WangChengQing 2018/8/31  此处由客户端主动断开socket连接
	return true; 
}

//创建牌友圈桌子
bool CHandleFromGate::CreateTableClub(STR_DBO_GC_CLUB_CREATE_TABLE * pDbo, CPlayer *pIServerUserItem)
{
	//用户效验
	if (INVALID_CHAIR != pIServerUserItem->GetChairID()) 
	{
		SendRequestFailure(pIServerUserItem, TEXT("椅子号不为空,无法创建牌友圈桌子"), REQUEST_FAILURE_NORMAL);
		return false;
	}
	if(INVALID_TABLE != pIServerUserItem->GetTableID())
	{
		SendRequestFailure(pIServerUserItem, TEXT("桌子号不为空,无法创建牌友圈桌子"), REQUEST_FAILURE_NORMAL);
		return false;
	}
	
	/* 第0步  判断公会房卡是否足够 */
	if(pDbo->lResultCode != 0)
	{
		SendRequestFailure(pIServerUserItem,TEXT("工会房卡不足, 请联系管理员！"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	STR_SUB_CG_USER_CREATE_ROOM CreateRoom = pDbo->strCreateRoom;
	tagTableRule *pCfg = (tagTableRule*)CreateRoom.CommonRule;

	/* 第一步 寻找空闲房间 */
	CTableFrame *pCurrTableFrame = GetNextEmptyTable();

	//桌子判断
	if(NULL == pCurrTableFrame)
	{
		SendRequestFailure(pIServerUserItem,TEXT("服务器已满,请稍后重试！"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 第二步 生成桌子 */
	srand(static_cast<unsigned int >(time(NULL)));
	DWORD dwPassword = GenerateTablePassword();

	//设置桌子属性
	//pCurTableFrame->SetTableOwner(pIServerUserItem->GetUserID());
	pCurrTableFrame->SetTableID(dwPassword);

	//设置房间自动解散，默认一分钟 -- 这里是指不开始游戏 自动一分钟后解散
	pCurrTableFrame->SetTableAutoDismiss();

	/* 桌创建子信息写入数据库 && 发送给客户端(在消息号的后续环节中发送) */
	DWORD dwClubRoomID = pDbo->dwClubRoomID;
	DWORD dwUserID = pIServerUserItem->GetUserID();
	DWORD dwTableID = pCurrTableFrame->GetTableID();
    DWORD dwLockState = (pDbo->byTableMode == 2) ? 0 : 1;
	ClubTableCreate(dwClubRoomID, dwUserID, dwTableID, dwLockState);

	/* 设置房间规则 */
	//设置通用房间规则  
	pCurrTableFrame->SetCommonRule(pCfg);

	//设置子游戏房间规则
	tagTableSubGameRule *pSubGameCfg = (tagTableSubGameRule*)(CreateRoom.SubGameRule);
	if(pSubGameCfg != NULL)
	{
		pCurrTableFrame->SetSubGameRule(pSubGameCfg);
	}

	/* 用户坐下 */
	//用户坐下		TODO 后面让客户端主动发送
	if(pCurrTableFrame->PlayerSitTable(INVALID_CHAIR, pIServerUserItem, 0, true) != 0 )
	{
		SendRequestFailure(pIServerUserItem, TEXT("用户坐下失败!"), REQUEST_FAILURE_NORMAL);
		return false;
	}
	
	//发送创建房间成功消息
	g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_CLUB_CREATE_TABKE, NULL, 0);	

	return true;
}

//创建桌子 金币大厅桌子
bool CHandleFromGate::CreateTableHallGold(STR_DBO_CG_USER_JOIN_TABLE_HALL_GOLD * pDbo, CPlayer *pIServerUserItem)
{
	//内部使用, 不校验指针
	tagTableRule *pCfg = (tagTableRule*)pDbo->strCreateRoom.CommonRule;


	//检查加入门票
	if(!CheckCreateTableTicket(pCfg, pIServerUserItem))
	{
		//return true; //TODONOW 如果为false 客户端就断线重连了， 之后修改掉
	}

	/* 第一步 寻找空闲房间 */
	CTableFrame *pCurrTableFrame = GetNextEmptyTable();

	//桌子判断
	if(NULL == pCurrTableFrame)
	{
		SendRequestFailure(pIServerUserItem,TEXT("服务器已满,请稍后重试！"),REQUEST_FAILURE_NORMAL);
		return true; //TODONOW 如果为false 客户端就断线重连了， 之后修改掉
	}

	/* 第二步 生成桌子 */
	srand(static_cast<unsigned int >(time(NULL)));
	DWORD dwPassword = GenerateTablePassword();
	CLog::Log(log_debug, "dwPassword: %d", dwPassword);

	//设置桌子属性
	//pCurTableFrame->SetTableOwner(pIServerUserItem->GetUserID());
	pCurrTableFrame->SetTableID(dwPassword);

	//设置房间自动解散，默认一分钟 -- 这里是指不开始游戏 自动一分钟后解散
	pCurrTableFrame->SetTableAutoDismiss();

	/* 桌创建子信息写入数据库 && 发送给客户端(在消息号的后续环节中发送) */
	DWORD dwUserID = pIServerUserItem->GetUserID();
	DWORD dwKindID = pDbo->dwKindID;
	BYTE  byGameType = pDbo->byGameType;
	DWORD dwTableID = pCurrTableFrame->GetTableID();
	HallTableCreate(dwUserID, dwKindID, byGameType, dwTableID);

	/* 设置房间规则 */
	//设置通用房间规则  
	pCurrTableFrame->SetCommonRule(pCfg);

	//设置子游戏房间规则
	tagTableSubGameRule *pSubGameCfg = (tagTableSubGameRule*)(pDbo->strCreateRoom.SubGameRule);
	if(pSubGameCfg != NULL)
	{
		pCurrTableFrame->SetSubGameRule(pSubGameCfg);
	}

	/* 用户坐下 */
	WORD wChairID = pCurrTableFrame->GetNullChairID(); 
	//椅子号有效
	if (INVALID_CHAIR != wChairID)
	{
		//用户坐下		TODO 后面让客户端主动发送
		if(pCurrTableFrame->PlayerSitTable(wChairID, pIServerUserItem, 0, true) != 0)
		{
			SendRequestFailure(pIServerUserItem, TEXT("用户坐下失败!"), REQUEST_FAILURE_NORMAL);
			return true; //TODONOW 如果为false 客户端就断线重连了， 之后修改掉
		}		
	}
	else
	{
		SendRequestFailure(pIServerUserItem, TEXT("房间已满,无法加入房间!"), REQUEST_FAILURE_NORMAL);
		return true ;//TODONOW 如果为false 客户端就断线重连了， 之后修改掉
	}
	
	//发送加入金币房间成功
	g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_JOIN_ROOM_SUCCESS, NULL, 0);

	return true;
}


//创建桌子 俱乐部桌子
bool CHandleFromGate::CreateTableAutoClub(STR_DBO_CG_USER_JOIN_TABLE_NO_PASS * pDbo, CPlayer *pIServerUserItem)
{
	//内部使用, 不校验指针
	tagTableRule *pCfg = (tagTableRule*)pDbo->strCreateRoom.CommonRule;

	/* 第一步 寻找空闲房间 */
	CTableFrame *pCurrTableFrame = GetNextEmptyTable();

	//桌子判断
	if(NULL == pCurrTableFrame)
	{
		SendRequestFailure(pIServerUserItem,TEXT("服务器已满,请稍后重试！"),REQUEST_FAILURE_NORMAL);
		return true; //TODONOW 如果为false 客户端就断线重连了， 之后修改掉
	}

	/* 第二步 生成桌子 */
	srand(static_cast<unsigned int >(time(NULL)));
	DWORD dwPassword = GenerateTablePassword();

	//设置桌子属性
	//pCurTableFrame->SetTableOwner(pIServerUserItem->GetUserID());
	pCurrTableFrame->SetTableID(dwPassword);

	//设置房间自动解散，默认一分钟 -- 这里是指不开始游戏 自动一分钟后解散
	pCurrTableFrame->SetTableAutoDismiss();

	/* 桌创建子信息写入数据库 && 发送给客户端(在消息号的后续环节中发送) */
	DWORD dwClubRoomID = pDbo->dwClubRoomID;
	DWORD dwUserID = pIServerUserItem->GetUserID();
	DWORD dwTableID = pCurrTableFrame->GetTableID();
    DWORD dwLockState =  1; //默认为普通桌子(而不是私密桌子)
	ClubTableCreate(dwClubRoomID, dwUserID, dwTableID, dwLockState);


	/* 设置房间规则 */
	//设置通用房间规则  
	pCurrTableFrame->SetCommonRule(pCfg);

	//设置子游戏房间规则
	tagTableSubGameRule *pSubGameCfg = (tagTableSubGameRule*)(pDbo->strCreateRoom.SubGameRule);
	if(pSubGameCfg != NULL)
	{
		pCurrTableFrame->SetSubGameRule(pSubGameCfg);
	}

	/* 用户坐下 */
	WORD wChairID = pCurrTableFrame->GetNullChairID(); 
	//椅子号有效
	if (INVALID_CHAIR != wChairID)
	{
		//用户坐下		TODO 后面让客户端主动发送
		if(pCurrTableFrame->PlayerSitTable(wChairID, pIServerUserItem, 0, true) != 0)
		{
			SendRequestFailure(pIServerUserItem, TEXT("用户坐下失败!"), REQUEST_FAILURE_NORMAL);
			return true; //TODONOW 如果为false 客户端就断线重连了， 之后修改掉
		}		
	}
	else
	{
		SendRequestFailure(pIServerUserItem, TEXT("房间已满,无法加入房间!"), REQUEST_FAILURE_NORMAL);
		return true; //TODONOW 如果为false 客户端就断线重连了， 之后修改掉
	}
	
	//发送加入房卡房间成功
	g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_JOIN_ROOM_SUCCESS, NULL, 0);

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
	CPlayer *pIServerUserItem = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if (NULL == pIServerUserItem) return false;

	//校验数据包
	if(wDataSize != sizeof(STR_SUB_CG_USER_JOIN_FK_ROOM))
	{
		SendRequestFailure(pIServerUserItem,TEXT("加入牌友圈桌子数据错误"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//数据定义
	STR_SUB_CG_USER_JOIN_FK_ROOM *pJoin = (STR_SUB_CG_USER_JOIN_FK_ROOM *)pData;

	//向数据库查询
	STR_DBR_JOIN_ROOM DBR;
	DBR.dwTableID = pJoin->dwPassword;
	DBR.dwUserID = pIServerUserItem->GetUserID();

	//投递请求
	return g_GameCtrl->PostDataBaseRequest(DBR_CG_JOIN_TABLE, dwSocketID, &DBR,sizeof(DBR));
}

//加入桌子 返回 -- 需要密码
bool CHandleFromGate::On_CMD_GC_JOIN_TABLE( DWORD dwSocketID, VOID * pData, WORD wDataSize)
{
	/* 1. 校验用户 */
	CPlayer *pIServerUserItem = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if (NULL == pIServerUserItem) return false;

	/* 2. 校验数据包 */
	if(wDataSize != sizeof(STR_DBR_JOIN_ROOM))
	{
		SendRequestFailure(pIServerUserItem,TEXT("加入房间数据错误！"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 3. 数据库校验 */
	STR_DBR_JOIN_ROOM *pJoin = (STR_DBR_JOIN_ROOM *)pData;
	if(pJoin->lResultCode != 0)
	{
		SendRequestFailure(pIServerUserItem,TEXT("非工会成员, 不能进入工会桌子"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 4. 桌子校验 */
	DWORD dwPassword = pJoin->dwTableID;	
	if(dwPassword == 0)
	{
		SendRequestFailure(pIServerUserItem, TEXT("房间号错误,请重新尝试"), REQUEST_FAILURE_PASSWORD);
		return false;
	}

	/* 5. 校验是否在之前桌子中 */	
	WORD wOldTableID = pIServerUserItem->GetTableID();	
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

	//还在之前的房间中(这里应该不会走进来, 因为有断线重连的存在)
	if(INVALID_TABLE != wOldTableID)
	{
		SendRequestFailure(pIServerUserItem,TEXT("您已经在游戏中,不能进入其他房间!"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 6. 寻找指定桌子 */
	CTableFrame *pCurrTableFrame = GetDesignatedTable(dwPassword);
	//桌子判断
	if(NULL == pCurrTableFrame)
	{
		SendRequestFailure(pIServerUserItem, TEXT("桌子不存在"), REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 7. 房间规则校验 */
	tagTableRule *pCfg = (tagTableRule*)pCurrTableFrame->GetCustomRule();
	if (NULL == pCfg)
	{
		SendRequestFailure(pIServerUserItem, TEXT("加入失败, 房间规则不存在"), REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 8. 桌子游戏状态 判断 */
	if(GAME_STATUS_FREE != pCurrTableFrame->GetGameStatus())
	{
		SendRequestFailure(pIServerUserItem, TEXT("房间正在游戏中,无法加入"), REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 9. 门票校验 */
	if(!CheckJoinTableTicket(pCfg, pIServerUserItem))
	{
		return false;
	}

	/* 10. 玩家坐下判断 */
	WORD wChairID = pCurrTableFrame->GetNullChairID();//寻找空椅子
	if (wChairID != INVALID_CHAIR)
	{
		if(pCurrTableFrame->PlayerSitTable(wChairID, pIServerUserItem, 0, true) != 0)
		{
			SendRequestFailure(pIServerUserItem,TEXT("加入房间失败, 坐下失败"),REQUEST_FAILURE_PASSWORD);
			return false;
		}	
	}
	else
	{
		SendRequestFailure(pIServerUserItem,TEXT("房间已满,请更换其他房间"),REQUEST_FAILURE_NORMAL);
		return false;
	}
	
	//发送加入房卡房间成功
	g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_JOIN_ROOM_SUCCESS, NULL, 0);

	return true;
}

//加入桌子 -- 不需要密码, 即快速开始
bool CHandleFromGate::On_SUB_CG_USER_JOIN_TABLE_NO_PASS(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验用户
	WORD wBindIndex = LOWORD(dwSocketID);
	CPlayer *pIServerUserItem = GetBindUserItem(wBindIndex);
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
	WORD wBindIndex = LOWORD(dwSocketID);
	CPlayer *pIServerUserItem = GetBindUserItem(wBindIndex);
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
		return CreateTableAutoClub(pJoin ,  pIServerUserItem);
	}


	/* 4. 桌子校验 */
	DWORD dwPassword = pJoin->dwPassword;
	if(dwPassword == 0)
	{
		SendRequestFailure(pIServerUserItem, TEXT("桌子号错误,请重新尝试"), REQUEST_FAILURE_PASSWORD);
		return false;
	}

	/* 5. 校验是否在之前的游戏中 */
	WORD wOldTableID = pIServerUserItem->GetTableID(); //旧桌子号	
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
	CTableFrame *pCurrTableFrame = GetDesignatedTable(dwPassword);
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
	WORD wChairID = pCurrTableFrame->GetNullChairID();//寻找空椅子
	if (wChairID != INVALID_CHAIR)
	{
		if(pCurrTableFrame->PlayerSitTable(wChairID, pIServerUserItem, 0, true) != 0)
		{
			SendRequestFailure(pIServerUserItem,TEXT("加入失败, 坐下失败!"),REQUEST_FAILURE_PASSWORD);
			return false;
		}
	}
	else
	{
		SendRequestFailure(pIServerUserItem,TEXT("房间已满,请更换其他房间!"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//发送加入房卡房间成功
	g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_JOIN_ROOM_SUCCESS, NULL, 0);

	return true;
}

//加入桌子 -- 金币大厅桌子
bool CHandleFromGate::On_SUB_CG_USER_JOIN_GOLD_HALL_ROOM(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//校验用户
	CPlayer *pIServerUserItem = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if (NULL == pIServerUserItem) return true;

	//校验数据包
	if(wDataSize != sizeof(STR_SUB_CG_USER_JOIN_GOLD_HALL_ROOM))
	{
		SendRequestFailure(pIServerUserItem,TEXT("加入房间 结构体数据错误！"),REQUEST_FAILURE_NORMAL);
		return true;
	}

	//数据定义
	STR_SUB_CG_USER_JOIN_GOLD_HALL_ROOM *pJoin = (STR_SUB_CG_USER_JOIN_GOLD_HALL_ROOM *)pData;

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CG_USER_JOIN_TABLE_HALL_GOLD, dwSocketID, pData,wDataSize);

	return true;
}

//加入桌子 -- 金币大厅桌子 返回
bool CHandleFromGate::On_CMD_GC_USER_JOIN_TABLE_HALL_GOLD( DWORD dwSocketID, VOID * pData, WORD wDataSize)
{
	/* 1. 校验用户 */
	CPlayer *pIServerUserItem = CPlayerManager::FindPlayerBySocketID(dwSocketID);
	if (NULL == pIServerUserItem) return false;

	/* 2. 校验数据包 */
	if(wDataSize != sizeof(STR_DBO_CG_USER_JOIN_TABLE_HALL_GOLD))
	{
		SendRequestFailure(pIServerUserItem,TEXT("加入房间数据错误！"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* 3. 数据库校验 */
	STR_DBO_CG_USER_JOIN_TABLE_HALL_GOLD *pJoin = (STR_DBO_CG_USER_JOIN_TABLE_HALL_GOLD *)pData;

	if( pJoin->lResultCode != 0)//返回值不为0, 则认为身上金币不足 或者 不是本公司的人
	{
		SendRequestFailure(pIServerUserItem,TEXT("身上金币不足, 无法加入"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	if( pJoin->dwPassword == 0)//桌子号为空, 则认为没有找到对应的桌子
	{
		//如果没有找到金币大厅的桌子, 则直接创建一个金币大厅的桌子
		return CreateTableHallGold(pJoin ,  pIServerUserItem);
	}

	/* 4. 桌子校验 */
	DWORD dwPassword = pJoin->dwPassword;	
	if(dwPassword == 0)
	{
		SendRequestFailure(pIServerUserItem, TEXT("桌子号错误,请重新尝试"), REQUEST_FAILURE_PASSWORD);
		return false;
	}

	/* 5. 校验是否在之前的游戏中 */
	WORD wOldTableID = pIServerUserItem->GetTableID(); //旧桌子号	
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
	CTableFrame *pCurrTableFrame = GetDesignatedTable(dwPassword);
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
	WORD wChairID = pCurrTableFrame->GetNullChairID();//寻找空椅子
	if (wChairID != INVALID_CHAIR)
	{
		if(pCurrTableFrame->PlayerSitTable(wChairID, pIServerUserItem, 0, true) != 0)
		{
			SendRequestFailure(pIServerUserItem,TEXT("加入失败, 坐下失败!"),REQUEST_FAILURE_PASSWORD);
			return false;
		}
	}
	else
	{
		SendRequestFailure(pIServerUserItem,TEXT("房间已满,请更换其他房间!"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//发送加入房间成功
	g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_JOIN_ROOM_SUCCESS, NULL, 0);
	return true;
}

//断线重连
bool CHandleFromGate::On_SUB_User_ReconnectRoom(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{	
	//获校验户
	WORD wBindIndex = LOWORD(dwSocketID);
	CPlayer *pIServerUserItem = GetBindUserItem(wBindIndex);
	if (NULL == pIServerUserItem)
	{
		return true;
	}

	//校验数据包
	if ( wDataSize != sizeof(STR_SUB_CG_USER_RECONNECT_ROOM))
	{
		return true;
	}

	//校验用户桌子号和椅子号
	WORD wChairID = pIServerUserItem->GetChairID();
	WORD wOldTableID = pIServerUserItem->GetTableID();

	//判断历史桌子是否还在游戏
	if(wOldTableID != INVALID_TABLE)
	{
		if(wOldTableID > CTableManager::TableCount()) //无效桌子
		{
			wOldTableID = INVALID_TABLE;
		}
		else //桌子已经结束, 则将wOldTableID设置为无效
		{
			CTableFrame* pOldTable = CTableManager::FindTableByTableID(wOldTableID);
			if(pOldTable == NULL)
			{
				wOldTableID = INVALID_TABLE;
			}
		}	
	}

	//椅子号或桌子号无效, 直接退出
	if (INVALID_TABLE == wOldTableID || INVALID_CHAIR == wChairID)
	{
		return true;
	}

	//构造断线重连返回数据
	STR_CMD_GC_USER_RECONNECT_ROOM ReJoinResult;
	ZeroMemory(&ReJoinResult, sizeof(STR_CMD_GC_USER_RECONNECT_ROOM));

	//用户坐下
	if( CTableManager::FindTableByTableID(wOldTableID)->PlayerSitTable(wChairID, pIServerUserItem) != 0 )
	{
		BYTE OldGameStatus = pIServerUserItem->GetOldGameStatus();

		if(OldGameStatus == US_READY)
		{
			pIServerUserItem->SetUserStatus(US_SIT, wOldTableID, wChairID);
		}
		else
		{
			pIServerUserItem->SetUserStatus(OldGameStatus, wOldTableID, wChairID);	
		}

		//发送房间消息
		tagTableRule* pRule = (tagTableRule* )CTableManager::FindTableByTableID(wOldTableID)->GetCustomRule();
		ReJoinResult.retCode = 0;
		memcpy(&ReJoinResult.strTableRule, pRule, sizeof(ReJoinResult.strTableRule));
		ReJoinResult.wChairID = wChairID;
		ReJoinResult.dwPassWord = CTableManager::FindTableByTableID(wOldTableID)->GetTableID();
		ReJoinResult.cbGameCount = pRule->GameCount;
		ReJoinResult.cbPlayerCount = pRule->PlayerCount;
		ReJoinResult.cbPayType = pRule->cbPayType;
		ReJoinResult.lSinglePayCost = pRule->lSinglePayCost;
		ReJoinResult.gameMod = pRule->GameMode;
	}
	else
	{
		return true;
	}

	//发送数据
	g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_RECONNECT_ROOM, &ReJoinResult, sizeof(STR_CMD_GC_USER_RECONNECT_ROOM));

	//断线重连成功，删除list
	if (ReJoinResult.retCode == 0)
	{
		//游戏服处理  TODONOW
		pIServerUserItem->SetOfflineGameID(0);

		//发送给协调服, 再转发给登录服
		tagOfflineUser data;
		data.dwUserID = pIServerUserItem->GetUserID();
		data.byMask = 2; //表示删除断线用户
		g_TCPSocketEngine->SendData(MDM_USER,SUB_CS_C_USER_OFFLINE,&data,sizeof(tagOfflineUser));
	}
	return true;
}

//创建牌友圈房间
bool CHandleFromGate::On_Sub_CG_User_CreateGroupRoom(VOID *pData, WORD wDataSize, DWORD dwSocketID)
{	
	//用户校验
	WORD wBindIndex = LOWORD(dwSocketID);
	CPlayer *pIServerUserItem = GetBindUserItem(wBindIndex);	
	if (pIServerUserItem==NULL) return true;

	//数据包校验
	if(wDataSize != sizeof(STR_SUB_CG_USER_CREATE_GROUP_ROOM))
		return true;

	//SUB数据
	STR_SUB_CG_USER_CREATE_GROUP_ROOM *pCreateRoom = (STR_SUB_CG_USER_CREATE_GROUP_ROOM*)pData;

	//校验圈主是不是已经创建过牌友圈房间，创建过且房间都未坐满人，就不能再次创建房间
	for (WORD i=0; i<CTableManager::TableCount(); i++)
	{
		//获取对象
		CTableFrame *pGroupTable = CTableManager::FindTableByIndex(i);
		if(NULL == pGroupTable) continue;

		//是牌友圈房间，并且有一个桌子未开始游戏，则说明还有空桌子加入，不需要再创建房间
		if ( pGroupTable->GetGroupID() == pCreateRoom->dwGroupID)
		{
			//返回创建成功
			STR_CMD_GC_USER_CREATE_GROUP_ROOM CMDCreate;
			ZeroMemory(&CMDCreate, sizeof(STR_CMD_GC_USER_CREATE_GROUP_ROOM));
			CMDCreate.dwResultCode = DB_SUCCESS;
			lstrcpyn(CMDCreate.szDescribeString, TEXT("牌友圈还有空房间"), sizeof(CMDCreate.szDescribeString));

			//发送数据
			g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_CREATE_GROUP_ROOM, &CMDCreate, sizeof(STR_CMD_GC_USER_CREATE_GROUP_ROOM));	

			return true;
		}
	}

	//构造DBR数据
	STR_DBR_CG_USER_CREATE_GROUP_ROOM CreateGroupRoom;
	ZeroMemory(&CreateGroupRoom, sizeof(STR_DBR_CG_USER_CREATE_GROUP_ROOM));

	//赋值
	memcpy_s(&CreateGroupRoom, sizeof(STR_DBR_CG_USER_CREATE_GROUP_ROOM), pCreateRoom, sizeof(STR_DBR_CG_USER_CREATE_GROUP_ROOM));

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CG_USER_CREATE_GROUP_ROOM, dwSocketID, &CreateGroupRoom, sizeof(STR_DBR_CG_USER_CREATE_GROUP_ROOM));
	
	return true;
}

//创建牌友圈房间返回
bool CHandleFromGate::On_CMD_GC_User_CreateGroupRoom(DWORD dwSocketID, VOID * pData, WORD wDataSize)
{
	//判断在线

	//数据校验
	ASSERT(wDataSize == sizeof(STR_DBO_CG_USER_CREATE_GROUP_ROOM));
	if(wDataSize != sizeof(STR_DBO_CG_USER_CREATE_GROUP_ROOM))
		return false;

	//获取用户
	WORD wBindIndex = LOWORD(dwSocketID);
	CPlayer *pIServerUserItem = GetBindUserItem(wBindIndex);

	//定义
	STR_DBO_CG_USER_CREATE_GROUP_ROOM *pDBOGroup = (STR_DBO_CG_USER_CREATE_GROUP_ROOM *)pData;

	//构造返回数据
	STR_CMD_GC_USER_CREATE_GROUP_ROOM CMDCreate;
	ZeroMemory(&CMDCreate, sizeof(STR_CMD_GC_USER_CREATE_GROUP_ROOM));

	//赋值
	CMDCreate.dwResultCode = pDBOGroup->dwResultCode;
	lstrcpyn(CMDCreate.szDescribeString, pDBOGroup->szDescribeString, CountArray(CMDCreate.szDescribeString));
	
	//执行失败
	if (DB_SUCCESS != pDBOGroup->dwResultCode )
	{
		g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_CREATE_GROUP_ROOM, &CMDCreate, sizeof(STR_CMD_GC_USER_CREATE_GROUP_ROOM));	
		return true;
	}

	//房间规则
	tagTableRule *pCfg = (tagTableRule* )pDBOGroup->rule;

	//校验用户房卡
	if ( pIServerUserItem->GetUserRoomCard() < pCfg->lSinglePayCost)
	{
		CMDCreate.dwResultCode = 1;
		lstrcpyn(CMDCreate.szDescribeString, TEXT("房卡不足，请充值"), CountArray(CMDCreate.szDescribeString));

		g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_CREATE_GROUP_ROOM, &CMDCreate, sizeof(STR_CMD_GC_USER_CREATE_GROUP_ROOM));	
		return true;
	}

	//用户最多能创建的牌友圈房间数
	WORD wCreateTableNum = static_cast<WORD> (pIServerUserItem->GetUserRoomCard() / pCfg->lSinglePayCost);
	
	//寻找空闲桌子，为牌友圈分配固定的桌子数
	srand(static_cast<unsigned int >(time(NULL)));
	int nTableNum = 0;
	for (WORD i=0; i<CTableManager::TableCount(); i++)
	{
		//获取对象
		CTableFrame *pGroupTable = CTableManager::FindTableByIndex(i);
		if(NULL == pGroupTable) continue;

		//桌子无人坐下，并且不是替别人开的房间
		if ( (pGroupTable->GetSitUserCount()==0) && 
			 (pGroupTable->GetCreateTableUser() == 0))
		{
			//牌友圈桌子数限制
			if ( (nTableNum >= CTableManager::TableCount()) || 
				 (NUM_MAX_GOURP_CREATE_ROOM == nTableNum) ||
				 nTableNum >= wCreateTableNum)
			{
				break;
			}

			//设置桌子属性	
			DWORD dwPassword = GenerateTablePassword();		
			pGroupTable->SetTableID(dwPassword);
			pGroupTable->SetGroupID(pDBOGroup->dwGroupID);			
			pGroupTable->SetCommonRule(pCfg);
			pGroupTable->SetCreateTableUser(pIServerUserItem->GetUserID());		//设置创建玩家ID

			nTableNum++;

			//TODO 需要添加开房信息
		}
	}

	//发送创建成功消息
	_sntprintf_s(CMDCreate.szDescribeString, CountArray(CMDCreate.szDescribeString), TEXT("您已经创建[%d]个牌友圈房间，扣除房卡[%ld]"), 
		      nTableNum, nTableNum*pCfg->lSinglePayCost);
	g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_CREATE_GROUP_ROOM, &CMDCreate, sizeof(STR_CMD_GC_USER_CREATE_GROUP_ROOM));	

	//扣除创建用户房卡
	SCORE lDeductRoomCard = (-nTableNum)*((int)pCfg->lSinglePayCost);
	
	//
	//OnEventUserOpenRoomCard(pIServerUserItem, pIServerUserItem->GetUserID(), lDeductRoomCard);

	return true;
}

//加入牌友圈房间
bool CHandleFromGate::On_Sub_CG_User_JoinGroupRoom(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验参数
	ASSERT(wDataSize == sizeof(STR_SUB_CG_USER_JOIN_GROUP_ROOM));
	if (wDataSize != sizeof(STR_SUB_CG_USER_JOIN_GROUP_ROOM)) return true;

	//处理消息
	STR_SUB_CG_USER_JOIN_GROUP_ROOM *pJoinGroupRoom = (STR_SUB_CG_USER_JOIN_GROUP_ROOM *)pData;

	//绑定信息
	CPlayer *pIServerUserItem = CPlayerManager::FindPlayerBySocketID(dwSocketID);

	//校验用户
	if (NULL == pIServerUserItem)
		return true;

	//校验用户是不是在游戏中
	WORD wOldTableID = pIServerUserItem->GetTableID();
	WORD wOldChairID = pIServerUserItem->GetChairID();
	BYTE cbUserStatus = pIServerUserItem->GetUserInfo()->cbUserStatus;

	//用户在游戏中
	if(wOldTableID != INVALID_TABLE || 
		wOldChairID != INVALID_CHAIR ||
		cbUserStatus == US_PLAYING )
	{
		SendRequestFailure(pIServerUserItem, TEXT("您已经在游戏中,不能进入其他房间!"), REQUEST_FAILURE_NORMAL);
		return true;
	}

	//构造DBR数据
	STR_DBR_CG_USER_JOIN_GROUP_ROOM JoinRoom;
	ZeroMemory(&JoinRoom, sizeof(STR_DBR_CG_USER_JOIN_GROUP_ROOM));

	//赋值
	memcpy_s(&JoinRoom, sizeof(STR_DBR_CG_USER_JOIN_GROUP_ROOM), pJoinGroupRoom, sizeof(STR_DBR_CG_USER_JOIN_GROUP_ROOM));

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CG_USER_JOIN_GROUP_ROOM, dwSocketID, &JoinRoom, sizeof(STR_DBR_CG_USER_JOIN_GROUP_ROOM));

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
	WORD wBindIndex = LOWORD(dwSocketID);
	CPlayer *pIServerUserItem = GetBindUserItem(wBindIndex);
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
	for(int i= 0;i < CTableManager::TableCount();i++)
	{
		CTableFrame *pTableFrame = CTableManager::FindTableByIndex(i);
		//校验桌子
		if( (NULL != pTableFrame) && 
			(pTableFrame->GetGroupID() == pDBOGroup->dwGroupID) && 
			(GAME_STATUS_FREE == pTableFrame->GetGameStatus()) && 
			(0 != pTableFrame->GetTableID()) )
		{
			//找到房间，判断是否有空位置
			WORD wChairID = pTableFrame->GetNullChairID();
			if(wChairID != INVALID_CHAIR)
			{
				//用户坐下
				if(pTableFrame->PlayerSitTable(wChairID, pIServerUserItem, 0, true) != 0)
				{
					SendRequestFailure(pIServerUserItem, TEXT("房间号不对!"), REQUEST_FAILURE_PASSWORD);
					return true;
				}	

				//用户可以加入牌友圈房间
				bHaveRoom = true;

				//发送加入成功消息
				g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_JOIN_ROOM_SUCCESS, NULL, 0);
				break;
			}
		}
	}

	//用户无法加入，发送错误提示
	if ( !bHaveRoom )
	{
		SendRequestFailure(pIServerUserItem, TEXT("牌友圈房间已满或不存在已开房间，请联系管理"), REQUEST_FAILURE_PASSWORD);
	}

	return true;
}

//加入金币房间
bool CHandleFromGate::On_SUB_User_JoinGoldRoom(VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	//用户效验
	WORD wBindIndex = LOWORD(dwSocketID);
	CPlayer *pIServerUserItem = GetBindUserItem(wBindIndex);		
	if (NULL == pIServerUserItem) return true;

	//数据包校验
	if ( wDataSize != sizeof(CMD_GR_Create_Gold_Room) )
		return false;

	//数据定义
	CMD_GR_Create_Gold_Room *pGoldRoom = (CMD_GR_Create_Gold_Room*)pData;

	//校验用户桌子和椅子
	if (pIServerUserItem->GetChairID()!=INVALID_CHAIR) 
	{
		SendRequestFailure(pIServerUserItem,TEXT("正在游戏中,无法加入！"),REQUEST_FAILURE_NORMAL);
		return true;
	}
	if(pIServerUserItem->GetTableID()!=INVALID_TABLE)
	{
		SendRequestFailure(pIServerUserItem,TEXT("正在房间中,无法加入！"),REQUEST_FAILURE_NORMAL);
		return true;
	}
	//校验用户金币	Modified by lizhihu 只和游戏底分校验
	if( pIServerUserItem->GetUserGold() < pGoldRoom->dwCellScore )
	{
		SendRequestFailure(pIServerUserItem,TEXT("金币不足,无法加入！"),REQUEST_FAILURE_NORMAL);
		return true;
	}

	//寻找金币房空椅子
	WORD wChairID;
	CTableFrame *pTableFrame = GetGlodRoomEmptyChair(wChairID);

	//找到金币房空椅子
	if ( (INVALID_CHAIR != wChairID) && (NULL != pTableFrame) )
	{
		//已有空椅子，用户坐下
		if(pTableFrame->PlayerSitTable(wChairID, pIServerUserItem, 0, true) == 0)
		{
			//发送数据
			g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_JOIN_ROOM_SUCCESS, NULL, 0);
			return true;
		}	
	}

	//没有金币场空位置，重新创建一个金币房间
	CTableFrame *pCurrTableFrame = GetNextEmptyTable();

	//桌子校验
	if(pCurrTableFrame == NULL)
	{
		SendRequestFailure(pIServerUserItem, TEXT("服务器已满,请稍后重试！"), REQUEST_FAILURE_NORMAL);
		return true;
	}

	//配置金币房规则	写在这里没有通用性，最好的是在客户端配置，规则由每个客户端的子游戏决定，传过来
	tagTableRule cfg;
	ZeroMemory(&cfg, sizeof(tagTableRule));

	cfg.GameMode = TABLE_MODE_GOLD;
	cfg.GameCount = 0;
	cfg.PlayerCount = 4;
	cfg.RobBankType = 3;
	cfg.CellScore = pGoldRoom->dwCellScore;
	cfg.JoinScore = 500;
	cfg.LeaveScore = 500;
	cfg.MaxFan = 4;
	/* 注释掉子游戏特有数据
	cfg.ZhangMao = 1;					//长毛
	cfg.XuanFengGang = 1;				//旋风杠
	cfg.QiXiaoDui = 1;					//七小对
	cfg.QingYiSe = 1;					//清一色
	cfg.KaiHuaGang = 1;					//杠开
	*/

	//pCurrTableFrame->SetTableOwner(pIServerUserItem->GetUserID());
	pCurrTableFrame->SetCommonRule(&cfg);

	//寻找空椅子
	wChairID = pCurrTableFrame->GetNullChairID();
	if (wChairID != INVALID_CHAIR)
	{
		//用户坐下
		if(!pCurrTableFrame->PlayerSitTable(wChairID, pIServerUserItem, 0, true) != 0)
		{
			SendRequestFailure(pIServerUserItem,TEXT("服务器已满,请稍后重试!"),REQUEST_FAILURE_NORMAL);
			return true;
		}		
	}
	else
	{
		SendRequestFailure(pIServerUserItem,TEXT("您已经在游戏中,无法加入!"),REQUEST_FAILURE_NORMAL);
		return true;
	}

	//返回加入成功
	g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_JOIN_ROOM_SUCCESS, NULL ,0);

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

//用户离开
VOID CHandleFromGate::OnEventUserLogout(CPlayer * pIServerUserItem, DWORD dwLeaveReason)
{
	//变量定义
	DBR_GR_LeaveGameServer LeaveGameServer;
	ZeroMemory(&LeaveGameServer,sizeof(LeaveGameServer));

	//LeaveGameServer.lControlScore = pIServerUserItem->GetUserControlScore();

	//用户信息
	LeaveGameServer.dwLeaveReason=dwLeaveReason;
	LeaveGameServer.dwUserID=pIServerUserItem->GetUserID();
	LeaveGameServer.dwOnLineTimeCount=(DWORD)(time(NULL))-pIServerUserItem->GetLogonTime();

	//连接信息
	LeaveGameServer.dwClientAddr=pIServerUserItem->GetClientAddr();
	lstrcpyn(LeaveGameServer.szMachineID,pIServerUserItem->GetMachineID(),CountArray(LeaveGameServer.szMachineID));

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_GR_LEAVE_GAME_SERVER,0L,&LeaveGameServer,sizeof(LeaveGameServer));

	return;
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

//绑定用户
CPlayer * CHandleFromGate::GetBindUserItem(WORD wBindIndex)
{
	CPlayer *pPlayer = NULL;

	//获取用户
	pPlayer = CPlayerManager::FindPlayerByEnum(wBindIndex);
	if (pPlayer == NULL)
	{
		return NULL;
	}

	return pPlayer;
}

//群发数据
bool CHandleFromGate::SendDataBatchToMobileUser(WORD wCmdTable, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//枚举用户
	WORD wEnumIndex=0;
	while(wEnumIndex<CPlayerManager::GetPlayerCount())
	{
		//过滤用户
		CPlayer *pIServerUserItem=CPlayerManager::FindPlayerByEnum(wEnumIndex++);
		if(pIServerUserItem==NULL) continue;


		//定义变量
		WORD wTagerTableID = pIServerUserItem->GetTableID();

		//状态过滤
		if(pIServerUserItem->GetUserStatus() >= US_SIT)
		{
			if(wTagerTableID != wCmdTable)continue;
		}


		//发送消息
		g_GameCtrl->SendData(pIServerUserItem,wMainCmdID,wSubCmdID,pData,wDataSize);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
//添加替他人开房
bool CHandleFromGate::AddOtherRoomInfo(DBR_GR_UpdateTableInfo* pTableInfo)
{
	//无效过滤
	if (pTableInfo==NULL) return false;

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_GR_ADD_TABLE_INFO, 0, pTableInfo, sizeof(DBR_GR_UpdateTableInfo));

	return true;
}

//开始替他人开房
void CHandleFromGate::StartTable(DWORD dwTableID)
{
	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_GR_START_TABLE,0,&dwTableID,sizeof(dwTableID));
}

//结束替他人开房
void CHandleFromGate::ConcludeTable(DWORD dwTableID)
{
	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_GR_END_TABLE,0,&dwTableID,sizeof(dwTableID));
}

#pragma region Club牌友圈2 事件通知(1.桌子信息 2.玩家信息)
//club 创建桌子
void CHandleFromGate::ClubTableCreate(DWORD dwClubRoomID, DWORD dwUserID, DWORD dwTableID, DWORD dwLockState)
{
    STR_DBR_CLUB_TABLE_INFO Dbr;
	ZeroMemory(&Dbr, sizeof(Dbr));
	Dbr.dwClubRoomID = dwClubRoomID;
	Dbr.dwUserID = dwUserID;
	Dbr.dwTableID =dwTableID;
    Dbr.dwLockState =dwLockState;
	Dbr.dwTableState = 0;
	Dbr.byCurrentRound = 0;
	Dbr.byMask =1;

	Dbr.byCompanyID = _MYSTERY;
	Dbr.dwGameID = (g_GameCtrl->GetServerID()) & 0xFFFFFF00;

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_CLUB_TABLE_INFO, 0, &Dbr, sizeof(Dbr));
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

//用户加入替他人开房
void CHandleFromGate::JoinTable(DWORD dwTableID, DWORD dwUserID)
{
	
	DBR_GP_AddTableUser AddTableUser;
	AddTableUser.dwTableID = dwTableID;
	AddTableUser.dwUserID = dwUserID;
	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_GR_ADD_TABLEUSER,0,&AddTableUser,sizeof(DBR_GP_AddTableUser));
}

//用户离开替他人开房
void CHandleFromGate::LeaveTable(DWORD dwTableID, DWORD dwUserID)
{
	DBR_GP_DeleteTableUser DeleteTableUser;
	DeleteTableUser.dwTableID = dwTableID;
	DeleteTableUser.dwUserID = dwUserID;

	//投递请求
	g_GameCtrl->PostDataBaseRequest(DBR_GR_DELETE_TABLEUSER,0,&DeleteTableUser,sizeof(DBR_GP_DeleteTableUser));
}

//抽奖结果
bool CHandleFromGate::OnDBLotteryResult( DWORD dwSocketID, VOID * pData, WORD wDataSize )
{
	//参数校验
	ASSERT(wDataSize==sizeof(DBO_GR_LotteryResult));
	if(wDataSize!=sizeof(DBO_GR_LotteryResult))
		return false;

	DBO_GR_LotteryResult *pLotteryResult = (DBO_GR_LotteryResult*)pData;

	CMD_GR_LotteryResult LotteryResult;
	ZeroMemory(&LotteryResult,sizeof(CMD_GR_LotteryResult));

	LotteryResult.byIndex = pLotteryResult->byIndex;
	LotteryResult.byType = pLotteryResult->byType;
	LotteryResult.dwRewardCount = pLotteryResult->dwRewardCount;
	lstrcpyn(LotteryResult.szDescribe,pLotteryResult->szDescribe,CountArray(LotteryResult.szDescribe));
	lstrcpyn(LotteryResult.szPacketID,pLotteryResult->szPacketID,CountArray(LotteryResult.szPacketID));
	//memcpy(&LotteryResult,pLotteryResult,sizeof(CMD_GP_LotteryResult));

	wprintf(TEXT("pLottery:%d,%d,%d,%s,%s\n"),pLotteryResult->byType,pLotteryResult->byIndex,pLotteryResult->dwRewardCount,pLotteryResult->szPacketID,pLotteryResult->szDescribe);
	wprintf(TEXT("Lottery:%d,%d,%d,%s,%s\n"),LotteryResult.byType,LotteryResult.byIndex,LotteryResult.dwRewardCount,LotteryResult.szPacketID,LotteryResult.szDescribe);

	g_GameCtrl->SendData(dwSocketID,MDM_G_FRAME,SUB_GR_LOTTERY_RESULT,&LotteryResult,sizeof(CMD_GR_LotteryResult));

	return true;
}

//Club牌友圈创建桌子
bool CHandleFromGate::On_CMD_CG_CLUB_CREATE_TABLE( DWORD dwSocketID, VOID * pData, WORD wDataSize )
{
	//获取用户
	WORD wBindIndex = LOWORD(dwSocketID);
	CPlayer *pIServerUserItem = GetBindUserItem(wBindIndex);
	//用户校验
	if (pIServerUserItem==NULL) return false;

	//校验参数
	if(wDataSize!=sizeof(STR_DBO_GC_CLUB_CREATE_TABLE))
		return false;

	STR_DBO_GC_CLUB_CREATE_TABLE *pDbo = (STR_DBO_GC_CLUB_CREATE_TABLE*)pData;

	//流程处理
	return CreateTableClub(pDbo, pIServerUserItem);
}

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

//生成桌子ID，即加入房间密码
DWORD CHandleFromGate::GenerateTablePassword()
{	
	//生成密码
	srand(static_cast<unsigned int >(time(NULL)));
	BYTE byHigh1 = 1+  rand() % 219 + 24;
	BYTE byHigh2 = ((g_GameCtrl->GetServerID()) >> 16) &  0xFF;
	BYTE byHign3 = ((g_GameCtrl->GetServerID()) >> 8) & 0xF;
	DWORD dwPassword = (byHigh1 << 12) + (byHigh2 <<4) + byHign3;

	//判断桌子密码是否存在
	while(true)
	{
		bool bFind = false;
		for (WORD i=0; i<CTableManager::TableCount(); i++)
		{
			//获取对象
			CTableFrame *pTableFrame = CTableManager::FindTableByIndex(i);

			//桌子状态判断
			if ( (NULL != pTableFrame) && 
				 (0 < pTableFrame->GetSitUserCount() ) &&
				 (dwPassword == pTableFrame->GetTableID()) )
			{
				bFind = true;
				break;
			}
		}
		//若有桌子已经用到该密码，重置密码	TODONOW 不能再用原来的生成方式
		if(bFind) 
		{
			//added by lizhihu
			byHigh1 = 1+  rand() % 219 + 24;
			dwPassword = (byHigh1 << 12) + (byHigh2 <<4) + byHign3;

			/*dwPassword = ((1 + rand() % 5) << 17) + ((rand() % 512) << 8) + (BYTE)(g_GameCtrl->GetServerID() & 0XFF);*/
		}
		else
			break;
	}

	return dwPassword;
}

//寻找下一个空桌子
CTableFrame* CHandleFromGate::GetNextEmptyTable()
{
	//建立对象
	CTableFrame *pTableFrame = NULL;

	//寻找下一个空桌子
	for (WORD i=0;i<CTableManager::TableCount();i++)
	{
		//获取对象
		pTableFrame = CTableManager::FindTableByIndex(i);

		//桌子判断
		if ( (NULL != pTableFrame) && 
			 (0 == pTableFrame->GetSitUserCount()) && 
			 (0 == pTableFrame->GetCreateTableUser()) ) 
		{
			CLog::Log(log_debug, "寻找到空桌子");
			return pTableFrame;
		}
	}

	//创建桌子
	pTableFrame = CTableManager::CreateTable();		//建桌函数待添加CreateTable
	if (pTableFrame == NULL)
	{
		//创建失败了
		return NULL;
	}

	CLog::Log(log_debug, "创建空桌子");
	return pTableFrame;
}

//根据密码寻找指定桌子
CTableFrame* CHandleFromGate::GetDesignatedTable(const DWORD &dwPassword)
{
	//寻找指定桌子
	for (WORD i=0; i<CTableManager::TableCount(); i++)
	{
		//获取对象
		CTableFrame *pTableFrame = CTableManager::FindTableByIndex(i);

		//桌子判断
		if ( (NULL != pTableFrame) && 
			 (dwPassword == pTableFrame->GetTableID()) )
		{
			return pTableFrame;
		}
	}

	return NULL;
}

//查找金币房空椅子
CTableFrame* CHandleFromGate::GetGlodRoomEmptyChair(WORD &wChairID)
{
	//变量定义
	CTableFrame *pTableFrame = NULL;
	wChairID = INVALID_CHAIR;

	//寻找金币房空椅子
	for (WORD i=0; i<CTableManager::TableCount(); i++)
	{
		//获取对象
		CTableFrame *pTableFrame = CTableManager::FindTableByIndex(i);

		//桌子校验
		if ( (NULL == pTableFrame) || 
			 (pTableFrame->GetGameStatus() != GAME_STATUS_FREE) || 
			 (pTableFrame->GetTableMode() != TABLE_MODE_GOLD))
			continue;

		//获取空椅子
		wChairID = pTableFrame->GetNullChairID();
		if(wChairID == INVALID_CHAIR)
			continue;

		return pTableFrame;
	}

	return NULL;
}

//处理替他人开房流程
void CHandleFromGate::HandleCreateTableForOthers(CTableFrame *pCurTableFrame, CPlayer *pIServerUserItem, tagTableRule *pCfg)
{
	//校验
	if ( NULL == pCurTableFrame || 
		 NULL == pIServerUserItem ||
		 NULL == pCfg)
	{
		return;
	}

	//生成桌子密码
	srand(static_cast<unsigned int >(time(NULL)));
	DWORD dwPassword = GenerateTablePassword();

	//设置房间密码和规则
	pCurTableFrame->SetTableID(dwPassword);

	//设置房间自动解散,替他人开房，房间暂时保留十分钟
	pCurTableFrame->SetTableAutoDismiss(10);
	pCurTableFrame->SetCreateTableUser(pIServerUserItem->GetUserID()); //设置创建玩家ID

	//添加开房信息
	DBR_GR_UpdateTableInfo TableInfo;
	TableInfo.byCost =  static_cast<BYTE>(pCfg->lSinglePayCost);
	TableInfo.byIsStart = 0;
	TableInfo.byMaxPlayerCount = 5;
	TableInfo.byMode = pCfg->GameMode;
	TableInfo.byPlayerCount = 0;
	TableInfo.byZhuangType = pCfg->RobBankType;
	TableInfo.dwCreaterID = pIServerUserItem->GetUserID();
	TableInfo.dwPassword = dwPassword;
	TableInfo.dwTableID = pCurTableFrame->GetTableID();
	TableInfo.wJuShu = pCfg->GameCount;

	//创建房间时间
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);
	_sntprintf_s(TableInfo.szTime, sizeof(TableInfo.szTime), TEXT("%04d-%02d-%02d %02d:%02d:%02d"), SystemTime.wYear, SystemTime.wMonth, SystemTime.wDay, SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond);

	//添加房间信息进数据库  TODO 流程没有DBO函数，看看是否需要修改
	AddOtherRoomInfo(&TableInfo);


	//发送创建房间成功消息
	g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_GC_USER_CREATE_ROOM_SUCCESS, NULL, 0);	

	//替他人开房，首先扣除创建桌子用户的房卡 替他人开房只支持房卡模式
	OnEventModifyUserTreasure(pIServerUserItem, dwPassword ,  TABLE_MODE_FK, 0,  -pCfg->lSinglePayCost, 0);
}

//处理正常开房流程
bool CHandleFromGate::HandleCreateTable(CTableFrame *pCurTableFrame, CPlayer *pIServerUserItem, tagTableRule *pCfg)
{
	//校验
	if ( NULL == pCurTableFrame || 
		 NULL == pIServerUserItem ||
		 NULL == pCfg)
	{
		return false;
	}

	//生成桌子密码，房间号 = 前面 + TABLEID
	srand(static_cast<unsigned int >(time(NULL)));
	DWORD dwPassword = GenerateTablePassword();

	CLog::Log(log_debug, "PassWord: %d", dwPassword);

	//设置桌子属性
	pCurTableFrame->SetTableOwner(pIServerUserItem->GetUserID());	//设置桌主
	pCurTableFrame->SetTableID(dwPassword);

	//设置房间自动解散，默认一分钟 -- 这里是指不开始游戏 自动一分钟后解散
	pCurTableFrame->SetTableAutoDismiss();
	
	//获得空椅子
	WORD wChairID = pCurTableFrame->GetNullChairID(); 

	//椅子号有效
	if (INVALID_CHAIR != wChairID)
	{
		//用户坐下		TODO 后面让客户端主动发送
		if(pCurTableFrame->PlayerSitTable(wChairID, pIServerUserItem, 0, true) != 0)
		{
			SendRequestFailure(pIServerUserItem, TEXT("用户坐下失败!"), REQUEST_FAILURE_NORMAL);
			return false;
		}		
	}
	else
	{
		SendRequestFailure(pIServerUserItem, TEXT("房间已满,无法加入房间!"), REQUEST_FAILURE_NORMAL);
		return false;
	}

	return true;
}

//检查加入桌子门票
bool CHandleFromGate::CheckJoinTableTicket(tagTableRule *pCfg, CPlayer *pIServerUserItem)
{
	//内部调用, 不校验指针

	switch( pCfg->GameMode )
	{
	case TABLE_MODE_FK://房卡模式
		{
			//大厅的房卡场：AA支付，且用户房卡不足
			if( (1 == pCfg->cbPayType) &&
				(0 == pCfg->byClubCreate) &&
				(pIServerUserItem->GetUserRoomCard() < pCfg->GameCountType) )
			{
				SendRequestFailure(pIServerUserItem, TEXT("您正在进入AA支付的房卡房, 因房卡不足,无法加入"), REQUEST_FAILURE_NORMAL);
				return false;
			}
			break;
		}
	case TABLE_MODE_JJC://竞技场模式
		{
			break;
		}
	case TABLE_MODE_GOLD://金币模式
		{
			//大厅的金币场
			if(  pIServerUserItem->GetUserGold() < pCfg->dwLevelGold)
			{
				SendRequestFailure(pIServerUserItem, TEXT("您正在进入金币房,因金币不足,无法加入"), REQUEST_FAILURE_NORMAL);
				return false;
			}
			break;
		}
	case TABLE_MODE_FK_GOLD://房卡金币场
		{
			//大厅的房卡金币场  校验一:  AA支付，且用户房卡不足
			if( (1 == pCfg->cbPayType) && 
				(0 == pCfg->byClubCreate) &&
				(pIServerUserItem->GetUserRoomCard() < pCfg->GameCountType) )
			{
				SendRequestFailure(pIServerUserItem, TEXT("您正在进入的为AA支付的房卡金币房, 因房卡不足,无法加入"), REQUEST_FAILURE_NORMAL);
				return false;
			}

			//俱乐部的房卡金币场 或者 大厅的房卡金币场
			if( pIServerUserItem->GetUserGold()  < pCfg->dwLevelGold)
			{
				SendRequestFailure(pIServerUserItem, TEXT("您正在进入房卡金币房, 因金币不足,无法加入"), REQUEST_FAILURE_NORMAL);
				return false;
			}
			break;
		}
	}

	return true;
}

//检查创建桌子门票
bool CHandleFromGate::CheckCreateTableTicket(tagTableRule *pCfg, CPlayer *pIServerUserItem)
{
	//内部调用, 不校验指针
	//仅 大厅 使用, 俱乐部不使用该函数

	switch( pCfg->GameMode )
	{
	case TABLE_MODE_FK://房卡模式
		{
			//校验用户房卡: 房主支付，且用户房卡不足
			if(0 == pCfg->cbPayType && 
				(pIServerUserItem->GetUserRoomCard() < ((pCfg->GameCountType) * (pCfg->PlayerCount)) ))	  
			{
				SendRequestFailure(pIServerUserItem, TEXT("您正在创建房主支付的房卡房, 因房卡不足,无法创建"), REQUEST_FAILURE_NORMAL);
				return false;
			}

			//校验用户房卡：AA支付，且用户房卡不足
			if( (1 == pCfg->cbPayType) && 
				(pIServerUserItem->GetUserRoomCard() < pCfg->GameCountType) )
			{
				SendRequestFailure(pIServerUserItem, TEXT("您正在创建AA支付的房卡房, 因房卡不足,无法创建"), REQUEST_FAILURE_NORMAL);
				return false;
			}
			break;
		}
	case TABLE_MODE_JJC://竞技场模式
		{
			break;
		}
	case TABLE_MODE_GOLD://金币模式
		{
			break;
		}
	case TABLE_MODE_FK_GOLD://房卡金币场
		{
			//校验用户房卡: 房主支付，且用户房卡不足
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

			//大厅的房卡金币场
			if( pIServerUserItem->GetUserGold()  < pCfg->dwLevelGold)
			{
				SendRequestFailure(pIServerUserItem, TEXT("您正在创建房卡金币房, 因金币不足,无法创建"), REQUEST_FAILURE_NORMAL);
				return false;
			}
			break;
		}
	}

	return true;

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
