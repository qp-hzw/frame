#include "Stdafx.h"
#include "TableFrame.h"
#include "DataBasePacket.h"
#include "GameCtrl.h"
#include <iostream>
#include "TableManager.h"
#include "RoomRuleManager.h"
//#include <algorithm>


//////////////////////////////////////////////////////////////////////////////////
//校验GPS距离
#define CHECK_USER_GPS_DISTANCE		200									//玩家GPS在多少距离内提示玩家

//解散等待
#define IDI_ROOM_AUTO_DISMISS		(TIME_TABLE_SINK_RANGE+3)			//房间自动解散定时器
#define TIME_ROOM_AUTO_DISMISS		300000L								//房间自动解散定时器时间

//续费等待
#define IDI_WAIT_CONTINUE			(TIME_TABLE_SINK_RANGE+4)			//等待续费标识
#define TIME_WAIT_CONTINUE			20000L								//等待续费时间

//表决解散房间
#define IDI_VOTE_DISMISS_ROOM		(TIME_TABLE_SINK_RANGE+6)			//表决解散房间
#define TIME_VOTE_DISMISS_ROOM		300000L								//事件 --5分钟 

//门票不足以继续游戏时候. 解散房间
#define IDI_CHECK_DISMISS_ROOM		(TIME_TABLE_SINK_RANGE+7)			//表决解散房间
#define TIME_CHECK_DISMISS_ROOM		4*1000L								//事件 --4秒钟 

//玩家自动准备
#define IDI_PLAYER_AUTO_READY		(TIME_TABLE_SINK_RANGE+8)			//玩家自动准备
#define TIME_PLAYER_AUTO_READY		3*1000L								//四秒钟

//开始下一阶段比赛
#define IDI_MATCH_NEXT_STAGE_START	(TIME_TABLE_SINK_RANGE+9)			//开始比赛下一阶段
#define TIME_MATCH_NEXT_STAGE_START	10*1000L							//10S

/////////////////////////////////////////////////////////////////////////////////

enum PLAYER_OP_ERRORCODE
{
	PLAYER_NOT_EXISIT = 1, //玩家不存在
	TABLE_FULL = 2,        //桌子已满
	TABLE_NOT_THIS = 3,    //不在同一个桌子上

	CHAIR_USERD=4,         //椅子上已经有人
	CHAIR_INVALID=5,       //椅子号不正确
	STATUS_ERR = 6,			//没坐下就要准备
	
	TICKET_NOT = 7,        //门票检测没通过
};

/////////////////////////////////////////////////////////////////////////////////
//构造函数
CTableFrame::CTableFrame()
{
	//子游戏
	m_pITableFrameSink= g_GameCtrl->GetITableFrameSink();
	/******************** 静态属性 **********************/
	ZeroMemory(&m_tagClubRoomRule, sizeof(m_tagClubRoomRule));
	m_wTableID=0;
	m_wChairCount=0;

	m_dwTableOwner=0L;

	/******************** 动态属性 **********************/
	m_cbGameStatus=GAME_STATUS_FREE;
	m_wCurGameRound=0;
	m_OnlyID = "";

	m_user_list.clear();
	m_player_list.clear();
	m_total_score.clear();

	//解散信息
	ZeroMemory(m_bResponseDismiss,sizeof(m_bResponseDismiss));
	ZeroMemory(m_bAgree,sizeof(m_bAgree));
	m_bUnderDissState = false;
	m_dissmisserChaiID = 0xFF;

	return;
}

//析构函数
CTableFrame::~CTableFrame()
{
	//释放对象
	if ( NULL != m_pITableFrameSink )
	{
		g_GameCtrl->FreeITableFrameSink(m_pITableFrameSink);
		m_pITableFrameSink = NULL;
	}
	return;
}

//拷贝构造
CTableFrame::CTableFrame(const CTableFrame& table)
{
	m_pITableFrameSink = table.m_pITableFrameSink; //浅拷贝
	m_wTableID = table.m_wTableID;
	m_wChairCount = table.m_wChairCount;
	memcpy(&m_tagTableRule, &table.m_tagTableRule, sizeof(tagTableRule));
	memcpy(&m_tagClubRoomRule, &table.m_tagClubRoomRule, sizeof(tagClubRoomRule));
	m_dwTableOwner = table.m_dwTableOwner;
	m_wCurGameRound = table.m_wCurGameRound;
	m_cbGameStatus = table.m_cbGameStatus;
	m_user_list = table.m_user_list;
	m_player_list = table.m_player_list;
	m_total_score = table.m_total_score;
	memcpy(m_bAgree, table.m_bAgree, sizeof(MAX_CHAIR));
	memcpy(m_bResponseDismiss, table.m_bResponseDismiss, sizeof(MAX_CHAIR));;
	m_bUnderDissState = table.m_bUnderDissState;
	m_dissmisserChaiID = table.m_dissmisserChaiID;
	memcpy(&m_GameScoreRecordActive, &table.m_GameScoreRecordActive,sizeof(CGameScoreRecordArray));
}

/*************************************** 桌子属性设置与获取 ***************************************************/
//设置房间自动解散时间
void CTableFrame::SetTableAutoDismiss(DWORD dwMinutes) 
{ 
	SetGameTimer(IDI_ROOM_AUTO_DISMISS, dwMinutes*TIME_ROOM_AUTO_DISMISS, 1, NULL); 
}

//检测房间
void CTableFrame::CheckRoomTruePlayer()
{
	bool flag = true;
	for (auto it = m_player_list.begin(); it != m_player_list.end(); it++)
	{
		if (((*it) != NULL) && (!(*it)->IsAndroidUser()))
			flag = false;
	}

	if (flag == true)
	{
		HandleDJGameEnd(GAME_CONCLUDE_NORMAL);
	}
}

//是否是机器人
bool CTableFrame::IsRobot(WORD wChairID)
{
	if (wChairID > m_wChairCount)
		return false;

	CPlayer *player = m_player_list[wChairID];
	if (player == NULL)
		return false;

	return player->IsAndroidUser();
}
//房间是否满了
bool CTableFrame::IsRoomFull()
{
	WORD wSitcount = 0;
	for (auto player : m_player_list)
	{
		if (NULL != player) 
			wSitcount++;
	}

	return m_wChairCount == wSitcount;
}

//设置表决解散房间定时器
void CTableFrame::SetVoteDismissRoom()
{
	SetGameTimer(IDI_VOTE_DISMISS_ROOM, TIME_VOTE_DISMISS_ROOM, 1, NULL);
}
//取消表决解散房间定时器
void CTableFrame::KillVoteDismissRoom()
{
	KillGameTimer(IDI_VOTE_DISMISS_ROOM);
}

//设置玩家自动准备定时器
void CTableFrame::SetPlayerAutoReady()
{
	//比赛场校验
	if (GetTableMode() == TABLE_MODE_MATCH)
		SetGameTimer(IDI_PLAYER_AUTO_READY, TIME_PLAYER_AUTO_READY, 1, NULL);
}
//设置开始下一阶段定时器
void CTableFrame::SetStageTimer()
{
	SetGameTimer(IDI_MATCH_NEXT_STAGE_START, TIME_MATCH_NEXT_STAGE_START, 1, NULL);
}

/***************************************   游戏流程函数    ***************************************************/
//开始游戏
bool CTableFrame::StartGame()
{
	//状态校验
	if(GetGameStatus() != GAME_STATUS_FREE) return false;

	//设置游戏状态为开始
	SetGameStatus(GAME_STATUS_PLAY);

	//删除自动解散定时器
	KillGameTimer(IDI_ROOM_AUTO_DISMISS);

	//player状态
	for(size_t i=0; i< m_player_list.size(); i++)
	{
		if(!m_player_list.at(i)) continue;
		m_player_list.at(i)->SetUserStatus(US_PLAYING,m_wTableID,i);
	}

	//录像初始化
	m_Record.Init();

	//录像玩家信息
	m_Record.AddPlayer(m_player_list);

	/* 通知数据库, 桌子开始 */      
	tagTableRule *pTableCfg = (tagTableRule*)GetCustomRule();
	BYTE byClubOrHalGold = 0; //1表示俱乐部房间(房卡或者房卡金币模式);  2表示大厅的金币模式;  其他字段无须处理
	BYTE GameMode = pTableCfg->GameMode;

	//TODONOW 需要增加这个功能
	//g_AttemperEngineSink->ClubTableStart(GetTableID(), byClubOrHalGold);

	//通知事件
	if (m_pITableFrameSink!=NULL) 
	{
		m_pITableFrameSink->OnEventGameStart();
	}

	return true;
}

//小局结束处理函数
bool CTableFrame::HandleXJGameEnd(BYTE byRound, WORD *wIdentity, SCORE *lUserTreasure)
{
	m_wCurGameRound = byRound;
	DWORD dwDataSize = 0;
	VOID *pData = m_Record.GetData(m_tagTableRule.PlayerCount, dwDataSize);
	if (m_OnlyID.empty())
	{
		m_OnlyID = std::to_string(time(0)) + std::to_string(m_wTableID);
	}

#pragma region 用户信息
	//扣除用户门票
	if(byRound == 1)
	{
		XJTickets();
	}

	//玩家状态
	for (int i = 0; i < m_player_list.size(); i++)
	{
		if (m_player_list[i] == NULL)	continue;
		m_player_list[i]->SetUserStatus(US_SIT, GetTableID(), i);
	}

	//记录分数 --比赛
	for (int i = 0; i < m_total_score.size(); i++)
	{
		m_total_score[i] += lUserTreasure[i];
	}

	//更新用户财富  -- 用户财富变更记录表
	XJModifyUserTreasure(lUserTreasure);

	//更新玩家输赢情况表 && 经验值
	int i =0;
	for(auto player : m_player_list)
	{
		player ->ModifyPlayerScore(byRound, wIdentity[i], lUserTreasure[i], m_OnlyID);
		player->ModifyPlayerExp();
		i++;
	}

	//更新用户任务状态
	//XJUpdateGameTaskStatus(byTableMode, byRound); //TODONOW 暂时只为房卡的
#pragma endregion

#pragma region 桌子信息
	//设置桌子状态
	SetGameStatus(GAME_STATUS_FREE);

	//更新桌子战绩
	XJUpdateTableRecord(byRound, m_OnlyID);

	//更新桌子录像
	XJUpdateTableVideo(byRound, m_OnlyID, pData, dwDataSize);
#pragma endregion
	return true;
}

//大局结束处理函数
bool CTableFrame::HandleDJGameEnd(BYTE cbGameStatus)
{
	//关闭所有定时器
	KillGameTimer(IDI_ROOM_AUTO_DISMISS);

	if (!m_OnlyID.empty())
	{
		//更新玩家输赢情况表
		int i =0;
		for(auto player : m_player_list)
		{
			if (player)
			{
				player ->ModifyPlayerScore(0, 0, m_total_score[i], m_OnlyID);
				i++;
			}
		}

		//更新桌子战绩
		XJUpdateTableRecord(0, m_OnlyID);
	}

	//结束等待续费，玩家需要从准备状态退出为坐下
	if (cbGameStatus == GAME_CONCLUDE_CONTINUE)
	{
		m_cbGameStatus=GAME_STATUS_FREE;

		ZeroMemory(m_bAgree,sizeof(m_bAgree));
		//设置房间不处于解散状态
		m_bUnderDissState = false;
		m_dissmisserChaiID = 0xFF;

		//状态切换为坐下，可以自由离开
		for(auto player : m_player_list)
		{
			player->SetUserStatus(US_SIT,m_wTableID,player->GetChairID());
		}

		//设置续费定时器
		SetGameTimer(IDI_WAIT_CONTINUE,TIME_WAIT_CONTINUE,1,NULL);
		return true;
	}

	//直接解散玩家全部站起
	if(cbGameStatus == GAME_CONCLUDE_NORMAL)
	{
		for(auto player : m_player_list)
		{
			PlayerLeaveTable(player);
		}

		CTableManager::DeleteTable(this);
		return true;
	}

	return true;
}


/*************************************   游戏流程辅助函数    *************************************************/
//每局游戏结束后，检测并扣除用户门票  
bool CTableFrame::XJTickets()
{
	switch(m_tagTableRule.GameMode)
	{
	case TABLE_MODE_FK:
		{
			if(m_tagTableRule.cbPayType == 0)//房主支付
			{
				int CountType = RoomRuleManager::GetCountType(m_tagTableRule.GameCount);
				SCORE cost = CountType * m_tagTableRule.PlayerCount;
				CPlayerManager::FindPlayerByID(m_dwTableOwner)->ModifyUserTreasure(TREASURE_FK, -cost, "房卡场房费");
			}
			else if(m_tagTableRule.cbPayType == 1) //AA支付
			{
				int CountType = RoomRuleManager::GetCountType(m_tagTableRule.GameCount);
				for(auto player : m_player_list)
				{
					player->ModifyUserTreasure(TREASURE_FK, -CountType, "房卡场房费");
				}
			}

			break;
		}
	case TABLE_MODE_MATCH:
		{
			break;
		}
	case TABLE_MODE_GOLD:
		{
			break;
		}
	case TABLE_MODE_FK_GOLD:
		{
			break;
		}
	case TABLE_MODE_CLUB:
		{
			break;
		}
	}

	return true;
}
//每局游戏结束后，更新用户财富信息
bool CTableFrame::XJModifyUserTreasure(SCORE *lGameScore)
{
	BYTE byTreasureType = 0;  //1房卡, 2金币, 3钻石, 4积分
	switch(m_tagTableRule.GameMode)
	{
	case TABLE_MODE_FK:
		{
			byTreasureType = TREASURE_JF;
			break;
		}
	case TABLE_MODE_MATCH:
		{
			break;
		}
	case TABLE_MODE_GOLD:
		{
			byTreasureType = TREASURE_GOLD;
			break;
		}
	case TABLE_MODE_FK_GOLD:
		{
			byTreasureType = TREASURE_GOLD;
			break;
		}
	case TABLE_MODE_CLUB:
		{
			byTreasureType = TREASURE_JF;
			break;
		}
	}

	int i =0;
	for(auto player : m_player_list)
	{
		player->ModifyUserTreasure(byTreasureType, lGameScore[i], "小局结算");
		i++;
	}
	return true;
}
//每局游戏结束后，更新游戏任务状态
bool CTableFrame::XJUpdateGameTaskStatus(const BYTE &cbTableMode, const BYTE &cbCurGameCount)
{
	//房间规则
	tagTableRule *pCfg = (tagTableRule *)GetCustomRule();	

	//校验桌子规则
	if ( pCfg->GameMode != cbTableMode )
		return false;

	//遍历桌子所有用户
	for (int i = 0; i < m_wChairCount; i++)
	{
		//获取用户
		CPlayer *pIServerUserItem = GetTableUserItem(i);

		//校验用户
		if(pIServerUserItem == NULL )
			continue;

		//构造DBR数据
		STR_DBR_CG_TABLE_UPDATE_TASK_STATUS TaskStatus;
		ZeroMemory(&TaskStatus, sizeof(STR_DBR_CG_TABLE_UPDATE_TASK_STATUS));

		//赋值
		TaskStatus.dwUserID = pIServerUserItem->GetUserID();
		TaskStatus.cbTableMode = cbTableMode;
		TaskStatus.cbCurGameCount = cbCurGameCount;

		//发送数据库请求,无法获得用户的socketID，发0
		g_GameCtrl->PostDataBaseRequest(DBR_SC_TABLE_UPDATE_TASK_STATUS, 0, &TaskStatus, sizeof(STR_DBR_CG_TABLE_UPDATE_TASK_STATUS));

	}

	return true;
}

//每局游戏结束后, 更新桌子战绩表
bool CTableFrame::XJUpdateTableRecord(BYTE round, string OnlyID)
{
	//输入参数
	g_TreasureDB->ResetParameter();
	g_TreasureDB->AddParameter(TEXT("@TableID"), m_wTableID);
	g_TreasureDB->AddParameter(TEXT("@GameMode"),m_tagTableRule.GameMode);
	g_TreasureDB->AddParameter(TEXT("@AllCount"),m_tagTableRule.GameCount);
	g_TreasureDB->AddParameter(TEXT("@CurCount"),round);
	g_TreasureDB->AddParameter(TEXT("@PlayerCount"),m_tagTableRule.PlayerCount);
	g_TreasureDB->AddParameter(TEXT("@KindID"),g_GameCtrl->GetKindID());
	g_TreasureDB->AddParameter(TEXT("@OnlyID"), OnlyID.c_str());

	//执行查询
	LONG lResultCode = g_TreasureDB->ExecuteProcess(TEXT("GSP_TableRecord_Insert"), true);

	return true;
}
//每局游戏结束后, 更新桌子录像表
bool CTableFrame::XJUpdateTableVideo(BYTE round, string OnlyID, VOID* pData, DWORD dwDataSize)
{
	BYTE Data[LEN_MAX_RECORD_SIZE];	//录像数据
	memcpy_s(Data, dwDataSize*sizeof(BYTE), pData, dwDataSize*sizeof(BYTE));

	if(dwDataSize>LEN_MAX_RECORD_SIZE)
	{
		dwDataSize = LEN_MAX_RECORD_SIZE;
	}
	
	//输入参数
	g_TreasureDB->ResetParameter();
	g_TreasureDB->AddParameter(TEXT("@TableID"), m_wTableID);
	g_TreasureDB->AddParameter(TEXT("@CurCount"),round);
	
	std::string szData = CWConvert::toHexString3(Data, LEN_MAX_RECORD_SIZE);
	g_TreasureDB->AddParameter(TEXT("@VideoData"),szData.c_str());
	g_TreasureDB->AddParameter(TEXT("@VideoSize"),dwDataSize);
	g_TreasureDB->AddParameter(TEXT("@OnlyID"), OnlyID.c_str());

	//执行查询
	LONG lResultCode = g_TreasureDB->ExecuteProcess(TEXT("GSP_TableVideo_Insert"), true);

	return true;
}

/****************************************    玩家动作     ***************************************************/
//玩家加入
int CTableFrame::PlayerEnterTable(CPlayer * pPlayer)
{
	//1. 校验
	int ret = CanPlayerEnterTable(pPlayer);
	if(ret != 0) return ret;

	//2. Player
	pPlayer->SetUserStatus(US_IN_TABLE, m_wTableID, INVALID_CHAIR);

	//3. Sendto Client
	CMD_GF_GameStatus GameStatus;
	ZeroMemory(&GameStatus, sizeof(GameStatus));
	GameStatus.cbUserAction = pPlayer->GetUserStatus();
	tagUserInfo *pUserInfo = pPlayer->GetUserInfo();
	CopyMemory(&GameStatus.UserInfo, pUserInfo, sizeof(tagUserInfo));

	//广播发送
	SendTableData(INVALID_CHAIR, CMD_GR_USER_STATUS, &GameStatus, sizeof(GameStatus), MDM_G_FRAME);

	//4. 增加
	m_user_list.push_back(pPlayer);

	return 0;
}
//玩家坐下
int CTableFrame::PlayerSitTable(CPlayer * pPlayer, WORD wChairID, bool bCheckUserGPS)
{
	//1. 校验
	int ret = CanPlayerSitTable(pPlayer, wChairID);
	if (ret !=0) return ret;

	//2. Table
	m_player_list[wChairID] = pPlayer;

	bool bflag = false;
	for (auto user : m_user_list)
	{
		if (user && user == pPlayer)
		{
			bflag = true;
			break;
		}
	}
	if (bflag == false)
		m_user_list.push_back(pPlayer);

	//3. Player
	pPlayer->SetUserStatus(US_SIT, m_wTableID, wChairID);

	//4. 给client发送坐下
	CMD_GF_GameStatus GameStatus;
	ZeroMemory(&GameStatus, sizeof(GameStatus));

	//赋值
	GameStatus.cbUserAction = pPlayer->GetUserStatus();
	tagUserInfo *pUserInfo = pPlayer->GetUserInfo();
	CopyMemory(&GameStatus.UserInfo, pUserInfo, sizeof(tagUserInfo));

	//广播发送
	SendTableData(INVALID_CHAIR, CMD_GR_USER_STATUS, &GameStatus, sizeof(GameStatus), MDM_G_FRAME);

	return 0;
}
//玩家站起
bool CTableFrame::PlayerUpTable(CPlayer *pPlayer)
{
	//1, 校验
	int ret = CanPlayerUpTable(pPlayer);
	if(ret !=0 ) return false;

	//2. Table
	WORD wChairID = pPlayer->GetChairID();
	m_player_list.at(wChairID) = NULL;

	//3. Player
	pPlayer->SetUserStatus(US_IN_TABLE, m_wTableID, INVALID_CHAIR);

	//发送给客户端
	CMD_GF_GameStatus GameStatus;
	ZeroMemory(&GameStatus, sizeof(GameStatus));

	//赋值
	GameStatus.cbUserAction = pPlayer->GetUserStatus();
	tagUserInfo *pUserInfo = pPlayer->GetUserInfo();
	CopyMemory(&GameStatus.UserInfo, pUserInfo, sizeof(tagUserInfo));

	//广播发送
	SendTableData(INVALID_CHAIR, CMD_GR_USER_STATUS, &GameStatus, sizeof(GameStatus), MDM_G_FRAME);

	return true;
}
//玩家离开
int CTableFrame::PlayerLeaveTable(CPlayer* pPlayer)
{
	//1. 校验
	int ret = CanPlayerLeaveTable(pPlayer);
	if(ret!=0) return ret;

	//发送给客户端
	CMD_GF_GameStatus GameStatus;
	ZeroMemory(&GameStatus, sizeof(GameStatus));

	//赋值
	GameStatus.cbUserAction = US_LEAVE;
	tagUserInfo *pUserInfo = pPlayer->GetUserInfo();
	CopyMemory(&GameStatus.UserInfo, pUserInfo, sizeof(tagUserInfo));

	//广播发送
	SendTableData(INVALID_CHAIR, CMD_GR_USER_STATUS, &GameStatus, sizeof(GameStatus), MDM_G_FRAME);

	//3. Player
	pPlayer->SetUserStatus(US_FREE, INVALID_TABLE, INVALID_CHAIR);

	//断开用户 socket (金币场结束不断)
	if ( m_tagTableRule.GameMode != TABLE_MODE_GOLD && m_tagTableRule.GameMode != TABLE_MODE_MATCH) 
		CPlayerManager::CloseSocket(pPlayer);

	//2. Table
	auto ite1 = find(m_user_list.begin(), m_user_list.end(), pPlayer);
	if (ite1 != m_user_list.end())
	{
		m_user_list.erase(ite1);
	}

	auto ite2 = find(m_player_list.begin(), m_player_list.end(), pPlayer);
	if (ite2 != m_player_list.end())
	{
		*ite2 = NULL;
	}

	return 0;
}
//玩家准备
int CTableFrame::PlayerReady(CPlayer* pPlayer) 
{
	//准备校验
	int ret = CanPlayerReady(pPlayer);
	if (ret != 0)	return ret;

	//获取参数
	DWORD	dwTableID = pPlayer->GetTableID();

	//设置用户状态为准备
	pPlayer->SetUserStatus(US_READY, dwTableID, pPlayer->GetChairID());

	//给客户端发送准备行为
	CMD_GF_GameStatus GameStatus;
	ZeroMemory(&GameStatus, sizeof(GameStatus));

	//赋值
	GameStatus.cbUserAction = pPlayer->GetUserStatus();
	tagUserInfo *pUserInfo = pPlayer->GetUserInfo();
	CopyMemory(&GameStatus.UserInfo, pUserInfo, sizeof(tagUserInfo));

	//广播发送
	SendTableData(INVALID_CHAIR, CMD_GR_USER_STATUS, &GameStatus, sizeof(GameStatus), MDM_G_FRAME);

	//判断三个玩家是否都准备
	WORD ReadyNum = 0;
	for (auto it = m_player_list.begin(); it != m_player_list.end(); it++)
	{
		if ((*it) != NULL && (US_READY == (*it)->GetUserStatus()))
			ReadyNum++;
	}

	CLog::Log(log_debug, "ReadyNum: %d", ReadyNum);

	//三个玩家准备 开始游戏	//不在子游戏处理
	if (ReadyNum >= m_tagTableRule.PlayerCount)
		StartGame();

	return 0;
}
//玩家断线
int CTableFrame::PlayerOffline(CPlayer* pPlayer) 
{
	CLog::Log(log_debug, "玩家%ld 掉线", pPlayer->GetUserID());

	//设置用户状态为断线
	pPlayer->SetUserStatus(US_OFFLINE, m_wTableID, pPlayer->GetChairID());

	//给客户端发送准备行为
	CMD_GF_GameStatus GameStatus;
	ZeroMemory(&GameStatus, sizeof(GameStatus));

	//赋值
	GameStatus.cbUserAction = pPlayer->GetUserStatus();
	tagUserInfo *pUserInfo = pPlayer->GetUserInfo();
	CopyMemory(&GameStatus.UserInfo, pUserInfo, sizeof(tagUserInfo));

	//广播发送
	SendTableData(INVALID_CHAIR, CMD_GR_USER_STATUS, &GameStatus, sizeof(GameStatus), MDM_G_FRAME);

	return 0;
}

//玩家能否加入
int CTableFrame::CanPlayerEnterTable(CPlayer* pPlayer)
{
	if (US_FREE != pPlayer->GetUserStatus())
		return -1;

	return 0;
}
//玩家能否坐下
int CTableFrame::CanPlayerSitTable(CPlayer* pPlayer, WORD &wChairID)
{
	//1. 指针校验
	if (NULL == pPlayer)
	{
		return PLAYER_NOT_EXISIT;
	}

	//2. 椅子判断
	if(wChairID == INVALID_CHAIR)
	{
		wChairID = GetNullChairID(); //client没有指定,则分配一个
	}
	if(wChairID >= m_wChairCount) return TABLE_FULL;

	//3. 判断椅子上是否有人
	CPlayer *pCurPlayer = GetTableUserItem(wChairID);		//桌子用户，肯定为NULL，用来校验用户是不是已经在桌子中
	if ( (NULL != pCurPlayer) && 
		(pPlayer->GetUserID() != pCurPlayer->GetUserID()) )
	{
		return CHAIR_USERD;
	}

	return 0;
}
//玩家能否站起
int CTableFrame::CanPlayerUpTable(CPlayer* pPlayer)
{
	//指针校验
	if(pPlayer == NULL) return PLAYER_NOT_EXISIT;

	//不在这个桌子上
	if(pPlayer->GetTableID() != m_wTableID) return TABLE_NOT_THIS;

	//椅子位置不正确 -- 旁观用户没必要起立
	if(pPlayer->GetChairID() >= m_wChairCount)  return CHAIR_INVALID;

	return 0;
}
//玩家能否离开
int CTableFrame::CanPlayerLeaveTable(CPlayer* pPlayer)
{
	//校验
	if (pPlayer == NULL)
		return 2;

	//玩家不再房间里  不能离开
	if (m_wTableID != pPlayer->GetTableID())
		return 3;

	//如果玩家在游戏中 不可离开
	if (US_PLAYING == pPlayer->GetUserStatus())
		return 1;

	return 0;
}
//玩家是否能准备
int CTableFrame::CanPlayerReady(CPlayer* pPlayer) 
{
	//校验
	if (pPlayer == NULL)  return PLAYER_NOT_EXISIT;

	//只有做下才能准备
	if (pPlayer->GetUserStatus() != US_SIT)	return STATUS_ERR;

	return 0;
}

/*************************************    待整理    *************************************************/
//游戏用户
CPlayer * CTableFrame::GetTableUserItem(WORD wChairID)
{
	//效验参数
	if (wChairID>=m_wChairCount) return NULL;

	//获取用户
	return m_player_list[wChairID];
}

//旁观用户
CPlayer * CTableFrame::EnumLookonUserItem(WORD wEnumIndex)
{
	if (wEnumIndex>=m_user_list.size()) return NULL;
	return NULL;
	//return m_user_list[wEnumIndex];
}

//设置时间
bool CTableFrame::SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter)
{
	//效验参数
	if ((dwTimerID<=0)||(dwTimerID>TIME_TABLE_MODULE_RANGE)) return false;

	//设置时间
	DWORD dwEngineTimerID=IDI_TABLE_MODULE_START + m_wTableID*TIME_TABLE_MODULE_RANGE;
	g_GameCtrl->SetTimer(dwEngineTimerID+dwTimerID,dwElapse,dwRepeat,dwBindParameter);

	return true;
}

//删除时间
bool CTableFrame::KillGameTimer(DWORD dwTimerID)
{
	//效验参数
	if ((dwTimerID<=0)||(dwTimerID>TIME_TABLE_MODULE_RANGE)) return false;

	//删除时间
	DWORD dwEngineTimerID=IDI_TABLE_MODULE_START + m_wTableID*TIME_TABLE_MODULE_RANGE;
	g_GameCtrl->KillTimer(dwEngineTimerID+dwTimerID);

	return true;
}

//获取定时器剩余毫秒数
DWORD CTableFrame::GetTimerLeftTickCount(DWORD dwTimerID)
{
	//效验参数
	if ((dwTimerID<=0)||(dwTimerID>TIME_TABLE_MODULE_RANGE)) return false;

	//删除时间
	DWORD dwEngineTimerID=IDI_TABLE_MODULE_START + m_wTableID*TIME_TABLE_MODULE_RANGE;
	return g_GameCtrl->GetTimerLeftTickCount(dwEngineTimerID+dwTimerID);
	return 0;
}

//发送数据
bool CTableFrame::SendTableData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize, WORD wMainCmdID)
{
	//数据录像
	if (wMainCmdID == MDM_GF_GAME)
	{
		m_Record.AddRecord(wChairID, wSubCmdID, pData, wDataSize);
	}

	//用户群发
	if (wChairID == INVALID_CHAIR)
	{
		for (auto it = m_user_list.begin(); it != m_user_list.end(); it++)   //这里旁观用户也会发送
		{
			if (((*it) != NULL) && (!(*it)->IsAndroidUser()))
			{
				g_GameCtrl->SendData((*it), wMainCmdID, wSubCmdID, pData, wDataSize);
			}
		}

		return true;
	}
	else
	{
		//获取用户
		CPlayer * pIServerUserItem = GetTableUserItem(wChairID);

		if (pIServerUserItem == NULL) return false;
		if (pIServerUserItem->IsAndroidUser()) return false;
		//发送数据
		g_GameCtrl->SendData(pIServerUserItem, wMainCmdID, wSubCmdID, pData, wDataSize);

		return true;
	}

	return false;
}

//发送场景 TODONOW 准备删除
bool CTableFrame::SendGameScene(WORD wChairID, VOID * pData, WORD wDataSize)
{
	//用户效验
	if(wChairID > m_player_list.size()) return false;

	g_GameCtrl->SendData(m_player_list.at(wChairID), MDM_G_FRAME,CMD_GR_FRAME_GAME_OPTION,pData,wDataSize);

	return true;
}

//发送消息
bool CTableFrame::SendGameMessage(LPCTSTR lpszMessage, WORD wType)
{
	//变量定义
	WORD wEnumIndex=0;

	//发送消息
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//获取用户
		CPlayer * pIServerUserItem=GetTableUserItem(i);
		if ((pIServerUserItem==NULL)) continue;

		//发送消息
		g_GameCtrl->SendGameMessage(pIServerUserItem,lpszMessage,wType);
	}

	//枚举用户
	do
	{
		//获取用户
		CPlayer * pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if (pIServerUserItem==NULL) break;

		//发送消息
		g_GameCtrl->SendGameMessage(pIServerUserItem,lpszMessage,wType);

	} while (true);

	return true;
}

//房间消息
bool CTableFrame::SendRoomMessage(CPlayer * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//用户效验
	if (pIServerUserItem==NULL) return false;
	//发送消息
	g_GameCtrl->SendRoomMessage(pIServerUserItem,lpszMessage,wType);

	return true;
}

//游戏消息
bool CTableFrame::SendGameMessage(CPlayer * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//用户效验
	if (pIServerUserItem==NULL) return false;

	//发送消息
	return g_GameCtrl->SendGameMessage(pIServerUserItem,lpszMessage,0);
}

//时间事件
bool CTableFrame::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//回调事件
	if ((dwTimerID>=0)&&(dwTimerID<TIME_TABLE_SINK_RANGE))
	{
		return m_pITableFrameSink->OnTimerMessage(dwTimerID,dwBindParameter);
	}

	//事件处理
	switch (dwTimerID)
	{
	case IDI_ROOM_AUTO_DISMISS:			//房间自动解散定时器
		{
			KillGameTimer(IDI_ROOM_AUTO_DISMISS);

			//结束游戏
			HandleDJGameEnd(GAME_CONCLUDE_NORMAL);
			return true;
		}
	case IDI_VOTE_DISMISS_ROOM:		//表决解散房间定时器
		{
			KillGameTimer(IDI_VOTE_DISMISS_ROOM);
			//默认同意解散
			for (int i=0; i<m_wChairCount; i++)
			{
				CPlayer *pTableUserItem = GetTableUserItem(i);
				if(pTableUserItem == NULL) continue;
				if(m_bResponseDismiss[i]) continue;
				OnEventVoteDismissRoom(i,true);
			}
			return true;
		}
	case IDI_CHECK_DISMISS_ROOM:	//门票不足以继续游戏时候， 解散桌子
		{
			KillGameTimer(IDI_CHECK_DISMISS_ROOM);

#pragma region 解散房间
			//设置房间处于解散状态
			SetDismissState(true);

			//通知客户端弹出大局结算面板
			STR_CMD_GR_FRAME_DISMISS_RESULT DismissResult;
			ZeroMemory(&DismissResult, sizeof(DismissResult));
			DismissResult.cbDismiss = 1;
			
			SendTableData(INVALID_CHAIR, CMD_GR_USER_DISMISS_RESULT, &DismissResult, sizeof(STR_CMD_GR_FRAME_DISMISS_RESULT));

			//通知子游戏解散房间
			//空闲状态 直接解散
			if(GAME_STATUS_FREE == GetGameStatus())
			{
				HandleDJGameEnd(GAME_CONCLUDE_NORMAL);
			}
			//游戏状态则 通知子游戏
			else
			{
				OnEventClubDismissRoom();
			}
			return true;
#pragma endregion
		}
	case IDI_PLAYER_AUTO_READY:		//玩家自动准备
		{
			KillGameTimer(IDI_PLAYER_AUTO_READY);

			for (auto player : m_player_list)
			{
				if (!player) continue;

				PlayerReady(player);
			}

			return true;
		}
	case IDI_MATCH_NEXT_STAGE_START:	//开始比赛下一阶段
		{
			KillGameTimer(IDI_MATCH_NEXT_STAGE_START);

			StartNextStage();

			return true;
		}
	case IDI_WAIT_CONTINUE:
		{
			KillGameTimer(IDI_WAIT_CONTINUE);

			HandleDJGameEnd(0xFE);

			return true;
		}
	}

	//错误断言
	ASSERT(FALSE);

	return false;
}

//游戏事件
bool CTableFrame::OnEventSocketGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, CPlayer * pIServerUserItem)
{
	//效验参数
	bool ret = m_pITableFrameSink->OnGameMessage(wSubCmdID,pData,wDataSize,GetPlayerChair(pIServerUserItem));
	if(!ret)
	{
	}

	//消息处理
	return ret;
}

//框架事件
bool CTableFrame::OnEventSocketFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, CPlayer * pIServerUserItem)
{
	//游戏处理 此处处理的应该是只有200的主消息号 TODONOW 细看	崩溃
	//if (m_pITableFrameSink->OnFrameMessage(wSubCmdID,pData,wDataSize,GetPlayerChair(pIServerUserItem))==true) return true;

	//默认处理
	switch (wSubCmdID)
	{
	case SUB_RG_ROOM_RULE:		//游戏规则
		{
			//发送房间规则
			STR_CMD_ROOM_RULE room_rule;
			memcpy(&room_rule.common, &m_tagTableRule, sizeof(room_rule.common));
			room_rule.TableID = m_wTableID;
			g_GameCtrl->SendData(pIServerUserItem, MDM_G_FRAME, CMD_GR_ROOM_RULE, &room_rule, sizeof(room_rule));

			return true;
		}
	case SUB_RG_PLAYER_INFO:		//请求用户信息
		{
			// 1、给加入玩家发送自己的数据
			WORD wChairID = pIServerUserItem->GetChairID();
			BYTE cbUserStatus = pIServerUserItem->GetUserStatus();

			//发送状态
			CMD_GF_GameStatus GameStatus1;
			GameStatus1.cbUserAction = cbUserStatus;

			tagUserInfo *pUserInfo = pIServerUserItem->GetUserInfo();
			memset(&GameStatus1.UserInfo, 0, sizeof(tagUserInfo));
			memcpy(&GameStatus1.UserInfo, pUserInfo, sizeof(tagUserInfo));
			CLog::Log(log_debug, "UserID: %d, wChairID: %d", GameStatus1.UserInfo.dwUserID, GameStatus1.UserInfo.wChairID);

			g_GameCtrl->SendData(pIServerUserItem, MDM_G_FRAME, CMD_GR_USER_STATUS, &GameStatus1, sizeof(GameStatus1));

			// 2、玩家加入房间  将在房间里的所有玩家信息发送给新玩家
			for (auto it = m_user_list.begin(); it != m_user_list.end(); it++)
			{
				if (((*it) != NULL) && (*it != pIServerUserItem))
				{
					//获取属性
					WORD wChairID = (*it)->GetChairID();
					BYTE cbUserStatus = (*it)->GetUserStatus();

					//发送状态
					CMD_GF_GameStatus GameStatus;
					GameStatus.cbUserAction = cbUserStatus;

					tagUserInfo *pUserInfo = (*it)->GetUserInfo();
					memset(&GameStatus.UserInfo, 0, sizeof(tagUserInfo));
					memcpy(&GameStatus.UserInfo, pUserInfo, sizeof(tagUserInfo));

					CLog::Log(log_debug, "UserID: %d, wChairID: %d", GameStatus.UserInfo.dwUserID, GameStatus.UserInfo.wChairID);

					g_GameCtrl->SendData(pIServerUserItem, MDM_G_FRAME, CMD_GR_USER_STATUS, &GameStatus, sizeof(GameStatus));
				}
			}

			// 3、给Client发送 用户信息发送完成的消息
			g_GameCtrl->SendData(pIServerUserItem, MDM_G_FRAME, CMD_GR_USER_STATUS_FINISH, NULL, 0);

			return true;
		}
	case SUB_RG_FRAME_GAME_OPTION:		//请求游戏场景
		{
			//变量定义
			WORD wChairID = pIServerUserItem->GetChairID();

			//发送场景
			m_pITableFrameSink->OnEventSendGameScene(wChairID);

			
			//发送解散面板状态 -- 只有处于解散状态 才会发送
			if(m_bUnderDissState)
			{
				STR_CMD_GR_FRAME_GAME_DISSMISS cmd_dismiss;
				ZeroMemory(&cmd_dismiss, sizeof(cmd_dismiss));

				BYTE num = 0;

				//发起者 放到第一位
				if (m_dissmisserChaiID != 0xFF)
				{
					CPlayer *pTableUserItem = GetTableUserItem(m_dissmisserChaiID);
					if(pTableUserItem != NULL)
					{
						cmd_dismiss.dwUserID[0] = pTableUserItem->GetUserID();

						if(!m_bResponseDismiss[m_dissmisserChaiID]) //未表决
						{
							cmd_dismiss.cbAgree[0] = 2;
						}
						else if(m_bAgree[m_dissmisserChaiID]) //同意
						{
							cmd_dismiss.cbAgree[0] = 1;
						}
						else
						{
							cmd_dismiss.cbAgree[0] = 0;
						}

						num ++;	
					}
				}

				for(int i=0; i<m_wChairCount && num <=m_wChairCount; i++)
				{
					if(i == m_dissmisserChaiID) continue;

					CPlayer *pTableUserItem = GetTableUserItem(i);
					if(pTableUserItem == NULL) continue;

					cmd_dismiss.dwUserID[num] = pTableUserItem->GetUserID();

					if(!m_bResponseDismiss[i]) //未表决
					{
						cmd_dismiss.cbAgree[num] = 2;
					}
					else if(m_bAgree[i]) //同意
					{
						cmd_dismiss.cbAgree[num] = 1;
					}
					else
					{
						cmd_dismiss.cbAgree[num] = 0;
					}

					num++;
				}

				g_GameCtrl->SendData(pIServerUserItem,MDM_G_FRAME,CMD_GR_FRAME_GAME_DISSMISS,&cmd_dismiss,sizeof(cmd_dismiss));
			}

			return true;
		}
	case SUB_RG_FRAME_CHAT:		//用户聊天
		{
			//效验参数
			ASSERT(wDataSize==sizeof(STR_SUB_RG_FRAME_CHAT));
			if (wDataSize != sizeof(STR_SUB_RG_FRAME_CHAT)) 
				return false;

			//变量定义
			STR_SUB_RG_FRAME_CHAT *pTableChat = (STR_SUB_RG_FRAME_CHAT *)pData;

			//获取用户
			CPlayer *pIServerUserItem = CPlayerManager::FindPlayerByID(pTableChat->dwUserID);
			if ( NULL == pIServerUserItem)
				return true;

			//构造返回数据
			STR_CMD_GR_FRAME_CHAT CMDChat;
			ZeroMemory(&CMDChat, sizeof(STR_CMD_GR_FRAME_CHAT));

			//赋值
			CMDChat.dwChairID = pIServerUserItem->GetChairID();
			CMDChat.dwMsgID = pTableChat->dwMsgID;
			CMDChat.dwUserID = pTableChat->dwUserID;

			//发送数据
			SendTableData(INVALID_CHAIR, CMD_GR_FRAME_CHAT, &CMDChat, sizeof(STR_CMD_GR_FRAME_CHAT));

			return true;
		}	
	case SUB_CG_EFFECT: //道具消耗
		{
			//效验参数
			if (wDataSize != sizeof(STR_SUB_CG_EFFECT)) 
				return false;

			//变量定义
			STR_SUB_CG_EFFECT *pSub = (STR_SUB_CG_EFFECT *)pData;

			//道具检测用户 -- 使用物品失败
			if( !(pIServerUserItem-> UseProp(pSub->dwGoodsID)) )
			{
				g_GameCtrl->SendData(pIServerUserItem, MDM_G_FRAME, CMD_GC_EFFECT_RESULT, NULL, 0);
				return true;
			}

			//使用成功, 广播给所有人
			STR_CMD_GC_EFFECT_BRODCAST cmd;
			ZeroMemory(&cmd, sizeof(cmd));

			//赋值
			cmd.from_chair =  pIServerUserItem->GetChairID();
			cmd.target_chair = pSub->target_chair;
			cmd.dwGoodsID = pSub->dwGoodsID;
			SendTableData(INVALID_CHAIR, CMD_GC_EFFECT_BRODCAST, &cmd, sizeof(cmd), MDM_G_FRAME);
			return true;
		}
	case SUB_GF_LOOKON_CONFIG:		//旁观配置
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_GF_LookonConfig));
			if (wDataSize<sizeof(CMD_GF_LookonConfig)) return false;

			//变量定义
			CMD_GF_LookonConfig * pLookonConfig=(CMD_GF_LookonConfig *)pData;

			//目标用户
			if ((pLookonConfig->dwUserID!=0)&&(CPlayerManager::FindPlayerByID(pLookonConfig->dwUserID)==NULL))
			{
				ASSERT(FALSE);
				return true;
			}

			//旁观处理
			if (pLookonConfig->dwUserID!=0L)
			{
				for (size_t i=0;i<m_user_list.size();i++)
				{
					//获取用户
					//CPlayer * pILookonUserItem=m_user_list[i];
					//if (pILookonUserItem->GetUserID()!=pLookonConfig->dwUserID) continue;
					//if (pILookonUserItem->GetChairID()!=pIServerUserItem->GetChairID()) continue;

					////构造消息
					//CMD_GF_LookonStatus LookonStatus;
					//LookonStatus.cbAllowLookon=pLookonConfig->cbAllowLookon;

					////发送消息
					//g_GameCtrl->SendData(pILookonUserItem,MDM_G_FRAME,CMD_GF_LOOKON_STATUS,&LookonStatus,sizeof(LookonStatus));

					break;
				}
			}
			else
			{
				//构造消息
				CMD_GF_LookonStatus LookonStatus;
				LookonStatus.cbAllowLookon=pLookonConfig->cbAllowLookon;

				//发送消息
				for (size_t i=0;i<m_user_list.size();i++)
				{
					////获取用户
					//CPlayer * pILookonUserItem=m_user_list[i];
					//if (pILookonUserItem->GetChairID()!=pIServerUserItem->GetChairID()) continue;

					////发送消息
					//g_GameCtrl->SendData(pILookonUserItem,MDM_G_FRAME,CMD_GF_LOOKON_STATUS,&LookonStatus,sizeof(LookonStatus));
				}
			}

			return true;
		}
	//case SUB_RG_FRAME_ASK_DISMISS:		//申请解散房间
	//	{
	//		//用户校验
	//		if (pIServerUserItem == NULL)
	//		return false;

	//		STR_SUB_RG_FRAME_ASK_DISMISS *pApply = (STR_SUB_RG_FRAME_ASK_DISMISS*)pData;

	//		CLog::Log(log_debug, "DISMISS UserID: %d", pIServerUserItem->GetUserID());

	//		//2. 校验房间是否已经处于解散状态
	//		if (m_bUnderDissState)
	//		{
	//			return true;
	//		}

	//		//重新点击解散按钮，需要删除定时器，防止一个人不停点击解散，定时器到自动退出房间
	//		KillGameTimer(IDI_VOTE_DISMISS_ROOM);

	//		OnEventApplyDismissRoom(pIServerUserItem->GetChairID(), true); //0为false  其他为true

	//		return true;
	//	}
	//case SUB_RG_FRAME_VOTE_DISMISS:		//表决解散房间
	//	{
	//		//数据包校验
	//		if ( wDataSize != sizeof(STR_SUB_RG_FRAME_VOTE_DISMISS) )
	//			return false;

	//		//变量定义
	//		STR_SUB_RG_FRAME_VOTE_DISMISS *pVote = (STR_SUB_RG_FRAME_VOTE_DISMISS*)pData;

	//		//解散成功
	//		OnEventVoteDismissRoom(pIServerUserItem->GetChairID(), (pVote->cbAgree != 0 ));
	//		return true;

	//	}
	case SUB_CG_COM_CHECK_USER_GPS:		//请求校验GPS
		{
			//数据包校验
			if ( wDataSize != sizeof(STR_SUB_CG_COM_CHECK_USER_GPS) )
				return false;

			//变量定义
			STR_SUB_CG_COM_CHECK_USER_GPS *pCheck = (STR_SUB_CG_COM_CHECK_USER_GPS*)pData;

			//校验
			if (1 != pCheck->cbRequestFlag)
				return true;

			//处理
			CheckUserDistance();
			return true;
		}
	case SUB_CG_COM_CHECK_USER_IP:		//请求校验IP
		{
			//数据包校验
			if ( wDataSize != sizeof(STR_SUB_CG_COM_CHECK_USER_IP) )
				return false;

			//变量定义
			STR_SUB_CG_COM_CHECK_USER_IP *pCheck = (STR_SUB_CG_COM_CHECK_USER_IP*)pData;

			//校验
			if (1 != pCheck->cbRequestFlag)
				return true;

			//处理
			CheckUserIpAddress();
			return true;
		}
	}

	return false;
}

//获取空位
WORD CTableFrame::GetNullChairID()
{
	//椅子搜索
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_player_list[i]==NULL)
		{
			return i;
		}
	}

	return INVALID_CHAIR;
}

//用户数目
WORD CTableFrame::GetSitUserCount()
{
	//变量定义
	WORD wUserCount=0;

	//数目统计
	for (WORD i=0; i<m_wChairCount; i++)
	{
		if (GetTableUserItem(i)!=NULL)
		{
			wUserCount++;
		}
	}

	return wUserCount;
}

WORD CTableFrame::GetPlayerChair(CPlayer* pPlayer)
{
	for(size_t i =0; i<m_player_list.size(); i++)
	{
		if(m_player_list.at(i) == pPlayer)
		{
			return i;
		}
	}

	return INVALID_CHAIR;
}

//桌子中是否有真实玩家
bool CTableFrame::IsTruePlayer()
{
	for (auto it = m_player_list.begin(); it != m_player_list.end(); it++)
	{
		if ((*it != NULL) && (!(*it)->IsAndroidUser()))
			return true;
	}

	return false;
}

//请求失败
bool CTableFrame::SendRequestFailure(CPlayer * pIServerUserItem, LPCTSTR pszDescribe, LONG lErrorCode)
{
	//变量定义
	CMD_GR_RequestFailure RequestFailure;
	ZeroMemory(&RequestFailure,sizeof(RequestFailure));

	//构造数据
	RequestFailure.lErrorCode=lErrorCode;
	lstrcpyn(RequestFailure.szDescribeString,pszDescribe,CountArray(RequestFailure.szDescribeString));

	//发送数据
	WORD wDataSize=CountStringBuffer(RequestFailure.szDescribeString);
	WORD wHeadSize=sizeof(RequestFailure)-sizeof(RequestFailure.szDescribeString);
	g_GameCtrl->SendData(pIServerUserItem,MDM_USER,SUB_GR_REQUEST_FAILURE,&RequestFailure,wHeadSize+wDataSize);

	std::wcout << "TF: " << pszDescribe << std::endl;

	return true;
}

//开始效验
bool CTableFrame::EfficacyStartGame(WORD wReadyChairID)
{
	TCHAR szString[512]=TEXT("");

	//准备人数
	WORD wReadyUserCount=0;
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//获取用户
		CPlayer * pITableUserItem=GetTableUserItem(i);
		if (pITableUserItem==NULL) continue;

		//用户统计
		if (pITableUserItem!=NULL)
		{
			//准备状态的玩家 数量
			if (pITableUserItem->GetUserStatus()==US_READY)
			{
				wReadyUserCount++;
			}
		}
	}

	//人数判断
	if (wReadyUserCount==m_wChairCount)
	{
		return true;
	}

	/*
	/配对开始
	{
	//数目判断
	if (wReadyUserCount==m_wChairCount) return true;
	if ((wReadyUserCount<2L)||(wReadyUserCount%2)!=0) return false;

	//位置判断
	for (WORD i=0;i<m_wChairCount;i++)
	{
	//获取用户
	CPlayer * pICurrentUserItem=GetTableUserItem(i);
	CPlayer * pITowardsUserItem=GetTableUserItem(i+m_wChairCount/2);

	//位置过滤
	if ((pICurrentUserItem==NULL)&&(pITowardsUserItem!=NULL)) return false;
	if ((pICurrentUserItem!=NULL)&&(pITowardsUserItem==NULL)) return false;
	}
	*/

	return false;
}

//地址效验
bool CTableFrame::EfficacyIPAddress(CPlayer * pIServerUserItem)
{
	//规则判断
	//	if (CServerRule::IsForfendGameRule(m_pGameServiceOption->dwServerRule)==true) return true;

	/*
	//地址效验
	const tagUserRule * *pTableUserRule=NULL;
	bool bCheckSameIP=pUserRule->bLimitSameIP;
	for (WORD i=0;i<m_wChairCount;i++)
	{
	//获取用户
	CPlayer * pITableUserItem=GetTableUserItem(i);
	if (pITableUserItem!=NULL && (!pITableUserItem->IsAndroidUser()) && (pITableUserItem->GetMasterOrder()==0))
	{
	pTableUserRule=pITableUserItem->GetUserRule();
	if (pTableUserRule->bLimitSameIP==true) 
	{
	bCheckSameIP=true;
	break;
	}
	}
	}

	//地址效验
	if (bCheckSameIP==true)
	{
	DWORD dwUserIP=pIServerUserItem->GetClientAddr();
	for (WORD i=0;i<m_wChairCount;i++)
	{
	//获取用户
	CPlayer * pITableUserItem=GetTableUserItem(i);
	if ((pITableUserItem!=NULL)&&(pITableUserItem != pIServerUserItem)&&(!pITableUserItem->IsAndroidUser())&&(pITableUserItem->GetMasterOrder()==0)&&(pITableUserItem->GetClientAddr()==dwUserIP))
	{
	if (!pUserRule->bLimitSameIP)
	{
	//发送信息
	LPCTSTR pszDescribe=TEXT("此游戏桌玩家设置了不跟相同 IP 地址的玩家游戏，您 IP 地址与此玩家的 IP 地址相同，不能加入游戏！");
	SendRequestFailure(pIServerUserItem,pszDescribe,REQUEST_FAILURE_NORMAL);
	return false;
	}
	else
	{
	//发送信息
	LPCTSTR pszDescribe=TEXT("您设置了不跟相同 IP 地址的玩家游戏，此游戏桌存在与您 IP 地址相同的玩家，不能加入游戏！");
	SendRequestFailure(pIServerUserItem,pszDescribe,REQUEST_FAILURE_NORMAL);
	return false;
	}
	}
	}
	for (WORD i=0;i<m_wChairCount;i++)
	{
	//获取用户
	CPlayer * pITableUserItem=GetTableUserItem(i);
	if (pITableUserItem!=NULL && (!pITableUserItem->IsAndroidUser()) && (pITableUserItem->GetMasterOrder()==0))
	{
	for (WORD j=i+1;j<m_wChairCount;j++)
	{
	//获取用户
	CPlayer * pITableNextUserItem=GetTableUserItem(j);
	if ((pITableNextUserItem!=NULL) && (!pITableNextUserItem->IsAndroidUser()) && (pITableNextUserItem->GetMasterOrder()==0)&&(pITableUserItem->GetClientAddr()==pITableNextUserItem->GetClientAddr()))
	{
	LPCTSTR pszDescribe=TEXT("您设置了不跟相同 IP 地址的玩家游戏，此游戏桌存在 IP 地址相同的玩家，不能加入游戏！");
	SendRequestFailure(pIServerUserItem,pszDescribe,REQUEST_FAILURE_NORMAL);
	return false;
	}
	}
	}
	}
	}
	*/
	return true;
}

//校验用户GPS距离
bool CTableFrame::CheckUserDistance()
{
	//遍历桌子上的其他用户，两两比较
	std::vector<STR_CMD_CG_COM_CHECK_USER_GPS> vecCheckGPS;

	//两两遍历
	for(int i=0; i<m_wChairCount; i++)
	{
		//获得该用户的位置信息
		CPlayer *pCheckUserItem = GetTableUserItem(i);
		if(NULL == pCheckUserItem) continue;
		double dCheckUserLong = 0.1;//pCheckUserItem->GetUserInfo()->dLongitude;
		double dCheckUserLat  = 0.1;//pCheckUserItem->GetUserInfo()->dLatitude;

		//校验位置信息
		if( (dCheckUserLong<0.01 && dCheckUserLong>-0.01) || 
			(dCheckUserLat<0.01 && dCheckUserLat>-0.01))
		{
			continue;
		}

		//计算与其他用户的距离
		for (int j = i+1; j < m_wChairCount; j++)
		{
			//校验桌子用户
			CPlayer *pTableUserItem = GetTableUserItem(j);
			if(NULL == pTableUserItem) continue;

			//获得桌子用户位置
			double dTableUserLong = 0.1;// pTableUserItem->GetUserInfo()->dLongitude;
			double dTableUserLat = 0.1;//pTableUserItem->GetUserInfo()->dLatitude;

			//校验用户位置信息
			if( (dTableUserLong<0.01 && dTableUserLong>-0.01) || 
				(dTableUserLat<0.01 && dTableUserLat>-0.01))
			{
				continue;
			}

			//比较两者距离
			double Distance = GetDistance(dCheckUserLong, dCheckUserLat, dTableUserLong, dTableUserLat);

			//发送数据至客户端
			STR_CMD_CG_COM_CHECK_USER_GPS CheckGPS;
			ZeroMemory(&CheckGPS, sizeof(STR_CMD_CG_COM_CHECK_USER_GPS));

			//赋值
			CheckGPS.fDistance = Distance;
			CheckGPS.dwFirstUserID = pCheckUserItem->GetUserID();
			CheckGPS.dwNextUserID = pTableUserItem->GetUserID();
			CheckGPS.cbIsCloser = (CHECK_USER_GPS_DISTANCE > Distance) ? 1 : 0;
			vecCheckGPS.push_back(CheckGPS);
		}
	}

	//发送数据
	for (WORD i =0; i<m_wChairCount; i++)
	{
		//获取用户
		CPlayer *pIServerUserItem = GetTableUserItem(i);
		if (pIServerUserItem==NULL) continue;

		//发送数据
		for (DWORD j = 0; j < vecCheckGPS.size(); j++)
		{
			g_GameCtrl->SendData(pIServerUserItem, MDM_G_FRAME, CMD_GC_COM_CHECK_USER_GPS, &vecCheckGPS[j], sizeof(STR_CMD_CG_COM_CHECK_USER_GPS));
		}	
	}


	return true;
}

//根据经纬度计算距离，单位m
double CTableFrame::GetDistance(double long1, double lat1, double long2, double lat2)
{
	//变量定义
	double dEarthRadius = 6378137;
	double dPi = 3.1415926;
	double distance = 0.0;

	//转化成弧度
	lat1 = lat1 * dPi / 180.0;
	lat2 = lat2 * dPi / 180.0;

	double dLatTheta = fabs(lat1 - lat2);
	double dLongTheta = fabs((long1 - long2)*dPi / 180.0);

	double sa2 = sin(dLatTheta / 2.0);
	double sb2 = sin(dLongTheta / 2.0);

	//计算距离
	distance = fabs( 2 * dEarthRadius * asin( sqrt(sa2*sa2 + cos(lat1)*cos(lat2)*sb2*sb2) ) );

	return distance;
}

//检验用户的IP地址
bool CTableFrame::CheckUserIpAddress()
{
	//遍历桌子上的其他用户，两两比较
	std::vector<STR_CMD_CG_COM_CHECK_USER_IP> vecCheckIP;

	//遍历其他玩家
	for (WORD i=0; i<m_wChairCount ; i++)
	{
		//用户IP地址
		CPlayer *pCheckUserItem = GetTableUserItem(i);
		if (NULL == pCheckUserItem) continue;
		DWORD dwUserIP = pCheckUserItem->GetClientAddr();

		//与其他用户比较IP
		for (int j = i+1; j < m_wChairCount; j++)
		{
			//获取用户IP
			CPlayer * pITableUserItem = GetTableUserItem(j);
			if ( (pITableUserItem != NULL) &&
				(pITableUserItem != pCheckUserItem) &&
				(!pITableUserItem->IsAndroidUser()) &&
				(pITableUserItem->GetClientAddr()==dwUserIP) )
			{
				//发送数据至客户端
				STR_CMD_CG_COM_CHECK_USER_IP CheckIP;
				ZeroMemory(&CheckIP, sizeof(STR_CMD_CG_COM_CHECK_USER_IP));

				//赋值
				CheckIP.dwFirstUserID = pCheckUserItem->GetUserID();
				CheckIP.dwNextUserID = pITableUserItem->GetUserID();
				CheckIP.cbISame = 1;
				vecCheckIP.push_back(CheckIP);
			}
		}
	}

	//发送数据
	for (WORD k =0; k<m_wChairCount; k++)
	{
		//获取用户
		CPlayer *pIServerUserItem = GetTableUserItem(k);
		if (pIServerUserItem==NULL) continue;

		//发送数据
		for (DWORD i = 0; i < vecCheckIP.size(); i++)
		{
			g_GameCtrl->SendData(pIServerUserItem, MDM_G_FRAME, CMD_GC_COM_CHECK_USER_IP, &vecCheckIP[i], sizeof(STR_CMD_CG_COM_CHECK_USER_IP));
		}	
	}

	return true;
}

//积分效验
bool CTableFrame::EfficacyScoreRule(CPlayer * pIServerUserItem)
{
	//变量定义
	WORD wWinRate=pIServerUserItem->GetUserWinRate();
	WORD wFleeRate=pIServerUserItem->GetUserFleeRate();

	//积分范围
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//获取用户
		CPlayer * pITableUserItem=GetTableUserItem(i);

		//规则效验
		if (pITableUserItem!=NULL)
		{
			/*
			//获取规则
			tagUserRule * pTableUserRule=pITableUserItem->GetUserRule();

			//逃率效验
			if ((pTableUserRule->bLimitFleeRate)&&(wFleeRate>pTableUserRule->wMaxFleeRate))
			{
			//构造信息
			TCHAR szDescribe[128]=TEXT("");
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("您的逃跑率太高，与 %s 设置的设置不符，不能加入游戏！"),pITableUserItem->GetNickName());

			//发送信息
			SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);

			return false;
			}

			//胜率效验
			if ((pTableUserRule->bLimitWinRate)&&(wWinRate<pTableUserRule->wMinWinRate))
			{
			//构造信息
			TCHAR szDescribe[128]=TEXT("");
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("您的胜率太低，与 %s 设置的设置不符，不能加入游戏！"),pITableUserItem->GetNickName());

			//发送信息
			SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);

			return false;
			}

			//积分效验
			if (pTableUserRule->bLimitGameScore==true)
			{
			//最高积分
			if (pIServerUserItem->GetUserScore()>pTableUserRule->lMaxGameScore)
			{
			//构造信息
			TCHAR szDescribe[128]=TEXT("");
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("您的积分太高，与 %s 设置的设置不符，不能加入游戏！"),pITableUserItem->GetNickName());

			//发送信息
			SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);

			return false;
			}

			//最低积分
			if (pIServerUserItem->GetUserScore()<pTableUserRule->lMinGameScore)
			{
			//构造信息
			TCHAR szDescribe[128]=TEXT("");
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("您的积分太低，与 %s 设置的设置不符，不能加入游戏！"),pITableUserItem->GetNickName());

			//发送信息
			SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);

			return false;
			}

			}
			*/
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
//牌友圈群主解散房间
void CTableFrame::OnEventClubDismissRoom()
{
	m_pITableFrameSink->OnEventGameConclude(GER_DISMISS);
}

/****************************************    解散房间     ***************************************************/
//申请解散房间
bool CTableFrame::OnEventApplyDismissRoom(WORD wChairID, bool bAgree)
{
	//用户校验
	CPlayer *pApplyUserItem = GetTableUserItem(wChairID);
	if (NULL == pApplyUserItem)
		return false;

	//变量赋值
	m_bAgree[wChairID] = bAgree;
	//设置用户已经响应
	m_bResponseDismiss[wChairID] = true;

	//房间处于非空闲状态
	if (GAME_STATUS_FREE != GetGameStatus())
	{
		//判断房间是否可以解散
		tagTableRule* pCfg = (tagTableRule*)GetCustomRule();
		//俱乐部模式 && 房间设置不可解散 时候才生效 金币场也不可以解散  比赛不可解散
		if ((1 == GetClubRoomRule()->bDissolve) || (pCfg->GameMode == 2) || (pCfg->GameMode == 1))
		{
			STR_CMD_GR_FRMAE_ASK_DISMISS_RESULT cmdResult;
			ZeroMemory(&cmdResult, sizeof(cmdResult));
			cmdResult.lResultCode = -1;
			lstrcpyn(cmdResult.szDescribeString,
				TEXT("请联系会长解散房间!"),
				CountArray(cmdResult.szDescribeString));

			SendTableData(wChairID, CMD_GR_USER_ASK_DISMISS_RESULT, &cmdResult, sizeof(cmdResult), MDM_USER);
			return true;
		}

		//设置自动响应定时器
		SetVoteDismissRoom();

		//玩家点击解散房间，给其他玩家发送【同意/拒绝】弹框消息
		STR_CMD_GR_FRMAE_VOTE_DISMISS VoteRet;
		ZeroMemory(&VoteRet, sizeof(STR_CMD_GR_FRMAE_VOTE_DISMISS));

		//赋值
		VoteRet.cbApplyChair = static_cast<BYTE>(wChairID);
		VoteRet.cbAgree = 1;
		VoteRet.cbChairID = static_cast<BYTE>(wChairID);

		//广播投票结果消息让客户端弹出解散框
		SendTableData(INVALID_CHAIR, CMD_GR_USER_VOTE_DISMISS, &VoteRet, sizeof(STR_CMD_GR_FRMAE_VOTE_DISMISS), MDM_USER);

		//设置房间处于解散状态
		m_bUnderDissState = true;
		m_dissmisserChaiID = wChairID;

		return true;
	}
	else		//房间处于空闲状态
	{
		//清空数据
		ZeroMemory(&(m_bResponseDismiss), sizeof(m_bResponseDismiss));
		ZeroMemory(&(m_bAgree), sizeof(m_bAgree));

		//比赛场空闲状态也不可解散
		tagTableRule* pCfg = (tagTableRule*)GetCustomRule();
		if (pCfg->GameMode == 2)
			return false;

		//发送解散成功消息
		STR_CMD_GR_FRAME_DISMISS_RESULT DismissResult;
		ZeroMemory(&DismissResult, sizeof(DismissResult));
		//赋值
		DismissResult.cbDismiss = 1;  //直接解散

		//判断玩家是不是桌主
		if (pApplyUserItem->GetUserID() == GetTableOwner())
		{
			SendTableData(INVALID_CHAIR, CMD_GR_USER_DISMISS_RESULT, &DismissResult, sizeof(STR_CMD_GR_FRAME_DISMISS_RESULT), MDM_USER);

			CLog::Log(log_debug, "解散房间 ");
			//结束游戏
			HandleDJGameEnd(GAME_CONCLUDE_NORMAL);
			return true;
		}
		else		//不是房主解散，玩家直接离开
		{
			//发送消息,代替站起，客户端直接站起，并不是解散
			SendTableData(wChairID, CMD_GR_USER_DISMISS_RESULT, &DismissResult, sizeof(STR_CMD_GR_FRAME_DISMISS_RESULT), MDM_USER);

			//用户离开			
			if (PlayerLeaveTable(pApplyUserItem))
			{
				return false;
			}

			//检测房间里是否还有真人 没有真人大局结束
			CheckRoomTruePlayer();
		}
	}

	return false;
}

//表决是否解散房间
bool CTableFrame::OnEventVoteDismissRoom(WORD wChairID, bool bAgree)
{
	//用户校验
	CPlayer *pVoteUserItem = GetTableUserItem(wChairID);
	if (NULL == pVoteUserItem)			//TODO 为了解决用户掉线被游戏服删除，定时器到无法响应的bug，不进行用户校验
		return false;

	//变量赋值
	m_bAgree[wChairID] = bAgree;
	//设置用户已经响应
	m_bResponseDismiss[wChairID] = true;

	//向Client发送玩家投票结果
	STR_CMD_GR_FRMAE_VOTE_DISMISS VoteDismissRet;
	ZeroMemory(&VoteDismissRet, sizeof(STR_CMD_GR_FRMAE_VOTE_DISMISS));

	//赋值
	VoteDismissRet.cbAgree = (bAgree == true) ? 1 : 0;
	VoteDismissRet.cbChairID = static_cast<BYTE>(wChairID);
	VoteDismissRet.cbApplyChair = static_cast<BYTE>(m_dissmisserChaiID);

	//广播发送
	SendTableData(INVALID_CHAIR, CMD_GR_USER_VOTE_DISMISS, &VoteDismissRet, sizeof(STR_CMD_GR_FRMAE_VOTE_DISMISS), MDM_USER);

	//房间处于非空闲状态
	if (GAME_STATUS_FREE != GetGameStatus())
	{
		//变量定义
		int nAgree = 0;						//同意解散玩家数
		int nResponse = 0;					//响应玩家数
		int nPlayer = GetSitUserCount();	//桌子玩家数

		//获得同意玩家数和响应玩家数
		for (int i = 0; i < GetChairCount(); i++)
		{
			CPlayer *pTableUserItem = GetTableUserItem(i);
			if (pTableUserItem == NULL) continue;
			if (m_bAgree[i]) nAgree++;
			if (m_bResponseDismiss[i]) nResponse++;
		}

		//构造解散结果数据
		STR_CMD_GR_FRAME_DISMISS_RESULT DismissResult;
		ZeroMemory(&DismissResult, sizeof(STR_CMD_GR_FRAME_DISMISS_RESULT));

		//所有玩家同意解散	（过半同意则解散房间）
		if (static_cast<DOUBLE>(nAgree) > static_cast<DOUBLE>(nPlayer / 2))
		{
			//清空数据
			ZeroMemory(&(m_bResponseDismiss), sizeof(m_bResponseDismiss));
			ZeroMemory(&(m_bAgree), sizeof(m_bAgree));
			KillVoteDismissRoom();

			//赋值
			DismissResult.cbDismiss = 1;

			//广播房间解散成功消息			
			SendTableData(INVALID_CHAIR, CMD_GR_USER_DISMISS_RESULT, &DismissResult, sizeof(STR_CMD_GR_FRAME_DISMISS_RESULT), MDM_USER);

			//设置房间不处于解散状态
			m_bUnderDissState = false;
			m_dissmisserChaiID = 0xFF;

			//游戏中，结束子游戏（大局结束）
			if (m_pITableFrameSink != NULL)
				m_pITableFrameSink->OnEventGameConclude(GER_DISMISS);

			return true;
		}
		else if (nResponse == nPlayer)		//所有玩家响应，但有玩家未同意
		{
			//清空数据
			ZeroMemory(&(m_bResponseDismiss), sizeof(m_bResponseDismiss));
			ZeroMemory(&(m_bAgree), sizeof(m_bAgree));
			KillVoteDismissRoom();

			//赋值
			DismissResult.cbDismiss = 2;

			//广播房间解散失败消息			
			SendTableData(INVALID_CHAIR, CMD_GR_USER_DISMISS_RESULT, &DismissResult, sizeof(STR_CMD_GR_FRAME_DISMISS_RESULT), MDM_USER);

			//设置房间不处于解散状态
			m_bUnderDissState = false;
			m_dissmisserChaiID = 0xFF;
		}
	}

	return false;
}


//房间是否处于解散状态
bool CTableFrame::GetDismissState()
{
	return m_bUnderDissState;
}

//设置房间处于解散状态
void CTableFrame::SetDismissState(bool bState)
{
	m_bUnderDissState = bState;
}
