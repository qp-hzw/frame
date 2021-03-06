#include "PlayerManager.h"
#include "Player.h"

std::list<CPlayer*> CPlayerManager::s_PlayerArray;
std::map<DWORD, DWORD> CPlayerManager::s_OfflinePlayer;

//增
bool CPlayerManager::InsertPlayer(DWORD dwSocketID, tagUserInfo& UserInfo)
{
	CPlayer * pPlayer=NULL;
	try
	{
		pPlayer=new CPlayer(dwSocketID, UserInfo);
		if (pPlayer == NULL)
			throw;
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
		if(player && player->GetSocketID() == dwScoketID)
			return player;
	}

	return NULL;
}
//查 俱乐部所有
std::vector<CPlayer*> CPlayerManager::FindPlayerByClubID(DWORD dwClubID)
{
	std::vector<CPlayer*> vec_play;
	vec_play.clear();

	/* TODONOW 需要增加俱乐部管理类
	for(auto player : s_PlayerArray)
	{
		if(player && player->GetSocketID() == dwScoketID)
			return player;
	}
	*/

	return vec_play;
}

//增加断线玩家
void CPlayerManager::AddOfflinePlayer(DWORD userID, DWORD serverID)
{
	CLog::Log(log_debug, "增加断线玩家 %ld", userID);
	s_OfflinePlayer[userID] = serverID;
}

//删除断线玩家
void CPlayerManager::DeleteOfflinePlayer(DWORD userID)
{
	CLog::Log(log_debug, "删除断线玩家 %ld", userID);
	s_OfflinePlayer.erase(userID);
}

//获得断线玩家数据
DWORD CPlayerManager::FindOfflineGameID(DWORD userID)
{
	if(s_OfflinePlayer.end() != s_OfflinePlayer.find(userID))
	{
		return s_OfflinePlayer[userID];
	}
	else
	{
		return 0;
	}
}
