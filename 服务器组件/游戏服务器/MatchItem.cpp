#include "stdafx.h"
#include "MatchItem.h"
#include "TableManager.h"
#include "GameCtrl.h"
#include "MatchManager.h"
#include <algorithm>
#include <iostream>

//定时器定义
#define	IDI_MATCH_TYPE_TIME			(IDI_MATCH_MODULE_START)			//定时开赛

CMatchItem::CMatchItem(MATCH_CONFIG config)
{
	m_Start = false;
	m_Stage = 0;

	m_Apply_Player.clear();
	m_Cur_Ranking.clear();
	m_room_player.clear();

	CopyMemory(&m_config, &config, sizeof(MATCH_CONFIG));

	//新建一个等待房间
	m_wait_room = CTableManager::CreateMatchRoom(this, m_Stage);
}


CMatchItem::~CMatchItem(void)
{
	CTableManager::DeleteTable(m_wait_room);
}

//初始化
void CMatchItem::Init()
{
	//定时开赛定时器
	if (m_config.wStartType == MATCH_START_TYPE_TIME)
	{
		g_GameCtrl->SetTimer(IDI_MATCH_TYPE_TIME + m_config.wMatchID, m_config.dwStartTime - time(0), 1, 0);
	}
}

//玩家报名
bool CMatchItem::On_User_Apply(CPlayer *player)
{
	//玩家校验
	if (player == NULL)
		return false;

	if (US_FREE != player->GetUserStatus())
		return false;

	//开始校验
	if (m_Start)
		return false;

	CLog::Log(log_debug, "报名玩家ID: %d", player->GetUserID());

	//玩家加入等待房间
	int ret = m_wait_room->PlayerEnterTable(player);
	if (ret != 0)
		return false;

	m_Apply_Player.push_back(player);

	//人满开赛
	if ((m_config.wStartType == MATCH_START_TYPE_COUNT) &&
		(m_Apply_Player.size() == m_config.dwLowestPlayer))
	{
		On_Apply_End();
	}

	return true;
}

//玩家取消报名
bool CMatchItem::On_User_UnApply(CPlayer *player)
{
	if (player == NULL)
		return false;

	if (m_Start)
		return false;

	for (auto it = m_Apply_Player.begin(); it != m_Apply_Player.end(); it++)
	{
		CPlayer *m_player = *it;
		if (m_player && m_player == player)
		{
			m_Apply_Player.erase(it);
			return true;
		}
	}

	//玩家离开等待房间
	m_wait_room->PlayerLeaveTable(player);

	return false;
}

//报名结束
bool CMatchItem::On_Apply_End()
{
	//报名人数校验
	DWORD wApplyCount = m_Apply_Player.size();
	DWORD wLeastCount = m_config.dwLowestPlayer;
	if (wApplyCount < wLeastCount)
	{
		//人数不够 比赛场结束
		CLog::Log(log_debug, "报名人数: %d，最少人数: %d, 人数不够, 取消比赛！", wApplyCount, wLeastCount);
		On_Match_Cancel();
		return false;
	}

	//踢出多余人数
	DWORD wFreeCount = wApplyCount % 3;			//待修改
	//1、 优先踢出Robot
	DWORD dwKick_Robot_count = 0;
	for (int i = 0; i < wFreeCount; i++)
	{
		for (auto ite = m_Apply_Player.begin(); ite != m_Apply_Player.end(); ite++)
		{
			CPlayer *player = *ite;
			if (player && player->IsAndroidUser())
			{
				m_wait_room->PlayerLeaveTable(player);
				ite = m_Apply_Player.erase(ite);
				dwKick_Robot_count++;
				break;
			}
		}
	}
	//2、 没有Robot踢出真实玩家
	for (int i = 0; i < wFreeCount - dwKick_Robot_count; i++)
	{
		for (auto ite = m_Apply_Player.rbegin(); ite != m_Apply_Player.rend(); ite++)
		{
			CPlayer *player = *ite;

			//构造消息
			STR_CMD_GC_MATCH_CANCEL cancel;
			ZeroMemory(&cancel, sizeof(STR_CMD_GC_MATCH_CANCEL));
			swprintf(cancel.szDescribe, 128, L"%S", "来晚了，把宁踢了！");
			SendDataAllPlayer(CMD_GC_MATCH_CANCEL, &cancel, sizeof(STR_CMD_GC_MATCH_CANCEL));

			//反向删除迭代器
			m_wait_room->PlayerLeaveTable(player);
			ite = std::list<CPlayer*>::reverse_iterator(m_Apply_Player.erase((++ite).base()));
			break;
		}
	}

	//开始比赛
	On_Match_Start();

	return true;
}

//比赛取消
bool CMatchItem::On_Match_Cancel()
{
	//设置状态
	m_Start = true;

	//构造消息
	STR_CMD_GC_MATCH_CANCEL cancel;
	ZeroMemory(&cancel, sizeof(STR_CMD_GC_MATCH_CANCEL));
	swprintf(cancel.szDescribe, 128, L"%S", "报名人数太少，比赛取消！！");

	//发送消息
	SendDataAllPlayer(CMD_GC_MATCH_CANCEL, &cancel, sizeof(STR_CMD_GC_MATCH_CANCEL));

	//玩家离开等待房间
	for (auto item : m_Apply_Player)
	{
		if (item)
			m_wait_room->PlayerLeaveTable(item);
	}

	m_Apply_Player.clear();

	//取消比赛
	CMatchManager::On_Match_Cancel(this);

	return true;
}

//比赛开始
bool CMatchItem::On_Match_Start()
{
	//创建下一场比赛
	CMatchManager::On_Match_Start(this);
	//设置比赛开始
	m_Start = true;

	//设置正在比赛的玩家的玩家信息
	for (auto user : m_Apply_Player)
	{
		player_info item;
		item.user = user;
		m_Cur_Ranking.push_back(item);
	}

	//设置比赛初赛的总人数
	m_config.stage[0].wPlayerCount = m_Apply_Player.size();

	//场次写入数据库

	//开始初赛
	On_Stage_Start();

	return true;
}

//开始当前阶段比赛
bool CMatchItem::On_Stage_Start()
{
	//随机分配房间
	//random_shuffle(m_Apply_Player.begin(), m_Apply_Player.end());

	//初始化所有房间的游戏状态  --所有桌都打完了才开始下一阶段
	m_Room_state.clear();
	m_Room_state.resize(m_Apply_Player.size() / 3);
	CLog::Log(log_debug, "阶段人数: %d", m_Apply_Player.size());
	CMatchRoom* room = NULL;
	int index = 0;
	for (auto item : m_Apply_Player)
	{
		//创建房间
		if (NULL == room)
		{
			CLog::Log(log_debug, "创建房间");
			room = CTableManager::CreateMatchRoom(this, m_Stage);
			room->set_index(index++);
		}

		//玩家离开等待房间
		m_wait_room->PlayerLeaveTable(item);

		//玩家加入到Table中
		int ret = room->PlayerSitTable(item);
        if (ret != 0)
        {
            return false;
        }

		//房间人满，比赛开始
		if (room->IsRoomFull())
		{
			CLog::Log(log_debug, "比赛开始！！！");
			// 通知比赛开始
			room->SendTableData(INVALID_CHAIR, CMD_GC_MATCH_START, NULL, 0, MDM_GR_MATCH);

			//设置准备定时器
			room->SetPlayerAutoReady();
		}

		//房间满了
		if (room->IsRoomFull())
		{
			room = NULL;
		}
	}

	return true;
}

//一轮结束处理
bool CMatchItem::On_Room_End(CMatchRoom *room)
{
	//设置Item房间状态
	int index = 0;
	for (auto ite = m_Room_state.begin(); ite != m_Room_state.end(); ite++)
	{
		if (index == room->get_index())
			*ite = true;
		index ++;
	}

	//是否要等待别的桌打完
	bool stage_end = true;
	for (auto state : m_Room_state)
	{
		if (!state)
		{
			stage_end = false;
			break;
		}
	}
	CLog::Log(log_debug, "等待桌子吗？？？ %d", !stage_end);
	//设置房间比赛状态
	if (!stage_end)
	{
		room->SetRoomState(wait_next);
	}

	//获取房间玩家排名
	std::list<player_info> ranking;
	for (auto item : m_Cur_Ranking)
	{
		if (item.room_id == room->GetTableID())
			ranking.push_back(item);
	}
	m_room_player[room->GetTableID()] = ranking;

	//根据规则处理玩家
	auto stage = m_config.stage[m_Stage];
	bool Jue_Sai = (m_Stage == (m_config.wStageSize - 1));
	if (stage.wJinJi_Rule == 1 && !Jue_Sai)	//同桌比较 --决赛放到阶段结束函数处理
	{
		//淘汰玩家
		std::list<player_info> TaoTai_Player;
		int flag = 0;	//对vector的无解
		for (auto player : ranking)
		{
			if (flag >= stage.wJinJi_Count)
				TaoTai_Player.push_back(player);
			flag ++;
		}
		On_Player_TaoTai(TaoTai_Player);

		//晋级玩家
		std::list<player_info> JinJi_Player;
		flag = 0;
		for (auto player : ranking)
		{
			if (flag < stage.wJinJi_Count)
				JinJi_Player.push_back(player);
			flag ++;
		}
		On_Player_JinJi(JinJi_Player);
	}
	else if (stage.wJinJi_Rule == 2 && !Jue_Sai && stage_end)
	{
		std::list<player_info> TaoTai_Player;
		std::list<player_info> JinJi_Player;
		for (int i = 0; i < m_Cur_Ranking.size(); i++)
		{
			if (i < stage.wJinJi_Count)
			{
				JinJi_Player.push_back(m_Cur_Ranking[i]);
			}
			else
			{
				TaoTai_Player.push_back(m_Cur_Ranking[i]);
				m_wait_room->PlayerLeaveTable(m_Cur_Ranking[i].user);
			}
		}
		On_Player_JinJi(JinJi_Player);
		On_Player_TaoTai(TaoTai_Player);
	}
	else if (stage.wJinJi_Rule == 2 && !Jue_Sai && !stage_end)
	{
		for (auto item : ranking)
		{
			//发送等待桌数
			Updata_WaitCount(item.user);
			//加入等待房间
			m_wait_room->PlayerEnterTable(item.user);
		}
	}

	//所有桌都打完了
	if (stage_end)
		On_Stage_End(room);

	return true;
}

//淘汰玩家
bool CMatchItem::On_Player_TaoTai(std::list<player_info> TaoTai_player)
{
	//构造
	STR_CMD_GC_MATCH_RESULT_TAOTAI taotai;

	//发送玩家淘汰
	for (auto item : TaoTai_player)
	{
		//玩家离开等待房间
		m_wait_room->PlayerLeaveTable(item.user);

		ZeroMemory(&taotai, sizeof(STR_CMD_GC_MATCH_RESULT_TAOTAI));

		swprintf(taotai.szStageName, 16, L"%S", m_config.stage[m_Stage].szName);
		taotai.wRanking = GetRanking(item.user);

		CLog::Log(log_debug, "玩家 :%d 淘汰！", item.user->GetUserID());
		if (!item.user->IsAndroidUser())
			g_GameCtrl->SendData(item.user, MDM_GR_MATCH, CMD_GC_MATCH_RESULT_TAOTAI, NULL, 0);
	}

	//从列表里删除玩家
	for (auto taotai : TaoTai_player)
	{
		for (auto ite = m_Apply_Player.begin(); ite != m_Apply_Player.end(); ite++)
		{
			if ((*ite) == taotai.user)
			{
				ite = m_Apply_Player.erase(ite);
				break;
			}
		}
	}

	CLog::Log(log_debug, "apply size: %d", m_Apply_Player.size());

	//从排行表里删除玩家
	for (auto taotai : TaoTai_player)
	{
		for (auto ite = m_Cur_Ranking.begin(); ite != m_Cur_Ranking.end(); ite++)
		{
			if ((*ite).user == taotai.user)
			{
				ite = m_Cur_Ranking.erase(ite);
				break;
			}
		}
	}
	CLog::Log(log_debug, "ranking size: %d", m_Cur_Ranking.size());

	return true;
}

//晋级玩家
bool CMatchItem::On_Player_JinJi(std::list<player_info> JinJi_player)
{
	//构造
	STR_CMD_GC_MATCH_RESULT_JINJI msgjinji;

	WORD playing_count = 0;
	for (auto item : m_Room_state)
	{
		if (!item)
			playing_count ++;
	}

	//发送玩家晋级消息
	for (auto jinji : JinJi_player)
	{
		CLog::Log(log_debug, "玩家 :%d 晋级！", jinji.user->GetUserID());

		ZeroMemory(&msgjinji, sizeof(STR_CMD_GC_MATCH_RESULT_JINJI));

		swprintf(msgjinji.szStageName, 16, L"%S", m_config.stage[m_Stage].szName);
		msgjinji.wWaitCount = playing_count;
		msgjinji.wRanking = GetRanking(jinji.user);

		if (!jinji.user->IsAndroidUser())
			g_GameCtrl->SendData(jinji.user, MDM_GR_MATCH, CMD_GC_MATCH_RESULT_JINJI, &msgjinji, sizeof(STR_CMD_GC_MATCH_RESULT_JINJI));
	}

	//玩家加入等待房间
	for (auto jinji : JinJi_player)
	{
		m_wait_room->PlayerEnterTable(jinji.user);
	}

	return true;
}

//一阶段比赛结束
bool CMatchItem::On_Stage_End(CMatchRoom *room)
{
	//决赛处理
	if (m_Stage == (m_config.wStageSize - 1))
	{
		//玩家
		auto ite = m_room_player.find(room->GetTableID());
		if (ite == m_room_player.end())
			return false;

		//比赛结束
		CMatchManager::On_Match_End(this, ite->second);
		return true;
	}

	//开始下一轮
	m_Stage ++;
	m_wait_room->SetStageTimer();

	return true;
}

//更新排名
bool CMatchItem::Update_Ranking(CMatchRoom *room)
{
	//获取玩家信息 //小局分和总分 在Sub小局结束时传给frame
	std::vector<CPlayer *> player_list = room->GetPlayer_list();
	std::vector<SCORE> TotalScore = room->GetTotalScore();
	CLog::Log(log_debug, "更新排名！");
	//更新玩家信息
	for (int i = 0; i < room->GetChairCount(); i++)
	{
		for (auto ite = m_Cur_Ranking.begin(); ite != m_Cur_Ranking.end(); ite++)
		{
			player_info *info = &(*ite);
			if (info->user == player_list[i])
			{
				info->score += TotalScore[i];
				info->room_id = room->GetTableID();
				break;
			}
		}
	}

	//m_Cur_Ranking排序
	std::sort(m_Cur_Ranking.begin(), m_Cur_Ranking.end(), [this](const player_info& item_1, const player_info& item_2)
	{
		if (item_1.score == item_2.score)
		{
			auto order_1 = std::find_if(m_Apply_Player.begin(), m_Apply_Player.end(), [item_1](CPlayer* src)
			{
				return src == item_1.user;
			});

			auto order_2 = std::find_if(m_Apply_Player.begin(), m_Apply_Player.end(), [item_2](CPlayer* src)
			{
				return src == item_2.user;
			});

			return std::distance(order_1, order_2) > 0;
		}
		return item_1.score > item_2.score;
	});

	return true;
}

//发送所有人排名
void CMatchItem::Send_Ranking(CPlayer *player)
{
	DWORD	dwindex = 0;
	DWORD	wPacketSize = 0;
	BYTE	byBuffer[MAX_ASYNCHRONISM_DATA/10];
	STR_CMD_GC_MATCH_RANKING  *rank = NULL;
	for (auto ranking : m_Cur_Ranking)
	{
		if (wPacketSize + sizeof(STR_CMD_GC_MATCH_RANKING) > sizeof(byBuffer))
		{
			//群发
			if (player == NULL)
				SendDataAllPlayer(CMD_GC_MATCH_RANKING, &ranking, sizeof(STR_CMD_GC_MATCH_RANKING));
			else
				g_GameCtrl->SendData(player, MDM_GR_MATCH, CMD_GC_MATCH_RANKING, byBuffer, wPacketSize);
		}

		rank = (STR_CMD_GC_MATCH_RANKING *)(byBuffer + wPacketSize);
		rank->llScore = ranking.score;
		rank->wRanking = ++dwindex;
		memcpy(rank->szName, ranking.user->GetNickName(), sizeof(TCHAR)*LEN_NICKNAME);

		wPacketSize += sizeof(STR_CMD_GC_MATCH_RANKING);
	}
	if (wPacketSize > 0)
	{
		//群发
		if (player == NULL)
			SendDataAllPlayer(CMD_GC_MATCH_RANKING, byBuffer, wPacketSize);
		else if (!player->IsAndroidUser())
			g_GameCtrl->SendData(player, MDM_GR_MATCH, CMD_GC_MATCH_RANKING, byBuffer, wPacketSize);
	}
}

//发送自己排名
void CMatchItem::Send_Self_Ranking(CPlayer *player)
{
	CMatchRoom *room = (CMatchRoom *)CTableManager::FindTableByTableID(player->GetTableID());
	if (room == NULL)
		return;

	STR_CMD_CG_MATCH_RANKING_MY rank;
	ZeroMemory(&rank, sizeof(STR_CMD_CG_MATCH_RANKING_MY));

	rank.dwAllUserCount = m_Apply_Player.size();
	rank.dwCurCount = room->GetCurGameRound();
	rank.dwAllCount = room->GetCustomRule()->GameCount;
	rank.dwRanking = GetRanking(player);
	memcpy(rank.szStageName, m_config.stage[m_Stage].szName, sizeof(TCHAR)*16);

	CLog::Log(log_debug, "all user count: %d", rank.dwAllUserCount);

	g_GameCtrl->SendData(player, MDM_GR_MATCH, CMD_GC_MATCH_RANKING_MY, &rank, sizeof(STR_CMD_CG_MATCH_RANKING_MY));
}

//比赛结束
bool CMatchItem::On_Match_End(std::list<player_info> player)
{
	//玩家奖励
	CLog::Log(log_debug, "比赛结束!!!");

	//战绩


	return true;
}

//群发数据
void CMatchItem::SendDataAllPlayer(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	for (auto player : m_Apply_Player)
	{
		if (!player) continue;
		if (player->IsAndroidUser()) continue;

		g_GameCtrl->SendData(player, MDM_GR_MATCH, wSubCmdID, pData, wDataSize);
	}
}

//发送等待桌数
void CMatchItem::Updata_WaitCount(CPlayer *player)
{
	WORD playing_count = 0;
	for (auto item : m_Room_state)
	{
		if (!item)
			playing_count ++;
	}

	STR_CMD_GC_MATCH_WAIT_COUNT wait;
	wait.wait_count = playing_count;

	if (!player->IsAndroidUser())
		g_GameCtrl->SendData(player, MDM_GR_MATCH, CMD_GC_MATCH_WAIT_COUNT, &wait, sizeof(STR_CMD_GC_MATCH_WAIT_COUNT));
}

//机器人是否报名满了
bool CMatchItem::IsRobotFull()
{
	WORD robot_count = 0;
	for (auto player : m_Apply_Player)
	{
		if (player && player->IsAndroidUser())
			robot_count++;
	}

	return (robot_count == m_config.dwRobotCount);
}

//获取玩家排名
WORD CMatchItem::GetRanking(CPlayer *player)
{
	WORD rank = 0;
	for (int i = 0; i < m_Cur_Ranking.size(); i++)
	{
		if (m_Cur_Ranking[i].user == player)
		{
			rank = i+1;
			return rank;
		}
	}

	return 0;
}