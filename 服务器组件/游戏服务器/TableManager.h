#ifndef TABLE_MANAGER_HEAD_FILE
#define TABLE_MANAGER_HEAD_FILE

#include <vector>
#include "Stdafx.h"

class CTableFrame;

class CTableManager
{
	//用户变量
private:
	static std::vector<CTableFrame*>                    s_TableArray;               //桌子vector

	//
public:
	//增
	static CTableFrame* CreateTable();
	//删
	static bool DeleteTable(CTableFrame* pTable);
	//删
	static bool DeleteTable(DWORD dwRoomID);
	//删 所有
	static bool DeleteAllTable();
	//查
	static CTableFrame* FindTableByTableID(DWORD dwTableID);
	//查
	static CTableFrame* FindTableByIndex(DWORD Index);

	//
public:
	//桌子总数
	static DWORD TableCount();
};

#endif