#include "stdafx.h"
#include "RobotManager.h"
#include "Player.h"

std::vector<CPlayer *> CRobotManager::s_RobotArray;


//��
bool CRobotManager::InsertRobot(tagUserInfo & UserInfo)
{
	CPlayer * pPlayer=NULL;
	try
	{
		pPlayer=new CPlayer(UserInfo);
		if (pPlayer == NULL)
			throw;

	}
	catch (...)
	{
		CLog::Log(log_error, "new CPlayer failed;");
		return false;
	}

	//�����û�
	s_RobotArray.push_back(pPlayer);

	return true;
}

//ɾ
bool CRobotManager::DeleteRobot(CPlayer * pPlayer)
{
	if (pPlayer==NULL) return false;

	//ɾ��list�б�
	for(auto ite = s_RobotArray.begin(); ite != s_RobotArray.end(); ite++)
	{
		if(*ite == pPlayer)
		{
			ite = s_RobotArray.erase(ite);
			delete pPlayer;
			pPlayer = NULL;
			break;
		}
	}

	return true;
}

//��
CPlayer * CRobotManager::FindRobotByID(DWORD dwUserID)
{
	for(auto player : s_RobotArray)
	{
		if(player && player->GetUserID() == dwUserID)
			return player;
	}

	return NULL;
}

//��
CPlayer * CRobotManager::FindRobotByIndex(WORD index)
{
	if (index >= s_RobotArray.size())
		return NULL;
	return s_RobotArray[index];
}

//������
DWORD CRobotManager::RobotCount()
{
	return s_RobotArray.size();
}