#ifndef TABLE_MANAGER_HEAD_FILE
#define TABLE_MANAGER_HEAD_FILE

#include <vector>
#include "Stdafx.h"

class CTableFrame;

//TODONOW ɾ�����ӵ�ʱ�� Ӧ��ɾ��new ������
class CTableManager
{
	//�û�����
private:
	static std::vector<CTableFrame*>                    s_TableArray;               //����vector

	//
public:
	//��
	static CTableFrame* CreateTable();
	//ɾ
	static bool DeleteTable(CTableFrame* pTable);
	//ɾ
	static bool DeleteTable(DWORD dwRoomID);
	//ɾ ����
	static bool DeleteAllTable();
	//��
	static CTableFrame* FindTableByTableID(DWORD dwTableID);
	//��
	static CTableFrame* FindTableByIndex(DWORD Index);
	//���ҽ�ҷ�������
	static CTableFrame* GetGlodRoomEmptyChair(WORD &wChairID, BYTE byType, BOOL bTypeFlag = false);

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