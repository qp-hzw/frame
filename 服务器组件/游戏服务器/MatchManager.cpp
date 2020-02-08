#include "stdafx.h"
#include "MatchManager.h"

STR_DBR_GR_MATCH_CONFIG			CMatchManager::m_match_cfg;
std::list<CMatchItem *>			CMatchManager::s_Item_Array;

//��ȡMatch����
bool CMatchManager::ReadMatchConfig(STR_DBR_GR_MATCH_CONFIG config)
{
	for (int i = 0; i < config.match_size; i++)
	{
		//����������������
		auto match = &config.match_list[i];
		if (match->wStartType == MATCH_START_TYPE_TIME)	
		{
			if (match->dwStartTime < time(0))	//�Ѿ���ʼ
			{
				//��ʼ��һ��
				match->dwStartTime = time(0) + 60000*3;	
			}
		}
		else if(match->wStartType == MATCH_START_TYPE_COUNT)
		{}

		//�������ٲ�������
		WORD wTotleCount = match->dwLowestPlayer;
		for (int j = 0; j < match->wStageSize; j++)
		{
			auto stage = &match->stage[j];

			stage->wPlayerCount = wTotleCount;
			if (stage->wJinJi_Rule == 1)	//ͬ���ȽϽ���
			{
				wTotleCount = (wTotleCount / 3) * stage->wJinJi_Count;  //������*��������
			}
			else if (stage->wJinJi_Rule == 2)
			{
				wTotleCount = stage->wJinJi_Count;
			}
		}
	}

	//����������Ϣ
	memcpy(&m_match_cfg, &config, sizeof(STR_DBR_GR_MATCH_CONFIG));

	//���ݱ�����������Ϣ ����������
	for (int i = 0; i < m_match_cfg.match_size; i++)
	{
		Create_Match(m_match_cfg.match_list[i]);
	}

	return true;
}

//����������
bool CMatchManager::Create_Match(MATCH_CONFIG config)
{
	//��������
	CMatchItem *Item = new CMatchItem(config);
	if (Item == NULL)
		return false;

	//��ʼ������
	Item->Init();

	//����list
	s_Item_Array.push_back(Item);

	return true;
}

//ɾ��������
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

//����
CMatchItem* CMatchManager::Find_Match_ByItemID(WORD wMatchID)
{
	for (auto Item : s_Item_Array)
	{
		if (Item && Item->GetMatchID() == wMatchID)
			return Item;
	}

	return NULL;
}

//��ʼ����
void CMatchManager::On_Match_Start(CMatchItem *Item)
{
	MATCH_CONFIG cfg = Item->GetConfig();
	if (cfg.wStartType == MATCH_START_TYPE_TIME)
	{
		cfg.dwStartTime = cfg.dwStartTime + 5*60000;
	}
	Create_Match(cfg);
}

//����ȡ��
void CMatchManager::On_Match_Cancel(CMatchItem *Item)
{
	Delete_Match(Item);
	On_Match_Start(Item);
}

//��������
void CMatchManager::On_Match_End(CMatchItem *Item, std::vector<player_info> playerInfo)
{
	Delete_Match(Item);
	Item->On_Match_End(playerInfo);
}