#include "stdafx.h"
#include "RobotManager.h"
#include "Player.h"

std::vector<CPlayer *> CRobotManager::s_RobotArray;


//增
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

	//插入用户
	s_RobotArray.push_back(pPlayer);

	return true;
}

//删
bool CRobotManager::DeleteRobot(CPlayer * pPlayer)
{
	if (pPlayer==NULL) return false;

	//删除list列表
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

//查
CPlayer * CRobotManager::FindRobotByID(DWORD dwUserID)
{
	for(auto player : s_RobotArray)
	{
		if(player && player->GetUserID() == dwUserID)
			return player;
	}

	return NULL;
}

//查
CPlayer * CRobotManager::FindRobotByIndex(WORD index)
{
	if (index >= s_RobotArray.size())
		return NULL;
	return s_RobotArray[index];
}

//查总数
DWORD CRobotManager::RobotCount()
{
	return s_RobotArray.size();
}