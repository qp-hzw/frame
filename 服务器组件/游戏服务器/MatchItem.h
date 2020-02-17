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
	MATCH_CONFIG				m_config;			//房间配置
	CMatchRoom*					m_wait_room;		//等待房间

private:
	bool						m_Start;			//比赛开始标识
	std::list<bool>				m_Room_state;		//当前比赛所有房间的游戏状态
	WORD						m_Stage;			//当前阶段

private:
	std::list<CPlayer *>		m_Apply_Player;	//报名的玩家
	std::vector<player_info>	m_Cur_Ranking;	//正在比赛的玩家排名
	std::map<int, std::list<player_info>>	m_room_player;	//在房间里的玩家  tableID ---> playerinfo

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
	bool On_Player_TaoTai(std::list<player_info> TaoTai_player);
	//晋级玩家
	bool On_Player_JinJi(std::list<player_info> JinJi_player);
	//比赛结束
	bool On_Match_End(std::list<player_info> player);

	//更新所有人排名
	bool Update_Ranking(CMatchRoom *room);
	//发送所有人排名
	void Send_Ranking(CPlayer *player = NULL);
	//发送自己排名
	void Send_Self_Ranking(CPlayer *player);
	//发送等待桌数
	void Updata_WaitCount(CPlayer *player);


	//功能函数
public:
	//比赛初始化
	void Init();

	//获取MatchID
	DWORD GetMatchID() {	return m_config.wMatchID; }
	//获取比赛配置
	MATCH_CONFIG GetConfig() { return m_config; }

	//已报名人数
	WORD GetApplyCount() { return m_Apply_Player.size(); }
	//机器人是否报名满了
	bool IsRobotFull();
	//比赛是否已开始
	bool IsMatchStart() { return m_Start; }

	//获取玩家排名
	WORD GetRanking(CPlayer *player);

	//群发数据
	void SendDataAllPlayer(WORD wSubCmdID, VOID * pData, WORD wDataSize);
};

#endif