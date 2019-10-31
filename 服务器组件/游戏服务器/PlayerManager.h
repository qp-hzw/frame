#ifndef PLAYER_MANAGER_H
#define PLAYER_MANAGER_H

#include <vector>
#include "Stdafx.h"

class CPlayer;

class  CPlayerManager
{
	//用户变量
private:
	static std::vector<CPlayer*>	            s_PlayerArray;					//用户数组

	//查找函数
public:
	//增
	static bool InsertPlayer(CPlayer * * pIServerUserResult, tagUserInfo & UserInfo, tagUserInfoPlus & UserInfoPlus);
	//删
	static bool DeletePlayer(CPlayer * pPlayer);
	//删 所有
	static bool DeleteAllPlayer();
	//查
	static CPlayer * FindPlayerByEnum(WORD wEnumIndex);
	//查
	static CPlayer * FindPlayerByID(DWORD dwUserID);

	//统计函数
public:
	//在线人数
	static DWORD GetPlayerCount() { return (DWORD)s_PlayerArray.size(); }
};

#endif