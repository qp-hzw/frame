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
    int room_id;
    int seat;
	TCHAR szname[LEN_NICKNAME];

    player_info()
    {
        user = NULL;
        score = 0;
        room_id = 0;
        seat = -1;
		ZeroMemory(&szname, sizeof(LEN_NICKNAME));
    }
};

class CMatchItem
{
private:
	MATCH_CONFIG				m_config;			//��������
	CMatchRoom*					m_wait_room;		//�ȴ�����

private:
	bool						m_Start;			//������ʼ��ʶ
	std::list<bool>				m_Room_state;		//��ǰ�������з������Ϸ״̬
	WORD						m_Stage;			//��ǰ�׶�

private:
	std::list<CPlayer *>		m_Apply_Player;	//���������
	std::vector<player_info>	m_Cur_Ranking;	//���ڱ������������
	std::map<int, std::list<player_info>>	m_room_player;	//�ڷ���������  tableID ---> playerinfo

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
	bool On_Player_TaoTai(std::list<player_info> TaoTai_player);
	//�������
	bool On_Player_JinJi(std::list<player_info> JinJi_player);
	//��������
	bool On_Match_End(std::list<player_info> player);

	//��������������
	bool Update_Ranking(CMatchRoom *room);
	//��������������
	void Send_Ranking(CPlayer *player = NULL);
	//�����Լ�����
	void Send_Self_Ranking(CPlayer *player);
	//���͵ȴ�����
	void Updata_WaitCount(CPlayer *player);


	//���ܺ���
public:
	//������ʼ��
	void Init();

	//��ȡMatchID
	DWORD GetMatchID() {	return m_config.wMatchID; }
	//��ȡ��������
	MATCH_CONFIG GetConfig() { return m_config; }

	//�ѱ�������
	WORD GetApplyCount() { return m_Apply_Player.size(); }
	//�������Ƿ�������
	bool IsRobotFull();
	//�����Ƿ��ѿ�ʼ
	bool IsMatchStart() { return m_Start; }

	//��ȡ�������
	WORD GetRanking(CPlayer *player);

	//Ⱥ������
	void SendDataAllPlayer(WORD wSubCmdID, VOID * pData, WORD wDataSize);
};

#endif