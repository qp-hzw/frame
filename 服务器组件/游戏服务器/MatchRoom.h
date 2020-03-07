#ifndef _MATCH_ROOM_H_
#define _MATCH_ROOM_H_

#include <list>
#include <map>
#include "TableFrame.h"

enum room_state
{
    wait_start, //�����׶�, �ȴ���ʼ
	wait_next,	//�ȴ���һ��
    game,       //��Ϸ��
};

class CMatchItem;

class CMatchRoom : public CTableFrame
{
private:
	room_state			m_state;		//��������״̬
	WORD				m_index;		//�������ı��
	CMatchItem			*m_Match_Item;	//�����ı�����	

public:
	CMatchRoom(CMatchItem *item, WORD stage);
	~CMatchRoom(void);
	CMatchRoom(const CMatchRoom& room);

public:
	//���÷���״̬
	void SetRoomState(room_state state) { m_state = state; }
	//��ȡ����״̬
	room_state GetRoomState() { return m_state; }

	//���÷����ʶ
	void set_index(WORD index) { m_index = index; }
	//��ȡ�����ʶ
	WORD get_index() { return m_index; }

	//��ȡ������
	CMatchItem * GetMatchItem() { return m_Match_Item; }

	//��Ҷ��� && ��������
public:
	//�������	
	virtual int PlayerSitTable(CPlayer * pIServerUserItem, WORD wChairID = INVALID_CHAIR, bool bCheckUserGPS = false);
	//С�ֽ���
	virtual bool HandleXJGameEnd(BYTE byRound, WORD *wIdentity, SCORE *lUserTreasure);
	//��ֽ���
	virtual bool HandleDJGameEnd(BYTE cbGameStatus);
	//��Ϸ��ʼ
	virtual bool StartGame();
	//��ʼ��һ�׶α���
	virtual void StartNextStage();

	//������������������
public:
	void On_Match_Offline(CPlayer *player);
};

#endif