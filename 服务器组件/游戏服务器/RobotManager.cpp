#include "stdafx.h"
#include "RobotManager.h"
#include "Player.h"
#include "GameCtrl.h"
#include "TableManager.h"

//时间标识
#define IDI_LOAD_ANDROID_USER			(IDI_ROBOT_MODULE_START+1)			//机器信息
//时间定义 秒
#define TIME_LOAD_ANDROID_USER				3000L								//加载机器

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

//设置机器人自动加入房间定时器
void CRobotManager::SetRobotTimer()
{
	g_GameCtrl->SetTimer(IDI_LOAD_ANDROID_USER,TIME_LOAD_ANDROID_USER,TIMES_INFINITY,NULL);
}

//设置机器人自动加入房间
bool CRobotManager::On_ANDROID_JOIN_GAME()
{
	// 1、校验

	// 2、查找金币场桌子
	WORD wChairID;
	CTableFrame *pTableFrame = CTableManager::GetGlodRoomEmptyChair(wChairID, 0, true);
	if (pTableFrame == NULL)
		return true;

	// 3、椅子校验
	if (wChairID == INVALID_CHAIR)
		return true;

	// 4、房间规则校验
	tagTableRule *pTableRule = (tagTableRule *)pTableFrame->GetCustomRule();
	if (pTableRule == NULL)
		return true;

	// 5、桌子是否在游戏中校验
	if (GAME_STATUS_FREE != pTableFrame->GetGameStatus())
		return true;

	// 6、桌子中是否有真实玩家
	if (!pTableFrame->IsTruePlayer())
		return true;

	// 7、查找符合条件的机器人
	CPlayer *pPlayer = GetAndroidUser(pTableFrame);
	if (pPlayer == NULL)
		return true;

	// 8、机器人坐下
	if (pTableFrame->PlayerSitTable(pPlayer, wChairID) != 0)
		return true;

	// 9、机器人准备
	if (pTableFrame->PlayerReady(wChairID, pPlayer) != 0)
		return true;

	return true;
}

//查找符合条件的机器人用户
CPlayer* CRobotManager::GetAndroidUser(CTableFrame *pTableFrame)
{

	for (int i = 0; i < RobotCount(); i++)
	{
		//获取机器人
		CPlayer *robot = FindRobotByIndex(i);

		//机器人校验
		if ((robot == NULL) || (US_FREE != robot->GetUserStatus()))
			continue;

		return robot;
	}
}