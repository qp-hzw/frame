#ifndef TABLE_MANAGER_HEAD_FILE
#define TABLE_MANAGER_HEAD_FILE

#include "Stdafx.h"
#include <vector>
#include <list>
#include "MatchRoom.h"
#include "MatchItem.h"

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
	//增 比赛场
	static CMatchRoom* CreateMatchRoom(CMatchItem* Item, WORD stage);
	//增 工会桌子
	static CTableFrame* CreateClubTable(tagTableRule* cfg, tagClubRoomRule* clubCfg);
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
	//获取 金币场换桌桌子 --不能重复加入
	static CTableFrame* ChangeGlodTable(BYTE byType, DWORD dwTableID);
	//获取所有空闲金币场桌子
	static std::list<CTableFrame*> GetAllGlodTable();

	//获取俱乐部 房间 所有桌子列表
	static std::list<CTableFrame*> GetAllClubRoomTable(DWORD dwClubID, DWORD dwRoomID); 

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