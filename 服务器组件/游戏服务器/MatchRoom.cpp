#include "stdafx.h"
#include "MatchRoom.h"
#include "MatchItem.h"
#include "RoomRuleManager.h"
#include "GameCtrl.h"

CMatchRoom::CMatchRoom(CMatchItem *item)
{
	m_Match_Item = item;

	//获取房间规则
	tagTableRule rule;
	ZeroMemory(&rule, sizeof(tagTableRule));
	RoomRuleManager::Instance()->SetMatchRule(rule);

	//设置规则
	SetCommonRule(&rule);
}


CMatchRoom::~CMatchRoom(void)
{
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
bool CMatchRoom::HandleXJGameEnd(BYTE cbCurGameCount, BYTE cbGameMode, SCORE *lGameScore)
{
	//正常小局结束流程
	CTableFrame::HandleXJGameEnd(cbCurGameCount, cbGameMode, lGameScore);

	//更新排名
	m_Match_Item->Update_Ranking(this);

	//判断大局是否结束
	if (false)
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
	//处理正常大局结束流程
	CTableFrame::HandleDJGameEnd(cbGameStatus);

	//处理比赛场一轮结束流程
	m_Match_Item->On_Room_End(this);

	return true;
}

//游戏开始
bool CMatchRoom::StartGame()
{
	return true;
}

//开始下一阶段比赛
void CMatchRoom::StartNextStage()
{
	m_Match_Item->On_Stage_Start();
}