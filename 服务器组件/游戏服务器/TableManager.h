#ifndef TABLE_MANAGER_HEAD_FILE
#define TABLE_MANAGER_HEAD_FILE

#include <vector>
#include "Stdafx.h"

class CTableFrame;

//TODONOW 删除桌子的时候， 应该删除new 出来的
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
	//查找金币房空椅子
	static CTableFrame* GetGlodRoomEmptyChair(WORD &wChairID, BYTE byType, BOOL bTypeFlag = false);

	//
public:
	//桌子总数
	static DWORD TableCount();


	//辅助函数
private:
	//生产房间号
	static DWORD GenerateTablePassword();
};

#endif