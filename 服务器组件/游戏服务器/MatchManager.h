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
	static std::list<MATCH_CONFIG>				m_match_cfg;			//������Ϣ
	
	//������Ϣ
private:
	static std::list<CMatchItem *>				s_Item_Array;			//������

public:
	//��ȡ������������Ϣ
	static bool ReadMatchConfig(DWORD kindid);
	//���ñ�����
	static bool MatchConfig();
	//��ȡ����
	static string GetDescribe(string key_name);
	//����
	static bool Create_Match(MATCH_CONFIG config);
	//ɾ��
	static bool Delete_Match(CMatchItem *Item);
	//����
	static CMatchItem* Find_Match_ByItemID(WORD wMatchID);
	//����ȫ��
	static std::list<CMatchItem *> Find_Match_All();
	//��ʼ����
	static void On_Match_Start(CMatchItem *Item);
	//����ȡ��
	static void On_Match_Cancel(CMatchItem *Item);
	//��������
	static void On_Match_End(CMatchItem *Item, std::list<player_info> playerInfo);
};

# endif