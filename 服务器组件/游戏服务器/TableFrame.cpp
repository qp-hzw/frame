#include "Stdafx.h"
#include "TableFrame.h"
#include "DataBasePacket.h"
#include "AttemperEngineSink.h"
#include <iostream>
#include <vector>
#include "GameCtrl.h"

//////////////////////////////////////////////////////////////////////////////////
//У��GPS����
#define CHECK_USER_GPS_DISTANCE		200									//���GPS�ڶ��پ�������ʾ���

#define IDI_GAME_CHECK				(TIME_TABLE_SINK_RANGE+2)			//��ʱ��ʶ
#define TIME_GAME_CHECK				3600000								//��ʱʱ��

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


//��Ϸ��¼
CGameScoreRecordArray				CTableFrame::m_GameScoreRecordBuffer;
//////////////////////////////////////////////////////////////////////////////////

//���캯��
CTableFrame::CTableFrame()
{

	m_pITableFrameSink= g_GameCtrl->GetITableFrameSink();

	//��������
	m_wTableID=0;
	m_wChairCount=0;
	m_cbStartMode=START_MODE_ALL_READY;
	m_wUserCount=0;
	m_cbTableMode = 0;

	//��־����
	m_bGameStarted=false;
	m_bDrawStarted=false;
	m_bTableStarted=false;
	ZeroMemory(m_bAllowLookon,sizeof(m_bAllowLookon));
	ZeroMemory(m_lFrozenedScore,sizeof(m_lFrozenedScore));

	//��Ϸ����
	m_cbGameStatus=GAME_STATUS_FREE;
	m_wDrawCount=0;

	m_dwGroupID = 0;
	m_dwCreateTableUser = 0;

	//ʱ�����
	m_dwDrawStartTime=0L;
	ZeroMemory(&m_SystemTimeStart,sizeof(m_SystemTimeStart));

	//��̬����
	m_dwTableOwner=0L;
	ZeroMemory(m_szEnterPassword,sizeof(m_szEnterPassword));

	//������Ϣ
	m_pGameParameter=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//�û�����
	ZeroMemory(m_TableUserItemArray,sizeof(m_TableUserItemArray));

	//��ɢ��Ϣ
	m_bUnderDissState = false;
	m_dissmisserChaiID = 0xFF;

	//������Ϣ
	m_dwPassword = 0;
	return;
}

//��������
CTableFrame::~CTableFrame()
{
	//�ͷŶ���
	if ( NULL != m_pITableFrameSink )
	{
		SafeRelease(m_pITableFrameSink);
	}

	if (m_pIGameMatchSink!=NULL)
	{
		SafeDelete(m_pIGameMatchSink);
	}

	return;
}

//������������һ�����µ����
void CTableFrame::SetTableOwner(DWORD dwUserID)
{
	m_dwTableOwner = dwUserID;
}

//�����������һ�����µ���
DWORD CTableFrame::GetTableOwner()
{
	return m_dwTableOwner;
}

//���÷����Զ���ɢʱ��
void CTableFrame::SetTableAutoDismiss(DWORD dwMinutes) 
{ 
	SetGameTimer(IDI_ROOM_AUTO_DISMISS, dwMinutes*TIME_ROOM_AUTO_DISMISS, 1, NULL); 
}

//��ʼ��Ϸ
bool CTableFrame::StartGame()
{
	//��Ϸ״̬
	if (m_bDrawStarted==true) return false;

	//ɾ���Զ���ɢ��ʱ��
	KillGameTimer(IDI_ROOM_AUTO_DISMISS);

	//�������
	bool bGameStarted=m_bGameStarted;
	bool bTableStarted=m_bTableStarted;

	//����״̬
	m_bGameStarted=true;
	m_bDrawStarted=true;
	m_bTableStarted=true;

	//��ɢ��Ϣ
	ZeroMemory(m_bResponseDismiss,sizeof(m_bResponseDismiss));
	ZeroMemory(m_bAgree,sizeof(m_bAgree));
	m_bUnderDissState = false;
	m_dissmisserChaiID = 0xFF;

	//��ʼʱ��
	GetLocalTime(&m_SystemTimeStart);
	m_dwDrawStartTime=(DWORD)time(NULL);

	//��ʼ����
	if (bGameStarted==false)
	{
		//�����û�
		for (WORD i=0;i<m_wChairCount && i<MAX_CHAIR;i++)
		{
			//��ȡ�û�
			CPlayer * pIServerUserItem=GetTableUserItem(i);

			//�����û�
			if (pIServerUserItem!=NULL)
			{
				//����״̬
				BYTE cbUserStatus=pIServerUserItem->GetUserStatus();
				if ((cbUserStatus!=US_OFFLINE)&&(cbUserStatus!=US_PLAYING)) pIServerUserItem->SetUserStatus(US_PLAYING,m_wTableID,i);
			}
		}

		//����״̬
		if (bTableStarted!=m_bTableStarted) SendTableStatus();
	}

	//������Ϸ״̬Ϊ��ʼ
	SetGameStatus(GAME_STATUS_PLAY);

	/* ֪ͨ���ݿ�, ���ӿ�ʼ */
	//TODONOW added by WangChengQing �˴�Ӧ������У��
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
	g_AttemperEngineSink->ClubTableStart(GetPassword(), byClubOrHalGold);

	//֪ͨ�¼�
	if (m_pITableFrameSink!=NULL) 
	{
		m_pITableFrameSink->OnEventGameStart();
	}

	return true;
}

//��ɢ��Ϸ
bool CTableFrame::DismissGame()
{
	//״̬�ж�
	ASSERT(m_bTableStarted==true);
	if (m_bTableStarted==false) return false;

	KillGameTimer(IDI_GAME_CHECK);

	//g_AttemperEngineSink->ConcludeTable(m_wTableID);

	//������Ϸ
	if ((m_bGameStarted==true)&&(m_pITableFrameSink->OnEventGameConclude(INVALID_CHAIR,NULL,GER_DISMISS)==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//����״̬
	if ((m_bGameStarted==false)&&(m_bTableStarted==true))
	{
		//���ñ���
		m_bTableStarted=false;

		//����״̬
		SendTableStatus();
	}

	return false;
}

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
	if(GAME_STATUS_FREE != m_cbGameStatus)
	{
		//�жϷ����Ƿ���Խ�ɢ
		tagTableRule* pCfg = (tagTableRule*)GetCustomRule();
		//���ֲ�ģʽ && �������ò��ɽ�ɢ ʱ�����Ч 
		if((0 != pCfg->byClubCreate) && (1 == pCfg -> bDissolve)) 
 		{
			STR_CMD_GR_FRMAE_ASK_DISMISS_RESULT cmdResult;
			ZeroMemory(&cmdResult, sizeof(cmdResult));
			cmdResult.lResultCode = 1;
			lstrcpyn(cmdResult.szDescribeString,
				TEXT("����ϵ�᳤��ɢ����!"),
				CountArray(cmdResult.szDescribeString));

			SendTableData(wChairID,CMD_GR_FRMAE_ASK_DISMISS_RESULT,&cmdResult, sizeof(cmdResult), MDM_G_FRAME);
			return true;
		}


		//�����Զ���Ӧ��ʱ��
		SetGameTimer(IDI_VOTE_DISMISS_ROOM, TIME_VOTE_DISMISS_ROOM, 1, NULL);

		//��ҵ����ɢ���䣬��������ҷ��͡�ͬ��/�ܾ���������Ϣ
		STR_CMD_GR_FRMAE_VOTE_DISMISS VoteDismiss;
		ZeroMemory(&VoteDismiss, sizeof(STR_CMD_GR_FRMAE_VOTE_DISMISS));

		//��ֵ
		VoteDismiss.dwVoteUser[0] = pApplyUserItem->GetUserID();
		int nVoteNum = 1;
		for(int i=0; i<MAX_CHAIR && nVoteNum < 5; i++)
		{
			CPlayer *pTableUserItem = GetTableUserItem(i);
			if(pTableUserItem == NULL || pTableUserItem == pApplyUserItem) continue;
			VoteDismiss.dwVoteUser[nVoteNum] = pTableUserItem->GetUserID();
			nVoteNum++;
		}

		lstrcpyn(VoteDismiss.szApplyUserNick, pApplyUserItem->GetUserInfo()->szNickName, CountArray(VoteDismiss.szApplyUserNick));

		//�������˷��� ���������ɢ��Ϣ
		SendTableData(INVALID_CHAIR, CMD_GR_FRMAE_VOTE_DISMISS, &VoteDismiss, sizeof(STR_CMD_GR_FRMAE_VOTE_DISMISS), MDM_G_FRAME);

		//���÷��䴦�ڽ�ɢ״̬
		m_bUnderDissState = true;
		m_dissmisserChaiID = wChairID;

		return true;
	}
	else		//���䴦�ڿ���״̬
	{
		//�������
		ZeroMemory(m_bResponseDismiss, sizeof(m_bResponseDismiss));
		ZeroMemory(m_bAgree, sizeof(m_bAgree));

		//���ͽ�ɢ�ɹ���Ϣ
		STR_CMD_GR_FRAME_DISMISS_RESULT DismissResult;
		ZeroMemory(&DismissResult, sizeof(DismissResult));
		//��ֵ
		DismissResult.cbDismiss = 1;
		DismissResult.cbAgree = 1;
		DismissResult.dwVoteUserID = pApplyUserItem->GetUserID();
		lstrcpyn(DismissResult.szVoteUserNick, pApplyUserItem->GetUserInfo()->szNickName, CountArray(DismissResult.szVoteUserNick));

		//�ж�����ǲ�������
		if(pApplyUserItem->GetUserID() == m_dwTableOwner)
		{
			//������Ϣ
			SendTableData(INVALID_CHAIR, CMD_GR_FRAME_DISMISS_RESULT, &DismissResult, sizeof(STR_CMD_GR_FRAME_DISMISS_RESULT), MDM_G_FRAME);

			//������Ϸ
			ConcludeGame(GAME_CONCLUDE_NORMAL);
			return true;
		}
		else		//���Ƿ�����ɢ�����ֱ������
		{
			//������Ϣ,����վ�𣬿ͻ���ֱ��վ�𣬲����ǽ�ɢ
			SendTableData(wChairID, CMD_GR_FRAME_DISMISS_RESULT, &DismissResult, sizeof(STR_CMD_GR_FRAME_DISMISS_RESULT), MDM_G_FRAME);

			//�û�վ��			
			if ( PerformStandUpAction(pApplyUserItem) ) 
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

	//���䴦�ڷǿ���״̬
	if(GAME_STATUS_FREE != m_cbGameStatus)
	{
		//��������
		int nAgree = 0;						//ͬ���ɢ�����
		int nResponse = 0;					//��Ӧ�����
		int nPlayer = GetSitUserCount();	//���������

		//���ͬ�����������Ӧ�����
		for (int i=0; i<m_wChairCount && i<MAX_CHAIR; i++)
		{
			CPlayer *pTableUserItem = GetTableUserItem(i);
			if(pTableUserItem == NULL) continue;
			if(m_bAgree[i]) nAgree++;
			if(m_bResponseDismiss[i]) nResponse++;
		}

		//�����ɢ�������
		STR_CMD_GR_FRAME_DISMISS_RESULT DismissResult;
		ZeroMemory(&DismissResult, sizeof(STR_CMD_GR_FRAME_DISMISS_RESULT));

		//��ֵ
		DismissResult.cbAgree = bAgree;
		if (NULL != pVoteUserItem)
		{
			DismissResult.dwVoteUserID = pVoteUserItem->GetUserID();
			lstrcpyn(DismissResult.szVoteUserNick, pVoteUserItem->GetUserInfo()->szNickName, CountArray(DismissResult.szVoteUserNick));
		}

		//�������ͬ���ɢ	������ͬ�����ɢ���䣩
		if(static_cast<DOUBLE>(nAgree) > static_cast<DOUBLE>(nPlayer/2))
		{
			//�������
			ZeroMemory(m_bResponseDismiss, sizeof(m_bResponseDismiss));
			ZeroMemory(m_bAgree, sizeof(m_bAgree));
			KillGameTimer(IDI_VOTE_DISMISS_ROOM);

			//��ֵ
			DismissResult.cbDismiss = 1;

			//�㲥�����ɢ�ɹ���Ϣ			
			SendTableData(INVALID_CHAIR, CMD_GR_FRAME_DISMISS_RESULT, &DismissResult, sizeof(STR_CMD_GR_FRAME_DISMISS_RESULT), MDM_G_FRAME);

			//���÷��䲻���ڽ�ɢ״̬
			m_bUnderDissState = false;
			m_dissmisserChaiID = 0xFF;

			//��Ϸ�У���������Ϸ����ֽ�����
			if (m_pITableFrameSink!=NULL) 
				m_pITableFrameSink->OnEventGameConclude(INVALID_CHAIR, NULL, GER_DISMISS);

			return true;
		}		
		else if(nResponse == nPlayer)		//���������Ӧ���������δͬ��
		{
			//�������
			ZeroMemory(m_bResponseDismiss, sizeof(m_bResponseDismiss));
			ZeroMemory(m_bAgree, sizeof(m_bAgree));
			KillGameTimer(IDI_VOTE_DISMISS_ROOM);

			//��ֵ
			DismissResult.cbDismiss = 2;

			//�㲥�����ɢʧ����Ϣ			
			SendTableData(INVALID_CHAIR, CMD_GR_FRAME_DISMISS_RESULT, &DismissResult, sizeof(STR_CMD_GR_FRAME_DISMISS_RESULT), MDM_G_FRAME);

			//���÷��䲻���ڽ�ɢ״̬
			m_bUnderDissState = false;
			m_dissmisserChaiID = 0xFF;
		}
		else					//����һ�δ������м�״̬
		{
			//��ֵ
			DismissResult.cbDismiss = 0;

			//�㲥��ұ����Ϣ			
			SendTableData(INVALID_CHAIR, CMD_GR_FRAME_DISMISS_RESULT, &DismissResult, sizeof(STR_CMD_GR_FRAME_DISMISS_RESULT), MDM_G_FRAME);
		}
	}

	return false;
}

//������Ϸ
bool CTableFrame::ConcludeGame(BYTE cbGameStatus)
{
	//���÷��䲻���ڽ�ɢ״̬
	m_bUnderDissState = false;
	m_dissmisserChaiID = 0xFF;

	//�����ȴ����ѣ������Ҫ��׼��״̬�˳�Ϊ����
	if (cbGameStatus == GAME_CONCLUDE_CONTINUE)
	{
		m_bGameStarted=false;

		m_bDrawStarted=false;

		m_cbGameStatus=GAME_STATUS_FREE;

		m_bTableStarted=false;

		ZeroMemory(m_bAgree,sizeof(m_bAgree));

		if (m_pITableFrameSink!=NULL) m_pITableFrameSink->RepositionSink();

		//״̬�л�Ϊ���£����������뿪
		for (WORD wChairID = 0;wChairID<m_wChairCount;wChairID++)
		{
			//��ȡ�û�
			CPlayer * pIServerUserItem=GetTableUserItem(wChairID);
			//�û�����
			if (pIServerUserItem!=NULL)
			{
				//����״̬
				//pIServerUserItem->SetUserStatus(US_NULL,m_wTableID,wChairID);	
				//����״̬
				pIServerUserItem->SetUserStatus(US_SIT,m_wTableID,wChairID,false);
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
				//1. �������û����ڶ���״̬��δ����
				if ( US_OFFLINE == pIServerUserItem->GetUserStatus() )
				{
					//��Ϸ������  TODONOW
					pIServerUserItem->SetOfflineGameID(0);

					//���͸�Э��������, ��ת������¼��
					tagOfflineUser data;
					data.dwUserID = pIServerUserItem->GetUserID();
					data.byMask = 2; //��ʾɾ�������û�

					//���͸�Э��������
					m_pITCPSocketEngine->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_USER_OFFLINE,&data,sizeof(tagOfflineUser));
				}

				//2. �û�վ��
				PerformStandUpAction(pIServerUserItem);			
			}
		}

		//������������
		m_bGameStarted=false;
		m_bDrawStarted=false;
		m_cbGameStatus=GAME_STATUS_FREE;
		m_bTableStarted=false;
		m_dwGroupID = 0;
		m_dwCreateTableUser = 0;
		m_cbTableMode = 0;
		SetPassword(0);
		ZeroMemory(m_bAgree,sizeof(m_bAgree));
		if (m_pITableFrameSink!=NULL) m_pITableFrameSink->RepositionSink();

		return true;
	}

	//Ч��״̬
	if (m_bGameStarted==false) return false;

	//�������
	bool bDrawStarted=m_bDrawStarted;

	//����״̬
	m_bDrawStarted=false;
	m_cbGameStatus=cbGameStatus;
	m_bGameStarted=(cbGameStatus>=GAME_STATUS_PLAY)?true:false;
	m_wDrawCount++;

	KillGameTimer(IDI_GAME_CHECK);

	//��Ϸ��¼
	RecordGameScore(bDrawStarted);

	//��������
	if (m_bGameStarted==false)
	{
		//��������
		bool bOffLineWait=false;

		//�����û�
		for (WORD i=0;i<m_wChairCount && i<MAX_CHAIR;i++)
		{
			//��ȡ�û�
			CPlayer * pIServerUserItem=GetTableUserItem(i);

			//�û�����
			if (pIServerUserItem!=NULL)
			{
				//����״̬
				if (pIServerUserItem->GetUserStatus()==US_OFFLINE)
				{
					//���ߴ���
					bOffLineWait=true;
					PerformStandUpAction(pIServerUserItem);
				}
				else
				{
					//����״̬
					//pIServerUserItem->SetUserStatus(US_SIT,m_wTableID,i);

					PerformStandUpAction(pIServerUserItem);

				}
			}
		}
	}

	//��������
	if (m_pITableFrameSink!=NULL) m_pITableFrameSink->RepositionSink();

	//�߳����
	if (m_bGameStarted==false)
	{
		for (WORD i=0;i<m_wChairCount && i<MAX_CHAIR;i++)
		{
			//��ȡ�û�
			if (m_TableUserItemArray[i]==NULL) continue;
			CPlayer * pIServerUserItem=m_TableUserItemArray[i];

			//�ر��ж�
			if ((pIServerUserItem->GetMasterOrder()==0))
			{
				//������Ϣ
				LPCTSTR pszMessage=TEXT("����ϵͳά������ǰ��Ϸ���ӽ�ֹ�û�������Ϸ��");
				SendGameMessage(pIServerUserItem,pszMessage,SMT_EJECT|SMT_CHAT|SMT_CLOSE_GAME);

				//�û�����
				PerformStandUpAction(pIServerUserItem);

				continue;
			}
		}
	}

	//��������
	ConcludeTable();

	//����״̬
	SendTableStatus();

	return true;
}

//��������
bool CTableFrame::ConcludeTable()
{
	//��������
	if ( !m_bGameStarted && m_bTableStarted )
	{
		//�����ж�
		WORD wTableUserCount = GetSitUserCount();
		if (wTableUserCount==0) m_bTableStarted=false;
		if (m_pGameServiceAttrib->wChairCount==MAX_CHAIR) m_bTableStarted=false;

		//ģʽ�ж�
		if (m_cbStartMode==START_MODE_FULL_READY) m_bTableStarted=false;
		if (m_cbStartMode==START_MODE_PAIR_READY) m_bTableStarted=false;
		if (m_cbStartMode==START_MODE_ALL_READY) m_bTableStarted=false;
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

	//�������
	tagTableRule* pCfg = (tagTableRule*)GetCustomRule();
	BYTE byTableMode = pCfg->GameMode;

	//�����û��Ƹ� ��Ӧ���ݿ� round =1 
	XJModifyUserTreasure(byTableMode, byRound, lUserTreasure, pCfg);

	//�����û�����״̬
	XJUpdateGameTaskStatus(byTableMode, byRound); //TODONOW ��ʱֻΪ������

	//�۳��û���Ʊ -- ��Ӧ���ݿ� round = 0
	XJGameTickets(byTableMode, byRound);

	//�¼�֪ͨ
	g_AttemperEngineSink->ClubTableXJ(GetPassword());

	return true;
}


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
	CPlayer *pOwnerUserItem = SearchUserItem(m_dwTableOwner);
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
		pOwnerUserItem->ModifyUserTreasure(GetPassword(), byTableMode, 0, -cost, 0);//��������ʱ��� byRount = 0
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
			pOwnerUserItem->ModifyUserTreasure(GetPassword(), byTableMode, 0, -cost, 0);//��������ʱ��� byRount = 0
		}
		else if(pCfg->cbPayType == 1)						//AA֧��
		{
			//���������ϵ�ÿ���ˣ��۳�����
			for (WORD i=0; i<m_wChairCount && i<MAX_CHAIR; i++)
			{
				//��ȡ�û�
				CPlayer *pIServerUserItem = GetTableUserItem(i);
				if (pIServerUserItem == NULL) continue;

				BYTE byGameCountType = pCfg->GameCountType;
				SCORE cost =  byGameCountType;

				//�۳����� 
				pIServerUserItem->ModifyUserTreasure(GetPassword(), byTableMode, 0, -cost, 0);//��������ʱ��� byRount = 0
			} 
		}	
	}


	//�Ǿ��ֲ� ���ģʽ
	if( (byTableMode == TABLE_MODE_GOLD)
		&& (1 == byRound))
	{
		//���������ϵ�ÿ���ˣ��۳����
		for (WORD i=0; i<m_wChairCount && i<MAX_CHAIR; i++)
		{
			//��ȡ�û�
			CPlayer *pIServerUserItem = GetTableUserItem(i);
			if (pIServerUserItem == NULL) continue;

			SCORE cost =  pCfg->lSinglePayCost;

			//�۳����� 
			pIServerUserItem->ModifyUserTreasure(GetPassword(), byTableMode, 0, -cost, 0);//��������ʱ��� byRount = 0
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
		if(pIServerUserItem == NULL || pIServerUserItem->IsVirtualUser())
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
		pIServerUserItem->ModifyUserTreasure(GetPassword(), byTableMode, byRound, lGameScore[i], byWin);

		//added by WangChengQing ���ݿ�ˢ�����ӳ�, ��˴˴���ʱ�ȸ���һ������
		if(TABLE_MODE_FK_GOLD == byTableMode)
		{
			pIServerUserItem->SetUserGold(pIServerUserItem->GetUserGold() + lGameScore[i]);
		}

		//�볡У��
		if (CheckUserLeave(byTableMode,  pIServerUserItem, pCfg))
		{
			bIsMoneyEmpty = true;

			/*
			//֪ͨ�û��Ƹ����㣬�޷�������Ϸ		
			STR_CMD_GR_FRAME_TREASURE_NO CMD;
			CMD.wChairID = i;
			lstrcpyn(CMD.szDescribe,TEXT("���Ҳ��㣬�޷�������Ϸ"),CountArray(CMD.szDescribe));		
			SendTableData(i, CMD_GR_FRAME_TREASURE_NO, &CMD, sizeof(CMD),MDM_G_FRAME);
			*/

			//�û�����		TODO ��������
			//PerformStandUpAction(pIServerUserItem);
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
			if(pIServerUserItem == NULL || pIServerUserItem->IsVirtualUser())
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

//С����Ϸ�����󣬸�����Ϸ����״̬
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
		if(pIServerUserItem == NULL || pIServerUserItem->IsVirtualUser())
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

//д��¼���¼ ���� С����,���ݺͳ���
bool CTableFrame::WriteRecordInfo(WORD wXJCount,TCHAR strScore[], VOID* pData, DWORD dwDataSize)
{
	//��ӱ�����Ϣ:����� ��Ϸģʽ ����
	DBR_GR_GameRecordInfo GameRecordInfo;
	ZeroMemory(&GameRecordInfo,sizeof(DBR_GR_GameRecordInfo));

	//��ֵ
	GameRecordInfo.dwTableID = GetPassword();	
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

//�����޶�
SCORE CTableFrame::QueryConsumeQuota(CPlayer * pIServerUserItem)
{
	//�û�Ч��
	ASSERT(pIServerUserItem->GetTableID()==m_wTableID);
	if (pIServerUserItem->GetTableID()!=m_wTableID) return 0L;

	//��ѯ���
	SCORE lTrusteeScore=pIServerUserItem->GetTrusteeScore();
	SCORE lUserConsumeQuota=m_pITableFrameSink->QueryConsumeQuota(pIServerUserItem);

	//Ч����
	ASSERT((lUserConsumeQuota>=0L)&&(lUserConsumeQuota<=pIServerUserItem->GetUserScore()-lMinEnterScore));
	if ((lUserConsumeQuota<0L)) return 0L;

	return lUserConsumeQuota+lTrusteeScore;
}

//Ѱ���û�
CPlayer * CTableFrame::SearchUserItem(DWORD dwUserID)
{
	//��������
	WORD wEnumIndex=0;
	CPlayer * pIServerUserItem=NULL;

	//�����û�
	for (WORD i=0;i<m_wChairCount && i<MAX_CHAIR;i++)
	{
		pIServerUserItem=GetTableUserItem(i);
		if ((pIServerUserItem!=NULL)&&(pIServerUserItem->GetUserID()==dwUserID)) return pIServerUserItem;
	}

	//�Թ��û�
	do
	{
		pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if ((pIServerUserItem!=NULL)&&(pIServerUserItem->GetUserID()==dwUserID)) return pIServerUserItem;
	} while (pIServerUserItem!=NULL);

	return NULL;
}

//��Ϸ�û�
CPlayer * CTableFrame::GetTableUserItem(WORD wChairID)
{
	//Ч�����
	ASSERT(wChairID<m_wChairCount);
	if (wChairID>=m_wChairCount) return NULL;

	//��ȡ�û�
	return m_TableUserItemArray[wChairID];
}

//�Թ��û�
CPlayer * CTableFrame::EnumLookonUserItem(WORD wEnumIndex)
{
	if (wEnumIndex>=m_LookonUserItemArray.GetCount()) return NULL;
	return m_LookonUserItemArray[wEnumIndex];
}

//����ʱ��
bool CTableFrame::SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter)
{
	//Ч�����
	ASSERT((dwTimerID>0)&&(dwTimerID<TIME_TABLE_MODULE_RANGE));
	if ((dwTimerID<=0)||(dwTimerID>=TIME_TABLE_MODULE_RANGE)) return false;

	//����ʱ��
	DWORD dwEngineTimerID=IDI_TABLE_MODULE_START+m_wTableID*TIME_TABLE_MODULE_RANGE;
	if (m_pITimerEngine!=NULL) m_pITimerEngine->SetTimer(dwEngineTimerID+dwTimerID,dwElapse,dwRepeat,dwBindParameter);

	return true;
}

//ɾ��ʱ��
bool CTableFrame::KillGameTimer(DWORD dwTimerID)
{
	//Ч�����
	ASSERT((dwTimerID>0)&&(dwTimerID<=TIME_TABLE_MODULE_RANGE));
	if ((dwTimerID<=0)||(dwTimerID>TIME_TABLE_MODULE_RANGE)) return false;

	//ɾ��ʱ��
	DWORD dwEngineTimerID=IDI_TABLE_MODULE_START+m_wTableID*TIME_TABLE_MODULE_RANGE;
	if (m_pITimerEngine!=NULL) m_pITimerEngine->KillTimer(dwEngineTimerID+dwTimerID);

	return true;
}

//��ȡ��ʱ��ʣ�������
DWORD CTableFrame::GetTimerLeftTickCount(DWORD dwTimerID)
{
	//Ч�����
	ASSERT((dwTimerID>0)&&(dwTimerID<=TIME_TABLE_MODULE_RANGE));
	if ((dwTimerID<=0)||(dwTimerID>TIME_TABLE_MODULE_RANGE)) return false;

	//ɾ��ʱ��
	DWORD dwEngineTimerID=IDI_TABLE_MODULE_START+m_wTableID*TIME_TABLE_MODULE_RANGE;
	if (m_pITimerEngine!=NULL) return m_pITimerEngine->GetTimerLeftTickCount(dwEngineTimerID+dwTimerID);

	return 0;

}

//��������
bool CTableFrame::SendUserItemData(CPlayer * pIServerUserItem, WORD wSubCmdID)
{
	//״̬Ч��
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->IsClientReady()==true));
	if ((pIServerUserItem==NULL)&&(pIServerUserItem->IsClientReady()==false)) return false;

	//��������
	g_GameCtrl->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);

	return true;
}

//��������
bool CTableFrame::SendUserItemData(CPlayer * pIServerUserItem, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//״̬Ч��
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->IsClientReady()==true));
	if ((pIServerUserItem==NULL)&&(pIServerUserItem->IsClientReady()==false)) return false;

	//��������
	g_GameCtrl->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,pData,wDataSize);

	return true;
}

//��������
bool CTableFrame::SendTableData(WORD wChairID, WORD wSubCmdID)
{
	//�û�Ⱥ��
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<m_wChairCount && i<MAX_CHAIR;i++)
		{
			//��ȡ�û�
			CPlayer * pIServerUserItem=GetTableUserItem(i);
			if (pIServerUserItem==NULL) continue;

			//Ч��״̬
			//ASSERT(pIServerUserItem->IsClientReady()==true);
			if (pIServerUserItem->IsClientReady()==false) continue;

			//��������
			g_GameCtrl->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);
		}

		return true;
	}
	else
	{
		//��ȡ�û�
		CPlayer * pIServerUserItem=GetTableUserItem(wChairID);
		if (pIServerUserItem==NULL) return false;

		//Ч��״̬
		ASSERT(pIServerUserItem->IsClientReady()==true);
		if (pIServerUserItem->IsClientReady()==false) return false;

		//��������
		g_GameCtrl->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);

		return true;
	}

	return false;
}

//��������
bool CTableFrame::SendTableData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize,WORD wMainCmdID)
{
	//�û�Ⱥ��
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<m_wChairCount && i<MAX_CHAIR;i++)
		{
			//��ȡ�û�
			CPlayer * pIServerUserItem=GetTableUserItem(i);
			if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

			//��ֹ����ͬһ��������  ���������յ���Ϣ��
			if(pIServerUserItem->GetTableID() != m_wTableID) continue;

			//��������
			g_GameCtrl->SendData(pIServerUserItem,wMainCmdID,wSubCmdID,pData,wDataSize);
		}

		return true;
	}
	else
	{
		//��ȡ�û�
		CPlayer * pIServerUserItem=GetTableUserItem(wChairID);

		if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) return false;
		//��������
		g_GameCtrl->SendData(pIServerUserItem,wMainCmdID,wSubCmdID,pData,wDataSize);

		return true;
	}

	return false;
}

//��������
bool CTableFrame::SendLookonData(WORD wChairID, WORD wSubCmdID)
{
	//��������
	WORD wEnumIndex=0;
	CPlayer * pIServerUserItem=NULL;

	//ö���û�
	do
	{
		//��ȡ�û�
		pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if(pIServerUserItem == NULL)
		{
			break;
		}

		if(wChairID == INVALID_CHAIR && pIServerUserItem->IsClientReady()==false)
		{
			continue;
		}
		else
		{
			//Ч��״̬
			ASSERT(pIServerUserItem->IsClientReady()==true);
			if (pIServerUserItem->IsClientReady()==false) return false;
		}

		//��������
		if ((wChairID==INVALID_CHAIR)||(pIServerUserItem->GetChairID()==wChairID))
		{
			g_GameCtrl->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);
		}

	} while (true);

	return true;
}

//��������
bool CTableFrame::SendLookonData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//��������
	WORD wEnumIndex=0;
	CPlayer * pIServerUserItem=NULL;

	//ö���û�
	do
	{
		//��ȡ�û�
		pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if(pIServerUserItem == NULL)
		{
			break;
		}

		if(wChairID == INVALID_CHAIR && pIServerUserItem->IsClientReady()==false)
		{
			continue;
		}
		else
		{
			//Ч��״̬
			ASSERT(pIServerUserItem->IsClientReady()==true);
			if (pIServerUserItem->IsClientReady()==false) return false;
		}

		//��������
		if ((wChairID==INVALID_CHAIR)||(pIServerUserItem->GetChairID()==wChairID))
		{
			g_GameCtrl->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,pData,wDataSize);
		}

	} while (true);

	return true;
}

//������Ϣ
bool CTableFrame::SendGameMessage(LPCTSTR lpszMessage, WORD wType)
{
	//��������
	WORD wEnumIndex=0;

	//������Ϣ
	for (WORD i=0;i<m_wChairCount && i<MAX_CHAIR;i++)
	{
		//��ȡ�û�
		CPlayer * pIServerUserItem=GetTableUserItem(i);
		if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

		//������Ϣ
		g_GameCtrl->SendGameMessage(pIServerUserItem,lpszMessage,wType);
	}

	//ö���û�
	do
	{
		//��ȡ�û�
		CPlayer * pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if (pIServerUserItem==NULL) break;

		//Ч��״̬
		ASSERT(pIServerUserItem->IsClientReady()==true);
		if (pIServerUserItem->IsClientReady()==false) return false;

		//������Ϣ
		g_GameCtrl->SendGameMessage(pIServerUserItem,lpszMessage,wType);

	} while (true);

	return true;
}

//������Ϣ
bool CTableFrame::SendRoomMessage(CPlayer * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;
	if(pIServerUserItem->IsVirtualUser())
		return true;
	//������Ϣ
	g_GameCtrl->SendRoomMessage(pIServerUserItem,lpszMessage,wType);

	return true;
}

//��Ϸ��Ϣ
bool CTableFrame::SendGameMessage(CPlayer * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;
	if(pIServerUserItem->IsVirtualUser())
		return true;
	//������Ϣ
	return g_GameCtrl->SendGameMessage(pIServerUserItem,lpszMessage,wType);
}


//���ͳ���
bool CTableFrame::SendGameScene(CPlayer * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	//�û�Ч��
	if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) return false;
	if(pIServerUserItem->IsVirtualUser())
	{
		return true;
	}

	g_GameCtrl->SendData(pIServerUserItem,MDM_G_FRAME,CMD_GR_FRAME_GAME_OPTION,pData,wDataSize);

	return true;
}

//�����¼�
bool CTableFrame::OnEventUserOffLine(CPlayer * pIServerUserItem)
{
	//����Ч��
	if (pIServerUserItem==NULL) return false;

	//�û�����
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
	CPlayer * pITableUserItem=m_TableUserItemArray[pUserInfo->wChairID];

	//�û�����
	WORD wChairID=pIServerUserItem->GetChairID();
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

	//���ߴ���		��Ϊ�� ���� ��Ϊnull  �ͽ��ж�������		
	if ( (cbUserStatus == US_PLAYING)  || (cbUserStatus == US_SIT) || (cbUserStatus == US_READY))
	{
		//У���û�
		if (pIServerUserItem!=GetTableUserItem(wChairID)) return false;

		//�û�����
		pIServerUserItem->SetClientReady(false);
		pIServerUserItem->SetOldGameStatus(cbUserStatus);
		pIServerUserItem->SetUserStatus(US_OFFLINE,m_wTableID,wChairID);

		if (m_pITableFrameSink!=NULL)
		{
			m_pITableFrameSink->OnActionUserNetCut(wChairID,pIServerUserItem,false);
		}
		else
		{
			m_pITableFrameSink->OnActionUserNetCutT(wChairID,pIServerUserItem,false);
		}

		return true;

	}
	else	//�Ƕ��ߴ��� 
	{
		//�û�����  Ϊ�˱��⣺���û��������ӣ�����Ϸδ��ʼ�͵��ߣ��ᵼ���޷����¼��뷿�䣬���Ҵ�������֮��ʱ�������߳�����ʱֱ���߳�
		PerformStandUpAction(pIServerUserItem);

		return false;
	}

	return false;
}

//�����¼�
bool CTableFrame::OnUserScroeNotify(WORD wChairID, CPlayer * pIServerUserItem, BYTE cbReason)
{
	//֪ͨ��Ϸ
	return m_pITableFrameSink->OnUserScroeNotify(wChairID,pIServerUserItem,cbReason);
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
	case IDI_GAME_CHECK:
		{
			//Ч��״̬
			ASSERT(m_bGameStarted==true);
			if (m_bGameStarted==false) return false;

			DismissGame();

			return true;
		}
	case IDI_ROOM_AUTO_DISMISS:			//�����Զ���ɢ��ʱ��
		{
			KillGameTimer(IDI_ROOM_AUTO_DISMISS);
			//��÷���
			CPlayer *pIServerUserItem = SearchUserItem(m_dwTableOwner);
			if ( NULL != pIServerUserItem)
			{
				//��ɢ����
				OnEventApplyDismissRoom(pIServerUserItem->GetChairID(), true);
			}

			return true;
		}
	case IDI_VOTE_DISMISS_ROOM:		//�����ɢ���䶨ʱ��
		{
			KillGameTimer(IDI_VOTE_DISMISS_ROOM);
			//Ĭ��ͬ���ɢ
			for (int i=0; i<m_wChairCount && i<MAX_CHAIR; i++)
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
			DismissResult.cbAgree = 1;					
			if (GetGameStatus() != GAME_STATUS_FREE)
			{
				DismissResult.cbClubQuit = 1;
			}

			SendTableData(INVALID_CHAIR, CMD_GR_FRAME_DISMISS_RESULT, &DismissResult, sizeof(STR_CMD_GR_FRAME_DISMISS_RESULT), MDM_G_FRAME);

			//֪ͨ����Ϸ��ɢ����
			//����״̬ ֱ�ӽ�ɢ
			if(GAME_STATUS_FREE == GetGameStatus())
			{
				ConcludeGame(GAME_CONCLUDE_NORMAL);
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

			ConcludeGame(0xFE);

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
				SendTableData(INVALID_CHAIR,SUB_GR_LOTTERY_NOTIFY,NULL,0,MDM_G_FRAME);
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
	bool ret = m_pITableFrameSink->OnGameMessage(wSubCmdID,pData,wDataSize,pIServerUserItem);
	if(!ret)
	{
	}

	//��Ϣ����
	return ret;
}

//����¼�
bool CTableFrame::OnEventSocketFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, CPlayer * pIServerUserItem)
{
	//��Ϸ���� �˴������Ӧ����ֻ��200������Ϣ�� TODONOW ϸ��
	if (m_pITableFrameSink->OnFrameMessage(wSubCmdID,pData,wDataSize,pIServerUserItem)==true) return true;

	//Ĭ�ϴ���
	switch (wSubCmdID)
	{
	case SUB_RG_FRAME_OPTION:	//��Ϸ����
		{
			//Ч�����
			if (wDataSize!=sizeof(CMD_GF_GameOption)) return false;

			//��������
			CMD_GF_GameOption * pGameOption=(CMD_GF_GameOption *)pData;

			//��ȡ����
			WORD wChairID=pIServerUserItem->GetChairID();
			BYTE cbUserStatus=pIServerUserItem->GetUserStatus();


			//����״̬
			pIServerUserItem->SetClientReady(true);
			if (cbUserStatus!=US_LOOKON) m_bAllowLookon[wChairID]=pGameOption->cbAllowLookon?true:false;

			//����״̬
			CMD_GF_GameStatus GameStatus;
			GameStatus.cbGameStatus=m_cbGameStatus;
			GameStatus.cbAllowLookon=m_bAllowLookon[wChairID]?TRUE:FALSE;

			g_GameCtrl->SendData(pIServerUserItem,MDM_G_FRAME,CMD_GR_FRAME_GAME_STATUS,&GameStatus,sizeof(GameStatus));

			//���ͳ���
			bool bSendSecret=((cbUserStatus!=US_LOOKON)||(m_bAllowLookon[wChairID]==true));
			m_pITableFrameSink->OnEventSendGameScene(wChairID,pIServerUserItem,m_cbGameStatus,bSendSecret);

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

				for(int i=0; i<MAX_CHAIR && num <=(MAX_CHAIR-1); i++)
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
	case SUB_GF_USER_READY:		//�û�׼��
		{
			//��ȡ����
			WORD wChairID=pIServerUserItem->GetChairID();
			BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

			//Ч��״̬
			if (GetTableUserItem(wChairID)!=pIServerUserItem) return false;

			//Ч��״̬
			if (cbUserStatus!=US_SIT) 
			{
				return true;
			}

			//�¼�֪ͨ
			if (m_pITableFrameSink!=NULL)
			{
				m_pITableFrameSink->OnActionUserOnReady(wChairID,pIServerUserItem,pData,wDataSize);
			}

			pIServerUserItem->SetUserStatus(US_READY,m_wTableID,wChairID);

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
			CPlayer *pIServerUserItem = SearchUserItem(pTableChat->dwUserID);
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
			SendTableData(INVALID_CHAIR, CMD_GR_FRAME_CHAT, &CMDChat, sizeof(STR_CMD_GR_FRAME_CHAT), MDM_G_FRAME);

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
			if ((pLookonConfig->dwUserID!=0)&&(SearchUserItem(pLookonConfig->dwUserID)==NULL))
			{
				ASSERT(FALSE);
				return true;
			}

			//�û�Ч��
			ASSERT(pIServerUserItem->GetUserStatus()!=US_LOOKON);
			if (pIServerUserItem->GetUserStatus()==US_LOOKON) return false;

			//�Թ۴���
			if (pLookonConfig->dwUserID!=0L)
			{
				for (INT_PTR i=0;i<m_LookonUserItemArray.GetCount();i++)
				{
					//��ȡ�û�
					CPlayer * pILookonUserItem=m_LookonUserItemArray[i];
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
				//�����ж�
				bool bAllowLookon=(pLookonConfig->cbAllowLookon==TRUE)?true:false;
				if (bAllowLookon==m_bAllowLookon[pIServerUserItem->GetChairID()]) return true;

				//���ñ���
				m_bAllowLookon[pIServerUserItem->GetChairID()]=bAllowLookon;

				//������Ϣ
				CMD_GF_LookonStatus LookonStatus;
				LookonStatus.cbAllowLookon=pLookonConfig->cbAllowLookon;

				//������Ϣ
				for (INT_PTR i=0;i<m_LookonUserItemArray.GetCount();i++)
				{
					//��ȡ�û�
					CPlayer * pILookonUserItem=m_LookonUserItemArray[i];
					if (pILookonUserItem->GetChairID()!=pIServerUserItem->GetChairID()) continue;

					//������Ϣ
					g_GameCtrl->SendData(pILookonUserItem,MDM_G_FRAME,CMD_GF_LOOKON_STATUS,&LookonStatus,sizeof(LookonStatus));
				}
			}

			return true;
		}
	case SUB_RG_FRAME_ASK_DISMISS:		//�����ɢ����
		{
			//0. ���ݰ�У��
			if ( wDataSize != sizeof(STR_SUB_RG_FRAME_ASK_DISMISS) )
				return false;

			STR_SUB_RG_FRAME_ASK_DISMISS *pApply = (STR_SUB_RG_FRAME_ASK_DISMISS*)pData;

			//1. ��ɢУ��
			if (0 == pApply->cbAgree)
			{
				return true;
			}

			//2. У�鷿���Ƿ��Ѿ����ڽ�ɢ״̬
			if (m_bUnderDissState || NULL == pIServerUserItem)
			{
				return true;
			}

			//���µ����ɢ��ť����Ҫɾ����ʱ������ֹһ���˲�ͣ�����ɢ����ʱ�����Զ��˳�����
			KillGameTimer(IDI_VOTE_DISMISS_ROOM);

			OnEventApplyDismissRoom(pIServerUserItem->GetChairID(), (pApply->cbAgree != 0)); //0Ϊfalse  ����Ϊtrue

			return true;
		}
	case SUB_RG_FRAME_VOTE_DISMISS:		//�����ɢ����
		{
			//���ݰ�У��
			if ( wDataSize != sizeof(STR_SUB_RG_FRAME_VOTE_DISMISS) )
				return false;

			//��������
			STR_SUB_RG_FRAME_VOTE_DISMISS *pVote = (STR_SUB_RG_FRAME_VOTE_DISMISS*)pData;

			//��ɢ�ɹ�
			OnEventVoteDismissRoom(pIServerUserItem->GetChairID(), (pVote->cbAgree != 0 ));
			return true;

		}
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
	DWORD ChairCount = MAX_CHAIR;
	if(m_wChairCount > 0)
	{
		ChairCount = m_wChairCount;
	}

	//��������
	for (WORD i=0;i<ChairCount;i++)
	{
		if (m_TableUserItemArray[i]==NULL)
		{
			return i;
		}
	}

	return INVALID_CHAIR;
}

//�����λ
WORD CTableFrame::GetRandNullChairID()
{
	//��������
	WORD wIndex = rand()%m_wChairCount;
	for (WORD i=wIndex;i<m_wChairCount && i<MAX_CHAIR+wIndex;i++)
	{
		if (m_TableUserItemArray[i%m_wChairCount]==NULL)
		{
			return i%m_wChairCount;
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
	for (WORD i=0; i<m_wChairCount && i<MAX_CHAIR; i++)
	{
		if (GetTableUserItem(i)!=NULL)
		{
			wUserCount++;
		}
	}

	return wUserCount;
}

//�Թ���Ŀ
WORD CTableFrame::GetLookonUserCount()
{
	//��ȡ��Ŀ
	INT_PTR nLookonCount=m_LookonUserItemArray.GetCount();

	return (WORD)(nLookonCount);
}

//����״��
WORD CTableFrame::GetTableUserInfo(tagTableUserInfo & TableUserInfo)
{
	//���ñ���
	ZeroMemory(&TableUserInfo,sizeof(TableUserInfo));

	//�û�����
	for (WORD i=0;i<m_pGameServiceAttrib->wChairCount;i++)
	{
		//��ȡ�û�
		CPlayer * pIServerUserItem=GetTableUserItem(i);
		if (pIServerUserItem==NULL) continue;

		//�û�����
		if (pIServerUserItem->IsAndroidUser()==false)
		{
			TableUserInfo.wTableUserCount++;
		}
		else
		{
			TableUserInfo.wTableAndroidCount++;
		}

		//׼���ж�
		if (pIServerUserItem->GetUserStatus()==US_READY)
		{
			TableUserInfo.wTableReadyCount++;
		}
	}

	//������Ŀ
	switch (m_cbStartMode)
	{
	case START_MODE_ALL_READY:		//����׼��
		{
			TableUserInfo.wMinUserCount=1;
			break;
		}
	case START_MODE_PAIR_READY:		//��Կ�ʼ
		{
			TableUserInfo.wMinUserCount=2;
			break;
		}
	case START_MODE_TIME_CONTROL:	//ʱ�����
		{
			TableUserInfo.wMinUserCount=1;
			break;
		}
	default:						//Ĭ��ģʽ
		{
			TableUserInfo.wMinUserCount=m_pGameServiceAttrib->wChairCount;
			break;
		}
	}

	return TableUserInfo.wTableAndroidCount+TableUserInfo.wTableUserCount;
}

//��������
bool CTableFrame::InitializationFrame(WORD wTableID, tagTableFrameParameter & TableFrameParameter)
{
	//���ñ���
	m_wTableID=wTableID;
	m_wChairCount=TableFrameParameter.pGameServiceAttrib->wChairCount;

	//���ò���
	m_pGameParameter=TableFrameParameter.pGameParameter;
	m_pGameServiceAttrib=TableFrameParameter.pGameServiceAttrib;
	m_pGameServiceOption=TableFrameParameter.pGameServiceOption;


	//�����ж�
	if (m_pITableFrameSink==NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	IUnknownEx * pITableFrame=static_cast<IUnknownEx*>(this);
	if (m_pITableFrameSink->Initialization(pITableFrame)==false) return false;

	return true;
}

//��������
bool CTableFrame::PerformStandUpAction(CPlayer *pIServerUserItem)
{
	//Ч�����
	if ( (NULL == pIServerUserItem) || 
		(pIServerUserItem->GetTableID() != m_wTableID) || 
		(pIServerUserItem->GetChairID() >= m_wChairCount) )
	{
		return false;
	}

	//�û�����
	WORD wChairID = pIServerUserItem->GetChairID();
	BYTE cbUserStatus = pIServerUserItem->GetUserStatus();
	//�����û�
	CPlayer *pITableUserItem = GetTableUserItem(wChairID);

	//�û�Ϊ�����û�
	if (pIServerUserItem == pITableUserItem)
	{
		//TODONOW added by WangChengQing �˴�Ӧ������У��
		tagTableRule *pTableCfg = (tagTableRule*)GetCustomRule();

		/* ֪ͨ���ݿ�, ������� */
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
		g_AttemperEngineSink->ClubPlayerSitUp(GetPassword(), pIServerUserItem->GetUserID(),0, byClubOrHalGold);


		/* ֪ͨ���ݿ�, ����Ϊ�ɼ��� -- ����������Ա״̬, �����뿪��, ��������Ϊ�ɼ��� */
		BYTE byPlayerNum = pTableCfg->PlayerCount;
		if(GetSitUserCount() == byPlayerNum)
		{
			g_AttemperEngineSink->ClubTableNotMax(GetPassword(), byClubOrHalGold);
		}

		//TODO ���û����µĶ�Ӧ������Ӧ��ֻ��Ҫ��¼�����˿������û��뿪��Ϣ
		g_AttemperEngineSink->LeaveTable(m_wTableID,pIServerUserItem->GetUserID());

		//�����û���ΪNULL
		m_TableUserItemArray[wChairID] = NULL;

		//�¼�֪ͨ��֪ͨ����Ϸ�û�վ��
		if (m_pITableFrameSink!=NULL)
		{
			m_pITableFrameSink->OnActionUserStandUp(wChairID, pIServerUserItem, false);
		}
		else
		{
			m_pITableFrameSink->OnActionUserStandUpT(wChairID, pIServerUserItem, false);
		}

		//�����û�״̬
		pIServerUserItem->SetClientReady(false);
		pIServerUserItem->SetUserStatus(US_FREE, INVALID_TABLE, INVALID_CHAIR);
		//pIServerUserItem->SetUserStatus((cbUserStatus==US_OFFLINE) ? US_OFFLINE:US_FREE, INVALID_TABLE, INVALID_CHAIR);

		//��������
		bool bTableLocked = IsTableLocked();
		bool bTableStarted = IsTableStarted();
		WORD wTableUserCount = GetSitUserCount();

		//���������û�����
		m_wUserCount = wTableUserCount;

		//�����һ���û�Ҳ�뿪��,��������
		if (wTableUserCount==0)
		{
			g_AttemperEngineSink->ClubLastPlayerSitUp(GetPassword(), pIServerUserItem->GetUserID(),0, byClubOrHalGold);

			//������������
			m_dwTableOwner = 0L;			
			m_bGameStarted = false;
			m_bDrawStarted = false;
			m_bTableStarted = false;
			m_cbGameStatus = GAME_STATUS_FREE;		
			m_dwGroupID = 0;
			m_dwCreateTableUser = 0;
			SetPassword(0);
			ZeroMemory(m_szEnterPassword,sizeof(m_szEnterPassword));
			ZeroMemory(m_bAgree,sizeof(m_bAgree));

			//����Ϸ��λ
			if (m_pITableFrameSink!=NULL) 
				m_pITableFrameSink->RepositionSink();	

			//�����Թ�
			for (int i=0; i<m_LookonUserItemArray.GetCount(); i++)
			{
				SendGameMessage(m_LookonUserItemArray[i], TEXT("�˷����ѽ�ɢ��"), SMT_CLOSE_GAME|SMT_EJECT);
			}
		}

		//��������,��������δδ��ʼ״̬
		ConcludeTable();


		//����״̬
		if ((bTableLocked != IsTableLocked()) || (bTableStarted != IsTableStarted()))
		{
			SendTableStatus();
		}

		return true;
	}
	else		//�û������������û�
	{
		//�ж��û��ǲ����Թ��û�
		for (int i=0; i<m_LookonUserItemArray.GetCount(); i++)
		{
			//�Թ��û�
			if (pIServerUserItem == m_LookonUserItemArray[i])
			{
				//ɾ������
				m_LookonUserItemArray.RemoveAt(i);

				//�¼�֪ͨ
				if (m_pITableFrameSink!=NULL)
				{
					m_pITableFrameSink->OnActionUserStandUp(wChairID,pIServerUserItem,true);
				}
				else
				{
					m_pITableFrameSink->OnActionUserStandUpT(wChairID,pIServerUserItem,false);
				}

				//�û�״̬
				pIServerUserItem->SetClientReady(false);
				pIServerUserItem->SetUserStatus(US_FREE,INVALID_TABLE,INVALID_CHAIR);

				return true;
			}
		}
	}

	return true;
}

//�Թ۶���
bool CTableFrame::PerformLookonAction(WORD wChairID, CPlayer * pIServerUserItem)
{
	//Ч�����
	ASSERT((pIServerUserItem!=NULL)&&(wChairID<m_wChairCount));
	ASSERT((pIServerUserItem->GetTableID()==INVALID_TABLE)&&(pIServerUserItem->GetChairID()==INVALID_CHAIR));

	//��������
	tagUserInfo * pUserInfo=pIServerUserItem->GetUserInfo();
	tagUserRule * pUserRule=pIServerUserItem->GetUserRule();
	CPlayer * pITableUserItem=GetTableUserItem(wChairID);

	//��Ϸ״̬
	if ((m_bGameStarted==false)&&(pIServerUserItem->GetMasterOrder()==0L))
	{
		SendRequestFailure(pIServerUserItem,TEXT("��Ϸ��û�п�ʼ�������Թ۴���Ϸ����"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	////�Թ��ж�
	//if (CServerRule::IsAllowAndroidSimulate(m_pGameServiceOption->dwServerRule)==true
	//	&& (CServerRule::IsAllowAndroidAttend(m_pGameServiceOption->dwServerRule)==false))
	//{
	//	if ((pITableUserItem!=NULL)&&(pITableUserItem->IsAndroidUser()==true))
	//	{
	//		SendRequestFailure(pIServerUserItem,TEXT("��Ǹ����ǰ��Ϸ�����ֹ�û��Թۣ�"),REQUEST_FAILURE_NORMAL);
	//		return false;
	//	}
	//}

	//״̬�ж�
	//if ((CServerRule::IsForfendGameLookon(m_pGameServiceOption->dwServerRule)==true)&&(pIServerUserItem->GetMasterOrder()==0))
	//{
	//	SendRequestFailure(pIServerUserItem,TEXT("��Ǹ����ǰ��Ϸ�����ֹ�û��Թۣ�"),REQUEST_FAILURE_NORMAL);
	//	return false;
	//}

	//�����ж�
	if ((pITableUserItem==NULL)&&(pIServerUserItem->GetMasterOrder()==0L))
	{
		SendRequestFailure(pIServerUserItem,TEXT("���������λ��û����Ϸ��ң��޷��Թ۴���Ϸ��"),REQUEST_FAILURE_NORMAL);
		return false;
	}

	//����Ч��
	if ((IsTableLocked()==true)&&(pIServerUserItem->GetMasterOrder()==0L)&&(lstrcmp(pUserRule->szPassword,m_szEnterPassword)!=0))
	{
		SendRequestFailure(pIServerUserItem,TEXT("��Ϸ���������벻��ȷ�������Թ���Ϸ��"),REQUEST_FAILURE_PASSWORD);
		return false;
	}

	//��չЧ��
	if (m_pITableFrameSink!=NULL)
	{
		//��������
		tagRequestResult RequestResult;
		ZeroMemory(&RequestResult,sizeof(RequestResult));

		//����Ч��
		if (m_pITableFrameSink->OnUserRequestLookon(wChairID,pIServerUserItem,RequestResult)==false)
		{
			//������Ϣ
			SendRequestFailure(pIServerUserItem,RequestResult.szFailureReason,RequestResult.cbFailureCode);

			return false;
		}
	}

	//�����û�
	m_LookonUserItemArray.Add(pIServerUserItem);

	//�û�״̬
	pIServerUserItem->SetClientReady(false);
	pIServerUserItem->SetUserStatus(US_LOOKON,m_wTableID,wChairID);

	//�¼�֪ͨ
	if (m_pITableFrameSink!=NULL)
	{
		m_pITableFrameSink->OnActionUserSitDown(wChairID,pIServerUserItem,true);
	}
	else
	{
		m_pITableFrameSink->OnActionUserSitDownT(wChairID,pIServerUserItem,true);
	}

	return true;
}

//���¶���
bool CTableFrame::PerformSitDownAction(WORD wChairID, CPlayer * pIServerUserItem, LPCTSTR lpszPassword, bool bCheckUserGPS)
{
	/* 0. Ч����� */
	if (NULL == pIServerUserItem)
	{
		return false;
	}
	
	if(wChairID > m_wChairCount)
	{
		return false;
	}

	//�������� TOODNOWW added by WangChengQing 2018/7/5 �˴�Ӧ�������ж�
	tagUserInfo *pUserInfo = pIServerUserItem->GetUserInfo();			//�û�������Ϣ
	tagUserRule *pUserRule = pIServerUserItem->GetUserRule();			//TODO ����û�������ò����ʱֻ�õ���������
	CPlayer *pITableUserItem = GetTableUserItem(wChairID);		//�����û����϶�ΪNULL������У���û��ǲ����Ѿ���������
	tagTableRule *pTableCfg = (tagTableRule*)GetCustomRule();


	/* �ж��������Ƿ����� */
	if ( (NULL != pITableUserItem) && 
		(pIServerUserItem->GetUserID() != pITableUserItem->GetUserID()) )
	{
		//������Ϣ
		TCHAR szDescribe[128]=TEXT("");
		_sntprintf_s(szDescribe,CountArray(szDescribe), TEXT("�����Ѿ��� [ %s ] �����ȵ��ˣ��´ζ���Ҫ����ˣ�"), pITableUserItem->GetNickName());

		//������Ϣ
		SendRequestFailure(pIServerUserItem,szDescribe,REQUEST_FAILURE_NORMAL);
		return false;
	}

	/* ����Ч��:����������״̬�Ҳ��ǹ���ԱȨ�ޣ������������ */
	if( ((IsTableLocked()) && (pIServerUserItem->GetMasterOrder()==0L))
		&&((lpszPassword==NULL)||(lstrcmp(lpszPassword, m_szEnterPassword)!=0)))
	{
		SendRequestFailure(pIServerUserItem, TEXT("��Ϸ���������벻��ȷ�����ܼ�����Ϸ��"), REQUEST_FAILURE_PASSWORD);
		return false;
	}

	//IPЧ����û�����У��
	//if (EfficacyIPAddress(pIServerUserItem)==false) return false;
	//if (EfficacyScoreRule(pIServerUserItem)==false) return false;
	 
	/* ��չЧ�� */
	if (NULL != m_pITableFrameSink)
	{
		//��������
		tagRequestResult RequestResult;
		ZeroMemory(&RequestResult,sizeof(RequestResult));

		//����Ч��
		if ( !m_pITableFrameSink->OnUserRequestSitDown(wChairID, pIServerUserItem, RequestResult) )
		{
			//������Ϣ
			SendRequestFailure(pIServerUserItem, RequestResult.szFailureReason, RequestResult.cbFailureCode);
			return false;
		}
	}

	//�û��û����������û�
	m_TableUserItemArray[wChairID] = pIServerUserItem;
	//��Ϸ����
	m_wDrawCount = 0;

	//TODO ����Ӧ����ҪУ�鷿��ģʽ����Ϊ���˿�������Ҫ�������ӵĴ���
	//g_AttemperEngineSink->JoinTable(m_wTableID, pIServerUserItem->GetUserID());

	/* ������Ϸ�û�״̬ */

	//����״̬
	pIServerUserItem->SetClientReady(false);
	pIServerUserItem->SetUserStatus(US_SIT, m_wTableID, wChairID);


	//���ñ���
	m_wUserCount = GetSitUserCount();

	/* ��������Ϊ1�������ø��û�Ϊ���� */
	if ( 1 == GetSitUserCount() )
	{
		//״̬����
		bool bTableLocked = IsTableLocked();

		//���÷���
		m_dwTableOwner = pIServerUserItem->GetUserID();
		lstrcpyn(m_szEnterPassword, pUserRule->szPassword, CountArray(m_szEnterPassword));

		//��������״̬��Ⱥ������״̬
		if (bTableLocked != IsTableLocked()) 
			SendTableStatus();
	}

	///* ��ҪУ���û�GPS���룬����̫�磬�ͻ��˳�����δ��ʼ��*/
	//if ( bCheckUserGPS )
	//{
	//	// У���û�GPS����
	//	//CheckUserDistance();

	//	//У��IP��ַ
	//	CheckUserIpAddress();
	//}

	/* ֪ͨ����Ϸ, ����Ϸ�����û�״̬ */
	if (NULL != m_pITableFrameSink)
	{
		//�û����£�����Ϸ�����û�����״̬
		m_pITableFrameSink->OnActionUserSitDown(wChairID, pIServerUserItem, false);
	}
	else
	{
		m_pITableFrameSink->OnActionUserSitDownT(wChairID, pIServerUserItem, false);
	}	

	/* ֪ͨ���ݿ�, ������� */
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

	g_AttemperEngineSink->ClubPlayerSitDown(GetPassword(), pIServerUserItem->GetUserID(),static_cast<BYTE>(wChairID), byClubOrHalGold);

	/* ֪ͨ���ݿ�, ���������� */
	//tagTableRule *pTableCfg = (tagTableRule*)GetCustomRule();
	if(pTableCfg != NULL)
	{
		BYTE byPlayerNum = pTableCfg->PlayerCount;
		if(GetSitUserCount() == byPlayerNum)
		{
			g_AttemperEngineSink->ClubTableMax(GetPassword(), byClubOrHalGold);
		}
	}
	
	return true;
}

//����״̬
bool CTableFrame::SendTableStatus()
{
	//��������
	CMD_GR_TableStatus TableStatus;
	ZeroMemory(&TableStatus, sizeof(TableStatus));

	//��������
	TableStatus.wTableID = m_wTableID;
	TableStatus.TableStatus.cbTableLock = IsTableLocked()?TRUE:FALSE;
	TableStatus.TableStatus.cbPlayStatus = IsTableStarted()?TRUE:FALSE;

	//��������
	g_GameCtrl->SendData(BG_COMPUTER, MDM_GR_STATUS, CMD_GR_TABLE_STATUS, &TableStatus, sizeof(TableStatus));

	//�ֻ�����

	return true;
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

	return true;
}

//��ʼЧ��
bool CTableFrame::EfficacyStartGame(WORD wReadyChairID)
{
	TCHAR szString[512]=TEXT("");

	//״̬�ж�
	if (m_bGameStarted==true) 
	{
		return false;
	}

	//ģʽ����
	if (m_cbStartMode==START_MODE_TIME_CONTROL) 
	{
		return false;
	}
	
	if (m_cbStartMode==START_MODE_MASTER_CONTROL)
	{
		return false;
	}
	
	//����ģʽ����Ϸ������������ʼ
	if (((tagTableRule*)GetCustomRule())->GameMode == TABLE_MODE_DW)
	{
		return true;
	}  

	//׼������
	WORD wReadyUserCount=0;
	for (WORD i=0;i<m_wChairCount && i<MAX_CHAIR;i++)
	{
		//��ȡ�û�
		CPlayer * pITableUserItem=GetTableUserItem(i);
		if (pITableUserItem==NULL) continue;

		//�û�ͳ��
		if (pITableUserItem!=NULL)
		{
			//״̬�ж� -- ���˵����򲻿�ʼ��Ϸ
			if (pITableUserItem->IsClientReady()==false)
			{
				return false;
			}

			//׼��״̬����� ����
			if (pITableUserItem->GetUserStatus()==US_READY)
			{
				wReadyUserCount++;
			}
		}
	}

	//��ʼ����
	switch (m_cbStartMode)
	{
	case START_MODE_ALL_READY:			//����׼��
		{
			//��Ŀ�ж�
			if (wReadyUserCount>=1L)
			{
				return true;
			}

			return false;
		}
	case START_MODE_FULL_READY:			//���˿�ʼ
		{
			//�����ж�
			if (wReadyUserCount==m_wChairCount)
			{
				return true;
			}

			return false;
		}
	case START_MODE_PAIR_READY:			//��Կ�ʼ
		{
			//��Ŀ�ж�
			if (wReadyUserCount==m_wChairCount) return true;
			if ((wReadyUserCount<2L)||(wReadyUserCount%2)!=0) return false;

			//λ���ж�
			for (WORD i=0;i<m_wChairCount && i<MAX_CHAIR/2;i++)
			{
				//��ȡ�û�
				CPlayer * pICurrentUserItem=GetTableUserItem(i);
				CPlayer * pITowardsUserItem=GetTableUserItem(i+m_wChairCount/2);

				//λ�ù���
				if ((pICurrentUserItem==NULL)&&(pITowardsUserItem!=NULL)) return false;
				if ((pICurrentUserItem!=NULL)&&(pITowardsUserItem==NULL)) return false;
			}

			return true;
		}
	default:
		{
			return false;
		}
	}

	return false;
}

//��ַЧ��
bool CTableFrame::EfficacyIPAddress(CPlayer * pIServerUserItem)
{
	//����Ա��������
	if(pIServerUserItem->GetMasterOrder()!=0) return true;

	//�����ж�
//	if (CServerRule::IsForfendGameRule(m_pGameServiceOption->dwServerRule)==true) return true;

	//��ַЧ��
	const tagUserRule * pUserRule=pIServerUserItem->GetUserRule(),*pTableUserRule=NULL;
	bool bCheckSameIP=pUserRule->bLimitSameIP;
	for (WORD i=0;i<m_wChairCount && i<MAX_CHAIR && i <MAX_CHAIR;i++)
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
		for (WORD i=0;i<m_wChairCount && i<MAX_CHAIR;i++)
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
		for (WORD i=0;i<m_wChairCount && i<MAX_CHAIR-1;i++)
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
	for (WORD i =0; i<m_wChairCount && i<MAX_CHAIR; i++)
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
	for (WORD i=0; i<m_wChairCount && i<MAX_CHAIR; i++)
	{
		//�û�IP��ַ
		CPlayer *pCheckUserItem = GetTableUserItem(i);
		if (NULL == pCheckUserItem) continue;
		DWORD dwUserIP = pCheckUserItem->GetClientAddr();

		//�������û��Ƚ�IP
		for (int j = i+1; j < m_wChairCount && i<MAX_CHAIR; j++)
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
	for (WORD k =0; k<m_wChairCount && k<MAX_CHAIR; k++)
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
	for (WORD i=0;i<m_wChairCount && i<MAX_CHAIR;i++)
	{
		//��ȡ�û�
		CPlayer * pITableUserItem=GetTableUserItem(i);

		//����Ч��
		if (pITableUserItem!=NULL)
		{
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
		}
	}

	return true;
}

bool CTableFrame::SetMatchInterface(IUnknownEx * pIUnknownEx)
{
	return NULL;
}

//������
bool CTableFrame::CheckDistribute()
{
//	//������
//	if(CServerRule::IsAllowAvertCheatMode(m_pGameServiceOption->dwServerRule))
//	{
//		//����״��
//		tagTableUserInfo TableUserInfo;
//		WORD wUserSitCount=GetTableUserInfo(TableUserInfo);
//
//		//�û�����
//		if(wUserSitCount < TableUserInfo.wMinUserCount)
//		{
//			return true;
//		}
//	}
//
	return false;
}

//��Ϸ��¼
void CTableFrame::RecordGameScore(bool bDrawStarted, DWORD dwStartGameTime)
{
	//��ʱ����Ϸֻ��startgame����һ�ε��¸�����Ϸ�޷�д��Ϸ��¼�����ע��
	//if (bDrawStarted==true)
	{
		//д���¼
		if (true )//CServerRule::IsRecordGameScore(m_pGameServiceOption->dwServerRule)==true)
		{
			//��������
			DBR_GR_GameScoreRecord GameScoreRecord;
			ZeroMemory(&GameScoreRecord,sizeof(GameScoreRecord));

			//���ñ���
			GameScoreRecord.wTableID=m_wTableID;
			GameScoreRecord.dwPlayTimeCount=(bDrawStarted==true)?(DWORD)time(NULL)-m_dwDrawStartTime:0;

			//��ӿ�����yang
			GameScoreRecord.wChangeStockScore = g_AttemperEngineSink->GetChangeStockScore();


			//��Ϸʱ��
			if(dwStartGameTime!=INVALID_DWORD)
			{
				CTime startTime(dwStartGameTime);
				startTime.GetAsSystemTime(GameScoreRecord.SystemTimeStart);
			}
			else
			{
				GameScoreRecord.SystemTimeStart=m_SystemTimeStart;
			}
			GetLocalTime(&GameScoreRecord.SystemTimeConclude);

			//�û�����
			for (INT_PTR i=0;i<m_GameScoreRecordActive.GetCount();i++)
			{
				//��ȡ����
				ASSERT(m_GameScoreRecordActive[i]!=NULL);
				tagGameScoreRecord * pGameScoreRecord=m_GameScoreRecordActive[i];

				//�û���Ŀ
				if (pGameScoreRecord->cbAndroid==FALSE)
				{
					GameScoreRecord.wUserCount++;
				}
				else
				{
					GameScoreRecord.wAndroidCount++;
				}

				//����ͳ��
				GameScoreRecord.dwUserMemal+=pGameScoreRecord->dwUserMemal;

				//ͳ����Ϣ
				if (pGameScoreRecord->cbAndroid==FALSE)
				{
					GameScoreRecord.lWasteCount-=(pGameScoreRecord->lScore+pGameScoreRecord->lRevenue);
					GameScoreRecord.lRevenueCount+=pGameScoreRecord->lRevenue;
				}

				//�ɼ���Ϣ
				if (GameScoreRecord.wRecordCount<CountArray(GameScoreRecord.GameScoreRecord))
				{
					WORD wIndex=GameScoreRecord.wRecordCount++;
					CopyMemory(&GameScoreRecord.GameScoreRecord[wIndex],pGameScoreRecord,sizeof(tagGameScoreRecord));
				}
			}

			//Ͷ������
			if(GameScoreRecord.wUserCount > 0)
			{
				WORD wHeadSize=sizeof(GameScoreRecord)-sizeof(GameScoreRecord.GameScoreRecord);
				WORD wDataSize=sizeof(GameScoreRecord.GameScoreRecord[0])*GameScoreRecord.wRecordCount;
				g_GameCtrl->PostDataBaseRequest(DBR_GR_GAME_SCORE_RECORD,0,&GameScoreRecord,wHeadSize+wDataSize);
			}
		}

		//�����¼
		if (m_GameScoreRecordActive.GetCount()>0L)
		{
			m_GameScoreRecordBuffer.Append(m_GameScoreRecordActive);
			m_GameScoreRecordActive.RemoveAll();
		}
	}

}

bool CTableFrame::SendAndroidUserData( WORD wChairID, WORD wSubCmdID )
{
	//�û�Ⱥ��
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<m_wChairCount && i<MAX_CHAIR;i++)
		{
			//��ȡ�û�
			CPlayer * pIServerUserItem=GetTableUserItem(i);
			if (pIServerUserItem==NULL) continue;
			if (pIServerUserItem->IsAndroidUser())
			{
				//Ч��״̬
				ASSERT(pIServerUserItem->IsClientReady()==true);
				if (pIServerUserItem->IsClientReady()==false) continue;

				//��������
				g_GameCtrl->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);

			}
		}

		return true;

	}
	else
	{
		//��ȡ�û�
		CPlayer * pIServerUserItem=GetTableUserItem(wChairID);
		if (pIServerUserItem==NULL) return false;

		if (pIServerUserItem->IsAndroidUser())
		{
			//Ч��״̬
			ASSERT(pIServerUserItem->IsClientReady()==true);
			if (pIServerUserItem->IsClientReady()==false) return false;

			//��������
			g_GameCtrl->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);
			return true;
		}

	}
	return false;
}

bool CTableFrame::SendAndroidUserData( WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize,WORD wMainCmdID/*=MDM_GF_GAME*/ )
{
	//�û�Ⱥ��
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<m_wChairCount && i<MAX_CHAIR;i++)
		{
			//��ȡ�û�
			CPlayer * pIServerUserItem=GetTableUserItem(i);
			if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

			if (pIServerUserItem->IsAndroidUser())
			{
				//��������
				g_GameCtrl->SendData(pIServerUserItem,wMainCmdID,wSubCmdID,pData,wDataSize);
			}
		}
		return true;
	}
	else
	{
		//��ȡ�û�
		CPlayer * pIServerUserItem=GetTableUserItem(wChairID);
		if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) return false;

		if (pIServerUserItem->IsAndroidUser())
		{
			//��������
			g_GameCtrl->SendData(pIServerUserItem,wMainCmdID,wSubCmdID,pData,wDataSize);

			return true;
		}

	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////
bool CTableFrame::AddVirtualUser(CPlayer* pIServerUserItem)
{
	//��������
	WORD wChairID= GetNullChairID();

	//�����û�
	if (wChairID!=INVALID_CHAIR)
	{
		pIServerUserItem->SetClientReady(true);
		pIServerUserItem->SetUserStatus(US_READY,m_wTableID,wChairID);

		if(PerformSitDownAction(wChairID,pIServerUserItem)==false)
		{
			SendRequestFailure(pIServerUserItem,TEXT("����Ų���!"),REQUEST_FAILURE_PASSWORD);
			return true;
		}	
	}
	else
	{
		SendRequestFailure(pIServerUserItem,TEXT("��������,�������������!"),REQUEST_FAILURE_NORMAL);
		return true;
	}

	return true;
}

//����ȦȺ����ɢ����
void CTableFrame::OnEventClubDismissRoom()
{
	m_pITableFrameSink->OnEventGameConclude(INVALID_CHAIR, NULL, GER_DISMISS);
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