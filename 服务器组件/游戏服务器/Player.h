#ifndef C_PLAYER_H
#define C_PLAYER_H
#include "Stdafx.h"
#include "TableFrameBase.h"

//用户状态
enum User_Action
{
	US_SIT,        //坐下动作
	US_IN_TABLE,   //在table中 -- 站立(旁观)动作
	US_READY,
	US_LEAVE,		//玩家离开
	US_OFFLINE,    //掉线动作
	US_PLAYING,    //游戏状态
	US_NULL,
	US_FREE,       //没有状态 -- 没有在Table中
};

class  CPlayer
{
	DWORD                           m_dwSocketID;                       //scoketID

	//属性变量
private:
	tagUserInfo						m_UserInfo;							//用户信息
	tagUserProperty                 m_UserProperty;                     //玩家道具

public:
	bool							m_bAndroidUser;						//机器用户

	//辅助变量
private:
	bool							m_bClientReady;						//连接标志
	bool							m_bModifyScore;						//修改标志
	TCHAR							m_szLogonPass[LEN_PASSWORD];		//用户密码

	//系统属性
private:
	DWORD							m_dwClientAddr;						//连接地址
	TCHAR							m_szMachineID[LEN_MACHINE_ID];		//机器标识

	//函数定义
public:
	//构造函数
	CPlayer(tagUserInfo & UserInfo);
	//析构函数
	~CPlayer(){}
	//拷贝构造
	CPlayer(const CPlayer &player);

	/**********************************  游戏静态数据 ****************************************/
public:
	//用户SocketID
	DWORD GetSocketID() { return m_dwSocketID;}
	void SetSocketID(DWORD socketID){m_dwSocketID = socketID;}

	//用户信息
	tagUserInfo * GetUserInfo() { return &m_UserInfo; }
	//道具信息
	tagUserProperty * GetUserProperty() { return &m_UserProperty; };

	//用户标识
	DWORD GetUserID() { return m_UserInfo.dwUserID; }
	//用户昵称
	LPCTSTR GetNickName() { return m_UserInfo.szNickName; }
	//用户性别
	BYTE GetSex() { return m_UserInfo.cbGender; }

	 //是否机器人
	  bool IsAndroidUser() { return m_bAndroidUser; }
	/**********************************  游戏动态数据 ****************************************/
public:
	//桌子号码
	 DWORD GetTableID() { return m_UserInfo.wTableID; }
	//椅子号码
	 virtual BYTE GetChairID() { return static_cast<BYTE>(m_UserInfo.wChairID); }

	//用户状态
	 BYTE GetUserStatus() { return m_UserInfo.cbUserStatus; }
	 bool SetUserStatus(BYTE cbUserStatus, DWORD wTableID, WORD wChairID);

	/**********************************  socket信息 ****************************************/
public:
	//用户地址
	 DWORD GetClientAddr() { return m_dwClientAddr; }
	//机器标识
	 LPCTSTR GetMachineID() { return m_szMachineID; }


	//用户道具
public:
	//获得用户房卡
	virtual SCORE GetUserRoomCard() { return m_UserInfo.lOpenRoomCard; }
	//设置用户房卡
	virtual void SetUserRoomCard(SCORE lRoomCard) { m_UserInfo.lOpenRoomCard = lRoomCard; }

	//用户钻石
	virtual SCORE GetUserDiamond(){ return m_UserInfo.lDiamond; };
	//设置用户钻石
	virtual void SetUserDiamond(SCORE lDiamod) { m_UserInfo.lDiamond = lDiamod; }

	//用户金币
	virtual SCORE GetUserGold(){ return m_UserInfo.lGold; };
	//设置用户金币
	virtual void SetUserGold(SCORE lGold){ m_UserInfo.lGold = lGold; }

	//用户使用道具 
	bool UseProp(DWORD dwGoodsID);

	//积分信息
public:
	//用户胜率
	virtual WORD GetUserWinRate();
	//用户输率
	virtual WORD GetUserLostRate();
	//用户和率
	virtual WORD GetUserDrawRate();
	//用户逃率
	virtual WORD GetUserFleeRate();
	//游戏局数
	virtual DWORD GetUserPlayCount() { return m_UserInfo.dwWinCount+m_UserInfo.dwLostCount+m_UserInfo.dwDrawCount+m_UserInfo.dwFleeCount; }

	//
public:
	//更新用户财富信息
	bool ModifyUserTreasure(BYTE byTreasureType, SCORE lUserTreasuse, string PayMsg);
	//更新用户输赢记录表
	bool ModifyPlayerScore(BYTE round, WORD wIdentity, SCORE lUserTreasuse, string PayMsg);
	//玩家经验
	bool ModifyPlayerExp();

	//辅助函数
private:
	//重置数据
	VOID ResetUserItem();

};

#endif