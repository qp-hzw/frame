#include "RoomRuleManager.h"
#include <iostream>

rule_arry                   RoomRuleManager::m_rule_arry;							//�������
BYTE						RoomRuleManager::m_frame_rule_count = 0;				//frame������� ����
BYTE						RoomRuleManager::m_subgame_rule_count = 0;				//����Ϸ������� ����

//TCHAR 2 string
std::string TCHAR2STRING(TCHAR *STR)
{	
	int iLen = WideCharToMultiByte(CP_ACP, 0,STR, -1, NULL, 0, NULL, NULL);
	char* chRtn =new char[iLen*sizeof(char)];
	WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, NULL, NULL);
	std::string str(chRtn);
	return str;
}

//��ȡ�������
tagTableRule RoomRuleManager::GetRoomRule(byte value[20])
{
	tagTableRule roomRule;
	for(int i=0; i<20; i++)
	{
		TCHAR temp[15];
		memcpy(temp, m_rule_arry.ItemArry[i].szHeadName, 15*sizeof(TCHAR));
		string key_name = TCHAR2STRING(temp);
		if(!key_name.empty())
		{
			SetRoomRule(roomRule, key_name, value[i]);
		}
	}

	return roomRule;
}

//��ȡͨ�÷��������ļ�
void RoomRuleManager::ReadFrameRoomRule()
{
	ZeroMemory(&m_rule_arry, sizeof(m_rule_arry));

	memcpy(m_rule_arry.ItemArry[0].szHeadName, TEXT("GameCount"), 15*sizeof(TCHAR));
	memcpy(m_rule_arry.ItemArry[0].szItemValue[0], TEXT("1"), 10*sizeof(TCHAR));
	memcpy(m_rule_arry.ItemArry[0].szItemValue[1], TEXT("2"), 10*sizeof(TCHAR));
	memcpy(m_rule_arry.ItemArry[0].szItemValue[2], TEXT("8"), 10*sizeof(TCHAR));
	
	memcpy(m_rule_arry.ItemArry[1].szHeadName, TEXT("PlayerCount"), 15*sizeof(TCHAR));
	memcpy(m_rule_arry.ItemArry[1].szItemValue[0], TEXT("2"), 10*sizeof(TCHAR));
	memcpy(m_rule_arry.ItemArry[1].szItemValue[1], TEXT("3"), 10*sizeof(TCHAR));
	memcpy(m_rule_arry.ItemArry[1].szItemValue[2], TEXT("4"), 10*sizeof(TCHAR));
	return;


	//��ʼ������
	ZeroMemory(&m_rule_arry, sizeof(m_rule_arry));
	m_frame_rule_count = 0;
	m_subgame_rule_count = 0;

	//���ļ�
	string file_path = "frame.rule";
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
		std::cout << strTemp << std::endl;
		std::cout << strTemp.c_str() << std::endl;

		m_frame_rule_count ++;
		//swprintf(m_rule_arry.ItemArry[i].szHeadName, 15, L"%S", strTemp.c_str());

		for(int j=0; j<4; j++)
		{
			char value[20];
			sprintf(value, "value_%d", j);
			iRet = CWHCfg::Instance()->GetItemValue(psz, value, strTemp);
			if(iRet != 0) continue;
			//swprintf(m_rule_arry.ItemArry[i].szItemValue[j], 10, L"%S", strTemp.c_str());
		}
	}

	//�ر��ļ�
	CWHCfg::Instance()->CloseFile();
}
//��ȡ����Ϸ���������ļ�
void RoomRuleManager::ReadSubGameRoomRule(int kindid)
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
		m_subgame_rule_count ++;
		//swprintf(m_rule_arry.ItemArry[i].szHeadName, 15, L"%S", strTemp.c_str());

		for(int j=0; j<4; j++)
		{
			char value[20];
			sprintf(value, "value_%d", j);
			iRet = CWHCfg::Instance()->GetItemValue(psz, value, strTemp);
			if(iRet != 0) continue;
			//swprintf(m_rule_arry.ItemArry[i].szItemValue[j], 10, L"%S", strTemp.c_str());
		}
	}

	//�ر��ļ�
	CWHCfg::Instance()->CloseFile();
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

	return describe;
}

//�����ֶ�����, Ϊ�ṹ���Ӧ�ֶθ�ֵ
void RoomRuleManager::SetRoomRule(tagTableRule &roomrule, string key_name, byte value)
{
	std::cout << key_name.c_str() << std::endl;

	if(key_name == "GameCount")
	{
		roomrule.GameCount = std::stoi(GetRoomValByKey(key_name, value));
	}
	else if(key_name == "PlayerCount")
	{
		roomrule.PlayerCount = std::stoi(GetRoomValByKey(key_name, value));
	}
	else if(key_name == "cbPayType")
	{
		roomrule.cbPayType = std::stoi(GetRoomValByKey(key_name, value));
	}
	else if(key_name == "bRefuseSameIP")
	{
		roomrule.bRefuseSameIP = std::stoi(GetRoomValByKey(key_name, value));
	}
	else if(key_name == "bDistanceIn300")
	{
		roomrule.bDistanceIn300 = std::stoi(GetRoomValByKey(key_name, value));
	}
	else if(key_name == "bAllowStranger")
	{
		roomrule.bAllowStranger = std::stoi(GetRoomValByKey(key_name, value));
	}
}

//�����ֶ�����, �ҵ���Ӧ���ֶ�ֵ
string RoomRuleManager::GetRoomValByKey(string key, byte value)
{
	if(value >=4 ) return NULL;

	for(int i=0; i<20; i++)
	{
		string tempkey = TCHAR2STRING(m_rule_arry.ItemArry[i].szHeadName);
		if( tempkey == key)
		{
			string val = TCHAR2STRING(m_rule_arry.ItemArry[i].szItemValue[value]);
			return val;
		}
	}

	return NULL;
}
