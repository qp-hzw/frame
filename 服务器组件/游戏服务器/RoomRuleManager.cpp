#include "stdafx.h"
#include "RoomRuleManager.h"
#include <iostream>
#include <stdlib.h>
#include "player.h"

rule_arry						RoomRuleManager::m_rule_arry;
ISubRoomRuleManager*            RoomRuleManager::m_SubRoomRuleManager = NULL;
std::map<BYTE,  STR_CMD_GC_USER_GOLD_INFO> RoomRuleManager::s_RoomInfo;

//��ʼ��
void RoomRuleManager::Init(int kindid)
{
	InitFK(kindid);
	InitGold(kindid);
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

//��ȡ������ ��������ѡ��
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


//��ȡ��ҳ� �������
tagTableRule RoomRuleManager::GetGoldRoomRule(BYTE byType)
{
	//���ݳ�ʼ��
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
int RoomRuleManager::CheckTickt(tagTableRule* rule, CPlayer* player)
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

//��ȡ�ֶ� ��Ӧ������
string RoomRuleManager::GetDescribe(string key_name)
{
	string describe;
	if(key_name == "GameCount")
	{
		describe = "����";
	}
	else if(key_name == "PlayerCount")
	{
		describe = "����";
	}
	else if(key_name == "cbPayType")
	{
		describe = "֧��ģʽ";
	}
	else if(key_name == "bRefuseSameIP")
	{
		describe = "����ͬIP";
	}
	else if(key_name == "bDistanceIn300")
	{
		describe = "����300��";
	}
	else if(key_name == "bAllowStranger")
	{
		describe = "����İ���˼���";
	}
	else 
	{
		std::cout << "key_name: " << key_name << std::endl;
		return m_SubRoomRuleManager->GetDescribe(key_name); 
	}

	return describe;
}
//�����ֶ�����, Ϊ�ṹ���Ӧ�ֶθ�ֵ
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
//�����ֶ�����, �ҵ���Ӧ���ֶ�ֵ
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
//�жϵ�ǰ��ѡ����, �ǵڴ���
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
