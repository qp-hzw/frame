#include "stdafx.h"
#include "MatchRoom.h"
#include "MatchItem.h"
#include "RoomRuleManager.h"
#include "GameCtrl.h"

CMatchRoom::CMatchRoom(CMatchItem *item)
{
	m_Match_Item = item;

	//��ȡ�������
	tagTableRule rule;
	ZeroMemory(&rule, sizeof(tagTableRule));
	RoomRuleManager::Instance()->SetMatchRule(rule);

	//���ù���
	SetCommonRule(&rule);
}


CMatchRoom::~CMatchRoom(void)
{
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
bool CMatchRoom::HandleXJGameEnd(BYTE cbCurGameCount, BYTE cbGameMode, SCORE *lGameScore)
{
	//����С�ֽ�������
	CTableFrame::HandleXJGameEnd(cbCurGameCount, cbGameMode, lGameScore);

	//��������
	m_Match_Item->Update_Ranking(this);

	//�жϴ���Ƿ����
	if (false)
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
	//����������ֽ�������
	CTableFrame::HandleDJGameEnd(cbGameStatus);

	//���������һ�ֽ�������
	m_Match_Item->On_Room_End(this);

	return true;
}

//��Ϸ��ʼ
bool CMatchRoom::StartGame()
{
	return true;
}

//��ʼ��һ�׶α���
void CMatchRoom::StartNextStage()
{
	m_Match_Item->On_Stage_Start();
}