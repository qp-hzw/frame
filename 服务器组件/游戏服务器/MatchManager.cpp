#include "stdafx.h"
#include "MatchManager.h"
#include <iostream>

std::list<MATCH_CONFIG>			CMatchManager::m_match_cfg;
std::list<CMatchItem *>			CMatchManager::s_Item_Array;

//��ȡ������������Ϣ
bool CMatchManager::ReadMatchConfig(DWORD kindid)
{
	for (int i = 0; i < MAX_MATCH; i++)
	{
		//���ļ�
		string file_path = std::to_string(kindid) + "_" + std::to_string(i+1) + ".match";
		int iRet = CWHCfg::Instance()->OpenFile(file_path);
		if(iRet != 0 )
		{
			continue;
		}

		MATCH_CONFIG config;
		ZeroMemory(&config, sizeof(MATCH_CONFIG));

		CWHCfg::Instance()->GetItemValue("MATCH", "matchid", config.wMatchID);		//�ں˼�һ��WORD����
		CWHCfg::Instance()->GetItemValue("MATCH", "basescore", config.llBaseScore);
		CWHCfg::Instance()->GetItemValue("MATCH", "starttype", config.wStartType);
		CWHCfg::Instance()->GetItemValue("MATCH", "leastcount", config.dwLowestPlayer);
		CWHCfg::Instance()->GetItemValue("MATCH", "feetype", config.FeeType);
		CWHCfg::Instance()->GetItemValue("MATCH", "fee", config.llFee);
		CWHCfg::Instance()->GetItemValue("MATCH", "reword", config.llReword);
		CWHCfg::Instance()->GetItemValue("MATCH", "robotcount", config.dwRobotCount);
		CWHCfg::Instance()->GetItemValue("MATCH", "timeperiod", config.dwTimePeriod);

		//����
		string strTemp;
		CWHCfg::Instance()->GetItemValue("MATCH", "title", strTemp);
		std::wstring wStr = CWConvert::s2ws(strTemp);
		memcpy(config.szTitle, wStr.c_str(), sizeof(TCHAR) *32);

		//����
		strTemp = "";
		CWHCfg::Instance()->GetItemValue("MATCH", "describe", strTemp);
		std::wstring wDescribe = CWConvert::s2ws(strTemp);
		memcpy(config.szDescribe, wDescribe.c_str(), sizeof(TCHAR) *128);

		for(int j=0; j<MAX_MATCH_STAGE; j++)
		{
			char psz[20];
			sprintf(psz, "STAGE_%d", j);
			int ret = CWHCfg::Instance()->GetItemValue(psz, "xjcount", config.stage[j].wXJCount);
			if (ret != 0)	continue;

			CWHCfg::Instance()->GetItemValue(psz, "jinji_count", config.stage[j].wJinJi_Count);
			CWHCfg::Instance()->GetItemValue(psz, "jinji_rule", config.stage[j].wJinJi_Rule);

			string tmp;
			CWHCfg::Instance()->GetItemValue(psz, "name", tmp);
			std::wstring wName = CWConvert::s2ws(tmp);
			memcpy(config.stage[j].szName, wName.c_str(), sizeof(TCHAR) *16);

			config.wStageSize++;
		}

		//�ر��ļ�
		CWHCfg::Instance()->CloseFile();
		m_match_cfg.push_back(config);
	}

	//���ñ�����
	MatchConfig();

	return true;
}

//���ñ�����
bool CMatchManager::MatchConfig()
{
	for (auto ite = m_match_cfg.begin(); ite != m_match_cfg.end(); ite++)
	{
		//����������������
		auto config = ite;
		if (config->wStartType == MATCH_START_TYPE_TIME)	
		{
			if (config->dwStartTime < time(0))	//�Ѿ���ʼ
			{
				//��ʼ��һ��
				config->dwStartTime = time(0) + 60*config->dwTimePeriod;	
				//config->dwStartTime = time(0) + 10000;	
			}
		}

		//�������ٲ�������
		DWORD wTotleCount = config->dwLowestPlayer;
		for (int j = 0; j < config->wStageSize; j++)
		{
			auto stage = &config->stage[j];

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

	//���ݱ�����������Ϣ ����������
	for (auto config : m_match_cfg)
	{
		Create_Match(config);
	}

	return true;
}

//��ȡ����������Ϣ
std::list<MATCH_CONFIG> CMatchManager::GetAllMatchConfig()
{
	return m_match_cfg;
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
			s_Item_Array.erase(it);
			delete match;
			break;
		}
	}

	return true;
}

//����
CMatchItem* CMatchManager::Find_Match_ByItemID(WORD wMatchID)
{
	//���ڱ����ı����������Managerɾ�� �Ӻ���ǰ��
	for (auto ite = s_Item_Array.rbegin(); ite != s_Item_Array.rend(); ite++)
	{
		CMatchItem * Item = *ite;
		if (Item && Item->GetMatchID() == wMatchID)
			return Item;
	}

	return NULL;
}

//����ȫ��
std::list<CMatchItem *> CMatchManager::Find_Match_All()
{
	return s_Item_Array;
}

//��ʼ����
void CMatchManager::On_Match_Start(CMatchItem *Item)
{
	MATCH_CONFIG cfg = Item->GetConfig();
	if (cfg.wStartType == MATCH_START_TYPE_TIME)
	{
		cfg.dwStartTime = time(0) + 60*Item->GetConfig().dwTimePeriod;
	}
	Create_Match(cfg);
}

//����ȡ��
void CMatchManager::On_Match_Cancel(CMatchItem *Item)
{
	On_Match_Start(Item);
	Delete_Match(Item);
}

//��������
void CMatchManager::On_Match_End(CMatchItem *Item, std::list<player_info> playerInfo)
{
	Item->On_Match_End(playerInfo);
	CLog::Log(log_debug, "Delete Match begin");
	Delete_Match(Item);
	CLog::Log(log_debug, "Delete Match end");
}