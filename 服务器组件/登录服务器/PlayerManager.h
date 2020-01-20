#ifndef PLAYER_MANAGER_H
#define PLAYER_MANAGER_H
#include <vector>
#include "Stdafx.h"
#include <map>

class CPlayer;

class  CPlayerManager
{
	//用户变量
private:
	static std::vector<CPlayer*>	            s_PlayerArray;					//用户数组
	static std::map<DWORD, DWORD>               s_OfflinePlayer;				//断线玩家列表

	//查找函数
public:
	//增
	static bool InsertPlayer(DWORD dwSocketID, tagUserInfo& UserInfoPlus);
	//删
	static bool DeletePlayer(CPlayer * pPlayer);
	//删
	static bool DeletePlayerByID(DWORD dwUserID);
	//删
	static bool DeletePlayerBySocketID(DWORD dwSocketID);
	//删 所有
	static bool DeleteAllPlayer();
	//查
	static CPlayer * FindPlayerByEnum(WORD wEnumIndex);
	//查
	static CPlayer * FindPlayerByID(DWORD dwUserID);
	//查
	static CPlayer * FindPlayerBySocketID(DWORD dwSocketID);
	//查 俱乐部所有
	static std::vector<CPlayer*> FindPlayerByClubID(DWORD dwClubID);


	//断线玩家
public:
	//增
	static void AddOfflinePlayer(DWORD userID, DWORD dwServerID);
	//删
	static void DeleteOfflinePlayer(DWORD userID);
	//查
	static DWORD FindOfflineGameID(DWORD userID);


	//统计函数
public:
	//在线人数
	static DWORD GetPlayerCount() { return (DWORD)s_PlayerArray.size(); }
};

#endif