#include "stdafx.h"
#include "RoomRuleManager.h"
#include <iostream>
#include <stdlib.h>
#include "player.h"
#include "DataBaseEngineSink.h"
#include "MatchItem.h"

rule_all_arry						RoomRuleManager::m_rule_arry;
ISubRoomRuleManager*            RoomRuleManager::m_SubRoomRuleManager = NULL;
std::map<BYTE,  STR_CMD_GC_USER_GOLD_INFO> RoomRuleManager::s_RoomInfo; 

//��ʼ��
void RoomRuleManager::Init(int kindid, string dll_name)
{
	InitFK(kindid);
	InitGold(kindid);

	m_SubRoomRuleManager = static_cast<ISubRoomRuleManager*>(CWHModule::GetSubRuleManager(dll_name));
}
//��ʼ�� ������ 
void RoomRuleManager::InitFK(int kindid)
{
	//���ļ�
	string file_path = std::to_string(kindid)+ ".rule";
	int iRet = CWHCfg::Instance()->OpenFile(file_path);
	if(iRet != 0 )
	{
		return ;
	}

	BYTE card_group_num = 0;
	for(int i=0; i<20; i++)
	{
		char psz[20];
		sprintf(psz, "RULE_%d", i);
		string strTemp;
		iRet = CWHCfg::Instance()->GetItemValue(psz, "head", strTemp);
		if(iRet != 0) continue;
		std::wstring wStr = CWConvert::s2ws(strTemp);
		memcpy(m_rule_arry.ItemArry[i].szHeadName, wStr.c_str(), sizeof(TCHAR)*15);

		for(int j=0; j<4; j++)
		{
			char value[20];
			sprintf(value, "value_%d", j);
			iRet = CWHCfg::Instance()->GetItemValue(psz, value, strTemp);
			if(iRet != 0) continue;
			std::wstring wStr = CWConvert::s2ws(strTemp);
			memcpy(m_rule_arry.ItemArry[i].szItemValue[j], wStr.c_str(), sizeof(TCHAR)*10);

			sprintf(value, "cvalue_%d", j);
			iRet = CWHCfg::Instance()->GetItemValue(psz, value, strTemp);
			if(iRet != 0) continue;
			wStr = CWConvert::s2ws(strTemp);
			memcpy(m_rule_arry.ItemArry[i].szItemValueForC[j], wStr.c_str(), sizeof(TCHAR)*10);
		}
	}

	//�ر��ļ�
	CWHCfg::Instance()->CloseFile();
}
//��ʼ�� ��ҳ�
void RoomRuleManager::InitGold(int kindid)
{
	//���ļ�
	string file_path = std::to_string(kindid)+ ".gold";
	int iRet = CWHCfg::Instance()->OpenFile(file_path);
	if(iRet != 0 )
	{
		return ;
	}

	for(int i=0; i<20; i++)
	{
		char psz[20];
		sprintf(psz, "ROOM_LEVEL_%d", i);

		STR_CMD_GC_USER_GOLD_INFO roomInfo;
		roomInfo.bGoldMod = i;
		iRet = CWHCfg::Instance()->GetItemValue(psz, "difen", roomInfo.dwScore);
		if(iRet != 0) continue;
		iRet = CWHCfg::Instance()->GetItemValue(psz, "enterGold", roomInfo.dwGold);
		if(iRet != 0) continue;

		memcpy(&s_RoomInfo[i], &roomInfo, sizeof(STR_CMD_GC_USER_GOLD_INFO));
	}

	for(auto item : s_RoomInfo)
	{
		CLog::Log(log_debug, "level %d,  score: %d,  gold: %d", item.second.bGoldMod, item.second.dwScore, item.second.dwGold);
	}

	//�ر��ļ�
	CWHCfg::Instance()->CloseFile();
}


//��ȡ������ ��������ѡ��
rule_arry RoomRuleManager::GetRoomRuleSetting()
{
	rule_arry ruleArry;
	for(int i=0; i<20; i++)
	{
		memcpy(ruleArry.ItemArry[i].szHeadName, m_rule_arry.ItemArry[i].szHeadName, sizeof(TCHAR)*15);
		memcpy(ruleArry.ItemArry[i].szItemValue, m_rule_arry.ItemArry[i].szItemValueForC, sizeof(TCHAR)*10*4);
	}
	return ruleArry;
}
//��ȡ������ �������
tagTableRule RoomRuleManager::GetFKRoomRule(byte value[20], byte GameMode)
{
	tagTableRule roomRule;
	ZeroMemory(&roomRule, sizeof(roomRule));
	roomRule.GameMode = GameMode;

	//���ݳ�ʼ��
	m_SubRoomRuleManager->Init();

	for(int i=0; i<20; i++)
	{
		std::wstring wStr(m_rule_arry.ItemArry[i].szHeadName);
		string key_name = CWConvert::ws2s(wStr);
		if(key_name.empty())
		{
			continue;
		}

		if(value[i] >= 4) continue;

		SetRoomRule(roomRule, key_name, value[i]);
	}
	return roomRule;
}

//��ȡ��ҳ� �������
tagTableRule RoomRuleManager::GetGoldRoomRule(BYTE byType)
{
	//���ݳ�ʼ��
	m_SubRoomRuleManager->Init();
	m_SubRoomRuleManager->SetGoldRule(&s_RoomInfo[byType-1]);	//ע��: client������˳����1234 server��˳����0123  TODOLATER

	tagTableRule roomRule;
	ZeroMemory(&roomRule, sizeof(tagTableRule));

	roomRule.GameMode = TABLE_MODE_GOLD;
	roomRule.GameRoomLevel = byType;
	roomRule.GameCount = 1;
	roomRule.PlayerCount =3;
	return roomRule;
}

//���ñ���������
void RoomRuleManager::SetMatchRule(tagTableRule& roomRule, MATCH_CONFIG *config, WORD wstage)
{
	//���ݳ�ʼ��
	m_SubRoomRuleManager->Init();
	m_SubRoomRuleManager->SetMatchRule((VOID *)config);

	roomRule.GameMode = TABLE_MODE_MATCH;
	roomRule.GameCount = config->stage[wstage].wXJCount;
	roomRule.PlayerCount =3;
}

//��ȡ���� ͨ�÷������
tagTableRule RoomRuleManager::GetClubComRoomRule(DWORD dwClubID, DWORD dwRoomID)
{
	//��ȡ�����������value
	BYTE value[20];
	memset(value, 0, 20);

	g_TreasureDB->ResetParameter();
	g_TreasureDB->AddParameter(TEXT("@ClubID"), dwClubID);
	g_TreasureDB->AddParameter(TEXT("@RoomID"), dwRoomID);
	LONG lResultCode=g_TreasureDB->ExecuteProcess(TEXT("GSP_CG_CLUB_QUERY_RULE"),true);
	
	if(lResultCode == DB_SUCCESS)
	{
		TCHAR ch[20];
		g_TreasureDB->GetValue_String(TEXT("RealRoomRule"), ch, 20);

		CWConvert::StrToBin2(ch, value, 0, 19);
	}

	//��ȡ���巿�����
	return GetFKRoomRule(value, TABLE_MODE_CLUB);
}
//��ȡ���᷿�����
tagClubRoomRule RoomRuleManager::GetClubRoomRule(DWORD dwClubID, DWORD dwRoomID, DWORD dwPasswd)
{
	tagClubRoomRule cfg;
	memset(&cfg, 0, sizeof(cfg));
	cfg.dwClubID = dwClubID;
	cfg.dwRoomID = dwRoomID;
	cfg.dwPasswd = dwPasswd;

	g_TreasureDB->ResetParameter();
	g_TreasureDB->AddParameter(TEXT("@ClubID"), dwClubID);
	g_TreasureDB->AddParameter(TEXT("@RoomID"), dwRoomID);
	LONG lResultCode=g_TreasureDB->ExecuteProcess(TEXT("GSP_CG_CLUB_QUERY_RULE"),true);

	if(lResultCode == DB_SUCCESS)
	{
		cfg.byGoldOrFK = g_TreasureDB->GetValue_BYTE(TEXT("ModeID"));
		cfg.bDissolve = g_TreasureDB->GetValue_BYTE(TEXT("Dissolve"));
		cfg.dwAmount = g_TreasureDB->GetValue_DWORD(TEXT("Gold"));
		cfg.dwOwnerPercentage = g_TreasureDB->GetValue_DWORD(TEXT("Revenue"));
		cfg.byMask = g_TreasureDB->GetValue_BYTE(TEXT("Mask"));
		cfg.dwDizhu = g_TreasureDB->GetValue_DWORD(TEXT("Dizhu"));
	}

	return cfg;
}

//��ȡ��ҳ� ���з�������
std::vector<STR_CMD_GC_USER_GOLD_INFO>  RoomRuleManager::GetAllRoomInfo()
{
	std::vector<STR_CMD_GC_USER_GOLD_INFO> vec;
	for(auto item : s_RoomInfo)
	{
		vec.push_back(item.second);
	}

	return vec;
}
//��ȡ��ҳ� �ض�����ȼ��� ��������
STR_CMD_GC_USER_GOLD_INFO RoomRuleManager::GetRoomInfo(BYTE level)
{
	return s_RoomInfo[level];
}


//������ ��Ʊ�Ƿ��������
int RoomRuleManager::CheckTickt(tagTableRule* rule, CPlayer* player, CTableFrame *table)
{
	switch( rule->GameMode )
	{
	case TABLE_MODE_FK://����ģʽ
		{
			int CountType = GetCountType(rule->GameCount);
			if(CountType == 0)
			{
				CLog::Log(log_error, "GetCountType error !!!!!");
				CountType = 1; //TODONOW �����ȡʧ����, ����͵Ŀ۳�
			}

			//����֧���� ����Ϊ countType * playerCount
			if (0 == rule->cbPayType)
			{
				//���뷿��
				if (table != NULL) return 0;

				//�Ƿ��� ����У�� TODONOW
				DWORD pay = (rule->PlayerCount) * CountType;
				if( player->GetUserRoomCard() < pay)
				{
					return -1;
				}
			}
			else if( 1 == rule->cbPayType) //AA֧��
			{
				DWORD pay = 1 * CountType;
				if( player->GetUserRoomCard() < pay) return -2;
			}

			break;
		}
	case TABLE_MODE_MATCH: //����
		{
			CMatchRoom *room = (CMatchRoom *)table;
			MATCH_CONFIG config = room->GetMatchItem()->GetConfig();

			if (config.FeeType == 0 && (player->GetUserGold() < config.llFee)) //���
				return -1;

			if (config.FeeType == 1 && (player->GetUserDiamond() < config.llFee)) //שʯ
				return -2;

			if (config.FeeType == 2 && (player->GetUserRoomCard() < config.llFee)) //����
				return -3;

			break;
		}
	case TABLE_MODE_GOLD: //���
		{
			//������ҽ����Ŀ
			if(player->GetUserGold()  <  GetRoomInfo(rule->GameRoomLevel).dwGold)
			{
				return -201;
			}

			break;
		}
	case TABLE_MODE_FK_GOLD://������ҳ�
		{
			break;
		}
	case TABLE_MODE_CLUB://����Ȧ
		{
			break;
		}
	default:
		{
			return -999;
		}
	}
	return 0;
}

//�����ֶ�����, Ϊ�ṹ���Ӧ�ֶθ�ֵ
void RoomRuleManager::SetRoomRule(tagTableRule &roomrule, string key_name, byte value)
{
	if(key_name == "����")
	{
		roomrule.GameCount = (BYTE)atoi(GetRoomValByKey(key_name, value).c_str());
	}
	else if(key_name == "����")
	{
		roomrule.PlayerCount = atoi(GetRoomValByKey(key_name, value).c_str());
	}
	else if(key_name == "֧��")
	{
		roomrule.cbPayType = atoi(GetRoomValByKey(key_name, value).c_str());
	}
	else if(key_name == "IP���")
	{
		roomrule.bRefuseSameIP = atoi(GetRoomValByKey(key_name, value).c_str());
	}
	else if(key_name == "����")
	{
		roomrule.bDistanceIn300 = atoi(GetRoomValByKey(key_name, value).c_str());
	}
	else if(key_name == "İ����")
	{
		roomrule.bAllowStranger = atoi(GetRoomValByKey(key_name, value).c_str());
	}
	else
	{
		m_SubRoomRuleManager->SetRoomRule(key_name, GetRoomValByKey(key_name, value));
	}
}
//�����ֶ�����, �ҵ���Ӧ���ֶ�ֵ
string RoomRuleManager::GetRoomValByKey(string key, byte value)
{
	for(int i=0; i<20; i++)
	{
		std::wstring wStr(m_rule_arry.ItemArry[i].szHeadName);
		string tempkey = CWConvert::ws2s(wStr);
		if( tempkey == key)
		{
			std::wstring wVal(m_rule_arry.ItemArry[i].szItemValue[value]);
			string val = CWConvert::ws2s(wVal);
			return val;
		}
	}

	return NULL;
}
//�жϵ�ǰ��ѡ����, �ǵڴ���
int RoomRuleManager::GetCountType(byte value)
{
	for(int i=0; i<20; i++)
	{
		std::wstring wStr(m_rule_arry.ItemArry[i].szHeadName);
		string tempkey = CWConvert::ws2s(wStr);		
		if( tempkey == "��Ϸ����")
		{
			for(int j=0; j<4; j++)
			{
				std::wstring wVal(m_rule_arry.ItemArry[i].szItemValue[j]);
				string val = CWConvert::ws2s(wVal);
				int iVal = atoi(val.c_str());
				if(iVal == value)
				{
					return j+1;
				}
			}
		}
	}

	return 0;
}

//����value -> DB�ַ���
string RoomRuleManager::GetRuleHexString(BYTE vlaue[20])
{
	return  CWConvert::toHexString2(vlaue, 20);
}