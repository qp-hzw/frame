#ifndef C_PLAYER_H
#define C_PLAYER_H
#include "Stdafx.h"

class  CPlayer
{
private:
	//Socket属性
	DWORD                           m_dwSocketID;                       //scoketID

	//属性变量
private:
	tagUserInfo						m_UserInfo;							//用户信息

	//
private:
	DWORD							m_dwOfflineGameID;					//断线时 所在的游戏服标志

	//函数定义
public:
	//构造函数
	CPlayer(DWORD dwSocketID, tagUserInfo& userinfo);
	//析构函数
	~CPlayer(){}

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

//	/**********************************  socket信息 ****************************************/
//public:
//	//绑带索引
//	 WORD GetBindIndex() { return m_wBindIndex; }
//	//用户地址
//	// DWORD GetClientAddr() { return m_dwClientAddr; }
//	//机器标识
//	 LPCTSTR GetMachineID() { return m_szMachineID; }
//
//	//登录信息
//public:
//	//请求标识
//	virtual DWORD GetDBQuestID() { return m_dwDBQuestID++; }
//
//	//积分信息
//public:
//	//用户积分
//	virtual SCORE GetUserScore() { return m_UserInfo.lScore; }
//	//用户成绩
//	virtual SCORE GetUserGrade() { return m_UserInfo.lGrade; }
//	//获得用户房卡
//	virtual SCORE GetUserRoomCard() { return m_UserInfo.lOpenRoomCard; }
//	//设置用户房卡
//	virtual void SetUserRoomCard(SCORE lRoomCard) { m_UserInfo.lOpenRoomCard = lRoomCard; }
//
//	//用户钻石
//	virtual SCORE GetUserDiamond(){ return m_UserInfo.lDiamond; };
//	//设置用户钻石
//	virtual void SetUserDiamond(SCORE lDiamod) { m_UserInfo.lDiamond = lDiamod; }
//
//	//用户金币
//	virtual SCORE GetUserGold(){ return m_UserInfo.lGold; };
//	//设置用户金币
//	virtual void SetUserGold(SCORE lGold){ m_UserInfo.lGold = lGold; }

};

#endif