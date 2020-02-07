#ifndef TABLE_MANAGER_HEAD_FILE
#define TABLE_MANAGER_HEAD_FILE

#include <vector>
#include "Stdafx.h"
#include <list>

class CTableFrame;

//TODONOW 删除桌子的时候， 应该删除new 出来的
class CTableManager
{
	//用户变量
private:
	static std::list <CTableFrame*>                    s_TableArray;               //桌子vector

	//
public:
	//增
	static CTableFrame* CreateTable(tagTableRule* cfg, DWORD fangzhu);
	//删
	static bool DeleteTable(CTableFrame* pTable);
	//删
	static bool DeleteTable(DWORD dwRoomID);
	//删 所有
	static bool DeleteAllTable();
	//查
	static CTableFrame* FindTableByTableID(DWORD dwTableID);

	//获取 空闲金币场桌子 byType: 初级, 中级, 高级场次
	static CTableFrame* GetGlodTable(BYTE byType);
	//获取所有空闲金币场桌子
	static std::list<CTableFrame*> GetAllGlodTable();

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