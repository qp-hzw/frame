#include "Stdafx.h"
#include "TableFrame.h"
#include "DataBasePacket.h"
#include "GameCtrl.h"
#include <iostream>
#include "TableManager.h"
#include "RoomRuleManager.h"
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

//�����ɢ����
#define IDI_VOTE_DISMISS_ROOM		(TIME_TABLE_SINK_RANGE+6)			//�����ɢ����
#define TIME_VOTE_DISMISS_ROOM		300000L								//�¼� --5���� 

//��Ʊ�����Լ�����Ϸʱ��. ��ɢ����
#define IDI_CHECK_DISMISS_ROOM		(TIME_TABLE_SINK_RANGE+7)			//�����ɢ����
#define TIME_CHECK_DISMISS_ROOM		4*1000L								//�¼� --4���� 

//����Զ�׼��
#define IDI_PLAYER_AUTO_READY		(TIME_TABLE_SINK_RANGE+8)			//����Զ�׼��
#define TIME_PLAYER_AUTO_READY		3*1000L								//������

//��ʼ��һ�׶α���
#define IDI_MATCH_NEXT_STAGE_START	(TIME_TABLE_SINK_RANGE+9)			//��ʼ������һ�׶�
#define TIME_MATCH_NEXT_STAGE_START	10*1000L							//10S

/////////////////////////////////////////////////////////////////////////////////

enum PLAYER_OP_ERRORCODE
{
	PLAYER_NOT_EXISIT = 1, //��Ҳ�����
	TABLE_FULL = 2,        //��������
	TABLE_NOT_THIS = 3,    //����ͬһ��������

	CHAIR_USERD=4,         //�������Ѿ�����
	CHAIR_INVALID=5,       //���ӺŲ���ȷ
	STATUS_ERR = 6,			//û���¾�Ҫ׼��
	
	TICKET_NOT = 7,        //��Ʊ���ûͨ��
};

/////////////////////////////////////////////////////////////////////////////////
//���캯��
CTableFrame::CTableFrame()
{
	//����Ϸ
	m_pITableFrameSink= g_GameCtrl->GetITableFrameSink();
	/******************** ��̬���� **********************/
	ZeroMemory(&m_tagClubRoomRule, sizeof(m_tagClubRoomRule));
	m_wTableID=0;
	m_wChairCount=0;

	m_dwTableOwner=0L;

	/******************** ��̬���� **********************/
	m_cbGameStatus=GAME_STATUS_FREE;
	m_wCurGameRound=0;
	m_OnlyID = "";

	m_user_list.clear();
	m_player_list.clear();
	m_total_score.clear();

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
		g_GameCtrl->FreeITableFrameSink(m_pITableFrameSink);
		m_pITableFrameSink = NULL;
	}
	return;
}

//��������
CTableFrame::CTableFrame(const CTableFrame& table)
{
	m_pITableFrameSink = table.m_pITableFrameSink; //ǳ����
	m_wTableID = table.m_wTableID;
	m_wChairCount = table.m_wChairCount;
	memcpy(&m_tagTableRule, &table.m_tagTableRule, sizeof(tagTableRule));
	memcpy(&m_tagClubRoomRule, &table.m_tagClubRoomRule, sizeof(tagClubRoomRule));
	m_dwTableOwner = table.m_dwTableOwner;
	m_wCurGameRound = table.m_wCurGameRound;
	m_cbGameStatus = table.m_cbGameStatus;
	m_user_list = table.m_user_list;
	m_player_list = table.m_player_list;
	m_total_score = table.m_total_score;
	memcpy(m_bAgree, table.m_bAgree, sizeof(MAX_CHAIR));
	memcpy(m_bResponseDismiss, table.m_bResponseDismiss, sizeof(MAX_CHAIR));;
	m_bUnderDissState = table.m_bUnderDissState;
	m_dissmisserChaiID = table.m_dissmisserChaiID;
	memcpy(&m_GameScoreRecordActive, &table.m_GameScoreRecordActive,sizeof(CGameScoreRecordArray));
}

/*************************************** ���������������ȡ ***************************************************/
//���÷����Զ���ɢʱ��
void CTableFrame::SetTableAutoDismiss(DWORD dwMinutes) 
{ 
	SetGameTimer(IDI_ROOM_AUTO_DISMISS, dwMinutes*TIME_ROOM_AUTO_DISMISS, 1, NULL); 
}

//��ⷿ��
void CTableFrame::CheckRoomTruePlayer()
{
	bool flag = true;
	for (auto it = m_player_list.begin(); it != m_player_list.end(); it++)
	{
		if (((*it) != NULL) && (!(*it)->IsAndroidUser()))
			flag = false;
	}

	if (flag == true)
	{
		HandleDJGameEnd(GAME_CONCLUDE_NORMAL);
	}
}

//�Ƿ��ǻ�����
bool CTableFrame::IsRobot(WORD wChairID)
{
	if (wChairID > m_wChairCount)
		return false;

	CPlayer *player = m_player_list[wChairID];
	if (player == NULL)
		return false;

	return player->IsAndroidUser();
}
//�����Ƿ�����
bool CTableFrame::IsRoomFull()
{
	WORD wSitcount = 0;
	for (auto player : m_player_list)
	{
		if (NULL != player) 
			wSitcount++;
	}

	return m_wChairCount == wSitcount;
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

//��������Զ�׼����ʱ��
void CTableFrame::SetPlayerAutoReady()
{
	//������У��
	if (GetTableMode() == TABLE_MODE_MATCH)
		SetGameTimer(IDI_PLAYER_AUTO_READY, TIME_PLAYER_AUTO_READY, 1, NULL);
}
//���ÿ�ʼ��һ�׶ζ�ʱ��
void CTableFrame::SetStageTimer()
{
	SetGameTimer(IDI_MATCH_NEXT_STAGE_START, TIME_MATCH_NEXT_STAGE_START, 1, NULL);
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
	for(size_t i=0; i< m_player_list.size(); i++)
	{
		if(!m_player_list.at(i)) continue;
		m_player_list.at(i)->SetUserStatus(US_PLAYING,m_wTableID,i);
	}

	//¼���ʼ��
	m_Record.Init();

	//¼�������Ϣ
	m_Record.AddPlayer(m_player_list);

	/* ֪ͨ���ݿ�, ���ӿ�ʼ */      
	tagTableRule *pTableCfg = (tagTableRule*)GetCustomRule();
	BYTE byClubOrHalGold = 0; //1��ʾ���ֲ�����(�������߷������ģʽ);  2��ʾ�����Ľ��ģʽ;  �����ֶ����봦��
	BYTE GameMode = pTableCfg->GameMode;

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
bool CTableFrame::HandleXJGameEnd(BYTE byRound, WORD *wIdentity, SCORE *lUserTreasure)
{
	m_wCurGameRound = byRound;
	DWORD dwDataSize = 0;
	VOID *pData = m_Record.GetData(m_tagTableRule.PlayerCount, dwDataSize);
	if (m_OnlyID.empty())
	{
		m_OnlyID = std::to_string(time(0)) + std::to_string(m_wTableID);
	}

#pragma region �û���Ϣ
	//�۳��û���Ʊ
	if(byRound == 1)
	{
		XJTickets();
	}

	//���״̬
	for (int i = 0; i < m_player_list.size(); i++)
	{
		if (m_player_list[i] == NULL)	continue;
		m_player_list[i]->SetUserStatus(US_SIT, GetTableID(), i);
	}

	//��¼���� --����
	for (int i = 0; i < m_total_score.size(); i++)
	{
		m_total_score[i] += lUserTreasure[i];
	}

	//�����û��Ƹ�  -- �û��Ƹ������¼��
	XJModifyUserTreasure(lUserTreasure);

	//���������Ӯ����� && ����ֵ
	int i =0;
	for(auto player : m_player_list)
	{
		player ->ModifyPlayerScore(byRound, wIdentity[i], lUserTreasure[i], m_OnlyID);
		player->ModifyPlayerExp();
		i++;
	}

	//�����û�����״̬
	//XJUpdateGameTaskStatus(byTableMode, byRound); //TODONOW ��ʱֻΪ������
#pragma endregion

#pragma region ������Ϣ
	//��������״̬
	SetGameStatus(GAME_STATUS_FREE);

	//��������ս��
	XJUpdateTableRecord(byRound, m_OnlyID);

	//��������¼��
	XJUpdateTableVideo(byRound, m_OnlyID, pData, dwDataSize);
#pragma endregion
	return true;
}

//��ֽ���������
bool CTableFrame::HandleDJGameEnd(BYTE cbGameStatus)
{
	//�ر����ж�ʱ��
	KillGameTimer(IDI_ROOM_AUTO_DISMISS);

	if (!m_OnlyID.empty())
	{
		//���������Ӯ�����
		int i =0;
		for(auto player : m_player_list)
		{
			if (player)
			{
				player ->ModifyPlayerScore(0, 0, m_total_score[i], m_OnlyID);
				i++;
			}
		}

		//��������ս��
		XJUpdateTableRecord(0, m_OnlyID);
	}

	//�����ȴ����ѣ������Ҫ��׼��״̬�˳�Ϊ����
	if (cbGameStatus == GAME_CONCLUDE_CONTINUE)
	{
		m_cbGameStatus=GAME_STATUS_FREE;

		ZeroMemory(m_bAgree,sizeof(m_bAgree));
		//���÷��䲻���ڽ�ɢ״̬
		m_bUnderDissState = false;
		m_dissmisserChaiID = 0xFF;

		//״̬�л�Ϊ���£����������뿪
		for(auto player : m_player_list)
		{
			player->SetUserStatus(US_SIT,m_wTableID,player->GetChairID());
		}

		//�������Ѷ�ʱ��
		SetGameTimer(IDI_WAIT_CONTINUE,TIME_WAIT_CONTINUE,1,NULL);
		return true;
	}

	//ֱ�ӽ�ɢ���ȫ��վ��
	if(cbGameStatus == GAME_CONCLUDE_NORMAL)
	{
		for(auto player : m_player_list)
		{
			PlayerLeaveTable(player);
		}

		CTableManager::DeleteTable(this);
		return true;
	}

	return true;
}


/*************************************   ��Ϸ���̸�������    *************************************************/
//ÿ����Ϸ�����󣬼�Ⲣ�۳��û���Ʊ  
bool CTableFrame::XJTickets()
{
	switch(m_tagTableRule.GameMode)
	{
	case TABLE_MODE_FK:
		{
			if(m_tagTableRule.cbPayType == 0)//����֧��
			{
				int CountType = RoomRuleManager::GetCountType(m_tagTableRule.GameCount);
				SCORE cost = CountType * m_tagTableRule.PlayerCount;
				CPlayerManager::FindPlayerByID(m_dwTableOwner)->ModifyUserTreasure(TREASURE_FK, -cost, "����������");
			}
			else if(m_tagTableRule.cbPayType == 1) //AA֧��
			{
				int CountType = RoomRuleManager::GetCountType(m_tagTableRule.GameCount);
				for(auto player : m_player_list)
				{
					player->ModifyUserTreasure(TREASURE_FK, -CountType, "����������");
				}
			}

			break;
		}
	case TABLE_MODE_MATCH:
		{
			break;
		}
	case TABLE_MODE_GOLD:
		{
			break;
		}
	case TABLE_MODE_FK_GOLD:
		{
			break;
		}
	case TABLE_MODE_CLUB:
		{
			break;
		}
	}

	return true;
}
//ÿ����Ϸ�����󣬸����û��Ƹ���Ϣ
bool CTableFrame::XJModifyUserTreasure(SCORE *lGameScore)
{
	BYTE byTreasureType = 0;  //1����, 2���, 3��ʯ, 4����
	switch(m_tagTableRule.GameMode)
	{
	case TABLE_MODE_FK:
		{
			byTreasureType = TREASURE_JF;
			break;
		}
	case TABLE_MODE_MATCH:
		{
			break;
		}
	case TABLE_MODE_GOLD:
		{
			byTreasureType = TREASURE_GOLD;
			break;
		}
	case TABLE_MODE_FK_GOLD:
		{
			byTreasureType = TREASURE_GOLD;
			break;
		}
	case TABLE_MODE_CLUB:
		{
			byTreasureType = TREASURE_JF;
			break;
		}
	}

	int i =0;
	for(auto player : m_player_list)
	{
		player->ModifyUserTreasure(byTreasureType, lGameScore[i], "С�ֽ���");
		i++;
	}
	return true;
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

//ÿ����Ϸ������, ��������ս����
bool CTableFrame::XJUpdateTableRecord(BYTE round, string OnlyID)
{
	//�������
	g_TreasureDB->ResetParameter();
	g_TreasureDB->AddParameter(TEXT("@TableID"), m_wTableID);
	g_TreasureDB->AddParameter(TEXT("@GameMode"),m_tagTableRule.GameMode);
	g_TreasureDB->AddParameter(TEXT("@AllCount"),m_tagTableRule.GameCount);
	g_TreasureDB->AddParameter(TEXT("@CurCount"),round);
	g_TreasureDB->AddParameter(TEXT("@PlayerCount"),m_tagTableRule.PlayerCount);
	g_TreasureDB->AddParameter(TEXT("@KindID"),g_GameCtrl->GetKindID());
	g_TreasureDB->AddParameter(TEXT("@OnlyID"), OnlyID.c_str());

	//ִ�в�ѯ
	LONG lResultCode = g_TreasureDB->ExecuteProcess(TEXT("GSP_TableRecord_Insert"), true);

	return true;
}
//ÿ����Ϸ������, ��������¼���
bool CTableFrame::XJUpdateTableVideo(BYTE round, string OnlyID, VOID* pData, DWORD dwDataSize)
{
	BYTE Data[LEN_MAX_RECORD_SIZE];	//¼������
	memcpy_s(Data, dwDataSize*sizeof(BYTE), pData, dwDataSize*sizeof(BYTE));

	if(dwDataSize>LEN_MAX_RECORD_SIZE)
	{
		dwDataSize = LEN_MAX_RECORD_SIZE;
	}
	
	//�������
	g_TreasureDB->ResetParameter();
	g_TreasureDB->AddParameter(TEXT("@TableID"), m_wTableID);
	g_TreasureDB->AddParameter(TEXT("@CurCount"),round);
	
	std::string szData = CWConvert::toHexString3(Data, LEN_MAX_RECORD_SIZE);
	g_TreasureDB->AddParameter(TEXT("@VideoData"),szData.c_str());
	g_TreasureDB->AddParameter(TEXT("@VideoSize"),dwDataSize);
	g_TreasureDB->AddParameter(TEXT("@OnlyID"), OnlyID.c_str());

	//ִ�в�ѯ
	LONG lResultCode = g_TreasureDB->ExecuteProcess(TEXT("GSP_TableVideo_Insert"), true);

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
	SendTableData(INVALID_CHAIR, CMD_GR_USER_STATUS, &GameStatus, sizeof(GameStatus), MDM_G_FRAME);

	//4. ����
	m_user_list.push_back(pPlayer);

	return 0;
}
//�������
int CTableFrame::PlayerSitTable(CPlayer * pPlayer, WORD wChairID, bool bCheckUserGPS)
{
	//1. У��
	int ret = CanPlayerSitTable(pPlayer, wChairID);
	if (ret !=0) return ret;

	//2. Table
	m_player_list[wChairID] = pPlayer;

	bool bflag = false;
	for (auto user : m_user_list)
	{
		if (user && user == pPlayer)
		{
			bflag = true;
			break;
		}
	}
	if (bflag == false)
		m_user_list.push_back(pPlayer);

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
	SendTableData(INVALID_CHAIR, CMD_GR_USER_STATUS, &GameStatus, sizeof(GameStatus), MDM_G_FRAME);

	return 0;
}
//���վ��
bool CTableFrame::PlayerUpTable(CPlayer *pPlayer)
{
	//1, У��
	int ret = CanPlayerUpTable(pPlayer);
	if(ret !=0 ) return false;

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
	SendTableData(INVALID_CHAIR, CMD_GR_USER_STATUS, &GameStatus, sizeof(GameStatus), MDM_G_FRAME);

	return true;
}
//����뿪
int CTableFrame::PlayerLeaveTable(CPlayer* pPlayer)
{
	//1. У��
	int ret = CanPlayerLeaveTable(pPlayer);
	if(ret!=0) return ret;

	//���͸��ͻ���
	CMD_GF_GameStatus GameStatus;
	ZeroMemory(&GameStatus, sizeof(GameStatus));

	//��ֵ
	GameStatus.cbUserAction = US_LEAVE;
	tagUserInfo *pUserInfo = pPlayer->GetUserInfo();
	CopyMemory(&GameStatus.UserInfo, pUserInfo, sizeof(tagUserInfo));

	//�㲥����
	SendTableData(INVALID_CHAIR, CMD_GR_USER_STATUS, &GameStatus, sizeof(GameStatus), MDM_G_FRAME);

	//3. Player
	pPlayer->SetUserStatus(US_FREE, INVALID_TABLE, INVALID_CHAIR);

	//�Ͽ��û� socket (��ҳ���������)
	if ( m_tagTableRule.GameMode != TABLE_MODE_GOLD && m_tagTableRule.GameMode != TABLE_MODE_MATCH) 
		CPlayerManager::CloseSocket(pPlayer);

	//2. Table
	auto ite1 = find(m_user_list.begin(), m_user_list.end(), pPlayer);
	if (ite1 != m_user_list.end())
	{
		m_user_list.erase(ite1);
	}

	auto ite2 = find(m_player_list.begin(), m_player_list.end(), pPlayer);
	if (ite2 != m_player_list.end())
	{
		*ite2 = NULL;
	}

	return 0;
}
//���׼��
int CTableFrame::PlayerReady(CPlayer* pPlayer) 
{
	//׼��У��
	int ret = CanPlayerReady(pPlayer);
	if (ret != 0)	return ret;

	//��ȡ����
	DWORD	dwTableID = pPlayer->GetTableID();

	//�����û�״̬Ϊ׼��
	pPlayer->SetUserStatus(US_READY, dwTableID, pPlayer->GetChairID());

	//���ͻ��˷���׼����Ϊ
	CMD_GF_GameStatus GameStatus;
	ZeroMemory(&GameStatus, sizeof(GameStatus));

	//��ֵ
	GameStatus.cbUserAction = pPlayer->GetUserStatus();
	tagUserInfo *pUserInfo = pPlayer->GetUserInfo();
	CopyMemory(&GameStatus.UserInfo, pUserInfo, sizeof(tagUserInfo));

	//�㲥����
	SendTableData(INVALID_CHAIR, CMD_GR_USER_STATUS, &GameStatus, sizeof(GameStatus), MDM_G_FRAME);

	//�ж���������Ƿ�׼��
	WORD ReadyNum = 0;
	for (auto it = m_player_list.begin(); it != m_player_list.end(); it++)
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
	SendTableData(INVALID_CHAIR, CMD_GR_USER_STATUS, &GameStatus, sizeof(GameStatus), MDM_G_FRAME);

	return 0;
}

//����ܷ����
int CTableFrame::CanPlayerEnterTable(CPlayer* pPlayer)
{
	if (US_FREE != pPlayer->GetUserStatus())
		return -1;

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

	//��Ҳ��ٷ�����  �����뿪
	if (m_wTableID != pPlayer->GetTableID())
		return 3;

	//����������Ϸ�� �����뿪
	if (US_PLAYING == pPlayer->GetUserStatus())
		return 1;

	return 0;
}
//����Ƿ���׼��
int CTableFrame::CanPlayerReady(CPlayer* pPlayer) 
{
	//У��
	if (pPlayer == NULL)  return PLAYER_NOT_EXISIT;

	//ֻ�����²���׼��
	if (pPlayer->GetUserStatus() != US_SIT)	return STATUS_ERR;

	return 0;
}

/*************************************    ������    *************************************************/
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
	return NULL;
	//return m_user_list[wEnumIndex];
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
	//����¼��
	if (wMainCmdID == MDM_GF_GAME)
	{
		m_Record.AddRecord(wChairID, wSubCmdID, pData, wDataSize);
	}

	//�û�Ⱥ��
	if (wChairID == INVALID_CHAIR)
	{
		for (auto it = m_user_list.begin(); it != m_user_list.end(); it++)   //�����Թ��û�Ҳ�ᷢ��
		{
			if (((*it) != NULL) && (!(*it)->IsAndroidUser()))
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

		if (pIServerUserItem == NULL) return false;
		if (pIServerUserItem->IsAndroidUser()) return false;
		//��������
		g_GameCtrl->SendData(pIServerUserItem, wMainCmdID, wSubCmdID, pData, wDataSize);

		return true;
	}

	return false;
}

//���ͳ��� TODONOW ׼��ɾ��
bool CTableFrame::SendGameScene(WORD wChairID, VOID * pData, WORD wDataSize)
{
	//�û�Ч��
	if(wChairID > m_player_list.size()) return false;

	g_GameCtrl->SendData(m_player_list.at(wChairID), MDM_G_FRAME,CMD_GR_FRAME_GAME_OPTION,pData,wDataSize);

	return true;
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

//ʱ���¼�
bool CTableFrame::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//�ص��¼�
	if ((dwTimerID>=0)&&(dwTimerID<TIME_TABLE_SINK_RANGE))
	{
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
	case IDI_PLAYER_AUTO_READY:		//����Զ�׼��
		{
			KillGameTimer(IDI_PLAYER_AUTO_READY);

			for (auto player : m_player_list)
			{
				if (!player) continue;

				PlayerReady(player);
			}

			return true;
		}
	case IDI_MATCH_NEXT_STAGE_START:	//��ʼ������һ�׶�
		{
			KillGameTimer(IDI_MATCH_NEXT_STAGE_START);

			StartNextStage();

			return true;
		}
	case IDI_WAIT_CONTINUE:
		{
			KillGameTimer(IDI_WAIT_CONTINUE);

			HandleDJGameEnd(0xFE);

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
	case SUB_RG_ROOM_RULE:		//��Ϸ����
		{
			//���ͷ������
			STR_CMD_ROOM_RULE room_rule;
			memcpy(&room_rule.common, &m_tagTableRule, sizeof(room_rule.common));
			room_rule.TableID = m_wTableID;
			g_GameCtrl->SendData(pIServerUserItem, MDM_G_FRAME, CMD_GR_ROOM_RULE, &room_rule, sizeof(room_rule));

			return true;
		}
	case SUB_RG_PLAYER_INFO:		//�����û���Ϣ
		{
			// 1����������ҷ����Լ�������
			WORD wChairID = pIServerUserItem->GetChairID();
			BYTE cbUserStatus = pIServerUserItem->GetUserStatus();

			//����״̬
			CMD_GF_GameStatus GameStatus1;
			GameStatus1.cbUserAction = cbUserStatus;

			tagUserInfo *pUserInfo = pIServerUserItem->GetUserInfo();
			memset(&GameStatus1.UserInfo, 0, sizeof(tagUserInfo));
			memcpy(&GameStatus1.UserInfo, pUserInfo, sizeof(tagUserInfo));
			CLog::Log(log_debug, "UserID: %d, wChairID: %d", GameStatus1.UserInfo.dwUserID, GameStatus1.UserInfo.wChairID);

			g_GameCtrl->SendData(pIServerUserItem, MDM_G_FRAME, CMD_GR_USER_STATUS, &GameStatus1, sizeof(GameStatus1));

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

					CLog::Log(log_debug, "UserID: %d, wChairID: %d", GameStatus.UserInfo.dwUserID, GameStatus.UserInfo.wChairID);

					g_GameCtrl->SendData(pIServerUserItem, MDM_G_FRAME, CMD_GR_USER_STATUS, &GameStatus, sizeof(GameStatus));
				}
			}

			// 3����Client���� �û���Ϣ������ɵ���Ϣ
			g_GameCtrl->SendData(pIServerUserItem, MDM_G_FRAME, CMD_GR_USER_STATUS_FINISH, NULL, 0);

			return true;
		}
	case SUB_RG_FRAME_GAME_OPTION:		//������Ϸ����
		{
			//��������
			WORD wChairID = pIServerUserItem->GetChairID();

			//���ͳ���
			m_pITableFrameSink->OnEventSendGameScene(wChairID);

			
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
	case SUB_CG_EFFECT: //��������
		{
			//Ч�����
			if (wDataSize != sizeof(STR_SUB_CG_EFFECT)) 
				return false;

			//��������
			STR_SUB_CG_EFFECT *pSub = (STR_SUB_CG_EFFECT *)pData;

			//���߼���û� -- ʹ����Ʒʧ��
			if( !(pIServerUserItem-> UseProp(pSub->dwGoodsID)) )
			{
				g_GameCtrl->SendData(pIServerUserItem, MDM_G_FRAME, CMD_GC_EFFECT_RESULT, NULL, 0);
				return true;
			}

			//ʹ�óɹ�, �㲥��������
			STR_CMD_GC_EFFECT_BRODCAST cmd;
			ZeroMemory(&cmd, sizeof(cmd));

			//��ֵ
			cmd.from_chair =  pIServerUserItem->GetChairID();
			cmd.target_chair = pSub->target_chair;
			cmd.dwGoodsID = pSub->dwGoodsID;
			SendTableData(INVALID_CHAIR, CMD_GC_EFFECT_BRODCAST, &cmd, sizeof(cmd), MDM_G_FRAME);
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
				for (size_t i=0;i<m_user_list.size();i++)
				{
					//��ȡ�û�
					//CPlayer * pILookonUserItem=m_user_list[i];
					//if (pILookonUserItem->GetUserID()!=pLookonConfig->dwUserID) continue;
					//if (pILookonUserItem->GetChairID()!=pIServerUserItem->GetChairID()) continue;

					////������Ϣ
					//CMD_GF_LookonStatus LookonStatus;
					//LookonStatus.cbAllowLookon=pLookonConfig->cbAllowLookon;

					////������Ϣ
					//g_GameCtrl->SendData(pILookonUserItem,MDM_G_FRAME,CMD_GF_LOOKON_STATUS,&LookonStatus,sizeof(LookonStatus));

					break;
				}
			}
			else
			{
				//������Ϣ
				CMD_GF_LookonStatus LookonStatus;
				LookonStatus.cbAllowLookon=pLookonConfig->cbAllowLookon;

				//������Ϣ
				for (size_t i=0;i<m_user_list.size();i++)
				{
					////��ȡ�û�
					//CPlayer * pILookonUserItem=m_user_list[i];
					//if (pILookonUserItem->GetChairID()!=pIServerUserItem->GetChairID()) continue;

					////������Ϣ
					//g_GameCtrl->SendData(pILookonUserItem,MDM_G_FRAME,CMD_GF_LOOKON_STATUS,&LookonStatus,sizeof(LookonStatus));
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
	for(size_t i =0; i<m_player_list.size(); i++)
	{
		if(m_player_list.at(i) == pPlayer)
		{
			return i;
		}
	}

	return INVALID_CHAIR;
}

//�������Ƿ�����ʵ���
bool CTableFrame::IsTruePlayer()
{
	for (auto it = m_player_list.begin(); it != m_player_list.end(); it++)
	{
		if ((*it != NULL) && (!(*it)->IsAndroidUser()))
			return true;
	}

	return false;
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
		double dCheckUserLong = 0.1;//pCheckUserItem->GetUserInfo()->dLongitude;
		double dCheckUserLat  = 0.1;//pCheckUserItem->GetUserInfo()->dLatitude;

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
			double dTableUserLong = 0.1;// pTableUserItem->GetUserInfo()->dLongitude;
			double dTableUserLat = 0.1;//pTableUserItem->GetUserInfo()->dLatitude;

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
	m_pITableFrameSink->OnEventGameConclude(GER_DISMISS);
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
		//���ֲ�ģʽ && �������ò��ɽ�ɢ ʱ�����Ч ��ҳ�Ҳ�����Խ�ɢ  �������ɽ�ɢ
		if ((1 == GetClubRoomRule()->bDissolve) || (pCfg->GameMode == 2) || (pCfg->GameMode == 1))
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
		VoteRet.cbApplyChair = static_cast<BYTE>(wChairID);
		VoteRet.cbAgree = 1;
		VoteRet.cbChairID = static_cast<BYTE>(wChairID);

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

		//����������״̬Ҳ���ɽ�ɢ
		tagTableRule* pCfg = (tagTableRule*)GetCustomRule();
		if (pCfg->GameMode == 2)
			return false;

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

			//��ⷿ�����Ƿ������� û�����˴�ֽ���
			CheckRoomTruePlayer();
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
	VoteDismissRet.cbChairID = static_cast<BYTE>(wChairID);
	VoteDismissRet.cbApplyChair = static_cast<BYTE>(m_dissmisserChaiID);

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
			if (m_pITableFrameSink != NULL)
				m_pITableFrameSink->OnEventGameConclude(GER_DISMISS);

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
