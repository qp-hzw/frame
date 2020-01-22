#include "Stdafx.h"
#include "TableManager.h"
#include "TableFrame.h"
#include <thread>
#include <chrono>
using namespace std;

std::vector<CTableFrame*>	               CTableManager::s_TableArray;					//��������

//��
CTableFrame* CTableManager::CreateTable()
{
	//����
	CTableFrame *pTableFrame = NULL;

	//new
	pTableFrame = new CTableFrame();
	if (pTableFrame == NULL)
		return false;

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
			ite = s_TableArray.erase(ite);
			delete pTable;
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

//��
CTableFrame* CTableManager::FindTableByIndex(DWORD dwIndex)
{
	if(dwIndex >= s_TableArray.size()) return NULL;

	return s_TableArray.at(dwIndex);
}

//��������
DWORD CTableManager::TableCount()
{
	return s_TableArray.size();
}