#ifndef TABLE_MANAGER_HEAD_FILE
#define TABLE_MANAGER_HEAD_FILE

#include "Stdafx.h"
#include <vector>
#include <list>
#include "MatchRoom.h"
#include "MatchItem.h"

class CTableFrame;

//TODONOW ɾ�����ӵ�ʱ�� Ӧ��ɾ��new ������
class CTableManager
{
	//�û�����
private:
	static std::list <CTableFrame*>                    s_TableArray;               //����vector

	//
public:
	//��
	static CTableFrame* CreateTable(tagTableRule* cfg, DWORD fangzhu);
	//ɾ
	static bool DeleteTable(CTableFrame* pTable);
	//ɾ
	static bool DeleteTable(DWORD dwRoomID);
	//ɾ ����
	static bool DeleteAllTable();
	//��
	static CTableFrame* FindTableByTableID(DWORD dwTableID);

	//��ȡ ���н�ҳ����� byType: ����, �м�, �߼�����
	static CTableFrame* GetGlodTable(BYTE byType);
	//��ȡ���п��н�ҳ�����
	static std::list<CTableFrame*> GetAllGlodTable();
	//��������������
	static CMatchRoom* CreateMatchRoom(CMatchItem* Item, WORD stage);

	//
public:
	//��������
	static DWORD TableCount();


	//��������
private:
	//���������
	static DWORD GenerateTablePassword();
};

#endif