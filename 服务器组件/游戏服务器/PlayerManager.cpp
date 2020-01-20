#include "Stdafx.h"
#include "PlayerManager.h"
#include "Player.h"

std::vector<CPlayer*> CPlayerManager::s_PlayerArray;

//��
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

	//�����û�
	s_PlayerArray.push_back(pPlayer);

	return true;
}

//ɾ
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

//ɾ
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

//ɾ
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

//ɾ ����
bool CPlayerManager::DeleteAllPlayer()
{
	s_PlayerArray.clear();
	return true;
}

//��
CPlayer * CPlayerManager::FindPlayerByEnum(WORD wEnumIndex)
{
	if (wEnumIndex >= s_PlayerArray.size()) return NULL;
	return s_PlayerArray[wEnumIndex];
}

//��
CPlayer * CPlayerManager::FindPlayerByID(DWORD dwUserID)
{
	for(auto player : s_PlayerArray)
	{
		if(player && player->GetUserID() == dwUserID)
			return player;
	}

	return NULL;
}

//��
CPlayer * CPlayerManager::FindPlayerBySocketID(DWORD dwScoketID)
{
	for(auto player : s_PlayerArray)
	{
		if(player && (player->GetSocketID() == dwScoketID))
			return player;
	}

	return NULL;
}