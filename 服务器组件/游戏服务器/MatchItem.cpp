#include "stdafx.h"
#include "MatchItem.h"
#include "TableManager.h"
#include "GameCtrl.h"
#include "MatchManager.h"
#include <algorithm>
#include <iostream>

//��ʱ������
#define	IDI_MATCH_TYPE_TIME			(IDI_MATCH_MODULE_START)			//��ʱ����

CMatchItem::CMatchItem(MATCH_CONFIG config)
{
	m_Start = false;
	m_Stage = 0;

	m_Apply_Player.clear();
	m_Cur_Ranking.clear();
	m_room_player.clear();

	CopyMemory(&m_config, &config, sizeof(MATCH_CONFIG));

	//�½�һ���ȴ�����
	m_wait_room = CTableManager::CreateMatchRoom(this, m_Stage);
}


CMatchItem::~CMatchItem(void)
{
	CTableManager::DeleteTable(m_wait_room);
}

//��ʼ��
void CMatchItem::Init()
{
	//��ʱ������ʱ��
	if (m_config.wStartType == MATCH_START_TYPE_TIME)
	{
		g_GameCtrl->SetTimer(IDI_MATCH_TYPE_TIME + m_config.wMatchID, m_config.dwStartTime - time(0), 1, 0);
	}
}

//��ұ���
bool CMatchItem::On_User_Apply(CPlayer *player)
{
	//���У��
	if (player == NULL)
		return false;

	if (US_FREE != player->GetUserStatus())
		return false;

	//��ʼУ��
	if (m_Start)
		return false;

	CLog::Log(log_debug, "�������ID: %d", player->GetUserID());

	//��Ҽ���ȴ�����
	int ret = m_wait_room->PlayerEnterTable(player);
	if (ret != 0)
		return false;

	m_Apply_Player.push_back(player);

	//��������
	if ((m_config.wStartType == MATCH_START_TYPE_COUNT) &&
		(m_Apply_Player.size() == m_config.dwLowestPlayer))
	{
		On_Apply_End();
	}

	return true;
}

//���ȡ������
bool CMatchItem::On_User_UnApply(CPlayer *player)
{
	if (player == NULL)
		return false;

	if (m_Start)
		return false;

	for (auto it = m_Apply_Player.begin(); it != m_Apply_Player.end(); it++)
	{
		CPlayer *m_player = *it;
		if (m_player && m_player == player)
		{
			m_Apply_Player.erase(it);
			return true;
		}
	}

	//����뿪�ȴ�����
	m_wait_room->PlayerLeaveTable(player);

	return false;
}

//��������
bool CMatchItem::On_Apply_End()
{
	//��������У��
	DWORD wApplyCount = m_Apply_Player.size();
	DWORD wLeastCount = m_config.dwLowestPlayer;
	if (wApplyCount < wLeastCount)
	{
		//�������� ����������
		CLog::Log(log_debug, "��������: %d����������: %d, ��������, ȡ��������", wApplyCount, wLeastCount);
		On_Match_Cancel();
		return false;
	}

	//�߳���������
	DWORD wFreeCount = wApplyCount % 3;			//���޸�
	//1�� �����߳�Robot
	DWORD dwKick_Robot_count = 0;
	for (int i = 0; i < wFreeCount; i++)
	{
		for (auto ite = m_Apply_Player.begin(); ite != m_Apply_Player.end(); ite++)
		{
			CPlayer *player = *ite;
			if (player && player->IsAndroidUser())
			{
				m_wait_room->PlayerLeaveTable(player);
				ite = m_Apply_Player.erase(ite);
				dwKick_Robot_count++;
				break;
			}
		}
	}
	//2�� û��Robot�߳���ʵ���
	for (int i = 0; i < wFreeCount - dwKick_Robot_count; i++)
	{
		for (auto ite = m_Apply_Player.rbegin(); ite != m_Apply_Player.rend(); ite++)
		{
			CPlayer *player = *ite;

			//������Ϣ
			STR_CMD_GC_MATCH_CANCEL cancel;
			ZeroMemory(&cancel, sizeof(STR_CMD_GC_MATCH_CANCEL));
			swprintf(cancel.szDescribe, 128, L"%S", "�����ˣ��������ˣ�");
			SendDataAllPlayer(CMD_GC_MATCH_CANCEL, &cancel, sizeof(STR_CMD_GC_MATCH_CANCEL));

			//����ɾ��������
			m_wait_room->PlayerLeaveTable(player);
			ite = std::list<CPlayer*>::reverse_iterator(m_Apply_Player.erase((++ite).base()));
			break;
		}
	}

	//��ʼ����
	On_Match_Start();

	return true;
}

//����ȡ��
bool CMatchItem::On_Match_Cancel()
{
	//����״̬
	m_Start = true;

	//������Ϣ
	STR_CMD_GC_MATCH_CANCEL cancel;
	ZeroMemory(&cancel, sizeof(STR_CMD_GC_MATCH_CANCEL));
	swprintf(cancel.szDescribe, 128, L"%S", "��������̫�٣�����ȡ������");

	//������Ϣ
	SendDataAllPlayer(CMD_GC_MATCH_CANCEL, &cancel, sizeof(STR_CMD_GC_MATCH_CANCEL));

	//����뿪�ȴ�����
	for (auto item : m_Apply_Player)
	{
		if (item)
			m_wait_room->PlayerLeaveTable(item);
	}

	m_Apply_Player.clear();

	//ȡ������
	CMatchManager::On_Match_Cancel(this);

	return true;
}

//������ʼ
bool CMatchItem::On_Match_Start()
{
	//������һ������
	CMatchManager::On_Match_Start(this);
	//���ñ�����ʼ
	m_Start = true;

	//�������ڱ�������ҵ������Ϣ
	for (auto user : m_Apply_Player)
	{
		player_info item;
		item.user = user;
		m_Cur_Ranking.push_back(item);
	}

	//���ñ���������������
	m_config.stage[0].wPlayerCount = m_Apply_Player.size();

	//����д�����ݿ�

	//��ʼ����
	On_Stage_Start();

	return true;
}

//��ʼ��ǰ�׶α���
bool CMatchItem::On_Stage_Start()
{
	//������䷿��
	//random_shuffle(m_Apply_Player.begin(), m_Apply_Player.end());

	//��ʼ�����з������Ϸ״̬  --�������������˲ſ�ʼ��һ�׶�
	m_Room_state.clear();
	m_Room_state.resize(m_Apply_Player.size() / 3);
	CLog::Log(log_debug, "�׶�����: %d", m_Apply_Player.size());
	CMatchRoom* room = NULL;
	int index = 0;
	for (auto item : m_Apply_Player)
	{
		//��������
		if (NULL == room)
		{
			CLog::Log(log_debug, "��������");
			room = CTableManager::CreateMatchRoom(this, m_Stage);
			room->set_index(index++);
		}

		//����뿪�ȴ�����
		m_wait_room->PlayerLeaveTable(item);

		//��Ҽ��뵽Table��
		int ret = room->PlayerSitTable(item);
        if (ret != 0)
        {
            return false;
        }

		//����������������ʼ
		if (room->IsRoomFull())
		{
			CLog::Log(log_debug, "������ʼ������");
			// ֪ͨ������ʼ
			room->SendTableData(INVALID_CHAIR, CMD_GC_MATCH_START, NULL, 0, MDM_GR_MATCH);

			//����׼����ʱ��
			room->SetPlayerAutoReady();
		}

		//��������
		if (room->IsRoomFull())
		{
			room = NULL;
		}
	}

	return true;
}

//һ�ֽ�������
bool CMatchItem::On_Room_End(CMatchRoom *room)
{
	//����Item����״̬
	int index = 0;
	for (auto ite = m_Room_state.begin(); ite != m_Room_state.end(); ite++)
	{
		if (index == room->get_index())
			*ite = true;
		index ++;
	}

	//�Ƿ�Ҫ�ȴ����������
	bool stage_end = true;
	for (auto state : m_Room_state)
	{
		if (!state)
		{
			stage_end = false;
			break;
		}
	}
	CLog::Log(log_debug, "�ȴ������𣿣��� %d", !stage_end);
	//���÷������״̬
	if (!stage_end)
	{
		room->SetRoomState(wait_next);
	}

	//��ȡ�����������
	std::list<player_info> ranking;
	for (auto item : m_Cur_Ranking)
	{
		if (item.room_id == room->GetTableID())
			ranking.push_back(item);
	}
	m_room_player[room->GetTableID()] = ranking;

	//���ݹ��������
	auto stage = m_config.stage[m_Stage];
	bool Jue_Sai = (m_Stage == (m_config.wStageSize - 1));
	if (stage.wJinJi_Rule == 1 && !Jue_Sai)	//ͬ���Ƚ� --�����ŵ��׶ν�����������
	{
		//��̭���
		std::list<player_info> TaoTai_Player;
		int flag = 0;	//��vector���޽�
		for (auto player : ranking)
		{
			if (flag >= stage.wJinJi_Count)
				TaoTai_Player.push_back(player);
			flag ++;
		}
		On_Player_TaoTai(TaoTai_Player);

		//�������
		std::list<player_info> JinJi_Player;
		flag = 0;
		for (auto player : ranking)
		{
			if (flag < stage.wJinJi_Count)
				JinJi_Player.push_back(player);
			flag ++;
		}
		On_Player_JinJi(JinJi_Player);
	}
	else if (stage.wJinJi_Rule == 2 && !Jue_Sai && stage_end)
	{
		std::list<player_info> TaoTai_Player;
		std::list<player_info> JinJi_Player;
		for (int i = 0; i < m_Cur_Ranking.size(); i++)
		{
			if (i < stage.wJinJi_Count)
			{
				JinJi_Player.push_back(m_Cur_Ranking[i]);
			}
			else
			{
				TaoTai_Player.push_back(m_Cur_Ranking[i]);
				m_wait_room->PlayerLeaveTable(m_Cur_Ranking[i].user);
			}
		}
		On_Player_JinJi(JinJi_Player);
		On_Player_TaoTai(TaoTai_Player);
	}
	else if (stage.wJinJi_Rule == 2 && !Jue_Sai && !stage_end)
	{
		for (auto item : ranking)
		{
			//���͵ȴ�����
			Updata_WaitCount(item.user);
			//����ȴ�����
			m_wait_room->PlayerEnterTable(item.user);
		}
	}

	//��������������
	if (stage_end)
		On_Stage_End(room);

	return true;
}

//��̭���
bool CMatchItem::On_Player_TaoTai(std::list<player_info> TaoTai_player)
{
	//����
	STR_CMD_GC_MATCH_RESULT_TAOTAI taotai;

	//���������̭
	for (auto item : TaoTai_player)
	{
		//����뿪�ȴ�����
		m_wait_room->PlayerLeaveTable(item.user);

		ZeroMemory(&taotai, sizeof(STR_CMD_GC_MATCH_RESULT_TAOTAI));

		swprintf(taotai.szStageName, 16, L"%S", m_config.stage[m_Stage].szName);
		taotai.wRanking = GetRanking(item.user);

		CLog::Log(log_debug, "��� :%d ��̭��", item.user->GetUserID());
		if (!item.user->IsAndroidUser())
			g_GameCtrl->SendData(item.user, MDM_GR_MATCH, CMD_GC_MATCH_RESULT_TAOTAI, NULL, 0);
	}

	//���б���ɾ�����
	for (auto taotai : TaoTai_player)
	{
		for (auto ite = m_Apply_Player.begin(); ite != m_Apply_Player.end(); ite++)
		{
			if ((*ite) == taotai.user)
			{
				ite = m_Apply_Player.erase(ite);
				break;
			}
		}
	}

	CLog::Log(log_debug, "apply size: %d", m_Apply_Player.size());

	//�����б���ɾ�����
	for (auto taotai : TaoTai_player)
	{
		for (auto ite = m_Cur_Ranking.begin(); ite != m_Cur_Ranking.end(); ite++)
		{
			if ((*ite).user == taotai.user)
			{
				ite = m_Cur_Ranking.erase(ite);
				break;
			}
		}
	}
	CLog::Log(log_debug, "ranking size: %d", m_Cur_Ranking.size());

	return true;
}

//�������
bool CMatchItem::On_Player_JinJi(std::list<player_info> JinJi_player)
{
	//����
	STR_CMD_GC_MATCH_RESULT_JINJI msgjinji;

	WORD playing_count = 0;
	for (auto item : m_Room_state)
	{
		if (!item)
			playing_count ++;
	}

	//������ҽ�����Ϣ
	for (auto jinji : JinJi_player)
	{
		CLog::Log(log_debug, "��� :%d ������", jinji.user->GetUserID());

		ZeroMemory(&msgjinji, sizeof(STR_CMD_GC_MATCH_RESULT_JINJI));

		swprintf(msgjinji.szStageName, 16, L"%S", m_config.stage[m_Stage].szName);
		msgjinji.wWaitCount = playing_count;
		msgjinji.wRanking = GetRanking(jinji.user);

		if (!jinji.user->IsAndroidUser())
			g_GameCtrl->SendData(jinji.user, MDM_GR_MATCH, CMD_GC_MATCH_RESULT_JINJI, &msgjinji, sizeof(STR_CMD_GC_MATCH_RESULT_JINJI));
	}

	//��Ҽ���ȴ�����
	for (auto jinji : JinJi_player)
	{
		m_wait_room->PlayerEnterTable(jinji.user);
	}

	return true;
}

//һ�׶α�������
bool CMatchItem::On_Stage_End(CMatchRoom *room)
{
	//��������
	if (m_Stage == (m_config.wStageSize - 1))
	{
		//���
		auto ite = m_room_player.find(room->GetTableID());
		if (ite == m_room_player.end())
			return false;

		//��������
		CMatchManager::On_Match_End(this, ite->second);
		return true;
	}

	//��ʼ��һ��
	m_Stage ++;
	m_wait_room->SetStageTimer();

	return true;
}

//��������
bool CMatchItem::Update_Ranking(CMatchRoom *room)
{
	//��ȡ�����Ϣ //С�ַֺ��ܷ� ��SubС�ֽ���ʱ����frame
	std::vector<CPlayer *> player_list = room->GetPlayer_list();
	std::vector<SCORE> TotalScore = room->GetTotalScore();
	CLog::Log(log_debug, "����������");
	//���������Ϣ
	for (int i = 0; i < room->GetChairCount(); i++)
	{
		for (auto ite = m_Cur_Ranking.begin(); ite != m_Cur_Ranking.end(); ite++)
		{
			player_info *info = &(*ite);
			if (info->user == player_list[i])
			{
				info->score += TotalScore[i];
				info->room_id = room->GetTableID();
				break;
			}
		}
	}

	//m_Cur_Ranking����
	std::sort(m_Cur_Ranking.begin(), m_Cur_Ranking.end(), [this](const player_info& item_1, const player_info& item_2)
	{
		if (item_1.score == item_2.score)
		{
			auto order_1 = std::find_if(m_Apply_Player.begin(), m_Apply_Player.end(), [item_1](CPlayer* src)
			{
				return src == item_1.user;
			});

			auto order_2 = std::find_if(m_Apply_Player.begin(), m_Apply_Player.end(), [item_2](CPlayer* src)
			{
				return src == item_2.user;
			});

			return std::distance(order_1, order_2) > 0;
		}
		return item_1.score > item_2.score;
	});

	return true;
}

//��������������
void CMatchItem::Send_Ranking(CPlayer *player)
{
	DWORD	dwindex = 0;
	DWORD	wPacketSize = 0;
	BYTE	byBuffer[MAX_ASYNCHRONISM_DATA/10];
	STR_CMD_GC_MATCH_RANKING  *rank = NULL;
	for (auto ranking : m_Cur_Ranking)
	{
		if (wPacketSize + sizeof(STR_CMD_GC_MATCH_RANKING) > sizeof(byBuffer))
		{
			//Ⱥ��
			if (player == NULL)
				SendDataAllPlayer(CMD_GC_MATCH_RANKING, &ranking, sizeof(STR_CMD_GC_MATCH_RANKING));
			else
				g_GameCtrl->SendData(player, MDM_GR_MATCH, CMD_GC_MATCH_RANKING, byBuffer, wPacketSize);
		}

		rank = (STR_CMD_GC_MATCH_RANKING *)(byBuffer + wPacketSize);
		rank->llScore = ranking.score;
		rank->wRanking = ++dwindex;
		memcpy(rank->szName, ranking.user->GetNickName(), sizeof(TCHAR)*LEN_NICKNAME);

		wPacketSize += sizeof(STR_CMD_GC_MATCH_RANKING);
	}
	if (wPacketSize > 0)
	{
		//Ⱥ��
		if (player == NULL)
			SendDataAllPlayer(CMD_GC_MATCH_RANKING, byBuffer, wPacketSize);
		else if (!player->IsAndroidUser())
			g_GameCtrl->SendData(player, MDM_GR_MATCH, CMD_GC_MATCH_RANKING, byBuffer, wPacketSize);
	}
}

//�����Լ�����
void CMatchItem::Send_Self_Ranking(CPlayer *player)
{
	CMatchRoom *room = (CMatchRoom *)CTableManager::FindTableByTableID(player->GetTableID());
	if (room == NULL)
		return;

	STR_CMD_CG_MATCH_RANKING_MY rank;
	ZeroMemory(&rank, sizeof(STR_CMD_CG_MATCH_RANKING_MY));

	rank.dwAllUserCount = m_Apply_Player.size();
	rank.dwCurCount = room->GetCurGameRound();
	rank.dwAllCount = room->GetCustomRule()->GameCount;
	rank.dwRanking = GetRanking(player);
	memcpy(rank.szStageName, m_config.stage[m_Stage].szName, sizeof(TCHAR)*16);

	CLog::Log(log_debug, "all user count: %d", rank.dwAllUserCount);

	g_GameCtrl->SendData(player, MDM_GR_MATCH, CMD_GC_MATCH_RANKING_MY, &rank, sizeof(STR_CMD_CG_MATCH_RANKING_MY));
}

//��������
bool CMatchItem::On_Match_End(std::list<player_info> player)
{
	//��ҽ���
	CLog::Log(log_debug, "��������!!!");

	//ս��


	return true;
}

//Ⱥ������
void CMatchItem::SendDataAllPlayer(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	for (auto player : m_Apply_Player)
	{
		if (!player) continue;
		if (player->IsAndroidUser()) continue;

		g_GameCtrl->SendData(player, MDM_GR_MATCH, wSubCmdID, pData, wDataSize);
	}
}

//���͵ȴ�����
void CMatchItem::Updata_WaitCount(CPlayer *player)
{
	WORD playing_count = 0;
	for (auto item : m_Room_state)
	{
		if (!item)
			playing_count ++;
	}

	STR_CMD_GC_MATCH_WAIT_COUNT wait;
	wait.wait_count = playing_count;

	if (!player->IsAndroidUser())
		g_GameCtrl->SendData(player, MDM_GR_MATCH, CMD_GC_MATCH_WAIT_COUNT, &wait, sizeof(STR_CMD_GC_MATCH_WAIT_COUNT));
}

//�������Ƿ�������
bool CMatchItem::IsRobotFull()
{
	WORD robot_count = 0;
	for (auto player : m_Apply_Player)
	{
		if (player && player->IsAndroidUser())
			robot_count++;
	}

	return (robot_count == m_config.dwRobotCount);
}

//��ȡ�������
WORD CMatchItem::GetRanking(CPlayer *player)
{
	WORD rank = 0;
	for (int i = 0; i < m_Cur_Ranking.size(); i++)
	{
		if (m_Cur_Ranking[i].user == player)
		{
			rank = i+1;
			return rank;
		}
	}

	return 0;
}