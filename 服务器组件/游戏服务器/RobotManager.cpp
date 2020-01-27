#include "stdafx.h"
#include "RobotManager.h"
#include "Player.h"
#include "GameCtrl.h"
#include "TableManager.h"
#include "PlayerManager.h"

#define IDI_LOAD_ANDROID_USER			(IDI_MAIN_MODULE_START+1)			//���ػ�����
#define TIME_LOAD_ANDROID_USER				3000L							//���ػ���


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
	CPlayerManager::InsertPlayer(0, *(pPlayer->GetUserInfo()));

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

//���û������Զ����뷿�䶨ʱ��
void CRobotManager::SetRobotTimer()
{
	g_GameCtrl->SetTimer(IDI_LOAD_ANDROID_USER,TIME_LOAD_ANDROID_USER,TIMES_INFINITY,NULL);
}

//���û������Զ����뷿��
void CRobotManager::On_ANDROID_JOIN_GAME()
{
	std::vector<CTableFrame*> table_array(CTableManager::GetAllGlodTable());
	for(auto pTable :table_array )
	{
		if (pTable == NULL) continue;

		//�������Ƿ�����ʵ���
		if (!pTable->IsTruePlayer()) 
		{
			CLog::Log(log_debug, "��ҳ� ���ӡ�%d�� û����ʵ���", pTable->GetTableID());
			continue;
		}

		//���ҿ��л�����
		CPlayer *pPlayer = GetFreeAndroid();
		if (pPlayer == NULL) 
		{
			CLog::Log(log_debug, "��ҳ� ���ӡ�%d�� û���ҵ�������", pTable->GetTableID());
			break;
		}

		//����
		if( 0 != pTable->PlayerSitTable(pPlayer))
		{
			CLog::Log(log_debug, "��ҳ� ���ӡ�%d�� ����������ʧ��", pTable->GetTableID());
			continue;
		}

		//׼��
		if (0 != pTable->PlayerReady(pPlayer)) 
		{
			CLog::Log(log_debug, "��ҳ� ���ӡ�%d�� ������׼��ʧ��", pTable->GetTableID());
			continue;
		}
	}
}

//���ҷ��������Ļ������û�
CPlayer* CRobotManager::GetFreeAndroid()
{
	for (int i = 0; i < s_RobotArray.size(); i++)
	{
		//��ȡ������
		CPlayer *robot = s_RobotArray[i];

		//������У��
		if (robot  && (US_FREE == robot->GetUserStatus()))
			return robot;
	}

	return NULL;
}