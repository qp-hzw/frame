#include "stdafx.h"
#include "MatchManager.h"

STR_DBR_GR_MATCH_CONFIG			CMatchManager::m_match_cfg;
std::list<CMatchItem *>			CMatchManager::s_Item_Array;

//读取Match配置
bool CMatchManager::ReadMatchConfig(STR_DBR_GR_MATCH_CONFIG config)
{
	for (int i = 0; i < config.match_size; i++)
	{
		//单个比赛场的配置
		auto match = &config.match_list[i];
		if (match->wStartType == MATCH_START_TYPE_TIME)	
		{
			if (match->dwStartTime < time(0))	//已经开始
			{
				//开始下一场
				match->dwStartTime = time(0) + 60000*3;	
			}
		}
		else if(match->wStartType == MATCH_START_TYPE_COUNT)
		{}

		//设置最少参赛人数
		WORD wTotleCount = match->dwLowestPlayer;
		for (int j = 0; j < match->wStageSize; j++)
		{
			auto stage = &match->stage[j];

			stage->wPlayerCount = wTotleCount;
			if (stage->wJinJi_Rule == 1)	//同桌比较晋级
			{
				wTotleCount = (wTotleCount / 3) * stage->wJinJi_Count;  //桌子数*晋级名额
			}
			else if (stage->wJinJi_Rule == 2)
			{
				wTotleCount = stage->wJinJi_Count;
			}
		}
	}

	//设置配置信息
	memcpy(&m_match_cfg, &config, sizeof(STR_DBR_GR_MATCH_CONFIG));

	//根据比赛场配置信息 创建比赛场
	for (int i = 0; i < m_match_cfg.match_size; i++)
	{
		Create_Match(m_match_cfg.match_list[i]);
	}

	return true;
}

//创建比赛场
bool CMatchManager::Create_Match(MATCH_CONFIG config)
{
	//创建比赛
	CMatchItem *Item = new CMatchItem(config);
	if (Item == NULL)
		return false;

	//初始化比赛
	Item->Init();

	//加入list
	s_Item_Array.push_back(Item);

	return true;
}

//删除比赛场
bool CMatchManager::Delete_Match(CMatchItem *Item)
{
	for (auto it = s_Item_Array.begin(); it != s_Item_Array.end(); it++)
	{
		CMatchItem *match = *it;
		if (match == Item)
		{
			it = s_Item_Array.erase(it);
			delete match;
		}
	}

	return true;
}

//查找
CMatchItem* CMatchManager::Find_Match_ByItemID(WORD wMatchID)
{
	for (auto Item : s_Item_Array)
	{
		if (Item && Item->GetMatchID() == wMatchID)
			return Item;
	}

	return NULL;
}

//开始比赛
void CMatchManager::On_Match_Start(CMatchItem *Item)
{
	MATCH_CONFIG cfg = Item->GetConfig();
	if (cfg.wStartType == MATCH_START_TYPE_TIME)
	{
		cfg.dwStartTime = cfg.dwStartTime + 5*60000;
	}
	Create_Match(cfg);
}

//比赛取消
void CMatchManager::On_Match_Cancel(CMatchItem *Item)
{
	Delete_Match(Item);
	On_Match_Start(Item);
}

//比赛结束
void CMatchManager::On_Match_End(CMatchItem *Item, std::vector<player_info> playerInfo)
{
	Delete_Match(Item);
	Item->On_Match_End(playerInfo);
}