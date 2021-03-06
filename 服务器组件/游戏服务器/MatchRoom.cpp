#include "stdafx.h"
#include "MatchRoom.h"
#include "MatchItem.h"
#include "RoomRuleManager.h"
#include "GameCtrl.h"
#include "TableManager.h"

CMatchRoom::CMatchRoom(CMatchItem *item, WORD stage)
{
	m_Match_Item = item;
	m_state = wait_start;
	m_index = 0;

	//获取房间规则
	tagTableRule rule;
	ZeroMemory(&rule, sizeof(tagTableRule));
	RoomRuleManager::SetMatchRule(rule, &(item->GetConfig()), stage);

	//设置规则
	SetCommonRule(&rule);
}


CMatchRoom::~CMatchRoom(void)
{
}

CMatchRoom::CMatchRoom(const CMatchRoom& room)
{
	m_state = room.m_state;
	m_index = room.m_index;
	m_Match_Item = room.m_Match_Item;
}

//玩家坐下
int CMatchRoom::PlayerSitTable(CPlayer * pIServerUserItem, WORD wChairID, bool bCheckUserGPS)
{
	if (m_state != wait_start)
		return -1;

	//先走正常坐下流程
	int ret = CTableFrame::PlayerSitTable(pIServerUserItem);
	if (ret != 0)
		return ret;

	//扣除报名费


	return 0;
}

//小局结束
bool CMatchRoom::HandleXJGameEnd(BYTE byRound, WORD *wIdentity, SCORE *lUserTreasure)
{
	//正常小局结束流程
	CTableFrame::HandleXJGameEnd(byRound, wIdentity, lUserTreasure);
	CLog::Log(log_debug, "小局结束！");
	//更新排名
	m_Match_Item->Update_Ranking(this);

	//判断大局是否结束
	if (GetCurGameRound() !=	GetCustomRule()->GameCount)
	{
		//更新比赛场信息

		//设置自动准备定时器
		SetPlayerAutoReady();	
	}

	return true;
}

//大局结束
bool CMatchRoom::HandleDJGameEnd(BYTE cbGameStatus)
{
	//玩家离开房间
	for(auto player : GetPlayer_list())
	{
		PlayerLeaveTable(player);
	}

	//处理比赛场一轮结束流程
	m_Match_Item->On_Room_End(this);

	//删除桌子
	CTableManager::DeleteTable(this);

	return true;
}

//游戏开始
bool CMatchRoom::StartGame()
{
	CTableFrame::StartGame();

	//给所有玩家更新自己的排名
	for (auto player : GetPlayer_list())
	{
		if (player && !player->IsAndroidUser())
		{
			m_Match_Item->Send_Self_Ranking(player);
		}
	}

	return true;
}

//开始下一阶段比赛
void CMatchRoom::StartNextStage()
{
	m_Match_Item->On_Stage_Start();
}

//比赛断线重连
void CMatchRoom::On_Match_Offline(CPlayer *player)
{
	//游戏中发送自己排名 等待下阶段发送等待桌数
	if (m_state == game)
	{
		m_Match_Item->Send_Self_Ranking(player);
	}
	else if(m_state == wait_next)
	{
		m_Match_Item->Updata_WaitCount(player);
	}
	else if(m_state == wait_start)
	{
		m_Match_Item->Apply_Offline(player);
	}
}