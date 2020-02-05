#include "stdafx.h"
#include "RoomRuleManager.h"
#include <iostream>
#include <stdlib.h>

RoomRuleManager* RoomRuleManager::_instance =NULL;

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
TCHAR *chr2wch(const char *buffer)
{
        size_t len = strlen(buffer);
        size_t wlen = MultiByteToWideChar(CP_ACP, 0, (const char*)buffer, int(len), NULL, 0);
        TCHAR *wBuf = new TCHAR[wlen + 1];
        MultiByteToWideChar(CP_ACP, 0, (const char*)buffer, int(len), wBuf, int(wlen));
        return wBuf;
}

//获取实例
RoomRuleManager* RoomRuleManager::Instance()
{
	if(_instance != NULL) return _instance;

	_instance = new RoomRuleManager();
	_instance->m_SubRoomRuleManager = static_cast<ISubRoomRuleManager*> (CWHModule::GetSubRuleManager("test.dll"));
	memset(&(_instance->m_rule_arry), 0, sizeof(_instance->m_rule_arry));

	if(_instance->m_SubRoomRuleManager == NULL)
	{
		CLog::Log(log_error, "RoomRuleManager:: m_SubRoomRuleManager is null");
	}

	return _instance;
}
//获取房间配置选项
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
		memcpy(szTchar, chr2wch(clientHead.c_str()), sizeof(TCHAR)*15);
		memcpy(client_rule_item.ItemArry[i].szHeadName, szTchar, sizeof(TCHAR)*15);
	}

	return client_rule_item;
}
//获取房间规则
void RoomRuleManager::GetRoomRule(tagTableRule& roomRule, byte value[20])
{
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
}

//设置金币场规则
void RoomRuleManager::SetGoldRule(tagTableRule& roomRule, BYTE byType)
{
	//数据初始化
	m_SubRoomRuleManager->Init();
	m_SubRoomRuleManager->SetGoldRule(byType);

	roomRule.GameMode = TABLE_MODE_GOLD;
	roomRule.GameCount = 1;
	roomRule.PlayerCount =3;
	roomRule.dwAmount = (byType == 1) ? 1000 : ((byType == 2) ? 2000 : ((byType == 3) ? 4000 : 10000));
}

//读取子游戏房间配置文件
void RoomRuleManager::Init(int kindid)
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
		m_subgame_rule_count ++;
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
