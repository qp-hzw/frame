#include "Stdafx.h"
#include "TableManager.h"
#include "TableFrame.h"
#include <thread>
#include <chrono>
using namespace std;

std::vector<CTableFrame*>	               CTableManager::s_TableArray;					//桌子数组

void DelayDeleteTable(CTableFrame * pTable)
{
	if(pTable == NULL) return;
	std::this_thread::sleep_for(chrono::milliseconds(3000));

	delete pTable;
}

//增
CTableFrame* CTableManager::CreateTable()
{
	//构建
	CTableFrame *pTableFrame = NULL;

	//new
	pTableFrame = new CTableFrame();
	if (pTableFrame == NULL)
		return false;

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
			ite = s_TableArray.erase(ite);
			break;
		}
	}

	std::thread t2(DelayDeleteTable, pTable);
	t2.detach();
	return true;
}

//删
bool CTableManager::DeleteTable(DWORD dwTableID)
{
	for (auto ite = s_TableArray.begin(); ite != s_TableArray.end(); ite++)
	{
		if (*ite  && ((*ite)->GetTableID() == dwTableID))
		{
			ite = s_TableArray.erase(ite);
			break;
		}
	}

	return true;
}

//删 所有
bool CTableManager::DeleteAllTable()
{
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

//查
CTableFrame* CTableManager::FindTableByIndex(DWORD dwIndex)
{
	if(dwIndex >= s_TableArray.size()) return NULL;

	return s_TableArray.at(dwIndex);
}

//桌子总数
DWORD CTableManager::TableCount()
{
	return s_TableArray.size();
}