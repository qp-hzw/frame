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
	CLog::Log(log_debug, "�ӳ� ɾ�� ��� %d", socketID);
	std::this_thread::sleep_for(chrono::milliseconds(3000));

	g_TCPNetworkEngine->CloseSocket(socketID);
}
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

	//�ر�socket����
	std::thread t1(CloseSocket, pPlayer->GetSocketID());
	//t1.join();

	//ɾ��list�б�
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
	//�ر�����
	CPlayer *player =  FindPlayerByID(dwUserID);
	if(player != NULL)
	{
		//�ر�socket����
		std::thread t1(CloseSocket, player->GetSocketID());
		//t1.join();
	}

	//ɾ��list
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
	CPlayer *player =  FindPlayerBySocketID(dwSocketID);
	if(player != NULL)
	{
		//�ر�socket����
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

//ɾ ����
bool CPlayerManager::DeleteAllPlayer()
{
	//�ر�����
	for(auto ite = s_PlayerArray.begin(); ite != s_PlayerArray.end(); ite++)
	{
		if(*ite)
		{
			//�ر�socket����
			std::thread t1(CloseSocket, (*ite)->GetSocketID());
			//t1.join();
		}
	}

	//ɾ��list
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