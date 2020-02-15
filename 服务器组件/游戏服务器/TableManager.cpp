#include "Stdafx.h"
#include "TableManager.h"
#include "TableFrame.h"
#include <thread>
#include <chrono>
#include "GameCtrl.h"
#include "RoomRuleManager.h"
#include "DataBaseEngineSink.h"

using namespace std;

std::list<CTableFrame*>	               CTableManager::s_TableArray;					//��������

//��
CTableFrame* CTableManager::CreateTable(tagTableRule* cfg, DWORD fangzhu)
{
	//��ȡ�����
	srand(static_cast<unsigned int >(time(NULL)));
	DWORD dwPassword = GenerateTablePassword();
	if(dwPassword == 0) return NULL;

	//����
	CTableFrame *pTableFrame = new CTableFrame();
	if (pTableFrame == NULL)
		return false;

	//���÷���
	pTableFrame->SetGameStatus(GAME_STATUS_FREE);
	pTableFrame->SetTableID(dwPassword);
	pTableFrame->SetCommonRule(cfg);
	pTableFrame->SetTableOwner(fangzhu);

	//���÷����Զ���ɢ��Ĭ��һ���� -- ������ָ����ʼ��Ϸ �Զ�һ���Ӻ��ɢ
	//pTableFrame->SetTableAutoDismiss();

	//����vector
	s_TableArray.push_back(pTableFrame);

	return pTableFrame;
}

//�� ������
CMatchRoom* CTableManager::CreateMatchRoom(CMatchItem* Item, WORD stage)
{
	if (Item == NULL)
		return NULL;

	//��ȡ�����
	srand(static_cast<unsigned int >(time(NULL)));
	DWORD dwPassword = GenerateTablePassword();
	if(dwPassword == 0) return NULL;

	//����
	CMatchRoom *pMatch = new CMatchRoom(Item, stage);
	if (pMatch == NULL)
		return NULL;

	//���÷���
	pMatch->SetGameStatus(GAME_STATUS_FREE);
	pMatch->SetTableID(dwPassword);

	s_TableArray.push_back(pMatch);

	return pMatch;
}

//�� ��������
CTableFrame* CTableManager::CreateClubTable(tagTableRule* cfg, tagClubRoomRule* clubCfg)
{
	//��ȡ�����
	srand(static_cast<unsigned int >(time(NULL)));
	DWORD dwTableID = GenerateTablePassword();
	if(dwTableID == 0) return NULL;

	//����
	CTableFrame *pTableFrame = new CTableFrame();
	if (pTableFrame == NULL)
		return false;

	//���÷���
	pTableFrame->SetGameStatus(GAME_STATUS_FREE);
	pTableFrame->SetTableID(dwTableID);
	pTableFrame->SetCommonRule(cfg);
	pTableFrame->SetClubRoomRule(clubCfg);

	//���÷����Զ���ɢ��Ĭ��һ���� -- ������ָ����ʼ��Ϸ �Զ�һ���Ӻ��ɢ
	//pTableFrame->SetTableAutoDismiss();

	//����vector
	s_TableArray.push_back(pTableFrame);

	return pTableFrame;
}

//ɾ
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

//ɾ
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

//ɾ ����
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

//��
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

//��������
DWORD CTableManager::TableCount()
{
	return s_TableArray.size();
}

//��������ID�������뷿������
DWORD CTableManager::GenerateTablePassword()
{	
	//��������
	srand(static_cast<unsigned int >(time(NULL)));
	BYTE byHigh1 = 1+  rand() % 219 + 24;
	BYTE byHigh2 = ((g_GameCtrl->GetServerID()) >> 16) &  0xFF;
	BYTE byHign3 = ((g_GameCtrl->GetServerID()) >> 8) & 0xF;
	DWORD dwPassword = (byHigh1 << 12) + (byHigh2 <<4) + byHign3;

	int times = 0;
	//�ж����������Ƿ����
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

		//���������Ѿ��õ������룬��������	TODONOW ��������ԭ�������ɷ�ʽ
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

//���ҽ�ҳ����� byType: ����, �м�, �߼�����
CTableFrame* CTableManager::GetGlodTable(BYTE byType)
{
	//��������
	CTableFrame *pTableFrameReturn = NULL;

	//Ѱ�ҽ�ҷ�������
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
		pTableFrameReturn = CreateTable(&(RoomRuleManager::GetGoldRoomRule(byType)), 0);
		if(pTableFrameReturn == NULL)
		{
			CLog::Log(log_error, "GetGlodTable  type:%d  failed", byType);
		}
	}

	return pTableFrameReturn;
}

//�������н�ҳ�����
std::list<CTableFrame*> CTableManager::GetAllGlodTable()
{
	std::list<CTableFrame*> glod_talbe_array;
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

//�������й��� ������ �����б�
std::list<CTableFrame*> CTableManager::GetAllClubRoomTable(DWORD dwClubID, DWORD dwRoomID)
{
	std::list<CTableFrame*> club_room_table_array;
	for(auto item : s_TableArray)
	{
		if(item)
		{
			tagClubRoomRule *rule = item->GetClubRoomRule();
			if( (rule->dwClubID == dwClubID)  &&
				(rule->dwRoomID == dwRoomID))
			{
				club_room_table_array.push_back(item);
			}
		}
	}

	return club_room_table_array;
}