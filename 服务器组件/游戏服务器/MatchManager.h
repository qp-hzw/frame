# ifndef _MATCH_MANAGER_H_
# define _MATCH_MANAGER_H_

# include "stdafx.h"
# include "DataBaseEngineSink.h"
# include "MatchItem.h"
# include <list>

class CMatchManager
{
	//������Ϣ
private:
	static STR_DBR_GR_MATCH_CONFIG				m_match_cfg;			//������Ϣ
	
	//������Ϣ
private:
	static std::list<CMatchItem *>		s_Item_Array;			//������

public:
	//��ȡ������������Ϣ
	static bool ReadMatchConfig(STR_DBR_GR_MATCH_CONFIG config);
	//����
	static bool Create_Match(MATCH_CONFIG config);
	//ɾ��
	static bool Delete_Match(CMatchItem *Item);
	//����
	static CMatchItem* Find_Match_ByItemID(WORD wMatchID);
	//��ʼ����
	static void On_Match_Start(CMatchItem *Item);
	//����ȡ��
	static void On_Match_Cancel(CMatchItem *Item);
	//��������
	static void On_Match_End(CMatchItem *Item, std::vector<player_info> playerInfo);
};

# endif