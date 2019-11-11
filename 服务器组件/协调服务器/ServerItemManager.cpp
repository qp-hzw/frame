#include "ServerItemManager.h"

std::vector<tagServerItem*>     CServerItemManager::s_server_list;

//增
int CServerItemManager::InsertItem(tagServerItem &item)
{
	//校验是否存在
	tagServerItem *pItem = FindItemBySocketID(item.dwSocketID);
	if(pItem != NULL)
	{
		return -1; //已存在相同的
	}

	pItem = new tagServerItem();
	memcpy(pItem, &item, sizeof(tagServerItem));
	s_server_list.push_back(pItem);

	return true;
}
//删
bool CServerItemManager::DeleteItem(DWORD dwSocketID)
{
	tagServerItem* item = FindItemBySocketID(dwSocketID);
	if(item == NULL) return true;

	auto ite = find(s_server_list.begin(), s_server_list.end(), item);
	if(ite != s_server_list.end())
	{
		s_server_list.erase(ite);
	}

	return true;
}

//查
tagServerItem* CServerItemManager::FindItemBySocketID(DWORD dwSocketID)
{
	for(auto item : s_server_list)
	{
		if( (item != NULL) && (item->dwSocketID == dwSocketID))
			return item;
	}

	return NULL;
}
//查
tagServerItem* CServerItemManager::FindItemByServerID(DWORD dwServerID)
{
	for(auto item : s_server_list)
	{
		if( (item != NULL) && (item->dwServerID == dwServerID))
			return item;
	}

	return NULL;
}
//查
tagServerItem* CServerItemManager::FindItemByKindID(DWORD dwKindID)
{
	for(auto item : s_server_list)
	{
		if(item == NULL) continue;
		if( ((item->dwServerID  &0x00FF0000 ) >> 16 )== dwKindID )
			return item;
	}

	return NULL;
}

//查所有
std::vector<tagServerItem*> CServerItemManager::FindAllGameServer()
{
	std::vector<tagServerItem*> vec_server;
	vec_server.clear();

	for(auto item : s_server_list)
	{
		if(item->byServerType == GAME_TYPE)
			vec_server.push_back(item);
	}

	return vec_server;
}
//查所有
std::vector<tagServerItem*> CServerItemManager::FindAllLobbyServer()
{
	std::vector<tagServerItem*> vec_server;
	vec_server.clear();

	for(auto item : s_server_list)
	{
		if(item->byServerType == LOBBY_TYPE)
			vec_server.push_back(item);
	}

	return vec_server;
}