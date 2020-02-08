#include "stdafx.h"
#include "MatchItem.h"
#include "TableManager.h"
#include "GameCtrl.h"
#include "MatchManager.h"
#include <algorithm>

//��ʱ������
#define	IDI_MATCH_TYPE_TIME			(IDI_MATCH_MODULE_START+1)			//��ʱ����

CMatchItem::CMatchItem(MATCH_CONFIG config)
{
	m_Start = false;
	m_Stage = 0;

	//�½�һ���ȴ�����
	m_wait_room = new CMatchRoom(this);

	CopyMemory(&m_config, &config, sizeof(MATCH_CONFIG));
}


CMatchItem::~CMatchItem(void)
{
}

//��ʼ��
void CMatchItem::Init()
{
	//1�������˶�ʱ��

	//2����ʱ������ʱ��
	if (m_config.wStartType == MATCH_START_TYPE_TIME)
	{
		g_GameCtrl->SetTimer(IDI_MATCH_TYPE_TIME, m_config.dwStartTime - time(0), 1, 0);
	}
}

//��ұ���
bool CMatchItem::On_User_Apply(CPlayer *player)
{
	if (player == NULL)
		return false;

	m_Apply_Player.push_back(player);

	//��Ҽ���ȴ�����
	m_wait_room->PlayerEnterTable(player);

	return true;
}

//���ȡ������
bool CMatchItem::On_User_UnApply(CPlayer *player)
{
	if (player == NULL)
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

	return false;
}

//��������
bool CMatchItem::On_Apply_End()
{
	//��������У��
	WORD wApplyCount = m_Apply_Player.size();
	WORD wLeastCount = m_config.dwLowestPlayer;
	if (wApplyCount < wLeastCount)
	{
		//�������� ����������
		On_Match_Cancel();
		return false;
	}

	//�߳���������
	WORD wFreeCount = wApplyCount % 3;			//���޸�
	//1�� �����߳�Robot
	//û��Robot�߳���ʵ���

	//��ʼ����
	On_Match_Start();

	return true;
}

//����ȡ��
bool CMatchItem::On_Match_Cancel()
{
	//������Ϣ
	
	//������Ϣ
	SendDataAllPlayer(CMD_GC_MATCH_CANCEL, NULL, 0);

	//����̭����

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
	//������һ�ֻ���	//�Ƿ��б�Ҫ
	m_Last_Score.clear();
	for (auto Item : m_Cur_Ranking)
	{
		CPlayer *player = Item.user;
		m_Last_Score.insert(std::make_pair(player->GetUserID(), Item.score));
	}

	//������䷿��
	random_shuffle(m_Apply_Player.begin(), m_Apply_Player.end());

	//��ʼ�����з������Ϸ״̬  --�������������˲ſ�ʼ��һ�׶�
	m_Room_state.clear();
	m_Room_state.resize(m_Apply_Player.size() / 3);

	CMatchRoom* room = NULL;
	int index = 0;
	for (auto item : m_Apply_Player)
	{
		//��������
		if (NULL == room)
		{
			room = CTableManager::CreateMatchRoom(this);
			room->set_index(index++);
		}

		//��Ҽ��뵽Table��
		int ret = room->PlayerSitTable(item);
        if (ret != 0)
        {
            return false;
        }

		//����������������ʼ
		if (room->IsRoomFull())
		{
			// ֪ͨ������ʼ
			room->SendTableData(INVALID_CHAIR, CMD_GC_MATCH_START, NULL, 0, MDM_GR_MATCH);
		}

		//���ͱ�������Ϣ
		//SendMatchInfo(item, 1,room);

		//���׼��
		ret = room->PlayerReady(item);
		if (ret != 0)
        {
            return false;
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
	m_Room_state[room->get_index()] = true;

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

	//���÷������״̬
	if (!stage_end)
	{
		room->SetRoomState(wait_next);
	}

	//��ȡ�����������
	std::vector<player_info> ranking;
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
		std::vector<player_info> TaoTai_Player;
		for (int i = stage.wJinJi_Count; i < room->GetChairCount(); i++)
		{
			TaoTai_Player.push_back(ranking[i]);
		}
		On_Player_TaoTai(TaoTai_Player, room);

		//�������
		std::vector<player_info> JinJi_Player;
		for (int i = 0; i < stage.wJinJi_Count; i++)
		{
			JinJi_Player.push_back(ranking[i]);
		}
		On_Player_JinJi(JinJi_Player, room);
	}

	//��������������
	if (stage_end)
		On_Stage_End(room);

	return true;
}

//��̭���
bool CMatchItem::On_Player_TaoTai(std::vector<player_info> TaoTai_player, CMatchRoom *room)
{
	//���������̭
	for (auto item : TaoTai_player)
	{
		g_GameCtrl->SendData(item.user, MDM_GR_MATCH, CMD_GC_MATCH_RESULT, NULL, 0);
	}

	//���б���ɾ�����
	for (auto ite = m_Apply_Player.begin(); ite != m_Apply_Player.end(); ite++)
	{
		CPlayer *player = *ite;
		for (auto taotai : TaoTai_player)
		{
			if (player && player == taotai.user)
				ite = m_Apply_Player.erase(ite);
		}
	}

	return true;
}

//�������
bool CMatchItem::On_Player_JinJi(std::vector<player_info> JinJi_player, CMatchRoom *room)
{
	//������ҽ�����Ϣ
	for (auto jinji : JinJi_player)
	{
		g_GameCtrl->SendData(jinji.user, MDM_GR_MATCH, CMD_GC_MATCH_RESULT, NULL, 0);
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
	}

	//��ʼ��һ��
	m_Stage ++;
	room->SetStageTimer();

	return true;
}

//��������
bool CMatchItem::Update_Ranking(CMatchRoom *room)
{
	//��ȡ�����Ϣ //С�ַֺ��ܷ� ��SubС�ֽ���ʱ����frame
	std::vector<CPlayer *> player_list = room->GetPlayer_list();
	std::vector<int> SingleScore;
	std::vector<int> TotalScore;

	//���������Ϣ
	for (int i = 0; i < room->GetChairCount(); i++)
	{
		for (auto player : m_Cur_Ranking)
		{
			if (player.user == player_list[i])
			{
				player.ju_score = SingleScore[i];
				player.score = TotalScore[i];
				player.room_id = room->GetTableID();
				player.seat = room->GetPlayerChair(player_list[i]);
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

	//��Client�����������
	Send_Ranking();

	return true;
}

//��������
void CMatchItem::Send_Ranking()
{
	STR_CMD_GC_MATCH_RANKING  ranking;
	ZeroMemory(&ranking, sizeof(STR_CMD_GC_MATCH_RANKING));

	WORD wRanking = 0;
	ranking.wPlayerCount = m_Cur_Ranking.size();
	for (int i = 0; i < m_Cur_Ranking.size(); i++)
	{
		ranking.player[i].wRanking = ++wRanking;
		ranking.player[i].llScore = m_Cur_Ranking[i].score;
		//ranking.player[i].szName = m_Cur_Ranking[i].user->GetNickName();
	}

	for (auto item : m_Apply_Player)
	{
		g_GameCtrl->SendData(item, MDM_GR_MATCH, CMD_GC_MATCH_RANKING, &ranking, sizeof(STR_CMD_GC_MATCH_RANKING));
	}
}

//��������
bool CMatchItem::On_Match_End(std::vector<player_info> player)
{
	//��ҽ���

	//ս��
	return true;
}

//Ⱥ������
void CMatchItem::SendDataAllPlayer(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	for (auto player : m_Apply_Player)
	{
		if (!player) continue;

		g_GameCtrl->SendData(player, MDM_GR_MATCH, wSubCmdID, pData, wDataSize);
	}
}