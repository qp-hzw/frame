#include "Stdafx.h"
#include "TableManager.h"
#include "TableFrame.h"
#include <thread>
#include <chrono>
#include "GameCtrl.h"

using namespace std;

std::list<CTableFrame*>	               CTableManager::s_TableArray;					//桌子数组

//增
CTableFrame* CTableManager::CreateTable()
{
	//获取房间号
	srand(static_cast<unsigned int >(time(NULL)));
	DWORD dwPassword = GenerateTablePassword();
	if(dwPassword == 0) return NULL;

	//构建
	CTableFrame *pTableFrame = new CTableFrame();
	if (pTableFrame == NULL)
		return false;

	//设置房间
	pTableFrame->SetGameStatus(GAME_STATUS_FREE);
	pTableFrame->SetTableID(dwPassword);

	//设置房间自动解散，默认一分钟 -- 这里是指不开始游戏 自动一分钟后解散
	//pTableFrame->SetTableAutoDismiss();

	//加入vector
	s_TableArray.push_back(pTableFrame);

	return pTableFrame;
}

//删
bool CTableManager::DeleteTable(CTableFrame* pTable) 
{
	if (pTable == NULL) return false;

	for (auto ite = s_TableArray.begin(); ite != s_TableArray.end(); ite++)
	{
		if (*ite == pTable)
		{
			CLog::Log(log_debug, "Delete Table : %d", pTable->GetTableID() );
			ite = s_TableArray.erase(ite);
			//delete pTable;
			pTable = NULL;
			break;
		}
	}
	
	return true;
}

//删
bool CTableManager::DeleteTable(DWORD dwTableID)
{
	for (auto ite = s_TableArray.begin(); ite != s_TableArray.end(); ite++)
	{
		if (*ite  && ((*ite)->GetTableID() == dwTableID))
		{
			delete *ite;
			*ite = NULL;
			ite = s_TableArray.erase(ite);
			break;
		}
	}

	return true;
}

//删 所有
bool CTableManager::DeleteAllTable()
{
	for (auto ite = s_TableArray.begin(); ite != s_TableArray.end(); ite++)
	{
		delete *ite;
		*ite = NULL;
	}

	s_TableArray.clear();
	return true;
}

//查
CTableFrame* CTableManager::FindTableByTableID(DWORD dwTableID)
{
	for (auto ite = s_TableArray.begin(); ite != s_TableArray.end(); ite++)
	{
		if (*ite  && ((*ite)->GetTableID() == dwTableID))
		{
			return *ite;
		}
	}

	return NULL;
}

//桌子总数
DWORD CTableManager::TableCount()
{
	return s_TableArray.size();
}

//生成桌子ID，即加入房间密码
DWORD CTableManager::GenerateTablePassword()
{	
	//生成密码
	srand(static_cast<unsigned int >(time(NULL)));
	BYTE byHigh1 = 1+  rand() % 219 + 24;
	BYTE byHigh2 = ((g_GameCtrl->GetServerID()) >> 16) &  0xFF;
	BYTE byHign3 = ((g_GameCtrl->GetServerID()) >> 8) & 0xF;
	DWORD dwPassword = (byHigh1 << 12) + (byHigh2 <<4) + byHign3;

	int times = 0;
	//判断桌子密码是否存在
	while(true)
	{
		bool bFind = false;
		for( auto table : s_TableArray)
		{
			if(!table) continue;
			if(dwPassword == table->GetTableID())
			{
				bFind = true;
				break;
			}
		}

		//若有桌子已经用到该密码，重置密码	TODONOW 不能再用原来的生成方式
		if(bFind) 
		{
			byHigh1 = 1+  rand() % 219 + 24;
			dwPassword = (byHigh1 << 12) + (byHigh2 <<4) + byHign3;
			times ++;

			if(times >= 100)
			{
				return 0;
			}
		}
		else
		{
			return dwPassword;
		}
	}
}

//查找金币场桌子 byType: 初级, 中级, 高级场次
CTableFrame* CTableManager::GetGlodTable(BYTE byType)
{
	//变量定义
	CTableFrame *pTableFrameReturn = NULL;

	//寻找金币房空椅子
	for(auto pTableFrame : s_TableArray)
	{
		if ( (NULL == pTableFrame) || 
			(pTableFrame->GetGameStatus() != GAME_STATUS_FREE) || 
			(pTableFrame->GetTableMode() != TABLE_MODE_GOLD) ||
			(pTableFrame->GetGoldType() != byType))
					continue;
		
		pTableFrameReturn = pTableFrame;
		break;
	}

	if(pTableFrameReturn == NULL)
	{
		pTableFrameReturn = CreateTable();
		if(pTableFrameReturn == NULL)
		{
			CLog::Log(log_error, "GetGlodTable  type:%d  failed", byType);
		}

		//设置房间规则
		tagTableRule roomRule;
		roomRule.GameMode = TABLE_MODE_GOLD;
		roomRule.GameCount = 1;
		roomRule.PlayerCount =3;
		pTableFrameReturn->SetCommonRule(&roomRule);
	}

	return pTableFrameReturn;
}

//查找所有金币场桌子
std::vector<CTableFrame*> CTableManager::GetAllGlodTable()
{
	std::vector<CTableFrame*> glod_talbe_array;
	for(auto item : s_TableArray)
	{
		if(item && 
			(item->GetGameStatus() == GAME_STATUS_FREE) &&
			(item->GetTableMode() == TABLE_MODE_GOLD)	&&
			item->GetSitUserCount() < item->GetChairCount())
		{
			glod_talbe_array.push_back(item);
		}
	}

	return glod_talbe_array;
}