#include "Stdafx.h"
#include "PlayerManager.h"
#include "Player.h"
#include "GameCtrl.h"
#include <thread>
#include <chrono>
using namespace std;

std::vector<CPlayer*> CPlayerManager::s_PlayerArray;

void CloseSocket(DWORD socketID)
{
	CLog::Log(log_debug, "延迟 删除 玩家 %d", socketID);
	std::this_thread::sleep_for(chrono::milliseconds(3000));

	g_TCPNetworkEngine->CloseSocket(socketID);
}
//增
bool CPlayerManager::InsertPlayer(DWORD dwSocketID, tagUserInfo & UserInfo)
{
	CPlayer * pPlayer=NULL;
	try
	{
		pPlayer=new CPlayer(UserInfo);
		if (pPlayer == NULL)
			throw;

		pPlayer->SetSocketID(dwSocketID);
	}
	catch (...)
	{
		CLog::Log(log_error, "new CPlayer failed;");
		return false;
	}

	//插入用户
	s_PlayerArray.push_back(pPlayer);

	return true;
}

//删
bool CPlayerManager::DeletePlayer(CPlayer * pPlayer)
{
	if (pPlayer==NULL) return false;

	//关闭socket连接
	std::thread t1(CloseSocket, pPlayer->GetSocketID());
	//t1.join();

	//删除list列表
	for(auto ite = s_PlayerArray.begin(); ite != s_PlayerArray.end(); ite++)
	{
		if(*ite == pPlayer)
		{
			ite = s_PlayerArray.erase(ite);
			break;
		}
	}

	return true;
}

//删
bool CPlayerManager::DeletePlayerByID(DWORD dwUserID)
{
	//关闭连接
	CPlayer *player =  FindPlayerByID(dwUserID);
	if(player != NULL)
	{
		//关闭socket连接
		std::thread t1(CloseSocket, player->GetSocketID());
		//t1.join();
	}

	//删除list
	for(auto ite = s_PlayerArray.begin(); ite != s_PlayerArray.end(); ite++)
	{
		if( (*ite)->GetUserID() == dwUserID)
		{
			ite = s_PlayerArray.erase(ite);
			break;
		}
	}

	return true;
}

//删
bool CPlayerManager::DeletePlayerBySocketID(DWORD dwSocketID)
{
	CPlayer *player =  FindPlayerBySocketID(dwSocketID);
	if(player != NULL)
	{
		//关闭socket连接
		std::thread t1(CloseSocket, player->GetSocketID());
		//t1.join();
	}

	for(auto ite = s_PlayerArray.begin(); ite != s_PlayerArray.end(); ite++)
	{
		if( (*ite)->GetSocketID() == dwSocketID)
		{
			ite = s_PlayerArray.erase(ite);
			break;
		}
	}

	return true;
}

//删 所有
bool CPlayerManager::DeleteAllPlayer()
{
	//关闭连接
	for(auto ite = s_PlayerArray.begin(); ite != s_PlayerArray.end(); ite++)
	{
		if(*ite)
		{
			//关闭socket连接
			std::thread t1(CloseSocket, (*ite)->GetSocketID());
			//t1.join();
		}
	}

	//删除list
	s_PlayerArray.clear();
	return true;
}

//查
CPlayer * CPlayerManager::FindPlayerByEnum(WORD wEnumIndex)
{
	if (wEnumIndex >= s_PlayerArray.size()) return NULL;
	return s_PlayerArray[wEnumIndex];
}

//查
CPlayer * CPlayerManager::FindPlayerByID(DWORD dwUserID)
{
	for(auto player : s_PlayerArray)
	{
		if(player && player->GetUserID() == dwUserID)
			return player;
	}

	return NULL;
}

//查
CPlayer * CPlayerManager::FindPlayerBySocketID(DWORD dwScoketID)
{
	for(auto player : s_PlayerArray)
	{
		if(player && (player->GetSocketID() == dwScoketID))
			return player;
	}

	return NULL;
}