#include "stdafx.h"
#include "RoomRuleManager.h"
#include <iostream>
#include <stdlib.h>
#include "player.h"

rule_arry						RoomRuleManager::m_rule_arry;
ISubRoomRuleManager*            RoomRuleManager::m_SubRoomRuleManager = NULL;
std::map<BYTE,  STR_CMD_GC_USER_GOLD_INFO> RoomRuleManager::s_RoomInfo;

//初始化
void RoomRuleManager::Init(int kindid)
{
	InitFK(kindid);
	InitGold(kindid);
}
//初始化 房卡场 
void RoomRuleManager::InitFK(int kindid)
{
	//打开文件
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
		swprintf(m_rule_arry.ItemArry[i].szHeadName, 15, L"%S", strTemp.c_str());

		for(int j=0; j<4; j++)
		{
			char value[20];
			sprintf(value, "value_%d", j);
			iRet = CWHCfg::Instance()->GetItemValue(psz, value, strTemp);
			if(iRet != 0) continue;
			swprintf(m_rule_arry.ItemArry[i].szItemValue[j], 10, L"%S", strTemp.c_str());
		}
	}

	//关闭文件
	CWHCfg::Instance()->CloseFile();
}
//初始化 金币场
void RoomRuleManager::InitGold(int kindid)
{
	//打开文件
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

	//关闭文件
	CWHCfg::Instance()->CloseFile();
}

//TCHAR 2 string
std::string TCHAR2STRING(TCHAR *STR)
{	
	int iLen = WideCharToMultiByte(CP_ACP, 0,STR, -1, NULL, 0, NULL, NULL);
	char* chRtn =new char[iLen*sizeof(char)];
	WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, NULL, NULL);
	std::string str(chRtn);
	return str;
}
//char* 2 TCHAR
TCHAR *chrTOwch(const char *buffer)
{
        size_t len = strlen(buffer);
        size_t wlen = MultiByteToWideChar(CP_ACP, 0, (const char*)buffer, int(len), NULL, 0);
        TCHAR *wBuf = new TCHAR[wlen + 1];
        MultiByteToWideChar(CP_ACP, 0, (const char*)buffer, int(len), wBuf, int(wlen));
        return wBuf;
}

//获取房卡场 房间配置选项
rule_arry RoomRuleManager::GetRoomRuleSetting()
{
	rule_arry client_rule_item;
	memcpy(&client_rule_item, &m_rule_arry, sizeof(rule_arry));
	for(int i =0; i < 20; i ++)
	{
		TCHAR szTemp[15];
		memcpy(szTemp, m_rule_arry.ItemArry[i].szHeadName, sizeof(TCHAR) *15);
		string clientHead = GetDescribe(TCHAR2STRING(szTemp));
		if(clientHead.empty()) continue;

		TCHAR szTchar[15] ; 
		memcpy(szTchar, chrTOwch(clientHead.c_str()), sizeof(TCHAR)*15);
		memcpy(client_rule_item.ItemArry[i].szHeadName, szTchar, sizeof(TCHAR)*15);
	}

	return client_rule_item;
}
//获取房卡场 房间规则
tagTableRule RoomRuleManager::GetFKRoomRule(byte value[20], byte GameMode)
{
	tagTableRule roomRule;
	ZeroMemory(&roomRule, sizeof(roomRule));
	roomRule.GameMode = GameMode;

	//数据初始化
	m_SubRoomRuleManager->Init();

	for(int i=0; i<20; i++)
	{
		TCHAR temp[15];
		memcpy(temp, m_rule_arry.ItemArry[i].szHeadName, 15*sizeof(TCHAR));
		string key_name = TCHAR2STRING(temp);
		if(key_name.empty())
		{
			continue;
		}

		if(value[i] >= 4) continue;

		SetRoomRule(roomRule, key_name, value[i]);
	}

	return roomRule;
}


//获取金币场 房间规则
tagTableRule RoomRuleManager::GetGoldRoomRule(BYTE byType)
{
	//数据初始化
	m_SubRoomRuleManager->Init();
	m_SubRoomRuleManager->SetGoldRule(byType);

	tagTableRule roomRule;
	ZeroMemory(&roomRule, sizeof(tagTableRule));

	roomRule.GameMode = TABLE_MODE_GOLD;
	roomRule.GameRoomLevel = byType;
	roomRule.GameCount = 1;
	roomRule.PlayerCount =3;
	return roomRule;
}
//获取金币场 所有房间数据
std::vector<STR_CMD_GC_USER_GOLD_INFO>  RoomRuleManager::GetAllRoomInfo()
{
	std::vector<STR_CMD_GC_USER_GOLD_INFO> vec;
	for(auto item : s_RoomInfo)
	{
		vec.push_back(item.second);
	}

	return vec;
}
//获取金币场 特定房间等级的 房间数据
STR_CMD_GC_USER_GOLD_INFO RoomRuleManager::GetRoomInfo(BYTE level)
{
	return s_RoomInfo[level];
}


//检测玩家 门票是否符合条件
int RoomRuleManager::CheckTickt(tagTableRule* rule, CPlayer* player)
{
	switch( rule->GameMode )
	{
	case TABLE_MODE_FK://房卡模式
		{
			int CountType = GetCountType(rule->GameCount);
			if(CountType == 0)
			{
				CLog::Log(log_error, "GetCountType error !!!!!");
				CountType = 1; //TODONOW 如果读取失败了, 则按最低的扣除
			}

			//房主支付， 房费为 countType * playerCount
			if (0 == rule->cbPayType)
			{
				//非房主 无须校验 TODONOW
				DWORD pay = (rule->PlayerCount) * CountType;
				if( player->GetUserRoomCard() < pay)
				{
					return -1;
				}
			}
			else if( 1 == rule->cbPayType) //AA支付
			{
				DWORD pay = 1 * CountType;
				if( player->GetUserRoomCard() < pay) return -2;
			}

			break;
		}
	case TABLE_MODE_MATCH: //比赛
		{
			break;
		}
	case TABLE_MODE_GOLD: //金币
		{
			//检验玩家金币数目
			if(player->GetUserGold()  <  GetRoomInfo(rule->GameRoomLevel).dwGold)
			{
				return -201;
			}

			break;
		}
	case TABLE_MODE_FK_GOLD://房卡金币场
		{
			break;
		}
	case TABLE_MODE_CLUB://牌友圈
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

//获取字段 对应的描述
string RoomRuleManager::GetDescribe(string key_name)
{
	string describe;
	if(key_name == "GameCount")
	{
		describe = "局数";
	}
	else if(key_name == "PlayerCount")
	{
		describe = "人数";
	}
	else if(key_name == "cbPayType")
	{
		describe = "支付模式";
	}
	else if(key_name == "bRefuseSameIP")
	{
		describe = "允许同IP";
	}
	else if(key_name == "bDistanceIn300")
	{
		describe = "允许300米";
	}
	else if(key_name == "bAllowStranger")
	{
		describe = "允许陌生人加入";
	}
	else 
	{
		std::cout << "key_name: " << key_name << std::endl;
		return m_SubRoomRuleManager->GetDescribe(key_name); 
	}

	return describe;
}
//根据字段名字, 为结构体对应字段赋值
void RoomRuleManager::SetRoomRule(tagTableRule &roomrule, string key_name, byte value)
{
	if(key_name == "GameCount")
	{
		roomrule.GameCount = (BYTE)atoi(GetRoomValByKey(key_name, value).c_str());
	}
	else if(key_name == "PlayerCount")
	{
		roomrule.PlayerCount = atoi(GetRoomValByKey(key_name, value).c_str());
	}
	else if(key_name == "cbPayType")
	{
		roomrule.cbPayType = atoi(GetRoomValByKey(key_name, value).c_str());
	}
	else if(key_name == "bRefuseSameIP")
	{
		roomrule.bRefuseSameIP = atoi(GetRoomValByKey(key_name, value).c_str());
	}
	else if(key_name == "bDistanceIn300")
	{
		roomrule.bDistanceIn300 = atoi(GetRoomValByKey(key_name, value).c_str());
	}
	else if(key_name == "bAllowStranger")
	{
		roomrule.bAllowStranger = atoi(GetRoomValByKey(key_name, value).c_str());
	}
	else
	{
		m_SubRoomRuleManager->SetRoomRule(key_name, GetRoomValByKey(key_name, value));
	}
}
//根据字段名字, 找到对应的字段值
string RoomRuleManager::GetRoomValByKey(string key, byte value)
{
	for(int i=0; i<20; i++)
	{
		TCHAR szTemp[15];
		memcpy(szTemp, m_rule_arry.ItemArry[i].szHeadName, sizeof(TCHAR) * 15);
		string tempkey = TCHAR2STRING(szTemp);
		if( tempkey == key)
		{
			TCHAR szVal[10];
			memcpy(szVal, m_rule_arry.ItemArry[i].szItemValue[value], sizeof(TCHAR) * 10);
			string val = TCHAR2STRING(szVal);
			return val;
		}
	}

	return NULL;
}
//判断当前所选局数, 是第大类
int RoomRuleManager::GetCountType(byte value)
{
	for(int i=0; i<20; i++)
	{
		TCHAR szTemp[15];
		memcpy(szTemp, m_rule_arry.ItemArry[i].szHeadName, sizeof(TCHAR) * 15);
		string tempkey = TCHAR2STRING(szTemp);
		if( tempkey == "GameCount")
		{
			for(int j=0; j<4; j++)
			{
				TCHAR szVal[10];
				memcpy(szVal, m_rule_arry.ItemArry[i].szItemValue[j], sizeof(TCHAR) * 10);
				string val = TCHAR2STRING(szVal);
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
