#ifndef _MATCH_ROOM_H_
#define _MATCH_ROOM_H_

#include <list>
#include <map>
#include "TableFrame.h"

enum room_state
{
    wait_start, //报名阶段, 等待开始
	wait_next,	//等待下一轮
    game,       //游戏中
};

class CMatchItem;

class CMatchRoom : public CTableFrame
{
private:
	room_state			m_state;		//比赛房间状态
	WORD				m_index;		//比赛房的标记
	CMatchItem			*m_Match_Item;	//归属的比赛场	

public:
	CMatchRoom(CMatchItem *item, WORD stage);
	~CMatchRoom(void);
	CMatchRoom(const CMatchRoom& room);

public:
	//设置房间状态
	void SetRoomState(room_state state) { m_state = state; }
	//获取房间状态
	room_state GetRoomState() { return m_state; }

	//设置房间标识
	void set_index(WORD index) { m_index = index; }
	//获取房间标识
	WORD get_index() { return m_index; }

	//获取比赛场
	CMatchItem * GetMatchItem() { return m_Match_Item; }

	//玩家动作 && 房间流程
public:
	//玩家坐下	
	virtual int PlayerSitTable(CPlayer * pIServerUserItem, WORD wChairID = INVALID_CHAIR, bool bCheckUserGPS = false);
	//小局结束
	virtual bool HandleXJGameEnd(BYTE byRound, WORD *wIdentity, SCORE *lUserTreasure);
	//大局结束
	virtual bool HandleDJGameEnd(BYTE cbGameStatus);
	//游戏开始
	virtual bool StartGame();
	//开始下一阶段比赛
	virtual void StartNextStage();

	//比赛场断线重连流程
public:
	void On_Match_Offline(CPlayer *player);
};

#endif