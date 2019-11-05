#include "PlayerManager.h"
#include "Player.h"

std::vector<CPlayer*> CPlayerManager::s_PlayerArray;

//��
bool CPlayerManager::InsertPlayer(CPlayer * * pIServerUserResult, tagUserInfo & UserInfo, tagUserInfoPlus & UserInfoPlus)
{
	CPlayer * pPlayer=NULL;
	try
	{
		pPlayer=new CPlayer(UserInfo, UserInfoPlus);
	}
	catch (...)
	{
		CLog::Log(log_error, "new CPlayer failed;");
		return false;
	}

	//�����û�
	s_PlayerArray.push_back(pPlayer);

	//���ñ���
	*pIServerUserResult=pPlayer;

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
		if(player && player->GetSocketID() == dwScoketID)
			return player;
	}

	return NULL;
}
