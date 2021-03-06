# ifndef _MATCH_MANAGER_H_
# define _MATCH_MANAGER_H_

# include "stdafx.h"
# include "DataBaseEngineSink.h"
# include "MatchItem.h"
# include <list>

class CMatchManager
{
	//配置信息
private:
	static std::list<MATCH_CONFIG>				m_match_cfg;			//配置信息
	
	//比赛信息
private:
	static std::list<CMatchItem *>				s_Item_Array;			//比赛项

public:
	//读取比赛的配置信息
	static bool ReadMatchConfig(DWORD kindid);
	//获取所有配置信息
	static std::list<MATCH_CONFIG> GetAllMatchConfig();
	//配置比赛场
	static bool MatchConfig();
	//创建
	static bool Create_Match(MATCH_CONFIG config);
	//删除
	static bool Delete_Match(CMatchItem *Item);
	//查找
	static CMatchItem* Find_Match_ByItemID(WORD wMatchID);
	//查找全部
	static std::list<CMatchItem *> Find_Match_All();
	//开始比赛
	static void On_Match_Start(CMatchItem *Item);
	//比赛取消
	static void On_Match_Cancel(CMatchItem *Item);
	//比赛结束
	static void On_Match_End(CMatchItem *Item, std::list<player_info> playerInfo);
};

# endif