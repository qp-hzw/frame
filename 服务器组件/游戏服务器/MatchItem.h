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
	MATCH_CONFIG				m_config;			//房间配置
	CMatchRoom*					m_wait_room;		//等待房间

private:
	bool						m_Start;			//比赛开始标识
	std::vector<bool>			m_Room_state;		//当前比赛所有房间的游戏状态
	WORD						m_Stage;			//当前阶段

private:
	std::vector<CPlayer *>		m_Apply_Player;	//报名的玩家
	std::vector<player_info>	m_Cur_Ranking;	//正在比赛的玩家排名
	std::map<int, int>			m_Last_Score;	//上一轮积分
	std::map<int, std::vector<player_info>>	m_room_player;	//在房间里的玩家  tableID ---> playerinfo

	//构造函数
public:
	CMatchItem(MATCH_CONFIG config);
	~CMatchItem(void);

	//比赛流程
public:
	//玩家报名
	bool On_User_Apply(CPlayer *player);
	//玩家取消报名
	bool On_User_UnApply(CPlayer *player);
	//报名结束
	bool On_Apply_End();
	//比赛取消
	bool On_Match_Cancel();
	//比赛开始
	bool On_Match_Start();
	//开始当前阶段比赛
	bool On_Stage_Start();
	//一阶段比赛结束
	bool On_Stage_End(CMatchRoom *room);
	//一轮结束处理
	bool On_Room_End(CMatchRoom *room);
	//淘汰玩家
	bool On_Player_TaoTai(std::vector<player_info> TaoTai_player, CMatchRoom *room);
	//晋级玩家
	bool On_Player_JinJi(std::vector<player_info> JinJi_player, CMatchRoom *room);
	//比赛结束
	bool On_Match_End(std::vector<player_info> player);

	//更新排名
	bool Update_Ranking(CMatchRoom *room);
	//发送排名
	void Send_Ranking();


	//功能函数
public:
	//比赛初始化
	void Init();

	//获取MatchID
	WORD GetMatchID() {	return m_config.wMatchID; }
	//获取比赛配置
	MATCH_CONFIG GetConfig() { return m_config; }

	//群发数据
	void SendDataAllPlayer(WORD wSubCmdID, VOID * pData, WORD wDataSize);
};

#endif