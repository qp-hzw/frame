#include "Stdafx.h"
#include "PlayerManager.h"
#include "Player.h"
#include "GameCtrl.h"
#include <thread>
#include <chrono>
using namespace std;

std::vector<CPlayer*> CPlayerManager::s_PlayerArray;


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

	//删除list列表
	for(auto ite = s_PlayerArray.begin(); ite != s_PlayerArray.end(); ite++)
	{
		if(*ite == pPlayer)
		{
			ite = s_PlayerArray.erase(ite);
			delete pPlayer;
			pPlayer = NULL;
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

	//删除list
	for(auto ite = s_PlayerArray.begin(); ite != s_PlayerArray.end(); ite++)
	{
		if( (*ite)->GetUserID() == dwUserID)
		{
			ite = s_PlayerArray.erase(ite);
			delete player;
			player = NULL;
			break;
		}
	}

	return true;
}

//删
bool CPlayerManager::DeletePlayerBySocketID(DWORD dwSocketID)
{
	CPlayer *player =  FindPlayerBySocketID(dwSocketID);

	for(auto ite = s_PlayerArray.begin(); ite != s_PlayerArray.end(); ite++)
	{
		if( (*ite)->GetSocketID() == dwSocketID)
		{
			ite = s_PlayerArray.erase(ite);
			delete player;
			player = NULL;
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
		delete *ite;
		*ite = NULL;
	}

	//删除list
	s_PlayerArray.clear();
	return true;
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


//断开玩家网络连接
 void CPlayerManager::CloseSocket(CPlayer* player)
{
	if(player == NULL ) return;
	if(player->IsAndroidUser()) return;

	CLog::Log(log_debug, "关闭玩家socket %d", player->GetSocketID());
	g_TCPNetworkEngine->CloseSocket(player->GetSocketID());
}
