#ifndef C_PLAYER_H_LOGON
#define C_PLAYER_H_LOGON
#include "Stdafx.h"
#include <list>
#include <vector>
#include <map>

class  CPlayer
{
private:
	DWORD                           m_dwSocketID;                       //scoketID

public:
	tagUserInfo						m_UserInfo;							//用户信息
	static std::map< DWORD, std::vector<tagUserProperty>>	m_ArrPrp;		//玩家物品


	//函数定义
public:
	//构造函数
	CPlayer(DWORD dwSocketID, tagUserInfo& userinfo);
	//析构函数
	~CPlayer();

	/**********************************  游戏静态数据 ****************************************/
public:
	//用户SocketID
	DWORD GetSocketID() { return m_dwSocketID;}
	//用户信息
	tagUserInfo * GetUserInfo() { return &m_UserInfo; }
	//用户标识
	DWORD GetUserID() { return m_UserInfo.dwUserID; }

	/**********************************  游戏动态数据 ****************************************/
public:
	//初始化背包数据 db数据->server
	void InitProp(tagUserProperty prop);
	//添加背包 client->server
	void AddProp(tagUserProperty prop);
	//获取玩家道具
	std::vector<tagUserProperty> GetProps();
};

#endif