#include "stdafx.h"
#include "RobotManager.h"
#include "Player.h"
#include "GameCtrl.h"
#include "TableManager.h"
#include "PlayerManager.h"

#define IDI_LOAD_ANDROID_USER			(IDI_MAIN_MODULE_START+1)			//加载机器人
#define TIME_LOAD_ANDROID_USER				3000L							//加载机器


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
	CPlayerManager::InsertPlayer(0, *(pPlayer->GetUserInfo()));

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

//设置机器人自动加入房间定时器
void CRobotManager::SetRobotTimer()
{
	g_GameCtrl->SetTimer(IDI_LOAD_ANDROID_USER,TIME_LOAD_ANDROID_USER,TIMES_INFINITY,NULL);
}

//设置机器人自动加入房间
void CRobotManager::On_ANDROID_JOIN_GAME()
{
	std::vector<CTableFrame*> table_array(CTableManager::GetAllGlodTable());
	for(auto pTable :table_array )
	{
		if (pTable == NULL) continue;

		//桌子中是否有真实玩家
		if (!pTable->IsTruePlayer()) 
		{
			CLog::Log(log_debug, "金币场 桌子【%d】 没有真实玩家", pTable->GetTableID());
			continue;
		}

		//查找空闲机器人
		CPlayer *pPlayer = GetFreeAndroid();
		if (pPlayer == NULL) 
		{
			CLog::Log(log_debug, "金币场 桌子【%d】 没有找到机器人", pTable->GetTableID());
			break;
		}

		//坐下
		if( 0 != pTable->PlayerSitTable(pPlayer))
		{
			CLog::Log(log_debug, "金币场 桌子【%d】 机器人坐下失败", pTable->GetTableID());
			continue;
		}

		//准备
		if (0 != pTable->PlayerReady(pPlayer)) 
		{
			CLog::Log(log_debug, "金币场 桌子【%d】 机器人准备失败", pTable->GetTableID());
			continue;
		}
	}
}

//查找符合条件的机器人用户
CPlayer* CRobotManager::GetFreeAndroid()
{
	for (int i = 0; i < s_RobotArray.size(); i++)
	{
		//获取机器人
		CPlayer *robot = s_RobotArray[i];

		//机器人校验
		if (robot  && (US_FREE == robot->GetUserStatus()))
			return robot;
	}

	return NULL;
}