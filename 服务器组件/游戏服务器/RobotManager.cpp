#include "stdafx.h"
#include "RobotManager.h"
#include "Player.h"
#include "GameCtrl.h"
#include "TableManager.h"

//ʱ���ʶ
#define IDI_LOAD_ANDROID_USER			(IDI_ROBOT_MODULE_START+1)			//������Ϣ
//ʱ�䶨�� ��
#define TIME_LOAD_ANDROID_USER				3000L								//���ػ���

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

//���û������Զ����뷿�䶨ʱ��
void CRobotManager::SetRobotTimer()
{
	g_GameCtrl->SetTimer(IDI_LOAD_ANDROID_USER,TIME_LOAD_ANDROID_USER,TIMES_INFINITY,NULL);
}

//���û������Զ����뷿��
bool CRobotManager::On_ANDROID_JOIN_GAME()
{
	// 1��У��

	// 2�����ҽ�ҳ�����
	WORD wChairID;
	CTableFrame *pTableFrame = CTableManager::GetGlodRoomEmptyChair(wChairID, 0, true);
	if (pTableFrame == NULL)
		return true;

	// 3������У��
	if (wChairID == INVALID_CHAIR)
		return true;

	// 4���������У��
	tagTableRule *pTableRule = (tagTableRule *)pTableFrame->GetCustomRule();
	if (pTableRule == NULL)
		return true;

	// 5�������Ƿ�����Ϸ��У��
	if (GAME_STATUS_FREE != pTableFrame->GetGameStatus())
		return true;

	// 6���������Ƿ�����ʵ���
	if (!pTableFrame->IsTruePlayer())
		return true;

	// 7�����ҷ��������Ļ�����
	CPlayer *pPlayer = GetAndroidUser(pTableFrame);
	if (pPlayer == NULL)
		return true;

	// 8������������
	if (pTableFrame->PlayerSitTable(pPlayer, wChairID) != 0)
		return true;

	// 9��������׼��
	if (pTableFrame->PlayerReady(wChairID, pPlayer) != 0)
		return true;

	return true;
}

//���ҷ��������Ļ������û�
CPlayer* CRobotManager::GetAndroidUser(CTableFrame *pTableFrame)
{

	for (int i = 0; i < RobotCount(); i++)
	{
		//��ȡ������
		CPlayer *robot = FindRobotByIndex(i);

		//������У��
		if ((robot == NULL) || (US_FREE != robot->GetUserStatus()))
			continue;

		return robot;
	}
}