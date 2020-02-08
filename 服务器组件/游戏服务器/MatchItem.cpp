#include "stdafx.h"
#include "MatchItem.h"
#include "TableManager.h"
#include "GameCtrl.h"
#include "MatchManager.h"
#include <algorithm>

//定时器定义
#define	IDI_MATCH_TYPE_TIME			(IDI_MATCH_MODULE_START+1)			//定时开赛

CMatchItem::CMatchItem(MATCH_CONFIG config)
{
	m_Start = false;
	m_Stage = 0;

	//新建一个等待房间
	m_wait_room = new CMatchRoom(this);

	CopyMemory(&m_config, &config, sizeof(MATCH_CONFIG));
}


CMatchItem::~CMatchItem(void)
{
}

//初始化
void CMatchItem::Init()
{
	//1、机器人定时器

	//2、定时开赛定时器
	if (m_config.wStartType == MATCH_START_TYPE_TIME)
	{
		g_GameCtrl->SetTimer(IDI_MATCH_TYPE_TIME, m_config.dwStartTime - time(0), 1, 0);
	}
}

//玩家报名
bool CMatchItem::On_User_Apply(CPlayer *player)
{
	if (player == NULL)
		return false;

	m_Apply_Player.push_back(player);

	//玩家加入等待房间
	m_wait_room->PlayerEnterTable(player);

	return true;
}

//玩家取消报名
bool CMatchItem::On_User_UnApply(CPlayer *player)
{
	if (player == NULL)
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

	return false;
}

//报名结束
bool CMatchItem::On_Apply_End()
{
	//报名人数校验
	WORD wApplyCount = m_Apply_Player.size();
	WORD wLeastCount = m_config.dwLowestPlayer;
	if (wApplyCount < wLeastCount)
	{
		//人数不够 比赛场结束
		On_Match_Cancel();
		return false;
	}

	//踢出多余人数
	WORD wFreeCount = wApplyCount % 3;			//待修改
	//1、 优先踢出Robot
	//没有Robot踢出真实玩家

	//开始比赛
	On_Match_Start();

	return true;
}

//比赛取消
bool CMatchItem::On_Match_Cancel()
{
	//构造消息
	
	//发送消息
	SendDataAllPlayer(CMD_GC_MATCH_CANCEL, NULL, 0);

	//走淘汰流程

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
	//更新上一轮积分	//是否有必要
	m_Last_Score.clear();
	for (auto Item : m_Cur_Ranking)
	{
		CPlayer *player = Item.user;
		m_Last_Score.insert(std::make_pair(player->GetUserID(), Item.score));
	}

	//随机分配房间
	random_shuffle(m_Apply_Player.begin(), m_Apply_Player.end());

	//初始化所有房间的游戏状态  --所有桌都打完了才开始下一阶段
	m_Room_state.clear();
	m_Room_state.resize(m_Apply_Player.size() / 3);

	CMatchRoom* room = NULL;
	int index = 0;
	for (auto item : m_Apply_Player)
	{
		//创建房间
		if (NULL == room)
		{
			room = CTableManager::CreateMatchRoom(this);
			room->set_index(index++);
		}

		//玩家加入到Table中
		int ret = room->PlayerSitTable(item);
        if (ret != 0)
        {
            return false;
        }

		//房间人满，比赛开始
		if (room->IsRoomFull())
		{
			// 通知比赛开始
			room->SendTableData(INVALID_CHAIR, CMD_GC_MATCH_START, NULL, 0, MDM_GR_MATCH);
		}

		//发送比赛场信息
		//SendMatchInfo(item, 1,room);

		//玩家准备
		ret = room->PlayerReady(item);
		if (ret != 0)
        {
            return false;
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
	m_Room_state[room->get_index()] = true;

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

	//设置房间比赛状态
	if (!stage_end)
	{
		room->SetRoomState(wait_next);
	}

	//获取房间玩家排名
	std::vector<player_info> ranking;
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
		std::vector<player_info> TaoTai_Player;
		for (int i = stage.wJinJi_Count; i < room->GetChairCount(); i++)
		{
			TaoTai_Player.push_back(ranking[i]);
		}
		On_Player_TaoTai(TaoTai_Player, room);

		//晋级玩家
		std::vector<player_info> JinJi_Player;
		for (int i = 0; i < stage.wJinJi_Count; i++)
		{
			JinJi_Player.push_back(ranking[i]);
		}
		On_Player_JinJi(JinJi_Player, room);
	}

	//所有桌都打完了
	if (stage_end)
		On_Stage_End(room);

	return true;
}

//淘汰玩家
bool CMatchItem::On_Player_TaoTai(std::vector<player_info> TaoTai_player, CMatchRoom *room)
{
	//发送玩家淘汰
	for (auto item : TaoTai_player)
	{
		g_GameCtrl->SendData(item.user, MDM_GR_MATCH, CMD_GC_MATCH_RESULT, NULL, 0);
	}

	//从列表里删除玩家
	for (auto ite = m_Apply_Player.begin(); ite != m_Apply_Player.end(); ite++)
	{
		CPlayer *player = *ite;
		for (auto taotai : TaoTai_player)
		{
			if (player && player == taotai.user)
				ite = m_Apply_Player.erase(ite);
		}
	}

	return true;
}

//晋级玩家
bool CMatchItem::On_Player_JinJi(std::vector<player_info> JinJi_player, CMatchRoom *room)
{
	//发送玩家晋级消息
	for (auto jinji : JinJi_player)
	{
		g_GameCtrl->SendData(jinji.user, MDM_GR_MATCH, CMD_GC_MATCH_RESULT, NULL, 0);
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
	}

	//开始下一轮
	m_Stage ++;
	room->SetStageTimer();

	return true;
}

//更新排名
bool CMatchItem::Update_Ranking(CMatchRoom *room)
{
	//获取玩家信息 //小局分和总分 在Sub小局结束时传给frame
	std::vector<CPlayer *> player_list = room->GetPlayer_list();
	std::vector<int> SingleScore;
	std::vector<int> TotalScore;

	//更新玩家信息
	for (int i = 0; i < room->GetChairCount(); i++)
	{
		for (auto player : m_Cur_Ranking)
		{
			if (player.user == player_list[i])
			{
				player.ju_score = SingleScore[i];
				player.score = TotalScore[i];
				player.room_id = room->GetTableID();
				player.seat = room->GetPlayerChair(player_list[i]);
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

	//向Client发送玩家排名
	Send_Ranking();

	return true;
}

//更新排名
void CMatchItem::Send_Ranking()
{
	STR_CMD_GC_MATCH_RANKING  ranking;
	ZeroMemory(&ranking, sizeof(STR_CMD_GC_MATCH_RANKING));

	WORD wRanking = 0;
	ranking.wPlayerCount = m_Cur_Ranking.size();
	for (int i = 0; i < m_Cur_Ranking.size(); i++)
	{
		ranking.player[i].wRanking = ++wRanking;
		ranking.player[i].llScore = m_Cur_Ranking[i].score;
		//ranking.player[i].szName = m_Cur_Ranking[i].user->GetNickName();
	}

	for (auto item : m_Apply_Player)
	{
		g_GameCtrl->SendData(item, MDM_GR_MATCH, CMD_GC_MATCH_RANKING, &ranking, sizeof(STR_CMD_GC_MATCH_RANKING));
	}
}

//比赛结束
bool CMatchItem::On_Match_End(std::vector<player_info> player)
{
	//玩家奖励

	//战绩
	return true;
}

//群发数据
void CMatchItem::SendDataAllPlayer(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	for (auto player : m_Apply_Player)
	{
		if (!player) continue;

		g_GameCtrl->SendData(player, MDM_GR_MATCH, wSubCmdID, pData, wDataSize);
	}
}