#include "Stdafx.h"
#include "TableFrame.h"
#include "DataBasePacket.h"
#include "GameCtrl.h"
#include <iostream>
#include "TableManager.h"
//#include <algorithm>


//////////////////////////////////////////////////////////////////////////////////
//У��GPS����
#define CHECK_USER_GPS_DISTANCE		200									//���GPS�ڶ��پ�������ʾ���

//��ɢ�ȴ�
#define IDI_ROOM_AUTO_DISMISS		(TIME_TABLE_SINK_RANGE+3)			//�����Զ���ɢ��ʱ��
#define TIME_ROOM_AUTO_DISMISS		300000L								//�����Զ���ɢ��ʱ��ʱ��

//���ѵȴ�
#define IDI_WAIT_CONTINUE			(TIME_TABLE_SINK_RANGE+4)			//�ȴ����ѱ�ʶ
#define TIME_WAIT_CONTINUE			20000L								//�ȴ�����ʱ��

//����¼�
#define IDI_WAIT_REDPACKET			(TIME_TABLE_SINK_RANGE+5)			//�ȴ��������ʶ
#define TIME_WAIT_REDPACKET			60000L								//ÿ���Ӽ��һ��

//�����ɢ����
#define IDI_VOTE_DISMISS_ROOM		(TIME_TABLE_SINK_RANGE+6)			//�����ɢ����
#define TIME_VOTE_DISMISS_ROOM		300000L								//�¼� --5���� 

//��Ʊ�����Լ�����Ϸʱ��. ��ɢ����
#define IDI_CHECK_DISMISS_ROOM		(TIME_TABLE_SINK_RANGE+7)			//�����ɢ����
#define TIME_CHECK_DISMISS_ROOM		4*1000L								//�¼� --4���� 

/////////////////////////////////////////////////////////////////////////////////

enum PLAYER_OP_ERRORCODE
{
	PLAYER_NOT_EXISIT = 1, //��Ҳ�����
	TABLE_FULL = 2,        //��������
	TABLE_NOT_THIS = 3,    //����ͬһ��������

	CHAIR_USERD=4,         //�������Ѿ�����
	CHAIR_INVALID=5,       //���ӺŲ���ȷ
	STATUS_ERR = 6,			//û���¾�Ҫ׼��
};

/////////////////////////////////////////////////////////////////////////////////
//���캯��
CTableFrame::CTableFrame()
{
	//����Ϸ
	m_pITableFrameSink= g_GameCtrl->GetITableFrameSink();
	/******************** ��̬���� **********************/
	m_wTableID=0;
	m_wChairCount=0;
	m_cbTableMode = 0;
	m_cbGoldType = 0;

	m_dwGroupID = 0;
	m_dwCreateTableUser = 0;
	m_dwTableOwner=0L;

	//��ʼʱ��
	GetLocalTime(&m_SystemTimeStart);
	//ZeroMemory(&m_SystemTimeStart,sizeof(m_SystemTimeStart));

	/******************** ��̬���� **********************/
	m_cbGameStatus=GAME_STATUS_FREE;
	m_wCurGameRound=0;
	
	m_user_list.clear();
	m_player_list.clear();

	//��ɢ��Ϣ
	ZeroMemory(m_bResponseDismiss,sizeof(m_bResponseDismiss));
	ZeroMemory(m_bAgree,sizeof(m_bAgree));
	m_bUnderDissState = false;
	m_dissmisserChaiID = 0xFF;

	return;
}

//��������
CTableFrame::~CTableFrame()
{
	//�ͷŶ���
	if ( NULL != m_pITableFrameSink )
	{
		SafeDelete(m_pITableFrameSink);
	}
	return;
}

/*************************************** ���������������ȡ ***************************************************/
//���÷����Զ���ɢʱ��
void CTableFrame::SetTableAutoDismiss(DWORD dwMinutes) 
{ 
	SetGameTimer(IDI_ROOM_AUTO_DISMISS, dwMinutes*TIME_ROOM_AUTO_DISMISS, 1, NULL); 
}

//���ñ����ɢ���䶨ʱ��
void CTableFrame::SetVoteDismissRoom()
{
	SetGameTimer(IDI_VOTE_DISMISS_ROOM, TIME_VOTE_DISMISS_ROOM, 1, NULL);
}
//ȡ�������ɢ���䶨ʱ��
void CTableFrame::KillVoteDismissRoom()
{
	KillGameTimer(IDI_VOTE_DISMISS_ROOM);
}

//��ʼ������Ϸ
bool CTableFrame::InitTableFrameSink()
{
	if (m_pITableFrameSink == NULL)
		return false;

	return m_pITableFrameSink->Initialization(this);
}

/***************************************   ��Ϸ���̺���    ***************************************************/
//��ʼ��Ϸ
bool CTableFrame::StartGame()
{
	//״̬У��
	if(GetGameStatus() != GAME_STATUS_FREE) return false;

	//������Ϸ״̬Ϊ��ʼ
	SetGameStatus(GAME_STATUS_PLAY);

	//ɾ���Զ���ɢ��ʱ��
	KillGameTimer(IDI_ROOM_AUTO_DISMISS);

	//player״̬
	for(int i=0; i< m_player_list.size(); i++)
	{
		if(!m_player_list.at(i)) continue;
		m_player_list.at(i)->SetUserStatus(US_PLAYING,m_wTableID,i);
	}

	//user״̬
	for (int i = 0; i< m_player_list.size(); i++)
	{
		if (!m_player_list.at(i)) continue;

		if (US_READY == m_player_list.at(i)->GetUserStatus())
		m_player_list.at(i)->SetUserStatus(US_PLAYING, m_wTableID, i);
	}

	/* ֪ͨ���ݿ�, ���ӿ�ʼ */      
	tagTableRule *pTableCfg = (tagTableRule*)GetCustomRule();
	BYTE byClubOrHalGold = 0; //1��ʾ���ֲ�����(�������߷������ģʽ);  2��ʾ�����Ľ��ģʽ;  �����ֶ����봦��
	BYTE byClubCreate = pTableCfg->byClubCreate;
	BYTE GameMode = pTableCfg->GameMode;

	if(( 1 == byClubCreate) || (2 == byClubCreate))
	{
		byClubOrHalGold = 1;
	}
	else if((0 == byClubCreate) && (2 == GameMode))
	{
		byClubOrHalGold = 2;
	}
	
	//TODONOW ��Ҫ�����������
	//g_AttemperEngineSink->ClubTableStart(GetTableID(), byClubOrHalGold);

	//֪ͨ�¼�
	if (m_pITableFrameSink!=NULL) 
	{
		m_pITableFrameSink->OnEventGameStart();
	}

	return true;
}

//С�ֽ���������
bool CTableFrame::HandleXJGameEnd(BYTE byRound, BYTE byTableMode_NO_USER, SCORE *lUserTreasure)
{
	//TODONOW 2018/5/27 byTableMode���ֶ���Ч����ʹ�ø��ֶ�, �´ε���ʱ�����ɾ�����ֶ� 

	//tableMode  ��  lUserTreasure ���ձ�
	//tableMode(����ģʽ)      byRound(��ǰ����  )     lUserTreasure(�û��Ƹ�)
	//0 ����ģʽ			   0					   ����
	//0 ����ģʽ			   ��0					   ����
	//1 ������ģʽ									   ����
	//2 ���ģʽ			   ����					   ���
	//3 �������ģʽ           0                       ����
	//3 �������ģʽ           ��0					   ���

	//����player��user״̬
	for (int i = 0; i< m_player_list.size(); i++)
	{
		if (!m_player_list.at(i)) continue;
		m_player_list.at(i)->SetUserStatus(US_SIT, m_wTableID, i);
	}

	for (int i = 0; i< m_user_list.size(); i++)
	{
		if (!m_user_list.at(i)) continue;

		if (US_PLAYING == m_player_list.at(i)->GetUserStatus())
			m_player_list.at(i)->SetUserStatus(US_SIT, m_wTableID, i);
	}

	//�������
	tagTableRule* pCfg = (tagTableRule*)GetCustomRule();
	BYTE byTableMode = pCfg->GameMode;

	//�����û��Ƹ� ��Ӧ���ݿ� round =1 
	//XJModifyUserTreasure(byTableMode, byRound, lUserTreasure, pCfg);

	//�����û�����״̬
	//XJUpdateGameTaskStatus(byTableMode, byRound); //TODONOW ��ʱֻΪ������

	//�۳��û���Ʊ -- ��Ӧ���ݿ� round = 0
	//XJGameTickets(byTableMode, byRound);

	//�¼�֪ͨ
	STR_DBR_CLUB_TABLE_INFO Dbr;
	ZeroMemory(&Dbr, sizeof(Dbr));
	Dbr.dwTableID = GetTableID();
	Dbr.dwTableState = 1;
	Dbr.byCurrentRound = 1;//����һ������ TODONOW �����޸�
	Dbr.byMask = 2;
	
	//g_GameCtrl->PostDataBaseRequest(DBR_CLUB_TABLE_INFO,0,&Dbr,sizeof(Dbr));

	return true;
}

//��ֽ���������
bool CTableFrame::HandleDJGameEnd(BYTE cbGameStatus)
{
	KillGameTimer(IDI_ROOM_AUTO_DISMISS);

	//���÷��䲻���ڽ�ɢ״̬
	m_bUnderDissState = false;
	m_dissmisserChaiID = 0xFF;

	//�ر����ж�ʱ��

	//�����ȴ����ѣ������Ҫ��׼��״̬�˳�Ϊ����
	if (cbGameStatus == GAME_CONCLUDE_CONTINUE)
	{
		m_cbGameStatus=GAME_STATUS_FREE;

		ZeroMemory(m_bAgree,sizeof(m_bAgree));

		//״̬�л�Ϊ���£����������뿪
		for (WORD wChairID = 0;wChairID<m_wChairCount;wChairID++)
		{
			//��ȡ�û�
			CPlayer * pIServerUserItem=GetTableUserItem(wChairID);
			//�û�����
			if (pIServerUserItem!=NULL)
			{
				//����״̬
				pIServerUserItem->SetUserStatus(US_SIT,m_wTableID,wChairID);
			}
		}

		//�������Ѷ�ʱ��
		SetGameTimer(IDI_WAIT_CONTINUE,TIME_WAIT_CONTINUE,1,NULL);

		return true;
	}

	//ֱ�ӽ�ɢ���ȫ��վ��
	if(cbGameStatus == GAME_CONCLUDE_NORMAL)
	{
		for (WORD wChairID = 0;wChairID<m_wChairCount;wChairID++)
		{
			//��ȡ�û�
			CPlayer * pIServerUserItem=GetTableUserItem(wChairID);

			//�û�����
			if (pIServerUserItem!=NULL)
			{
				//�������վ��
				PlayerLeaveTable(pIServerUserItem);

				//1. �������û����ڶ���״̬��δ����
				if ( US_OFFLINE == pIServerUserItem->GetUserStatus() )
				{
					//���͸�Э��������, ��ת������¼��
					tagOfflineUser data;
					data.dwUserID = pIServerUserItem->GetUserID();
					data.byMask = 2; //��ʾɾ�������û�

					//���͸�Э��������
					g_TCPSocketEngine->SendData(MDM_USER,SUB_CS_C_USER_OFFLINE,&data,sizeof(tagOfflineUser));
				}
			}

			//�Ͽ��û� socket
			CPlayerManager::CloseSocket(pIServerUserItem);
		}

		CTableManager::DeleteTable(this);
		return true;
	}

	return true;
}


/*************************************   ��Ϸ���̸�������    *************************************************/
//ÿ����Ϸ�����󣬼�Ⲣ�۳��û���Ʊ  
bool CTableFrame::XJGameTickets(BYTE byTableMode, BYTE byRound)
{
	//��ע1:�۳���Ʊ���ձ�
	//�����˿���  �ڴ������ӵ�ʱ��Ϳ۳��� -- �б�Ҫ��ô����???
	//�Լ����� �۳����ձ�
	//TableMode			�Ƿ���Ҫ�۳�	�ڼ��ֿ۳�       �۳��ĲƸ�����
	//0����ģʽ			��				1				 ����
	//1���ģʽ			����				
	//2�������ģʽ		��				1				 ����

	//�����˿������ڴ�������ʱ�Ѿ��۳�����������Ҫ�ٿ۳�
	if (0 != m_dwCreateTableUser)
		return true;

	//�������
	tagTableRule* pCfg = (tagTableRule*)GetCustomRule();

	//��ȡ����,����һ�����ߣ����������û����������ϣ���������ֻ��ͨ�����������ǿ۳���ʱ��ͨ��USERIDɾ�����ݿ�����
	CPlayer *pOwnerUserItem = CPlayerManager::FindPlayerByID(m_dwTableOwner);
	if(pOwnerUserItem == NULL)
		return false;


	//���ֲ� ������| ������ҳ�  ��һС��ʱ��
	if((byTableMode == TABLE_MODE_FK || byTableMode == TABLE_MODE_FK_GOLD)
		&& (1 == byRound)
		&&(pCfg->byClubCreate != 0))
	{
		BYTE byPlayerNum = pCfg->PlayerCount;
		BYTE byGameCountType = pCfg->GameCountType;
		SCORE cost = byPlayerNum * byGameCountType;

		//�۳�����
		pOwnerUserItem->ModifyUserTreasure(GetTableID(), byTableMode, 0, -cost, 0);//��������ʱ��� byRount = 0
		return true;
	}

	//�Ǿ��ֲ� ����ģʽ���������ģʽ
	if( (byTableMode == TABLE_MODE_FK || byTableMode == TABLE_MODE_FK_GOLD)
		&& (1 == byRound))
	{
		if( pCfg->cbPayType == 0 )							//����֧��
		{
			BYTE byPlayerNum = pCfg->PlayerCount;
			BYTE byGameCountType = pCfg->GameCountType;
			SCORE cost = byPlayerNum * byGameCountType;

			//�۳�����
			pOwnerUserItem->ModifyUserTreasure(GetTableID(), byTableMode, 0, -cost, 0);//��������ʱ��� byRount = 0
		}
		else if(pCfg->cbPayType == 1)						//AA֧��
		{
			//���������ϵ�ÿ���ˣ��۳�����
			for (WORD i=0; i<m_wChairCount; i++)
			{
				//��ȡ�û�
				CPlayer *pIServerUserItem = GetTableUserItem(i);
				if (pIServerUserItem == NULL) continue;

				BYTE byGameCountType = pCfg->GameCountType;
				SCORE cost =  byGameCountType;

				//�۳����� 
				pIServerUserItem->ModifyUserTreasure(GetTableID(), byTableMode, 0, -cost, 0);//��������ʱ��� byRount = 0
			} 
		}	
	}


	//�Ǿ��ֲ� ���ģʽ
	if( (byTableMode == TABLE_MODE_GOLD)
		&& (1 == byRound))
	{
		//���������ϵ�ÿ���ˣ��۳����
		for (WORD i=0; i<m_wChairCount; i++)
		{
			//��ȡ�û�
			CPlayer *pIServerUserItem = GetTableUserItem(i);
			if (pIServerUserItem == NULL) continue;

			SCORE cost =  pCfg->lSinglePayCost;

			//�۳����� 
			pIServerUserItem->ModifyUserTreasure(GetTableID(), byTableMode, 0, -cost, 0);//��������ʱ��� byRount = 0
		}
	}

	return true;
}
//ÿ����Ϸ�����󣬸����û��Ƹ���Ϣ
bool CTableFrame::XJModifyUserTreasure(BYTE byTableMode, BYTE byRound, SCORE *lGameScore, tagTableRule *pCfg)
{
	bool bIsMoneyEmpty = false; //�Ƿ��������㲻������Ϸ������

	//�������������û�
	for (int i = 0; i < m_wChairCount; i++)
	{
		//��ȡ�û�
		CPlayer * pIServerUserItem = GetTableUserItem(i);

		//У���û�
		if(pIServerUserItem == NULL )
			continue;

		BYTE byWin = 1;	//1�����Ǵ�Ӯ��, 0������
		for(int j = 0; j < m_wChairCount; j++)
		{
			if((i != j) && (lGameScore[j] > lGameScore[i]))
			{
				byWin = 0;
				break;
			}
		}


		//������Ϣ
		pIServerUserItem->ModifyUserTreasure(GetTableID(), byTableMode, byRound, lGameScore[i], byWin);

		//added by WangChengQing ���ݿ�ˢ�����ӳ�, ��˴˴���ʱ�ȸ���һ������
		if(TABLE_MODE_FK_GOLD == byTableMode)
		{
			pIServerUserItem->SetUserGold(pIServerUserItem->GetUserGold() + lGameScore[i]);
		}

		//�볡У��
		if (CheckUserLeave(byTableMode,  pIServerUserItem, pCfg))
		{
			bIsMoneyEmpty = true;
		}
	}

	//������˲�����. ������Ϣ
	if(bIsMoneyEmpty)
	{
		for (int i = 0; i < m_wChairCount; i++)
		{
			//��ȡ�û�
			CPlayer * pIServerUserItem = GetTableUserItem(i);

			//У���û�
			if(pIServerUserItem == NULL )
				continue;

			//֪ͨ�������� ��ʱע�͵�
			//SendRequestFailure(pIServerUserItem, szString, REQUEST_FAILURE_NORMAL);

			//�����Զ���Ӧ��ʱ��
			SetGameTimer(IDI_CHECK_DISMISS_ROOM, TIME_CHECK_DISMISS_ROOM, 1, NULL);
		}
	}
	return true;
}
//ÿ����Ϸ������, ����û��Ƹ��Ƿ���Լ�����Ϸ
bool CTableFrame::CheckUserLeave(BYTE byTableMode,CPlayer *pIServerUserItem, tagTableRule *pCfg)
{
	//��ע1: true��ʾ���ܼ�����Ϸ.   false��ʾ���Լ�����Ϸ
	//��ע2: �ڲ�ʹ��, ��У���ָ��
	//��ע3: У����ձ�
	//TableMode		Check��			Treasure
	//0������			��
	//1������			����
	//2��ҳ�			��				���
	//3������ҳ�		��				���
	switch(byTableMode)
	{
	case TABLE_MODE_GOLD:
	case TABLE_MODE_FK_GOLD:
		{
			//�����޸�Ϊ��Ʊ�� 10%
			if(pIServerUserItem->GetUserGold() < (pCfg->dwLevelGold * 0.1))
			{
				return true;
			}
		}
	}
	return false;
}
//ÿ����Ϸ�����󣬸�����Ϸ����״̬
bool CTableFrame::XJUpdateGameTaskStatus(const BYTE &cbTableMode, const BYTE &cbCurGameCount)
{
	//�������
	tagTableRule *pCfg = (tagTableRule *)GetCustomRule();	

	//У�����ӹ���
	if ( pCfg->GameMode != cbTableMode )
		return false;

	//�������������û�
	for (int i = 0; i < m_wChairCount; i++)
	{
		//��ȡ�û�
		CPlayer *pIServerUserItem = GetTableUserItem(i);

		//У���û�
		if(pIServerUserItem == NULL )
			continue;

		//����DBR����
		STR_DBR_CG_TABLE_UPDATE_TASK_STATUS TaskStatus;
		ZeroMemory(&TaskStatus, sizeof(STR_DBR_CG_TABLE_UPDATE_TASK_STATUS));

		//��ֵ
		TaskStatus.dwUserID = pIServerUserItem->GetUserID();
		TaskStatus.cbTableMode = cbTableMode;
		TaskStatus.cbCurGameCount = cbCurGameCount;

		//�������ݿ�����,�޷�����û���socketID����0
		g_GameCtrl->PostDataBaseRequest(DBR_SC_TABLE_UPDATE_TASK_STATUS, 0, &TaskStatus, sizeof(STR_DBR_CG_TABLE_UPDATE_TASK_STATUS));

	}

	return true;
}


/****************************************    ��Ҷ���     ***************************************************/
//��Ҽ���
int CTableFrame::PlayerEnterTable(CPlayer * pPlayer)
{
	//1. У��
	int ret = CanPlayerEnterTable(pPlayer);
	if(ret != 0) return ret;

	//2. Player
	pPlayer->SetUserStatus(US_IN_TABLE, m_wTableID, INVALID_CHAIR);

	//3. Sendto Client
	CMD_GF_GameStatus GameStatus;
	ZeroMemory(&GameStatus, sizeof(GameStatus));
	GameStatus.cbUserAction = pPlayer->GetUserStatus();
	tagUserInfo *pUserInfo = pPlayer->GetUserInfo();
	CopyMemory(&GameStatus.UserInfo, pUserInfo, sizeof(tagUserInfo));

	//�㲥����
	SendTableData(INVALID_CHAIR, SUB_GR_USER_STATUS, &GameStatus, sizeof(GameStatus), MDM_USER);

	//4. ����
	m_user_list.push_back(pPlayer);
}
//�������
int CTableFrame::PlayerSitTable(WORD wChairID, CPlayer * pPlayer, LPCTSTR lpszPassword, bool bCheckUserGPS)
{
	//1. У��
	int ret = CanPlayerSitTable(pPlayer, wChairID);
	if (ret !=0) return ret;

	//2. Table
	m_player_list[wChairID] = pPlayer;
	m_user_list[wChairID] = pPlayer;

	//3. Player
	pPlayer->SetUserStatus(US_SIT, m_wTableID, wChairID);

	//4. ��client��������
	CMD_GF_GameStatus GameStatus;
	ZeroMemory(&GameStatus, sizeof(GameStatus));

	//��ֵ
	GameStatus.cbUserAction = pPlayer->GetUserStatus();
	tagUserInfo *pUserInfo = pPlayer->GetUserInfo();
	CopyMemory(&GameStatus.UserInfo, pUserInfo, sizeof(tagUserInfo));

	//�㲥����
	SendTableData(INVALID_CHAIR, SUB_GR_USER_STATUS, &GameStatus, sizeof(GameStatus), MDM_USER);

	return 0;
}
//���վ��
bool CTableFrame::PlayerUpTable(CPlayer *pPlayer)
{
	//1, У��
	int ret = CanPlayerUpTable(pPlayer);
	if(ret !=0 ) return ret;

	//2. Table
	WORD wChairID = pPlayer->GetChairID();
	m_player_list.at(wChairID) = NULL;

	//3. Player
	pPlayer->SetUserStatus(US_IN_TABLE, m_wTableID, INVALID_CHAIR);

	//���͸��ͻ���
	CMD_GF_GameStatus GameStatus;
	ZeroMemory(&GameStatus, sizeof(GameStatus));

	//��ֵ
	GameStatus.cbUserAction = pPlayer->GetUserStatus();
	tagUserInfo *pUserInfo = pPlayer->GetUserInfo();
	CopyMemory(&GameStatus.UserInfo, pUserInfo, sizeof(tagUserInfo));

	//�㲥����
	SendTableData(INVALID_CHAIR, SUB_GR_USER_STATUS, &GameStatus, sizeof(GameStatus), MDM_USER);

	return true;
}
//����뿪
int CTableFrame::PlayerLeaveTable(CPlayer* pPlayer)
{
	//1. У��
	int ret = CanPlayerLeaveTable(pPlayer);
	if(ret!=0) return ret;

	//3. Player
	pPlayer->SetUserStatus(US_FREE, INVALID_TABLE, INVALID_CHAIR);

	//���͸��ͻ���
	CMD_GF_GameStatus GameStatus;
	ZeroMemory(&GameStatus, sizeof(GameStatus));

	//��ֵ
	GameStatus.cbUserAction = pPlayer->GetUserStatus();
	tagUserInfo *pUserInfo = pPlayer->GetUserInfo();
	CopyMemory(&GameStatus.UserInfo, pUserInfo, sizeof(tagUserInfo));

	//�㲥����
	SendTableData(INVALID_CHAIR, SUB_GR_USER_STATUS, &GameStatus, sizeof(GameStatus), MDM_USER);

	//2. Table
	auto ite1 = find(m_user_list.begin(), m_user_list.end(), pPlayer);
	if (ite1 != m_user_list.end())
	{
		*ite1 = NULL;
	}

	auto ite2 = find(m_player_list.begin(), m_player_list.end(), pPlayer);
	if (ite2 != m_player_list.end())
	{
		*ite2 = NULL;
	}
}
//���׼��
int CTableFrame::PlayerReady(WORD wChairID, CPlayer* pPlayer) 
{
	//׼��У��
	int ret = CanPlayerReady(pPlayer);
	if (ret != 0)	return ret;

	//��ȡ����
	DWORD	dwTableID = pPlayer->GetTableID();

	//�����û�״̬Ϊ׼��
	pPlayer->SetUserStatus(US_READY, dwTableID, wChairID);

	//���ͻ��˷���׼����Ϊ
	CMD_GF_GameStatus GameStatus;
	ZeroMemory(&GameStatus, sizeof(GameStatus));

	//��ֵ
	GameStatus.cbUserAction = pPlayer->GetUserStatus();
	tagUserInfo *pUserInfo = pPlayer->GetUserInfo();
	CopyMemory(&GameStatus.UserInfo, pUserInfo, sizeof(tagUserInfo));

	//�㲥����
	SendTableData(INVALID_CHAIR, SUB_GR_USER_STATUS, &GameStatus, sizeof(GameStatus), MDM_USER);

	//�ж���������Ƿ�׼��
	WORD ReadyNum = 0;
	for (auto it = m_user_list.begin(); it != m_user_list.end(); it++)
	{
		if ((*it) != NULL && (US_READY == (*it)->GetUserStatus()))
			ReadyNum++;
	}

	CLog::Log(log_debug, "ReadyNum: %d", ReadyNum);

	//�������׼�� ��ʼ��Ϸ	//��������Ϸ����
	if (ReadyNum >= m_tagTableRule.PlayerCount)
		StartGame();

	return 0;
}
//��Ҷ���
int CTableFrame::PlayerOffline(CPlayer* pPlayer) 
{
	CLog::Log(log_debug, "���%ld ����", pPlayer->GetUserID());

	//�����û�״̬Ϊ����
	pPlayer->SetUserStatus(US_OFFLINE, m_wTableID, pPlayer->GetChairID());

	//���ͻ��˷���׼����Ϊ
	CMD_GF_GameStatus GameStatus;
	ZeroMemory(&GameStatus, sizeof(GameStatus));

	//��ֵ
	GameStatus.cbUserAction = pPlayer->GetUserStatus();
	tagUserInfo *pUserInfo = pPlayer->GetUserInfo();
	CopyMemory(&GameStatus.UserInfo, pUserInfo, sizeof(tagUserInfo));

	//�㲥����
	SendTableData(INVALID_CHAIR, SUB_GR_USER_STATUS, &GameStatus, sizeof(GameStatus), MDM_USER);

	return 0;
}

//����ܷ����
int CTableFrame::CanPlayerEnterTable(CPlayer* pPlayer)
{
	return 0;
}
//����ܷ�����
int CTableFrame::CanPlayerSitTable(CPlayer* pPlayer, WORD &wChairID)
{
	//1. ָ��У��
	if (NULL == pPlayer)
	{
		return PLAYER_NOT_EXISIT;
	}

	//2. �����ж�
	if(wChairID == INVALID_CHAIR)
	{
		wChairID = GetNullChairID(); //clientû��ָ��,�����һ��
	}
	if(wChairID >= m_wChairCount) return TABLE_FULL;

	//3. �ж��������Ƿ�����
	CPlayer *pCurPlayer = GetTableUserItem(wChairID);		//�����û����϶�ΪNULL������У���û��ǲ����Ѿ���������
	if ( (NULL != pCurPlayer) && 
		(pPlayer->GetUserID() != pCurPlayer->GetUserID()) )
	{
		return CHAIR_USERD;
	}

	return 0;
}
//����ܷ�վ��
int CTableFrame::CanPlayerUpTable(CPlayer* pPlayer)
{
	//ָ��У��
	if(pPlayer == NULL) return PLAYER_NOT_EXISIT;

	//�������������
	if(pPlayer->GetTableID() != m_wTableID) return TABLE_NOT_THIS;

	//����λ�ò���ȷ -- �Թ��û�û��Ҫ����
	if(pPlayer->GetChairID() >= m_wChairCount)  return CHAIR_INVALID;

	return 0;
}
//����ܷ��뿪
int CTableFrame::CanPlayerLeaveTable(CPlayer* pPlayer)
{
	//У��
	if (pPlayer == NULL)
		return 2;

	//����������Ϸ�� �����뿪
	if (US_PLAYING == pPlayer->GetUserStatus())
		return 1;

	return 0;
}
////����Ƿ���׼��
int CTableFrame::CanPlayerReady(CPlayer* pPlayer) 
{
	//У��
	if (pPlayer == NULL)  return PLAYER_NOT_EXISIT;

	//ֻ�����²���׼��
	if (pPlayer->GetUserStatus() != US_SIT)	return STATUS_ERR;

	return 0;
}

/*************************************    ������    *************************************************/
//д��¼���¼ ���� С����,���ݺͳ���
bool CTableFrame::WriteRecordInfo(WORD wXJCount,TCHAR strScore[], VOID* pData, DWORD dwDataSize)
{
	//��ӱ�����Ϣ:����� ��Ϸģʽ ����
	DBR_GR_GameRecordInfo GameRecordInfo;
	ZeroMemory(&GameRecordInfo,sizeof(DBR_GR_GameRecordInfo));

	//��ֵ
	GameRecordInfo.dwTableID = GetTableID();	
	GameRecordInfo.wCurrentCount = wXJCount;

	if(dwDataSize>LEN_MAX_RECORD_SIZE)
	{
		dwDataSize = LEN_MAX_RECORD_SIZE;
	}

	memcpy_s(GameRecordInfo.szData, dwDataSize*sizeof(BYTE), pData, dwDataSize*sizeof(BYTE));

	//д�����ݿ�
	g_GameCtrl->PostDataBaseRequest(DBR_GR_SAVE_RECORDINFO, 0, &GameRecordInfo, sizeof(DBR_GR_GameRecordInfo));

	return true;
}

//��Ϸ�û�
CPlayer * CTableFrame::GetTableUserItem(WORD wChairID)
{
	//Ч�����
	if (wChairID>=m_wChairCount) return NULL;

	//��ȡ�û�
	return m_player_list[wChairID];
}

//�Թ��û�
CPlayer * CTableFrame::EnumLookonUserItem(WORD wEnumIndex)
{
	if (wEnumIndex>=m_user_list.size()) return NULL;
	return m_user_list[wEnumIndex];
}

//����ʱ��
bool CTableFrame::SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter)
{
	//Ч�����
	if ((dwTimerID<=0)||(dwTimerID>TIME_TABLE_MODULE_RANGE)) return false;

	//����ʱ��
	DWORD dwEngineTimerID=IDI_TABLE_MODULE_START + m_wTableID*TIME_TABLE_MODULE_RANGE;
	g_GameCtrl->SetTimer(dwEngineTimerID+dwTimerID,dwElapse,dwRepeat,dwBindParameter);

	return true;
}

//ɾ��ʱ��
bool CTableFrame::KillGameTimer(DWORD dwTimerID)
{
	//Ч�����
	if ((dwTimerID<=0)||(dwTimerID>TIME_TABLE_MODULE_RANGE)) return false;

	//ɾ��ʱ��
	DWORD dwEngineTimerID=IDI_TABLE_MODULE_START + m_wTableID*TIME_TABLE_MODULE_RANGE;
	g_GameCtrl->KillTimer(dwEngineTimerID+dwTimerID);

	return true;
}

//��ȡ��ʱ��ʣ�������
DWORD CTableFrame::GetTimerLeftTickCount(DWORD dwTimerID)
{
	//Ч�����
	if ((dwTimerID<=0)||(dwTimerID>TIME_TABLE_MODULE_RANGE)) return false;

	//ɾ��ʱ��
	DWORD dwEngineTimerID=IDI_TABLE_MODULE_START + m_wTableID*TIME_TABLE_MODULE_RANGE;
	return g_GameCtrl->GetTimerLeftTickCount(dwEngineTimerID+dwTimerID);
	return 0;
}

//��������
bool CTableFrame::SendTableData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize, WORD wMainCmdID)
{
	//�û�Ⱥ��
	if (wChairID == INVALID_CHAIR)
	{
		for (auto it = m_user_list.begin(); it != m_user_list.end(); it++)   //�����Թ��û�Ҳ�ᷢ��
		{
			if ((*it) != NULL)
			{
				g_GameCtrl->SendData((*it), wMainCmdID, wSubCmdID, pData, wDataSize);
			}
		}

		return true;
	}
	else
	{
		//��ȡ�û�
		CPlayer * pIServerUserItem = GetTableUserItem(wChairID);

		if ((pIServerUserItem == NULL)) return false;
		//��������
		g_GameCtrl->SendData(pIServerUserItem, wMainCmdID, wSubCmdID, pData, wDataSize);

		return true;
	}

	return false;
}

//������Ϣ
bool CTableFrame::SendGameMessage(LPCTSTR lpszMessage, WORD wType)
{
	//��������
	WORD wEnumIndex=0;

	//������Ϣ
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//��ȡ�û�
		CPlayer * pIServerUserItem=GetTableUserItem(i);
		if ((pIServerUserItem==NULL)) continue;

		//������Ϣ
		g_GameCtrl->SendGameMessage(pIServerUserItem,lpszMessage,wType);
	}

	//ö���û�
	do
	{
		//��ȡ�û�
		CPlayer * pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if (pIServerUserItem==NULL) break;

		//������Ϣ
		g_GameCtrl->SendGameMessage(pIServerUserItem,lpszMessage,wType);

	} while (true);

	return true;
}

//������Ϣ
bool CTableFrame::SendRoomMessage(CPlayer * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//�û�Ч��
	if (pIServerUserItem==NULL) return false;
	//������Ϣ
	g_GameCtrl->SendRoomMessage(pIServerUserItem,lpszMessage,wType);

	return true;
}

//��Ϸ��Ϣ
bool CTableFrame::SendGameMessage(CPlayer * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//�û�Ч��
	if (pIServerUserItem==NULL) return false;

	//������Ϣ
	return g_GameCtrl->SendGameMessage(pIServerUserItem,lpszMessage,0);
}


//���ͳ���
bool CTableFrame::SendGameScene(IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	//�û�Ч��
	if ((pIServerUserItem==NULL)) return false;

	g_GameCtrl->SendData((CPlayer* )pIServerUserItem,MDM_G_FRAME,CMD_GR_FRAME_GAME_OPTION,pData,wDataSize);

	return true;
}

//ʱ���¼�
bool CTableFrame::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//�ص��¼�
	if ((dwTimerID>=0)&&(dwTimerID<TIME_TABLE_SINK_RANGE))
	{
		ASSERT(m_pITableFrameSink!=NULL);
		return m_pITableFrameSink->OnTimerMessage(dwTimerID,dwBindParameter);
	}

	//�¼�����
	switch (dwTimerID)
	{
	case IDI_ROOM_AUTO_DISMISS:			//�����Զ���ɢ��ʱ��
		{
			KillGameTimer(IDI_ROOM_AUTO_DISMISS);

			//������Ϸ
			HandleDJGameEnd(GAME_CONCLUDE_NORMAL);
			return true;
		}
	case IDI_VOTE_DISMISS_ROOM:		//�����ɢ���䶨ʱ��
		{
			KillGameTimer(IDI_VOTE_DISMISS_ROOM);
			//Ĭ��ͬ���ɢ
			for (int i=0; i<m_wChairCount; i++)
			{
				CPlayer *pTableUserItem = GetTableUserItem(i);
				if(pTableUserItem == NULL) continue;
				if(m_bResponseDismiss[i]) continue;
				OnEventVoteDismissRoom(i,true);
			}
			return true;
		}
	case IDI_CHECK_DISMISS_ROOM:	//��Ʊ�����Լ�����Ϸʱ�� ��ɢ����
		{
			KillGameTimer(IDI_CHECK_DISMISS_ROOM);

#pragma region ��ɢ����
			//���÷��䴦�ڽ�ɢ״̬
			SetDismissState(true);

			//֪ͨ�ͻ��˵�����ֽ������
			STR_CMD_GR_FRAME_DISMISS_RESULT DismissResult;
			ZeroMemory(&DismissResult, sizeof(DismissResult));
			DismissResult.cbDismiss = 1;
			
			SendTableData(INVALID_CHAIR, CMD_GR_USER_DISMISS_RESULT, &DismissResult, sizeof(STR_CMD_GR_FRAME_DISMISS_RESULT));

			//֪ͨ����Ϸ��ɢ����
			//����״̬ ֱ�ӽ�ɢ
			if(GAME_STATUS_FREE == GetGameStatus())
			{
				HandleDJGameEnd(GAME_CONCLUDE_NORMAL);
			}
			//��Ϸ״̬�� ֪ͨ����Ϸ
			else
			{
				OnEventClubDismissRoom();
			}
			return true;
#pragma endregion
		}
	case IDI_WAIT_CONTINUE:
		{
			KillGameTimer(IDI_WAIT_CONTINUE);

			HandleDJGameEnd(0xFE);

			return true;
		}
	case IDI_WAIT_REDPACKET:
		{
			SYSTEMTIME sysTime; 
			GetSystemTime(&sysTime);

			if((sysTime.wHour == 8 || sysTime.wHour == 14 || sysTime.wHour == 20)
				&& sysTime.wMinute == 0)
			{
				//�������������û����ͳ���֪ͨ
				SendTableData(INVALID_CHAIR,SUB_GR_LOTTERY_NOTIFY,NULL,0);
			}
			return true;
		}
	}

	//�������
	ASSERT(FALSE);

	return false;
}

//��Ϸ�¼�
bool CTableFrame::OnEventSocketGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, CPlayer * pIServerUserItem)
{
	//Ч�����
	bool ret = m_pITableFrameSink->OnGameMessage(wSubCmdID,pData,wDataSize,GetPlayerChair(pIServerUserItem));
	if(!ret)
	{
	}

	//��Ϣ����
	return ret;
}

//����¼�
bool CTableFrame::OnEventSocketFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, CPlayer * pIServerUserItem)
{
	//��Ϸ���� �˴������Ӧ����ֻ��200������Ϣ�� TODONOW ϸ��	����
	//if (m_pITableFrameSink->OnFrameMessage(wSubCmdID,pData,wDataSize,GetPlayerChair(pIServerUserItem))==true) return true;

	//Ĭ�ϴ���
	switch (wSubCmdID)
	{
	case SUB_RG_FRAME_OPTION:	//��Ϸ����
		{
			//��������
			CMD_GF_GameOption * pGameOption=(CMD_GF_GameOption *)pData;

			//���ͷ������
			STR_CMD_ROOM_RULE room_rule;
			memcpy(&room_rule.common, &m_tagTableRule, sizeof(room_rule.common));
			room_rule.TableID = m_wTableID;
			g_GameCtrl->SendData(pIServerUserItem, MDM_USER, CMD_ROOM_RULE, &room_rule, sizeof(room_rule));

			// 1����������ҷ����Լ�������
			WORD wChairID = pIServerUserItem->GetChairID();
			BYTE cbUserStatus = pIServerUserItem->GetUserStatus();

			//����״̬
			CMD_GF_GameStatus GameStatus1;
			GameStatus1.cbUserAction = cbUserStatus;

			tagUserInfo *pUserInfo = pIServerUserItem->GetUserInfo();
			memset(&GameStatus1.UserInfo, 0, sizeof(tagUserInfo));
			memcpy(&GameStatus1.UserInfo, pUserInfo, sizeof(tagUserInfo));

			std::wcout << GameStatus1.UserInfo.szHeadUrl << std::endl;

			g_GameCtrl->SendData(pIServerUserItem, MDM_USER, SUB_GR_USER_STATUS, &GameStatus1, sizeof(GameStatus1));

			// 2����Ҽ��뷿��  ���ڷ���������������Ϣ���͸������
			for (auto it = m_user_list.begin(); it != m_user_list.end(); it++)
			{
				if (((*it) != NULL) && (*it != pIServerUserItem))
				{
					//��ȡ����
					WORD wChairID = (*it)->GetChairID();
					BYTE cbUserStatus = (*it)->GetUserStatus();

					//����״̬
					CMD_GF_GameStatus GameStatus;
					GameStatus.cbUserAction = cbUserStatus;

					tagUserInfo *pUserInfo = (*it)->GetUserInfo();
					memset(&GameStatus.UserInfo, 0, sizeof(tagUserInfo));
					memcpy(&GameStatus.UserInfo, pUserInfo, sizeof(tagUserInfo));

					std::wcout << GameStatus.UserInfo.szHeadUrl << std::endl;
					CLog::Log(log_debug, "wChairID: %d", GameStatus.UserInfo.wChairID);

					g_GameCtrl->SendData(pIServerUserItem, MDM_USER, SUB_GR_USER_STATUS, &GameStatus, sizeof(GameStatus));
				}
			}

			//���ͳ���
			m_pITableFrameSink->OnEventSendGameScene(wChairID, pIServerUserItem, m_cbGameStatus, true);

			
			//���ͽ�ɢ���״̬ -- ֻ�д��ڽ�ɢ״̬ �Żᷢ��
			if(m_bUnderDissState)
			{
				STR_CMD_GR_FRAME_GAME_DISSMISS cmd_dismiss;
				ZeroMemory(&cmd_dismiss, sizeof(cmd_dismiss));

				BYTE num = 0;

				//������ �ŵ���һλ
				if (m_dissmisserChaiID != 0xFF)
				{
					CPlayer *pTableUserItem = GetTableUserItem(m_dissmisserChaiID);
					if(pTableUserItem != NULL)
					{
						cmd_dismiss.dwUserID[0] = pTableUserItem->GetUserID();

						if(!m_bResponseDismiss[m_dissmisserChaiID]) //δ���
						{
							cmd_dismiss.cbAgree[0] = 2;
						}
						else if(m_bAgree[m_dissmisserChaiID]) //ͬ��
						{
							cmd_dismiss.cbAgree[0] = 1;
						}
						else
						{
							cmd_dismiss.cbAgree[0] = 0;
						}

						num ++;	
					}
				}

				for(int i=0; i<m_wChairCount && num <=m_wChairCount; i++)
				{
					if(i == m_dissmisserChaiID) continue;

					CPlayer *pTableUserItem = GetTableUserItem(i);
					if(pTableUserItem == NULL) continue;

					cmd_dismiss.dwUserID[num] = pTableUserItem->GetUserID();

					if(!m_bResponseDismiss[i]) //δ���
					{
						cmd_dismiss.cbAgree[num] = 2;
					}
					else if(m_bAgree[i]) //ͬ��
					{
						cmd_dismiss.cbAgree[num] = 1;
					}
					else
					{
						cmd_dismiss.cbAgree[num] = 0;
					}

					num++;
				}

				g_GameCtrl->SendData(pIServerUserItem,MDM_G_FRAME,CMD_GR_FRAME_GAME_DISSMISS,&cmd_dismiss,sizeof(cmd_dismiss));
			}
			return true;
		}
	case SUB_RG_FRAME_CHAT:		//�û�����
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(STR_SUB_RG_FRAME_CHAT));
			if (wDataSize != sizeof(STR_SUB_RG_FRAME_CHAT)) 
				return false;

			//��������
			STR_SUB_RG_FRAME_CHAT *pTableChat = (STR_SUB_RG_FRAME_CHAT *)pData;

			//��ȡ�û�
			CPlayer *pIServerUserItem = CPlayerManager::FindPlayerByID(pTableChat->dwUserID);
			if ( NULL == pIServerUserItem)
				return true;

			//���췵������
			STR_CMD_GR_FRAME_CHAT CMDChat;
			ZeroMemory(&CMDChat, sizeof(STR_CMD_GR_FRAME_CHAT));

			//��ֵ
			CMDChat.dwChairID = pIServerUserItem->GetChairID();
			CMDChat.dwMsgID = pTableChat->dwMsgID;
			CMDChat.dwUserID = pTableChat->dwUserID;

			//��������
			SendTableData(INVALID_CHAIR, CMD_GR_FRAME_CHAT, &CMDChat, sizeof(STR_CMD_GR_FRAME_CHAT));

			return true;
		}	
	case SUB_GF_LOOKON_CONFIG:		//�Թ�����
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GF_LookonConfig));
			if (wDataSize<sizeof(CMD_GF_LookonConfig)) return false;

			//��������
			CMD_GF_LookonConfig * pLookonConfig=(CMD_GF_LookonConfig *)pData;

			//Ŀ���û�
			if ((pLookonConfig->dwUserID!=0)&&(CPlayerManager::FindPlayerByID(pLookonConfig->dwUserID)==NULL))
			{
				ASSERT(FALSE);
				return true;
			}

			//�Թ۴���
			if (pLookonConfig->dwUserID!=0L)
			{
				for (INT_PTR i=0;i<m_user_list.size();i++)
				{
					//��ȡ�û�
					CPlayer * pILookonUserItem=m_user_list[i];
					if (pILookonUserItem->GetUserID()!=pLookonConfig->dwUserID) continue;
					if (pILookonUserItem->GetChairID()!=pIServerUserItem->GetChairID()) continue;

					//������Ϣ
					CMD_GF_LookonStatus LookonStatus;
					LookonStatus.cbAllowLookon=pLookonConfig->cbAllowLookon;

					//������Ϣ
					g_GameCtrl->SendData(pILookonUserItem,MDM_G_FRAME,CMD_GF_LOOKON_STATUS,&LookonStatus,sizeof(LookonStatus));

					break;
				}
			}
			else
			{
				//������Ϣ
				CMD_GF_LookonStatus LookonStatus;
				LookonStatus.cbAllowLookon=pLookonConfig->cbAllowLookon;

				//������Ϣ
				for (INT_PTR i=0;i<m_user_list.size();i++)
				{
					//��ȡ�û�
					CPlayer * pILookonUserItem=m_user_list[i];
					if (pILookonUserItem->GetChairID()!=pIServerUserItem->GetChairID()) continue;

					//������Ϣ
					g_GameCtrl->SendData(pILookonUserItem,MDM_G_FRAME,CMD_GF_LOOKON_STATUS,&LookonStatus,sizeof(LookonStatus));
				}
			}

			return true;
		}
	//case SUB_RG_FRAME_ASK_DISMISS:		//�����ɢ����
	//	{
	//		//�û�У��
	//		if (pIServerUserItem == NULL)
	//		return false;

	//		STR_SUB_RG_FRAME_ASK_DISMISS *pApply = (STR_SUB_RG_FRAME_ASK_DISMISS*)pData;

	//		CLog::Log(log_debug, "DISMISS UserID: %d", pIServerUserItem->GetUserID());

	//		//2. У�鷿���Ƿ��Ѿ����ڽ�ɢ״̬
	//		if (m_bUnderDissState)
	//		{
	//			return true;
	//		}

	//		//���µ����ɢ��ť����Ҫɾ����ʱ������ֹһ���˲�ͣ�����ɢ����ʱ�����Զ��˳�����
	//		KillGameTimer(IDI_VOTE_DISMISS_ROOM);

	//		OnEventApplyDismissRoom(pIServerUserItem->GetChairID(), true); //0Ϊfalse  ����Ϊtrue

	//		return true;
	//	}
	//case SUB_RG_FRAME_VOTE_DISMISS:		//�����ɢ����
	//	{
	//		//���ݰ�У��
	//		if ( wDataSize != sizeof(STR_SUB_RG_FRAME_VOTE_DISMISS) )
	//			return false;

	//		//��������
	//		STR_SUB_RG_FRAME_VOTE_DISMISS *pVote = (STR_SUB_RG_FRAME_VOTE_DISMISS*)pData;

	//		//��ɢ�ɹ�
	//		OnEventVoteDismissRoom(pIServerUserItem->GetChairID(), (pVote->cbAgree != 0 ));
	//		return true;

	//	}
	case SUB_CG_COM_CHECK_USER_GPS:		//����У��GPS
		{
			//���ݰ�У��
			if ( wDataSize != sizeof(STR_SUB_CG_COM_CHECK_USER_GPS) )
				return false;

			//��������
			STR_SUB_CG_COM_CHECK_USER_GPS *pCheck = (STR_SUB_CG_COM_CHECK_USER_GPS*)pData;

			//У��
			if (1 != pCheck->cbRequestFlag)
				return true;

			//����
			CheckUserDistance();
			return true;
		}
	case SUB_CG_COM_CHECK_USER_IP:		//����У��IP
		{
			//���ݰ�У��
			if ( wDataSize != sizeof(STR_SUB_CG_COM_CHECK_USER_IP) )
				return false;

			//��������
			STR_SUB_CG_COM_CHECK_USER_IP *pCheck = (STR_SUB_CG_COM_CHECK_USER_IP*)pData;

			//У��
			if (1 != pCheck->cbRequestFlag)
				return true;

			//����
			CheckUserIpAddress();
			return true;
		}
	}

	return false;
}

//��ȡ��λ
WORD CTableFrame::GetNullChairID()
{
	//��������
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_player_list[i]==NULL)
		{
			return i;
		}
	}

	return INVALID_CHAIR;
}

//�û���Ŀ
WORD CTableFrame::GetSitUserCount()
{
	//��������
	WORD wUserCount=0;

	//��Ŀͳ��
	for (WORD i=0; i<m_wChairCount; i++)
	{
		if (GetTableUserItem(i)!=NULL)
		{
			wUserCount++;
		}
	}

	return wUserCount;
}

WORD CTableFrame::GetPlayerChair(CPlayer* pPlayer)
{
	for(int i =0; i<m_player_list.size(); i++)
	{
		if(m_player_list.at(i) == pPlayer)
		{
			return i;
		}
	}

	return INVALID_CHAIR;
}

//����ʧ��
bool CTableFrame::SendRequestFailure(CPlayer * pIServerUserItem, LPCTSTR pszDescribe, LONG lErrorCode)
{
	//��������
	CMD_GR_RequestFailure RequestFailure;
	ZeroMemory(&RequestFailure,sizeof(RequestFailure));

	//��������
	RequestFailure.lErrorCode=lErrorCode;
	lstrcpyn(RequestFailure.szDescribeString,pszDescribe,CountArray(RequestFailure.szDescribeString));

	//��������
	WORD wDataSize=CountStringBuffer(RequestFailure.szDescribeString);
	WORD wHeadSize=sizeof(RequestFailure)-sizeof(RequestFailure.szDescribeString);
	g_GameCtrl->SendData(pIServerUserItem,MDM_USER,SUB_GR_REQUEST_FAILURE,&RequestFailure,wHeadSize+wDataSize);

	std::wcout << "TF: " << pszDescribe << std::endl;

	return true;
}

//��ʼЧ��
bool CTableFrame::EfficacyStartGame(WORD wReadyChairID)
{
	TCHAR szString[512]=TEXT("");

	//����ģʽ����Ϸ������������ʼ
	if (((tagTableRule*)GetCustomRule())->GameMode == TABLE_MODE_DW)
	{
		return true;
	}  

	//׼������
	WORD wReadyUserCount=0;
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//��ȡ�û�
		CPlayer * pITableUserItem=GetTableUserItem(i);
		if (pITableUserItem==NULL) continue;

		//�û�ͳ��
		if (pITableUserItem!=NULL)
		{
			//׼��״̬����� ����
			if (pITableUserItem->GetUserStatus()==US_READY)
			{
				wReadyUserCount++;
			}
		}
	}

	//�����ж�
	if (wReadyUserCount==m_wChairCount)
	{
		return true;
	}

	/*
	/��Կ�ʼ
	{
	//��Ŀ�ж�
	if (wReadyUserCount==m_wChairCount) return true;
	if ((wReadyUserCount<2L)||(wReadyUserCount%2)!=0) return false;

	//λ���ж�
	for (WORD i=0;i<m_wChairCount;i++)
	{
	//��ȡ�û�
	CPlayer * pICurrentUserItem=GetTableUserItem(i);
	CPlayer * pITowardsUserItem=GetTableUserItem(i+m_wChairCount/2);

	//λ�ù���
	if ((pICurrentUserItem==NULL)&&(pITowardsUserItem!=NULL)) return false;
	if ((pICurrentUserItem!=NULL)&&(pITowardsUserItem==NULL)) return false;
	}
	*/

	return false;
}

//��ַЧ��
bool CTableFrame::EfficacyIPAddress(CPlayer * pIServerUserItem)
{
	//����Ա��������
	if(pIServerUserItem->GetMasterOrder()!=0) return true;

	//�����ж�
	//	if (CServerRule::IsForfendGameRule(m_pGameServiceOption->dwServerRule)==true) return true;

	/*
	//��ַЧ��
	const tagUserRule * *pTableUserRule=NULL;
	bool bCheckSameIP=pUserRule->bLimitSameIP;
	for (WORD i=0;i<m_wChairCount;i++)
	{
	//��ȡ�û�
	CPlayer * pITableUserItem=GetTableUserItem(i);
	if (pITableUserItem!=NULL && (!pITableUserItem->IsAndroidUser()) && (pITableUserItem->GetMasterOrder()==0))
	{
	pTableUserRule=pITableUserItem->GetUserRule();
	if (pTableUserRule->bLimitSameIP==true) 
	{
	bCheckSameIP=true;
	break;
	}
	}
	}

	//��ַЧ��
	if (bCheckSameIP==true)
	{
	DWORD dwUserIP=pIServerUserItem->GetClientAddr();
	for (WORD i=0;i<m_wChairCount;i++)
	{
	//��ȡ�û�
	CPlayer * pITableUserItem=GetTableUserItem(i);
	if ((pITableUserItem!=NULL)&&(pITableUserItem != pIServerUserItem)&&(!pITableUserItem->IsAndroidUser())&&(pITableUserItem->GetMasterOrder()==0)&&(pITableUserItem->GetClientAddr()==dwUserIP))
	{
	if (!pUserRule->bLimitSameIP)
	{
	//������Ϣ
	LPCTSTR pszDescribe=TEXT("����Ϸ����������˲�����ͬ IP ��ַ�������Ϸ���� IP ��ַ�����ҵ� IP ��ַ��ͬ�����ܼ�����Ϸ��");
	SendRequestFailure(pIServerUserItem,pszDescribe,REQUEST_FAILURE_NORMAL);
	return false;
	}
	else
	{
	//������Ϣ
	LPCTSTR pszDescribe=TEXT("�������˲�����ͬ IP ��ַ�������Ϸ������Ϸ���������� IP ��ַ��ͬ����ң����ܼ�����Ϸ��");
	SendRequestFailure(pIServerUserItem,pszDescribe,REQUEST_FAILURE_NORMAL);
	return false;
	}
	}
	}
	for (WORD i=0;i<m_wChairCount;i++)
	{
	//��ȡ�û�
	CPlayer * pITableUserItem=GetTableUserItem(i);
	if (pITableUserItem!=NULL && (!pITableUserItem->IsAndroidUser()) && (pITableUserItem->GetMasterOrder()==0))
	{
	for (WORD j=i+1;j<m_wChairCount;j++)
	{
	//��ȡ�û�
	CPlayer * pITableNextUserItem=GetTableUserItem(j);
	if ((pITableNextUserItem!=NULL) && (!pITableNextUserItem->IsAndroidUser()) && (pITableNextUserItem->GetMasterOrder()==0)&&(pITableUserItem->GetClientAddr()==pITableNextUserItem->GetClientAddr()))
	{
	LPCTSTR pszDescribe=TEXT("�������˲�����ͬ IP ��ַ�������Ϸ������Ϸ������ IP ��ַ��ͬ����ң����ܼ�����Ϸ��");
	SendRequestFailure(pIServerUserItem,pszDescribe,REQUEST_FAILURE_NORMAL);
	return false;
	}
	}
	}
	}
	}
	*/
	return true;
}

//У���û�GPS����
bool CTableFrame::CheckUserDistance()
{
	//���������ϵ������û��������Ƚ�
	std::vector<STR_CMD_CG_COM_CHECK_USER_GPS> vecCheckGPS;

	//��������
	for(int i=0; i<m_wChairCount; i++)
	{
		//��ø��û���λ����Ϣ
		CPlayer *pCheckUserItem = GetTableUserItem(i);
		if(NULL == pCheckUserItem) continue;
		double dCheckUserLong = pCheckUserItem->GetUserInfo()->dLongitude;
		double dCheckUserLat  = pCheckUserItem->GetUserInfo()->dLatitude;

		//У��λ����Ϣ
		if( (dCheckUserLong<0.01 && dCheckUserLong>-0.01) || 
			(dCheckUserLat<0.01 && dCheckUserLat>-0.01))
		{
			continue;
		}

		//�����������û��ľ���
		for (int j = i+1; j < m_wChairCount; j++)
		{
			//У�������û�
			CPlayer *pTableUserItem = GetTableUserItem(j);
			if(NULL == pTableUserItem) continue;

			//��������û�λ��
			double dTableUserLong = pTableUserItem->GetUserInfo()->dLongitude;
			double dTableUserLat = pTableUserItem->GetUserInfo()->dLatitude;

			//У���û�λ����Ϣ
			if( (dTableUserLong<0.01 && dTableUserLong>-0.01) || 
				(dTableUserLat<0.01 && dTableUserLat>-0.01))
			{
				continue;
			}

			//�Ƚ����߾���
			double Distance = GetDistance(dCheckUserLong, dCheckUserLat, dTableUserLong, dTableUserLat);

			//�����������ͻ���
			STR_CMD_CG_COM_CHECK_USER_GPS CheckGPS;
			ZeroMemory(&CheckGPS, sizeof(STR_CMD_CG_COM_CHECK_USER_GPS));

			//��ֵ
			CheckGPS.fDistance = Distance;
			CheckGPS.dwFirstUserID = pCheckUserItem->GetUserID();
			CheckGPS.dwNextUserID = pTableUserItem->GetUserID();
			CheckGPS.cbIsCloser = (CHECK_USER_GPS_DISTANCE > Distance) ? 1 : 0;
			vecCheckGPS.push_back(CheckGPS);
		}
	}

	//��������
	for (WORD i =0; i<m_wChairCount; i++)
	{
		//��ȡ�û�
		CPlayer *pIServerUserItem = GetTableUserItem(i);
		if (pIServerUserItem==NULL) continue;

		//��������
		for (DWORD j = 0; j < vecCheckGPS.size(); j++)
		{
			g_GameCtrl->SendData(pIServerUserItem, MDM_G_FRAME, CMD_GC_COM_CHECK_USER_GPS, &vecCheckGPS[j], sizeof(STR_CMD_CG_COM_CHECK_USER_GPS));
		}	
	}


	return true;
}

//���ݾ�γ�ȼ�����룬��λm
double CTableFrame::GetDistance(double long1, double lat1, double long2, double lat2)
{
	//��������
	double dEarthRadius = 6378137;
	double dPi = 3.1415926;
	double distance = 0.0;

	//ת���ɻ���
	lat1 = lat1 * dPi / 180.0;
	lat2 = lat2 * dPi / 180.0;

	double dLatTheta = fabs(lat1 - lat2);
	double dLongTheta = fabs((long1 - long2)*dPi / 180.0);

	double sa2 = sin(dLatTheta / 2.0);
	double sb2 = sin(dLongTheta / 2.0);

	//�������
	distance = fabs( 2 * dEarthRadius * asin( sqrt(sa2*sa2 + cos(lat1)*cos(lat2)*sb2*sb2) ) );

	return distance;
}

//�����û���IP��ַ
bool CTableFrame::CheckUserIpAddress()
{
	//���������ϵ������û��������Ƚ�
	std::vector<STR_CMD_CG_COM_CHECK_USER_IP> vecCheckIP;

	//�����������
	for (WORD i=0; i<m_wChairCount ; i++)
	{
		//�û�IP��ַ
		CPlayer *pCheckUserItem = GetTableUserItem(i);
		if (NULL == pCheckUserItem) continue;
		DWORD dwUserIP = pCheckUserItem->GetClientAddr();

		//�������û��Ƚ�IP
		for (int j = i+1; j < m_wChairCount; j++)
		{
			//��ȡ�û�IP
			CPlayer * pITableUserItem = GetTableUserItem(j);
			if ( (pITableUserItem != NULL) &&
				(pITableUserItem != pCheckUserItem) &&
				(!pITableUserItem->IsAndroidUser()) &&
				(pITableUserItem->GetClientAddr()==dwUserIP) )
			{
				//�����������ͻ���
				STR_CMD_CG_COM_CHECK_USER_IP CheckIP;
				ZeroMemory(&CheckIP, sizeof(STR_CMD_CG_COM_CHECK_USER_IP));

				//��ֵ
				CheckIP.dwFirstUserID = pCheckUserItem->GetUserID();
				CheckIP.dwNextUserID = pITableUserItem->GetUserID();
				CheckIP.cbISame = 1;
				vecCheckIP.push_back(CheckIP);
			}
		}
	}

	//��������
	for (WORD k =0; k<m_wChairCount; k++)
	{
		//��ȡ�û�
		CPlayer *pIServerUserItem = GetTableUserItem(k);
		if (pIServerUserItem==NULL) continue;

		//��������
		for (DWORD i = 0; i < vecCheckIP.size(); i++)
		{
			g_GameCtrl->SendData(pIServerUserItem, MDM_G_FRAME, CMD_GC_COM_CHECK_USER_IP, &vecCheckIP[i], sizeof(STR_CMD_CG_COM_CHECK_USER_IP));
		}	
	}

	return true;
}

//����Ч��
bool CTableFrame::EfficacyScoreRule(CPlayer * pIServerUserItem)
{
	//����Ա��������
	if(pIServerUserItem->GetMasterOrder()!=0) return true;

	//�����ж�
	//	if (CServerRule::IsForfendGameRule(m_pGameServiceOption->dwServerRule)==true) return true;

	//��������
	WORD wWinRate=pIServerUserItem->GetUserWinRate();
	WORD wFleeRate=pIServerUserItem->GetUserFleeRate();

	//���ַ�Χ
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//��ȡ�û�
		CPlayer * pITableUserItem=GetTableUserItem(i);

		//����Ч��
		if (pITableUserItem!=NULL)
		{
			/*
			//��ȡ����
			tagUserRule * pTableUserRule=pITableUserItem->GetUserRule();

			//����Ч��
			if ((pTableUserRule->bLimitFleeRate)&&(wFleeRate>pTableUserRule->wMaxFleeRate))
			{
			//������Ϣ
			TCHAR szDescribe[128]=TEXT("");
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("����������̫�ߣ��� %s ���õ����ò��������ܼ�����Ϸ��"),pITableUserItem->GetNickName());

			//������Ϣ
			SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);

			return false;
			}

			//ʤ��Ч��
			if ((pTableUserRule->bLimitWinRate)&&(wWinRate<pTableUserRule->wMinWinRate))
			{
			//������Ϣ
			TCHAR szDescribe[128]=TEXT("");
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("����ʤ��̫�ͣ��� %s ���õ����ò��������ܼ�����Ϸ��"),pITableUserItem->GetNickName());

			//������Ϣ
			SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);

			return false;
			}

			//����Ч��
			if (pTableUserRule->bLimitGameScore==true)
			{
			//��߻���
			if (pIServerUserItem->GetUserScore()>pTableUserRule->lMaxGameScore)
			{
			//������Ϣ
			TCHAR szDescribe[128]=TEXT("");
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("���Ļ���̫�ߣ��� %s ���õ����ò��������ܼ�����Ϸ��"),pITableUserItem->GetNickName());

			//������Ϣ
			SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);

			return false;
			}

			//��ͻ���
			if (pIServerUserItem->GetUserScore()<pTableUserRule->lMinGameScore)
			{
			//������Ϣ
			TCHAR szDescribe[128]=TEXT("");
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("���Ļ���̫�ͣ��� %s ���õ����ò��������ܼ�����Ϸ��"),pITableUserItem->GetNickName());

			//������Ϣ
			SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);

			return false;
			}

			}
			*/
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
//����ȦȺ����ɢ����
void CTableFrame::OnEventClubDismissRoom()
{
	m_pITableFrameSink->OnEventGameConclude(INVALID_CHAIR, NULL, GER_DISMISS);
}

/****************************************    ��ɢ����     ***************************************************/
//�����ɢ����
bool CTableFrame::OnEventApplyDismissRoom(WORD wChairID, bool bAgree)
{
	//�û�У��
	CPlayer *pApplyUserItem = GetTableUserItem(wChairID);
	if (NULL == pApplyUserItem)
		return false;

	//������ֵ
	m_bAgree[wChairID] = bAgree;
	//�����û��Ѿ���Ӧ
	m_bResponseDismiss[wChairID] = true;

	//���䴦�ڷǿ���״̬
	if (GAME_STATUS_FREE != GetGameStatus())
	{
		//�жϷ����Ƿ���Խ�ɢ
		tagTableRule* pCfg = (tagTableRule*)GetCustomRule();
		//���ֲ�ģʽ && �������ò��ɽ�ɢ ʱ�����Ч ��ҳ�Ҳ�����Խ�ɢ
		if (((0 != pCfg->byClubCreate) && (1 == pCfg->bDissolve)) || (pCfg->GameMode == 2))
		{
			STR_CMD_GR_FRMAE_ASK_DISMISS_RESULT cmdResult;
			ZeroMemory(&cmdResult, sizeof(cmdResult));
			cmdResult.lResultCode = -1;
			lstrcpyn(cmdResult.szDescribeString,
				TEXT("����ϵ�᳤��ɢ����!"),
				CountArray(cmdResult.szDescribeString));

			SendTableData(wChairID, CMD_GR_USER_ASK_DISMISS_RESULT, &cmdResult, sizeof(cmdResult), MDM_USER);
			return true;
		}

		//�����Զ���Ӧ��ʱ��
		SetVoteDismissRoom();

		//��ҵ����ɢ���䣬��������ҷ��͡�ͬ��/�ܾ���������Ϣ
		STR_CMD_GR_FRMAE_VOTE_DISMISS VoteRet;
		ZeroMemory(&VoteRet, sizeof(STR_CMD_GR_FRMAE_VOTE_DISMISS));

		//��ֵ
		VoteRet.cbApplyChair = wChairID;
		VoteRet.cbAgree = 1;
		VoteRet.cbChairID = wChairID;

		//�㲥ͶƱ�����Ϣ�ÿͻ��˵�����ɢ��
		SendTableData(INVALID_CHAIR, CMD_GR_USER_VOTE_DISMISS, &VoteRet, sizeof(STR_CMD_GR_FRMAE_VOTE_DISMISS), MDM_USER);

		//���÷��䴦�ڽ�ɢ״̬
		m_bUnderDissState = true;
		m_dissmisserChaiID = wChairID;

		return true;
	}
	else		//���䴦�ڿ���״̬
	{
		//�������
		ZeroMemory(&(m_bResponseDismiss), sizeof(m_bResponseDismiss));
		ZeroMemory(&(m_bAgree), sizeof(m_bAgree));

		//���ͽ�ɢ�ɹ���Ϣ
		STR_CMD_GR_FRAME_DISMISS_RESULT DismissResult;
		ZeroMemory(&DismissResult, sizeof(DismissResult));
		//��ֵ
		DismissResult.cbDismiss = 1;  //ֱ�ӽ�ɢ

		//�ж�����ǲ�������
		if (pApplyUserItem->GetUserID() == GetTableOwner())
		{
			SendTableData(INVALID_CHAIR, CMD_GR_USER_DISMISS_RESULT, &DismissResult, sizeof(STR_CMD_GR_FRAME_DISMISS_RESULT), MDM_USER);

			CLog::Log(log_debug, "��ɢ���� ");
			//������Ϸ
			HandleDJGameEnd(GAME_CONCLUDE_NORMAL);
			return true;
		}
		else		//���Ƿ�����ɢ�����ֱ���뿪
		{
			//������Ϣ,����վ�𣬿ͻ���ֱ��վ�𣬲����ǽ�ɢ
			SendTableData(wChairID, CMD_GR_USER_DISMISS_RESULT, &DismissResult, sizeof(STR_CMD_GR_FRAME_DISMISS_RESULT), MDM_USER);

			//�û��뿪			
			if (PlayerLeaveTable(pApplyUserItem))
			{
				return false;
			}
		}
	}

	return false;
}

//����Ƿ��ɢ����
bool CTableFrame::OnEventVoteDismissRoom(WORD wChairID, bool bAgree)
{
	//�û�У��
	CPlayer *pVoteUserItem = GetTableUserItem(wChairID);
	if (NULL == pVoteUserItem)			//TODO Ϊ�˽���û����߱���Ϸ��ɾ������ʱ�����޷���Ӧ��bug���������û�У��
		return false;

	//������ֵ
	m_bAgree[wChairID] = bAgree;
	//�����û��Ѿ���Ӧ
	m_bResponseDismiss[wChairID] = true;

	//��Client�������ͶƱ���
	STR_CMD_GR_FRMAE_VOTE_DISMISS VoteDismissRet;
	ZeroMemory(&VoteDismissRet, sizeof(STR_CMD_GR_FRMAE_VOTE_DISMISS));

	//��ֵ
	VoteDismissRet.cbAgree = (bAgree == true) ? 1 : 0;
	VoteDismissRet.cbChairID = wChairID;
	VoteDismissRet.cbApplyChair = m_dissmisserChaiID;

	//�㲥����
	SendTableData(INVALID_CHAIR, CMD_GR_USER_VOTE_DISMISS, &VoteDismissRet, sizeof(STR_CMD_GR_FRMAE_VOTE_DISMISS), MDM_USER);

	//���䴦�ڷǿ���״̬
	if (GAME_STATUS_FREE != GetGameStatus())
	{
		//��������
		int nAgree = 0;						//ͬ���ɢ�����
		int nResponse = 0;					//��Ӧ�����
		int nPlayer = GetSitUserCount();	//���������

		//���ͬ�����������Ӧ�����
		for (int i = 0; i < GetChairCount(); i++)
		{
			CPlayer *pTableUserItem = GetTableUserItem(i);
			if (pTableUserItem == NULL) continue;
			if (m_bAgree[i]) nAgree++;
			if (m_bResponseDismiss[i]) nResponse++;
		}

		//�����ɢ�������
		STR_CMD_GR_FRAME_DISMISS_RESULT DismissResult;
		ZeroMemory(&DismissResult, sizeof(STR_CMD_GR_FRAME_DISMISS_RESULT));

		//�������ͬ���ɢ	������ͬ�����ɢ���䣩
		if (static_cast<DOUBLE>(nAgree) > static_cast<DOUBLE>(nPlayer / 2))
		{
			//�������
			ZeroMemory(&(m_bResponseDismiss), sizeof(m_bResponseDismiss));
			ZeroMemory(&(m_bAgree), sizeof(m_bAgree));
			KillVoteDismissRoom();

			//��ֵ
			DismissResult.cbDismiss = 1;

			//�㲥�����ɢ�ɹ���Ϣ			
			SendTableData(INVALID_CHAIR, CMD_GR_USER_DISMISS_RESULT, &DismissResult, sizeof(STR_CMD_GR_FRAME_DISMISS_RESULT), MDM_USER);

			//���÷��䲻���ڽ�ɢ״̬
			m_bUnderDissState = false;
			m_dissmisserChaiID = 0xFF;

			//��Ϸ�У���������Ϸ����ֽ�����
			ITableFrameSink * m_pITableFrameSink = GetTableFrameSink();
			if (m_pITableFrameSink != NULL)
				m_pITableFrameSink->OnEventGameConclude(INVALID_CHAIR, NULL, GER_DISMISS);

			return true;
		}
		else if (nResponse == nPlayer)		//���������Ӧ���������δͬ��
		{
			//�������
			ZeroMemory(&(m_bResponseDismiss), sizeof(m_bResponseDismiss));
			ZeroMemory(&(m_bAgree), sizeof(m_bAgree));
			KillVoteDismissRoom();

			//��ֵ
			DismissResult.cbDismiss = 2;

			//�㲥�����ɢʧ����Ϣ			
			SendTableData(INVALID_CHAIR, CMD_GR_USER_DISMISS_RESULT, &DismissResult, sizeof(STR_CMD_GR_FRAME_DISMISS_RESULT), MDM_USER);

			//���÷��䲻���ڽ�ɢ״̬
			m_bUnderDissState = false;
			m_dissmisserChaiID = 0xFF;
		}
	}

	return false;
}


//�����Ƿ��ڽ�ɢ״̬
bool CTableFrame::GetDismissState()
{
	return m_bUnderDissState;
}

//���÷��䴦�ڽ�ɢ״̬
void CTableFrame::SetDismissState(bool bState)
{
	m_bUnderDissState = bState;
}
