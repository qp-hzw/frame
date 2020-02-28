#include "stdafx.h"
#include "MatchRoom.h"
#include "MatchItem.h"
#include "RoomRuleManager.h"
#include "GameCtrl.h"
#include "TableManager.h"

CMatchRoom::CMatchRoom(CMatchItem *item, WORD stage)
{
	m_Match_Item = item;
	m_state = wait_start;
	m_index = 0;

	//��ȡ�������
	tagTableRule rule;
	ZeroMemory(&rule, sizeof(tagTableRule));
	RoomRuleManager::SetMatchRule(rule, &(item->GetConfig()), stage);

	//���ù���
	SetCommonRule(&rule);
}


CMatchRoom::~CMatchRoom(void)
{
}

CMatchRoom::CMatchRoom(const CMatchRoom& room)
{
	m_state = room.m_state;
	m_index = room.m_index;
	m_Match_Item = room.m_Match_Item;
}

//�������
int CMatchRoom::PlayerSitTable(CPlayer * pIServerUserItem, WORD wChairID, bool bCheckUserGPS)
{
	if (m_state != wait_start)
		return -1;

	//����������������
	int ret = CTableFrame::PlayerSitTable(pIServerUserItem);
	if (ret != 0)
		return ret;

	//�۳�������


	return 0;
}

//С�ֽ���
bool CMatchRoom::HandleXJGameEnd(BYTE byRound, WORD *wIdentity, SCORE *lUserTreasure, VOID* pData, DWORD dwDataSize)
{
	//����С�ֽ�������
	CTableFrame::HandleXJGameEnd(byRound, wIdentity, lUserTreasure, pData, dwDataSize);
	CLog::Log(log_debug, "С�ֽ�����");
	//��������
	m_Match_Item->Update_Ranking(this);

	//�жϴ���Ƿ����
	if (GetCurGameRound() !=	GetCustomRule()->GameCount)
	{
		//���±�������Ϣ

		//�����Զ�׼����ʱ��
		SetPlayerAutoReady();	
	}

	return true;
}

//��ֽ���
bool CMatchRoom::HandleDJGameEnd(BYTE cbGameStatus)
{
	//����뿪����
	for(auto player : GetPlayer_list())
	{
		PlayerLeaveTable(player);
	}

	//���������һ�ֽ�������
	m_Match_Item->On_Room_End(this);

	//ɾ������
	CTableManager::DeleteTable(this);

	return true;
}

//��Ϸ��ʼ
bool CMatchRoom::StartGame()
{
	CTableFrame::StartGame();

	//��������Ҹ����Լ�������
	for (auto player : GetPlayer_list())
	{
		if (player && !player->IsAndroidUser())
		{
			m_Match_Item->Send_Self_Ranking(player);
		}
	}

	return true;
}

//��ʼ��һ�׶α���
void CMatchRoom::StartNextStage()
{
	m_Match_Item->On_Stage_Start();
}