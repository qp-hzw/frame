#ifndef _MATCH_ITEM_H_
#define _MATCH_ITEM_H_

#include "Stdafx.h"
#include "Player.h"
#include "MatchRoom.h"
#include <list>
#include <map>
#include <vector>

struct player_info
{
    CPlayer* user;
    int score;
    int ju_score;
    int room_id;
    int seat;

    player_info()
    {
        user = NULL;
        score = 0;
        ju_score = 0;
        room_id = 0;
        seat = -1;
    }
};

class CMatchItem
{
private:
	MATCH_CONFIG				m_config;			//��������
	CMatchRoom*					m_wait_room;		//�ȴ�����

private:
	bool						m_Start;			//������ʼ��ʶ
	std::vector<bool>			m_Room_state;		//��ǰ�������з������Ϸ״̬
	WORD						m_Stage;			//��ǰ�׶�

private:
	std::vector<CPlayer *>		m_Apply_Player;	//���������
	std::vector<player_info>	m_Cur_Ranking;	//���ڱ������������
	std::map<int, int>			m_Last_Score;	//��һ�ֻ���
	std::map<int, std::vector<player_info>>	m_room_player;	//�ڷ���������  tableID ---> playerinfo

	//���캯��
public:
	CMatchItem(MATCH_CONFIG config);
	~CMatchItem(void);

	//��������
public:
	//��ұ���
	bool On_User_Apply(CPlayer *player);
	//���ȡ������
	bool On_User_UnApply(CPlayer *player);
	//��������
	bool On_Apply_End();
	//����ȡ��
	bool On_Match_Cancel();
	//������ʼ
	bool On_Match_Start();
	//��ʼ��ǰ�׶α���
	bool On_Stage_Start();
	//һ�׶α�������
	bool On_Stage_End(CMatchRoom *room);
	//һ�ֽ�������
	bool On_Room_End(CMatchRoom *room);
	//��̭���
	bool On_Player_TaoTai(std::vector<player_info> TaoTai_player, CMatchRoom *room);
	//�������
	bool On_Player_JinJi(std::vector<player_info> JinJi_player, CMatchRoom *room);
	//��������
	bool On_Match_End(std::vector<player_info> player);

	//��������
	bool Update_Ranking(CMatchRoom *room);
	//��������
	void Send_Ranking();


	//���ܺ���
public:
	//������ʼ��
	void Init();

	//��ȡMatchID
	WORD GetMatchID() {	return m_config.wMatchID; }
	//��ȡ��������
	MATCH_CONFIG GetConfig() { return m_config; }

	//Ⱥ������
	void SendDataAllPlayer(WORD wSubCmdID, VOID * pData, WORD wDataSize);
};

#endif